/*
 * fcs.h - header file of the test functions for Freecell Solver.
 *
 * The test functions code is found in freecell.c
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#ifndef __TESTS_H
#define __TESTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "fcs_isa.h"
#include "fcs.h"

#include "test_arr.h"

/*
 * The number of cards that can be moved is
 * (freecells_number + 1) * 2 ^ (free_stacks_number)
 *
 * See the Freecell FAQ and the source code of PySol
 *
 * */
#define calc_max_sequence_move(fc_num, fs_num)                    \
    ((instance->empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD) ? \
        (                                                         \
            (instance->unlimited_sequence_move) ?                 \
                INT_MAX :                                         \
                (((fc_num)+1)<<(fs_num))                          \
        ) :                                                       \
        ((fc_num)+1)                                              \
    )

#include "caas.h"

/*
 *  Those are some macros to make it easier for the programmer.
 * */
#define state_with_locations (*ptr_state_with_locations)
#define state (ptr_state_with_locations->s)
#define new_state_with_locations (*ptr_new_state_with_locations)
#define new_state (ptr_new_state_with_locations->s)

#define sfs_check_state_init() \
    fcs_state_ia_alloc_into_var(ptr_new_state_with_locations, hard_thread);

#define sfs_check_state_begin()                                                \
    sfs_check_state_init();                                                    \
    fcs_duplicate_state(new_state_with_locations, state_with_locations, hard_thread->indirect_stacks_buffer);       \
    /* Some A* and BFS parameters that need to be initialized in               \
     * the derived state.                                                      \
     * */                                                                      \
    ptr_new_state_with_locations->parent = ptr_state_with_locations;           \
    ptr_new_state_with_locations->moves_to_parent = moves;                     \
    /* Make sure depth is consistent with the game graph.                      \
     * I.e: the depth of every newly discovered state is derived from          \
     * the state from which it was discovered. */                              \
    ptr_new_state_with_locations->depth = ptr_state_with_locations->depth + 1; \
    /* Mark this state as a state that was not yet visited */                  \
    ptr_new_state_with_locations->visited = 0;                                 \
    /* It's a newly created state which does not have children yet. */         \
    ptr_new_state_with_locations->num_active_children = 0;                     \
    memset(ptr_new_state_with_locations->scan_visited, '\0',                   \
        sizeof(ptr_new_state_with_locations->scan_visited)                     \
        );                                                                     \
    fcs_move_stack_reset(moves);                                               \


#define sfs_check_state_end()                                             \
/* The last move in a move stack should be FCS_MOVE_TYPE_CANONIZE         \
 * because it indicates that the order of the stacks and freecells        \
 * need to be recalculated                                                \
 * */                                                                     \
fcs_move_set_type(temp_move,FCS_MOVE_TYPE_CANONIZE);                      \
fcs_move_stack_push(moves, temp_move);                                    \
                                                                          \
{                                                                         \
    fcs_state_with_locations_t * existing_state;                          \
    check = freecell_solver_check_and_add_state(                          \
        soft_thread,                                                      \
        ptr_new_state_with_locations,                                     \
        &existing_state,                                                  \
        depth);                                                           \
    if ((check == FCS_STATE_BEGIN_SUSPEND_PROCESS) ||                     \
        (check == FCS_STATE_SUSPEND_PROCESS))                             \
    {                                                                     \
        /* This state is not going to be used, so                         \
         * let's clean it. */                                             \
        fcs_clean_state(ptr_new_state_with_locations);                    \
        fcs_state_ia_release(hard_thread);                                \
        return check;                                                     \
    }                                                                     \
    else if (check == FCS_STATE_ALREADY_EXISTS)                           \
    {                                                                     \
        fcs_state_ia_release(hard_thread);                                \
        /* Re-parent the existing state to this one.                      \
         *                                                                \
         * What it means is that if the depth of the state if it          \
         * can be reached from this one is lower than what it             \
         * already have, then re-assign its parent to this state.         \
         * */                                                             \
        if (reparent &&                                                   \
           (existing_state->depth > ptr_state_with_locations->depth+1))   \
        {                                                                 \
            /* Make a copy of "moves" because "moves will be destoryed */ \
            existing_state->moves_to_parent = freecell_solver_move_stack_compact_allocate(hard_thread, moves);                 \
            existing_state->parent = ptr_state_with_locations;            \
            existing_state->depth = ptr_state_with_locations->depth + 1;  \
        }                                                                 \
        fcs_derived_states_list_add_state(                                \
            derived_states_list,                                          \
            existing_state                                                \
            );                                                            \
    }                                                                     \
    else                                                                  \
    {                                                                     \
        fcs_derived_states_list_add_state(                                \
            derived_states_list,                                          \
            ptr_new_state_with_locations                                  \
            );                                                            \
   }                                                                      \
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



extern int freecell_solver_sfs_simple_simon_move_sequence_to_founds(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        );
extern int freecell_solver_sfs_simple_simon_move_sequence_to_true_parent(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        );

extern int freecell_solver_sfs_simple_simon_move_whole_stack_sequence_to_false_parent(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        );

extern int freecell_solver_sfs_simple_simon_move_sequence_to_true_parent_with_some_cards_above(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        );

extern int freecell_solver_sfs_simple_simon_move_sequence_with_some_cards_above_to_true_parent(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        );

extern int freecell_solver_sfs_simple_simon_move_sequence_with_junk_seq_above_to_true_parent_with_some_cards_above(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        );

extern int freecell_solver_sfs_simple_simon_move_whole_stack_sequence_to_false_parent_with_some_cards_above(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        );

extern int freecell_solver_sfs_simple_simon_move_sequence_to_parent_on_the_same_stack(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        );

#ifdef __cplusplus
}
#endif


#endif /* __TESTS_H */
