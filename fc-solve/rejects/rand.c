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
 * rand.c: the Freecell Solver pseudo-random number generator.
 */
#include <stdlib.h>

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#include "rand.h"

#if 0
fcs_rand_t * fc_solve_rand_alloc(unsigned int seed)
{
    fcs_rand_t * ret;

    ret = malloc(sizeof(fcs_rand_t));
    ret->seed = (long)seed;

    return ret;
}

void fc_solve_rand_free(fcs_rand_t * rand)
{
    free(rand);
}
#endif

#if 0
void fc_solve_rand_srand(fcs_rand_t * rand, int seed)
{
    rand->seed = seed;
}
#endif
