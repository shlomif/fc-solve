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
 * meta_move_funcs_helpers.h - header file of the move functions of Freecell
 * Solver.
 *
 * The move functions code itself is found in freecell.c and simpsim.c.
 *
 */

#ifndef FC_SOLVE__META_MOVE_FUNCS_HELPERS_H
#define FC_SOLVE__META_MOVE_FUNCS_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "inline.h"
#include "bool.h"

#include "instance.h"

#ifdef FCS_FREECELL_ONLY
#define calc_max_sequence_move(fc_num, fs_num) (((fc_num)+1)<<(fs_num))
#else
/*
 * The number of cards that can be moved is
 * (freecells_number + 1) * 2 ^ (free_stacks_number)
 *
 * See the Freecell FAQ and the source code of PySol
 *
 * TODO : place INSTANCE_GAME_FLAGS inside a local variable.
 * */
#define calc_max_sequence_move(fc_num, fs_num)                    \
    (INSTANCE_UNLIMITED_SEQUENCE_MOVE ?                         \
            INT_MAX :                                             \
    ((empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD) ? \
                (((fc_num)+1)<<(fs_num))                        : \
        ((fc_num)+1)                                              \
    ))

#define calc_max_simple_simon_seq_move(fs_num) (1 << (fs_num))
#endif

/*
 *  These are some macros to make it easier for the programmer.
 * */
#define key_ptr_state_key (raw_ptr_state_raw->key)
#define val_ptr_state_val (raw_ptr_state_raw->val)

#define ptr_new_state &(pass_new_state)
#define ptr_state_key (key_ptr_state_key)
#define ptr_state_val (val_ptr_state_val)
#define state_key (*key_ptr_state_key)
#define state_val (*val_ptr_state_val)
#define new_state_key (*(pass_new_state.key))
#define new_state new_state_key
#define new_state_val (*(pass_new_state.val))
#define NEW_STATE_BY_REF() (&pass_new_state)
#define state  (state_key)

#define sfs_check_state_begin()                                     \
    {         \
        fc_solve_sfs_check_state_begin(hard_thread,  \
                &pass_new_state,                      \
                raw_ptr_state_raw,                   \
                moves);                              \
    }

#define sfs_check_state_end() \
    { \
        fc_solve_sfs_check_state_end(soft_thread, raw_ptr_state_raw, &pass_new_state, state_context_value, moves, derived_states_list); \
    }

/*
    This macro checks if the top card in the stack is a flipped card
    , and if so flips it so its face is up.
  */
#ifndef FCS_WITHOUT_CARD_FLIPPING
#define fcs_flip_top_card(stack_idx)                                   \
{                                                                  \
    fcs_cards_column_t flip_top_card_col;                                        \
    flip_top_card_col = fcs_state_get_col(new_state, stack_idx);                 \
                                                                   \
    if (fcs_col_len(flip_top_card_col) > 0)                                             \
    {                                                              \
        if (fcs_card_get_flipped(                                  \
                fcs_col_get_card(flip_top_card_col, fcs_col_len(flip_top_card_col)-1)        \
                ) == 1                                             \
           )                                                       \
        {                                                          \
            fcs_col_flip_card(flip_top_card_col, fcs_col_len(flip_top_card_col)-1);                   \
            fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_FLIP_CARD); \
            fcs_int_move_set_src_stack(temp_move, stack_idx);              \
                                                                   \
            fcs_move_stack_push(moves, temp_move);                 \
        }                                                          \
    }                                                              \
}
#else
#define fcs_flip_top_card(stack_idx) {}
#endif

static GCC_INLINE void fc_solve_move_sequence_function(
#define STATE_KEY() (*(new_state_kv_ptr->key))
        fcs_kv_state_t * const new_state_kv_ptr,
        fcs_move_stack_t * const moves,
        const int dest_idx,
        const int source_idx,
        const int start,
        const int end
        )
{
    fcs_cards_column_t new_src_col = fcs_state_get_col(STATE_KEY(), source_idx);
    fcs_cards_column_t new_dest_col = fcs_state_get_col(STATE_KEY(), dest_idx);

    for ( int i = start ; i <= end ; i++)
    {
        fcs_col_push_col_card(new_dest_col, new_src_col, i);
    }

    for ( int i = start ; i <= end ; i++)
    {
        fcs_col_pop_top(new_src_col);
    }

    fcs_internal_move_t temp_move;
    fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_STACK_TO_STACK);
    fcs_int_move_set_src_stack(temp_move, source_idx);
    fcs_int_move_set_dest_stack(temp_move, dest_idx);
    fcs_int_move_set_num_cards_in_seq(temp_move, end-start+1);

    fcs_move_stack_push(moves, temp_move);
}
#undef STATE_KEY

