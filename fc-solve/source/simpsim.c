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
 * simpsim.c - a module that contains Simple Simon Moves.
 *
 */

#define BUILDING_DLL 1

#include <stdio.h>

#include "config.h"
#include "simpsim.h"

#include "bool.h"
#include "scans.h"

/* This is a fallback in case this module is still compiled with
 * FCS_DISABLE_SIMPLE_SIMON.
 * */
#ifdef FCS_DISABLE_SIMPLE_SIMON

char fc_solve_simple_simon_nothing;

#else

#include "instance.h"

#include "meta_move_funcs_helpers.h"

#define fcs_is_ss_false_parent(parent, child) \
    (fcs_card_rank(parent) == fcs_card_rank(child)+1)

#define fcs_suit_is_ss_true_parent(parent_suit, child_suit) \
    ((parent_suit) == (child_suit))

static GCC_INLINE const fcs_bool_t fcs_is_ss_suit_true(const fcs_card_t parent, const fcs_card_t child)
{
    return fcs_suit_is_ss_true_parent(
        fcs_card_suit(parent),fcs_card_suit(child)
    );
}

static GCC_INLINE const fcs_bool_t fcs_is_ss_true_parent(const fcs_card_t parent, const fcs_card_t child)
{
    return
    (
        fcs_is_ss_false_parent(parent, child) &&
        fcs_is_ss_suit_true(parent, child)
    );
}

#define STACK_SOURCE_LOOP_START(min_num_cards) \
    for (int stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++) \
    { \
        const fcs_cards_column_t col = fcs_state_get_col(state, stack_idx); \
        const int cards_num = fcs_col_len(col); \
        if (cards_num < min_num_cards) \
        { \
            continue; \
        } \


#define STACK_SOURCE_LOOP_END() \
    } \

#define STACK_DEST_LOOP_START(min_num_cards) \
    for (int ds = 0 ; ds<LOCAL_STACKS_NUM ; ds++) \
    { \
        if (ds == stack_idx) \
        { \
            continue; \
        } \
        \
        const fcs_cards_column_t dest_col = fcs_state_get_col(state, ds); \
        const int dest_cards_num = fcs_col_len(dest_col); \
        \
        if (dest_cards_num < min_num_cards) \
        { \
            continue; \
        } \


#define STACK_DEST_LOOP_END() \
    } \

#define DC_LOOP_START(offset, filter) \
    for (int dc = dest_cards_num-offset ; dc >= 0 ; dc--) \
    {   \
        if (!filter(fcs_col_get_card(dest_col, dc), card)) \
        { \
            continue; \
        } \


#define DC_LOOP_END() \
    }

#define DS_DC_LOOP_START(min_num_cards, offset, filter) \
    STACK_DEST_LOOP_START(min_num_cards) \
        DC_LOOP_START(offset, filter)

#define DS_DC_LOOP_END() \
    DC_LOOP_END() \
    STACK_DEST_LOOP_END()


#ifndef HARD_CODED_NUM_STACKS
#define SIMPS_SET_GAME_PARAMS() SET_GAME_PARAMS()
#else
#define SIMPS_SET_GAME_PARAMS()
#endif

#define SIMPS_define_accessors() \
    tests_define_accessors(); \
    SIMPS_SET_GAME_PARAMS()

#define SIMPS_define_vacant_stacks_accessors() \
    SIMPS_define_accessors(); \
    fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks

#define STACKS_MAP_LEN MAX_NUM_STACKS

static GCC_INLINE void init_stacks_map(fcs_bool_t * const stacks_map, const int stack_idx, const int ds)
{
    for (int i=0 ; i < STACKS_MAP_LEN ; i++)
    {
        stacks_map[i] = FALSE;
    }
    stacks_map[stack_idx] = stacks_map[ds] = TRUE;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_simple_simon_move_sequence_to_founds)
{
    /*
     * stack_idx - the stack index from which to move cards to the founds.
     * cards_num - the number of cards in "stack"
     * suit - the suit of the complete sequence
     * a - the height of the card
     * */
    /*
     * card - the current card (at height a)
     * above_card - the card above it.
     * */
    SIMPS_define_accessors();

    STACK_SOURCE_LOOP_START(13)
        fcs_card_t card = fcs_col_get_card(col, cards_num-1);

        /* Check if the top 13 cards are a sequence */
        int a;
        for (a=2 ; a<=13 ; a++)
        {
            const fcs_card_t above_card = fcs_col_get_card(col, cards_num-a);
            if (! fcs_is_ss_true_parent(above_card, card))
            {
                break;
            }
            card = above_card;
        }
        if (a == 14)
        {
            /* We can move this sequence up there */

            sfs_check_state_begin();

            my_copy_stack(stack_idx);

            fcs_cards_column_t new_src_col = fcs_state_get_col(
                new_state, stack_idx
            );
            for (a=0 ; a<13 ; a++)
            {
                fcs_col_pop_top(new_src_col);
            }

            const int suit = fcs_card_suit(card);
            fcs_set_foundation(new_state, suit, a);

            fcs_move_stack_non_seq_push(moves,
                FCS_MOVE_TYPE_SEQ_TO_FOUNDATION,
                stack_idx, suit);

            sfs_check_state_end();
        }
    STACK_SOURCE_LOOP_END()

    return;
}


