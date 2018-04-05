/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */

/*
    pi_make_microsoft_freecell_board.c - Program to generate the initial
    board of Microsoft Freecell or Freecell Pro for input to
    Freecell Solver.

    Usage: pi-make-microsoft-freecell-board [board-number] | fc-solve

    Note: this program is platform independent because it uses its own srand()
    and random() functions which are simliar to the ones used by the Microsoft
    32-bit compilers.

    Based on the code by Jim Horne (who wrote the original Microsoft Freecell)
    Based on code from the Microsoft C Run-Time Library.

    Modified By Shlomi Fish, 2000
*/

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "alloc_wrap.h"
#include "bool.h"
#include "range_solvers_gen_ms_boards.h"

int main(int argc, char *argv[])
{
    fcs_bool_t print_ts = FALSE;

    int arg = 1;
    if (arg < argc)
    {
        if (!strcmp(argv[arg], "-t"))
        {
            print_ts = TRUE;
            ++arg;
        }
    }
    const long long gamenumber =
        ((arg < argc) ? atoll(argv[arg++]) : (long long)time(NULL));

    fcs_state_string_t s;
    get_board_l(gamenumber, s);

    if (print_ts)
    {
        fputs(s, stdout);
    }
    else
    {
        const char *p = s;
        while (*p)
        {
            if (*p == 'T')
            {
                fputs("10", stdout);
            }
            else
            {
                putc(*p, stdout);
            }
            ++p;
        }
    }
    return 0;
}
