
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
 * rand.h - header for the Freecell Solver pseudo-random number generator.
 */
#ifndef FC_SOLVE__RAND_H
#define FC_SOLVE__RAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "inline.h"

typedef long fcs_rand_t;

static GCC_INLINE void fc_solve_rand_init(fcs_rand_t * rand, unsigned int seed)
{
    *rand = (long)seed;
}

static GCC_INLINE int fc_solve_rand_rand15(fcs_rand_t * rand)
{
    *rand = ((*rand) * 214013 + 2531011);
    return ((*rand) >> 16) & 0x7fff;
}

/*
 *
 * This function constructs a larger integral number of out of two
 * 15-bit ones.
 *
 * */
static GCC_INLINE int fc_solve_rand_get_random_number(fcs_rand_t * rand)
{
    int one, two;
    one = fc_solve_rand_rand15(rand);
    two = fc_solve_rand_rand15(rand);

    return (one | (two << 15));
}


#ifdef __cplusplus
}
#endif

#endif
