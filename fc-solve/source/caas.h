
#ifndef __CAAS_H
#define __CAAS_H

#ifdef __cplusplus
extern "C" {
#endif

/* #define FCS_USE_INLINE */
    
/*
 * check_and_add_state is defined in caas.c.
 *
 * DFS stands for Depth First Search which is the type of scan Freecell
 * Solver uses to solve a given board.
 * */

extern int freecell_solver_check_and_add_state(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * new_state,
    fcs_state_with_locations_t * * existing_state
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __CAAS_H */
