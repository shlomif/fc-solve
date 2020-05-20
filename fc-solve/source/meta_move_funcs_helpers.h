// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
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

static inline size_t calc_max_simple_simon_seq_move(const int num_empty_cols)
{
    return ((num_empty_cols < 0) ? 0 : (1 << num_empty_cols));
}
#endif

// These are some macros to make it easier for the programmer.
#define ptr_state_key (raw_state_raw.key)
#define ptr_new_state &(pass_new_state)
#define state_key (*ptr_state_key)
#define new_state_key (*(pass_new_state.key))

#define sfs_check_state_begin()                                                \
    fc_solve_sfs_check_state_begin(hard_thread, &pass_new_state,               \
        raw_state_raw SFS__PASS_MOVE_STACK(moves))

#ifdef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
#define sfs_check_state_end__arg
#else
#define sfs_check_state_end__arg raw_state_raw,
#endif

#define sfs_check_state_end()                                                  \
    fc_solve_derived_states_list_add_state(derived_states_list,                \
        fc_solve_sfs_check_state_end(soft_thread,                              \
            sfs_check_state_end__arg &pass_new_state FCS__pass_moves(moves)),  \
        state_context_value)

static inline void fc_solve_move_sequence_function(
    fcs_state *const s FCS__pass_moves(fcs_move_stack *const moves),
    const size_t dest_col_i, const size_t src_col_i, const size_t cards_num)
{
    fcs_col_transfer_cards(fcs_state_get_col(*s, dest_col_i),
        fcs_state_get_col(*s, src_col_i), cards_num);
    fcs_move_stack_params_push(
        moves, FCS_MOVE_TYPE_STACK_TO_STACK, src_col_i, dest_col_i, cards_num);
}

#define fcs_move_sequence(to, from, cards_num)                                 \
    fc_solve_move_sequence_function(                                           \
        &(new_state_key)FCS__pass_moves(moves), to, from, cards_num)

#ifdef FCS_RCS_STATES

#define tests_define_accessors_rcs_states()                                    \
    fcs_state my_new_out_state_key;                                            \
    pass_new_state.key = &my_new_out_state_key

#else
#define tests_define_accessors_rcs_states()
#endif

#ifdef FCS_FREECELL_ONLY

#define tests_define_accessors_freecell_only()                                 \
    {                                                                          \
    }

#define tests__is_filled_by_any_card() true
#define IS_FILLED_BY_KINGS_ONLY() false
#define IS_FILLED_BY_NONE() false

#else

#ifdef FCS_SINGLE_HARD_THREAD
#define tests_define_accessors_freecell_only()                                 \
    fcs_instance *const instance = hard_thread;
#else
#define tests_define_accessors_freecell_only()                                 \
    fcs_instance *const instance = hard_thread->instance;
#endif

#define tests__is_filled_by_any_card()                                         \
    (empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)

#define IS_FILLED_BY_KINGS_ONLY()                                              \
    (empty_stacks_fill == FCS_ES_FILLED_BY_KINGS_ONLY)

#define IS_FILLED_BY_NONE() (empty_stacks_fill == FCS_ES_FILLED_BY_NONE)

#endif

#ifdef FCS_WITH_MOVES
#define tests_define_accessors_move_stack()                                    \
    fcs_move_stack *const moves = &(HT_FIELD(hard_thread, reusable_move_stack))
#else
#define tests_define_accessors_move_stack()
#endif

#define tests_state_context_val int state_context_value = 0;
/*
 * This macro defines these accessors to have some value.
 * */
#define tests_define_accessors_no_stacks(MORE)                                 \
    fcs_hard_thread *const hard_thread = soft_thread->hard_thread;             \
    tests_define_accessors_move_stack();                                       \
    MORE fcs_kv_state pass_new_state;                                          \
    tests_define_accessors_freecell_only();                                    \
    tests_define_accessors_rcs_states()

#ifdef INDIRECT_STACK_STATES
#define tests_define_indirect_stack_states_accessors()                         \
    fcs_card *indirect_stacks_buffer =                                         \
        HT_FIELD(hard_thread, indirect_stacks_buffer);
