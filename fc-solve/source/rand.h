
#ifndef __RAND_H
#define __RAND_H

#ifdef __cplusplus
extern "C" {
#endif

struct fcs_rand_struct
{
    long seed;
};

typedef struct fcs_rand_struct fcs_rand_t;

extern fcs_rand_t * freecell_solver_rand_alloc(unsigned int seed);
extern void freecell_solver_rand_free(fcs_rand_t * rand);
extern int freecell_solver_rand_get_random_number(fcs_rand_t * rand);

extern void freecell_solver_rand_srand(fcs_rand_t * rand, int seed);

#ifdef __cplusplus
}
#endif

#endif
