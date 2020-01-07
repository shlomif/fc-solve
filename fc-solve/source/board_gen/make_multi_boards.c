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

int main(int argc, char *argv[])
{
    int arg = populate_deals_from_argv(argc, argv);

    DEALS_ITERATE__START(board_num)
    fcs_state_string s;
    get_board_l(board_num, s);
    char filename[256];
    sprintf(filename, "../foo/%llu.board", board_num);
    FILE *f = fopen(filename, "wt");
    fputs(s, f);
    fclose(f);
    DEALS_ITERATE__END()

    return 0;
}
