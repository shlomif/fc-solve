
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
 * TODO : Add a description of this file.
 */
#ifndef FC_SOLVE__RAND_H
#define FC_SOLVE__RAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "inline.h"

struct fcs_rand_struct
{
    long seed;
};

typedef struct fcs_rand_struct fcs_rand_t;

extern fcs_rand_t * fc_solve_rand_alloc(unsigned int seed);
extern void fc_solve_rand_free(fcs_rand_t * rand);

extern void fc_solve_rand_srand(fcs_rand_t * rand, int seed);

static GCC_INLINE int freecell_solver_rand_rand15(fcs_rand_t * rand)
{
    rand->seed = (rand->seed * 214013 + 2531011);
    return (rand->seed >> 16) & 0x7fff;
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
    one = freecell_solver_rand_rand15(rand);
    two = freecell_solver_rand_rand15(rand);

    return (one | (two << 15));
}


#ifdef __cplusplus
}
#endif

#endif
