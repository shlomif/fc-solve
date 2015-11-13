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
 * scans.h - header file for the scans.c - Best-First-Search and Soft-DFS
 * scans.
 */

#ifndef FC_SOLVE__SCANS_H
#define FC_SOLVE__SCANS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "instance.h"

#include "inline.h"
#include "likely.h"

/* We need 2 chars per card - one for the column_idx and one
 * for the card_idx.
 *
 * We also need it times 13 for each of the ranks.
 *
 * We need (4*LOCAL_DECKS_NUM+1) slots to hold the cards plus a
 * (-1,-1) (= end) padding.
 * */
#define FCS_POS_BY_RANK_WIDTH (MAX_NUM_DECKS << 3)

static GCC_INLINE fcs_pos_by_rank_t * fc_solve_calc_positions_by_rank_location(
    fc_solve_soft_thread_t * const soft_thread
)
{
    if (soft_thread->super_method_type == FCS_SUPER_METHOD_DFS)
    {
        return (
            DFS_VAR(soft_thread, soft_dfs_info)[
            DFS_VAR(soft_thread, depth)
            ].positions_by_rank.p
        );
    }
    else
    {
        return (
            BEFS_M_VAR(soft_thread, befs_positions_by_rank)
        ).p;
    }
}


static GCC_INLINE void fc_solve__calc_positions_by_rank_data(
    fc_solve_soft_thread_t * const soft_thread,
    const fcs_state_t * const the_state,
    fcs_positions_by_rank_map_t * const positions_by_rank
)
{
#ifndef HARD_CODED_NUM_STACKS
    fc_solve_instance_t * const instance = HT_INSTANCE(soft_thread->hard_thread);
    SET_GAME_PARAMS();
#endif

#define FCS_POS_IDX(rank, suit) ( (suit)*FCS_SS_POS_BY_RANK_WIDTH + (rank) )
    memset(positions_by_rank, -1, sizeof(*positions_by_rank));
    for (int ds = 0 ; ds < LOCAL_STACKS_NUM ; ds++)
    {
        const fcs_const_cards_column_t dest_col = fcs_state_get_col(*the_state, ds);
        const int dest_cards_num = fcs_col_len(dest_col);

        for (int dc = 0 ; dc < dest_cards_num ; dc++)
        {
            const fcs_card_t card = fcs_col_get_card(dest_col, dc);
            positions_by_rank->p[
                FCS_POS_IDX(fcs_card_rank(card), fcs_card_suit(card))
            ] = (fcs_pos_by_rank_t) {.col = ds, .height = dc};
        }
    }
}

extern int fc_solve_sfs_check_state_begin(
    fc_solve_hard_thread_t * const hard_thread,
    fcs_kv_state_t * const out_new_state_out,
    fcs_kv_state_t * const raw_ptr_state_raw,
    fcs_move_stack_t * const moves
    );

extern void fc_solve_sfs_check_state_end(
    fc_solve_soft_thread_t * const soft_thread,
    fcs_kv_state_t * const key_ptr_state_key,
    fcs_kv_state_t * const key_ptr_new_state_key,
    const int state_context_value,
    fcs_move_stack_t * const moves,
    fcs_derived_states_list_t * const derived_states_list
    );

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__SCANS_H */
