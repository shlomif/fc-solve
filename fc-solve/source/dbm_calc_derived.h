/* Copyright (c) 2012 Shlomi Fish
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
 * dbm_calc_derived.h - contains functions to calculate the derived states
 * from a certain position.
 *
 */
#ifndef FC_SOLVE_DBM_CALC_DERIVED_H
#define FC_SOLVE_DBM_CALC_DERIVED_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>

#include "config.h"

#include "bool.h"
#include "inline.h"
#include "state.h"
#include "meta_alloc.h"
#include "fcs_enums.h"

#include "fcs_dllexport.h"
#include "dbm_common.h"
#include "dbm_solver_key.h"
#include "dbm_calc_derived_iface.h"
#include "indirect_buffer.h"
#include "fcc_brfs_test.h"

struct fcs_derived_state_struct
{
    fcs_state_keyval_pair_t state;
    fcs_encoded_state_buffer_t key;
    fcs_dbm_record_t * parent;
    struct fcs_derived_state_struct * next;
    fcs_bool_t is_reversible_move;
    fcs_fcc_move_t move;
    int num_non_reversible_moves;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
};

typedef struct fcs_derived_state_struct fcs_derived_state_t;

#define MAKE_MOVE(src, dest) ((src) | ((dest) << 4))
#define COL2MOVE(idx) (idx)
#define FREECELL2MOVE(idx) (idx+8)
#define FOUND2MOVE(idx) ((idx)+8+4)

#ifdef INDIRECT_STACK_STATES

#define COPY_INDIRECT_COLS() \
{ \
    for (copy_col_idx=0;copy_col_idx < LOCAL_STACKS_NUM ; copy_col_idx++) \
    { \
        copy_stack_col = fcs_state_get_col((ptr_new_state->state.s), copy_col_idx); \
        memcpy(&(ptr_new_state->indirect_stacks_buffer[copy_col_idx << 7]), copy_stack_col, fcs_col_len(copy_stack_col)+1); \
        fcs_state_get_col((ptr_new_state->state.s), copy_col_idx) = &(ptr_new_state->indirect_stacks_buffer[copy_col_idx << 7]); \
   } \
}

#else

#define COPY_INDIRECT_COLS() {}

#endif

#define BEGIN_NEW_STATE() \
{ \
    if (*derived_list_recycle_bin) \
    {  \
        (*derived_list_recycle_bin) = \
        (ptr_new_state =  \
         (*derived_list_recycle_bin) \
        )->next; \
    } \
    else \
    { \
        ptr_new_state = \
        (fcs_derived_state_t *) \
        fcs_compact_alloc_ptr( \
                derived_list_allocator, \
                sizeof(*ptr_new_state) \
                ); \
    } \
    fcs_duplicate_state(  \
        &(ptr_new_state->state), \
        init_state_kv_ptr \
    ); \
        \
    COPY_INDIRECT_COLS() \
}

#define COMMIT_NEW_STATE(src, dest, is_reversible) \
{ \
 \
    ptr_new_state->parent = parent_ptr; \
    ptr_new_state->move = MAKE_MOVE((src), (dest)); \
 \
    ptr_new_state->is_reversible_move = (is_reversible); \
    /* Finally, enqueue the new state. */ \
    ptr_new_state->next = (*derived_list); \
    (*derived_list) = ptr_new_state; \
 \
}

static GCC_INLINE int calc_foundation_to_put_card_on(
        fcs_state_t * my_ptr_state,
        fcs_card_t card
        )
{
    int deck;

    for(deck=0;deck < INSTANCE_DECKS_NUM;deck++)
    {
        if (fcs_foundation_value(*my_ptr_state, (deck<<2)+fcs_card_suit(card)) == fcs_card_rank(card) - 1)
        {
            int other_deck_idx;

            for (other_deck_idx = 0 ; other_deck_idx < (INSTANCE_DECKS_NUM << 2) ; other_deck_idx++)
            {
                if (fcs_foundation_value(*my_ptr_state, other_deck_idx)
                        < fcs_card_rank(card) - 2 -
                        ((other_deck_idx&0x1) == (fcs_card_suit(card)&0x1))
                   )
                {
                    break;
                }
            }
            if (other_deck_idx == (INSTANCE_DECKS_NUM << 2))
            {
                return (deck<<2)+fcs_card_suit(card);
            }
        }
    }
    return -1;
}

typedef struct {
    fcs_fcc_moves_list_item_t * recycle_bin;
    fcs_compact_allocator_t * allocator;
} fcs_fcc_moves_seq_allocator_t;

