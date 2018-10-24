/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2016 Shlomi Fish
 */
#pragma once

#include "main_cl_callback_common.h"

static inline FILE *fc_solve_calc_file_handle(
    const int argc, char **const argv, const int arg)
{
    if ((arg == argc) || (!strcmp(argv[arg], "-")))
    {
        if (!getenv("FREECELL_SOLVER_QUIET"))
        {
            fprintf(stderr, "%s",
                "Reading the board from the standard input.\n"
                "Please refer to the documentation for more usage "
                "information:\n"
                "    http://fc-solve.shlomifish.org/docs/\n"
                "To cancel this message set the FREECELL_SOLVER_QUIET "
                "environment variable.\n");
        }
        return stdin;
    }
    else if (argv[arg][0] == '-')
    {
        fprintf(stderr,
            "Unknown option \"%s\". "
            "Type \"%s --help\" for usage information.\n",
            argv[arg], argv[0]);
        return NULL;
    }
    else
    {
        FILE *const f = fopen(argv[arg], "r");
        if (!f)
        {
            fprintf(stderr, "Could not open file \"%s\" for input. Exiting.\n",
                argv[arg]);
        }
        return f;
    }
}

int main(const int argc, char **const argv)
{
    display_context = INITIAL_DISPLAY_CONTEXT;
    int arg = 1;
    instance = alloc_instance_and_parse(argc, argv, &arg, known_parameters,
        fc_solve__cmd_line_callback, &display_context, FALSE);

    FILE *const f = fc_solve_calc_file_handle(argc, argv, arg);
    if (!f)
    {
        freecell_solver_user_free(instance);
        return -1;
    }

    const fcs_user_state_str user_state = read_state(f);
/* Win32 Does not have those signals */
#ifndef WIN32
    signal(SIGUSR1, command_signal_handler);
    signal(SIGUSR2, select_signal_handler);
    signal(SIGABRT, abort_signal_handler);
#endif
    const int ret = solve_board(instance, user_state.s);
    exit_code_type exit_code = SUCCESS;
    switch (ret)
    {
    case FCS_STATE_INVALID_STATE:
    {
#ifdef FCS_WITH_ERROR_STRS
        char error_string[120];

        freecell_solver_user_get_invalid_state_error_into_string(instance,
            error_string FC_SOLVE__PASS_T(display_context.display_10_as_t));
        fprintf(stderr, "%s\n", error_string);
#else
        fprintf(stderr, "%s\n", "Invalid state");
#endif
        exit_code = ERROR;
    }
    break;

    case FCS_STATE_FLARES_PLAN_ERROR:
        print_flares_plan_error(instance);
        exit_code = ERROR;
        break;

    default:
    {
        FILE *output_fh;

        if (display_context.output_filename)
        {
            if (!(output_fh = fopen(display_context.output_filename, "wt")))
            {
                fprintf(stderr, "Could not open output file '%s' for writing!",
                    display_context.output_filename);
                return -1;
            }
        }
        else
        {
            output_fh = stdout;
        }

        fc_solve_output_result_to_file(
            output_fh, instance, ret, &display_context);

        if (display_context.output_filename)
        {
            fclose(output_fh);
        }
    }
    break;
    }

    freecell_solver_user_free(instance);

    return exit_code;
}
