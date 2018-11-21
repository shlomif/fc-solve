/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// fcs_user.h - main header file for the Freecell Solver library.
#pragma once
#include <stddef.h>

#include "freecell-solver/fcs_dllexport.h"
#include "freecell-solver/fcs_enums.h"
#include "freecell-solver/fcs_move.h"
#include "freecell-solver/fcs_limit.h"
#include "freecell-solver/fcs_pats_xy_param.h"
#include "freecell-solver/fcs_back_compat.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FC_SOLVE_STRINGS_COMPAT
typedef char *freecell_solver_str_t;
#else
typedef const char *freecell_solver_str_t;
#endif

DLLEXPORT extern void *freecell_solver_user_alloc(void);

DLLEXPORT extern int freecell_solver_user_apply_preset(
    void *instance, const char *preset_name);

DLLEXPORT extern void freecell_solver_user_limit_iterations_long(
    void *user_instance, fcs_int_limit_t max_iters);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern void freecell_solver_user_limit_iterations(
    void *user_instance, int max_iters);

DLLEXPORT extern int freecell_solver_user_set_tests_order(void *user_instance,
    const char *moves_order FCS__PASS_ERR_STR(char **error_string));
#endif

DLLEXPORT extern int freecell_solver_user_solve_board(
    void *user_instance, const char *state_as_string);

DLLEXPORT extern int freecell_solver_user_resume_solution(void *user_instance);

DLLEXPORT extern int freecell_solver_user_get_next_move(
    void *user_instance, fcs_move_t *move);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern char *freecell_solver_user_current_state_as_string(
    void *user_instance
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    ,
    int parseable_output
#endif
    ,
    int canonized_order_output
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    ,
    int display_10_as_t
#endif
);
#endif

DLLEXPORT extern void freecell_solver_user_current_state_stringify(
    void *user_instance, char *const output_string
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    ,
    int parseable_output
#endif
    ,
    int canonized_order_output
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    ,
    int display_10_as_t
#endif
);

DLLEXPORT extern void freecell_solver_user_free(void *user_instance);

DLLEXPORT extern int freecell_solver_user_get_current_depth(
    void *user_instance);

DLLEXPORT extern void freecell_solver_user_set_solving_method(
    void *const user_instance, const int method);

DLLEXPORT extern fcs_int_limit_t freecell_solver_user_get_num_times_long(
    void *user_instance);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern int freecell_solver_user_get_num_times(void *user_instance);

DLLEXPORT extern int freecell_solver_user_get_limit_iterations(
    void *user_instance);
#endif

DLLEXPORT extern int freecell_solver_user_get_moves_left(void *user_instance);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern int freecell_solver_user_set_game(void *user_instance,
    int freecells_num, int stacks_num, int decks_num,
    int sequences_are_built_by, int unlimited_sequence_move,
    int empty_stacks_fill);
#endif

DLLEXPORT extern void freecell_solver_user_set_solution_optimization(
    void *user_instance, int optimize);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern char *freecell_solver_user_move_to_string(
    fcs_move_t move, int standard_notation);

DLLEXPORT extern char *freecell_solver_user_move_to_string_w_state(
    void *user_instance, fcs_move_t move, int standard_notation);
#endif

DLLEXPORT extern void freecell_solver_user_stringify_move_w_state(
    void *user_instance, char *output_string, fcs_move_t move,
    int standard_notation);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern void freecell_solver_user_limit_depth(
    void *user_instance, int max_depth);
#endif

DLLEXPORT extern int freecell_solver_user_set_num_freecells(
    void *user_instance, int freecells_num);

DLLEXPORT extern int freecell_solver_user_get_max_num_freecells(void);

DLLEXPORT extern int freecell_solver_user_set_num_stacks(
    void *user_instance, int stacks_num);

DLLEXPORT extern int freecell_solver_user_get_max_num_stacks(void);

DLLEXPORT extern int freecell_solver_user_set_num_decks(
    void *user_instance, int decks_num);

DLLEXPORT extern int freecell_solver_user_get_max_num_decks(void);

#ifdef FCS_WITH_ERROR_STRS
DLLEXPORT void freecell_solver_user_get_invalid_state_error_into_string(
    void *const api_instance, char *const string
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    ,
    const int print_ts
#endif
);
#endif

#ifdef FCS_WITH_ERROR_STRS
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern char *freecell_solver_user_get_invalid_state_error_string(
    void *user_instance
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    ,
    int print_ts
#endif
);
#endif
#endif

DLLEXPORT extern int freecell_solver_user_set_sequences_are_built_by_type(
    void *user_instance, int sequences_are_built_by);

DLLEXPORT extern int freecell_solver_user_set_empty_stacks_filled_by(
    void *user_instance, int es_fill);

DLLEXPORT extern int freecell_solver_user_set_sequence_move(
    void *user_instance, int unlimited_sequence_move);

DLLEXPORT extern int freecell_solver_user_set_a_star_weight(
    void *const user_instance, const int my_index, const double weight);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
typedef void (*freecell_solver_user_iter_handler_t)(void *user_instance,
    int iter_num, int depth, void *ptr_state, int parent_iter_num,
    void *context);
#endif

typedef void (*freecell_solver_user_long_iter_handler_t)(void *user_instance,
    fcs_int_limit_t iter_num, int depth, void *ptr_state,
    fcs_int_limit_t parent_iter_num, void *context);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern void freecell_solver_user_set_iter_handler(void *user_instance,
    freecell_solver_user_iter_handler_t iter_handler,
    void *iter_handler_context);
#endif

