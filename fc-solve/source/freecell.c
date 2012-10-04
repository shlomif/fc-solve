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
 * freecell.c - The various movement tests performed by Freecell Solver
 *
 */

#define BUILDING_DLL 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <assert.h>


#include "config.h"

#include "state.h"
#include "card.h"
#include "instance.h"
#include "scans.h"

#include "meta_move_funcs_helpers.h"
#include "unused.h"
#include "freecell.h"

#include "inline.h"
#include "likely.h"

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

/*
 * Throughout this code the following local variables are used to quickly
 * access the instance's members:
 *
 * LOCAL_STACKS_NUM - the number of stacks in the state
 * LOCAL_FREECELLS_NUM - the number of freecells in the state
 * sequences_are_built_by - the type of sequences of this board.
 * */

/*
 * This function tries to move stack cards that are present at the
 * top of stacks to the foundations.
 * */
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_top_stack_cards_to_founds)
{
    tests_declare_accessors()

    tests_define_accessors();

#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    for (int stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        int cards_num = fcs_col_len(col);
        if (cards_num)
        {
            /* Get the top card in the stack */
            fcs_card_t card = fcs_col_get_card(col, cards_num-1);
            for (int deck=0;deck < INSTANCE_DECKS_NUM;deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_rank(card) - 1)
                {
                    /* We can put it there */

                    sfs_check_state_begin();

                    my_copy_stack(stack_idx);
                    {
                        fcs_cards_column_t new_temp_col;
                        new_temp_col = fcs_state_get_col(new_state, stack_idx);
                        fcs_col_pop_top(new_temp_col);
                    }

                    fcs_increment_foundation(new_state, deck*4+fcs_card_suit(card));

                    fcs_internal_move_t temp_move;

                    fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FOUNDATION);
                    fcs_int_move_set_src_stack(temp_move,stack_idx);
                    fcs_int_move_set_foundation(temp_move,deck*4+fcs_card_suit(card));

                    fcs_move_stack_push(moves, temp_move);

                    fcs_flip_top_card(stack_idx);

                    sfs_check_state_end()
                    break;
                }
            }
        }
    }

    return;
}

static GCC_INLINE void sort_derived_states(
        fcs_derived_states_list_t * derived_states_list,
        int initial_derived_states_num_states
        )
{
    fcs_derived_states_list_item_t * start =
        derived_states_list->states + initial_derived_states_num_states;
    fcs_derived_states_list_item_t * limit =
        derived_states_list->states + derived_states_list->num_states;

    for (fcs_derived_states_list_item_t * b = start+1 ; b < limit ; b++)
    {
        for (fcs_derived_states_list_item_t * c = b ; (c > start) && (c[0].context.i < c[-1].context.i); c--)
        {
            fcs_derived_states_list_item_t temp = c[-1];
            c[-1] = c[0];
            c[0] = temp;
        }
    }
}

/*
 * This test moves single cards that are present in the freecells to
 * the foundations.
 * */
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_to_founds)
{
    tests_declare_accessors_no_stacks();

    tests_define_accessors_no_stacks();

#ifndef HARD_CODED_NUM_FREECELLS
    SET_GAME_PARAMS();
#endif

    /* Now check the same for the free cells */
    for (int fc=0 ; fc < LOCAL_FREECELLS_NUM ; fc++)
    {
        fcs_card_t card = fcs_freecell_card(state, fc);
        if (fcs_card_is_valid(card))
        {
            for (int deck=0 ; deck < INSTANCE_DECKS_NUM ; deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_rank(card) - 1)
                {
                    /* We can put it there */
                    sfs_check_state_begin()

                    fcs_empty_freecell(new_state, fc);

                    fcs_increment_foundation(new_state, deck*4+fcs_card_suit(card));

                    fcs_internal_move_t temp_move;

                    fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION);
                    fcs_int_move_set_src_freecell(temp_move,fc);
                    fcs_int_move_set_foundation(temp_move,deck*4+fcs_card_suit(card));

                    fcs_move_stack_push(moves, temp_move);

                    sfs_check_state_end();
                }
            }
        }
    }

    return;
}

/*
 * This function empties two stacks from the new state
 * into freeeclls and empty columns
 */
static GCC_INLINE int empty_two_cols_from_new_state(
    fc_solve_soft_thread_t * soft_thread,
    fcs_kv_state_t * kv_ptr_new_state,
    fcs_move_stack_t * moves,
    const int cols_indexes[3],
    int nc1, int nc2
)
{
#define key_ptr_new_state_key (kv_ptr_new_state->key)
#define my_new_out_state_key (*key_ptr_new_state_key)
#define temp_new_state_key (*key_ptr_new_state_key)
    int ret = -1;

    int num_cards_to_move_from_columns[3];
    num_cards_to_move_from_columns[0] = nc1;
    num_cards_to_move_from_columns[1] = nc2;
    num_cards_to_move_from_columns[2] = -1;

    const int * col_idx = cols_indexes;
    int * col_num_cards = num_cards_to_move_from_columns;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_INSTANCE_GAME_PARAMS(soft_thread->hard_thread->instance);
#endif

#ifdef INDIRECT_STACK_STATES
    char * indirect_stacks_buffer = soft_thread->hard_thread->indirect_stacks_buffer;
#endif


    {
        int dest_fc_idx;

        dest_fc_idx = 0;

        while (1)
        {
            while ((*col_num_cards) == 0)
            {
                col_num_cards++;
                if (*(++col_idx) == -1)
                {
                    return ret;
                }
            }

            /* Find a vacant freecell */
            for( ; dest_fc_idx < LOCAL_FREECELLS_NUM ; dest_fc_idx++)
            {
                if (fcs_freecell_rank(
                        temp_new_state_key, dest_fc_idx
                    ) == 0)
                {
                    break;
                }
            }
            if (dest_fc_idx == LOCAL_FREECELLS_NUM)
            {
                /*  Move on to the stacks. */
                break;
            }

            fcs_cards_column_t new_from_which_col = fcs_state_get_col(temp_new_state_key, *col_idx);

            fcs_card_t top_card;
            fcs_col_pop_card(new_from_which_col, top_card);

            fcs_put_card_in_freecell(
                temp_new_state_key,
                dest_fc_idx,
                top_card
            );

            fcs_internal_move_t temp_move;
            fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
            fcs_int_move_set_src_stack(temp_move,*col_idx);
            fcs_int_move_set_dest_freecell(temp_move,dest_fc_idx);
            fcs_move_stack_push(moves, temp_move);

            ret = dest_fc_idx;

            fcs_flip_top_card(*col_idx);

            (*col_num_cards)--;
            dest_fc_idx++;
        }
    }

    while ((*col_num_cards) == 0)
    {
        col_num_cards++;
        if (*(++col_idx) == -1)
        {
            return ret;
        }
    }

    {
        int put_cards_in_col_idx;

        put_cards_in_col_idx = 0;
        /* Fill the free stacks with the cards below them */
        while (1)
        {
            fcs_cards_column_t new_b_col;

            while ((*col_num_cards) == 0)
            {
                col_num_cards++;
                if (*(++col_idx) == -1)
                {
                    return ret;
                }
            }

            /*  Find a vacant stack */
            for( ; put_cards_in_col_idx < LOCAL_STACKS_NUM ; put_cards_in_col_idx++)
            {
                if (fcs_col_len(
                    fcs_state_get_col(temp_new_state_key, put_cards_in_col_idx)
                    ) == 0)
                {
                    break;
                }
            }

            assert(put_cards_in_col_idx < LOCAL_STACKS_NUM );

            fcs_copy_stack(temp_new_state_key, *(kv_ptr_new_state->val), put_cards_in_col_idx, indirect_stacks_buffer);

            new_b_col = fcs_state_get_col(temp_new_state_key, put_cards_in_col_idx);

            fcs_cards_column_t new_from_which_col = fcs_state_get_col(temp_new_state_key, *col_idx);

            fcs_card_t top_card;
            fcs_col_pop_card(new_from_which_col, top_card);
            fcs_col_push_card(new_b_col, top_card);

            fcs_internal_move_t temp_move;
            fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
            fcs_int_move_set_src_stack(temp_move,*col_idx);
            fcs_int_move_set_dest_stack(temp_move,put_cards_in_col_idx);
            fcs_int_move_set_num_cards_in_seq(temp_move,1);
            fcs_move_stack_push(moves, temp_move);

            ret = (put_cards_in_col_idx | (1 << 8));

            fcs_flip_top_card(*col_idx);

            (*col_num_cards)--;
            put_cards_in_col_idx++;
        }
    }
#undef key_ptr_new_state_key
#undef my_new_out_state_key
#define temp_new_state_key (*key_ptr_new_state_key)
}

