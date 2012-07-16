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

#include "portable_int64.h"
#include "portable_time.h"

#include "fcs_user.h"
#include "fcs_cl.h"
#include "unused.h"
#include "inline.h"
#include "bool.h"
#include "range_solvers_gen_ms_boards.h"

struct fc_solve_display_information_context_struct
{
    fcs_bool_t debug_iter_state_output;
    int freecells_num;
    int stacks_num;
    int decks_num;
    fcs_bool_t parseable_output;
    fcs_bool_t canonized_order_output;
    fcs_bool_t display_10_as_t;
    fcs_bool_t display_parent_iter_num;
    fcs_bool_t debug_iter_output_on;
    fcs_bool_t display_moves;
    fcs_bool_t display_states;
    fcs_bool_t standard_notation;
};

typedef struct fc_solve_display_information_context_struct fc_solve_display_information_context_t;

static void my_iter_handler(
    void * user_instance,
    int iter_num,
    int depth,
    void * ptr_state,
    int parent_iter_num,
    void * lp_context
    )
{
    fc_solve_display_information_context_t * context;
    context = (fc_solve_display_information_context_t*)lp_context;

    fprintf(stdout, "Iteration: %i\n", iter_num);
    fprintf(stdout, "Depth: %i\n", depth);
    if (context->display_parent_iter_num)
    {
        fprintf(stdout, "Parent Iteration: %i\n", parent_iter_num);
    }
    fprintf(stdout, "\n");


    if (context->debug_iter_state_output)
    {
        char * state_string =
            freecell_solver_user_iter_state_as_string(
                user_instance,
                ptr_state,
                context->parseable_output,
                context->canonized_order_output,
                context->display_10_as_t
                );
        printf("%s\n---------------\n\n\n", state_string);
        free((void*)state_string);
    }

}

struct pack_item_struct
{
    fc_solve_display_information_context_t display_context;
    void * instance;
};

typedef struct pack_item_struct pack_item_t;


static int cmd_line_callback(
    void * instance,
    int argc GCC_UNUSED,
    const char * argv[],
    int arg,
    int * num_to_skip,
    int * ret GCC_UNUSED,
    void * context
    )
{
    pack_item_t * item;
    fc_solve_display_information_context_t * dc;
    item = (pack_item_t * )context;
    dc = &(item->display_context);

    *num_to_skip = 0;

    if ((!strcmp(argv[arg], "-i")) || (!strcmp(argv[arg], "--iter-output")))
    {
        freecell_solver_user_set_iter_handler(
            instance,
            my_iter_handler,
            dc
            );
        dc->debug_iter_output_on = TRUE;
    }
    else if ((!strcmp(argv[arg], "-s")) || (!strcmp(argv[arg], "--state-output")))
    {
        dc->debug_iter_state_output = TRUE;
    }
    else if ((!strcmp(argv[arg], "-p")) || (!strcmp(argv[arg], "--parseable-output")))
    {
        dc->parseable_output = TRUE;
    }
    else if ((!strcmp(argv[arg], "-c")) || (!strcmp(argv[arg], "--canonized-order-output")))
    {
        dc->canonized_order_output = TRUE;
    }
    else if ((!strcmp(argv[arg], "-t")) || (!strcmp(argv[arg], "--display-10-as-t")))
    {
        dc->display_10_as_t = TRUE;
    }
    else if ((!strcmp(argv[arg], "-m")) || (!strcmp(argv[arg], "--display-moves")))
    {
        dc->display_moves = TRUE;
        dc->display_states = FALSE;
    }
    else if ((!strcmp(argv[arg], "-sn")) || (!strcmp(argv[arg], "--standard-notation")))
    {
        dc->standard_notation = TRUE;
    }
    else if ((!strcmp(argv[arg], "-sam")) || (!strcmp(argv[arg], "--display-states-and-moves")))
    {
        dc->display_moves = TRUE;
        dc->display_states = TRUE;
    }
    else if ((!strcmp(argv[arg], "-pi")) || (!strcmp(argv[arg], "--display-parent-iter")))
    {
        dc->display_parent_iter_num = TRUE;
    }
    else
    {
        fprintf(stderr, "Unknown option %s!\n", argv[arg]);
        exit(-1);
        return 0;
    }
    *num_to_skip = 1;
    return FCS_CMD_LINE_SKIP;
}

static const char * known_parameters[] = {
    "-i", "--iter-output",
    "-s", "--state-output",
    "-p", "--parseable-output",
    "-t", "--display-10-as-t",
    "-pi", "--display-parent-iter",
    NULL
    };

#define BINARY_OUTPUT_NUM_INTS 16

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
static int end_board;
static pthread_mutex_t next_board_num_lock;
static int board_num_step = 1;
static int update_total_num_iters_threshold = 1000000;

typedef struct {
    int argc;
    char * * argv;
    int arg;
    int stop_at;
} context_t;

fcs_int_limit_t total_iterations_limit_per_board = -1;

fcs_int64_t total_num_iters = 0;
static pthread_mutex_t total_num_iters_lock;

