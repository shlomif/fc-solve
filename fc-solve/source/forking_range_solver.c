/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 *  forking_range_solver.c - a range solver that solves different boards in
 *  several UNIX processes.
 *
 *  See also:
 *      - fc_pro_range_solver.c
 *      - test_multi_parallel.c
 *      - threaded_range_solver.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#define USE_EPOLL
#endif

#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifdef USE_EPOLL
#include <sys/epoll.h>
#endif

#include "fcs_user.h"
#include "fcs_cl.h"
#include "range_solvers_gen_ms_boards.h"
#include "handle_parsing.h"
#include "rinutils.h"
#include "help_err.h"
#include "range_solvers.h"

static void print_help(void)
{
    printf("\n%s",
        "freecell-solver-fork-solve start end print_step\n"
        "    [--num-workers n] [--worker-step step] [--total-iterations-limit "
        "limit]\n"
        "   [fc-solve Arguments...]\n"
        "\n"
        "Solves a sequence of boards from the Microsoft/Freecell Pro Deals\n"
        "\n"
        "start - the first board in the sequence\n"
        "end - the last board in the sequence (inclusive)\n"
        "print_step - at which division to print a status line\n"
        "\n"
        "--total-iterations-limit  limit\n"
        "     Limits each board for up to 'limit' iterations.\n");
}

static long long total_num_iters = 0;

#define READ_FD 0
#define WRITE_FD 1
typedef struct
{
    int child_to_parent_pipe[2];
    int parent_to_child_pipe[2];
} worker_t;

typedef struct
{
    long long board_num;
    long long quota_end;
} request_t;

typedef struct
{
    fcs_int_limit_t num_iters;
    int num_finished_boards;
} response_t;

static inline int worker_func(const worker_t w, void *const instance)
{
    /* I'm one of the slaves */
    request_t request;
    response_t response;

    while (1)
    {
        response.num_iters = 0;

        read(w.parent_to_child_pipe[READ_FD], &request, sizeof(request));

        if (request.board_num == -1)
        {
            break;
        }

        response.num_finished_boards =
            request.quota_end - request.board_num + 1;

#define board_num (request.board_num)
        for (; board_num <= request.quota_end; board_num++)
        {
            range_solvers__solve(instance, board_num, &response.num_iters);
            freecell_solver_user_recycle(instance);
        }
#undef board_num

        write(w.child_to_parent_pipe[WRITE_FD], &response, sizeof(response));
    }

    /* Cleanup */
    freecell_solver_user_free(instance);

    close(w.child_to_parent_pipe[WRITE_FD]);
    close(w.parent_to_child_pipe[READ_FD]);

    return 0;
}

static inline void write_request(const long long end_board,
    const long long board_num_step, long long *const next_board_num_ptr,
    const worker_t *const worker)
{
    request_t request;
    if ((*next_board_num_ptr) > end_board)
    {
        /* We only absolutely need to initialize .board_num here, but the
         * Coverity Scan scanner complains about quota_end being uninitialized
         * when passed to write() so we initialize it here as well.
         * */
        request = (request_t){.board_num = -1, .quota_end = -1};
    }
    else
    {
        request.board_num = *(next_board_num_ptr);
        if (((*next_board_num_ptr) += board_num_step) > end_board)
        {
            (*next_board_num_ptr) = end_board + 1;
        }
        request.quota_end = (*next_board_num_ptr) - 1;
    }

    write(worker->parent_to_child_pipe[WRITE_FD], &request, sizeof(request));
}

static inline void transaction(const worker_t *const worker, const int read_fd,
    int *const total_num_finished_boards, const long long end_board,
    const long long board_num_step, long long *const next_board_num_ptr)
{
    response_t response;
    if (read(read_fd, &response, sizeof(response)) <
        (ssize_t)(sizeof(response)))
    {
        return;
    }

    total_num_iters += response.num_iters;
    (*total_num_finished_boards) += response.num_finished_boards;

    write_request(end_board, board_num_step, next_board_num_ptr, worker);
}

static inline int read_fd(const worker_t *const worker)
{
    return worker->child_to_parent_pipe[READ_FD];
}