#define CALC_POSITIONS_BY_RANK() \
    char * positions_by_rank = \
        fc_solve_get_the_positions_by_rank_data( \
            soft_thread, \
            raw_ptr_state_raw \
        )

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_on_top_of_stacks)
{
    tests_declare_accessors()

    tests_define_accessors();
    tests_define_seqs_built_by();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif

    fcs_game_limit_t num_vacant_freecells = soft_thread->num_vacant_freecells;
    fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks;

    int initial_derived_states_num_states = derived_states_list->num_states;

    CALC_POSITIONS_BY_RANK();

    /* Let's try to put cards in the freecells on top of stacks */

    /* Scan the freecells */
    for (int fc=0 ; fc < LOCAL_FREECELLS_NUM ; fc++)
    {
        fcs_card_t src_card = fcs_freecell_card(state, fc);

        /* If the freecell is not empty and dest_card is its parent
         * */
        if (
                /* The Cell should not be empty. */
                fcs_card_is_valid(src_card)
                &&
                /* We cannot put a king anywhere. */
                (fcs_card_rank(src_card) != 13)
            )
        {

#define FCS_POS_BY_RANK_MAP(x) ((x) << 1)

#ifdef FCS_FREECELL_ONLY

#define FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(card) ((fcs_card_suit(card)^0x1)&(0x2-1))

#define FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_STEP() (2)

#else

#define FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(card) \
            ((sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK) ? 0 \
                : (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT) ? \
                fcs_card_suit(card) : ((fcs_card_suit(card)^0x1)&(0x2-1)) \
            )

#define FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_STEP() \
            ((sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK) ? 1 \
                : (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT) ? \
                4 : 2 \
            )

#endif

#define FCS_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(card) FCS_POS_BY_RANK_MAP(FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(card))
#define FCS_CARD_SUIT_POSITIONS_BY_RANK_STEP() FCS_POS_BY_RANK_MAP(FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_STEP())

#define FCS_POS_IDX_TO_CHECK_START_LOOP(src_card) \
            char * pos_idx_to_check = &positions_by_rank[ \
                (FCS_POS_BY_RANK_WIDTH * (fcs_card_rank(src_card))) \
            ]; \
            char * last_pos_idx; \
                 \
            for (last_pos_idx = pos_idx_to_check + FCS_POS_BY_RANK_WIDTH, \
                 pos_idx_to_check += FCS_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(src_card) \
                ; \
                pos_idx_to_check < last_pos_idx \
                ; \
                pos_idx_to_check += FCS_CARD_SUIT_POSITIONS_BY_RANK_STEP() \
               )

            FCS_POS_IDX_TO_CHECK_START_LOOP(src_card)
            {
                int ds;
                if ((ds = pos_idx_to_check[0]) == -1)
                {
                    continue;
                }
                int dc = pos_idx_to_check[1];

                fcs_cards_column_t dest_col = fcs_state_get_col(state, ds);
                fcs_card_t dest_card = fcs_col_get_card(dest_col, dc);

                int dest_cards_num = fcs_col_len(dest_col);
                /* Let's check if we can put it there */

                /* Check if the destination card is already below a
                 * suitable card */
                fcs_bool_t is_seq_in_dest = FALSE;
                if (dest_cards_num - 1 > dc)
                {
                    fcs_card_t dest_below_card =
                        fcs_col_get_card(dest_col, dc+1);
                    if (fcs_is_parent_card(dest_below_card, dest_card))
                    {
                        is_seq_in_dest = TRUE;
                    }
                }

                if (! is_seq_in_dest)
                {
                    int num_cards_to_relocate = dest_cards_num - dc - 1;

                    int freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                    num_cards_to_relocate -= freecells_to_fill;

                    int freestacks_to_fill;
                    if (tests__is_filled_by_any_card())
                    {
                        freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

                        num_cards_to_relocate -= freestacks_to_fill;
                    }
                    else
                    {
                        freestacks_to_fill = 0;
                    }

                    if (num_cards_to_relocate == 0)
                    {
                        int cols_indexes[3];
                        /* We can move it */
                        fcs_cards_column_t new_dest_col;

                        sfs_check_state_begin()

                        /* Fill the freecells with the top cards */

                        my_copy_stack(ds);

                        cols_indexes[0] = ds;
                        cols_indexes[1] = -1;
                        cols_indexes[2] = -1;

                        empty_two_cols_from_new_state(
                                soft_thread,
                                NEW_STATE_BY_REF(),
                                moves,
                                cols_indexes,
                                dest_cards_num - dc - 1,
                                0
                        );

                        new_dest_col = fcs_state_get_col(new_state, ds);

                        /* Now put the freecell card on top of the stack */
                        fcs_col_push_card(new_dest_col, src_card);
                        fcs_empty_freecell(new_state, fc);

                        fcs_internal_move_t temp_move;
                        fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_STACK);
                        fcs_int_move_set_src_freecell(temp_move,fc);
                        fcs_int_move_set_dest_stack(temp_move,ds);
                        fcs_move_stack_push(moves, temp_move);

                        /*
                         * This is to preserve the order that the
                         * initial (non-optimized) version of the
                         * function used - for backwards-compatibility
                         * and consistency.
                         * */
                        state_context_value =
                            ((ds << 16) | ((255-dc) << 8) | fc)
                            ;

                        sfs_check_state_end()
                    }
                }
            }
        }
    }

    sort_derived_states(derived_states_list, initial_derived_states_num_states);

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_non_top_stack_cards_to_founds)
{
    tests_declare_accessors()

    tests_define_accessors();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif
    fcs_game_limit_t num_vacant_freecells = soft_thread->num_vacant_freecells;
    fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks;

    /* Now let's check if a card that is under some other cards can be placed
     * in the foundations. */

    for (int stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        int cards_num = fcs_col_len(col);
        /*
         * We starts from cards_num-2 because the top card is already covered
         * by move_top_stack_cards_to_founds.
         * */
        for (int c = cards_num-2 ; c >= 0 ; c--)
        {
            fcs_card_t card = fcs_col_get_card(col, c);
            for (int deck = 0 ; deck < INSTANCE_DECKS_NUM ; deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_rank(card)-1)
                {
                    /* The card is foundation-able. Now let's check if we
                     * can move the cards above it to the freecells and
                     * stacks */

                    if ((num_vacant_freecells +
                        ((tests__is_filled_by_any_card()) ?
                            num_vacant_stacks :
                            0
                        ))
                            >= cards_num-(c+1))
                    {
                        int cols_indexes[3];
                        fcs_cards_column_t new_src_col;
                        /* We can move it */

                        sfs_check_state_begin()

                        my_copy_stack(stack_idx);

                        cols_indexes[0] = stack_idx;
                        cols_indexes[1] = -1;
                        cols_indexes[2] = -1;

                        empty_two_cols_from_new_state(
                            soft_thread,
                            NEW_STATE_BY_REF(),
                            moves,
                            cols_indexes,
                            cards_num-(c+1),
                            0
                        );

                        new_src_col = fcs_state_get_col(new_state, stack_idx);

                        fcs_card_t top_card;
                        fcs_col_pop_card(new_src_col, top_card);
                        fcs_increment_foundation(new_state, deck*4+fcs_card_suit(top_card));

                        fcs_internal_move_t temp_move;
                        fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FOUNDATION);
                        fcs_int_move_set_src_stack(temp_move,stack_idx);
                        fcs_int_move_set_foundation(temp_move,deck*4+fcs_card_suit(top_card));

                        fcs_move_stack_push(moves, temp_move);

                        fcs_flip_top_card(stack_idx);

                        sfs_check_state_end()
                    }
                    break;
                }
            }
        }
    }

    return;
}


DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_stack_cards_to_a_parent_on_the_same_stack)
{
    tests_declare_accessors()
    tests_define_accessors();
    tests_define_seqs_built_by();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    fcs_game_limit_t num_vacant_freecells = soft_thread->num_vacant_freecells;
    fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks;

    /*
     * Now let's try to move a stack card to a parent card which is found
     * on the same stack.
     * */
    for (int stack_idx = 0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        int cards_num = fcs_col_len(col);

        for (int c=0 ; c<cards_num ; c++)
        {
            /* Find a card which this card can be put on; */

            fcs_card_t card = fcs_col_get_card(col, c);

            /* Do not move cards that are already found above a suitable
             * parent */
            /* TODO : is this code safe for variants that are not Freecell? */
            fcs_bool_t should_perform_move = TRUE;
            if (c != 0)
            {
                fcs_card_t prev_card = fcs_col_get_card(col, c-1);
                if ((fcs_card_rank(prev_card) == fcs_card_rank(card)+1) &&
                    ((fcs_card_suit(prev_card) & 0x1) != (fcs_card_suit(card) & 0x1)))
                {
                   should_perform_move = FALSE;
                }
            }
            if (should_perform_move)
            {
#define ds stack_idx
#define dest_col col
#define dest_cards_num cards_num
                /* Check if it can be moved to something on the same stack */
                for (int dc = 0 ; dc < c-1 ; dc++)
                {
                    fcs_card_t dest_card = fcs_col_get_card(dest_col, dc);
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* Corresponding cards - see if it is feasible to move
                           the source to the destination. */

                        fcs_bool_t is_seq_in_dest = FALSE;
                        fcs_card_t dest_below_card = fcs_col_get_card(dest_col, dc+1);
                        if (fcs_is_parent_card(dest_below_card, dest_card))
                        {
                            is_seq_in_dest = TRUE;
                        }

                        if (!is_seq_in_dest)
                        {
                            int num_cards_to_relocate = dest_cards_num - dc - 1;

                            int freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                            num_cards_to_relocate -= freecells_to_fill;

                            int freestacks_to_fill;
                            if (tests__is_filled_by_any_card())
                            {
                                freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

                                num_cards_to_relocate -= freestacks_to_fill;
                            }
                            else
                            {
                                freestacks_to_fill = 0;
                            }

                            if (num_cards_to_relocate == 0)
                            {
                                /* We can move it */
                                sfs_check_state_begin()

                                my_copy_stack(ds);

                                fcs_cards_column_t new_dest_col = fcs_state_get_col(new_state, ds);

                                int cols_indexes[3];
                                cols_indexes[0] = ds;
                                cols_indexes[1] = -1;
                                cols_indexes[2] = -1;

                                int last_dest = empty_two_cols_from_new_state(
                                    soft_thread,
                                    NEW_STATE_BY_REF(),
                                    moves,
                                    cols_indexes,
                                    /* We're moving one extra card */
                                    cards_num - c,
                                    0
                                );

                                int source_index = last_dest & 0xFF;

                                empty_two_cols_from_new_state(
                                    soft_thread,
                                    NEW_STATE_BY_REF(),
                                    moves,
                                    cols_indexes,
                                    c - dc - 1,
                                    0
                                );

                                fcs_card_t moved_card;
                                if (!( (last_dest >> 8)&0x1))
                                {
                                    moved_card = fcs_freecell_card(new_state, source_index);
                                    fcs_empty_freecell(new_state, source_index);


                                    fcs_internal_move_t temp_move;
                                    fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_STACK);
                                    fcs_int_move_set_src_freecell(temp_move,source_index);
                                    fcs_int_move_set_dest_stack(temp_move,ds);
                                    fcs_move_stack_push(moves, temp_move);
                                }
                                else
                                {
                                    fcs_cards_column_t new_source_col;

                                    new_source_col = fcs_state_get_col(new_state, source_index);

                                    fcs_col_pop_card(new_source_col, moved_card);

                                    fcs_internal_move_t temp_move;
                                    fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                    fcs_int_move_set_src_stack(temp_move,source_index);
                                    fcs_int_move_set_dest_stack(temp_move,ds);
                                    fcs_int_move_set_num_cards_in_seq(temp_move,1);
                                    fcs_move_stack_push(moves, temp_move);
                                }

                                fcs_col_push_card(new_dest_col, moved_card);

                                sfs_check_state_end()
                            }
                        }
                    }
                }
            }
        }
    }

    return;
}
#undef ds
#undef dest_col
#undef dest_cards_num


DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_stack_cards_to_different_stacks)
{
    tests_declare_accessors()

    const fcs_game_limit_t num_vacant_freecells
        = soft_thread->num_vacant_freecells;
    const fcs_game_limit_t num_vacant_stacks
         = soft_thread->num_vacant_stacks;

    tests_define_accessors();
    tests_define_seqs_built_by();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif

    const int initial_derived_states_num_states =
        derived_states_list->num_states;

    CALC_POSITIONS_BY_RANK();

    /* Now let's try to move a card from one stack to the other     *
     * Note that it does not involve moving cards lower than king   *
     * to empty stacks                                              */

    for (int stack_idx = 0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        const fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);

        int seq_end;
        for (int c=0 ; c<fcs_col_len(col) ; c=seq_end+1)
        {
            /* Check if there is a sequence here. */
            for(seq_end=c ; seq_end<fcs_col_len(col)-1 ; seq_end++)
            {
                const fcs_card_t this_card = fcs_col_get_card(col, seq_end+1);
                const fcs_card_t prev_card = fcs_col_get_card(col, seq_end);

                if (fcs_is_parent_card(this_card,prev_card))
                {
                }
                else
                {
                    break;
                }
            }

            if (tests__should_not_empty_columns())
            {
                if (c == 0)
                {
                    continue;
                }
            }

            /* Find a card which this card can be put on; */

            fcs_card_t card = fcs_col_get_card(col, c);

#ifndef FCS_WITHOUT_CARD_FLIPPING
            /* Make sure the card is not flipped or else we can't move it */
            if (fcs_card_get_flipped(card))
            {
                continue;
            }
#endif

            /* Skip if it's a King - nothing to put it on. */
            if (unlikely(fcs_card_rank(card) == 13))
            {
                continue;
            }

            FCS_POS_IDX_TO_CHECK_START_LOOP(card)
            {
                const int ds = pos_idx_to_check[0];

                if ((ds >= 0) && (ds != stack_idx))
                {
                const int dc = pos_idx_to_check[1];
                const fcs_cards_column_t dest_col = fcs_state_get_col(state, ds);

                int num_cards_to_relocate = fcs_col_len(dest_col) - dc - 1 + fcs_col_len(col) - seq_end - 1;

                const int freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                num_cards_to_relocate -= freecells_to_fill;

                int freestacks_to_fill;
                if (tests__is_filled_by_any_card())
                {
                    freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

                    num_cards_to_relocate -= freestacks_to_fill;
                }
                else
                {
                    freestacks_to_fill = 0;
                }

                if (unlikely((num_cards_to_relocate == 0) &&
                   (calc_max_sequence_move(num_vacant_freecells-freecells_to_fill, num_vacant_stacks-freestacks_to_fill) >=
                    seq_end - c + 1)))
                {
                    sfs_check_state_begin()

                    my_copy_stack(stack_idx);
                    my_copy_stack(ds);

                    int cols_indexes[3];
                    cols_indexes[0] = ds;
                    cols_indexes[1] = stack_idx;
                    cols_indexes[2] = -1;

                    empty_two_cols_from_new_state(
                        soft_thread,
                        NEW_STATE_BY_REF(),
                        moves,
                        cols_indexes,
                        fcs_col_len(dest_col) - dc - 1,
                        fcs_col_len(col) - seq_end - 1
                    );

                    fcs_move_sequence(ds, stack_idx, c, seq_end);

                    fcs_flip_top_card(stack_idx);

                    /*
                     * This is to preserve the order that the
                     * initial (non-optimized) version of the
                     * function used - for backwards-compatibility
                     * and consistency.
                     * */
                    state_context_value = ((((((stack_idx << 8) | c) << 8) | ds) << 8) | dc);

                    sfs_check_state_end()
                }
                }
            }
        }
    }

    sort_derived_states(derived_states_list, initial_derived_states_num_states);

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_sequences_to_free_stacks)
{
    tests_declare_accessors()
    tests_define_accessors();
    tests_define_empty_stacks_fill();
    tests_define_seqs_built_by();

    if (tests__is_filled_by_none())
    {
        return;
    }

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif
    const fcs_game_limit_t num_vacant_freecells = soft_thread->num_vacant_freecells;
    const fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks;

    const int max_sequence_len = calc_max_sequence_move(num_vacant_freecells, num_vacant_stacks-1);

    /* Now try to move sequences to empty stacks */

    if (num_vacant_stacks > 0)
    {
        int dest_stack_idx = -1;
        for (int stack_idx = 0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
        {
            fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
            int cards_num = fcs_col_len(col);

            int seq_end;
            for (int c=0; c < cards_num; c = seq_end+1)
            {
                /* Check if there is a sequence here. */
                for(seq_end=c ; seq_end<cards_num-1; seq_end++)
                {
                    fcs_card_t this_card = fcs_col_get_card(col, seq_end+1);
                    fcs_card_t prev_card = fcs_col_get_card(col, seq_end);

                    if (! fcs_is_parent_card(this_card, prev_card))
                    {
                        break;
                    }
                }

                if ((fcs_col_get_rank(col, c) != 13) &&
                    (tests__is_filled_by_kings_only()))
                {
                    continue;
                }

                if (seq_end == cards_num -1)
                {
                    /* One stack is the destination stack, so we have one     *
                     * less stack in that case                                */
                    while ((max_sequence_len < cards_num -c) && (c > 0))
                    {
                        c--;
                    }

                    if (
                        (c > 0) &&
                        ((tests__is_filled_by_kings_only()) ?
                            (fcs_col_get_rank(col, c) == 13) :
                            1
                        )
                       )
                    {
                        sfs_check_state_begin();

                        if (dest_stack_idx < 0)
                        {
                            for (dest_stack_idx = 0 ;
                                 dest_stack_idx < LOCAL_STACKS_NUM;
                                 dest_stack_idx++)
                            {
                                if (fcs_col_len(
                                    fcs_state_get_col(new_state, dest_stack_idx)
                                    ) == 0)
                                {
                                    break;
                                }
                            }
                        }

                        my_copy_stack(dest_stack_idx);
                        my_copy_stack(stack_idx);

                        fcs_move_sequence(
                            dest_stack_idx,
                            stack_idx, c, cards_num-1
                        );

                        sfs_check_state_end()
                    }
                }
                else
                {
                    int num_cards_to_relocate = cards_num - seq_end - 1;

                    const int freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                    num_cards_to_relocate -= freecells_to_fill;

                    int freestacks_to_fill;
                    if (tests__is_filled_by_any_card())
                    {
                        freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

                        num_cards_to_relocate -= freestacks_to_fill;
                    }
                    else
                    {
                        freestacks_to_fill = 0;
                    }

                    if ((num_cards_to_relocate == 0) && (num_vacant_stacks-freestacks_to_fill > 0))
                    {
                        /* We can move it */
                        int seq_start = c;
                        while (
                            (calc_max_sequence_move(
                                num_vacant_freecells-freecells_to_fill,
                                num_vacant_stacks-freestacks_to_fill-1) < seq_end-seq_start+1)
                                &&
                            (seq_start <= seq_end)
                            )
                        {
                            seq_start++;
                        }
                        if ((seq_start <= seq_end) &&
                            ((tests__is_filled_by_kings_only()) ?
                                (fcs_col_get_rank(col, seq_start)
                                    == 13) :
                                1
                            )
                        )
                        {
                            int cols_indexes[3];
                            int empty_ret;

                            sfs_check_state_begin();

                            /* Fill the freecells with the top cards */

                            my_copy_stack(stack_idx);

                            cols_indexes[0] = stack_idx;
                            cols_indexes[1] = -1;
                            cols_indexes[2] = -1;

                            empty_ret = empty_two_cols_from_new_state(
                                soft_thread,
                                NEW_STATE_BY_REF(),
                                moves,
                                cols_indexes,
                                freecells_to_fill + freestacks_to_fill,
                                0
                            );

                            int b;
                            /* Find a vacant stack */
                            for (
                                    b=(
                                        ((empty_ret >> 8)&0x1)
                                        ? (empty_ret&0xFF) + 1
                                        : 0
                                      )
                                    ;
                                    b < LOCAL_STACKS_NUM
                                    ;
                                    b++
                                    )
                            {
                                if (fcs_col_len(
                                    fcs_state_get_col(new_state, b)
                                    ) == 0)
                                {
                                    break;
                                }
                            }

                            my_copy_stack(b);

                            fcs_move_sequence(b, stack_idx, seq_start, seq_end);

                            sfs_check_state_end();
                        }
                    }
                }
            }
        }
    }

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_to_empty_stack)
{
    tests_declare_accessors()

    /* Let's try to put cards that occupy freecells on an empty stack */

    tests_define_accessors();
    tests_define_empty_stacks_fill();

    if (tests__is_filled_by_none())
    {
        return;
    }

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    if (soft_thread->num_vacant_stacks)
    {
        int stack_idx;
        for (stack_idx = 0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
        {
            if (fcs_col_len(
                fcs_state_get_col(state, stack_idx)
                ) == 0)
            {
                break;
            }
        }

        for (int fc = 0 ; fc < LOCAL_FREECELLS_NUM ; fc++)
        {
            fcs_card_t card = fcs_freecell_card(state, fc);
            if (
                (tests__is_filled_by_kings_only())
                ? (fcs_card_rank(card) == 13)
                : fcs_card_is_valid(card)
               )
            {
                fcs_cards_column_t new_src_col;
                /* We can move it */

                sfs_check_state_begin();

                my_copy_stack(stack_idx);

                new_src_col = fcs_state_get_col(new_state, stack_idx);

                fcs_col_push_card(new_src_col, card);
                fcs_empty_freecell(new_state, fc);

                fcs_internal_move_t temp_move;
                fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_STACK);
                fcs_int_move_set_src_freecell(temp_move,fc);
                fcs_int_move_set_dest_stack(temp_move,stack_idx);
                fcs_move_stack_push(moves, temp_move);

                sfs_check_state_end()
            }
        }
    }

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_cards_to_a_different_parent)
{
    tests_declare_accessors()
    tests_define_accessors();
    tests_define_seqs_built_by();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif
    const fcs_game_limit_t num_vacant_freecells = soft_thread->num_vacant_freecells;
    const fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks;

    const int initial_derived_states_num_states = derived_states_list->num_states;

    CALC_POSITIONS_BY_RANK();

    /* This time try to move cards that are already on top of a parent to a different parent */

    for (int stack_idx = 0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        int cards_num = fcs_col_len(col);

        /*
         * If there's only one card in the column, then it won't be above a
         * parent, so there's no sense in moving it.
         *
         * If there are no cards in the column, then there's nothing to do
         * here, and the algorithm will be confused
         * */
        if (cards_num < 2)
        {
            continue;
        }

        fcs_card_t upper_card = fcs_col_get_card(col, cards_num-1);

        /*
         * min_card_height is the minimal height of the card that is above
         * a true parent.
         *
         * It must be:
         *
         * 1. >= 1 - because the height 0 should not be moved.
         *
         * 2. <= cards_num-1 - because the height 0
         */
        int min_card_height;
        fcs_card_t lower_card;
        for (min_card_height = cards_num-2
            ; min_card_height >= 0
            ; upper_card = lower_card, min_card_height--
            )
        {
            lower_card = fcs_col_get_card(col, min_card_height);
            if (! fcs_is_parent_card(upper_card, lower_card))
            {
                break;
            }
        }

        min_card_height += 2;

        for(int c=min_card_height ; c < cards_num ; c++)
        {
            /* Find a card which this card can be put on; */

            fcs_card_t card = fcs_col_get_card(col, c);

#ifndef FCS_WITHOUT_CARD_FLIPPING
            /*
             * Do not move cards that are flipped.
             * */
            if (fcs_card_get_flipped(card))
            {
                continue;
            }
#endif

            FCS_POS_IDX_TO_CHECK_START_LOOP(card)
            {
                const int ds = pos_idx_to_check[0];
                if ((ds == -1) || (ds == stack_idx))
                {
                    continue;
                }
                const int dc = pos_idx_to_check[1];

                fcs_cards_column_t dest_col = fcs_state_get_col(state, ds);
                int dest_cards_num = fcs_col_len(dest_col);
                fcs_card_t dest_card = fcs_col_get_card(dest_col, dc);

                /* Corresponding cards - see if it is feasible to move
                   the source to the destination. */

                /* Don't move if there's a sequence of cards in the
                 * destination.
                 * */
                if ((dc + 1 < dest_cards_num)
                        &&
                    fcs_is_parent_card(
                        fcs_col_get_card(dest_col, dc+1),
                        dest_card
                    )
                   )
                {
                    continue;
                }

                int num_cards_to_relocate = dest_cards_num - dc - 1;

                const int freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                num_cards_to_relocate -= freecells_to_fill;

                int freestacks_to_fill;
                if (tests__is_filled_by_any_card())
                {
                    freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

                    num_cards_to_relocate -= freestacks_to_fill;
                }
                else
                {
                    freestacks_to_fill = 0;
                }

                if (!(
                    (num_cards_to_relocate == 0)
                    && (calc_max_sequence_move(
                            num_vacant_freecells - freecells_to_fill,
                            num_vacant_stacks - freestacks_to_fill
                            )
                            >=
                        cards_num - c
                       )
                    ))
                {
                    continue;
                }

                /* We can move it */

                {
                    int cols_indexes[3];

                    sfs_check_state_begin()

                    /* Fill the freecells with the top cards */

                    my_copy_stack(ds);

                    cols_indexes[0] = ds;
                    cols_indexes[1] = -1;
                    cols_indexes[2] = -1;

                    empty_two_cols_from_new_state(
                        soft_thread,
                        NEW_STATE_BY_REF(),
                        moves,
                        cols_indexes,
                        freestacks_to_fill + freecells_to_fill,
                        0
                    );

                    my_copy_stack(stack_idx);

                    fcs_move_sequence(ds, stack_idx, c, cards_num-1);

                    state_context_value =
                        ((((((stack_idx << 8) | c) << 8) | ds) << 8) | dc);

                    sfs_check_state_end()
                }
            }
        }
    }

    sort_derived_states(derived_states_list, initial_derived_states_num_states);

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_empty_stack_into_freecells)
{
    tests_declare_accessors()
    tests_define_accessors();
    tests_define_empty_stacks_fill();

    if (tests__is_filled_by_none())
    {
        return;
    }

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    const fcs_game_limit_t num_vacant_freecells = soft_thread->num_vacant_freecells;
    const fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks;

    /* Now, let's try to empty an entire stack into the freecells, so other cards can
     * inhabit it */

    if (num_vacant_stacks == 0)
    {
        for (int stack_idx = 0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
        {
            fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
            int cards_num = fcs_col_len(col);

            if (cards_num <= num_vacant_freecells)
            {
                /* We can empty it */
                sfs_check_state_begin()

                my_copy_stack(stack_idx);

                fcs_cards_column_t new_src_col = fcs_state_get_col(new_state, stack_idx);

                int b = 0;
                for (int c = 0 ; c < cards_num ; c++, b++)
                {
                    /* Find a vacant freecell */
                    for ( ; b < LOCAL_FREECELLS_NUM ; b++)
                    {
                        if (fcs_freecell_is_empty(new_state, b))
                        {
                            break;
                        }
                    }
                    fcs_card_t top_card;
                    fcs_col_pop_card(new_src_col, top_card);

                    fcs_put_card_in_freecell(new_state, b, top_card);

                    fcs_internal_move_t temp_move;
                    fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                    fcs_int_move_set_src_stack(temp_move,stack_idx);
                    fcs_int_move_set_dest_freecell(temp_move,b);
                    fcs_move_stack_push(moves, temp_move);
                }

                sfs_check_state_end()
            }
        }
    }

    return;

}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_yukon_do_nothing)
{
    return;
}

/* Disabling Yukon solving for the time being. */
#if 0
DECLARE_MOVE_FUNCTION(fc_solve_sfs_yukon_move_card_to_parent)
{
    tests_declare_accessors()


    int stack_idx, cards_num, c, ds;
    int dest_cards_num;
    fcs_card_t card;
    fcs_card_t dest_card;
    fcs_cards_column_t dest_col, col;

#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif

    fcs_internal_move_t temp_move;

    tests_define_accessors();

    temp_move = fc_solve_empty_move;

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = INSTANCE_STACKS_NUM;
#endif

    for( ds=0 ; ds < LOCAL_STACKS_NUM ; ds++ )
    {
        dest_col = fcs_state_get_col(state, ds);
        dest_cards_num = fcs_col_len(dest_col);
        if (dest_cards_num > 0)
        {
            dest_card = fcs_col_get_card(dest_col, dest_cards_num-1);
            for( stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
            {
                if (stack_idx == ds)
                {
                    continue;
                }
                col = fcs_state_get_col(state, stack_idx);
                cards_num = fcs_col_len(col);
                for( c=cards_num-1 ; c >= 0 ; c--)
                {
                    card = fcs_col_get_card(col, c);
#ifndef FCS_WITHOUT_CARD_FLIPPING
                    if (fcs_card_get_flipped(card))
                    {
                        break;
                    }
#endif
                    if (fcs_is_parent_card(card, dest_card))
                    {

                        /* We can move it there - now let's check to see
                         * if it is already above a suitable parent. */
                        if ((c == 0) ||
                            (! fcs_is_parent_card(card, fcs_col_get_card(col, c-1))))
                        {
                            /* Let's move it */
                            sfs_check_state_begin();

                            my_copy_stack(stack_idx);
                            my_copy_stack(ds);

                            fcs_move_sequence(ds, stack_idx, c, cards_num-1);

                            fcs_flip_top_card(stack_idx);

                            sfs_check_state_end();
                        }

                    }
                }
            }
        }
    }

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_yukon_move_kings_to_empty_stack)
{
    tests_declare_accessors()


    int stack_idx, cards_num, c, ds;
    fcs_card_t card;
    fcs_cards_column_t col;

#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    fcs_game_limit_t num_vacant_stacks;

    fcs_internal_move_t temp_move;

    tests_define_accessors();

    num_vacant_stacks = soft_thread->num_vacant_stacks;
    temp_move = fc_solve_empty_move;

    if (num_vacant_stacks == 0)
    {
        return;
    }

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = INSTANCE_STACKS_NUM;
#endif

    for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
    {
        if (fcs_col_len(
            fcs_state_get_col(state, ds)
            ) == 0)
        {
            break;
        }
    }

    for( stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        col = fcs_state_get_col(state, stack_idx);
        cards_num = fcs_col_len(col);
        for( c=cards_num-1 ; c >= 1 ; c--)
        {
            card = fcs_col_get_card(col, c);
#ifndef FCS_WITHOUT_CARD_FLIPPING
            if (fcs_card_get_flipped(card))
            {
                break;
            }
#endif
            if (fcs_card_rank(card) == 13)
            {
                /* It's a King - so let's move it */
                sfs_check_state_begin();

                my_copy_stack(stack_idx);
                my_copy_stack(ds);
                fcs_move_sequence(ds, stack_idx, c, cards_num-1);

                fcs_flip_top_card(stack_idx);

                sfs_check_state_end();
            }
        }
    }

    return;
}
#endif

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_empty_stack)
{
    tests_declare_accessors()
    tests_define_accessors();
    tests_define_empty_stacks_fill();

    if (tests__is_filled_by_none())
    {
        return;
    }

    const fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks;

    if (num_vacant_stacks == 0)
    {
        return;
    }

#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    int empty_stack_idx;
    for (empty_stack_idx = 0 ; empty_stack_idx < LOCAL_STACKS_NUM ; empty_stack_idx++)
    {
        if (fcs_col_len(
            fcs_state_get_col(state, empty_stack_idx)
            ) == 0)
        {
            break;
        }
    }

    for (int stack_idx = 0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        int cards_num = fcs_col_len(col);

        /* Bug fix: if there's only one card in a column, there's no
         * point moving it to a new empty column.
         * */
        if (cards_num > 1)
        {
            fcs_card_t card = fcs_col_get_card(col, cards_num-1);
            if (tests__is_filled_by_kings_only() &&
                (fcs_card_rank(card) != 13))
            {
                continue;
            }
            /* Let's move it */
            {
                sfs_check_state_begin();

                my_copy_stack(stack_idx);

                fcs_cards_column_t new_src_col = fcs_state_get_col(new_state, stack_idx);

                fcs_col_pop_top(new_src_col);

                my_copy_stack(empty_stack_idx);

                fcs_cards_column_t empty_stack_col = fcs_state_get_col(new_state, empty_stack_idx);
                fcs_col_push_card(empty_stack_col, card);

                fcs_internal_move_t temp_move;
                fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_STACK_TO_STACK);
                fcs_int_move_set_src_stack(temp_move, stack_idx);
                fcs_int_move_set_dest_stack(temp_move, empty_stack_idx);
                fcs_int_move_set_num_cards_in_seq(temp_move, 1);

                fcs_move_stack_push(moves, temp_move);

                sfs_check_state_end()
            }
        }
    }

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_parent)
{
    tests_declare_accessors()
    tests_define_accessors();
    tests_define_seqs_built_by();
    tests_define_empty_stacks_fill();

#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    const int num_cards_in_col_threshold = tests__should_not_empty_columns() ? 1 : 0;

    for (int stack_idx = 0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        int cards_num = fcs_col_len(col);

        if (cards_num > num_cards_in_col_threshold)
        {
            fcs_card_t card = fcs_col_get_card(col, cards_num-1);

            for (int ds = 0 ; ds < LOCAL_STACKS_NUM ; ds++)
            {
                if (ds == stack_idx)
                {
                    continue;
                }

                fcs_cards_column_t dest_col = fcs_state_get_col(state, ds);

                if (fcs_col_len(dest_col) > 0)
                {
                    fcs_card_t dest_card = fcs_col_get_card(dest_col,
                            fcs_col_len(dest_col)-1);
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* Let's move it */
                        {
                            sfs_check_state_begin();

                            my_copy_stack(stack_idx);
                            my_copy_stack(ds);

                            fcs_cards_column_t new_src_col = fcs_state_get_col(new_state, stack_idx);
                            fcs_cards_column_t new_dest_col = fcs_state_get_col(new_state, ds);

                            fcs_col_pop_top(new_src_col);

                            fcs_col_push_card(new_dest_col, card);

                            fcs_internal_move_t temp_move;
                            fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_STACK_TO_STACK);
                            fcs_int_move_set_src_stack(temp_move, stack_idx);
                            fcs_int_move_set_dest_stack(temp_move, ds);
                            fcs_int_move_set_num_cards_in_seq(temp_move, 1);

                            fcs_move_stack_push(moves, temp_move);

                            sfs_check_state_end()
                        }
                    }
                }
            }
        }
    }

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_freecell)
{
    tests_declare_accessors()
    tests_define_accessors();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    const fcs_game_limit_t num_vacant_freecells = soft_thread->num_vacant_freecells;

    if (num_vacant_freecells == 0)
    {
        return;
    }

    const int num_cards_in_col_threshold = tests__should_not_empty_columns() ? 1 : 0;

    int ds;
    for (ds = 0 ; ds < LOCAL_FREECELLS_NUM ; ds++)
    {
        if (fcs_freecell_is_empty(state, ds))
        {
            break;
        }
    }

    for (int stack_idx = 0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        int cards_num = fcs_col_len(col);
        if (cards_num > num_cards_in_col_threshold)
        {
            fcs_card_t card = fcs_col_get_card(col, cards_num-1);

            /* Let's move it */
            {
                sfs_check_state_begin();

                my_copy_stack(stack_idx);
                fcs_cards_column_t new_src_col =
                    fcs_state_get_col(new_state, stack_idx);

                fcs_col_pop_top(new_src_col);

                fcs_put_card_in_freecell(new_state, ds, card);

                fcs_internal_move_t temp_move;
                fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_STACK_TO_FREECELL);
                fcs_int_move_set_src_stack(temp_move, stack_idx);
                fcs_int_move_set_dest_freecell(temp_move, ds);

                fcs_move_stack_push(moves, temp_move);

                sfs_check_state_end()
            }
        }
    }

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_freecell_card_to_parent)
{
    tests_declare_accessors()
    tests_define_accessors();
    tests_define_seqs_built_by();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    for (int fc = 0 ; fc < LOCAL_FREECELLS_NUM ; fc++)
    {
        fcs_card_t card = fcs_freecell_card(state, fc);
        if (fcs_card_is_empty(card))
        {
            continue;
        }

        for (int ds = 0 ; ds < LOCAL_STACKS_NUM ; ds++)
        {
            fcs_cards_column_t dest_col = fcs_state_get_col(state, ds);
            if (fcs_col_len(dest_col) > 0)
            {
                fcs_card_t dest_card = fcs_col_get_card(dest_col, fcs_col_len(dest_col)-1);
                if (fcs_is_parent_card(card, dest_card))
                {
                    /* Let's move it */
                    sfs_check_state_begin();

                    my_copy_stack(ds);

                    fcs_cards_column_t new_dest_col = fcs_state_get_col(new_state, ds);

                    fcs_empty_freecell(new_state, fc);

                    fcs_col_push_card(new_dest_col, card);

                    fcs_internal_move_t temp_move;
                    fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_FREECELL_TO_STACK);
                    fcs_int_move_set_src_freecell(temp_move, fc);
                    fcs_int_move_set_dest_stack(temp_move, ds);
                    fcs_int_move_set_num_cards_in_seq(temp_move, 1);

                    fcs_move_stack_push(moves, temp_move);

                    sfs_check_state_end()
                }
            }
        }
    }

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_freecell_card_to_empty_stack)
{
    tests_declare_accessors()
    tests_define_accessors();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    if (tests__is_filled_by_none())
    {
        return;
    }

    const fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks;

    if (num_vacant_stacks == 0)
    {
        return;
    }

    /* Find a vacant stack */
    int ds;
    for (ds = 0 ; ds < LOCAL_STACKS_NUM ; ds++)
    {
        if (fcs_col_len(
            fcs_state_get_col(state, ds)
            ) == 0)
        {
            break;
        }
    }

    for (int fc = 0 ; fc < LOCAL_FREECELLS_NUM ; fc++)
    {
        fcs_card_t card = fcs_freecell_card(state, fc);
        if (fcs_card_is_empty(card) ||
            (tests__is_filled_by_kings_only() &&
             (fcs_card_rank(card) != 13))
           )
        {
            continue;
        }

        {
            sfs_check_state_begin();

            my_copy_stack(ds);

            fcs_cards_column_t new_dest_col = fcs_state_get_col(new_state, ds);

            fcs_empty_freecell(new_state, fc);

            fcs_col_push_card(new_dest_col, card);

            fcs_internal_move_t temp_move;
            fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_FREECELL_TO_STACK);
            fcs_int_move_set_src_freecell(temp_move, fc);
            fcs_int_move_set_dest_stack(temp_move, ds);
            fcs_int_move_set_num_cards_in_seq(temp_move, 1);

            fcs_move_stack_push(moves, temp_move);

            sfs_check_state_end()
        }
    }

    return;
}

