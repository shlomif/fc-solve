/* Copyright (c) 2000 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
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

#include "alloc_wrap.h"
#include "portable_int64.h"
#include "portable_time.h"

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
#include "unused.h"
#include "inline.h"
#include "range_solvers_gen_ms_boards.h"

#define BINARY_OUTPUT_NUM_INTS 16

static void print_help(void)
{
    printf("\n%s",
"freecell-solver-fork-solve start end print_step\n"
"    [--num-workers n] [--worker-step step] [--total-iterations-limit limit]\n"
"   [fc-solve Arguments...]\n"
"\n"
"Solves a sequence of boards from the Microsoft/Freecell Pro Deals\n"
"\n"
"start - the first board in the sequence\n"
"end - the last board in the sequence (inclusive)\n"
"print_step - at which division to print a status line\n"
"\n"
"--total-iterations-limit  limit\n"
"     Limits each board for up to 'limit' iterations.\n"
          );
}


static fcs_int64_t total_num_iters = 0;

#define READ_FD 0
#define WRITE_FD 1
typedef struct
{
    int child_to_parent_pipe[2];
    int parent_to_child_pipe[2];
} worker_t;

typedef struct
{
    int board_num;
    int quota_end;
} request_t;

typedef struct
{
    fcs_int_limit_t num_iters;
    int num_finished_boards;
} response_t;

static GCC_INLINE int worker_func(const worker_t w, void * const instance)
{
    /* I'm one of the slaves */
    request_t request;
    response_t response;
    fcs_state_string_t state_string;
    fcs_portable_time_t mytime;

    while(1)
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
#define total_num_iters_temp (response.num_iters)
        for(;board_num<=request.quota_end;board_num++)
        {
            get_board(board_num, state_string);


            switch (
                freecell_solver_user_solve_board(
                    instance,
                    state_string
                    )
            )
            {
                case FCS_STATE_SUSPEND_PROCESS:
                {
                    FCS_PRINT_INTRACTABLE_BOARD(mytime, board_num);
                    fflush(stdout);
                }
                break;
                case FCS_STATE_FLARES_PLAN_ERROR:
                {
                    fprintf(stderr, "Flares Plan: %s\n", freecell_solver_user_get_last_error_string(instance));

                    goto next_board;
                }
                break;

                case FCS_STATE_IS_NOT_SOLVEABLE:
                {
                    FCS_PRINT_UNSOLVED_BOARD(mytime, board_num);
                    fflush(stdout);
                }
                break;
            }

            total_num_iters_temp += freecell_solver_user_get_num_times_long(instance);

            /*  TODO : implement at the master. */
#if 0

#endif

next_board:
            freecell_solver_user_recycle(instance);
        }
#undef board_num
#undef total_num_iters_temp

        write(w.child_to_parent_pipe[WRITE_FD], &response, sizeof(response));
    }

    /* Cleanup */
    freecell_solver_user_free(instance);

    close(w.child_to_parent_pipe[WRITE_FD]);
    close(w.parent_to_child_pipe[READ_FD]);

    return 0;
}

static GCC_INLINE void write_request(
    const int end_board,
    const int board_num_step,
    int * const next_board_num_ptr,
    const worker_t * const worker
)
{
    request_t request;
    if ((*next_board_num_ptr) > end_board)
    {
        request.board_num = -1;
    }
    else
    {
        request.board_num = *(next_board_num_ptr);
        if (((*next_board_num_ptr) += board_num_step) > end_board)
        {
            (*next_board_num_ptr) = end_board+1;
        }
        request.quota_end = (*next_board_num_ptr)-1;
    }

    write(
        worker->parent_to_child_pipe[WRITE_FD],
        &request,
        sizeof(request)
    );
}

static GCC_INLINE void transaction(
    const worker_t * const worker,
    const int read_fd,
    int * const total_num_finished_boards,
    const int end_board,
    const int board_num_step,
    int * const next_board_num_ptr
)
{
    response_t response;
    if (read (read_fd, &response, sizeof(response)) < sizeof(response))
    {
        return;
    }

    total_num_iters += response.num_iters;
    (*total_num_finished_boards) += response.num_finished_boards;

    write_request(end_board, board_num_step,
        next_board_num_ptr, worker
    );
}

