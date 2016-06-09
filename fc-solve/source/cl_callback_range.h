/*
 * Copyright (c) 2016 Shlomi Fish
 *
 * Licensed under the MIT/X11 license.
 * */
#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "fcs_cl.h"
#include "rinutils.h"
#include "output_to_file.h"
#include "handle_parsing.h"
#include "range_solvers_gen_ms_boards.h"
#include "range_solvers_binary_output.h"
#include "cl_callback_common.h"

static int cmd_line_callback(
    void * const instance,
    const int argc GCC_UNUSED,
    freecell_solver_str_t argv[],
    const int arg,
    int * const num_to_skip,
    int * const ret GCC_UNUSED,
    void * const lp_context
    )
{
    fc_solve_display_information_context_t * const dc = (fc_solve_display_information_context_t *)lp_context;

    *num_to_skip = 0;
    const char * const arg_str = argv[arg];

    if (! cmd_line_cb__handle_common(arg_str, instance, dc))
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
