/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#pragma once

#include <string.h>
#include "fcs_err.h"

static const char *try_argv_param(
    const int argc, char **const argv, int *const arg, const char *const flag)
{
    if (!strcmp(argv[*arg], flag))
    {
        if (++(*arg) == argc)
        {
            fc_solve_err("%s came without an argument!\n", flag);
        }
        return argv[*arg];
    }
    return NULL;
}

#define TRY_PARAM(s) try_argv_param(argc, argv, arg, s)
#define TRY_P(s) try_argv_param(argc, argv, &arg, s)