/*
 * TODO:
 *
 * Convert to fc_solve_get_the_positions_by_rank_data.
 * */
DECLARE_MOVE_FUNCTION(fc_solve_sfs_simple_simon_move_sequence_to_true_parent)
{
    /*
     * stack_idx - the source stack index on which the sequence currently
     * resides.
     * cards_num - the number of cards in "stack".
     * suit - the suit of the current card
     * a - a temporary variable that designates a card height
     * h - the current height in stack
     * card - the current card (at height h)
     * above_card - the card above it.
     * dest_card - the destination card on which to put the sequence
     * rank - the rank (i.e: A, 2 ,3 ... K) of the card, or
     * its previous one.
     * num_true_seqs - the number of true sequences (i.e: sequences of a
     * unified suit) in the source sequence.
     * ds - the destination stack index.
     * dest_cards_num - the number of cards in "ds".
     * */

    SIMPS_define_vacant_stacks_accessors();

    STACK_SOURCE_LOOP_START(1)
        /* Loop on the cards in the stack and try to look for a true
         * parent on top one of the stacks */
        fcs_card_t card = fcs_col_get_card(col, cards_num-1);
        int num_true_seqs = 1;

        for (int h=cards_num-2 ; h>=-1 ; h--)
        {
            STACK_DEST_LOOP_START(1)
                if (! fcs_is_ss_true_parent(fcs_col_get_card(dest_col, dest_cards_num-1), card))
                {
                    continue;
                }

                /* This is a suitable parent - let's check if we
                 * have enough empty stacks to make the move feasible */
                /* We can do it - so let's move */

                sfs_check_state_begin();

                my_copy_stack(stack_idx);
                my_copy_stack(ds);


                fcs_move_sequence(ds, stack_idx, h+1, cards_num-1);
                sfs_check_state_end();
            STACK_DEST_LOOP_END()

            /* Stop if we reached the bottom of the stack */
            if (h == -1)
            {
                break;
            }

            const fcs_card_t prev_card = card;
            card = fcs_col_get_card(col, h);
            /* If this is no longer a sequence - move to the next stack */
            if (!fcs_is_ss_false_parent(card, prev_card))
            {
                break;
            }
            if (!fcs_is_ss_suit_true(card, prev_card))
            {
                num_true_seqs++;
                /* We can no longer perform the move so go to the next
                 * stack. */
                if (calc_max_simple_simon_seq_move(num_vacant_stacks)
                    < num_true_seqs)
                {
                    break;
                }
            }
        }
    STACK_SOURCE_LOOP_END()
}

static GCC_INLINE int get_seq_h(const fcs_cards_column_t col, int * num_true_seqs_out_ptr)
{
    const int cards_num = fcs_col_len(col);

    fcs_card_t card = fcs_col_get_card(col,cards_num-1);
    int num_true_seqs = 1;

    int h;
    /* Stop if we reached the bottom of the stack */
    for ( h=cards_num-2 ; h>-1 ; h--)
    {
        const fcs_card_t next_card = fcs_col_get_card(col, h);
        /* If this is no longer a sequence - move to the next stack */
        if (!fcs_is_ss_false_parent(next_card, card))
        {
            break;
        }

        if (!fcs_is_ss_suit_true(next_card, card))
        {
            num_true_seqs++;
        }

        card = next_card;
    }

    *num_true_seqs_out_ptr = num_true_seqs;

    return h+1;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_simple_simon_move_whole_stack_sequence_to_false_parent)
{
    /*
     * stack_idx - the source stack index
     * cards_num - number of cards in "stack"
     * ds - the dest stack index
     * dest_cards_num - number of cards in "ds".
     * card - the current card
     * rank - its rank
     * suit - its suit
     * dest_card - the card at the top of "ds".
     * h - the height of the current card on "stack"
     * num_true_seqs - the number of true sequences on the current
     *                 false sequence
     * */
    SIMPS_define_vacant_stacks_accessors();

    STACK_SOURCE_LOOP_START(1)
        int num_true_seqs;
        int h = get_seq_h(col, &num_true_seqs);
        if (calc_max_simple_simon_seq_move(num_vacant_stacks) < num_true_seqs)
        {
            continue;
        }

        /* This means that the loop exited prematurely and the stack does
         * not contain a sequence. */
        if ((h > 0)
            || (calc_max_simple_simon_seq_move(num_vacant_stacks) < num_true_seqs)
        )
        {
            continue;
        }

        const fcs_card_t card = fcs_col_get_card(col, h);

        STACK_DEST_LOOP_START(1)
            const fcs_card_t dest_card =
                fcs_col_get_card(dest_col, dest_cards_num-1);
            if (!
                (fcs_is_ss_false_parent(dest_card, card))
               )
            {
                continue;
            }
            /* This is a suitable parent - let's check if we
             * have enough empty stacks to make the move feasible */
            /* We can do it - so let's move */

            sfs_check_state_begin();

            my_copy_stack(stack_idx);
            my_copy_stack(ds);

            fcs_move_sequence(ds, stack_idx, h, cards_num-1);

            sfs_check_state_end();
        STACK_DEST_LOOP_END()
    STACK_SOURCE_LOOP_END()

    return;
}

