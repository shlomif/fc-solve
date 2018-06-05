/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#include <assert.h>
#include "fc_pro_iface_pos.h"
#include "rinutils.h"

#define MOVES_PROCESSED_GROW_BY 32
static inline void moves_processed_add_new_move(
    fcs_moves_processed *const moves, const fcs_extended_move new_move)
{
    if (!((++moves->num_moves) & (MOVES_PROCESSED_GROW_BY - 1)))
    {
        moves->moves =
            SREALLOC(moves->moves, moves->num_moves + MOVES_PROCESSED_GROW_BY);
    }
    moves->moves[moves->num_moves - 1] = new_move;
}

static inline bool fc_solve_fc_pro__can_be_moved(
    fcs_state *const s, const fcs_card card)
{
    const int rank = fcs_card_rank(card);
    const int suit = fcs_card_suit(card);
    if ((fcs_foundation_value(*s, suit ^ 0x1) >= rank - 2) &&
        (fcs_foundation_value(*s, suit ^ 0x1 ^ 0x2) >= rank - 2) &&
        (fcs_foundation_value(*s, suit ^ 0x2) >= rank - 3) &&
        (fcs_foundation_value(*s, suit) == rank - 1))
    {
        fcs_increment_foundation(*s, suit);
        return TRUE;
    }
    return FALSE;
}

DLLEXPORT void fc_solve_moves_processed_gen(fcs_moves_processed *const ret,
    fcs_state_keyval_pair *const orig, const int num_freecells GCC_UNUSED,
    const fcs_moves_sequence_t *const moves_seq)
{
    fcs_state_keyval_pair pos_proto;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    FCS_STATE__DUP_keyval_pair(pos_proto, *orig);
    for (int i = 0; i < 8; ++i)
    {
        fcs_copy_stack(pos_proto.s, pos_proto.info, i, indirect_stacks_buffer);
    }

#define pos (pos_proto.s)
    int virtual_stack_len[8];
    const int num_back_end_moves = moves_seq->num_moves;
    var_PTR(next_move_ptr, moves_seq->moves - 1);
    ret->num_moves = 0;
    ret->moves = SMALLOC(ret->moves, MOVES_PROCESSED_GROW_BY);
    ret->next_move_idx = 0;
    for (int i = 0; i < 8; ++i)
    {
        virtual_stack_len[i] = fcs_state_col_len(pos, i);
    }

    for (int move_idx = 0; move_idx < num_back_end_moves; move_idx++)
    {
    // Move safe cards to the foundations
    loop_start:
        for (int i = 0; i < 8; ++i)
        {
            var_AUTO(col, fcs_state_get_col(pos, i));
            const_AUTO(l, fcs_col_len(col));
            if (l && fc_solve_fc_pro__can_be_moved(
                         &pos, fcs_col_get_card(col, l - 1)))
            {
                fcs_col_pop_top(col);
                goto loop_start;
            }
        }
#if MAX_NUM_FREECELLS > 0
        for (int j = 0; j < num_freecells; j++)
        {
            const fcs_card card = fcs_freecell_card(pos, j);
            if (fcs_card_is_valid(card) &&
                fc_solve_fc_pro__can_be_moved(&pos, card))
            {
                fcs_empty_freecell(pos, j);
                goto loop_start;
            }
        }
#endif
        const_AUTO(move, *(++next_move_ptr));

        switch (fcs_move_get_type(move))
        {
        case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
        {
            const int src = fcs_move_get_src_stack(move);
            var_AUTO(col, fcs_state_get_col(pos, src));
            assert(virtual_stack_len[src] >= fcs_col_len(col));
            if (virtual_stack_len[src] == fcs_col_len(col))
            {
                fcs_card card;
                fcs_col_pop_card(col, card);
                fcs_increment_foundation(pos, fcs_card_suit(card));
                moves_processed_add_new_move(ret,
                    (fcs_extended_move){.move = move, .to_empty_stack = FALSE});
            }
            --virtual_stack_len[src];
        }
        break;

        case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
#if MAX_NUM_FREECELLS > 0
        {
            const int src = fcs_move_get_src_freecell(move);
            if (!fcs_freecell_is_empty(pos, src))
                moves_processed_add_new_move(ret,
                    (fcs_extended_move){.move = move, .to_empty_stack = FALSE});
            fcs_increment_foundation(
                pos, fcs_card_suit(fcs_freecell_card(pos, src)));
            fcs_empty_freecell(pos, src);
        }
#endif
        break;

        case FCS_MOVE_TYPE_FREECELL_TO_STACK:
#if MAX_NUM_FREECELLS > 0
        {
            const_AUTO(src, fcs_move_get_src_freecell(move));
            const_AUTO(dest, fcs_move_get_dest_stack(move));
            if (!fcs_freecell_is_empty(pos, src))
            {
                moves_processed_add_new_move(ret,
                    (fcs_extended_move){.move = move, .to_empty_stack = FALSE});
                fcs_state_push(&pos, dest, fcs_freecell_card(pos, src));
                fcs_empty_freecell(pos, src);
            }
            ++virtual_stack_len[dest];
        }
#endif
        break;

        case FCS_MOVE_TYPE_STACK_TO_FREECELL:
#if MAX_NUM_FREECELLS > 0
        {
            const int src = fcs_move_get_src_stack(move);
            const int dest = fcs_move_get_dest_freecell(move);
            assert(virtual_stack_len[src] > 0);
            var_AUTO(col, fcs_state_get_col(pos, src));
            assert(fcs_col_len(col) <= virtual_stack_len[src]);
            if (fcs_col_len(col) >= virtual_stack_len[src])
            {
                moves_processed_add_new_move(ret,
                    (fcs_extended_move){.move = move, .to_empty_stack = FALSE});
                fcs_card temp_card;
                fcs_col_pop_card(col, temp_card);
                fcs_put_card_in_freecell(pos, dest, temp_card);
            }
            --virtual_stack_len[src];
        }
#endif
        break;

        case FCS_MOVE_TYPE_STACK_TO_STACK:
        {
            const_AUTO(src, fcs_move_get_src_freecell(move));
            const_AUTO(dest, fcs_move_get_dest_stack(move));
            int num_cards = fcs_move_get_num_cards_in_seq(move);
            var_AUTO(src_col, fcs_state_get_col(pos, src));
            var_AUTO(dest_col, fcs_state_get_col(pos, dest));
            const int src_len = fcs_col_len(src_col);
            assert(virtual_stack_len[src] >= src_len);
            if (virtual_stack_len[src] > src_len)
            {
                const int virt_num_cards =
                    min((virtual_stack_len[src] - src_len), num_cards);
                virtual_stack_len[src] -= virt_num_cards;
                virtual_stack_len[dest] += virt_num_cards;
                num_cards -= virt_num_cards;
            }
            if (num_cards > 0)
            {
                fcs_move_t out_move;
                fcs_move_set_type(out_move, FCS_MOVE_TYPE_STACK_TO_STACK);
                fcs_move_set_src_stack(out_move, src);
                fcs_move_set_dest_stack(out_move, dest);
                fcs_move_set_num_cards_in_seq(out_move, num_cards);
                moves_processed_add_new_move(
                    ret, (fcs_extended_move){.move = out_move,
                             .to_empty_stack = (fcs_col_len(dest_col) == 0)});
                fcs_col_transfer_cards(dest_col, src_col, num_cards);
                virtual_stack_len[dest] += num_cards;
                virtual_stack_len[src] -= num_cards;
            }
        }
        break;
        }
    }
}