#else
#define tests_define_indirect_stack_states_accessors()
#endif

#define tests_define_accessors__generic(MORE)                                  \
    tests_define_accessors_no_stacks(MORE);                                    \
    tests_define_indirect_stack_states_accessors()

#define tests_define_accessors()                                               \
    tests_define_accessors__generic(tests_state_context_val)
#define my_copy_stack(idx)                                                     \
    fcs_copy_stack(                                                            \
        new_state_key, *(pass_new_state.val), idx, indirect_stacks_buffer)
#define copy_two_stacks(idx1, idx2)                                            \
    my_copy_stack(idx1);                                                       \
    my_copy_stack(idx2)

/*
 * This macro assists in implementing this prune:
 *
 * https://groups.yahoo.com/neo/groups/fc-solve-discuss/conversations/topics/1121
 * :
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

#define MOVE_FUNCS__define_common()                                            \
    tests_define_accessors();                                                  \
    MOVE_FUNCS__define_seqs_built_by();                                        \
    MOVE_FUNCS__define_empty_stacks_fill()

#ifdef FCS_FREECELL_ONLY

#define MOVE_FUNCS__define_seqs_built_by()
#define MOVE_FUNCS__define_empty_stacks_fill()
#define PASS_sequences_are_built_by(param)
#include "pos_by_rank__freecell.h"
#define POS_BY_RANK_STEP (2)
#define FCS_POS_IDX_TO_CHECK_START_LOOP(src_card)                              \
    const_AUTO(pos_pos, pos_by_rank__freecell[(int)src_card]);                 \
    const int8_t *pos_idx_to_check = &positions_by_rank[pos_pos.start];        \
    const int8_t *const last_pos_idx = &positions_by_rank[pos_pos.end];        \
                                                                               \
    for (; pos_idx_to_check < last_pos_idx;                                    \
         pos_idx_to_check += suit_positions_by_rank_step)

#else

#define MOVE_FUNCS__define_seqs_built_by()                                     \
    const int sequences_are_built_by =                                         \
        GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance);
#define MOVE_FUNCS__define_empty_stacks_fill()                                 \
    const int empty_stacks_fill = INSTANCE_EMPTY_STACKS_FILL;
#define PASS_sequences_are_built_by(param) , param
#define POS_BY_RANK_STEP                                                       \
    ((sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK)                         \
            ? 1                                                                \
            : (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT) ? 4 : 2)
#define FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(card)             \
    ((sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK)                         \
            ? 0                                                                \
            : (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT)                \
                  ? fcs_card_suit(card)                                        \
                  : ((fcs_card_suit(card) ^ 0x1) & (0x2 - 1)))

#define FCS_POS_IDX_TO_CHECK_START_LOOP(src_card)                              \
    const int8_t *pos_idx_to_check = &positions_by_rank[(                      \
        FCS_POS_BY_RANK_WIDTH * (fcs_card_rank(src_card)))];                   \
    const int8_t *const last_pos_idx =                                         \
        pos_idx_to_check + FCS_POS_BY_RANK_WIDTH;                              \
                                                                               \
    for (pos_idx_to_check += POS_BY_RANK_MAP(                                  \
             FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(src_card));  \
         pos_idx_to_check < last_pos_idx;                                      \
         pos_idx_to_check += suit_positions_by_rank_step)

#endif

#define SET_empty_stack_idx(empty_stack_idx)                                   \
    stack_i empty_stack_idx;                                                   \
    for (empty_stack_idx = 0; empty_stack_idx < LOCAL_STACKS_NUM;              \
         empty_stack_idx++)                                                    \
    {                                                                          \
        if (fcs_state_col_is_empty(state_key, empty_stack_idx))                \
        {                                                                      \
            break;                                                             \
        }                                                                      \
    }

#define CALC_num_cards_in_col_threshold()                                      \
    (MOVE_FUNCS__should_not_empty_columns() ? 1 : 0)

#ifdef __cplusplus
}
#endif
