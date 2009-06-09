/* Copyright (c) 2000 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
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

