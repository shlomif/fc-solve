/*
 * move.h - main header file for the Freecell Solver library.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */
#ifndef __FCS_USER_H
#define __FCS_USER_H

#include "fcs_enums.h"
#include "fcs_move.h"

#ifdef __cplusplus
extern "C" {
#endif


extern void * freecell_solver_user_alloc(void);

extern int freecell_solver_user_apply_preset(
    void * instance,
    const char * preset_name
    );

extern void freecell_solver_user_limit_iterations(
    void * user_instance,
    int max_iters
    );

extern int freecell_solver_user_set_tests_order(
    void * user_instance,
    const char * tests_order,
    char * * error_string
    );

extern int freecell_solver_user_solve_board(
    void * user_instance,
    const char * state_as_string
    );

extern int freecell_solver_user_resume_solution(
    void * user_instance
    );

extern int freecell_solver_user_get_next_move(
    void * user_instance,
    fcs_move_t * move
    );

extern char * freecell_solver_user_current_state_as_string(
    void * user_instance,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    );

extern void freecell_solver_user_free(
    void * user_instance
    );

extern int freecell_solver_user_get_current_depth(
    void * user_instance
    );

extern void freecell_solver_user_set_solving_method(
    void * user_instance,
    int method
    );

extern int freecell_solver_user_get_num_times(
    void * user_instance
    );

extern int freecell_solver_user_get_limit_iterations(
    void * user_instance
    );

extern int freecell_solver_user_get_moves_left(
    void * user_instance
    );

extern int freecell_solver_user_set_game(
    void * user_instance,
    int freecells_num,
    int stacks_num,
    int decks_num,
    int sequences_are_built_by,
    int unlimited_sequence_move,
    int empty_stacks_fill
    );

extern void freecell_solver_user_set_solution_optimization(
    void * user_instance,
    int optimize
);

extern char * freecell_solver_user_move_to_string(
    fcs_move_t move,
    int standard_notation
    );

extern void freecell_solver_user_limit_depth(
    void * user_instance,
    int max_depth
    );

extern int freecell_solver_user_set_num_freecells(
    void * user_instance,
    int freecells_num
    );

extern int freecell_solver_user_get_max_num_freecells(void);

extern int freecell_solver_user_set_num_stacks(
    void * user_instance,
    int stacks_num
    );

extern int freecell_solver_user_get_max_num_stacks(void);

extern int freecell_solver_user_set_num_decks(
    void * user_instance,
    int decks_num
    );

extern int freecell_solver_user_get_max_num_decks(void);


extern char * freecell_solver_user_get_invalid_state_error_string(
    void * user_instance,
    int print_ts
    );

extern int freecell_solver_user_set_sequences_are_built_by_type(
    void * user_instance,
    int sbb
    );

extern int freecell_solver_user_set_empty_stacks_filled_by(
    void * user_instance,
    int es_fill
    );

extern int freecell_solver_user_set_sequence_move(
    void * user_instance,
    int unlimited
    );

extern int freecell_solver_user_set_a_star_weight(
    void * user_instance,
    int index,
    double weight
    );

typedef void (*freecell_solver_user_iter_handler_t)
    (
     void * user_instance,
     int iter_num,
     int depth,
     void * ptr_state,
     int parent_iter_num,
     void * context
     );

extern void freecell_solver_user_set_iter_handler(
    void * user_instance,
    freecell_solver_user_iter_handler_t iter_handler,
    void * iter_handler_context
    );


extern char * freecell_solver_user_iter_state_as_string(
    void * user_instance,
    void * ptr_state,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    );

extern void freecell_solver_user_set_random_seed(
    void * user_instance,
    int seed
    );

extern int freecell_solver_user_get_num_states_in_collection(
    void * user_instance
    );

extern void freecell_solver_user_limit_num_states_in_collection(
    void * user_instance,
    int max_num_states
    );

extern int freecell_solver_user_next_soft_thread(
    void * user_instance
    );

extern void freecell_solver_user_set_soft_thread_step(
    void * user_instance,
    int num_times_step
    );

extern int freecell_solver_user_next_hard_thread(
    void * user_instance
    );

extern int freecell_solver_user_get_num_soft_threads_in_instance(
    void * user_instance
    );

extern void freecell_solver_user_set_calc_real_depth(
    void * user_instance,
    int calc_real_depth
    );

extern void freecell_solver_user_set_soft_thread_name(
    void * user_instance,
    char * name
    );

extern int freecell_solver_user_set_hard_thread_prelude(
    void * user_instance,
    char * prelude
    );

extern void freecell_solver_user_recycle(
    void * user_instance
    );

extern int freecell_solver_user_set_optimization_scan_tests_order(
    void * user_instance,
    const char * tests_order,
    char * * error_string
    );

extern void freecell_solver_user_set_reparent_states(
    void * user_instance,
    int to_reparent_states
    );

extern void freecell_solver_user_set_scans_synergy(
    void * user_instance,
    int synergy
    );

#ifdef __cplusplus
}
#endif

#endif /* __FCS_USER_H */
