
#ifndef __CAAS_H
#define __CAAS_H

#ifdef __cplusplus
extern "C" {
#endif

/* #define FCS_USE_INLINE */

#ifdef FCS_USE_INLINE

#ifndef GCC_INLINE
#ifdef __GNUC__
#define GCC_INLINE static inline
#else
#define GCC_INLINE
#endif
#endif

#include "caas.c"

#else   /* #ifdef FCS_USE_INLINE */

#define GCC_INLINE
/*
 * check_and_add_state is defined in caas.c.
 *
 * DFS stands for Depth First Search which is the type of scan Freecell
 * Solver uses to solve a given board.
 * */
extern int freecell_solver_check_and_add_state(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * new_state,
    fcs_state_with_locations_t * * existing_state,
    int depth);
#endif /* #ifdef FCS_USE_INLINE */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __CAAS_H */
