// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
//  forking_range_solver.c - a range solver that solves different boards in
//  several UNIX processes.
//
//  See also:
//      - fc_pro_range_solver.c
//      - serial_range_solver.c
//      - threaded_range_solver.c
#ifdef __linux__
#define USE_EPOLL
#include <sys/epoll.h>
#endif
#include <sys/wait.h>
#include "range_solvers.h"
#include "try_param.h"
#include "print_time.h"

#ifndef FCS_WITHOUT_CMD_LINE_HELP
static void print_help(void)
{
    printf("\n%s",
        "freecell-solver-fork-solve start end print_step\n"
        "    [--num-workers n] [--worker-step step] [fc-solve Arguments...]\n"
        "\n"
        "Solves a sequence of boards from the Microsoft/Freecell Pro Deals\n"
        "\n"
        "start - the first board in the sequence\n"
        "end - the last board in the sequence (inclusive)\n"
        "print_step - at which division to print a status line\n");
}
#endif

static fcs_iters_int total_num_iters = 0;
static fc_solve_ms_deal_idx_type total_num_finished_boards = 0;

#define READ_FD 0
#define WRITE_FD 1
typedef struct
{
    int child_to_parent_pipe[2], parent_to_child_pipe[2];
} fcs_worker;

typedef struct
{
    fc_solve_ms_deal_idx_type board_num, quota_end;
} request_type;

typedef struct
{
    fcs_iters_int num_iters;
    fc_solve_ms_deal_idx_type num_finished_boards;
} response_type;

static inline void write_request(const fc_solve_ms_deal_idx_type end_board,
    const fc_solve_ms_deal_idx_type board_num_step,
    fc_solve_ms_deal_idx_type *const next_board_num_ptr,
    const fcs_worker *const worker)
{
    if ((*next_board_num_ptr) > end_board)
    {
        close(worker->parent_to_child_pipe[WRITE_FD]);
        return;
    }
    request_type req;
    req.board_num = *(next_board_num_ptr);
    if (((*next_board_num_ptr) += board_num_step) > end_board)
    {
        (*next_board_num_ptr) = end_board + 1;
    }
    req.quota_end = (*next_board_num_ptr) - 1;
    const_AUTO(
        ret, write(worker->parent_to_child_pipe[WRITE_FD], &req, sizeof(req)));
    if (ret != sizeof(req))
    {
        exit(-1);
    }
}

static inline void transaction(const fcs_worker *const worker,
    const int read_fd, const fc_solve_ms_deal_idx_type end_board,
    const fc_solve_ms_deal_idx_type board_num_step,
    fc_solve_ms_deal_idx_type *const next_board_num_ptr)
{
    response_type response;
    if (read(read_fd, &response, sizeof(response)) <
        (ssize_t)(sizeof(response)))
    {
        return;
    }
    total_num_iters += response.num_iters;
    total_num_finished_boards += response.num_finished_boards;

    write_request(end_board, board_num_step, next_board_num_ptr, worker);
}

static inline int read_fd(const fcs_worker *const worker)
{
    return worker->child_to_parent_pipe[READ_FD];
}

