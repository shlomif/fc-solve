#include <stdlib.h>

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#include "rand.h"

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


void fc_solve_rand_srand(fcs_rand_t * rand, int seed)
{
    rand->seed = seed;
}