static GCC_INLINE void populate_seq_points(
    const fcs_cards_column_t dest_col,
    const int dc,
    int * const seq_points,
    int * const above_num_true_seqs,
    int * const num_separate_false_seqs_out_ptr
)
{
    const int dest_cards_num = fcs_col_len(dest_col);
    int num_separate_false_seqs = 0;
    above_num_true_seqs[num_separate_false_seqs] = 1;
    fcs_card_t above_card = fcs_col_get_card(dest_col, dest_cards_num-1);
    for (int above_c = dest_cards_num-2 ; above_c > dc ; above_c--)
    {
        const fcs_card_t up_above_card = fcs_col_get_card(dest_col, above_c);
        if (! fcs_is_ss_false_parent(up_above_card, above_card))
        {
            seq_points[num_separate_false_seqs++] = above_c+1;
            above_num_true_seqs[num_separate_false_seqs] = 1;
        }
        above_num_true_seqs[num_separate_false_seqs] +=
            ! fcs_is_ss_suit_true(up_above_card, above_card);
        above_card = up_above_card;
    }

    if (dc < dest_cards_num - 1)
    {
        seq_points[num_separate_false_seqs++] = dc+1;
    }

    *num_separate_false_seqs_out_ptr = num_separate_false_seqs;
}

static GCC_INLINE const int false_seq_index_loop(
    fc_solve_soft_thread_t * const soft_thread,
    fc_solve_instance_t * const instance,
    fcs_kv_state_t * const raw_ptr_state_raw,
    int num_vacant_stacks,
    const fcs_cards_column_t col,
    const int num_separate_false_seqs,
    int * const seq_points,
    int * const stacks_map,
    int * const above_num_true_seqs,
    int * const junk_move_to_stacks,
    int * const after_junk_num_freestacks_ptr,
    const fcs_bool_t behaviour_flag
    )
{
    SIMPS_SET_GAME_PARAMS();

    int false_seq_index;
    int after_junk_num_freestacks = num_vacant_stacks;

    for (false_seq_index = 0 ;
        false_seq_index < num_separate_false_seqs ;
        false_seq_index++)
    {
        /* Find a suitable place to put it */
        int clear_junk_dest_stack;
        const fcs_card_t the_card =
            (fcs_col_get_card(col, seq_points[false_seq_index]))
            ;
        const int the_num_true_seqs =
            above_num_true_seqs[false_seq_index];

        /* Let's try to find a suitable parent on top one of the stacks */
        for(clear_junk_dest_stack=0;
            clear_junk_dest_stack < LOCAL_STACKS_NUM;
            clear_junk_dest_stack++
        )
        {
            const fcs_cards_column_t clear_junk_dest_col = fcs_state_get_col(state, clear_junk_dest_stack);
            const int clear_junk_stack_len = fcs_col_len(clear_junk_dest_col);

            if (! ((clear_junk_stack_len > 0) && (! stacks_map[clear_junk_dest_stack])))
            {
                continue;
            }

            const fcs_card_t clear_junk_dest_card = fcs_col_get_card(clear_junk_dest_col, clear_junk_stack_len-1);
            if (fcs_is_ss_false_parent(clear_junk_dest_card, the_card))
            {
                if (calc_max_simple_simon_seq_move(after_junk_num_freestacks) >= the_num_true_seqs)
                {
                    stacks_map[clear_junk_dest_stack] = TRUE;
                    break;
                }
            }
        }

        if (clear_junk_dest_stack == LOCAL_STACKS_NUM)
        {
            /* Check if there is a vacant stack */
            if (behaviour_flag || (!
                (
                    (num_vacant_stacks > 0)
                    &&
                    (calc_max_simple_simon_seq_move(after_junk_num_freestacks-1) >= the_num_true_seqs)
                )
            ))
            {
                break;
            }
            /* Find an empty stack and designate it as the destination for the junk */
            for(
                clear_junk_dest_stack = 0;
                clear_junk_dest_stack < LOCAL_STACKS_NUM;
                clear_junk_dest_stack++
            )
            {
                if ((fcs_col_len(fcs_state_get_col(state, clear_junk_dest_stack)) == 0) && (! stacks_map[clear_junk_dest_stack]))
                {
                    stacks_map[clear_junk_dest_stack] = TRUE;
                    break;
                }
            }
            after_junk_num_freestacks--;
        }

        junk_move_to_stacks[false_seq_index] = clear_junk_dest_stack;
    }

    *after_junk_num_freestacks_ptr = after_junk_num_freestacks;
    return false_seq_index;
}

