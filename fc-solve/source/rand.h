/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// rand.h - header for the Freecell Solver pseudo-random number generator.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rinutils.h"

typedef long fcs_rand_gen;

static inline void fc_solve_rand_init(
    fcs_rand_gen *const my_rand, const unsigned int seed)
{
    *my_rand = (long)seed;
}

static inline int fc_solve_rand_rand15(fcs_rand_gen *const my_rand)
{
    return ((((*my_rand) = ((*my_rand) * 214013 + 2531011)) >> 16) & 0x7fff);
}

// This function constructs a larger integral number of out of two
// 15-bit ones.
static inline int fc_solve_rand_get_random_number(fcs_rand_gen *const my_rand)
{
    const int first = fc_solve_rand_rand15(my_rand);
    return (first | (fc_solve_rand_rand15(my_rand) << 15));
}

#ifdef __cplusplus
}
#endif
