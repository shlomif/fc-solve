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
 * fcs_limit.h - the purpose of this file is to define the fcs_int_limit_t
 * type, and the FCS_INT_LIMIT_MAX constant.
 */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static inline __attribute__((noreturn)) void fc_solve_err(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    exit(-1);
}