#define IS_false_seq_index_loop(col, behavior_flag) \
    (false_seq_index_loop( \
        soft_thread, \
        instance, \
        raw_ptr_state_raw, \
        num_vacant_stacks, \
        col, \
        num_separate_false_seqs, \
        seq_points, \
        stacks_map, \
        above_num_true_seqs, \
        junk_move_to_stacks, \
        &after_junk_num_freestacks, \
        behavior_flag \
    ) == num_separate_false_seqs) \

DECLARE_MOVE_FUNCTION(fc_solve_sfs_simple_simon_move_sequence_to_true_parent_with_some_cards_above)
{
    /*
     * stack - the source stack index
     * cards_num - the number of cards in "stack"
     * h - the height of the current card in "stack"
     * card - the card in height "h"
     * suit - its suit
     * rank - its rank
     * ds - the destionation stack index
     * dest_cards_num - the number of cards in "ds"
     * dc - the index of the current card in "ds".
     * num_separate_false_seqs - this variable tells how many distinct false
     *      sequences exist above the true parent
     * above_num_true_seqs[] - the number of true sequences in each false
     *      sequence
     * seq_points[] - the separation points of the false sequences (i.e: where
     *      they begin and end)
     * stacks_map[] - a boolean map that indicates if one can place a card
     *      on this stack or is it already taken.
     * junk_move_to_stacks[] - the stacks to move each false sequence of the
     *      junk to.
     * false_seq_index - an iterator to hold the index of the current false
     *      sequence.
     * after_junk_num_freestacks - this variable holds the number of stacks
     *      that remained unoccupied during and after the process of moving
     *      the junk sequences to different stacks.
     *
     * */

    SIMPS_define_vacant_stacks_accessors();

    STACK_SOURCE_LOOP_START(1)
        fcs_card_t card = fcs_col_get_card(col, cards_num-1);

        int num_true_seqs = 1;

        for (int h=cards_num-2 ; h>=-1 ; h--)
        {
            fcs_card_t h_above_card;
            fcs_bool_t should_search = TRUE;
            fcs_bool_t should_increment_num_true_seqs = FALSE;
            fcs_bool_t should_break = FALSE;
            /* Stop if we reached the bottom of the stack */
            if (h == -1)
            {
                should_break = TRUE;
            }
            else
            {
                h_above_card = fcs_col_get_card(col, h);
                /* If this is no longer a sequence - move to the next stack */
                if (! fcs_is_ss_false_parent(h_above_card, card))
                {
                    should_break = TRUE;
                }
                else if ((should_search = (!fcs_is_ss_suit_true(h_above_card, card))))
                {
                    should_increment_num_true_seqs = TRUE;
                }
            }
            if (should_search)
            {
                DS_DC_LOOP_START(1,1, fcs_is_ss_true_parent)
                    /* This is a suitable parent - let's check if there's a sequence above it. */

                    /*
                     * above_c - the height of the card that is to be checked.
                     * above_card - the card at height above_c+1
                     * up_above_card - the card at height above_c
                     *
                     * */
                    int num_separate_false_seqs;
                    int seq_points[MAX_NUM_CARDS_IN_A_STACK];
                    int above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK];

                    populate_seq_points(dest_col, dc, seq_points,
                        above_num_true_seqs, &num_separate_false_seqs);

                    fcs_bool_t stacks_map[STACKS_MAP_LEN];
                    init_stacks_map(stacks_map, stack_idx, ds);

                    int junk_move_to_stacks[MAX_NUM_STACKS];
                    int after_junk_num_freestacks;

                    if (!
                        (
                            IS_false_seq_index_loop(dest_col, FALSE)
                                &&
                                (calc_max_simple_simon_seq_move(after_junk_num_freestacks) >= num_true_seqs)
                        )
                    )
                    {
                        continue;
                    }
                    /*
                     * We can do it - so let's move everything.
                     * Notice that we only put the child in a different stack
                     * then the parent and let it move to the parent in the
                     * next iteration of the program
                     * */

                    sfs_check_state_begin();

                    my_copy_stack(ds);
                    my_copy_stack(stack_idx);


                    /* Move the junk cards to their place */

                    for (int seq_index = 0;
                        seq_index < num_separate_false_seqs;
                        seq_index++
                    )
                    {
                        const int dest_index = junk_move_to_stacks[seq_index];
                        my_copy_stack(dest_index);

                        fcs_move_sequence(
                            dest_index,
                            ds,
                            seq_points[seq_index],
                            ((seq_index == 0) ? (dest_cards_num-1) : (seq_points[seq_index-1]-1))
                        );
                    }

                    /* Move the source seq on top of the dest seq */
                    fcs_move_sequence(ds, stack_idx, h+1, cards_num-1);

                    sfs_check_state_end();
                DS_DC_LOOP_END()
            }

            if (should_break)
            {
                break;
            }
            if (should_increment_num_true_seqs)
            {
                num_true_seqs++;
            }
            card = h_above_card;
        }
    STACK_SOURCE_LOOP_END()

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_simple_simon_move_sequence_with_some_cards_above_to_true_parent)
{
    SIMPS_define_vacant_stacks_accessors();

    STACK_SOURCE_LOOP_START(1)
        for (int src_card_height = cards_num-1 ; src_card_height >= 0 ; src_card_height--)
        {
            int end_of_src_seq;

            const fcs_card_t h_card = fcs_col_get_card(col, src_card_height);
            fcs_card_t card = h_card;

            int num_true_seqs = 1;

            for (end_of_src_seq = src_card_height+1; end_of_src_seq < cards_num ; end_of_src_seq++)
            {
                const fcs_card_t above_card
                    = fcs_col_get_card(col, end_of_src_seq);
                if (!fcs_is_ss_false_parent(card, above_card))
                {
                    break;
                }
                if (!fcs_is_ss_suit_true(card, above_card))
                {
                    num_true_seqs++;
                }
                card = above_card;
            }

            if (end_of_src_seq == cards_num)
            {
                continue;
            }

            /* Split the cards above it into false sequences */


            STACK_DEST_LOOP_START(1)
                if (!fcs_is_ss_true_parent(fcs_col_get_card(dest_col, dest_cards_num-1), h_card))
                {
                    continue;
                }

                /* This is a suitable parent - let's check if we
                 * have enough empty stacks to make the move feasible */
                int num_separate_false_seqs;
                int seq_points[MAX_NUM_CARDS_IN_A_STACK];
                int above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK];

                populate_seq_points(col, end_of_src_seq-1, seq_points,
                    above_num_true_seqs, &num_separate_false_seqs);

                fcs_bool_t stacks_map[STACKS_MAP_LEN];
                init_stacks_map(stacks_map, stack_idx, ds);

                int junk_move_to_stacks[MAX_NUM_STACKS];
                int after_junk_num_freestacks;

                if (!
                    (
                        IS_false_seq_index_loop(col, FALSE)
                        &&
                        (calc_max_simple_simon_seq_move(after_junk_num_freestacks) > num_true_seqs)
                    )
                )
                {
                    continue;
                }

                sfs_check_state_begin();

                my_copy_stack(stack_idx);
                my_copy_stack(ds);

                /* Let's boogie - we can move everything */

                /* Move the junk cards to their place */

                for (int seq_index = 0;
                    seq_index < num_separate_false_seqs;
                    seq_index++
                )
                {
                    const int dest_index = junk_move_to_stacks[seq_index];

                    my_copy_stack(dest_index);

                    fcs_move_sequence(
                        dest_index,
                        stack_idx,
                        seq_points[seq_index],
                        ((seq_index == 0) ? (cards_num-1) : (seq_points[seq_index-1]-1))
                    );
                }

                fcs_move_sequence(ds, stack_idx, src_card_height, end_of_src_seq-1);

                sfs_check_state_end();
            STACK_DEST_LOOP_END()
        }
    STACK_SOURCE_LOOP_END()

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_simple_simon_move_sequence_with_junk_seq_above_to_true_parent_with_some_cards_above)
{
    /*
     * stack_idx - the source stack index
     * cards_num - the number of cards in "stack"
     * h - the height of the current card in "stack".
     * card - the current card in "stack"
     * suit - its suit
     * rank - its rank
     * ds - the index of the destination stack
     * dest_cards_num - the number of cards in "ds".
     * dc - the height of the current card in "ds".
     * num_separate_false_seqs - the number of false sequences
     * seq_points[] - the places in which the false sequences of the junk begin
     *      and end
     * false_seq_index - an iterator that marks the index of the current
     *      false sequence
     * stacks_map[] - a map of booleans that indicates if one can place a card
     *      on this stack or is already taken.
     * above_num_true_seqs[] - the number of true sequences in each false
     *      sequence
     * num_src_junk_true_seqs - the number of true seqs in the false seq above
     *      the source card.
     * end_of_junk - the height marking the end of the source junk.
     * num_true_seqs - the number of true sequences in the false seq which we
     *      wish to move.
     * */
    SIMPS_define_vacant_stacks_accessors();

    STACK_SOURCE_LOOP_START(1)
        int num_src_junk_true_seqs = 1;

        int h = get_seq_h(col, &num_src_junk_true_seqs);
        if (! h)
        {
            continue;
        }

        fcs_card_t card = fcs_col_get_card(col, h);

        const int end_of_junk = (--h);
        int num_true_seqs = 1;

        for(;h>-1;h--)
        {
            const fcs_card_t next_card = fcs_col_get_card(col, h);
            if (!fcs_is_ss_false_parent(next_card, card))
            {
                card = next_card;
                break;
            }
            if (!fcs_is_ss_suit_true(next_card, card))
            {
                num_true_seqs++;
            }
            card = next_card;
        }

        /* Start at the card below the top one, so we will
         * make sure there's at least some junk above it
         * */
        DS_DC_LOOP_START(2,2, fcs_is_ss_true_parent)
            /* This is a suitable parent - let's check if there's a sequence above it. */
            int num_separate_false_seqs;
            int seq_points[MAX_NUM_CARDS_IN_A_STACK];
            int above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK];

            populate_seq_points(dest_col, dc, seq_points,
                above_num_true_seqs, &num_separate_false_seqs);

            fcs_bool_t stacks_map[STACKS_MAP_LEN];
            init_stacks_map(stacks_map, stack_idx, ds);

            int after_junk_num_freestacks = num_vacant_stacks;

            int junk_move_to_stacks[MAX_NUM_STACKS];
            int false_seq_index;
            for (false_seq_index = 0 ;
                false_seq_index < num_separate_false_seqs+1 ;
                false_seq_index++)
            {
                /* Find a suitable place to put it */

                fcs_card_t the_card =
                (
                    (fcs_card_t)
                    (
                        (false_seq_index == num_separate_false_seqs) ?
                            (fcs_col_get_card(col, end_of_junk+1)) :
                            (fcs_col_get_card(dest_col, seq_points[false_seq_index]))
                    )
                )
                ;

                int the_num_true_seqs =
                    (
                        (false_seq_index == num_separate_false_seqs) ?
                            num_src_junk_true_seqs :
                            above_num_true_seqs[false_seq_index]
                    );

                /* Let's try to find a suitable parent on top one of the stacks */
                int clear_junk_dest_stack;
                for(clear_junk_dest_stack=0;
                    clear_junk_dest_stack < LOCAL_STACKS_NUM;
                    clear_junk_dest_stack++
                   )
                {
                    const fcs_cards_column_t clear_junk_dest_col = fcs_state_get_col(state, clear_junk_dest_stack);
                    const int clear_junk_stack_len = fcs_col_len(clear_junk_dest_col);

                    if ((clear_junk_stack_len > 0) && (! stacks_map[clear_junk_dest_stack]))
                    {
                        fcs_card_t clear_junk_dest_card;

                        clear_junk_dest_card = fcs_col_get_card(clear_junk_dest_col, clear_junk_stack_len-1);
                        if (fcs_is_ss_false_parent(clear_junk_dest_card, the_card))
                        {
                            if (calc_max_simple_simon_seq_move(after_junk_num_freestacks) >= the_num_true_seqs)
                            {
                                stacks_map[clear_junk_dest_stack] = TRUE;
                                break;
                            }
                        }
                    }
                }

                if (clear_junk_dest_stack == LOCAL_STACKS_NUM)
                {
                    /* Check if there is a vacant stack */
                    if (!
                        (
                            (num_vacant_stacks > 0)
                                &&
                            (calc_max_simple_simon_seq_move(after_junk_num_freestacks-1) >= the_num_true_seqs)
                        )
                    )
                    {
                        break;
                    }
                    /* Find an empty stack and designate it as the destination for the junk */
                    for(
                        clear_junk_dest_stack = 0;
                        clear_junk_dest_stack < LOCAL_STACKS_NUM;
                        clear_junk_dest_stack++
                       )
                    {
                        if ((fcs_col_len(fcs_state_get_col(state, clear_junk_dest_stack)) == 0) && (! stacks_map[clear_junk_dest_stack]))
                        {
                            stacks_map[clear_junk_dest_stack] = TRUE;
                            break;
                        }
                    }
                    after_junk_num_freestacks--;
                }

                junk_move_to_stacks[false_seq_index] = clear_junk_dest_stack;
            }

            if (!(
                    (false_seq_index == num_separate_false_seqs+1)
                        &&
                (calc_max_simple_simon_seq_move(after_junk_num_freestacks) >= num_true_seqs)
                )
            )
            {
                continue;
            }
            /* We can do it - so let's move everything */

            sfs_check_state_begin();

            my_copy_stack(stack_idx);
            my_copy_stack(ds);


            /* Move the junk cards to their place */

            for (int seq_index = 0;
                seq_index < num_separate_false_seqs+1;
                seq_index++
            )
            {
                int start;
                int end;
                int src_stack;

                if (seq_index == num_separate_false_seqs)
                {
                    start = end_of_junk+1;
                    end = cards_num-1;
                    src_stack = stack_idx;
                }
                else
                {
                    start = seq_points[seq_index];
                    end = ((seq_index == 0) ? (dest_cards_num-1) : (seq_points[seq_index-1]-1));
                    src_stack = ds;
                }

                my_copy_stack(src_stack);

                const int dest_index = junk_move_to_stacks[seq_index];
                my_copy_stack(dest_index);

                fcs_move_sequence(dest_index, src_stack, start, end);
            }

            /* Move the source seq on top of the dest seq */
            fcs_move_sequence(ds, stack_idx, h, end_of_junk);

            sfs_check_state_end();
        DS_DC_LOOP_END()
    STACK_SOURCE_LOOP_END()

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_simple_simon_move_whole_stack_sequence_to_false_parent_with_some_cards_above)
{
    /*
     * stack_idx - the source stack index
     * cards_num - the number of cards in "stack"
     * h - the height of the current card in stack
     * card - the current card
     * suit - its suit
     * rank - its rank
     * ds - the destination stack index.
     * dest_cards_num - the number of cards in it.
     * dc - the height of the card in "ds".
     * num_separate_false_seqs - this variable tells how many distinct false
     *      sequences exist above the false parent
     * above_num_true_seqs[] - the number of true sequences in each false
     *      sequence
     * seq_points[] - the separation points of the false sequences (i.e: where
     *      they begin and end)
     * stacks_map[] - a boolean map that indicates if one can place a card
     *      on this stack or is it already taken.
     * junk_move_to_stacks[] - the stacks to move each false sequence of the
     *      junk to.
     * false_seq_index - an iterator to hold the index of the current false
     *      sequence.
     * after_junk_num_freestacks - a variable that holds the number of stacks
     *      that are left unoccupied as part of the junk disposal process.
     *
     * */
    SIMPS_define_vacant_stacks_accessors();

    STACK_SOURCE_LOOP_START(1)
        int num_true_seqs;
        if (get_seq_h(col, &num_true_seqs))
        {
            continue;
        }
#define h 0
        fcs_card_t card = fcs_col_get_card(col, h);

        DS_DC_LOOP_START(1,1, fcs_is_ss_false_parent)
            /* This is a suitable parent - let's check if there's a sequence above it. */
            int num_separate_false_seqs;
            int seq_points[MAX_NUM_CARDS_IN_A_STACK];
            int above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK];

            populate_seq_points(dest_col, dc, seq_points,
                above_num_true_seqs, &num_separate_false_seqs);

            fcs_bool_t stacks_map[STACKS_MAP_LEN];
            init_stacks_map(stacks_map, stack_idx, ds);

            int junk_move_to_stacks[MAX_NUM_STACKS];
            int after_junk_num_freestacks;

            if (! IS_false_seq_index_loop(dest_col, TRUE))
            {
                continue;
            }
            /* This is a suitable parent - let's check if we
             * have enough empty stacks to make the move feasible */
            if (calc_max_simple_simon_seq_move(num_vacant_stacks) < num_true_seqs)
            {
                continue;
            }
            /* We can do it - so let's move */

            sfs_check_state_begin();

            my_copy_stack(stack_idx);
            my_copy_stack(ds);

            /* Move the junk cards to their place */

            for (int seq_index = 0;
                seq_index < num_separate_false_seqs;
                seq_index++
            )
            {
                const int src_stack_idx = ds;
                const int dest_stack_idx
                    = junk_move_to_stacks[
                        seq_index
                    ];

                my_copy_stack(src_stack_idx);
                my_copy_stack(dest_stack_idx);

                fcs_move_sequence(
                    dest_stack_idx,
                    src_stack_idx,
                    seq_points[seq_index],
                    ((seq_index == 0) ? (dest_cards_num-1) : (seq_points[seq_index-1]-1))
                );
            }

            fcs_move_sequence( ds, stack_idx, h, cards_num-1);

            sfs_check_state_end();
