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
 * move.c - move and move stacks routines for Freecell Solver
 */

#include "dll_thunk.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "move.h"
#include "state.h"
#include "fcs_enums.h"

#ifdef FCS_WITH_MOVES

#ifdef FCS_USE_COMPACT_MOVE_TOKENS
const fcs_internal_move_t fc_solve_empty_move = {0, 0, 0, 0};
#else
const fcs_internal_move_t fc_solve_empty_move = {"\0\0\0\0"};
#endif

/*
 * This function performs a given move on a state
 */
void fc_solve_apply_move(fcs_state_t *const ptr_state_key,
    fcs_state_locs_struct_t *const locs,
    const fcs_internal_move_t move FREECELLS_AND_STACKS_ARGS())
{
    fcs_card_t card;
    fcs_cards_column_t col;

#define state_key (ptr_state_key)
    switch (fcs_int_move_get_type(move))
    {
    case FCS_MOVE_TYPE_STACK_TO_STACK:
    {
        fcs_col_transfer_cards(
            fcs_state_get_col(*state_key, fcs_int_move_get_dest_stack(move)),
            fcs_state_get_col(*state_key, fcs_int_move_get_src_stack(move)),
            fcs_int_move_get_num_cards_in_seq(move));
    }
    break;
    case FCS_MOVE_TYPE_FREECELL_TO_STACK:
    {
        col = fcs_state_get_col(*state_key, fcs_int_move_get_dest_stack(move));
        fcs_col_push_card(col,
            fcs_freecell_card(*state_key, fcs_int_move_get_src_freecell(move)));
        fcs_empty_freecell(*state_key, fcs_int_move_get_src_freecell(move));
    }
    break;
    case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
    {
        card =
            fcs_freecell_card(*state_key, fcs_int_move_get_src_freecell(move));
        fcs_put_card_in_freecell(
            *state_key, fcs_int_move_get_dest_freecell(move), card);
        fcs_empty_freecell(*state_key, fcs_int_move_get_src_freecell(move));
    }
    break;

    case FCS_MOVE_TYPE_STACK_TO_FREECELL:
    {
        col = fcs_state_get_col(*state_key, fcs_int_move_get_src_stack(move));
        fcs_col_pop_card(col, card);
        fcs_put_card_in_freecell(
            *state_key, fcs_int_move_get_dest_freecell(move), card);
    }
    break;

    case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
    {
        col = fcs_state_get_col(*state_key, fcs_int_move_get_src_stack(move));
        fcs_col_pop_top(col);
        fcs_increment_foundation(*state_key, fcs_int_move_get_foundation(move));
    }
    break;

    case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
    {
        fcs_empty_freecell(*state_key, fcs_int_move_get_src_freecell(move));
        fcs_increment_foundation(*state_key, fcs_int_move_get_foundation(move));
    }
    break;

#ifndef FCS_FREECELL_ONLY
    case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
    {
        col = fcs_state_get_col(*state_key, fcs_int_move_get_src_stack(move));
        for (int i = 0; i < 13; i++)
        {
            fcs_col_pop_top(col);
            fcs_increment_foundation(
                *state_key, fcs_int_move_get_foundation(move));
        }
    }
    break;
#endif

    case FCS_MOVE_TYPE_CANONIZE:
    {
        if (locs)
        {
            fc_solve_canonize_state_with_locs(state_key,
                locs PASS_FREECELLS(freecells_num) PASS_STACKS(stacks_num));
        }
        else
        {
            fc_solve_canonize_state(state_key PASS_FREECELLS(freecells_num)
                    PASS_STACKS(stacks_num));
        }
    }
    break;
    }
#undef state_key
}
#endif

#define DERIVED_STATES_LIST_GROW_BY 16
void fc_solve_derived_states_list_add_state(
    fcs_derived_states_list_t *const list, fcs_collectible_state_t *const state,
    const int context)
{
    if ((!((list->num_states + (list->states != NULL)) &
            (DERIVED_STATES_LIST_GROW_BY - 1))))
    {
        list->states =
            SREALLOC(list->states, list->num_states + (list->states != NULL) +
                                       DERIVED_STATES_LIST_GROW_BY);
    }
    list->states[list->num_states++] = (fcs_derived_states_list_item_t){
        .state_ptr = state, .context.i = context};
}
