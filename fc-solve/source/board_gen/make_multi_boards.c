// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
/*
    pi_make_microsoft_freecell_board.c - Program to generate the initial
    board of Microsoft Freecell or Freecell Pro for input to Freecell Solver.

    Usage: pi-make-microsoft-freecell-board -t [board-number] | fc-solve

    Based on the code by Jim Horne (who wrote the original Microsoft Freecell)
    Based on code from the Microsoft C Run-Time Library.
    Modified By Shlomi Fish, 2000
*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "range_solvers_gen_ms_boards.h"
#include "deals_populator.h"
#include "try_param.h"

static void __attribute__((noreturn)) print_help(void)
{
    printf("\n%s", "summary-fc-solve [deal1_idx] [deal2_idx] .. -- \n"
                   "   [--variant variant_str] [fc-solve theme args]\n"
                   "\n"
                   "Attempts to solve several arbitrary deal indexes from the\n"
                   "Microsoft/Freecell Pro deals using the fc-solve's theme "
                   "and reports a\n"
                   "summary of their results to STDOUT\n");
    exit(-1);
}

int main(int argc, char *argv[])
{
    int arg = 1;
    const char *dir = NULL;
    const char *suffix = "";
    for (; arg < argc; ++arg)
    {
        const char *param;
        if ((param = TRY_P("--dir")))
        {
            if (strlen(dir = param) > 100)
            {
                fprintf(stderr, "--dir's argument is too long!\n");
                print_help();
            }
        }
        else if ((param = TRY_P("--suffix")))
        {
            if (strlen(suffix = param) > 20)
            {
                fprintf(stderr, "--suffix's argument is too long!\n");
                print_help();
            }
        }
        else
        {
            break;
        }
    }
    if (!dir)
    {
        fprintf(stderr, "--dir must be specified!\n");
        print_help();
    }
    arg = populate_deals_from_argv(argc, argv, arg);

    DEALS_ITERATE__START(board_num)
    fcs_state_string s;
    get_board_l(board_num, s);
    char filename[256];
    sprintf(filename, ("%s/" RIN_ULL_FMT "%s"), dir, board_num, suffix);
    FILE *f = fopen(filename, "wt");
    fputs(s, f);
    fclose(f);
    DEALS_ITERATE__END()

    return 0;
}
