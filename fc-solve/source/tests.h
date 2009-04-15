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
 * tests.h - header file of the test functions for Freecell Solver.
 *
 * The test functions code is found in freecell.c
 *
 */

#ifndef FC_SOLVE__TESTS_H
#define FC_SOLVE__TESTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "fcs_isa.h"
#include "fcs.h"

#include "test_arr.h"


#ifdef FCS_FREECELL_ONLY
#define calc_max_sequence_move(fc_num, fs_num) (((fc_num)+1)<<(fs_num))
#else
/*
 * The number of cards that can be moved is
 * (freecells_number + 1) * 2 ^ (free_stacks_number)
 *
 * See the Freecell FAQ and the source code of PySol
 *
 * */
#define calc_max_sequence_move(fc_num, fs_num)                    \
    ((instance->unlimited_sequence_move) ?                         \
            INT_MAX :                                             \
    ((instance->empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD) ? \
                (((fc_num)+1)<<(fs_num))                        : \
        ((fc_num)+1)                                              \
    ))
#endif

#include "caas.h"

/*
 *  These are some macros to make it easier for the programmer.
 * */
#define ptr_state_key (ptr_state_key)
#define ptr_state_val (ptr_state_val)
#define state_key (*ptr_state_key)
#define state  (state_key)
#define state_val (*ptr_state_val)
#define new_state (*ptr_new_state_key)
#define new_state_key (new_state)
#define new_state_val (*ptr_new_state_val)

extern int fc_solve_sfs_check_state_begin(
    fc_solve_hard_thread_t * hard_thread,
    fcs_state_t * * out_ptr_state_key,
    fcs_state_extra_info_t * * out_ptr_new_state_val,
    fcs_state_extra_info_t * ptr_state_val,
    fcs_move_stack_t * moves
    );

#define sfs_check_state_begin()                                     \
    {         \
        fc_solve_sfs_check_state_begin(hard_thread,  \
                &ptr_new_state_key,                  \
                &ptr_new_state_val,                  \
                ptr_state_val,                       \
                moves);                              \
    }

int fc_solve_sfs_check_state_end(
    fc_solve_soft_thread_t * soft_thread,
    fcs_state_extra_info_t * ptr_state_val,
    fcs_state_extra_info_t * ptr_new_state_val,
    fcs_move_stack_t * moves,
    fcs_derived_states_list_t * derived_states_list
    );

#define sfs_check_state_end()                                             \
    { \
        check = fc_solve_sfs_check_state_end(soft_thread, ptr_state_val, ptr_new_state_val, moves, derived_states_list);                         \
        if ((check == FCS_STATE_BEGIN_SUSPEND_PROCESS) ||                   \
            (check == FCS_STATE_SUSPEND_PROCESS))                            \
        {         \
            return check;         \
        }         \
    }

/*
    This macro checks if the top card in the stack is a flipped card
    , and if so flips it so its face is up.
  */
#define fcs_flip_top_card(stack)                                   \
{                                                                  \
    int cards_num;                                                 \
    cards_num = fcs_stack_len(new_state,stack);                    \
                                                                   \
    if (cards_num > 0)                                             \
    {                                                              \
        if (fcs_card_get_flipped(                                  \
                fcs_stack_card(                                    \
                    new_state,                                     \
                    stack,                                         \
                    cards_num-1)                                   \
                ) == 1                                             \
           )                                                       \
        {                                                          \
            fcs_flip_stack_card(new_state,stack,cards_num-1);      \
            fcs_move_set_type(temp_move, FCS_MOVE_TYPE_FLIP_CARD); \
            fcs_move_set_src_stack(temp_move, stack);              \
                                                                   \
            fcs_move_stack_push(moves, temp_move);                 \
        }                                                          \
    }                                                              \
}


/*
 * dest is the destination stack
 * source is the source stack
 * start is the start height
 * end is the end height
 * a is the iterator
 * */
#define fcs_move_sequence(dest, source, start, end, a)              \
{                                                                   \
    for ( a = (start) ; a <= (end) ; a++)                           \
    {                                                               \
        fcs_push_stack_card_into_stack(new_state, dest, source, a); \
    }                                                               \
                                                                    \
    for ( a = (start) ; a <= (end) ; a++)                           \
    {                                                               \
        fcs_pop_stack_card(new_state, source, temp_card);           \
    }                                                               \
                                                                    \
    fcs_move_set_type(temp_move, FCS_MOVE_TYPE_STACK_TO_STACK);     \
    fcs_move_set_src_stack(temp_move, source);                      \
    fcs_move_set_dest_stack(temp_move, dest);                       \
    fcs_move_set_num_cards_in_seq(temp_move, (end)-(start)+1);      \
                                                                    \
    fcs_move_stack_push(moves, temp_move);                          \
}

#ifdef FCS_FREECELL_ONLY
#define tests_declare_accessors_freecell_only()
#else
#define tests_declare_accessors_freecell_only() \
    int sequences_are_built_by;  \
    int empty_stacks_fill;
#endif

/*
 * This test declares a few access variables that are used in all
 * the tests.
 * */
#define tests_declare_accessors()                              \
    fc_solve_hard_thread_t * hard_thread;               \
    fc_solve_instance_t * instance;                     \
    fcs_state_t * ptr_state_key; \
    fcs_state_t * ptr_new_state_key; \
    fcs_state_extra_info_t * ptr_new_state_val; \
    fcs_move_stack_t * moves;                                  \
    char * indirect_stacks_buffer;                             \
    tests_declare_accessors_freecell_only()

#ifdef FCS_FREECELL_ONLY

#define tests_define_accessors_freecell_only() {}

#define tests__is_filled_by_any_card() 1

#define tests__is_filled_by_kings_only() 0

#define tests__is_filled_by_none() 0

#else

#define tests_define_accessors_freecell_only() \
{ \
    sequences_are_built_by = instance->sequences_are_built_by; \
    empty_stacks_fill = instance->empty_stacks_fill;           \
}

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
#define tests_define_accessors()                                  \
    ptr_state_key = ptr_state_val->key;                           \
    hard_thread = soft_thread->hard_thread;                       \
    instance = hard_thread->instance;                             \
    moves = hard_thread->reusable_move_stack;                     \
    indirect_stacks_buffer = hard_thread->indirect_stacks_buffer; \
    tests_define_accessors_freecell_only()

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

#ifdef __cplusplus
}
#endif

#define my_copy_stack(idx) fcs_copy_stack(new_state_key, new_state_val, idx, indirect_stacks_buffer);

#endif /* FC_SOLVE__TESTS_H */
