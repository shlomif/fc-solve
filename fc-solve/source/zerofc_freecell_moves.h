// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish

#pragma once
#include "freecell.h"
#include "meta_move_funcs_helpers.h"

#ifdef FCS_ZERO_FREECELLS_MODE
DECLARE_MOVE_FUNCTION(fc_solve_sfs_zerofc_0AB_atomic_all_moves)
{
    MOVE_FUNCS__define_common();
    STACKS__SET_PARAMS();

    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
        const size_t cards_num = fcs_col_len(col);
        if (!cards_num)
        {
            continue;
        }
        /* Get the top card in the stack */
        const fcs_card card = fcs_col_get_card(col, cards_num - 1);
        for (size_t deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
        {
            if (fcs_foundation_value(state_key,
                    deck * 4 + fcs_card_suit(card)) != fcs_card_rank(card) - 1)
            {
                continue;
            }
            /* We can put it there */
            sfs_check_state_begin();

            my_copy_stack(stack_idx);
            fcs_state_pop_col_top(&new_state_key, stack_idx);
            fcs_increment_foundation(
                new_state_key, deck * 4 + fcs_card_suit(card));

            fcs_move_stack_non_seq_push(moves,
                FCS_MOVE_TYPE_STACK_TO_FOUNDATION, stack_idx,
                deck * 4 + fcs_card_suit(card));

            sfs_check_state_end();
// #ifdef FCS_BREAK_BACKWARD_COMPAT_2
#if 0
            return;
#else
            break;
#endif
        }
    }
    const int num_cards_in_col_threshold = CALC_num_cards_in_col_threshold();
    if (IS_FILLED_BY_NONE())
    {
        goto after_empty;
    }
    if (soft_thread->num_vacant_stacks == 0)
    {
        goto after_empty;
    }
    SET_empty_stack_idx(empty_stack_idx);

    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
        const int cards_num = fcs_col_len(col);

        /* Bug fix: if there's only one card in a column, there's no
         * point moving it to a new empty column.
         * */
        if (cards_num <= 1)
        {
            continue;
        }
        const fcs_card card = fcs_col_get_card(col, cards_num - 1);

        if (IS_FILLED_BY_KINGS_ONLY() && (!fcs_card_is_king(card)))
        {
            continue;
        }
        /* Let's move it */
        sfs_check_state_begin();
        copy_two_stacks(stack_idx, empty_stack_idx);
        fcs_state_pop_col_top(&new_state_key, stack_idx);
        fcs_state_push(&new_state_key, empty_stack_idx, card);
        fcs_push_1card_seq(moves, stack_idx, empty_stack_idx);

        sfs_check_state_end();
    }
after_empty:

    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
        const int cards_num = fcs_col_len(col);

        if (cards_num <= num_cards_in_col_threshold)
        {
            continue;
        }
        const fcs_card card = fcs_col_get_card(col, cards_num - 1);

        for (int ds = 0; ds < LOCAL_STACKS_NUM; ds++)
        {
            if ((stack_i)ds == stack_idx)
            {
                continue;
            }

            var_AUTO(dest_col, fcs_state_get_col(state_key, ds));
            const int dest_cards_num = fcs_col_len(dest_col);

            if (!dest_cards_num)
            {
                continue;
            }
            const fcs_card dest_card =
                fcs_col_get_card(dest_col, fcs_col_len(dest_col) - 1);
            if (!fcs_is_parent_card(card, dest_card))
            {
                continue;
            }
            sfs_check_state_begin();
            copy_two_stacks(stack_idx, ds);
            fcs_state_pop_col_top(&new_state_key, stack_idx);
            fcs_state_push(&new_state_key, (stack_i)ds, card);
            fcs_push_1card_seq(moves, stack_idx, (stack_i)ds);

            sfs_check_state_end();
        }
    }
}
#endif