static void * worker_thread(void * void_context)
{
    context_t * context;
    pack_item_t user;
    char * error_string;
    int arg;
    int parser_ret;
    int ret;
    char * * argv;
    int board_num;
    int quota_end;
    int stop_at;
    fcs_portable_time_t mytime;
    fcs_int_limit_t total_num_iters_temp = 0;
    /* 52 cards of 3 chars (suit+rank+whitespace) each,
     * plus 8 newlines, plus one '\0' terminator*/
    fcs_state_string_t state_string;

    context = (context_t *)void_context;
    arg = context->arg;
    argv = context->argv;
    stop_at = context->stop_at;

    user.instance = freecell_solver_user_alloc();

    parser_ret =
        freecell_solver_user_cmd_line_parse_args(
            user.instance,
            context->argc,
            (const char * *)(void *)argv,
            arg,
            known_parameters,
            cmd_line_callback,
            &user,
            &error_string,
            &arg
            );

    if (parser_ret == FCS_CMD_LINE_UNRECOGNIZED_OPTION)
    {
        fprintf(stderr, "Unknown option: %s", argv[arg]);
        goto ret_label;
    }
    else if (parser_ret == FCS_CMD_LINE_PARAM_WITH_NO_ARG)
    {
        fprintf(stderr, "The command line parameter \"%s\" requires an argument"
                " and was not supplied with one.\n", argv[arg]);
        goto ret_label;
    }
    else if (parser_ret == FCS_CMD_LINE_ERROR_IN_ARG)
    {
        if (error_string != NULL)
        {
            fprintf(stderr, "%s", error_string);
            free(error_string);
        }
        goto ret_label;
    }

    ret = 0;

    do
    {
        pthread_mutex_lock(&next_board_num_lock);
        board_num = next_board_num;
        quota_end = (next_board_num += board_num_step);
        pthread_mutex_unlock(&next_board_num_lock);

        if (quota_end > end_board+1)
        {
            quota_end = end_board+1;
        }
        for(;board_num<quota_end;board_num++)
        {
            get_board(board_num, state_string);

            freecell_solver_user_limit_iterations_long(user.instance, total_iterations_limit_per_board);

            ret =
                freecell_solver_user_solve_board(
                    user.instance,
                    state_string
                    );

            if (ret == FCS_STATE_SUSPEND_PROCESS)
            {
                FCS_PRINT_INTRACTABLE_BOARD(mytime, board_num);
                fflush(stdout);
            }
            else if (ret == FCS_STATE_FLARES_PLAN_ERROR)
            {
                const char * flares_error_string;

                flares_error_string =
                    freecell_solver_user_get_last_error_string(user.instance);

                fprintf(stderr, "Flares Plan: %s\n", flares_error_string);

                goto theme_error;
            }
            else if (ret == FCS_STATE_IS_NOT_SOLVEABLE)
            {
                FCS_PRINT_UNSOLVED_BOARD(mytime, board_num);

            }

            total_num_iters_temp += freecell_solver_user_get_num_times_long(user.instance);
            if (total_num_iters_temp >= update_total_num_iters_threshold)
            {
                pthread_mutex_lock(&total_num_iters_lock);
                total_num_iters += total_num_iters_temp;
                pthread_mutex_unlock(&total_num_iters_lock);
                total_num_iters_temp = 0;
            }

            if (board_num % stop_at == 0)
            {
                fcs_int64_t total_num_iters_copy;

                pthread_mutex_lock(&total_num_iters_lock);
                total_num_iters_copy = (total_num_iters += total_num_iters_temp);
                pthread_mutex_unlock(&total_num_iters_lock);
                total_num_iters_temp = 0;

                FCS_PRINT_REACHED_BOARD(
                    mytime,
                    board_num,
                    total_num_iters_copy
                );
                fflush(stdout);
            }

            freecell_solver_user_recycle(user.instance);
        }
    } while (board_num <= end_board);

    pthread_mutex_lock(&total_num_iters_lock);
    total_num_iters += total_num_iters_temp;
    pthread_mutex_unlock(&total_num_iters_lock);

theme_error:
    freecell_solver_user_free(user.instance);

ret_label:
    return NULL;
}


int main(int argc, char * argv[])
{
    int stop_at;
    fcs_portable_time_t mytime;

    int num_workers = 3;
    pthread_t * workers;

    int arg = 1, start_from_arg, idx, check;

    context_t context;

    next_board_num_lock = initial_mutex_constant;
    total_num_iters_lock = initial_mutex_constant;
    if (argc < 4)
    {
        fprintf(stderr, "Not Enough Arguments!\n");
        print_help();
        exit(-1);
    }
    next_board_num = atoi(argv[arg++]);
    end_board = atoi(argv[arg++]);
    stop_at = atoi(argv[arg++]);

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
            total_iterations_limit_per_board = atol(argv[arg]);
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
        else if (!strcmp(argv[arg], "--iters-update-on"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--iters-update-on came without an argument!\n");
                print_help();
                exit(-1);
            }
            update_total_num_iters_threshold = atoi(argv[arg]);
        }
        else
        {
            break;
        }
    }

    start_from_arg = arg;

    FCS_PRINT_STARTED_AT(mytime);
    fflush(stdout);

    context.argc = argc;
    context.argv = argv;
    context.arg = start_from_arg;
    context.stop_at = stop_at;

    workers = malloc(sizeof(workers[0])*num_workers);

    for ( idx = 0 ; idx < num_workers ; idx++)
    {
        check = pthread_create(
            &workers[idx],
            NULL,
            worker_thread,
            &context
        );
        if (check)
        {
            fprintf(stderr,
                "Worker Thread No. %d Initialization failed!\n",
                idx
            );
            exit(-1);
        }
    }

    /* Wait for all threads to finish. */
    for( idx = 0 ; idx < num_workers ; idx++)
    {
        pthread_join(workers[idx], NULL);
    }

    FCS_PRINT_FINISHED(mytime, total_num_iters);

    free(workers);

    return 0;
}

