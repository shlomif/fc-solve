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
#include "default_iter_handler.h"
#include "cl_callback_range.h"
#include "handle_parsing.h"
#include "trace_mem.h"

int main(int argc, char *argv[])
{
    fcs_portable_time_t mytime;
    fcs_int64_t total_num_iters = 0;
    fcs_bool_t was_total_iterations_limit_per_board_set = FALSE;
    fcs_int_limit_t total_iterations_limit_per_board = -1;
    binary_output_t binary_output = {
        .filename = NULL,
    };
    const char *solutions_directory = NULL;
    char *solution_filename = NULL;
    int arg = 1;

    if (argc < 4)
    {
        fprintf(stderr, "Not Enough Arguments!\n");
        print_help();
        exit(-1);
    }
    long long start_board_idx = atoll(argv[arg++]);
    long long end_board_idx = atoll(argv[arg++]);
    const long long stop_at = atoll(argv[arg++]);
    if (stop_at <= 0)
    {
        fprintf(stderr,
            "print_step (the third argument) must be greater than 0.\n");
        print_help();
        exit(-1);
    }

    for (; arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--binary-output-to"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(
                    stderr, "--binary-output-to came without an argument!\n");
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
                fprintf(stderr,
                    "--total-iterations-limit came without an argument!\n");
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
                fprintf(stderr,
                    "--solutions-directory came without an argument!\n");
                print_help();
                exit(-1);
            }
            solutions_directory = argv[arg];
            solution_filename =
                SMALLOC(solution_filename, strlen(solutions_directory) + 1024);
        }
        else
        {
            break;
        }
    }

    FCS_PRINT_STARTED_AT(mytime);
    fflush(stdout);

    fc_solve_display_information_context_t display_context =
        INITIAL_DISPLAY_CONTEXT;
    void *const instance = alloc_instance_and_parse(argc, argv, &arg,
        known_parameters, cmd_line_callback, &display_context, TRUE);

    bin_init(&binary_output, &start_board_idx, &end_board_idx,
        &total_iterations_limit_per_board);

    if (was_total_iterations_limit_per_board_set)
    {
        freecell_solver_user_limit_iterations_long(
            instance, total_iterations_limit_per_board);
    }

    for (long long board_num = start_board_idx; board_num <= end_board_idx;
         board_num++)
    {
        fcs_state_string_t state_string;
        get_board(board_num, state_string);

        const int ret =
            freecell_solver_user_solve_board(instance, state_string);

        switch (ret)
        {
        case FCS_STATE_SUSPEND_PROCESS:
            FCS_PRINT_INTRACTABLE_BOARD(mytime, board_num);
            print_int(&binary_output, -1);
            break;

        case FCS_STATE_FLARES_PLAN_ERROR:
#ifdef FCS_WITH_ERROR_STRS
            printf("Flares Plan: %s\n",
                freecell_solver_user_get_last_error_string(instance));
#else
            printf("%s\n", "Flares Plan Error");
#endif
            goto out_of_loop;

        case FCS_STATE_IS_NOT_SOLVEABLE:
            FCS_PRINT_UNSOLVED_BOARD(mytime, board_num);
            print_int(&binary_output, -2);
            break;

        default:
            print_int(&binary_output,
                (int)freecell_solver_user_get_num_times_long(instance));
            break;
        }

        if (solutions_directory)
        {
            sprintf(solution_filename, "%s%09lld.sol", solutions_directory,
                board_num);
            FILE *const output_fh = fopen(solution_filename, "wt");
            if (!output_fh)
            {
                fprintf(stderr, "Could not open output file '%s' for writing!",
                    solution_filename);
                return -1;
            }
            fc_solve_output_result_to_file(
                output_fh, instance, ret, &display_context);
            fclose(output_fh);
        }
        total_num_iters += freecell_solver_user_get_num_times_long(instance);

        if (board_num % stop_at == 0)
        {
            FCS_PRINT_REACHED_BOARD(mytime, board_num, total_num_iters);
        }

        freecell_solver_user_recycle(instance);

        trace_mem(board_num);
    }
out_of_loop:

    freecell_solver_user_free(instance);
    bin_close(&binary_output);
    free(solution_filename);
    solution_filename = NULL;

    return 0;
}
