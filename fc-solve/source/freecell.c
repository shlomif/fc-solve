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
int fc_solve_sfs_move_top_stack_cards_to_founds(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()
    int stack_idx;
    fcs_cards_column_t col;
    int cards_num;
    int deck;
    fcs_card_t card;
    int check;
#ifndef HARD_CODED_NUM_STACKS
    DECLARE_GAME_PARAMS();
#endif

    fcs_internal_move_t temp_move;

    tests_define_accessors();

#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    temp_move = fc_solve_empty_move;

    for( stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        col = fcs_state_get_col(state, stack_idx);
        cards_num = fcs_col_len(col);
        if (cards_num)
        {
            /* Get the top card in the stack */
            card = fcs_col_get_card(col, cards_num-1);
            for(deck=0;deck < INSTANCE_DECKS_NUM;deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_card_num(card) - 1)
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

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


/*
 * This test moves single cards that are present in the freecells to
 * the foundations.
 * */
int fc_solve_sfs_move_freecell_cards_to_founds(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors_no_stacks();
    int fc;
    int deck;
    fcs_card_t card;
    int check;
    fcs_internal_move_t temp_move;
#ifndef HARD_CODED_NUM_FREECELLS
    DECLARE_GAME_PARAMS();
#endif

    tests_define_accessors_no_stacks();

#ifndef HARD_CODED_NUM_FREECELLS
    SET_GAME_PARAMS();
#endif

    temp_move = fc_solve_empty_move;

    /* Now check the same for the free cells */
    for( fc=0 ; fc < LOCAL_FREECELLS_NUM ; fc++)
    {
        card = fcs_freecell_card(state, fc);
        if (fcs_card_card_num(card) != 0)
        {
            for(deck=0;deck<INSTANCE_DECKS_NUM;deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_card_num(card) - 1)
                {
                    /* We can put it there */
                    sfs_check_state_begin()

                    fcs_empty_freecell(new_state, fc);

                    fcs_increment_foundation(new_state, deck*4+fcs_card_suit(card));

                    fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION);
                    fcs_int_move_set_src_freecell(temp_move,fc);
                    fcs_int_move_set_foundation(temp_move,deck*4+fcs_card_suit(card));

                    fcs_move_stack_push(moves, temp_move);

                    sfs_check_state_end();
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

static int derived_states_compare_callback(
        const void * void_a,
        const void * void_b
        )
{
    int a = ((const fcs_derived_states_list_item_t *)void_a)->context.i;
    int b = ((const fcs_derived_states_list_item_t *)void_b)->context.i;
    
    return ((a < b) ? (-1) : (a > b) ? (1) : 0);
}

/*
 * This function empties two stacks from the new state
 * into freeeclls and empty columns
 */
static GCC_INLINE int empty_two_cols_from_new_state(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_new_state_val,
        fcs_move_stack_t * moves,
        const int cols_indexes[3],
        int nc1, int nc2
        )
{
    int num_cards_to_move_from_columns[3];
    const int * col_idx;
    int * col_num_cards;
    fcs_cards_column_t new_from_which_col;
    fcs_card_t top_card;
    fcs_internal_move_t temp_move;
    fcs_state_t * ptr_new_state_key;
    int ret = -1;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif
#ifdef INDIRECT_STACK_STATES
    char * indirect_stacks_buffer;
#endif

    ptr_new_state_key = ptr_new_state_val->key;


    num_cards_to_move_from_columns[0] = nc1;
    num_cards_to_move_from_columns[1] = nc2;
    num_cards_to_move_from_columns[2] = -1;

    col_idx = cols_indexes;
    col_num_cards = num_cards_to_move_from_columns;

    temp_move = fc_solve_empty_move;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_INSTANCE_GAME_PARAMS(soft_thread->hard_thread->instance);
#endif

#ifdef INDIRECT_STACK_STATES
    indirect_stacks_buffer = soft_thread->hard_thread->indirect_stacks_buffer;
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
                if (fcs_freecell_card_num(
                            new_state, dest_fc_idx
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

            new_from_which_col = fcs_state_get_col(new_state, *col_idx);

            fcs_col_pop_card(new_from_which_col, top_card);

            fcs_put_card_in_freecell(
                new_state, 
                dest_fc_idx,
                top_card
            );

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
                    fcs_state_get_col(new_state, put_cards_in_col_idx)
                    ) == 0)
                {
                    break;
                }
            }

            assert(put_cards_in_col_idx < LOCAL_STACKS_NUM );

            my_copy_stack(put_cards_in_col_idx);

            new_b_col = fcs_state_get_col(new_state, put_cards_in_col_idx);

            new_from_which_col = fcs_state_get_col(new_state, *col_idx);

            fcs_col_pop_card(new_from_which_col, top_card);
            fcs_col_push_card(new_b_col, top_card);

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
}

int fc_solve_sfs_move_freecell_cards_on_top_of_stacks(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * final_derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_seqs_built_by()
    tests_declare_empty_stacks_fill()

    fcs_derived_states_list_t derived_states_struct;
    fcs_derived_states_list_t * derived_states_list;
    int dest_cards_num;
    int ds, fc, dc;
    fcs_cards_column_t dest_col;
    fcs_card_t dest_card, src_card, dest_below_card;
    int check;

    fcs_internal_move_t temp_move;
    int is_seq_in_dest;
    int num_cards_to_relocate;
    int freecells_to_fill, freestacks_to_fill;
    int a;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    DECLARE_GAME_PARAMS();
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;
    char * positions_by_rank;
    char * pos_idx_to_check, * last_pos_idx;

    tests_define_accessors();
    tests_define_seqs_built_by();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif

    temp_move = fc_solve_empty_move;

    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    derived_states_list = &derived_states_struct;
    derived_states_struct.num_states = 0;
    derived_states_struct.states = NULL;

    positions_by_rank =
        fc_solve_get_the_positions_by_rank_data(
            soft_thread,
            ptr_state_val
        );

    /* Let's try to put cards in the freecells on top of stacks */

    /* Scan the freecells */
    for(fc=0;fc<LOCAL_FREECELLS_NUM;fc++)
    {
        src_card = fcs_freecell_card(state, fc);

        /* If the freecell is not empty and dest_card is its parent
         * */
        if (
                /* The Cell should not be empty. */
                (fcs_card_card_num(src_card) != 0) 
                && 
                /* We cannot put a king anywhere. */
                (fcs_card_card_num(src_card) != 13)
            )
        {
#define FCS_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(card) \
            (((sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK) ? 0 \
                : (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT) ? \
                fcs_card_suit(card) : ((fcs_card_suit(card)^0x1)&(0x2-1)) \
            ) << 1)
#define FCS_CARD_SUIT_POSITIONS_BY_RANK_STEP() \
            (((sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK) ? 1 \
                : (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT) ? \
                4 : 2 \
            ) << 1)

#define FCS_POS_IDX_TO_CHECK_START_LOOP(src_card) \
            pos_idx_to_check = &positions_by_rank[ \
                (FCS_POS_BY_RANK_WIDTH * (fcs_card_card_num(src_card))) \
            ]; \
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
                if ((ds = pos_idx_to_check[0]) == -1)
                {
                    continue;
                }
                dc = pos_idx_to_check[1];

                dest_col = fcs_state_get_col(state, ds);
                dest_card = fcs_col_get_card(dest_col, dc);

                dest_cards_num = fcs_col_len(dest_col);
                /* Let's check if we can put it there */

                /* Check if the destination card is already below a
                 * suitable card */
                is_seq_in_dest = 0;
                if (dest_cards_num - 1 > dc)
                {
                    dest_below_card =
                        fcs_col_get_card(dest_col, dc+1);
                    if (fcs_is_parent_card(dest_below_card, dest_card))
                    {
                        is_seq_in_dest = 1;
                    }
                }

                if (! is_seq_in_dest)
                {
                    num_cards_to_relocate = dest_cards_num - dc - 1;

                    freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                    num_cards_to_relocate -= freecells_to_fill;

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
                                ptr_new_state_val,
                                moves,
                                cols_indexes,
                                dest_cards_num - dc - 1,
                                0
                        );

                        new_dest_col = fcs_state_get_col(new_state, ds);

                        /* Now put the freecell card on top of the stack */
                        fcs_col_push_card(new_dest_col, src_card);
                        fcs_empty_freecell(new_state, fc);

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

    qsort(
        derived_states_struct.states,
        derived_states_struct.num_states,
        sizeof(derived_states_struct.states[0]),
        derived_states_compare_callback
    );

    for (a=0 ; a<derived_states_struct.num_states ; a++)
    {
        fc_solve_derived_states_list_add_state(
            final_derived_states_list,
            derived_states_struct.states[a].state_ptr,
            0
        );
    }

    free(derived_states_struct.states);

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_move_non_top_stack_cards_to_founds(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_empty_stacks_fill()

    int check;

    int stack_idx;
    int cards_num;
    int c;
    fcs_card_t top_card, card;
    fcs_cards_column_t col;
    int deck;
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;

    fcs_internal_move_t temp_move;

    tests_define_accessors();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif
    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    temp_move = fc_solve_empty_move;

    /* Now let's check if a card that is under some other cards can be placed
     * in the foundations. */

    for(stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
    {
        col = fcs_state_get_col(state, stack_idx);
        cards_num = fcs_col_len(col);
        /*
         * We starts from cards_num-2 because the top card is already covered
         * by move_top_stack_cards_to_founds.
         * */
        for(c=cards_num-2 ; c >= 0 ; c--)
        {
            card = fcs_col_get_card(col, c);
            for(deck=0;deck<INSTANCE_DECKS_NUM;deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_card_num(card)-1)
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
                            ptr_new_state_val,
                            moves,
                            cols_indexes,
                            cards_num-(c+1),
                            0
                        );

                        new_src_col = fcs_state_get_col(new_state, stack_idx);

                        fcs_col_pop_card(new_src_col, top_card);
                        fcs_increment_foundation(new_state, deck*4+fcs_card_suit(top_card));

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

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


int fc_solve_sfs_move_stack_cards_to_a_parent_on_the_same_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_seqs_built_by()
    tests_declare_empty_stacks_fill()

    int check;

    int stack_idx, c, cards_num, a, dc;
    int is_seq_in_dest;
    fcs_card_t card, prev_card;
    fcs_card_t dest_below_card, dest_card;
    int freecells_to_fill, freestacks_to_fill;
    int num_cards_to_relocate;
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;
    fcs_cards_column_t col;

    fcs_internal_move_t temp_move;

    tests_define_accessors();
    tests_define_seqs_built_by();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    /*
     * Now let's try to move a stack card to a parent card which is found
     * on the same stack.
     * */
    for (stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
    {
        col = fcs_state_get_col(state, stack_idx);
        cards_num = fcs_col_len(col);

        for (c=0 ; c<cards_num ; c++)
        {
            /* Find a card which this card can be put on; */

            card = fcs_col_get_card(col, c);

            /* Do not move cards that are already found above a suitable 
             * parent */
            /* TODO : is this code safe for variants that are not Freecell? */
            a = 1;
            if (c != 0)
            {
                prev_card = fcs_col_get_card(col, c-1);
                if ((fcs_card_card_num(prev_card) == fcs_card_card_num(card)+1) &&
                    ((fcs_card_suit(prev_card) & 0x1) != (fcs_card_suit(card) & 0x1)))
                {
                   a = 0;
                }
            }
            if (a)
            {
#define ds stack_idx
#define dest_col col
#define dest_cards_num cards_num
                /* Check if it can be moved to something on the same stack */
                for(dc=0;dc<c-1;dc++)
                {
                    dest_card = fcs_col_get_card(dest_col, dc);
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* Corresponding cards - see if it is feasible to move
                           the source to the destination. */

                        is_seq_in_dest = 0;
                        dest_below_card = fcs_col_get_card(dest_col, dc+1);
                        if (fcs_is_parent_card(dest_below_card, dest_card))
                        {
                            is_seq_in_dest = 1;
                        }

                        if (!is_seq_in_dest)
                        {
                            num_cards_to_relocate = dest_cards_num - dc - 1;

                            freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                            num_cards_to_relocate -= freecells_to_fill;

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
                                int last_dest;
                                int source_index;
                                fcs_cards_column_t new_dest_col;
                                fcs_card_t moved_card;
                                /* We can move it */

                                sfs_check_state_begin()

                                my_copy_stack(ds);

                                new_dest_col = fcs_state_get_col(new_state, ds);

                                cols_indexes[0] = ds;
                                cols_indexes[1] = -1;
                                cols_indexes[2] = -1;

                                last_dest = empty_two_cols_from_new_state(
                                    soft_thread,
                                    ptr_new_state_val,
                                    moves,
                                    cols_indexes,
                                    /* We're moving one extra card */
                                    cards_num - c,
                                    0
                                );

                                source_index = last_dest & 0xFF;

                                empty_two_cols_from_new_state(
                                    soft_thread,
                                    ptr_new_state_val,
                                    moves,
                                    cols_indexes,
                                    c - dc - 1,
                                    0
                                );

                                if (!( (last_dest >> 8)&0x1))
                                {
                                    moved_card = fcs_freecell_card(new_state, source_index);
                                    fcs_empty_freecell(new_state, source_index);

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

    return FCS_STATE_IS_NOT_SOLVEABLE;
}
#undef ds
#undef dest_col
#undef dest_cards_num

int fc_solve_sfs_move_stack_cards_to_different_stacks(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * final_derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_seqs_built_by()
    tests_declare_empty_stacks_fill()

    fcs_derived_states_list_t derived_states_struct;
    fcs_derived_states_list_t * derived_states_list;    
    int check;

    int stack_idx, c, dc, ds, a;
    fcs_card_t card, this_card, prev_card;
    fcs_card_t dest_card;
    int freecells_to_fill, freestacks_to_fill;
    int num_cards_to_relocate;
    int seq_end;
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    DECLARE_GAME_PARAMS();
#endif
    char * positions_by_rank;
    char * pos_idx_to_check, * last_pos_idx;
    int num_vacant_freecells;
    int num_vacant_stacks;
    fcs_cards_column_t col, dest_col;

    fcs_internal_move_t temp_move;

    tests_define_accessors();
    tests_define_seqs_built_by();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif

    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    derived_states_list = &derived_states_struct;
    derived_states_struct.num_states = 0;
    derived_states_struct.states = NULL;
    
    temp_move = fc_solve_empty_move;

    positions_by_rank = 
        fc_solve_get_the_positions_by_rank_data(
            soft_thread,
            ptr_state_val
        );

    /* Now let's try to move a card from one stack to the other     *
     * Note that it does not involve moving cards lower than king   *
     * to empty stacks                                              */

    for (stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
    {
        col = fcs_state_get_col(state, stack_idx);

        for (c=0 ; c<fcs_col_len(col) ; c=seq_end+1)
        {
            /* Check if there is a sequence here. */
            for(seq_end=c ; seq_end<fcs_col_len(col)-1 ; seq_end++)
            {
                this_card = fcs_col_get_card(col, seq_end+1);
                prev_card = fcs_col_get_card(col, seq_end);

                if (fcs_is_parent_card(this_card,prev_card))
                {
                }
                else
                {
                    break;
                }
            }

            /* Find a card which this card can be put on; */

            card = fcs_col_get_card(col, c);

#ifndef FCS_WITHOUT_CARD_FLIPPING
            /* Make sure the card is not flipped or else we can't move it */
            if (fcs_card_get_flipped(card))
            {
                continue;
            }
#endif

            /* Skip if it's a King - nothing to put it on. */
            if (fcs_card_card_num(card) == 13)
            {
                continue;
            }

            FCS_POS_IDX_TO_CHECK_START_LOOP(card)
            {
                if ((ds = pos_idx_to_check[0]) == -1)
                {
                    continue;
                }
                dc = pos_idx_to_check[1];

                if (ds == stack_idx)
                {
                    continue;
                }
                dest_col = fcs_state_get_col(state, ds);
                dest_card = fcs_col_get_card(dest_col, dc);

                num_cards_to_relocate = fcs_col_len(dest_col) - dc - 1 + fcs_col_len(col) - seq_end - 1;

                freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                num_cards_to_relocate -= freecells_to_fill;

                if (tests__is_filled_by_any_card())
                {
                    freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

                    num_cards_to_relocate -= freestacks_to_fill;
                }
                else
                {
                    freestacks_to_fill = 0;
                }

                if ((num_cards_to_relocate == 0) &&
                   (calc_max_sequence_move(num_vacant_freecells-freecells_to_fill, num_vacant_stacks-freestacks_to_fill) >=
                    seq_end - c + 1))
                {
                    int cols_indexes[3];
                    sfs_check_state_begin()

                    my_copy_stack(stack_idx);
                    my_copy_stack(ds);

                    cols_indexes[0] = ds;
                    cols_indexes[1] = stack_idx;
                    cols_indexes[2] = -1;

                    empty_two_cols_from_new_state(
                        soft_thread,
                        ptr_new_state_val,
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


    qsort(
        derived_states_struct.states,
        derived_states_struct.num_states,
        sizeof(derived_states_struct.states[0]),
        derived_states_compare_callback
    );

    for (a=0 ; a<derived_states_struct.num_states ; a++)
    {
        fc_solve_derived_states_list_add_state(
            final_derived_states_list,
            derived_states_struct.states[a].state_ptr,
            0
        );
    }

    free(derived_states_struct.states);
    
    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_move_sequences_to_free_stacks(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_empty_stacks_fill()
    tests_declare_seqs_built_by()
    int check;

    int stack_idx, cards_num, c, ds, b, seq_end;
    fcs_card_t this_card, prev_card;
    int max_sequence_len;
    int num_cards_to_relocate, freecells_to_fill, freestacks_to_fill;
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;
    fcs_cards_column_t col;

    fcs_internal_move_t temp_move;

    tests_define_accessors();
    tests_define_empty_stacks_fill();
    tests_define_seqs_built_by();

    if (tests__is_filled_by_none())
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    temp_move = fc_solve_empty_move;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif
    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    max_sequence_len = calc_max_sequence_move(num_vacant_freecells, num_vacant_stacks-1);

    /* Now try to move sequences to empty stacks */

    if (num_vacant_stacks > 0)
    {
        ds = -1;
        for(stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
        {
            col = fcs_state_get_col(state, stack_idx);
            cards_num = fcs_col_len(col);

            for(c=0; c<cards_num; c=seq_end+1)
            {
                /* Check if there is a sequence here. */
                for(seq_end=c ; seq_end<cards_num-1; seq_end++)
                {
                    this_card = fcs_col_get_card(col, seq_end+1);
                    prev_card = fcs_col_get_card(col, seq_end);

                    if (! fcs_is_parent_card(this_card, prev_card))
                    {
                        break;
                    }
                }

                if ((fcs_col_get_card_num(col, c) != 13) &&
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
                            (fcs_col_get_card_num(col, c) == 13) :
                            1
                        )
                       )
                    {
                        sfs_check_state_begin();

                        if (ds < 0)
                        {
                            for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
                            {
                                if (fcs_col_len(
                                    fcs_state_get_col(new_state, ds)
                                    ) == 0)
                                {
                                    break;
                                }
                            }
                        }

                        my_copy_stack(ds);
                        my_copy_stack(stack_idx);

                        fcs_move_sequence(ds, stack_idx, c, cards_num-1);

                        sfs_check_state_end()
                    }
                }
                else
                {
                    num_cards_to_relocate = cards_num - seq_end - 1;

                    freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                    num_cards_to_relocate -= freecells_to_fill;

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
                                (fcs_col_get_card_num(col, seq_start)
                                    == 13) :
                                1
                            )
                        )
                        {
                            fcs_cards_column_t new_src_col;
                            int cols_indexes[3];
                            int empty_ret;

                            sfs_check_state_begin();

                            /* Fill the freecells with the top cards */

                            my_copy_stack(stack_idx);

                            new_src_col = fcs_state_get_col(new_state, stack_idx);

                            cols_indexes[0] = stack_idx;
                            cols_indexes[1] = -1;
                            cols_indexes[2] = -1;

                            empty_ret = empty_two_cols_from_new_state(
                                soft_thread,
                                ptr_new_state_val,
                                moves,
                                cols_indexes,
                                freecells_to_fill + freestacks_to_fill,
                                0
                            );

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

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_move_freecell_cards_to_empty_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_empty_stacks_fill()

    int check;
    int fc, stack_idx;
    fcs_card_t card;

    fcs_internal_move_t temp_move;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif

    /* Let's try to put cards that occupy freecells on an empty stack */

    tests_define_accessors();
    tests_define_empty_stacks_fill();

    if (tests__is_filled_by_none())
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    temp_move = fc_solve_empty_move;

    if (soft_thread->num_vacant_stacks)
    {
        for(stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
        {
            if (fcs_col_len(
                fcs_state_get_col(state, stack_idx)
                ) == 0)
            {
                break;
            }
        }

        for(fc=0;fc<LOCAL_FREECELLS_NUM;fc++)
        {
            card = fcs_freecell_card(state, fc);
            if (
                (tests__is_filled_by_kings_only()) ?
                    (fcs_card_card_num(card) == 13) :
                    (fcs_card_card_num(card) != 0)
               )
            {
                fcs_cards_column_t new_src_col;
                /* We can move it */

                sfs_check_state_begin();

                my_copy_stack(stack_idx);

                new_src_col = fcs_state_get_col(new_state, stack_idx);

                fcs_col_push_card(new_src_col, card);
                fcs_empty_freecell(new_state, fc);

                fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_STACK);
                fcs_int_move_set_src_freecell(temp_move,fc);
                fcs_int_move_set_dest_stack(temp_move,stack_idx);
                fcs_move_stack_push(moves, temp_move);

                sfs_check_state_end()
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_move_cards_to_a_different_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * final_derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_seqs_built_by()
    tests_declare_empty_stacks_fill()

    int check;
    fcs_derived_states_list_t derived_states_struct;
    fcs_derived_states_list_t * derived_states_list;

    int stack_idx, cards_num, c, min_card_height, ds, dc, a;
    int num_cards_to_relocate;
    int dest_cards_num;
    fcs_card_t card, upper_card, lower_card;
    fcs_card_t dest_card;
    int freecells_to_fill, freestacks_to_fill;
    fcs_cards_column_t col, dest_col;

    fcs_internal_move_t temp_move;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    DECLARE_GAME_PARAMS();
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;
    char * positions_by_rank;
    char * pos_idx_to_check, * last_pos_idx;

    tests_define_accessors();
    tests_define_seqs_built_by();
    tests_define_empty_stacks_fill();

    temp_move = fc_solve_empty_move;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif
    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    derived_states_list = &derived_states_struct;
    derived_states_struct.num_states = 0;
    derived_states_struct.states = NULL;

    positions_by_rank =
        fc_solve_get_the_positions_by_rank_data(
            soft_thread,
            ptr_state_val
        );

    /* This time try to move cards that are already on top of a parent to a different parent */

    for (stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
    {
        col = fcs_state_get_col(state, stack_idx);
        cards_num = fcs_col_len(col);

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

        upper_card = fcs_col_get_card(col, cards_num-1);

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

        for(c=min_card_height ; c < cards_num ; c++)
        {
            /* Find a card which this card can be put on; */

            card = fcs_col_get_card(col, c);

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
                if ((ds = pos_idx_to_check[0]) == -1)
                {
                    continue;
                }
                if (ds == stack_idx)
                {
                    continue;
                }
                dc = pos_idx_to_check[1];


                dest_col = fcs_state_get_col(state, ds);
                dest_cards_num = fcs_col_len(dest_col);
                dest_card = fcs_col_get_card(dest_col, dc);

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

                num_cards_to_relocate = dest_cards_num - dc - 1;

                freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                num_cards_to_relocate -= freecells_to_fill;

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
                    fcs_cards_column_t new_dest_col;

                    sfs_check_state_begin()

                    /* Fill the freecells with the top cards */

                    my_copy_stack(ds);

                    new_dest_col = fcs_state_get_col(new_state, ds);

                    cols_indexes[0] = ds;
                    cols_indexes[1] = -1;
                    cols_indexes[2] = -1;

                    empty_two_cols_from_new_state(
                        soft_thread,
                        ptr_new_state_val,
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

    qsort(
        derived_states_struct.states,
        derived_states_struct.num_states,
        sizeof(derived_states_struct.states[0]),
        derived_states_compare_callback
    );

    for (a=0 ; a<derived_states_struct.num_states ; a++)
    {
        fc_solve_derived_states_list_add_state(
            final_derived_states_list,
            derived_states_struct.states[a].state_ptr,
            0
        );
    }

    free(derived_states_struct.states);

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_empty_stack_into_freecells(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_empty_stacks_fill()

    int check;

    int stack_idx, cards_num, c, b;
    fcs_card_t top_card;
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif

    int num_vacant_freecells;
    int num_vacant_stacks;
    fcs_cards_column_t col;

    fcs_internal_move_t temp_move;

    tests_define_accessors();
    tests_define_empty_stacks_fill();

    if (tests__is_filled_by_none())
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    temp_move = fc_solve_empty_move;
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    num_vacant_stacks = soft_thread->num_vacant_stacks;
    num_vacant_freecells = soft_thread->num_vacant_freecells;


    /* Now, let's try to empty an entire stack into the freecells, so other cards can
     * inhabit it */

    if (num_vacant_stacks == 0)
    {
        for(stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
        {
            col = fcs_state_get_col(state, stack_idx);
            cards_num = fcs_col_len(col);
            if (cards_num <= num_vacant_freecells)
            {
                /* We can empty it */
                fcs_cards_column_t new_src_col;

                sfs_check_state_begin()

                my_copy_stack(stack_idx);

                new_src_col = fcs_state_get_col(new_state, stack_idx);

                for(c=0;c<cards_num;c++)
                {
                    /* Find a vacant freecell */
                    for(b=0; b<LOCAL_FREECELLS_NUM; b++)
                    {
                        if (fcs_freecell_card_num(new_state, b) == 0)
                        {
                            break;
                        }
                    }
                    fcs_col_pop_card(new_src_col, top_card);

                    fcs_put_card_in_freecell(new_state, b, top_card);

                    fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                    fcs_int_move_set_src_stack(temp_move,stack_idx);
                    fcs_int_move_set_dest_freecell(temp_move,b);
                    fcs_move_stack_push(moves, temp_move);
                }

                sfs_check_state_end()
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;

}

int fc_solve_sfs_yukon_do_nothing(
        fc_solve_soft_thread_t * soft_thread GCC_UNUSED,
        fcs_state_extra_info_t * ptr_state_val GCC_UNUSED,
        fcs_derived_states_list_t * derived_states_list GCC_UNUSED
        )
{
    return FCS_STATE_IS_NOT_SOLVEABLE;
}

/* Disabling Yukon solving for the time being. */
#if 0
int fc_solve_sfs_yukon_move_card_to_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()

    int check;

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

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_yukon_move_kings_to_empty_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()

    int check;

    int stack_idx, cards_num, c, ds;
    fcs_card_t card;
    fcs_cards_column_t col;

#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    int num_vacant_stacks;

    fcs_internal_move_t temp_move;

    tests_define_accessors();

    num_vacant_stacks = soft_thread->num_vacant_stacks;
    temp_move = fc_solve_empty_move;

    if (num_vacant_stacks == 0)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
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
            if (fcs_card_card_num(card) == 13)
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

    return FCS_STATE_IS_NOT_SOLVEABLE;
}
#endif

#ifdef FCS_WITH_TALONS
/*
    Let's try to deal the Gypsy-type Talon.

  */
int fc_solve_sfs_deal_gypsy_talon(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()

    int check;

    fcs_card_t temp_card;
    int a;

    fcs_internal_move_t temp_move;

    tests_define_accessors();

    if (instance->talon_type != FCS_TALON_GYPSY)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    if (fcs_talon_pos(state) < fcs_talon_len(state))
    {
        sfs_check_state_begin()
        for(a=0;a<LOCAL_STACKS_NUM;a++)
        {
            fcs_cards_column_t new_a_col;

            temp_card = fcs_get_talon_card(new_state, fcs_talon_pos(new_state)+a);
            my_copy_stack(a);

            new_a_col = fcs_state_get_col(new_state, a);

            fcs_col_push_card(new_a_col,temp_card);
        }
        fcs_talon_pos(new_state) += LOCAL_STACKS_NUM;
        fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_DEAL_GYPSY_TALON);
        fcs_move_stack_push(moves, temp_move);

        sfs_check_state_end()
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


int fc_solve_sfs_get_card_from_klondike_talon(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()

    fcs_internal_move_t temp_move;

    int check;
    int num_redeals_left, num_redeals_done, num_cards_moved[2];
    int first_iter;
    fcs_card_t card_to_check, top_card;
    int s;
    int cards_num;
    int a;
    fcs_cards_column_t col;

    fcs_state_extra_info_t talon_temp;

    tests_define_accessors();

    if (instance->talon_type != FCS_TALON_KLONDIKE)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    /* Duplicate the talon and its parameters into talon_temp */
    talon_temp->s.talon = malloc(fcs_klondike_talon_len(state)+1);
    memcpy(
        talon_temp->s.talon,
        ptr_state_key->talon,
        fcs_klondike_talon_len(state)+1
        );
    memcpy(
        talon_temp->s.talon_params,
        ptr_state_key->talon_params,
        sizeof(ptr_state_key->talon_params)
        );

    /* Make sure we redeal the talon only once */
    num_redeals_left = fcs_klondike_talon_num_redeals_left(state);
    if ((num_redeals_left > 0) || (num_redeals_left < 0))
    {
        num_redeals_left = 1;
    }
    num_redeals_done = 0;
    num_cards_moved[0] = 0;
    num_cards_moved[1] = 0;

    first_iter = 1;
    while (num_redeals_left >= 0)
    {
        if ((fcs_klondike_talon_stack_pos(talon_temp->s) == -1) &&
            (fcs_klondike_talon_queue_pos(talon_temp->s) == fcs_klondike_talon_len(talon_temp->s)))
        {
            break;
        }
        if ((!first_iter) || (fcs_klondike_talon_stack_pos(talon_temp->s) == -1))
        {
            if (fcs_klondike_talon_queue_pos(talon_temp->s) == fcs_klondike_talon_len(talon_temp->s))
            {
                if (num_redeals_left > 0)
                {
                    fcs_klondike_talon_len(talon_temp->s) = fcs_klondike_talon_stack_pos(talon_temp->s);
                    fcs_klondike_talon_redeal_bare(talon_temp->s);

                    num_redeals_left--;
                    num_redeals_done++;
                }
                else
                {
                    break;
                }
            }
            fcs_klondike_talon_queue_to_stack(talon_temp->s);
            num_cards_moved[num_redeals_done]++;
        }
        first_iter = 0;

        card_to_check = fcs_klondike_talon_get_top_card(talon_temp->s);
        for(s=0 ; s<LOCAL_STACKS_NUM ; s++)
        {
            col = fcs_state_get_col(state, s);
            cards_num = fcs_col_len(col);
            top_card = fcs_col_get_card(col, cards_num-1);
            if (fcs_is_parent_card(card_to_check, top_card))
            {
                fcs_cards_column_t new_s_col;
                /* 
                 * We have a card in the talon that we can move
                 * to the column, so let's move it 
                 * */
                sfs_check_state_begin()

                my_copy_stack(s);

                new_s_col = fcs_state_get_col(new_state, s);

                new_state.talon = malloc(fcs_klondike_talon_len(talon_temp->s)+1);
                memcpy(
                    new_state.talon,
                    talon_temp->s.talon,
                    fcs_klondike_talon_len(talon_temp->s)+1
                    );

                memcpy(
                    ptr_new_state_key->talon_params,
                    talon_temp->s.talon_params,
                    sizeof(ptr_state_key->talon_params)
                );

                for(a=0;a<=num_redeals_done;a++)
                {
                    fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_KLONDIKE_FLIP_TALON);
                    fcs_int_move_set_num_cards_flipped(temp_move, num_cards_moved[a]);
                    fcs_move_stack_push(moves, temp_move);
                    if (a != num_redeals_done)
                    {
                        fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_KLONDIKE_REDEAL_TALON);
                        fcs_move_stack_push(moves,temp_move);
                    }
                }
                fcs_col_push_card(new_s_col, fcs_klondike_talon_get_top_card(new_state));
                fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_KLONDIKE_TALON_TO_STACK);
                fcs_int_move_set_dest_stack(temp_move, s);
                fcs_klondike_talon_decrement_stack(new_state);

                sfs_check_state_end()
            }
        }
    }

    free(talon_temp->s.talon);
    free(talon_temp);

    return FCS_STATE_IS_NOT_SOLVEABLE;

}

#endif

int fc_solve_sfs_atomic_move_card_to_empty_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_empty_stacks_fill()
#ifndef HARD_CODED_NUM_STACKS
    DECLARE_GAME_PARAMS();
#endif
    int stack_idx, cards_num;
    fcs_card_t card;
    fcs_internal_move_t temp_move;
    int check;
    int empty_stack_idx;
    int num_vacant_stacks;
    fcs_cards_column_t col;

    tests_define_accessors();
    tests_define_empty_stacks_fill();

    num_vacant_stacks = soft_thread->num_vacant_stacks;

    if (num_vacant_stacks == 0)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    for(empty_stack_idx=0;empty_stack_idx<LOCAL_STACKS_NUM;empty_stack_idx++)
    {
        if (fcs_col_len(
            fcs_state_get_col(state, empty_stack_idx)
            ) == 0)
        {
            break;
        }
    }

    if (tests__is_filled_by_none())
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    for(stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
    {
        col = fcs_state_get_col(state, stack_idx);
        cards_num = fcs_col_len(col);
        if (cards_num > 0)
        {
            card = fcs_col_get_card(col, cards_num-1);
            if (tests__is_filled_by_kings_only() &&
                (fcs_card_card_num(card) != 13))
            {
                continue;
            }
            /* Let's move it */
            {
                fcs_cards_column_t new_src_col;
                fcs_cards_column_t empty_stack_col;
                sfs_check_state_begin();

                my_copy_stack(stack_idx);

                new_src_col = fcs_state_get_col(new_state, stack_idx);

                fcs_col_pop_top(new_src_col);

                my_copy_stack(empty_stack_idx);

                empty_stack_col = fcs_state_get_col(new_state, empty_stack_idx);
                fcs_col_push_card(empty_stack_col, card);

                fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_STACK_TO_STACK);
                fcs_int_move_set_src_stack(temp_move, stack_idx);
                fcs_int_move_set_dest_stack(temp_move, empty_stack_idx);
                fcs_int_move_set_num_cards_in_seq(temp_move, 1);

                fcs_move_stack_push(moves, temp_move);

                sfs_check_state_end()
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_atomic_move_card_to_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_seqs_built_by()
#ifndef HARD_CODED_NUM_STACKS
    DECLARE_GAME_PARAMS();
#endif
    int stack_idx, cards_num, ds;
    fcs_card_t card, dest_card;
    fcs_internal_move_t temp_move;
    int check;
    fcs_cards_column_t col, dest_col;

    tests_define_accessors();
    tests_define_seqs_built_by();

#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    for(stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
    {
        col = fcs_state_get_col(state, stack_idx);
        cards_num = fcs_col_len(col);
        if (cards_num > 0)
        {
            card = fcs_col_get_card(col, cards_num-1);

            for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
            {
                if (ds == stack_idx)
                {
                    continue;
                }

                dest_col = fcs_state_get_col(state, ds);

                if (fcs_col_len(dest_col) > 0)
                {
                    dest_card = fcs_col_get_card(dest_col, 
                            fcs_col_len(dest_col)-1);
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* Let's move it */
                        {
                            fcs_cards_column_t new_src_col;
                            fcs_cards_column_t new_dest_col;
                            sfs_check_state_begin();

                            my_copy_stack(stack_idx);
                            my_copy_stack(ds);

                            new_src_col = fcs_state_get_col(new_state, stack_idx);
                            new_dest_col = fcs_state_get_col(new_state, ds);

                            fcs_col_pop_top(new_src_col);

                            fcs_col_push_card(new_dest_col, card);

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

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_atomic_move_card_to_freecell(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif

    int stack_idx, cards_num, ds;
    fcs_card_t card;
    fcs_internal_move_t temp_move;
    int check;
    int num_vacant_freecells;
    fcs_cards_column_t col;

    tests_define_accessors();

    temp_move = fc_solve_empty_move;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    num_vacant_freecells = soft_thread->num_vacant_freecells;

    if (num_vacant_freecells == 0)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    for(ds=0;ds<LOCAL_FREECELLS_NUM;ds++)
    {
        if (fcs_freecell_card_num(state, ds) == 0)
        {
            break;
        }
    }



    for(stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
    {
        col = fcs_state_get_col(state, stack_idx);
        cards_num = fcs_col_len(col);
        if (cards_num > 0)
        {
            card = fcs_col_get_card(col, cards_num-1);

            /* Let's move it */
            {
                fcs_cards_column_t new_src_col;
                sfs_check_state_begin();

                my_copy_stack(stack_idx);
                new_src_col = fcs_state_get_col(new_state, stack_idx);

                fcs_col_pop_top(new_src_col);

                fcs_put_card_in_freecell(new_state, ds, card);

                fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_STACK_TO_FREECELL);
                fcs_int_move_set_src_stack(temp_move, stack_idx);
                fcs_int_move_set_dest_freecell(temp_move, ds);

                fcs_move_stack_push(moves, temp_move);

                sfs_check_state_end()
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_atomic_move_freecell_card_to_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_seqs_built_by()

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif

    int fc, ds;
    fcs_card_t card, dest_card;
    fcs_internal_move_t temp_move;
    int check;
    fcs_cards_column_t dest_col;

    tests_define_accessors();
    tests_define_seqs_built_by();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    for(fc=0;fc<LOCAL_FREECELLS_NUM;fc++)
    {
        card = fcs_freecell_card(state, fc);
        if (fcs_card_card_num(card) == 0)
        {
            continue;
        }

        for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
        {
            dest_col = fcs_state_get_col(state, ds);
            if (fcs_col_len(dest_col) > 0)
            {
                dest_card = fcs_col_get_card(dest_col, fcs_col_len(dest_col)-1);
                if (fcs_is_parent_card(card, dest_card))
                {
                    /* Let's move it */
                    {
                        fcs_cards_column_t new_dest_col;
                        sfs_check_state_begin();

                        my_copy_stack(ds);

                        new_dest_col = fcs_state_get_col(new_state, ds);

                        fcs_empty_freecell(new_state, fc);

                        fcs_col_push_card(new_dest_col, card);

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
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_atomic_move_freecell_card_to_empty_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors()
    tests_declare_empty_stacks_fill()

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif

    int fc, ds;
    fcs_card_t card;
    fcs_internal_move_t temp_move;
    int check;
    int num_vacant_stacks;

    tests_define_accessors();
    tests_define_empty_stacks_fill();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    if (num_vacant_stacks == 0)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    if (tests__is_filled_by_none())
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    /* Find a vacant stack */
    for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
    {
        if (fcs_col_len(
            fcs_state_get_col(state, ds)
            ) == 0)
        {
            break;
        }
    }

    for(fc=0;fc<LOCAL_FREECELLS_NUM;fc++)
    {
        card = fcs_freecell_card(state, fc);
        if (fcs_card_card_num(card) == 0)
        {
            continue;
        }

        if (tests__is_filled_by_kings_only() &&
            (fcs_card_card_num(card) != 13))
        {
            continue;
        }

        {
            fcs_cards_column_t new_dest_col;

            sfs_check_state_begin();

            my_copy_stack(ds);

            new_dest_col = fcs_state_get_col(new_state, ds);

            fcs_empty_freecell(new_state, fc);

            fcs_col_push_card(new_dest_col, card);

            fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_FREECELL_TO_STACK);
            fcs_int_move_set_src_freecell(temp_move, fc);
            fcs_int_move_set_dest_stack(temp_move, ds);
            fcs_int_move_set_num_cards_in_seq(temp_move, 1);

            fcs_move_stack_push(moves, temp_move);

            sfs_check_state_end()
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

#undef state
#undef new_state

