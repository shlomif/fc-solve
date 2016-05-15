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
 * test_multi_parallel.c - the Freecell Solver range solver.
 *
 * Also see:
 *  - fc_pro_range_solver.c - range solver that can count the moves in
 *  the solution and display the FC-Pro ones.
 *  - threaded_range_solver.c - solve different boards in several POSIX
 *  threads.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fcs_user.h"
#include "fcs_cl.h"
#include "rinutils.h"
#include "range_solvers_gen_ms_boards.h"
#include "range_solvers_binary_output.h"
#include "output_to_file.h"
#include "handle_parsing.h"

#ifdef FCS_TRACE_MEM
#include <unistd.h>
#include <sys/types.h>
#endif

static void my_iter_handler(
    void * user_instance,
    fcs_int_limit_t iter_num,
    int depth,
    void * ptr_state,
    fcs_int_limit_t parent_iter_num,
    void * lp_context
    )
{
    fc_solve_display_information_context_t * context;
    context = (fc_solve_display_information_context_t*)lp_context;

    fprintf(stdout, "Iteration: %li\n", (long)iter_num);
    fprintf(stdout, "Depth: %i\n", depth);
    if (context->display_parent_iter_num)
    {
        fprintf(stdout, "Parent Iteration: %li\n", (long)parent_iter_num);
    }
    fprintf(stdout, "\n");


    if (context->debug_iter_state_output)
    {
        char state_string[1000];
        freecell_solver_user_iter_state_stringify(
            user_instance,
            state_string,
            ptr_state
            FC_SOLVE__PASS_PARSABLE(context->parseable_output)
            , context->canonized_order_output
            FC_SOLVE__PASS_T(context->display_10_as_t)
        );
        printf("%s\n---------------\n\n\n", state_string);
    }

}

typedef struct {
    fc_solve_display_information_context_t display_context;
    void * instance;
}pack_item_t;

static int cmd_line_callback(
    void * instance,
    int argc GCC_UNUSED,
    freecell_solver_str_t argv[],
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
        freecell_solver_user_set_iter_handler_long(
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
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
        dc->parseable_output = TRUE;
#endif
    }
    else if ((!strcmp(argv[arg], "-c")) || (!strcmp(argv[arg], "--canonized-order-output")))
    {
        dc->canonized_order_output = TRUE;
    }
    else if ((!strcmp(argv[arg], "-t")) || (!strcmp(argv[arg], "--display-10-as-t")))
    {
#ifndef FC_SOLVE_IMPLICIT_T_RANK
        dc->display_10_as_t = TRUE;
#endif
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
    }
    *num_to_skip = 1;
    return FCS_CMD_LINE_SKIP;
}

static const char * known_parameters[] = {
    "-i", "--iter-output",
    "-s", "--state-output",
    "-p", "--parseable-output",
    "-c", "--canonized-order-output",
    "-sn", "--standard-notation",
    "-snx", "--standard-notation-extended",
    "-sam", "--display-states-and-moves",
    "-t", "--display-10-as-t",
    "-pi", "--display-parent-iter",
    NULL
    };

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
"--binary-output-to   filename\n"
"     Outputs statistics to binary file 'filename'\n"
"--total-iterations-limit  limit\n"
"     Limits each board for up to 'limit' iterations.\n"
          );
}