#define CALC_FOUNDATION_TO_PUT_CARD_ON__STATE_PARAMS() pass_new_state.key, card

#define CALC_FOUNDATION_TO_PUT_CARD_ON() calc_foundation_to_put_card_on(soft_thread, CALC_FOUNDATION_TO_PUT_CARD_ON__STATE_PARAMS())

#ifdef FCS_FREECELL_ONLY
#define SEQS_ARE_BUILT_BY_RANK() FALSE
#else
#define SEQS_ARE_BUILT_BY_RANK() (sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK)
#endif

static GCC_INLINE int calc_foundation_to_put_card_on(
    fc_solve_soft_thread_t * soft_thread,
    fcs_state_t * my_ptr_state,
    fcs_card_t card
)
{
#ifndef FCS_FREECELL_ONLY
    fc_solve_instance_t * instance = soft_thread->hard_thread->instance;
#endif

    tests_define_seqs_built_by();

    for (int deck = 0 ; deck < INSTANCE_DECKS_NUM ; deck++)
    {
        if (fcs_foundation_value(*my_ptr_state, (deck<<2)+fcs_card_suit(card)) == fcs_card_rank(card) - 1)
        {
            int ret_val = (deck<<2)+fcs_card_suit(card);
            /* Always put on the foundation if it is built-by-suit */
#ifndef FCS_FREECELL_ONLY
            if (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT)
            {
                return ret_val;
            }
#endif

            int other_deck_idx;
            for (other_deck_idx = 0 ; other_deck_idx < (INSTANCE_DECKS_NUM << 2) ; other_deck_idx++)
            {
                if (fcs_foundation_value(*my_ptr_state, other_deck_idx)
                        < fcs_card_rank(card) - 2 -
                        (
                            SEQS_ARE_BUILT_BY_RANK()
                            ? 0
                            : ((other_deck_idx&0x1) == (fcs_card_suit(card)&0x1))
                        )
                   )
                {
                    break;
                }
            }
            if (other_deck_idx == (INSTANCE_DECKS_NUM << 2))
            {
                return ret_val;
            }
        }
    }

    return -1;
}

