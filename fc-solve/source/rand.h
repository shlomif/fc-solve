
#ifndef __RAND_H
#define __RAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "inline.h"

struct fcs_rand_struct
{
    long seed;
};

typedef struct fcs_rand_struct fcs_rand_t;

extern fcs_rand_t * freecell_solver_rand_alloc(unsigned int seed);
extern void freecell_solver_rand_free(fcs_rand_t *);

extern void freecell_solver_rand_srand(fcs_rand_t *, int seed);

static GCC_INLINE int freecell_solver_rand_rand15(fcs_rand_t * self)
{
    self->seed = (self->seed * 214013 + 2531011);
    return (self->seed >> 16) & 0x7fff;
}

/*
 *
 * This function constructs a larger integral number of out of two
 * 15-bit ones.
 *
 * */
static GCC_INLINE int freecell_solver_rand_get_random_number(fcs_rand_t * self)
{
    int one, two;
    one = freecell_solver_rand_rand15(self);
    two = freecell_solver_rand_rand15(self);

    return (one | (two << 15));
}


#ifdef __cplusplus
}
#endif

#endif
