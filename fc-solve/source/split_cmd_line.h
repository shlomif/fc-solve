/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// split_cmd_line.h: split command line arguments from a big string according
// to a subset of Bourne shell's semantics. Useful for reading command
// line arguments from files.
#pragma once
#include "rinutils.h"

#define FC_SOLVE__ARGS_MAN_GROW_BY 32

typedef struct
{
    int argc;
    char **argv;
} fcs_args_man;

extern void fc_solve_args_man_free(fcs_args_man *const manager);
extern fcs_args_man fc_solve_args_man_chop(const char *const string);
