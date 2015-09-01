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

#include "alloc_wrap.h"
#include "portable_int64.h"
#include "portable_time.h"

#include "fcs_user.h"
#include "fcs_cl.h"
#include "unused.h"
#include "inline.h"
#include "bool.h"
#include "min_and_max.h"

#include "range_solvers_gen_ms_boards.h"

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
"     Limits each board for up to 'limit' iterations.\n"
          );
}

static const pthread_mutex_t initial_mutex_constant =
    PTHREAD_MUTEX_INITIALIZER
    ;

static int next_board_num;
static pthread_mutex_t next_board_num_lock;

typedef struct {
    int argc;
    char * * argv;
    int arg;
    int stop_at;
    int past_end_board;
    int board_num_step;
    int update_total_num_iters_threshold;
    fcs_int_limit_t total_iterations_limit_per_board;
} context_t;

static context_t context = {.arg = 1, .board_num_step = 1, .update_total_num_iters_threshold = 1000000, .total_iterations_limit_per_board = -1};


static fcs_int64_t total_num_iters = 0;
static pthread_mutex_t total_num_iters_lock;

static void * worker_thread(void * GCC_UNUSED void_context)
{
    void * const instance = freecell_solver_user_alloc();

    {
        int arg = context.arg;
        char * error_string;
        switch(
            freecell_solver_user_cmd_line_parse_args(
                instance,
                context.argc,
                (const char * *)(void *)context.argv,
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
                fprintf(stderr, "Unknown option: %s", context.argv[arg]);
                goto ret_label;
            }
            break;

            case FCS_CMD_LINE_PARAM_WITH_NO_ARG:
            {
                fprintf(stderr, "The command line parameter \"%s\" requires an argument"
                    " and was not supplied with one.\n", context.argv[arg]);
                goto ret_label;
            }
            break;

            case FCS_CMD_LINE_ERROR_IN_ARG:
            {
                if (error_string != NULL)
                {
                    fprintf(stderr, "%s", error_string);
                    free(error_string);
                }
                goto ret_label;
            }
            break;
        }
    }
    freecell_solver_user_limit_iterations_long(instance, context.total_iterations_limit_per_board);

    fcs_portable_time_t mytime;
    fcs_int_limit_t total_num_iters_temp = 0;
    int board_num;
    do
    {
        pthread_mutex_lock(&next_board_num_lock);
        board_num = next_board_num;
        const int proposed_quota_end = (next_board_num += context.board_num_step);
        pthread_mutex_unlock(&next_board_num_lock);

        const int quota_end = min(proposed_quota_end, context.past_end_board);

        for ( ; board_num < quota_end ; board_num++ )
        {
            fcs_state_string_t state_string;
            get_board(board_num, state_string);


            switch(
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
                    const char * flares_error_string;

                    flares_error_string =
                        freecell_solver_user_get_last_error_string(instance);

                    fprintf(stderr, "Flares Plan: %s\n", flares_error_string);

                    goto theme_error;
                }
                break;

                case FCS_STATE_IS_NOT_SOLVEABLE:
                {
                    FCS_PRINT_UNSOLVED_BOARD(mytime, board_num);
                }
                break;
            }

            total_num_iters_temp += freecell_solver_user_get_num_times_long(instance);
            if (total_num_iters_temp >= context.update_total_num_iters_threshold)
            {
                pthread_mutex_lock(&total_num_iters_lock);
                total_num_iters += total_num_iters_temp;
                pthread_mutex_unlock(&total_num_iters_lock);
                total_num_iters_temp = 0;
            }

            if (board_num % context.stop_at == 0)
            {
                pthread_mutex_lock(&total_num_iters_lock);
                fcs_int64_t total_num_iters_copy = (total_num_iters += total_num_iters_temp);
                pthread_mutex_unlock(&total_num_iters_lock);
                total_num_iters_temp = 0;

                FCS_PRINT_REACHED_BOARD(
                    mytime,
                    board_num,
                    total_num_iters_copy
                );
                fflush(stdout);
            }

            freecell_solver_user_recycle(instance);
        }
    } while (board_num < context.past_end_board);

    pthread_mutex_lock(&total_num_iters_lock);
    total_num_iters += total_num_iters_temp;
    pthread_mutex_unlock(&total_num_iters_lock);

theme_error:
    freecell_solver_user_free(instance);

ret_label:
    return NULL;
}


int main(int argc, char * argv[])
{
    next_board_num_lock = initial_mutex_constant;
    total_num_iters_lock = initial_mutex_constant;
    if (argc < 4)
    {
        fprintf(stderr, "Not Enough Arguments!\n");
        print_help();
        exit(-1);
    }
    next_board_num = atoi(argv[context.arg++]);
    context.past_end_board = 1 + atoi(argv[context.arg++]);
    ;

    if ((context.stop_at = atoi(argv[context.arg++])) <= 0)
    {
        fprintf(stderr, "print_step (the third argument) must be greater than 0.\n");
        print_help();
        exit(-1);

    }

    int num_workers = 3;
    for (;context.arg < argc; context.arg++)
    {
        if (!strcmp(argv[context.arg], "--total-iterations-limit"))
        {
            context.arg++;
            if (context.arg == argc)
            {
                fprintf(stderr, "--total-iterations-limit came without an argument!\n");
                print_help();
                exit(-1);
            }
            context.total_iterations_limit_per_board = atol(argv[context.arg]);
        }
        else if (!strcmp(argv[context.arg], "--num-workers"))
        {
            context.arg++;
            if (context.arg == argc)
            {
                fprintf(stderr, "--num-workers came without an argument!\n");
                print_help();
                exit(-1);
            }
            num_workers = atoi(argv[context.arg]);
        }
        else if (!strcmp(argv[context.arg], "--worker-step"))
        {
            context.arg++;
            if (context.arg == argc)
            {
                fprintf(stderr, "--worker-step came without an argument!\n");
                print_help();
                exit(-1);
            }
            context.board_num_step = atoi(argv[context.arg]);
        }
        else if (!strcmp(argv[context.arg], "--iters-update-on"))
        {
            context.arg++;
            if (context.arg == argc)
            {
                fprintf(stderr, "--iters-update-on came without an argument!\n");
                print_help();
                exit(-1);
            }
            context.update_total_num_iters_threshold = atoi(argv[context.arg]);
        }
        else
        {
            break;
        }
    }

    fcs_portable_time_t mytime;
    FCS_PRINT_STARTED_AT(mytime);
    fflush(stdout);

    context.argc = argc;
    context.argv = argv;

    pthread_t * const workers = SMALLOC(workers, num_workers);

    for ( int idx = 0 ; idx < num_workers ; idx++)
    {
        const int check = pthread_create(
            &workers[idx],
            NULL,
            worker_thread,
            NULL
        );
        if (check)
        {
            fprintf(stderr,
                "Worker Thread No. %d Initialization failed with error %d!\n",
                idx, check
            );
            exit(-1);
        }
    }

    /* Wait for all threads to finish. */
    for( int idx = 0 ; idx < num_workers ; idx++)
    {
        pthread_join(workers[idx], NULL);
    }

    FCS_PRINT_FINISHED(mytime, total_num_iters);

    free(workers);

    return 0;
}

