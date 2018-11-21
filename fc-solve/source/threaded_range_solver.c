/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// threaded_range_solver.c - a range solver that solves different boards in
// several POSIX threads.
//
// See also:
// - fc_pro_range_solver.c
// - forking_range_solver.c
// - serial_range_solver.c
#include <pthread.h>
#include "range_solvers.h"
#include "try_param.h"

#ifndef FCS_WITHOUT_CMD_LINE_HELP
static void print_help(void)
{
    printf("\n%s",
        "freecell-solver-multi-thread-solve start end print_step\n"
        "   [--binary-output-to filename] [fc-solve Arguments...]\n"
        "\n"
        "Solves a sequence of boards from the Microsoft/Freecell Pro Deals\n"
        "\n"
        "start - the first board in the sequence\n"
        "end - the last board in the sequence (inclusive)\n"
        "print_step - at which division to print a status line\n");
}
#endif

static const pthread_mutex_t initial_mutex_constant = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t next_board_num_lock;
static long long next_board_num, stop_at, past_end_board, board_num_step = 1;
#define UPDATE_TOTAL_NUM_ITERS_THRESHOLD 1000000
#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
static char **context_argv;
static int arg = 1, context_argc;
#endif
static long long total_num_iters = 0;

static void *worker_thread(void *const void_arg)
{
#ifdef FCS_USE_PRECOMPILED_CMD_LINE_THEME
    void *const instance = simple_alloc_and_parse(0, NULL, 0);
#else
    void *const instance =
        simple_alloc_and_parse(context_argc, context_argv, arg);
#endif
    typeof(total_num_iters) total_num_iters_temp = 0;
    long long board_num;
    do
    {
        pthread_mutex_lock(&next_board_num_lock);
        board_num = next_board_num;
        const long long proposed_quota_end = (next_board_num += board_num_step);
        pthread_mutex_unlock(&next_board_num_lock);

        const long long quota_end = min(proposed_quota_end, past_end_board);

        for (; board_num < quota_end; board_num++)
        {
            if (range_solvers__solve(
                    instance, board_num, &total_num_iters_temp))
            {
                goto theme_error;
            }
            if (unlikely(board_num % stop_at == 0))
            {
                fc_solve_print_reached_no_iters(board_num);
            }
            freecell_solver_user_recycle(instance);
        }
    } while (board_num < past_end_board);

theme_error:
    freecell_solver_user_free(instance);
    *(typeof(total_num_iters) *)void_arg = total_num_iters_temp;

    return NULL;
}

static inline int range_solvers_main(int argc, char *argv[], const int par__arg,
    const long long par__next_board_num, const long long par__end_board,
    const long long par__stop_at)
{
    next_board_num_lock = initial_mutex_constant;
#ifdef FCS_USE_PRECOMPILED_CMD_LINE_THEME
    int arg = par__arg;
#else
    arg = par__arg;
#endif
    next_board_num = par__next_board_num;
    past_end_board = 1 + par__end_board;
    stop_at = par__stop_at;

    size_t num_workers = 3;
    for (; arg < argc; ++arg)
    {
        const char *param;
        if ((param = TRY_P("--num-workers")))
        {
            num_workers = (size_t)atol(param);
        }
        else if ((param = TRY_P("--worker-step")))
        {
            board_num_step = atoll(param);
        }
        else
        {
            break;
        }
    }

    fc_solve_print_started_at();
#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
    context_argc = argc;
    context_argv = argv;
#endif
    pthread_t workers[num_workers];
    typeof(total_num_iters) num_iters[num_workers];
    for (size_t idx = 0; idx < num_workers; ++idx)
    {
        const int check =
            pthread_create(&workers[idx], NULL, worker_thread, &num_iters[idx]);
        if (check)
        {
            fc_solve_err(
                "Worker Thread No. %lu Initialization failed with error %d!\n",
                (unsigned long)idx, check);
        }
    }

    // Wait for all threads to finish.
    for (size_t idx = 0; idx < num_workers; ++idx)
    {
        pthread_join(workers[idx], NULL);
        total_num_iters += num_iters[idx];
    }
    fc_solve_print_finished(total_num_iters);

    return 0;
}
