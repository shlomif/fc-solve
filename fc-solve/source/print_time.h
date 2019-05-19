/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2010 Shlomi Fish
 */
// portable_time.h - the Freecell Solver mostly portable time handling
// routines. Works on Win32 and UNIX-like systems.
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "portable_time.h"

static inline void fc_solve_print_reached(
    const long long board_num, const fcs_iters_int total_num_iters)
{
    const_AUTO(mytime, rinutils_get_time());
    printf("Reached " FCS_B_AT_FMT " (total_num_iters=" RIN_ULL_FMT ")\n",
        board_num, RIN_TIME__GET_BOTH(mytime),
        (unsigned long long)total_num_iters);
}

static inline void fc_solve_print_finished(const fcs_iters_int total_num_iters)
{
    const_AUTO(mytime, rinutils_get_time());
    printf(("Finished at " RIN_TIME_FMT " (total_num_iters=" RIN_ULL_FMT ")\n"),
        RIN_TIME__GET_BOTH(mytime), (unsigned long long)total_num_iters);
}

#ifdef __cplusplus
};
#endif