#undef h
        DS_DC_LOOP_END()
    STACK_SOURCE_LOOP_END()

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_simple_simon_move_sequence_to_parent_on_the_same_stack)
{
    SIMPS_define_vacant_stacks_accessors();

    STACK_SOURCE_LOOP_START(3)
        /* Search for a parent card */
        for(int parent_card_height = 0 ; parent_card_height < cards_num-1 ; parent_card_height++)
        {
            const fcs_card_t parent_card
                = fcs_col_get_card(col, parent_card_height);
            if (
                fcs_is_ss_true_parent(
                    parent_card,
                    fcs_col_get_card(col, parent_card_height+1)
                    )
               )
            {
                continue;
            }

            for(int child_card_height = parent_card_height + 2 ; child_card_height < cards_num ; child_card_height++)
            {
                if (! fcs_is_ss_true_parent(
                        parent_card,
                        fcs_col_get_card(col, child_card_height)
                        )
                   )
                {
                    continue;
                }
                /* We have a matching parent and child cards */
#if 0
                printf("Stack %i, Parent %i, Child %i\n", stack_idx, parent_card_height, child_card_height);
                fflush(stdout);
#endif

                /*
                 * Now let's try to find stacks to place the cards above
                 * the child card.
                 * */

                int end_of_child_seq = child_card_height;
                int child_num_true_seqs = 1;
                while ((end_of_child_seq+1 < cards_num) &&
                      fcs_is_ss_false_parent(
                            fcs_col_get_card(col, end_of_child_seq),
                            fcs_col_get_card(col, end_of_child_seq+1)
                       )
                      )
                {
                    child_num_true_seqs += (!fcs_is_ss_true_parent(
                            fcs_col_get_card(col, end_of_child_seq),
                            fcs_col_get_card(col, end_of_child_seq+1)
                       ));
                    end_of_child_seq++;
                }

                int num_separate_false_seqs;
                int seq_points[MAX_NUM_CARDS_IN_A_STACK];
                int above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK];

                populate_seq_points(col, end_of_child_seq, seq_points,
                    above_num_true_seqs, &num_separate_false_seqs);

                /* Add the child to the seq_points */
                const int child_seq_index = num_separate_false_seqs;
                above_num_true_seqs[num_separate_false_seqs] = child_num_true_seqs;
                seq_points[num_separate_false_seqs++] = child_card_height;

                /* Add the cards between the parent and the child to the seq_points */

                fcs_card_t above_card = fcs_col_get_card(col, child_card_height-1);
                above_num_true_seqs[num_separate_false_seqs] = 1;

                {
                    int above_c;

                    for(above_c = child_card_height-2;
                        above_c > parent_card_height ;
                        above_c--
                    )
                    {
                        const fcs_card_t up_above_card = fcs_col_get_card(col, above_c);
                        if (! fcs_is_ss_false_parent(up_above_card, above_card))
                        {
                            seq_points[num_separate_false_seqs++] = above_c+1;
                            above_num_true_seqs[num_separate_false_seqs] = 1;
                        }
                        above_num_true_seqs[num_separate_false_seqs] +=
                            ! fcs_is_ss_suit_true(up_above_card, above_card);
                        above_card = up_above_card;
                    }

                    if (parent_card_height < child_card_height - 1)
                    {
                        seq_points[num_separate_false_seqs++] = above_c+1;
                    }
                }

                fcs_bool_t stacks_map[STACKS_MAP_LEN];
                init_stacks_map(stacks_map, stack_idx, stack_idx);

                int junk_move_to_stacks[MAX_NUM_STACKS];
                int after_junk_num_freestacks;

                /* Let's check if we can move the child after we are done
                 * moving all the junk cards */
                if (!
                    (
                        IS_false_seq_index_loop(col, FALSE)
                        &&
                        (calc_max_simple_simon_seq_move(after_junk_num_freestacks) >= child_num_true_seqs)
                    )
                )
                {
                    continue;
                }
                /* We can do it - so let's move everything */

                sfs_check_state_begin();

                /* Move the junk cards to their place */

                my_copy_stack(stack_idx);

                for (int seq_index = 0;
                     seq_index < num_separate_false_seqs;
                     seq_index++
                    )
                {
                    const int dest_idx
                        = junk_move_to_stacks[seq_index];
                    my_copy_stack(dest_idx);

                    fcs_move_sequence (
                        dest_idx,
                        stack_idx,
                        seq_points[seq_index],
                        ((seq_index == 0) ? (cards_num-1) : (seq_points[seq_index-1]-1))
                    );
                }

                {
                    const int source_idx
                        = junk_move_to_stacks[child_seq_index];

                    const fcs_cards_column_t move_junk_to_col
                        = fcs_state_get_col(new_state, source_idx);

                    const int end = fcs_col_len(move_junk_to_col)-1;

                    my_copy_stack(source_idx);

                    fcs_move_sequence(
                        stack_idx,
                        source_idx,
                        end-(end_of_child_seq-child_card_height),
                        end
                    );
                }

                sfs_check_state_end();
            }
        }
    STACK_SOURCE_LOOP_END()

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_simple_simon_move_sequence_to_false_parent)
{
    /*
     * stack - the source stack index
     * cards_num - number of cards in "stack"
     * ds - the dest stack index
     * dest_cards_num - number of cards in "ds".
     * card - the current card
     * next_card - the next card on the stack.
     * h - the height of the current card on "stack"
     * num_true_seqs - the number of true sequences on the current
     *                 false sequence
     * */

    SIMPS_define_vacant_stacks_accessors();

    STACK_SOURCE_LOOP_START(1)
        int num_true_seqs;
        int h = get_seq_h(col, &num_true_seqs);
        if (calc_max_simple_simon_seq_move(num_vacant_stacks) < num_true_seqs)
        {
            continue;
        }
        const fcs_card_t card = fcs_col_get_card(col, h);

        /* take the sequence and try and put it on another stack */
        STACK_DEST_LOOP_START(1)
            /* If this is a suitable parent - let's check if we
             * have enough empty stacks to make the move feasible */
            if (!fcs_is_ss_false_parent(
                        fcs_col_get_card( dest_col, dest_cards_num-1),
                        card
                   )
            )
            {
                continue;
            }

            /* We can do it - so let's perform the move */
            sfs_check_state_begin();

            my_copy_stack(stack_idx);
            my_copy_stack(ds);

            fcs_move_sequence(ds, stack_idx, h, cards_num-1);
            sfs_check_state_end();
        STACK_DEST_LOOP_END()
    STACK_SOURCE_LOOP_END()

    return;
}

#undef state
#undef new_state

#endif /* #ifdef FCS_DISABLE_SIMPLE_SIMON */

