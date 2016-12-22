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
 *  threaded_range_solver.c - a range solver that solves different boards in
 *  several POSIX threads.
 *
 *  See also:
 *      - fc_pro_range_solver.c
 *      - test_multi_parallel.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "rinutils.h"
#include "fcs_user.h"
#include "fcs_cl.h"
#include "range_solvers_gen_ms_boards.h"
#include "handle_parsing.h"
#include "help_err.h"

static void print_help(void)
{
    printf("\n%s",
        "freecell-solver-range-parallel-solve start end print_step\n"
        "   [--binary-output-to filename] [--total-iterations-limit limit]\n"
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

static const pthread_mutex_t initial_mutex_constant = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t next_board_num_lock;
static long long next_board_num, stop_at, past_end_board, board_num_step = 1;
fcs_int_limit_t total_iterations_limit_per_board = -1;
fcs_int_limit_t update_total_num_iters_threshold = 1000000;
char **context_argv;
int context_arg = 1, context_argc;
static long long total_num_iters = 0;
static pthread_mutex_t total_num_iters_lock;

static void *worker_thread(void *GCC_UNUSED void_arg)
{
    int arg = context_arg;
    void *const instance =
        simple_alloc_and_parse(context_argc, context_argv, &arg);
    freecell_solver_user_limit_iterations_long(
        instance, total_iterations_limit_per_board);

    fcs_portable_time_t mytime;
    fcs_int_limit_t total_num_iters_temp = 0;
    long long board_num;
    do
    {
        pthread_mutex_lock(&next_board_num_lock);
        board_num = next_board_num;
        const int proposed_quota_end = (next_board_num += board_num_step);
        pthread_mutex_unlock(&next_board_num_lock);

        const int quota_end = min(proposed_quota_end, past_end_board);

        for (; board_num < quota_end; board_num++)
        {
            fcs_state_string_t state_string;
            get_board(board_num, state_string);

            switch (freecell_solver_user_solve_board(instance, state_string))
            {
            case FCS_STATE_SUSPEND_PROCESS:
                fc_solve_print_intractable(board_num);
                break;

            case FCS_STATE_FLARES_PLAN_ERROR:
#ifdef FCS_WITH_ERROR_STRS
                fprintf(stderr, "Flares Plan: %s\n",
                    freecell_solver_user_get_last_error_string(instance));
#endif
                goto theme_error;

            case FCS_STATE_IS_NOT_SOLVEABLE:
                fc_solve_print_unsolved(board_num);
                break;
            }

            total_num_iters_temp +=
                freecell_solver_user_get_num_times_long(instance);
            if (total_num_iters_temp >= update_total_num_iters_threshold)
            {
                pthread_mutex_lock(&total_num_iters_lock);
                total_num_iters += total_num_iters_temp;
                pthread_mutex_unlock(&total_num_iters_lock);
                total_num_iters_temp = 0;
            }

            if (unlikely(board_num % stop_at == 0))
            {
                pthread_mutex_lock(&total_num_iters_lock);
                long long total_num_iters_copy =
                    (total_num_iters += total_num_iters_temp);
                pthread_mutex_unlock(&total_num_iters_lock);
                total_num_iters_temp = 0;

                FCS_PRINT_REACHED_BOARD(
                    mytime, board_num, total_num_iters_copy);
            }
            freecell_solver_user_recycle(instance);
        }
    } while (board_num < past_end_board);

    pthread_mutex_lock(&total_num_iters_lock);
    total_num_iters += total_num_iters_temp;
    pthread_mutex_unlock(&total_num_iters_lock);

theme_error:
    freecell_solver_user_free(instance);

    return NULL;
}

int main(int argc, char *argv[])
{
    next_board_num_lock = initial_mutex_constant;
    total_num_iters_lock = initial_mutex_constant;
    if (argc < 4)
    {
        help_err("Not Enough Arguments!\n");
    }
    next_board_num = atoll(argv[context_arg++]);
    past_end_board = 1 + atoll(argv[context_arg++]);
    ;

    if ((stop_at = atoll(argv[context_arg++])) <= 0)
    {
        help_err("print_step (the third argument) must be greater than 0.\n");
    }

    int num_workers = 3;
    for (; context_arg < argc; context_arg++)
    {
        if (!strcmp(argv[context_arg], "--total-iterations-limit"))
        {
            context_arg++;
            if (context_arg == argc)
            {
                help_err(
                    "--total-iterations-limit came without an argument!\n");
            }
            total_iterations_limit_per_board = atol(argv[context_arg]);
        }
        else if (!strcmp(argv[context_arg], "--num-workers"))
        {
            context_arg++;
            if (context_arg == argc)
            {
                help_err("--num-workers came without an argument!\n");
            }
            num_workers = atoi(argv[context_arg]);
        }
        else if (!strcmp(argv[context_arg], "--worker-step"))
        {
            context_arg++;
            if (context_arg == argc)
            {
                help_err("--worker-step came without an argument!\n");
            }
            board_num_step = atoll(argv[context_arg]);
        }
        else if (!strcmp(argv[context_arg], "--iters-update-on"))
        {
            context_arg++;
            if (context_arg == argc)
            {
                help_err("--iters-update-on came without an argument!\n");
            }
            update_total_num_iters_threshold = atol(argv[context_arg]);
        }
        else
        {
            break;
        }
    }

    fc_solve_print_started_at();
    context_argc = argc;
    context_argv = argv;
    pthread_t workers[num_workers];

    for (int idx = 0; idx < num_workers; idx++)
    {
        const int check =
            pthread_create(&workers[idx], NULL, worker_thread, NULL);
        if (check)
        {
            fc_solve_err(
                "Worker Thread No. %d Initialization failed with error %d!\n",
                idx, check);
        }
    }

    /* Wait for all threads to finish. */
    for (int idx = 0; idx < num_workers; idx++)
    {
        pthread_join(workers[idx], NULL);
    }

    fc_solve_print_finished(total_num_iters);

    return 0;
}
