// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2009 Shlomi Fish
/*
 * scans.h - header file for the scans.c - Best-First-Search and Soft-DFS
 * scans.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "instance.h"

static inline void fc_solve__assign_dest_stack_and_col_ptr(
    int8_t *const positions_by_rank, const int8_t dest_stack,
    const int8_t dest_col, const fcs_card dest_card)
{
#ifdef FCS_FREECELL_ONLY
    int8_t *ptr = &positions_by_rank[(dest_card - 4) << 1];
#else
    int8_t *ptr = &positions_by_rank[(FCS_POS_BY_RANK_WIDTH *
                                         (fcs_card_rank(dest_card) - 1)) +
                                     (fcs_card_suit(dest_card) << 1)];
#endif

#if ((!defined(HARD_CODED_NUM_DECKS)) || (HARD_CODED_NUM_DECKS > 1))
    for (; (*ptr) != -1; ptr += (4 << 1))
    {
    }
#endif

    *(ptr++) = dest_stack;
    *(ptr) = dest_col;
}

static inline void fc_solve__calc_positions_by_rank_data(
    fcs_soft_thread *const soft_thread GCC_UNUSED,
    const fcs_state *const ptr_state_key,
    fcs__positions_by_rank positions_by_rank)
{
#ifndef HARD_CODED_ALL
    var_AUTO(instance, fcs_st_instance(soft_thread));
    SET_GAME_PARAMS();
#endif

    memset(positions_by_rank, -1, sizeof(fcs__positions_by_rank));

#ifndef FCS_DISABLE_SIMPLE_SIMON
    if (instance->is_simple_simon)
    {
#define FCS_POS_IDX(rank, suit) ((suit)*FCS_SS_POS_BY_RANK_WIDTH + (rank))
        fcs_pos_by_rank *const p_by_r = (fcs_pos_by_rank *)positions_by_rank;
        for (int ds = 0; ds < LOCAL_STACKS_NUM; ds++)
        {
            const_AUTO(dest_col, fcs_state_get_col(*ptr_state_key, ds));
            const int dest_cards_num = fcs_col_len(dest_col);

            for (int dc = 0; dc < dest_cards_num; dc++)
            {
                const fcs_card card = fcs_col_get_card(dest_col, dc);
                const int suit = fcs_card_suit(card);
                const int rank = fcs_card_rank(card);

                p_by_r[FCS_POS_IDX(rank, suit)] =
                    (fcs_pos_by_rank){.col = (int8_t)ds, .height = (int8_t)dc};
            }
        }
    }
    else
#endif
    {
#define state_key (*ptr_state_key)

        FCS_ON_NOT_FC_ONLY(const int sequences_are_built_by =
                               GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance));

        /* We need 2 chars per card - one for the column_idx and one
         * for the card_idx.
         *
         * We also need it times 13 for each of the ranks.
         *
         * We need (4*LOCAL_DECKS_NUM+1) slots to hold the cards plus a
         * (-1,-1) (= end) padding.             * */
        /* Populate positions_by_rank by looping over the stacks and
         * indices looking for the cards and filling them. */

        for (int ds = 0; ds < LOCAL_STACKS_NUM; ds++)
        {
            const_AUTO(dest_col, fcs_state_get_col(state_key, ds));
            int top_card_idx = fcs_col_len(dest_col);

            if (unlikely((top_card_idx--) == 0))
            {
                continue;
            }

            fcs_card dest_card;
            {
                fcs_card dest_below_card;
                dest_card = fcs_col_get_card(dest_col, 0);
                for (int dc = 0; dc < top_card_idx;
                     dc++, dest_card = dest_below_card)
                {
                    dest_below_card = fcs_col_get_card(dest_col, dc + 1);
                    if (!fcs_is_parent_card(dest_below_card, dest_card))
                    {
                        fc_solve__assign_dest_stack_and_col_ptr(
                            positions_by_rank, (int8_t)ds, (int8_t)dc,
                            dest_card);
                    }
                }
            }
            fc_solve__assign_dest_stack_and_col_ptr(
                positions_by_rank, (int8_t)ds, (int8_t)top_card_idx, dest_card);
        }
    }
#undef state_key
#undef ptr_state_key
}

static inline const int8_t *fc_solve_calc_positions_by_rank_location(
    fcs_soft_thread *const soft_thread)
{
    if (soft_thread->super_method_type == FCS_SUPER_METHOD_DFS)
    {
        return (DFS_VAR(soft_thread, soft_dfs_info)[DFS_VAR(soft_thread, depth)]
                    .positions_by_rank);
    }
    else
    {
        return (BEFS_M_VAR(soft_thread, befs_positions_by_rank));
    }
}

static inline void add_to_move_funcs_list(
    fcs_move_func **const out_move_funcs_list, size_t *const num_so_far,
    const fcs_move_func *const indexes, const size_t count_to_add)
{
#ifndef FCS_ZERO_FREECELLS_MODE
    size_t num = *num_so_far;
    fcs_move_func *const move_funcs_list =
        SREALLOC(*out_move_funcs_list, num + count_to_add);
    for (size_t i = 0; i < count_to_add; ++i)
    {
        move_funcs_list[num++].f = fc_solve_sfs_move_funcs[indexes[i].idx];
    }

    *out_move_funcs_list = move_funcs_list;
    *num_so_far = num;
#endif
}

extern int fc_solve_sfs_check_state_begin(fcs_hard_thread *const,
    fcs_kv_state *const,
    fcs_kv_state SFS__PASS_MOVE_STACK(fcs_move_stack *const));

extern fcs_collectible_state *fc_solve_sfs_check_state_end(fcs_soft_thread *,
#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
    fcs_kv_state,
#endif
    fcs_kv_state *FCS__pass_moves(fcs_move_stack *));

#ifdef FCS_WITH_MOVES
#define FCS_SET_final_state() instance->final_state = PTR_STATE
#else
#define FCS_SET_final_state()
#endif

#ifdef __cplusplus
}
#endif
