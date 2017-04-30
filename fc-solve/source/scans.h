/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2009 Shlomi Fish
 */
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
    const int8_t dest_col, const fcs_card_t dest_card)
{
#ifdef FCS_FREECELL_ONLY
    int8_t *ptr = &positions_by_rank[(dest_card - 4) << 1];
#else
    int8_t *ptr = &positions_by_rank[(FCS_POS_BY_RANK_WIDTH *
                                         (fcs_card_rank(dest_card) - 1)) +
                                     (fcs_card_suit(dest_card) << 1)];
#endif

// #if ((!defined(HARD_CODED_NUM_DECKS)) || (HARD_CODED_NUM_DECKS > 1))
#if 1
    for (; (*ptr) != -1; ptr += (4 << 1))
    {
    }
#endif

    *(ptr++) = dest_stack;
    *(ptr) = dest_col;
}

static inline void fc_solve__calc_positions_by_rank_data(
    fc_solve_soft_thread_t *const soft_thread,
    const fcs_state_t *const ptr_state_key,
    fcs__positions_by_rank_t positions_by_rank
#ifndef FCS_DISABLE_SIMPLE_SIMON
    ,
    const fcs_bool_t is_simple_simon
#endif
    )
{
#if (!(defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS) &&      \
         defined(FCS_DISABLE_SIMPLE_SIMON)))
    var_AUTO(instance, fcs_st_instance(soft_thread));
    SET_GAME_PARAMS();
#endif

    memset(positions_by_rank, -1, sizeof(fcs__positions_by_rank_t));

#ifndef FCS_DISABLE_SIMPLE_SIMON
    if (is_simple_simon)
    {
#define FCS_SS_POS_BY_RANK_SIZE                                                \
    (sizeof(fcs_pos_by_rank_t) * FCS_SS_POS_BY_RANK_LEN)
#define FCS_POS_IDX(rank, suit) ((suit)*FCS_SS_POS_BY_RANK_WIDTH + (rank))
        fcs_pos_by_rank_t *const p_by_r =
            (fcs_pos_by_rank_t *)positions_by_rank;
        for (int ds = 0; ds < LOCAL_STACKS_NUM; ds++)
        {
            const fcs_const_cards_column_t dest_col =
                fcs_state_get_col(*ptr_state_key, ds);
            const int dest_cards_num = fcs_col_len(dest_col);

            for (int dc = 0; dc < dest_cards_num; dc++)
            {
                const fcs_card_t card = fcs_col_get_card(dest_col, dc);
                const int suit = fcs_card_suit(card);
                const int rank = fcs_card_rank(card);

                p_by_r[FCS_POS_IDX(rank, suit)] =
                    (fcs_pos_by_rank_t){.col = ds, .height = dc};
            }
        }
    }
    else
#endif
    {
#define state_key (*ptr_state_key)

#ifndef FCS_FREECELL_ONLY
        const int sequences_are_built_by =
            GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance);
#endif

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
            const fcs_const_cards_column_t dest_col =
                fcs_state_get_col(state_key, ds);
            int top_card_idx = fcs_col_len(dest_col);

            if (unlikely((top_card_idx--) == 0))
            {
                continue;
            }

            fcs_card_t dest_card;
            {
                fcs_card_t dest_below_card;
                dest_card = fcs_col_get_card(dest_col, 0);
                for (int dc = 0; dc < top_card_idx;
                     dc++, dest_card = dest_below_card)
                {
                    dest_below_card = fcs_col_get_card(dest_col, dc + 1);
                    if (!fcs_is_parent_card(dest_below_card, dest_card))
                    {
                        fc_solve__assign_dest_stack_and_col_ptr(
                            positions_by_rank, ds, dc, dest_card);
                    }
                }
            }
            fc_solve__assign_dest_stack_and_col_ptr(
                positions_by_rank, ds, top_card_idx, dest_card);
        }
    }
#undef state_key
#undef ptr_state_key
}

static inline const int8_t *fc_solve_calc_positions_by_rank_location(
    fc_solve_soft_thread_t *const soft_thread)
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
    fc_solve_solve_for_state_move_func_t **const out_move_funcs_list,
    size_t *const num_so_far, const size_t *const indexes,
    const size_t count_to_add)
{
    const size_t num = *num_so_far;
    fc_solve_solve_for_state_move_func_t *const move_funcs_list =
        SREALLOC(*out_move_funcs_list, num + count_to_add);
    size_t next_i = num;
    for (size_t i = 0; i < count_to_add; i++)
    {
        move_funcs_list[next_i++] = fc_solve_sfs_move_funcs[indexes[i]];
    }

    *out_move_funcs_list = move_funcs_list;
    *num_so_far = next_i;
}

extern int fc_solve_sfs_check_state_begin(fc_solve_hard_thread_t *const,
    fcs_kv_state_t *const,
    fcs_kv_state_t *const SFS__PASS_MOVE_STACK(fcs_move_stack_t *const));

extern void fc_solve_sfs_check_state_end(fc_solve_soft_thread_t *const,
    fcs_kv_state_t *const, fcs_kv_state_t *const,
    const int FCS__pass_moves(fcs_move_stack_t *const),
    fcs_derived_states_list_t *const);

#ifdef __cplusplus
}
#endif
