/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// serial_range_solver.c - the Freecell Solver range solver.
//
// Also see:
//  - fc_pro_range_solver.c - range solver that can count the moves in
//  the solution and display the FC-Pro ones.
//  - threaded_range_solver.c - solve different boards in several POSIX
//  threads.
#include "default_iter_handler.h"
#include "cl_callback_range.h"
#include "trace_mem.h"
#include "try_param.h"
#include "range_solvers.h"

static inline int range_solvers_main(int argc, char *argv[], int arg,
    long long start_board, long long end_board, const long long stop_at)
{
    long long total_num_iters = 0;
#ifndef FCS_WITHOUT_MAX_NUM_STATES
    bool was_total_iterations_limit_per_board_set = FALSE;
#endif
    fcs_int_limit_t total_iterations_limit_per_board = -1;
    fcs_binary_output binary_output = INIT_BINARY_OUTPUT;
    const char *solutions_directory = NULL;
    char *solution_fn = NULL;

    for (; arg < argc; arg++)
    {
        const char *param;
        if ((param = TRY_P("--binary-output-to")))
        {
            binary_output.filename = param;
        }
        else if ((param = TRY_P("--total-iterations-limit")))
        {
#ifndef FCS_WITHOUT_MAX_NUM_STATES
            was_total_iterations_limit_per_board_set = TRUE;
            total_iterations_limit_per_board = (fcs_int_limit_t)atol(param);
#endif
        }
        else if ((param = TRY_P("--solutions-directory")))
        {
            solutions_directory = param;
            solution_fn =
                SREALLOC(solution_fn, strlen(solutions_directory) + 1024);
        }
        else
        {
            break;
        }
    }

    fc_solve_print_started_at();
    fflush(stdout);

    fc_solve_display_information_context display_context =
        INITIAL_DISPLAY_CONTEXT;
    void *const instance = alloc_instance_and_parse(argc, argv, &arg,
        known_parameters, cmd_line_callback, &display_context, TRUE);

    bin_init(&binary_output, &start_board, &end_board,
        &total_iterations_limit_per_board);

#ifndef FCS_WITHOUT_MAX_NUM_STATES
    if (was_total_iterations_limit_per_board_set)
    {
        freecell_solver_user_limit_iterations_long(
            instance, total_iterations_limit_per_board);
    }
#endif

    for (long long board_num = start_board; board_num <= end_board; ++board_num)
    {
        fcs_state_string state_string;
        get_board(board_num, state_string);

        const int ret =
            freecell_solver_user_solve_board(instance, state_string);

        switch (ret)
        {
        case FCS_STATE_SUSPEND_PROCESS:
            fc_solve_print_intractable(board_num);
            print_int(&binary_output, -1);
            break;

        case FCS_STATE_FLARES_PLAN_ERROR:
            print_flares_plan_error(instance);
            goto out_of_loop;

        case FCS_STATE_IS_NOT_SOLVEABLE:
            fc_solve_print_unsolved(board_num);
            print_int(&binary_output, -2);
            break;

        case FCS_STATE_WAS_SOLVED:
#ifdef FCS_RANGE_SOLVERS_PRINT_SOLVED
            printf("Solved Board No. " FCS_LL_FMT "\n", board_num);
#endif
            print_int(&binary_output,
                (int)freecell_solver_user_get_num_times_long(instance));
            break;

        default:
            fc_solve_err("%s", "Unknown ret code!");
        }

        if (solutions_directory)
        {
            sprintf(solution_fn, "%s" FCS_LL9_FMT ".sol", solutions_directory,
                board_num);
            FILE *const output_fh = fopen(solution_fn, "wt");
            if (!output_fh)
            {
                fc_solve_err("Could not open output file '%s' for writing!",
                    solution_fn);
            }
            fc_solve_output_result_to_file(
                output_fh, instance, ret, &display_context);
            fclose(output_fh);
        }
        total_num_iters += freecell_solver_user_get_num_times_long(instance);

        if (board_num % stop_at == 0)
        {
            fc_solve_print_reached(board_num, total_num_iters);
        }
        freecell_solver_user_recycle(instance);
        trace_mem(board_num);
    }
out_of_loop:

    freecell_solver_user_free(instance);
    bin_close(&binary_output);
    free(solution_fn);
    solution_fn = NULL;

    return 0;
}