DLLEXPORT extern void freecell_solver_user_set_iter_handler_long(
    void *user_instance, freecell_solver_user_long_iter_handler_t iter_handler,
    void *iter_handler_context);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern char *freecell_solver_user_iter_state_as_string(
    void *const user_instance, void *const ptr_state
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    ,
    const int parseable_output
#endif
    ,
    const int canonized_order_output
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    ,
    const int display_10_as_t
#endif
);
#endif

DLLEXPORT extern void freecell_solver_user_iter_state_stringify(
    void *const user_instance, char *output_string, void *const ptr_state
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    ,
    const int parseable_output
#endif
    ,
    const int canonized_order_output
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    ,
    const int display_10_as_t
#endif
);

DLLEXPORT extern void freecell_solver_user_set_random_seed(
    void *const user_instance, const int seed);

#ifndef FCS_DISABLE_NUM_STORED_STATES
DLLEXPORT fcs_int_limit_t
freecell_solver_user_get_num_states_in_collection_long(
    void *const user_instance);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern int freecell_solver_user_get_num_states_in_collection(
    void *const user_instance);
#endif

DLLEXPORT extern void freecell_solver_user_limit_num_states_in_collection_long(
    void *user_instance, fcs_int_limit_t max_num_states);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern void freecell_solver_user_limit_num_states_in_collection(
    void *user_instance, int max_num_states);
#endif

DLLEXPORT extern void freecell_solver_set_stored_states_trimming_limit(
    void *user_instance, long max_num_states);
#endif

DLLEXPORT extern int freecell_solver_user_next_soft_thread(void *user_instance);

DLLEXPORT extern void freecell_solver_user_set_soft_thread_step(
    void *const user_instance, const int checked_states_step);

DLLEXPORT extern int freecell_solver_user_next_hard_thread(
    void *const user_instance);

DLLEXPORT extern int freecell_solver_user_get_num_soft_threads_in_instance(
    void *const user_instance);

DLLEXPORT extern void freecell_solver_user_set_calc_real_depth(
    void *const user_instance, const int calc_real_depth);

DLLEXPORT extern void freecell_solver_user_set_soft_thread_name(
    void *const user_instance, freecell_solver_str_t name);

DLLEXPORT extern void freecell_solver_user_set_flare_name(
    void *const user_instance, freecell_solver_str_t name);

DLLEXPORT extern int freecell_solver_user_set_hard_thread_prelude(
    void *const user_instance, freecell_solver_str_t prelude);

DLLEXPORT extern int freecell_solver_user_set_flares_plan(
    void *const api_instance, freecell_solver_str_t plan);

DLLEXPORT extern void freecell_solver_user_recycle(void *const user_instance);

DLLEXPORT extern int freecell_solver_user_set_optimization_scan_tests_order(
    void *const user_instance,
    const char *const moves_order FCS__PASS_ERR_STR(char **const error_string));

DLLEXPORT extern void freecell_solver_user_set_reparent_states(
    void *const user_instance, const int to_reparent_states);

DLLEXPORT extern void freecell_solver_user_set_scans_synergy(
    void *const user_instance, const int synergy);

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern void freecell_solver_user_limit_current_instance_iterations(
    void *user_instance, int max_iters);
#endif

DLLEXPORT extern int freecell_solver_user_next_instance(
    void *const user_instance);

DLLEXPORT extern int freecell_solver_user_next_flare(void *const api_instance);

/*
 * This function resets the user_instance, making it lose
 * all the previous command line arguments it encountered
 * */
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT extern int freecell_solver_user_reset(void *const user_instance);

DLLEXPORT const char *freecell_solver_user_get_lib_version(
    void *const user_instance);
#endif

DLLEXPORT const char *freecell_solver_user_get_current_soft_thread_name(
    void *const user_instance);

/*
 * This error string should be strdup()'ed or something if one wishes
 * to reuse it. Otherwise, it is constant.
 * */
#ifdef FCS_WITH_ERROR_STRS
DLLEXPORT const char *freecell_solver_user_get_last_error_string(
    void *const api_instance);
#endif

/*
 * This sets the moves_order from min_depth onwards.
 * */
DLLEXPORT extern int freecell_solver_user_set_depth_tests_order(
    void *const user_instance, const int min_depth,
    const char *const moves_order FCS__PASS_ERR_STR(char **const error_string));

#ifndef FCS_ENABLE_PRUNE__R_TF__UNCOND
DLLEXPORT extern int freecell_solver_user_set_pruning(void *const user_instance,
    const char *const pruning FCS__PASS_ERR_STR(char **const error_string));
#endif

DLLEXPORT extern int freecell_solver_user_set_cache_limit(
    void *const user_instance, long limit);

DLLEXPORT extern int freecell_solver_user_get_moves_sequence(
    void *const user_instance, fcs_moves_sequence_t *const moves_seq);

DLLEXPORT extern int freecell_solver_user_set_flares_choice(
    void *const user_instance, const char *const new_flares_choice_string);

DLLEXPORT extern void freecell_solver_user_set_flares_iters_factor(
    void *const user_instance, const double new_factor);

DLLEXPORT extern int freecell_solver_user_set_patsolve_x_param(
    void *const api_instance, const int position,
    const int x_param_val FCS__PASS_ERR_STR(char **const error_string));

DLLEXPORT extern int freecell_solver_user_set_patsolve_y_param(
    void *const api_instance, const int position,
    const double y_param_val FCS__PASS_ERR_STR(char **const error_string));

typedef struct
{
    size_t scan_idx;
    size_t quota;
} fc_solve_prelude_item;

DLLEXPORT extern void fc_solve_user_set_ht_compiled_prelude(
    void *, size_t, const fc_solve_prelude_item *);

#ifdef __cplusplus
}
#endif