static inline int range_solvers_main(int argc, char *argv[], int arg,
    fc_solve_ms_deal_idx_type next_board_num,
    const fc_solve_ms_deal_idx_type end_board,
    const fc_solve_ms_deal_idx_type stop_at)
{
    size_t num_workers = 3;
    fc_solve_ms_deal_idx_type board_num_step = 1;
    for (; arg < argc; ++arg)
    {
        const char *param;
        if ((param = TRY_P("--num-workers")))
        {
            num_workers = (size_t)atoi(param);
        }
        else if ((param = TRY_P("--worker-step")))
        {
            board_num_step = fcs_str2msdeal(param);
        }
        else
        {
            break;
        }
    }

    fc_solve_print_started_at();
    void *const instance = simple_alloc_and_parse(argc, argv, arg);
    fcs_worker workers[num_workers];

    for (size_t idx = 0; idx < num_workers; ++idx)
    {
        if (pipe(workers[idx].child_to_parent_pipe))
        {
            exit_error("C->P Pipe for worker No. %lu failed! Exiting.\n",
                (unsigned long)idx);
        }
        if (pipe(workers[idx].parent_to_child_pipe))
        {
            exit_error("P->C Pipe for worker No. %lu failed! Exiting.\n",
                (unsigned long)idx);
        }

        switch (fork())
        {
        case -1:
            exit_error("Fork for worker No. %lu failed! Exiting.\n",
                (unsigned long)idx);

        case 0: {
            // I'm the child.
            const_AUTO(w, workers[idx]);
            close(w.parent_to_child_pipe[WRITE_FD]);
            close(w.child_to_parent_pipe[READ_FD]);
            // I'm one of the workers
            request_type req;
            fcs_state_string state_string;
            get_board__setup_string(state_string);
            while (read(w.parent_to_child_pipe[READ_FD], &req, sizeof(req)) ==
                   sizeof(req))
            {
                response_type response = {
                    .num_iters = 0,
                    .num_finished_boards = req.quota_end - req.board_num + 1,
                };
                for (; req.board_num <= req.quota_end; ++req.board_num)
                {
                    range_solvers__solve(state_string, instance, req.board_num,
                        &response.num_iters);
                    freecell_solver_user_recycle(instance);
                }
                if (sizeof(response) != write(w.child_to_parent_pipe[WRITE_FD],
                                            &response, sizeof(response)))
                {
                    abort();
                }
            }
            freecell_solver_user_free(instance);
            close(w.child_to_parent_pipe[WRITE_FD]);
            close(w.parent_to_child_pipe[READ_FD]);
            return 0;
        }

        default:
            // I'm the parent.
            close(workers[idx].parent_to_child_pipe[READ_FD]);
            close(workers[idx].child_to_parent_pipe[WRITE_FD]);
            break;
        }
    }

    freecell_solver_user_free(instance);

// I'm the master.
#ifdef USE_EPOLL
    const int epollfd = epoll_create1(0);
    if (epollfd == -1)
    {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
#else
    fd_set initial_readers;
    FD_ZERO(&initial_readers);
    int mymax = -1;
#endif

    for (size_t idx = 0; idx < num_workers; ++idx)
    {
        const int fd = read_fd(&workers[idx]);
#ifdef USE_EPOLL
        struct epoll_event ev = {
            .events = EPOLLIN,
            .data.ptr = &(workers[idx]),
        };
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1)
        {
            perror("epoll_ctl: listen_sock");
            exit(EXIT_FAILURE);
        }
#else
        FD_SET(fd, &initial_readers);
        if (fd > mymax)
        {
            mymax = fd;
        }
#endif
    }
#ifndef USE_EPOLL
    ++mymax;
#endif
    const fc_solve_ms_deal_idx_type total_num_boards_to_check =
        end_board - next_board_num + 1;

    fc_solve_ms_deal_idx_type next_milestone = stop_at;
    for (size_t idx = 0; idx < num_workers; ++idx)
    {
        write_request(
            end_board, board_num_step, &next_board_num, &(workers[idx]));
    }

    while (total_num_finished_boards < total_num_boards_to_check)
    {
        while (total_num_finished_boards >= next_milestone)
        {
            fc_solve_print_reached(next_milestone, total_num_iters);
            next_milestone += stop_at;
        }
#ifdef USE_EPOLL
        struct epoll_event events[4];
        const int nfds = epoll_wait(epollfd, events, COUNT(events), -1);
        if (nfds == -1)
        {
            perror("epoll_pwait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < nfds; ++i)
        {
            const fcs_worker *const worker = events[i].data.ptr;
            transaction(worker, read_fd(worker), end_board, board_num_step,
                &next_board_num);
        }
#else
        fd_set readers = initial_readers;
        // I'm the master.
        const int select_ret = select(mymax, &readers, NULL, NULL, NULL);

        if (select_ret == -1)
        {
            perror("select()");
        }
        else if (select_ret)
        {
            for (size_t idx = 0; idx < num_workers; ++idx)
            {
                const int fd = read_fd(&workers[idx]);

                if (FD_ISSET(fd, &readers))
                {
                    // FD_ISSET can be set on EOF, so we check if read failed.
                    transaction(&(workers[idx]), fd, end_board, board_num_step,
                        &next_board_num);
                }
            }
        }
#endif
    }
    while (total_num_finished_boards >= next_milestone)
    {
        fc_solve_print_reached(next_milestone, total_num_iters);
        next_milestone += stop_at;
    }

    for (size_t idx = 0; idx < num_workers; ++idx)
    {
        wait(NULL);
    }
    fc_solve_print_finished(total_num_iters);
    return 0;
}
