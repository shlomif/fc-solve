/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// measure_depth_dep_tests_order_performance.c - measure the relative
// depth-dependent performance of two scans.
#include "rinutils.h"
#include "freecell-solver/fcs_cl.h"
#include "range_solvers_gen_ms_boards.h"
#include "try_param.h"
#include "range_solvers.h"

#ifndef FCS_WITHOUT_CMD_LINE_HELP
static void print_help(void)
{
    printf("\n%s",
        "measure-depth-dep-tests-order-performance start end stop_at\n"
        "   [--args-start] [--args-end] [--output-to]\n"
        "[--scan1-to] [--scan2-to] [--iters-limit]\n"
        "[--max-ver-depth]"
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
        "     Limits each board for up to 'limit' iterations.\n");
}
#endif

typedef struct
{
    fcs_portable_time start_time, end_time;
    fcs_int_limit_t num_iters;
    int verdict;
} result;

static const char *known_parameters[] = {NULL};

static void set_tests_order(
    void *const instance, const int min_depth, const char *const moves_order)
{
    FCS__DECL_ERR_PTR(error_string);
    if (freecell_solver_user_set_depth_tests_order(
            instance, min_depth, moves_order FCS__PASS_ERR_STR(&error_string)))
    {
#ifdef FCS_WITH_ERROR_STRS
        fprintf(stderr, "Error! Got '%s'!\n", error_string);
        free(error_string);
#else
        fprintf(stderr, "%s\n", "depth-tests-order Error!");
#endif
        exit(-1);
    }
}

static inline int range_solvers_main(int argc, char *argv[], int arg,
    const long long start_board, const long long end_board,
    const long long stop_at GCC_UNUSED)
{
    /* char buffer[2048]; */
#ifndef FCS_WITHOUT_MAX_NUM_STATES
    fcs_int_limit_t iters_limit = 100000;
#endif
    int max_var_depth_to_check = 100;
    FCS__DECL_ERR_PTR(error_string);
    const char *scan1_to = NULL, *scan2_to = NULL;
    const char *output_filename = NULL;

    int start_from_arg = -1, end_args = -1;

    for (; arg < argc; arg++)
    {
        const char *param;
        if (!strcmp(argv[arg], "--args-start"))
        {
            arg++;

            start_from_arg = arg;
            while (arg < argc)
            {
                if (!strcmp(argv[arg], "--args-end"))
                {
                    break;
                }
                arg++;
            }
            end_args = arg;
        }
        else if ((param = TRY_P("--output-to")))
        {
            output_filename = param;
        }
        else if ((param = TRY_P("--scan1-to")))
        {
            scan1_to = param;
        }
        else if ((param = TRY_P("--scan2-to")))
        {
            scan2_to = param;
        }
        else if ((param = TRY_P("--iters-limit")))
        {
#ifndef FCS_WITHOUT_MAX_NUM_STATES
            iters_limit = (fcs_int_limit_t)atol(param);
#endif
        }
        else if ((param = TRY_P("--max-var-depth")))
        {
            max_var_depth_to_check = atoi(param);
        }
        else
        {
            fc_solve_err("Unknown argument - '%s'!\n", argv[arg]);
        }
    }

    if (!scan1_to)
    {
        fc_solve_err("--scan1-to not specified!\n");
    }

    if (!scan2_to)
    {
        fc_solve_err("--scan2-to not specified!\n");
    }

    if (!output_filename)
    {
        fc_solve_err("output filename not specified");
    }

    fc_solve_print_started_at();
    fflush(stdout);

    result *const results =
        SMALLOC(results, (size_t)(end_board - start_board + 1));

    FILE *const output_fh = fopen(output_filename, "wt");
    for (int min_depth_for_scan2 = 0;
         min_depth_for_scan2 < max_var_depth_to_check; min_depth_for_scan2++)
    {
        void *const instance = freecell_solver_user_alloc();
        if (start_from_arg >= 0)
        {
            freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
                instance, end_args, (freecell_solver_str_t *)(void *)argv,
                start_from_arg, known_parameters, NULL,
                NULL FCS__PASS_ERR_STR(&error_string), &arg, -1, NULL);
        }
        set_tests_order(instance, 0, scan1_to);
        set_tests_order(instance, min_depth_for_scan2, scan2_to);

        result *curr_result;
        int board_num;
        for (board_num = start_board, curr_result = results;
             board_num <= end_board; board_num++, curr_result++)
        {
            fcs_state_string state_string;
            get_board(board_num, state_string);

#ifndef FCS_WITHOUT_MAX_NUM_STATES
            freecell_solver_user_limit_iterations_long(instance, iters_limit);
#endif
            curr_result->start_time = fcs_get_time();
            curr_result->verdict =
                freecell_solver_user_solve_board(instance, state_string);
            curr_result->end_time = fcs_get_time();
            curr_result->num_iters =
                freecell_solver_user_get_num_times_long(instance);

            freecell_solver_user_recycle(instance);
        }

        freecell_solver_user_free(instance);

        printf("Reached depth %d\n", min_depth_for_scan2);

        fprintf(output_fh, "Depth == %d\n\n", min_depth_for_scan2);
        for (board_num = start_board, curr_result = results;
             board_num <= end_board; board_num++, curr_result++)
        {
            fprintf(output_fh, "board[%d].ret == %d\n", board_num,
                curr_result->verdict);
            fprintf(output_fh, "board[%d].iters == %ld\n", board_num,
                (long)curr_result->num_iters);

#define FPRINTF_TIME(label, field)                                             \
    fprintf(output_fh, "board[%d].%s = " FCS_T_FMT "\n", board_num, label,     \
        FCS_TIME_GET_SEC(curr_result->field),                                  \
        FCS_TIME_GET_USEC(curr_result->field));

            FPRINTF_TIME("start", start_time);
            FPRINTF_TIME("end", end_time);
        }
        fflush(output_fh);
    }

    fclose(output_fh);

    return 0;
}
