
#ifndef FC_SOLVE__CAAS_H
#define FC_SOLVE__CAAS_H

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

extern int fc_solve_check_and_add_state(
    fc_solve_soft_thread_t * soft_thread,
    fcs_state_t * new_state_key,
    fcs_state_extra_info_t * new_state_val,
    fcs_state_t * * existing_state_key,
    fcs_state_extra_info_t * * existing_state_val
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FC_SOLVE__CAAS_H */
