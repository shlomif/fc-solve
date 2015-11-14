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

static GCC_INLINE void fc_solve__assign_dest_stack_and_col_ptr(
    char * const positions_by_rank,
    const char dest_stack,
    const char dest_col,
    const fcs_card_t dest_card
)
{
    char * ptr = &positions_by_rank[
        (FCS_POS_BY_RANK_WIDTH *
         (fcs_card_rank(dest_card)-1)
        )
        +
        (fcs_card_suit(dest_card)<<1)
        ];

#if (!defined(HARD_CODED_NUM_DECKS) || (HARD_CODED_NUM_DECKS == 1))
    for(;(*ptr) != -1;ptr += (4 << 1))
    {
    }
#endif

    *(ptr++) = dest_stack;
    *(ptr) = dest_col;
}

static GCC_INLINE void fc_solve_get_the_positions_by_rank_data__freecell_generator(
    fc_solve_soft_thread_t * const soft_thread,
    const fcs_state_t * const ptr_state_key,
    fcs__positions_by_rank_t * const positions_by_rank
)
{
#define state_key (*ptr_state_key)

#if (!(defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
    fc_solve_instance_t * const instance = HT_INSTANCE(soft_thread->hard_thread);
    SET_GAME_PARAMS();
#endif

#ifndef FCS_FREECELL_ONLY
    const int sequences_are_built_by = GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance);
#endif

    /* We need 2 chars per card - one for the column_idx and one
     * for the card_idx.
     *
     * We also need it times 13 for each of the ranks.
     *
     * We need (4*LOCAL_DECKS_NUM+1) slots to hold the cards plus a
     * (-1,-1) (= end) padding.             * */
#define FCS_POS_BY_RANK_SIZE (sizeof(positions_by_rank->freecell[0]) * FCS_POS_BY_RANK_LEN)

    memset(positions_by_rank, -1, FCS_POS_BY_RANK_SIZE);

    {
        /* Populate positions_by_rank by looping over the stacks and
         * indices looking for the cards and filling them. */

        for (int ds = 0 ; ds < LOCAL_STACKS_NUM ; ds++)
        {
            const fcs_const_cards_column_t dest_col = fcs_state_get_col(state_key, ds);
            int top_card_idx = fcs_col_len(dest_col);

            if (unlikely((top_card_idx--) == 0))
            {
                continue;
            }

            fcs_card_t dest_card;
            {
                fcs_card_t dest_below_card;
                dest_card = fcs_col_get_card(dest_col, 0);
                for (
                    int dc=0
                    ;
                    dc < top_card_idx
                    ;
                    dc++,
                    dest_card = dest_below_card
                )
                {
                    dest_below_card = fcs_col_get_card(dest_col, dc+1);
                    if (!fcs_is_parent_card(dest_below_card, dest_card))
                    {
                        fc_solve__assign_dest_stack_and_col_ptr(
                            positions_by_rank->freecell,
                            ds,
                            dc,
                            dest_card
                        );
                    }
                }
            }
            fc_solve__assign_dest_stack_and_col_ptr(
                positions_by_rank->freecell,
                ds,
                top_card_idx,
                dest_card
            );
        }
    }
}
#undef state_key
#undef ptr_state_key

#define FCS_SS_POS_BY_RANK_SIZE (sizeof(positions_by_rank->simpsim[0]) * FCS_SS_POS_BY_RANK_LEN)
static GCC_INLINE void fc_solve_get_the_positions_by_rank_data__ss_generator(
    fc_solve_soft_thread_t * const soft_thread,
    const fcs_state_t * const the_state,
    fcs__positions_by_rank_t * const positions_by_rank
)
{
    fc_solve_instance_t * const instance = HT_INSTANCE(soft_thread->hard_thread);
    SET_GAME_PARAMS();

#define FCS_POS_IDX(rank, suit) ( (suit)*FCS_SS_POS_BY_RANK_WIDTH + (rank) )

    memset(positions_by_rank, -1, FCS_SS_POS_BY_RANK_SIZE);
    for (int ds = 0 ; ds < LOCAL_STACKS_NUM ; ds++)
    {
        const fcs_const_cards_column_t dest_col = fcs_state_get_col(*the_state, ds);
        const int dest_cards_num = fcs_col_len(dest_col);

        for (int dc = 0 ; dc < dest_cards_num ; dc++)
        {
            const fcs_card_t card = fcs_col_get_card(dest_col, dc);
            const int suit = fcs_card_suit(card);
            const int rank = fcs_card_rank(card);

            positions_by_rank->simpsim[FCS_POS_IDX(rank, suit)] = (fcs_pos_by_rank_t) {.col = ds, .height = dc};
        }
    }
}
static GCC_INLINE void fc_solve__calc_positions_by_rank_data(
    fc_solve_soft_thread_t * const soft_thread,
    const fcs_state_t * const ptr_state_key,
    fcs__positions_by_rank_t * const positions_by_rank
#ifndef FCS_DISABLE_SIMPLE_SIMON
    , const fcs_bool_t is_simple_simon
#endif
)
{
#ifndef FCS_DISABLE_SIMPLE_SIMON
    if (is_simple_simon)
    {
        fc_solve_get_the_positions_by_rank_data__ss_generator(
            soft_thread,
            ptr_state_key,
            positions_by_rank
        );
    }
    else
#endif
    {
        fc_solve_get_the_positions_by_rank_data__freecell_generator(
            soft_thread,
            ptr_state_key,
            positions_by_rank
        );
    }
}


static GCC_INLINE fcs__positions_by_rank_t * fc_solve_calc_positions_by_rank_location(
    fc_solve_soft_thread_t * const soft_thread
)
{
    if (soft_thread->super_method_type == FCS_SUPER_METHOD_DFS)
    {
        return &(
            DFS_VAR(soft_thread, soft_dfs_info)[
            DFS_VAR(soft_thread, depth)
            ].positions_by_rank
        );
    }
    else
    {
        return &(
            BEFS_M_VAR(soft_thread, befs_positions_by_rank)
        );
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
