#include <stdlib.h>

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#include "rand.h"

fcs_rand_t * freecell_solver_rand_alloc(unsigned int seed)
{
    fcs_rand_t * ret;

    ret = malloc(sizeof(fcs_rand_t));
    ret->seed = (long)seed;

    return ret;
}

void freecell_solver_rand_free(fcs_rand_t * rand)
{
    free(rand);
}

static int freecell_solver_rand_rand15(fcs_rand_t * rand)
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
int freecell_solver_rand_get_random_number(fcs_rand_t * rand)
{
    int one, two;
    one = freecell_solver_rand_rand15(rand);
    two = freecell_solver_rand_rand15(rand);

    return (one | (two << 15));
}

void freecell_solver_rand_srand(fcs_rand_t * rand, int seed)
{
    rand->seed = seed;
}