int main(int argc, char * argv[])
{

    int arg = 1;

    if (argc < 4)
    {
        fprintf(stderr, "Not Enough Arguments!\n");
        print_help();
        exit(-1);
    }
    int next_board_num = atoi(argv[arg++]);
    const int end_board = atoi(argv[arg++]);
    const int stop_at = atoi(argv[arg++]);
    if (stop_at <= 0)
    {
        fprintf(stderr, "print_step (the third argument) must be greater than 0.\n");
        print_help();
        exit(-1);

    }

    int num_workers = 3;
    int board_num_step = 1;
    fcs_int_limit_t total_iterations_limit_per_board = -1;

    for (;arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--total-iterations-limit"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--total-iterations-limit came without an argument!\n");
                print_help();
                exit(-1);
            }
            total_iterations_limit_per_board = (fcs_int_limit_t)atol(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--num-workers"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--num-workers came without an argument!\n");
                print_help();
                exit(-1);
            }
            num_workers = atoi(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--worker-step"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--worker-step came without an argument!\n");
                print_help();
                exit(-1);
            }
            board_num_step = atoi(argv[arg]);
        }
        else
        {
            break;
        }
    }

    fcs_portable_time_t mytime;
    FCS_PRINT_STARTED_AT(mytime);
    fflush(stdout);

    void * const instance = freecell_solver_user_alloc();

    char * error_string;
    switch (
        freecell_solver_user_cmd_line_parse_args(
            instance,
            argc,
            (freecell_solver_str_t *)(void *)argv,
            arg,
            NULL,
            NULL,
            NULL,
            &error_string,
            &arg
        )
    )
    {
        case FCS_CMD_LINE_UNRECOGNIZED_OPTION:
        {
            fprintf(stderr, "Unknown option: %s", argv[arg]);
            return -1;
        }
        case FCS_CMD_LINE_PARAM_WITH_NO_ARG:
        {
            fprintf(stderr, "The command line parameter \"%s\" requires an argument"
                " and was not supplied with one.\n", argv[arg]);
            return -1;
        }
        case FCS_CMD_LINE_ERROR_IN_ARG:
        {
            if (error_string != NULL)
            {
                fprintf(stderr, "%s", error_string);
                free(error_string);
            }
            return -1;
        }
    }

    freecell_solver_user_limit_iterations_long(
        instance,
        total_iterations_limit_per_board
    );
    worker_t workers[num_workers];

    for ( int idx = 0 ; idx < num_workers ; idx++)
    {
        int fork_ret;

        if (pipe(workers[idx].child_to_parent_pipe))
        {
            fprintf(stderr, "C->P Pipe for worker No. %i failed! Exiting.\n", idx);
            exit(-1);
        }
        if (pipe(workers[idx].parent_to_child_pipe))
        {
            fprintf(stderr, "P->C Pipe for worker No. %i failed! Exiting.\n", idx);
            exit(-1);
        }


        fork_ret = fork();

        if (fork_ret == -1)
        {
            fprintf(stderr, "Fork for worker No. %i failed! Exiting.\n", idx);
            exit(-1);
        }

        if (! fork_ret)
        {
            /* I'm the child. */
            const worker_t w = workers[idx];
            close(w.parent_to_child_pipe[WRITE_FD]);
            close(w.child_to_parent_pipe[READ_FD]);
            return worker_func(w, instance);
        }
        else
        {
            /* I'm the parent. */
            close(workers[idx].parent_to_child_pipe[READ_FD]);
            close(workers[idx].child_to_parent_pipe[WRITE_FD]);
        }
    }

    freecell_solver_user_free(instance);

    {
        /* I'm the master. */
#ifdef USE_EPOLL
#define MAX_EVENTS 10
        struct epoll_event ev, events[MAX_EVENTS];
        int epollfd = epoll_create1(0);
        if (epollfd == -1) {
            perror("epoll_create1");
            exit(EXIT_FAILURE);
        }
#else
        fd_set initial_readers;
        FD_ZERO(&initial_readers);
        int mymax = -1;
#endif

        for (int idx=0 ; idx < num_workers ; idx++)
        {
#define GET_READ_FD(worker) ((worker).child_to_parent_pipe[READ_FD])
            const int fd = GET_READ_FD(workers[idx]);
#ifdef USE_EPOLL
            ev.events = EPOLLIN;
            ev.data.ptr = &(workers[idx]);

            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
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
        const int total_num_boards_to_check = end_board - next_board_num + 1;

        int next_milestone = next_board_num + stop_at;
        next_milestone -= (next_milestone % stop_at);

        for (int idx=0 ; idx < num_workers ; idx++)
        {
            write_request(end_board, board_num_step,
                &next_board_num, &(workers[idx])
            );
        }


        while (total_num_finished_boards < total_num_boards_to_check)
        {
            if (total_num_finished_boards >= next_milestone)
            {
                FCS_PRINT_REACHED_BOARD(
                    mytime,
                    next_milestone,
                    total_num_iters
                );
                fflush(stdout);

                next_milestone += stop_at;
            }

#ifdef USE_EPOLL
            const int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
            if (nfds == -1)
            {
                perror("epoll_pwait");
                exit(EXIT_FAILURE);
            }

            for (int i = 0 ; i < nfds ; i++)
            {
                const worker_t * const worker = events[i].data.ptr;
                transaction(
                    worker, GET_READ_FD(*worker), &total_num_finished_boards,
                    end_board, board_num_step, &next_board_num
                );
            }

#else
            fd_set readers = initial_readers;
            /* I'm the master. */
            const int select_ret = select (mymax, &readers, NULL, NULL, NULL);

            if (select_ret == -1)
            {
                perror("select()");
            }
            else if (select_ret)
            {
                for(int idx = 0 ; idx < num_workers ; idx++)
                {
                    const int fd = workers[idx].child_to_parent_pipe[READ_FD];

                    if (FD_ISSET(fd, &readers))
                    {
                        /* FD_ISSET can be set on EOF, so we check if
                         * read failed. */
                        transaction(
                            &(workers[idx]), fd, &total_num_finished_boards,
                            end_board, board_num_step, &next_board_num
                        );
                    }
                }
            }
#endif
        }
    }


    {
        int status;
        for(int idx=0 ; idx < num_workers ; idx++)
        {
            wait(&status);
        }
    }

    FCS_PRINT_FINISHED(mytime, total_num_iters);

    return 0;
}

