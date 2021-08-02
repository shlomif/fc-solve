// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish

// pi_make_microsoft_freecell_board.c - Program to generate the initial
// board of Microsoft Freecell or Freecell Pro for input to Freecell Solver.
//
// Usage: pi-make-microsoft-freecell-board -t [board-number] | fc-solve
//
// Based on the code by Jim Horne (who wrote the original Microsoft Freecell)
// Based on code from the Microsoft C Run-Time Library.
// Modified By Shlomi Fish, 2000
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "range_solvers_gen_ms_boards.h"

int main(int argc, char *argv[])
{
    bool print_ts = false;

    int arg = 1;
    if (arg < argc && !strcmp(argv[arg], "-t"))
    {
        print_ts = true;
        ++arg;
    }
    const char *const deal_s = argv[arg];

    const fc_solve_ms_deal_idx_type gamenumber =
        ((arg < argc) ? fcs_str2msdeal(deal_s) : 1);
    const fc_solve_ms_deal_idx_type MAX = ((1ULL << 33) - 1);
    const fc_solve_ms_deal_idx_type MIN = 1;
    if (gamenumber < MIN || gamenumber > MAX)
    {
        fprintf(stderr,
            "Deal No. \"%s\" is out of the valid range ( " RIN_ULL_FMT
            " - " RIN_ULL_FMT " )!\n",
            deal_s, MIN, MAX);
        return -1;
    }

    fcs_state_string s;
    get_board_l(gamenumber, s);

    if (print_ts)
    {
        fputs(s, stdout);
    }
    else
    {
        const char *p = s;
        char c;
        while ((c = *(p++)) != '\0')
        {
            if (c == 'T')
            {
                fputs("10", stdout);
            }
            else
            {
                putc(c, stdout);
            }
        }
    }
    return 0;
}