extern int fc_solve_sfs_raymond_prune(
    fc_solve_soft_thread_t * soft_thread,
    fcs_kv_state_t * raw_ptr_state_raw,
    fcs_collectible_state_t * * ptr_next_state_val
)
{
    tests_declare_accessors()
    tests_define_accessors();
#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    fcs_derived_states_list_t derived_states_list_struct;
    derived_states_list_struct.states = NULL;
    derived_states_list_struct.num_states = 0;

    sfs_check_state_begin();

    int num_total_cards_moved = 0;
    int num_cards_moved = 0;
    do {
        num_cards_moved = 0;
        for ( int stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
        {
            fcs_cards_column_t col = fcs_state_get_col(new_state, stack_idx);
            const int cards_num = fcs_col_len(col);

            if (cards_num)
            {
                /* Get the top card in the stack */
                const fcs_card_t card = fcs_col_get_card(col, cards_num-1);

                const int dest_foundation = CALC_FOUNDATION_TO_PUT_CARD_ON();
                if (dest_foundation >= 0)
                {
                    /* We can safely move it. */
                    num_cards_moved++;

                    my_copy_stack(stack_idx);
                    {
                        fcs_cards_column_t new_temp_col;
                        new_temp_col = fcs_state_get_col(new_state, stack_idx);
                        fcs_col_pop_top(new_temp_col);
                    }

                    fcs_increment_foundation(new_state, dest_foundation);

                    fcs_internal_move_t temp_move;
                    fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FOUNDATION);
                    fcs_int_move_set_src_stack(temp_move,stack_idx);
                    fcs_int_move_set_foundation(temp_move,dest_foundation);

                    fcs_move_stack_push(moves, temp_move);

                    fcs_flip_top_card(stack_idx);
                }
            }
        }

        /* Now check the same for the free cells */
        for ( int fc=0 ; fc < LOCAL_FREECELLS_NUM ; fc++)
        {
            const fcs_card_t card = fcs_freecell_card(new_state, fc);
            if (fcs_card_is_valid(card))
            {
                const int dest_foundation = CALC_FOUNDATION_TO_PUT_CARD_ON();
                if (dest_foundation >= 0)
                {
                    num_cards_moved++;

                    /* We can put it there */

                    fcs_empty_freecell(new_state, fc);

                    fcs_internal_move_t temp_move;
                    fcs_increment_foundation(new_state, dest_foundation);

                    fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION);
                    fcs_int_move_set_src_freecell(temp_move,fc);
                    fcs_int_move_set_foundation(temp_move,dest_foundation);

                    fcs_move_stack_push(moves, temp_move);
                }
            }
        }
        num_total_cards_moved += num_cards_moved;
    } while (num_cards_moved);

#define derived_states_list (&derived_states_list_struct)
    sfs_check_state_end();
#undef derived_states_list

    {
        register int ret_code;

        if (num_total_cards_moved)
        {
            register fcs_collectible_state_t * ptr_next_state;

            *ptr_next_state_val
                = ptr_next_state
                = derived_states_list_struct.states[0].state_ptr;

            /*
             * Set the GENERATED_BY_PRUNING flag uncondtionally. It won't
             * hurt if it's already there, and if it's a state that was
             * found by other means, we still shouldn't prune it, because
             * it is already "prune-perfect".
             * */
            FCS_S_VISITED(ptr_next_state) |= FCS_VISITED_GENERATED_BY_PRUNING;

            ret_code = PRUNE_RET_FOLLOW_STATE;
        }
        else
        {
            *ptr_next_state_val = NULL;
            ret_code = PRUNE_RET_NOT_FOUND;
        }

        free(derived_states_list_struct.states);

        return ret_code;
    }
}
