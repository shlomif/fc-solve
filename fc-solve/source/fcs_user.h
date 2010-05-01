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
 * move.h - main header file for the Freecell Solver library.
 *
 */
#ifndef FC_SOLVE__FCS_USER_H
#define FC_SOLVE__FCS_USER_H

#include "fcs_dllexport.h"
#include "fcs_enums.h"
#include "fcs_move.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FC_SOLVE_STRINGS_COMPAT
typedef char * freecell_solver_str_t;
#else
typedef const char * freecell_solver_str_t;
#endif

DLLEXPORT extern void * freecell_solver_user_alloc(void);

DLLEXPORT extern int freecell_solver_user_apply_preset(
    void * instance,
    const char * preset_name
    );

DLLEXPORT extern void freecell_solver_user_limit_iterations(
    void * user_instance,
    int max_iters
    );

DLLEXPORT extern int freecell_solver_user_set_tests_order(
    void * user_instance,
    const char * tests_order,
    char * * error_string
    );

DLLEXPORT extern int freecell_solver_user_solve_board(
    void * user_instance,
    const char * state_as_string
    );

DLLEXPORT extern int freecell_solver_user_resume_solution(
    void * user_instance
    );

DLLEXPORT extern int freecell_solver_user_get_next_move(
    void * user_instance,
    fcs_move_t * move
    );

DLLEXPORT extern char * freecell_solver_user_current_state_as_string(
    void * user_instance,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    );

DLLEXPORT extern void freecell_solver_user_free(
    void * user_instance
    );

DLLEXPORT extern int freecell_solver_user_get_current_depth(
    void * user_instance
    );

DLLEXPORT extern void freecell_solver_user_set_solving_method(
    void * user_instance,
    int method
    );

DLLEXPORT extern int freecell_solver_user_get_num_times(
    void * user_instance
    );

DLLEXPORT extern int freecell_solver_user_get_limit_iterations(
    void * user_instance
    );

DLLEXPORT extern int freecell_solver_user_get_moves_left(
    void * user_instance
    );

DLLEXPORT extern int freecell_solver_user_set_game(
    void * user_instance,
    int freecells_num,
    int stacks_num,
    int decks_num,
    int sequences_are_built_by,
    int unlimited_sequence_move,
    int empty_stacks_fill
    );

DLLEXPORT extern void freecell_solver_user_set_solution_optimization(
    void * user_instance,
    int optimize
);

DLLEXPORT extern char * freecell_solver_user_move_to_string(
    fcs_move_t move,
    int standard_notation
    );

DLLEXPORT extern char * freecell_solver_user_move_to_string_w_state(
    void * user_instance,
    fcs_move_t move,
    int standard_notation
    );

DLLEXPORT extern void freecell_solver_user_limit_depth(
    void * user_instance,
    int max_depth
    );

DLLEXPORT extern int freecell_solver_user_set_num_freecells(
    void * user_instance,
    int freecells_num
    );

DLLEXPORT extern int freecell_solver_user_get_max_num_freecells(void);

DLLEXPORT extern int freecell_solver_user_set_num_stacks(
    void * user_instance,
    int stacks_num
    );

DLLEXPORT extern int freecell_solver_user_get_max_num_stacks(void);

DLLEXPORT extern int freecell_solver_user_set_num_decks(
    void * user_instance,
    int decks_num
    );

DLLEXPORT extern int freecell_solver_user_get_max_num_decks(void);


DLLEXPORT extern char * freecell_solver_user_get_invalid_state_error_string(
    void * user_instance,
    int print_ts
    );

DLLEXPORT extern int freecell_solver_user_set_sequences_are_built_by_type(
    void * user_instance,
    int sbb
    );

DLLEXPORT extern int freecell_solver_user_set_empty_stacks_filled_by(
    void * user_instance,
    int es_fill
    );

DLLEXPORT extern int freecell_solver_user_set_sequence_move(
    void * user_instance,
    int unlimited_sequence_move
    );

DLLEXPORT extern int freecell_solver_user_set_a_star_weight(
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

DLLEXPORT extern void freecell_solver_user_set_iter_handler(
    void * user_instance,
    freecell_solver_user_iter_handler_t iter_handler,
    void * iter_handler_context
    );


DLLEXPORT extern char * freecell_solver_user_iter_state_as_string(
    void * user_instance,
    void * ptr_state,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    );

DLLEXPORT extern void freecell_solver_user_set_random_seed(
    void * user_instance,
    int seed
    );

DLLEXPORT extern int freecell_solver_user_get_num_states_in_collection(
    void * user_instance
    );

DLLEXPORT extern void freecell_solver_user_limit_num_states_in_collection(
    void * user_instance,
    int max_num_states
    );

DLLEXPORT extern int freecell_solver_user_next_soft_thread(
    void * user_instance
    );

DLLEXPORT extern void freecell_solver_user_set_soft_thread_step(
    void * user_instance,
    int num_times_step
    );

DLLEXPORT extern int freecell_solver_user_next_hard_thread(
    void * user_instance
    );

DLLEXPORT extern int freecell_solver_user_get_num_soft_threads_in_instance(
    void * user_instance
    );

DLLEXPORT extern void freecell_solver_user_set_calc_real_depth(
    void * user_instance,
    int calc_real_depth
    );

DLLEXPORT extern void freecell_solver_user_set_soft_thread_name(
    void * user_instance,
    freecell_solver_str_t name
    );

DLLEXPORT extern void freecell_solver_user_set_flare_name(
    void * user_instance,
    freecell_solver_str_t name
    );

DLLEXPORT extern int freecell_solver_user_set_hard_thread_prelude(
    void * user_instance,
    freecell_solver_str_t prelude
    );

DLLEXPORT extern void freecell_solver_user_recycle(
    void * user_instance
    );

DLLEXPORT extern int freecell_solver_user_set_optimization_scan_tests_order(
    void * user_instance,
    const char * tests_order,
    char * * error_string
    );

DLLEXPORT extern void freecell_solver_user_set_reparent_states(
    void * user_instance,
    int to_reparent_states
    );

DLLEXPORT extern void freecell_solver_user_set_scans_synergy(
    void * user_instance,
    int synergy
    );

DLLEXPORT extern void freecell_solver_user_limit_current_instance_iterations(
    void * user_instance,
    int max_iters
    );

DLLEXPORT extern int freecell_solver_user_next_instance(
    void * user_instance
    );

DLLEXPORT extern int freecell_solver_user_next_flare(
    void * api_instance
    );

/*
 * This function resets the user_instance, making it lose
 * all the previous command line arguments it encountered
 * */
DLLEXPORT extern int freecell_solver_user_reset(
    void * user_instance
    );


DLLEXPORT const char * freecell_solver_user_get_lib_version(
    void * user_instance
    );

DLLEXPORT const char * freecell_solver_user_get_current_soft_thread_name(
    void * user_instance
    );

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__FCS_USER_H */