/*
 * dest is the destination stack
 * source is the source stack
 * start is the start height
 * end is the end height
 * a is the iterator
 * */
#define fcs_move_sequence(dest_idx, source_idx, start_idx, end_idx) \
    {   \
        fc_solve_move_sequence_function(&pass_new_state, moves, dest_idx, source_idx, start_idx, end_idx); \
    }

#ifdef FCS_RCS_STATES

#define tests_define_accessors_rcs_states()     \
    fcs_state_t my_new_out_state_key;           \
{                                               \
    pass_new_state.key = &my_new_out_state_key; \
}

#else
#define tests_define_accessors_rcs_states() {}
#endif

#ifdef FCS_FREECELL_ONLY

#define tests_define_accessors_freecell_only() {}

#define tests__is_filled_by_any_card() TRUE

#define tests__is_filled_by_kings_only() FALSE

#define tests__is_filled_by_none() FALSE

#else

#define tests_define_accessors_freecell_only() \
    fc_solve_instance_t * const instance = hard_thread->instance;

#define tests__is_filled_by_any_card() \
    (empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)

#define tests__is_filled_by_kings_only() \
    (empty_stacks_fill == FCS_ES_FILLED_BY_KINGS_ONLY)

#define tests__is_filled_by_none() \
    (empty_stacks_fill == FCS_ES_FILLED_BY_NONE)

#endif

/*
 * This macro defines these accessors to have some value.
 * */
#define tests_define_accessors_no_stacks()                                  \
    fc_solve_hard_thread_t * const hard_thread = soft_thread->hard_thread;        \
    fcs_move_stack_t * const moves = &(hard_thread->reusable_move_stack);         \
    int state_context_value = 0;                                            \
    fcs_kv_state_t pass_new_state;                                          \
    tests_define_accessors_freecell_only();   \
    tests_define_accessors_rcs_states();

#ifdef INDIRECT_STACK_STATES
#define tests_define_indirect_stack_states_accessors() \
    char * indirect_stacks_buffer = hard_thread->indirect_stacks_buffer;
#else
#define tests_define_indirect_stack_states_accessors()
#endif

#define tests_define_accessors()  \
    tests_define_accessors_no_stacks();       \
    tests_define_indirect_stack_states_accessors();

#ifdef FCS_FREECELL_ONLY

#define tests_define_seqs_built_by()
#define tests_define_empty_stacks_fill()
#else

#define tests_define_seqs_built_by()   \
    const int sequences_are_built_by = GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance);

#define tests_define_empty_stacks_fill() \
    const int empty_stacks_fill = INSTANCE_EMPTY_STACKS_FILL;

#endif

#define my_copy_stack(idx) fcs_copy_stack(new_state_key, new_state_val, idx, indirect_stacks_buffer);

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
#define tests__should_not_empty_columns() tests__is_filled_by_none()

#define SET_VACANT_PTR() \
    const fcs_vacant_state_resources_info_t * const \
        vacant_state_resources_ptr = \
            soft_thread->vacant_state_resources_ptr;

/* Requires adding a SET_VACANT_PTR() before it. */
#define SET_VACANT_FREECELLS() \
    const fcs_game_limit_t num_vacant_freecells = \
        vacant_state_resources_ptr->num_vacant_freecells;

/* Requires adding a SET_VACANT_PTR() before it. */
#define SET_VACANT_STACKS() \
    const fcs_game_limit_t num_vacant_stacks = \
        vacant_state_resources_ptr->num_vacant_stacks;

#define SET_VACANT_FREECELLS_AND_STACKS() \
    SET_VACANT_FREECELLS(); \
    SET_VACANT_STACKS();

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__META_MOVE_FUNCS_HELPERS_H */