int main(int argc, char *argv[])
{
    int arg = 1;

    if (argc < 4)
    {
        help_err("Not Enough Arguments!\n");
    }
    long long next_board_num = atoll(argv[arg++]);
    const long long end_board = atoll(argv[arg++]);
    const long long stop_at = atoll(argv[arg++]);
    if (stop_at <= 0)
    {
        help_err("print_step (the third argument) must be greater than 0.\n");
    }

    size_t num_workers = 3;
    int board_num_step = 1;
    fcs_int_limit_t total_iterations_limit_per_board = -1;

    for (; arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--total-iterations-limit"))
        {
            arg++;
            if (arg == argc)
            {
                help_err(
                    "--total-iterations-limit came without an argument!\n");
            }
            total_iterations_limit_per_board = (fcs_int_limit_t)atol(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--num-workers"))
        {
            arg++;
            if (arg == argc)
            {
                help_err("--num-workers came without an argument!\n");
            }
            num_workers = atoi(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--worker-step"))
        {
            arg++;
            if (arg == argc)
            {
                help_err("--worker-step came without an argument!\n");
            }
            board_num_step = atoi(argv[arg]);
        }
        else
        {
            break;
        }
    }

    fc_solve_print_started_at();
    void *const instance = simple_alloc_and_parse(argc, argv, &arg);
    freecell_solver_user_limit_iterations_long(
        instance, total_iterations_limit_per_board);
    worker_t workers[num_workers];

    for (size_t idx = 0; idx < num_workers; idx++)
    {
        if (pipe(workers[idx].child_to_parent_pipe))
        {
            fc_solve_err(
                "C->P Pipe for worker No. %zd failed! Exiting.\n", idx);
        }
        if (pipe(workers[idx].parent_to_child_pipe))
        {
            fc_solve_err(
                "P->C Pipe for worker No. %zd failed! Exiting.\n", idx);
        }

        switch (fork())
        {
        case -1:
        {
            fc_solve_err("Fork for worker No. %zd failed! Exiting.\n", idx);
        }

        case 0:
        {
            /* I'm the child. */
            const_AUTO(w, workers[idx]);
            close(w.parent_to_child_pipe[WRITE_FD]);
            close(w.child_to_parent_pipe[READ_FD]);
            return worker_func(w, instance);
        }

        default:
        {
            /* I'm the parent. */
            close(workers[idx].parent_to_child_pipe[READ_FD]);
            close(workers[idx].child_to_parent_pipe[WRITE_FD]);
        }
        break;
        }
    }

    freecell_solver_user_free(instance);

    {
/* I'm the master. */
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

        for (size_t idx = 0; idx < num_workers; idx++)
        {
            const int fd = read_fd(&workers[idx]);
#ifdef USE_EPOLL
            struct epoll_event ev = {
                .events = EPOLLIN, .data.ptr = &(workers[idx]),
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
        mymax++;
#endif
        int total_num_finished_boards = 0;
        const long long total_num_boards_to_check =
            end_board - next_board_num + 1;

        long long next_milestone = next_board_num + stop_at;
        next_milestone -= (next_milestone % stop_at);

        for (size_t idx = 0; idx < num_workers; idx++)
        {
            write_request(
                end_board, board_num_step, &next_board_num, &(workers[idx]));
        }

#ifdef USE_EPOLL
#define MAX_EVENTS 10
        struct epoll_event events[MAX_EVENTS];
#endif

        while (total_num_finished_boards < total_num_boards_to_check)
        {
            if (total_num_finished_boards >= next_milestone)
            {
                fc_solve_print_reached(next_milestone, total_num_iters);
                next_milestone += stop_at;
            }

#ifdef USE_EPOLL
            const int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
            if (nfds == -1)
            {
                perror("epoll_pwait");
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < nfds; i++)
            {
                const worker_t *const worker = events[i].data.ptr;
                transaction(worker, read_fd(worker), &total_num_finished_boards,
                    end_board, board_num_step, &next_board_num);
            }

#else
            fd_set readers = initial_readers;
            /* I'm the master. */
            const int select_ret = select(mymax, &readers, NULL, NULL, NULL);

            if (select_ret == -1)
            {
                perror("select()");
            }
            else if (select_ret)
            {
                for (size_t idx = 0; idx < num_workers; idx++)
                {
                    const int fd = read_fd(&workers[idx]);

                    if (FD_ISSET(fd, &readers))
                    {
                        /* FD_ISSET can be set on EOF, so we check if
                         * read failed. */
                        transaction(&(workers[idx]), fd,
                            &total_num_finished_boards, end_board,
                            board_num_step, &next_board_num);
                    }
                }
            }
#endif
        }
    }

    int status;
    for (size_t idx = 0; idx < num_workers; idx++)
    {
        wait(&status);
    }

    fc_solve_print_finished(total_num_iters);
    return 0;
}
