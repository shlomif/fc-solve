/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * meta_move_funcs_helpers.h - header file of the move functions of Freecell
 * Solver.
 *
 * The move functions code itself is found in freecell.c and simpsim.c.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "instance.h"

static inline int max0(const int e) { return max(e, 0); }
#ifdef FCS_FREECELL_ONLY
#define calc_max_sequence_move(num_freecells, num_empty_cols)                  \
    (((num_freecells) + 1) << (num_empty_cols))
#else
/*
 * The number of cards that can be moved is
 * (freecells_number + 1) * 2 ^ (free_stacks_number)
 *
 * See the Freecell FAQ and the source code of PySol
 *
 * TODO : place INSTANCE_GAME_FLAGS inside a local variable.
 * */
#define calc_max_sequence_move(num_freecells, num_empty_cols)                  \
    (INSTANCE_UNLIMITED_SEQUENCE_MOVE                                          \
            ? INT_MAX                                                          \
            : ((empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)                \
                      ? (((num_freecells) + 1) << (num_empty_cols))            \
                      : ((num_freecells) + 1)))

static inline int calc_max_simple_simon_seq_move(const int num_empty_cols)
{
    return ((num_empty_cols < 0) ? 0 : (1 << num_empty_cols));
}
#endif

// These are some macros to make it easier for the programmer.
#define ptr_state_key (raw_ptr_state_raw->key)
#define val_ptr_state_val (raw_ptr_state_raw->val)

#define ptr_new_state &(pass_new_state)
#define state_key (*ptr_state_key)
#define state_val (*val_ptr_state_val)
#define new_state_key (*(pass_new_state.key))
#define state (state_key)

#define sfs_check_state_begin()                                                \
    {                                                                          \
        fc_solve_sfs_check_state_begin(hard_thread, &pass_new_state,           \
            raw_ptr_state_raw SFS__PASS_MOVE_STACK(moves));                    \
    }

#define sfs_check_state_end()                                                  \
    fc_solve_sfs_check_state_end(soft_thread, raw_ptr_state_raw,               \
        &pass_new_state, state_context_value FCS__pass_moves(moves),           \
        derived_states_list)

/*
    This macro checks if the top card in the stack is a flipped card
    , and if so flips it so its face is up.
  */

#define STATE_KEY() (*(new_state_kv_ptr->key))
static inline void fc_solve_move_sequence_function(
    fcs_kv_state_t *const new_state_kv_ptr FCS__pass_moves(
        fcs_move_stack_t *const moves),
    const size_t dest_col_i, const size_t src_col_i, const size_t cards_num)
{
    fcs_col_transfer_cards(fcs_state_get_col(STATE_KEY(), dest_col_i),
        fcs_state_get_col(STATE_KEY(), src_col_i), cards_num);
    fcs_move_stack_params_push(
        moves, FCS_MOVE_TYPE_STACK_TO_STACK, src_col_i, dest_col_i, cards_num);
}
#undef STATE_KEY

#define fcs_move_sequence(to, from, cards_num)                                 \
    fc_solve_move_sequence_function(                                           \
        &pass_new_state FCS__pass_moves(moves), to, from, cards_num)

#ifdef FCS_RCS_STATES

#define tests_define_accessors_rcs_states()                                    \
    fcs_state_t my_new_out_state_key;                                          \
    {                                                                          \
        pass_new_state.key = &my_new_out_state_key;                            \
    }

#else
#define tests_define_accessors_rcs_states()                                    \
    {                                                                          \
    }
#endif

#ifdef FCS_FREECELL_ONLY

#define tests_define_accessors_freecell_only()                                 \
    {                                                                          \
    }

#define tests__is_filled_by_any_card() TRUE

#define IS_FILLED_BY_KINGS_ONLY() FALSE

#define IS_FILLED_BY_NONE() FALSE

#else

#ifdef FCS_SINGLE_HARD_THREAD
#define tests_define_accessors_freecell_only()                                 \
    fc_solve_instance_t *const instance = hard_thread;
#else
#define tests_define_accessors_freecell_only()                                 \
    fc_solve_instance_t *const instance = hard_thread->instance;
#endif

#define tests__is_filled_by_any_card()                                         \
    (empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)

#define IS_FILLED_BY_KINGS_ONLY()                                              \
    (empty_stacks_fill == FCS_ES_FILLED_BY_KINGS_ONLY)

#define IS_FILLED_BY_NONE() (empty_stacks_fill == FCS_ES_FILLED_BY_NONE)

#endif

#ifdef FCS_WITH_MOVES
#define tests_define_accessors_move_stack()                                    \
    fcs_move_stack_t *const moves =                                            \
        &(HT_FIELD(hard_thread, reusable_move_stack))
#else
#define tests_define_accessors_move_stack()
#endif

/*
 * This macro defines these accessors to have some value.
 * */
#define tests_define_accessors_no_stacks()                                     \
    fc_solve_hard_thread_t *const hard_thread = soft_thread->hard_thread;      \
    tests_define_accessors_move_stack();                                       \
    int state_context_value = 0;                                               \
    fcs_kv_state_t pass_new_state;                                             \
    tests_define_accessors_freecell_only();                                    \
    tests_define_accessors_rcs_states();

#ifdef INDIRECT_STACK_STATES
#define tests_define_indirect_stack_states_accessors()                         \
    char *indirect_stacks_buffer =                                             \
        HT_FIELD(hard_thread, indirect_stacks_buffer);
#else
#define tests_define_indirect_stack_states_accessors()
#endif

#define tests_define_accessors()                                               \
    tests_define_accessors_no_stacks();                                        \
    tests_define_indirect_stack_states_accessors();

#define my_copy_stack(idx)                                                     \
    fcs_copy_stack(                                                            \
        new_state_key, *(pass_new_state.val), idx, indirect_stacks_buffer);

/*
 * This macro assists in implementing this prune:
 *
 * http://tech.groups.yahoo.com/group/fc-solve-discuss/message/1121 :
 *
 * There is no point in moving the last card in a
 * column to a parent on a different column, because then the column won't
 * be able to be filled, and will be left to disuse. Furthermore, after
 * moved to a parent, the card might block other cards that can be placed
 * on the parent.
 *
 * TODO : implement it for FCS_ES_FILLED_BY_KINGS_ONLY
 * */
#define MOVE_FUNCS__should_not_empty_columns() IS_FILLED_BY_NONE()

#ifdef __cplusplus
}
#endif