int main(int argc, char * argv[])
{
    pack_item_t user = {.display_context = INITIAL_DISPLAY_CONTEXT};
    /* char buffer[2048]; */
    int board_num;
    int start_board, end_board, stop_at;
    fcs_portable_time_t mytime;

    fcs_int64_t total_num_iters = 0;

    fcs_bool_t was_total_iterations_limit_per_board_set = FALSE;
    fcs_int_limit_t total_iterations_limit_per_board = -1;

    fcs_state_string_t state_string;

    binary_output_t binary_output;
    binary_output.filename = NULL;
    const char * solutions_directory = NULL;
    char * solution_filename = NULL;

    int arg = 1;
#ifdef FCS_TRACE_MEM
    int rss_found = 0;
    long max_rss_delta_deal = -1;
    long long int prev_rss = -1, max_rss_delta = -1;
#endif

    if (argc < 4)
    {
        fprintf(stderr, "Not Enough Arguments!\n");
        print_help();
        exit(-1);
    }
    start_board = atoi(argv[arg++]);
    end_board = atoi(argv[arg++]);
    stop_at = atoi(argv[arg++]);

    if (stop_at <= 0)
    {
        fprintf(stderr, "print_step (the third argument) must be greater than 0.\n");
        print_help();
        exit(-1);

    }

    for (;arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--binary-output-to"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--binary-output-to came without an argument!\n");
                print_help();
                exit(-1);
            }
            binary_output.filename = argv[arg];
        }
        else if (!strcmp(argv[arg], "--total-iterations-limit"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--total-iterations-limit came without an argument!\n");
                print_help();
                exit(-1);
            }
            was_total_iterations_limit_per_board_set = TRUE;
            total_iterations_limit_per_board = (fcs_int_limit_t)atol(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--solutions-directory"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--solutions-directory came without an argument!\n");
                print_help();
                exit(-1);
            }
            solutions_directory = argv[arg];
            solution_filename = SMALLOC(
                solution_filename,
                strlen(solutions_directory) + 1024
            );
        }
        else
        {
            break;
        }
    }

    FCS_PRINT_STARTED_AT(mytime);
    fflush(stdout);

    bin_init(&binary_output, &start_board, &end_board, &total_iterations_limit_per_board);


    user.instance = alloc_instance_and_parse(
        argc,
        argv,
        &arg,
        known_parameters,
        cmd_line_callback,
        &user,
        TRUE
    );

    for(board_num=start_board;board_num<=end_board;board_num++)
    {
        get_board(board_num, state_string);

        if (was_total_iterations_limit_per_board_set)
        {
            freecell_solver_user_limit_iterations_long(user.instance, total_iterations_limit_per_board);
        }

        const int ret =
            freecell_solver_user_solve_board(
                user.instance,
                state_string
                );

        switch (ret)
        {
            case FCS_STATE_SUSPEND_PROCESS:
            FCS_PRINT_INTRACTABLE_BOARD(mytime, board_num);
            print_int(&binary_output, -1);
            break;

            case FCS_STATE_FLARES_PLAN_ERROR:
            printf("Flares Plan: %s\n", freecell_solver_user_get_last_error_string(user.instance));

            goto out_of_loop;

            case FCS_STATE_IS_NOT_SOLVEABLE:
            FCS_PRINT_UNSOLVED_BOARD(mytime, board_num);
            print_int(&binary_output, -2);
            break;

            default:
            print_int(&binary_output, (int)freecell_solver_user_get_num_times_long(user.instance));
            break;
        }

        if (solutions_directory)
        {
            FILE * output_fh;
            sprintf(solution_filename, "%s%09d.sol",
                solutions_directory, board_num
            );

            output_fh = fopen(solution_filename, "wt");

            if (! output_fh)
            {
                fprintf(stderr,
                        "Could not open output file '%s' for writing!",
                        solution_filename
                       );
                return -1;
            }

            fc_solve_output_result_to_file(
                output_fh, user.instance, ret, &user.display_context
            );

            fclose(output_fh);
            output_fh = NULL;
        }

        total_num_iters += freecell_solver_user_get_num_times_long(user.instance);

        if (board_num % stop_at == 0)
        {
            FCS_PRINT_REACHED_BOARD(mytime, board_num, total_num_iters);
            fflush(stdout);
        }

        freecell_solver_user_recycle(user.instance);

#if 0
#elif defined(FCS_TRACE_MEM)
    {
#define STEP 100000
        long long int rss;
        unsigned long long unsigned_foo;
        char stat_fn[1024], foo_str[1024];
        FILE * stat;

        snprintf(stat_fn, sizeof(stat_fn), "/proc/%ld/stat", (long)(getpid()));

        {
            /* This was taken from:
             *
             * http://www.brokestream.com/procstat.html
             * */
            stat = fopen(stat_fn, "r");
#define readone(foo) (fscanf(stat, "%lld ", &rss))
#define readstr(foo) (fscanf(stat, "%1000s ", foo_str))
#define readchar(foo) (fscanf(stat, "%c ", foo_str))
#define readunsigned(foo) (fscanf(stat, "%llu ", &unsigned_foo))
            readone(&pid);
            readstr(tcomm);
            readchar(&state);
            readone(&ppid);
            readone(&pgid);
            readone(&sid);
            readone(&tty_nr);
            readone(&tty_pgrp);
            readone(&flags);
            readone(&min_flt);
            readone(&cmin_flt);
            readone(&maj_flt);
            readone(&cmaj_flt);
            readone(&utime);
            readone(&stimev);
            readone(&cutime);
            readone(&cstime);
            readone(&priority);
            readone(&nicev);
            readone(&num_threads);
            readone(&it_real_value);
            readunsigned(&start_time);
            readone(&vsize);
            readone(&rss);
#undef readone
#undef readunsigned
#undef readchar
#undef readstr

            fclose(stat);

            long long rss_delta = rss - prev_rss;
            if (rss_found == 1)
            {
                max_rss_delta = rss_delta;
                max_rss_delta_deal = board_num;
            }
            else if (rss_found == 2)
            {
                if (rss_delta > max_rss_delta)
                {
                    max_rss_delta = rss_delta;
                    max_rss_delta_deal = board_num;
                }
                printf("Max RSS delta %lld encountered at deal %ld\n",
                    max_rss_delta, max_rss_delta_deal
                );
            }
            prev_rss = rss;
            if (rss_found < 2)
            {
                rss_found++;
            }
        }
    }
#endif
    }
out_of_loop:

    freecell_solver_user_free(user.instance);
    bin_close(&binary_output);
    free (solution_filename);
    solution_filename = NULL;

    return 0;
}