static GCC_INLINE fcs_fcc_moves_list_item_t * fc_solve_fcc_alloc_moves_list_item(
    fcs_fcc_moves_seq_allocator_t * allocator
)
{
    fcs_fcc_moves_list_item_t * new_item;

    if (allocator->recycle_bin)
    {
        allocator->recycle_bin = (new_item = allocator->recycle_bin)->next;
    }
    else
    {
        new_item = (fcs_fcc_moves_list_item_t *)
            fcs_compact_alloc_ptr(
                allocator->allocator,
                sizeof(*new_item)
                );
    }
    new_item->next = NULL;

    return new_item;
}

/* Returns the additional number of cards moved to the foundation  */
static GCC_INLINE int horne_prune(
    fcs_state_keyval_pair_t * init_state_kv_ptr,
    fcs_fcc_moves_seq_t * moves_seq,
    fcs_fcc_moves_seq_allocator_t * allocator
)
{
    int stack_idx, fc;
    fcs_cards_column_t col;
    int cards_num;
    int dest_foundation;
    int num_cards_moved;
    fcs_card_t card;
    /*  */
    fcs_fcc_move_t additional_moves[RANK_KING * 4 * DECKS_NUM];
    int count_moves_so_far = 0;

#define the_state (init_state_kv_ptr->s)
    do {
        num_cards_moved = 0;
        for( stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
        {
            col = fcs_state_get_col(the_state, stack_idx);
            cards_num = fcs_col_len(col);
            if (cards_num)
            {
                /* Get the top card in the stack */
                card = fcs_col_get_card(col, cards_num-1);

                if ((dest_foundation =
                    calc_foundation_to_put_card_on(&the_state, card)) >= 0)
                {
                    /* We can safely move it. */
                    num_cards_moved++;

                    fcs_col_pop_top(col);

                    fcs_increment_foundation(the_state, dest_foundation);

                    additional_moves[count_moves_so_far++]
                        = MAKE_MOVE(COL2MOVE(stack_idx), FOUND2MOVE(dest_foundation));
                }
            }
        }

        /* Now check the same for the free cells */
        for( fc=0 ; fc < LOCAL_FREECELLS_NUM ; fc++)
        {
            card = fcs_freecell_card(the_state, fc);
            if (fcs_card_is_valid(card))
            {
                if ((dest_foundation =
                    calc_foundation_to_put_card_on(&the_state, card)) >= 0)
                {
                    num_cards_moved++;

                    /* We can put it there */

                    fcs_empty_freecell(the_state, fc);
                    fcs_increment_foundation(the_state, dest_foundation);
                    additional_moves[count_moves_so_far++]
                        = MAKE_MOVE(COL2MOVE(fc), FOUND2MOVE(dest_foundation));
                }
            }
        }
    } while (num_cards_moved);

    /* modify moves_seq in-place. */
    if (count_moves_so_far && moves_seq)
    {
        fcs_fcc_moves_list_item_t * * iter;
        int pos, count, pos_moves_so_far;

        iter = &(moves_seq->moves_list);

        /* Assuming FCS_FCC_NUM_MOVES_IN_ITEM is 8 and we want (*iter)
         * to point at the place to either write the new moves or alternatively
         * (on parity) on the pointer to allocate a new list_item for the
         * moves.
         *
         * If count is 0, then we should move 0.
         * If count is 1, then we should move 0.
         * .
         * .
         * .
         * If count is 7, then we should move 0.
         * If count is 8, then we should move 1 time.
         *
         * to sum up we need to move count / FCS_FCC_NUM_MOVES_IN_ITEM .
         *
         * */
        count = moves_seq->count;
        for (pos = 0 ;
             pos <= count - FCS_FCC_NUM_MOVES_IN_ITEM ;
             pos += FCS_FCC_NUM_MOVES_IN_ITEM
        )
        {
            iter = &((*iter)->next);
        }

        pos = count;

        for (pos_moves_so_far = 0 ;
             pos_moves_so_far < count_moves_so_far ;
             pos_moves_so_far++)
        {
            if (pos % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                (*iter) = fc_solve_fcc_alloc_moves_list_item(allocator);
            }
            (*iter)->data.s[pos % FCS_FCC_NUM_MOVES_IN_ITEM] = additional_moves[pos_moves_so_far];
            if ((++pos) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                iter = &((*iter)->next);
            }
        }
        moves_seq->count += count_moves_so_far;
    }

    return count_moves_so_far;
}

static GCC_INLINE fcs_bool_t instance_solver_thread_calc_derived_states(
    fcs_state_keyval_pair_t * init_state_kv_ptr,
    fcs_encoded_state_buffer_t * key,
    fcs_dbm_record_t * parent_ptr,
    fcs_derived_state_t * * derived_list,
    fcs_derived_state_t * * derived_list_recycle_bin,
    fcs_compact_allocator_t * derived_list_allocator,
    const fcs_bool_t perform_horne_prune
)
{
    fcs_derived_state_t * ptr_new_state;
    int stack_idx, cards_num, ds;
    fcs_cards_column_t col, dest_col;
#ifdef INDIRECT_STACK_STATES
    fcs_cards_column_t copy_stack_col;
    int copy_col_idx;
#endif
    fcs_card_t card, dest_card;
    int deck, suit;
#ifndef FCS_FREECELL_ONLY
    /* needed by the macros. */
    int sequences_are_built_by;
#endif
    int empty_fc_idx = -1;
    int empty_stack_idx = -1;

#ifndef FCS_FREECELL_ONLY
    sequences_are_built_by = FCS_SEQ_BUILT_BY_ALTERNATE_COLOR;
#endif

#define the_state (init_state_kv_ptr->s)
#define new_state (ptr_new_state->state.s)

#define SUIT_LIMIT ( DECKS_NUM * 4 )
    for (suit = 0 ; suit < SUIT_LIMIT ; suit++)
    {
        if (fcs_foundation_value(the_state, suit) < RANK_KING)
        {
            break;
        }
    }

    if (suit == SUIT_LIMIT)
    {
        /* Solved state. */
        return TRUE;
    }

    /* Move top stack cards to foundations. */
    for (stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        col = fcs_state_get_col(the_state, stack_idx);
        cards_num = fcs_col_len(col);
        if (cards_num)
        {
            /* Get the top card in the stack */
            card = fcs_col_get_card(col, cards_num-1);
            suit = fcs_card_suit(card);
            for (deck=0 ; deck < INSTANCE_DECKS_NUM ; deck++)
            {

                if (fcs_foundation_value(the_state, deck*4+suit) == fcs_card_rank(card) - 1)
                {
                    /* We can put it there */
                    BEGIN_NEW_STATE()

                    {
                        fcs_cards_column_t new_temp_col;
                        new_temp_col = fcs_state_get_col(new_state, stack_idx);
                        fcs_col_pop_top(new_temp_col);
                    }

                    fcs_increment_foundation(new_state, deck*4+suit);

                    COMMIT_NEW_STATE(COL2MOVE(stack_idx), FOUND2MOVE(suit), FALSE)
                }
            }
        }
        else
        {
            empty_stack_idx = stack_idx;
        }
    }

#define fc_idx stack_idx
    /* Move freecell stack cards to foundations. */
    for (fc_idx=0 ; fc_idx < LOCAL_FREECELLS_NUM ; fc_idx++)
    {
        card = fcs_freecell_card(the_state, fc_idx);
        suit = fcs_card_suit(card);
        if (fcs_card_is_valid(card))
        {
            for(deck=0;deck<INSTANCE_DECKS_NUM;deck++)
            {
                if (fcs_foundation_value(the_state, deck*4+suit) == fcs_card_rank(card) - 1)
                {
                    BEGIN_NEW_STATE()

                    /* We can put it there */
                    fcs_empty_freecell(new_state, fc_idx);

                    fcs_increment_foundation(new_state, deck*4+suit);

                    COMMIT_NEW_STATE(
                        FREECELL2MOVE(fc_idx), FOUND2MOVE(suit), FALSE
                    )
                }
            }
        }
        else
        {
            empty_fc_idx = fc_idx;
        }
    }

    /* Move stack card on top of a parent */
    for (stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
    {
        col = fcs_state_get_col(the_state, stack_idx);
        cards_num = fcs_col_len(col);
        if (cards_num > 0)
        {
            card = fcs_col_get_card(col, cards_num-1);

            for (ds=0;ds<LOCAL_STACKS_NUM;ds++)
            {
                if (ds == stack_idx)
                {
                    continue;
                }

                dest_col = fcs_state_get_col(the_state, ds);

                if (fcs_col_len(dest_col) > 0)
                {
                    dest_card = fcs_col_get_card(dest_col,
                            fcs_col_len(dest_col)-1);
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* Let's move it */
                        BEGIN_NEW_STATE()

                        {
                            fcs_cards_column_t new_src_col;
                            fcs_cards_column_t new_dest_col;

                            new_src_col = fcs_state_get_col(new_state, stack_idx);
                            new_dest_col = fcs_state_get_col(new_state, ds);

                            fcs_col_pop_top(new_src_col);
                            fcs_col_push_card(new_dest_col, card);
                        }
#define FROM_COL_IS_REVERSIBLE_MOVE() \
                        ((cards_num <= 1) ? TRUE \
                             : fcs_is_parent_card(card, fcs_col_get_card( \
                                     col, cards_num-2) \
                               ) \
                        )

                        COMMIT_NEW_STATE(
                            COL2MOVE(stack_idx), COL2MOVE(ds),
                            FROM_COL_IS_REVERSIBLE_MOVE()
                        )
                    }
                }
            }
        }
    }

    /* Move freecell card on top of a parent */
    for (fc_idx=0 ; fc_idx < LOCAL_FREECELLS_NUM ; fc_idx++)
    {
        card = fcs_freecell_card(the_state, fc_idx);
        if (fcs_card_is_valid(card))
        {
            for (ds=0;ds<LOCAL_STACKS_NUM;ds++)
            {
                dest_col = fcs_state_get_col(the_state, ds);

                if (fcs_col_len(dest_col) > 0)
                {
                    dest_card = fcs_col_get_card(dest_col,
                            fcs_col_len(dest_col)-1);
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* Let's move it */
                        BEGIN_NEW_STATE()

                        {
                            fcs_cards_column_t new_dest_col;

                            new_dest_col = fcs_state_get_col(new_state, ds);

                            fcs_col_push_card(new_dest_col, card);

                            fcs_empty_freecell(new_state, fc_idx);
                        }

                        COMMIT_NEW_STATE(
                            FREECELL2MOVE(fc_idx), COL2MOVE(ds),
                            TRUE
                        )
                    }
                }
            }
        }
    }

    if (empty_stack_idx >= 0)
    {
        /* Stack Card to Empty Stack */
        for(stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
        {
            col = fcs_state_get_col(the_state, stack_idx);
            cards_num = fcs_col_len(col);
            /* Bug fix: if there's only one card in a column, there's no
             * point moving it to a new empty column.
             * */
            if (cards_num > 1)
            {
                card = fcs_col_get_card(col, cards_num-1);
                /* Let's move it */
                {
                    BEGIN_NEW_STATE()

                    {
                        fcs_cards_column_t new_src_col;
                        fcs_cards_column_t empty_stack_col;

                        new_src_col = fcs_state_get_col(new_state, stack_idx);

                        fcs_col_pop_top(new_src_col);

                        empty_stack_col = fcs_state_get_col(new_state, empty_stack_idx);
                        fcs_col_push_card(empty_stack_col, card);
                    }
                    COMMIT_NEW_STATE(
                        COL2MOVE(stack_idx), COL2MOVE(empty_stack_idx),
                        FROM_COL_IS_REVERSIBLE_MOVE()
                    )
                }
            }
        }

        /* Freecell card -> Empty Stack. */
        for (fc_idx=0;fc_idx<LOCAL_FREECELLS_NUM;fc_idx++)
        {
            card = fcs_freecell_card(the_state, fc_idx);
            if (fcs_card_is_empty(card))
            {
                continue;
            }

            {
                BEGIN_NEW_STATE()

                {
                    fcs_cards_column_t new_dest_col;
                    new_dest_col = fcs_state_get_col(new_state, empty_stack_idx);
                    fcs_col_push_card(new_dest_col, card);
                    fcs_empty_freecell(new_state, fc_idx);
                }

                COMMIT_NEW_STATE(
                    FREECELL2MOVE(fc_idx), COL2MOVE(empty_stack_idx),
                    TRUE
                );
            }
        }
    }

    if (empty_fc_idx >= 0)
    {
        /* Stack Card to Empty Freecell */
        for (stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
        {
            col = fcs_state_get_col(the_state, stack_idx);
            cards_num = fcs_col_len(col);
            if (cards_num > 0)
            {
                card = fcs_col_get_card(col, cards_num-1);
                /* Let's move it */
                {
                    BEGIN_NEW_STATE()

                    {
                        fcs_cards_column_t new_src_col;

                        new_src_col = fcs_state_get_col(new_state, stack_idx);

                        fcs_col_pop_top(new_src_col);

                        fcs_put_card_in_freecell(new_state, empty_fc_idx, card);
                    }
                    COMMIT_NEW_STATE(
                        COL2MOVE(stack_idx), FREECELL2MOVE(empty_fc_idx),
                        FROM_COL_IS_REVERSIBLE_MOVE()
                    )
                }
            }
        }
    }
#undef fc_idx

    /* Perform Horne's Prune on all the states. */
    if (perform_horne_prune)
    {
        fcs_derived_state_t * derived_iter;

        for (derived_iter = (*derived_list);
            derived_iter ;
            derived_iter = derived_iter->next
        )
        {
            derived_iter->num_non_reversible_moves =
                (derived_iter->is_reversible_move ? 0 : 1)
                + horne_prune(&(derived_iter->state), NULL, NULL);
        }
    }
    else
    {
        fcs_derived_state_t * derived_iter;

        for (derived_iter = (*derived_list);
            derived_iter ;
            derived_iter = derived_iter->next
        )
        {
            derived_iter->num_non_reversible_moves =
                (derived_iter->is_reversible_move ? 0 : 1);
        }
    }

    return FALSE;
}

#undef FROM_COL_IS_REVERSIBLE_MOVE
#undef the_state
#undef new_state


#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_DBM_CALC_DERIVED_H */
