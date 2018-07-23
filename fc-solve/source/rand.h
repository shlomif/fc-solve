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
 * rand.h - header for the Freecell Solver pseudo-random number generator.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rinutils.h"

#ifdef FCS_BREAK_BACKWARD_COMPAT_2
#include <dSFMT.h>
typedef dsfmt_t fcs_rand_gen;
#else
typedef long fcs_rand_gen;
#endif

static inline void fc_solve_rand_init(
    fcs_rand_gen *const my_rand, const unsigned int seed)
{
#ifdef FCS_BREAK_BACKWARD_COMPAT_2
    dsfmt_init_gen_rand(my_rand, seed);
#else
    *my_rand = (long)seed;
#endif
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_2
static inline int fc_solve_rand_rand15(fcs_rand_gen *const my_rand)
{
    return ((((*my_rand) = ((*my_rand) * 214013 + 2531011)) >> 16) & 0x7fff);
}
#endif

/*
 *
 * This function constructs a larger integral number of out of two
 * 15-bit ones.
 *
 * */
static inline int fc_solve_rand_get_random_number(fcs_rand_gen *const my_rand, uint32_t mod)
{
#ifdef FCS_BREAK_BACKWARD_COMPAT_2
    return dsfmt_genrand_close_open(my_rand) * mod;
#else
    const int first = fc_solve_rand_rand15(my_rand);
    return ((first | (fc_solve_rand_rand15(my_rand) << 15)) % mod);
#endif
}

#ifdef __cplusplus
}
#endif
