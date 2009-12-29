/* Copyright (c) 2009 Shlomi Fish
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
 * simpsim.h - header file of the Simple Simon move functions
 * ("solve-for-state"/"sfs").
 *
 */

#ifndef FC_SOLVE__SIMPSIM_H
#define FC_SOLVE__SIMPSIM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
/* For fcs_state_extra_info_t */
#include "state.h"
/* For fc_solve_soft_thread_t and fcs_derived_states_list_t */
#include "instance.h"

#ifndef FCS_DISABLE_SIMPLE_SIMON
extern int fc_solve_sfs_simple_simon_move_sequence_to_founds(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        );

extern int fc_solve_sfs_simple_simon_move_sequence_to_true_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        );

extern int fc_solve_sfs_simple_simon_move_whole_stack_sequence_to_false_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        );

extern int fc_solve_sfs_simple_simon_move_sequence_to_true_parent_with_some_cards_above(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        );

extern int fc_solve_sfs_simple_simon_move_sequence_with_some_cards_above_to_true_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        );

extern int fc_solve_sfs_simple_simon_move_sequence_with_junk_seq_above_to_true_parent_with_some_cards_above(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        );

extern int fc_solve_sfs_simple_simon_move_whole_stack_sequence_to_false_parent_with_some_cards_above(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        );

extern int fc_solve_sfs_simple_simon_move_sequence_to_parent_on_the_same_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        );

extern int fc_solve_sfs_simple_simon_move_sequence_to_false_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        );

#endif /* #ifndef FCS_DISABLE_SIMPLE_SIMON */

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__SIMPSIM_H */

