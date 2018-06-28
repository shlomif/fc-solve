/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// simpsim.c - a module that contains Simple Simon moves.
#include "simpsim.h"
#include "scans.h"

// This is a fallback in case this module is still compiled with
// FCS_DISABLE_SIMPLE_SIMON.
#ifdef FCS_DISABLE_SIMPLE_SIMON
char fc_solve_simple_simon_nothing;
#else

#include "meta_move_funcs_helpers.h"

static inline bool fcs_is_ss_false_parent(
    const fcs_card parent, const fcs_card child)
{
    return (fcs_card_rank(parent) == fcs_card_rank(child) + 1);
}

static inline bool fcs_is_ss_suit_true(
    const fcs_card parent, const fcs_card child)
{
    return (fcs_card_suit(parent) == fcs_card_suit(child));
}

static inline bool fcs_is_ss_true_parent(
    const fcs_card parent, const fcs_card child)
{
    return (fcs_is_ss_false_parent(parent, child) &&
            fcs_is_ss_suit_true(parent, child));
}

#define STACK_SOURCE_LOOP_START(min_num_cards)                                 \
    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)         \
    {                                                                          \
        const_AUTO(col, fcs_state_get_col(state, stack_idx));                  \
        const int cards_num = fcs_col_len(col);                                \
        if (cards_num < min_num_cards)                                         \
        {                                                                      \
            continue;                                                          \
        }

#define STACK_SOURCE_LOOP_END() }

#define STACK_DEST_LOOP_START(min_num_cards)                                   \
    for (int ds = 0; ds < LOCAL_STACKS_NUM; ds++)                              \
    {                                                                          \
        if (ds == stack_idx)                                                   \
        {                                                                      \
            continue;                                                          \
        }                                                                      \
                                                                               \
        const_AUTO(dest_col, fcs_state_get_col(state, ds));                    \
        const int dest_cards_num = fcs_col_len(dest_col);                      \
                                                                               \
        if (dest_cards_num < min_num_cards)                                    \
        {                                                                      \
            continue;                                                          \
        }

#define STACK_DEST_LOOP_END() }

#ifndef HARD_CODED_NUM_STACKS
#define SIMPS_SET_GAME_PARAMS() SET_GAME_PARAMS()
#else
#define SIMPS_SET_GAME_PARAMS()
#endif

#define SIMPS_define_accessors()                                               \
    tests_define_accessors();                                                  \
    SIMPS_SET_GAME_PARAMS()

#define SIMPS_define_vacant_stacks_accessors()                                 \
    SIMPS_define_accessors();                                                  \
    const fcs_game_limit num_vacant_stacks = soft_thread->num_vacant_stacks

#define CALC_POSITIONS_BY_RANK()                                               \
    const fcs_pos_by_rank *const positions_by_rank =                           \
        (const fcs_pos_by_rank *)fc_solve_calc_positions_by_rank_location(     \
            soft_thread)

#define STACKS_MAP_LEN MAX_NUM_STACKS

static inline void init_stacks_map(
    bool *const stacks_map, const int stack_idx, const int ds)
{
    for (int i = 0; i < STACKS_MAP_LEN; i++)
    {
        stacks_map[i] = FALSE;
    }
    stacks_map[stack_idx] = stacks_map[ds] = TRUE;
}

typedef struct
{
    size_t num_separate_false_seqs;
    int seq_points[MAX_NUM_CARDS_IN_A_STACK];
    size_t junk_move_to_stacks[MAX_NUM_STACKS];
    int after_junk_num_freestacks;
    size_t above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK];
} sequences_analysis;

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
    fcs_card card = fcs_col_get_card(col, cards_num - 1);

    /* Check if the top 13 cards are a sequence */
    int a;
    for (a = 2; a <= FCS_MAX_RANK; a++)
    {
        const fcs_card above_card = fcs_col_get_card(col, cards_num - a);
        if (!fcs_is_ss_true_parent(above_card, card))
        {
            break;
        }
        card = above_card;
    }
    if (a > FCS_MAX_RANK)
    {
        /* We can move this sequence up there */
        sfs_check_state_begin();

        my_copy_stack(stack_idx);

        var_AUTO(new_src_col, fcs_state_get_col(new_state_key, stack_idx));
        for (a = 0; a < FCS_MAX_RANK; a++)
        {
            fcs_col_pop_top(new_src_col);
        }

        const int suit = fcs_card_suit(card);
        fcs_set_foundation(new_state_key, suit, FCS_MAX_RANK);

        fcs_move_stack_non_seq_push(
            moves, FCS_MOVE_TYPE_SEQ_TO_FOUNDATION, stack_idx, suit);

        sfs_check_state_end();
    }
    STACK_SOURCE_LOOP_END()
}

#define LOOK_FOR_TRUE_PARENT_with_ds_dc__START(card)                           \
    if (!fcs_card_is_king(card))                                               \
    {                                                                          \
        const fcs_pos_by_rank pos = positions_by_rank[FCS_POS_IDX(             \
            fcs_card_rank(card) + 1, fcs_card_suit(card))];                    \
        const int ds = pos.col;                                                \
                                                                               \
        if (ds != stack_idx)                                                   \
        {                                                                      \
            const int dc = pos.height;                                         \
            const_AUTO(dest_col, fcs_state_get_col(state, ds));                \
            const int dest_cards_num = fcs_col_len(dest_col);

#define LOOK_FOR_TRUE_PARENT_with_ds_dc__END()                                 \
    }                                                                          \
    }

#define LOOK_FOR_TRUE_PARENT_AT_TOP__START(card)                               \
    LOOK_FOR_TRUE_PARENT_with_ds_dc__START(card) if (dc == dest_cards_num - 1) \
    {
#define LOOK_FOR_TRUE_PARENT_AT_TOP__END()                                     \
    }                                                                          \
    LOOK_FOR_TRUE_PARENT_with_ds_dc__END()

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

    CALC_POSITIONS_BY_RANK();

    STACK_SOURCE_LOOP_START(1)
    /* Loop on the cards in the stack and try to look for a true
     * parent on top one of the stacks */
    fcs_card card = fcs_col_get_card(col, cards_num - 1);
    size_t num_true_seqs = 1;

    for (int h = cards_num - 2; h >= -1; h--)
    {
        LOOK_FOR_TRUE_PARENT_AT_TOP__START(card)
        /* This is a suitable parent - let's check if we
         * have enough empty stacks to make the move feasible */
        /* We can do it - so let's move */

        sfs_check_state_begin();
        copy_two_stacks(stack_idx, ds);
        fcs_move_sequence(ds, stack_idx, cards_num - h - 1);
        sfs_check_state_end();
        LOOK_FOR_TRUE_PARENT_AT_TOP__END()
        /* Stop if we reached the bottom of the stack */
        if (h == -1)
        {
            break;
        }

        const fcs_card prev_card = card;
        card = fcs_col_get_card(col, h);
        /* If this is no longer a sequence - move to the next stack */
        if (!fcs_is_ss_false_parent(card, prev_card))
        {
            break;
        }
        if (!fcs_is_ss_suit_true(card, prev_card))
        {
            ++num_true_seqs;
            /* We can no longer perform the move so go to the next
             * stack. */
            if (calc_max_simple_simon_seq_move(num_vacant_stacks) <
                num_true_seqs)
            {
                break;
            }
        }
    }
    STACK_SOURCE_LOOP_END()
}

static inline int get_seq_h(
    const fcs_const_cards_column col, size_t *const num_true_seqs_out_ptr)
{
    const int cards_num = fcs_col_len(col);

    fcs_card card = fcs_col_get_card(col, cards_num - 1);
    size_t num_true_seqs = 1;

    int h;
    /* Stop if we reached the bottom of the stack */
    for (h = cards_num - 2; h > -1; h--)
    {
        const fcs_card next_card = fcs_col_get_card(col, h);
        /* If this is no longer a sequence - move to the next stack */
        if (!fcs_is_ss_false_parent(next_card, card))
        {
            break;
        }

        if (!fcs_is_ss_suit_true(next_card, card))
        {
            ++num_true_seqs;
        }

        card = next_card;
    }

    *num_true_seqs_out_ptr = num_true_seqs;

    return h + 1;
}

DECLARE_MOVE_FUNCTION(
    fc_solve_sfs_simple_simon_move_whole_stack_sequence_to_false_parent)
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

    const size_t max_seq_move =
        calc_max_simple_simon_seq_move(num_vacant_stacks);

    STACK_SOURCE_LOOP_START(1)
    {
        size_t num_true_seqs;
        /* This means that the loop exited prematurely and the stack does
         * not contain a sequence. */
        if ((get_seq_h(col, &num_true_seqs) > 0) ||
            (max_seq_move < num_true_seqs))
        {
            continue;
        }
    }

#define h 0
    const fcs_card card = fcs_col_get_card(col, h);

    STACK_DEST_LOOP_START(1)
    const fcs_card dest_card = fcs_col_get_card(dest_col, dest_cards_num - 1);
    if (!(fcs_is_ss_false_parent(dest_card, card)))
    {
        continue;
    }
    /* This is a suitable parent - let's check if we
     * have enough empty stacks to make the move feasible */
    /* We can do it - so let's move */

    sfs_check_state_begin();
    copy_two_stacks(stack_idx, ds);
    fcs_move_sequence(ds, stack_idx, cards_num - h);
    sfs_check_state_end();
    STACK_DEST_LOOP_END()
#undef h
    STACK_SOURCE_LOOP_END()
}

/*
 * above_c - the height of the card that is to be checked.
 * above_card - the card at height above_c+1
 * up_above_card - the card at height above_c
 *
 */
static inline void generic_populate_seq_points(const fcs_cards_column dest_col,
    const int dc, sequences_analysis *const seqs, const int dest_cards_num)
{
    size_t num_separate_false_seqs = seqs->num_separate_false_seqs;
    seqs->above_num_true_seqs[num_separate_false_seqs] = 1;
    fcs_card above_card = fcs_col_get_card(dest_col, dest_cards_num - 1);
    for (int above_c = dest_cards_num - 2; above_c > dc; above_c--)
    {
        const fcs_card up_above_card = fcs_col_get_card(dest_col, above_c);
        if (!fcs_is_ss_false_parent(up_above_card, above_card))
        {
            seqs->seq_points[num_separate_false_seqs++] = above_c + 1;
            seqs->above_num_true_seqs[num_separate_false_seqs] = 1;
        }
        seqs->above_num_true_seqs[num_separate_false_seqs] +=
            !fcs_is_ss_suit_true(up_above_card, above_card);
        above_card = up_above_card;
    }

    if (dc <= dest_cards_num - 2)
    {
        seqs->seq_points[num_separate_false_seqs++] = dc + 1;
    }

    seqs->num_separate_false_seqs = num_separate_false_seqs;
}

static inline void populate_seq_points(const fcs_cards_column dest_col,
    const int dc, sequences_analysis *const seqs)
{
    seqs->num_separate_false_seqs = 0;
    generic_populate_seq_points(dest_col, dc, seqs, fcs_col_len(dest_col));
}

static inline bool generic_false_seq_index_loop(const int stacks_num,
    fcs_kv_state raw_state_raw, const int num_vacant_stacks,
    const fcs_cards_column col, sequences_analysis *const seqs,
    const int stack_idx, const int ds, const bool behavior_flag,
    const bool should_src_col, const fcs_card src_card,
    const size_t num_src_junk_true_seqs)
{
    const size_t num_separate_false_seqs = seqs->num_separate_false_seqs;
    bool stacks_map[STACKS_MAP_LEN];
    init_stacks_map(stacks_map, stack_idx, ds);

    int after_junk_num_freestacks = num_vacant_stacks;

    const size_t false_seq_index_limit =
        num_separate_false_seqs + (should_src_col ? 1 : 0);

    size_t false_seq_idx;

    for (false_seq_idx = 0; false_seq_idx < false_seq_index_limit;
         false_seq_idx++)
    {
        const bool is_ultimate_iter =
            (false_seq_idx == num_separate_false_seqs);

        /* Find a suitable place to put it */
        const fcs_card the_card =
            is_ultimate_iter
                ? src_card
                : fcs_col_get_card(col, seqs->seq_points[false_seq_idx]);

        const size_t the_num_true_seqs =
            is_ultimate_iter ? num_src_junk_true_seqs
                             : seqs->above_num_true_seqs[false_seq_idx];

        /* Let's try to find a suitable parent on top one of the stacks */
        int clear_junk_dest_stack;
        for (clear_junk_dest_stack = 0; clear_junk_dest_stack < stacks_num;
             ++clear_junk_dest_stack)
        {
            const fcs_const_cards_column clear_junk_dest_col =
                fcs_state_get_col(state, clear_junk_dest_stack);
            const int clear_junk_stack_len = fcs_col_len(clear_junk_dest_col);

            if (!((clear_junk_stack_len > 0) &&
                    (!stacks_map[clear_junk_dest_stack])))
            {
                continue;
            }

            if (fcs_is_ss_false_parent(fcs_col_get_card(clear_junk_dest_col,
                                           clear_junk_stack_len - 1),
                    the_card))
            {
                if (calc_max_simple_simon_seq_move(after_junk_num_freestacks) >=
                    the_num_true_seqs)
                {
                    goto found;
                }
            }
        }

        /* Check if there is a vacant stack */
        if (behavior_flag ||
            (!((num_vacant_stacks > 0) &&
                (calc_max_simple_simon_seq_move(
                     after_junk_num_freestacks - 1) >= the_num_true_seqs))))
        {
            break;
        }
        --after_junk_num_freestacks;
        /* Find an empty stack and designate it as the destination for the
         * junk */
        for (clear_junk_dest_stack = 0;; ++clear_junk_dest_stack)
        {
            if (fcs_state_col_is_empty(state, clear_junk_dest_stack) &&
                (!stacks_map[clear_junk_dest_stack]))
            {
                break;
            }
        }

    found:
        stacks_map[clear_junk_dest_stack] = TRUE;
        seqs->junk_move_to_stacks[false_seq_idx] = clear_junk_dest_stack;
    }

    seqs->after_junk_num_freestacks = after_junk_num_freestacks;
    return (false_seq_idx == false_seq_index_limit);
}

static inline bool false_seq_index_loop(const int stacks_num,
    fcs_kv_state raw_state_raw, const int num_vacant_stacks,
    const fcs_cards_column col, sequences_analysis *const seqs,
    const int stack_idx, const int ds, const bool behavior_flag)
{
    return generic_false_seq_index_loop(stacks_num, raw_state_raw,
        num_vacant_stacks, col, seqs, stack_idx, ds, behavior_flag,
        /* Params that should be ignored in this case. */
        FALSE, fc_solve_empty_card, 0);
}

#define IS_false_seq_index_loop(col, behavior_flag, stack_idx, ds)             \
    false_seq_index_loop(LOCAL_STACKS_NUM, raw_state_raw, num_vacant_stacks,   \
        col, &seqs, stack_idx, ds, behavior_flag)

#define POPULATE_AND_CHECK_IF_FALSE_SEQ(                                       \
    col, height, stack_idx, ds, behavior_flag)                                 \
    ({                                                                         \
        populate_seq_points(col, height, &seqs);                               \
        IS_false_seq_index_loop(col, behavior_flag, stack_idx, ds);            \
    })

static inline void move_sequences_analysis_seqs_loop(
    fcs_kv_state *const ptr_to_pass_new_state SFS__PASS_MOVE_STACK(
        fcs_move_stack *const moves),
    const sequences_analysis *const seqs_ptr, int source_col_idx,
    int source_col_cards_num IND_BUF_T_PARAM(indirect_stacks_buffer))
{
#define pass_new_state (*ptr_to_pass_new_state)
    for (size_t seq_index = 0; seq_index < seqs_ptr->num_separate_false_seqs;
         seq_index++)
    {
        const_AUTO(dest_col_i, seqs_ptr->junk_move_to_stacks[seq_index]);
        my_copy_stack(dest_col_i);

        fcs_move_sequence(dest_col_i, source_col_idx,
            ((seq_index == 0) ? source_col_cards_num
                              : seqs_ptr->seq_points[seq_index - 1]) -
                seqs_ptr->seq_points[seq_index]);
    }
#undef pass_new_state
}

DECLARE_MOVE_FUNCTION(
    fc_solve_sfs_simple_simon_move_sequence_to_true_parent_with_some_cards_above)
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
     * after_junk_num_freestacks - this variable holds the number of stacks
     *      that remained unoccupied during and after the process of moving
     *      the junk sequences to different stacks.
     *
     * */

    SIMPS_define_vacant_stacks_accessors();
    CALC_POSITIONS_BY_RANK();

    STACK_SOURCE_LOOP_START(1)
    size_t num_true_seqs = 1;

    for (int h = cards_num - 2; h >= -1; h--)
    {
        const fcs_card card = fcs_col_get_card(col, h + 1);
        bool should_search = TRUE;
        bool should_increment_num_true_seqs = FALSE;
        bool should_break;
        /* Stop if we reached the bottom of the stack */
        if (!((should_break = (h == -1))))
        {
            const fcs_card h_above_card = fcs_col_get_card(col, h);
            /* If this is no longer a sequence - move to the next stack */
            if (!fcs_is_ss_false_parent(h_above_card, card))
            {
                should_break = TRUE;
            }
            else if ((should_search =
                             (!fcs_is_ss_suit_true(h_above_card, card))))
            {
                should_increment_num_true_seqs = TRUE;
            }
        }
        if (should_search)
        {
            LOOK_FOR_TRUE_PARENT_with_ds_dc__START(card)
                /* This is a suitable parent - let's check if there's a sequence
                   above it. */
                sequences_analysis seqs;

            if ((POPULATE_AND_CHECK_IF_FALSE_SEQ(
                     dest_col, dc, stack_idx, ds, FALSE) &&
                    (calc_max_simple_simon_seq_move(
                         seqs.after_junk_num_freestacks) >= num_true_seqs)))
            {
                /*
                 * We can do it - so let's move everything.
                 * Notice that we only put the child in a different stack
                 * then the parent and let it move to the parent in the
                 * next iteration of the program
                 * */

                sfs_check_state_begin();
                copy_two_stacks(stack_idx, ds);
                // Move the junk cards to their place
                move_sequences_analysis_seqs_loop(
                    &pass_new_state SFS__PASS_MOVE_STACK(moves), &seqs, ds,
                    dest_cards_num PASS_IND_BUF_T(indirect_stacks_buffer));

                /* Move the source seq on top of the dest seq */
                fcs_move_sequence(ds, stack_idx, cards_num - h - 1);

                sfs_check_state_end();
            }
            LOOK_FOR_TRUE_PARENT_with_ds_dc__END()
        }

        if (should_break)
        {
            break;
        }
        if (should_increment_num_true_seqs)
        {
            ++num_true_seqs;
        }
    }
    STACK_SOURCE_LOOP_END()
}

DECLARE_MOVE_FUNCTION(
    fc_solve_sfs_simple_simon_move_sequence_with_some_cards_above_to_true_parent)
{
    SIMPS_define_vacant_stacks_accessors();
    CALC_POSITIONS_BY_RANK();

    STACK_SOURCE_LOOP_START(1)
    for (ssize_t src_card_height = cards_num - 1; src_card_height >= 0;
         src_card_height--)
    {
        const fcs_card h_card = fcs_col_get_card(col, src_card_height);
        fcs_card card = h_card;

        size_t num_true_seqs = 1;

        for (size_t end_of_src_seq = src_card_height + 1;
             end_of_src_seq < (size_t)cards_num; ++end_of_src_seq)
        {
            const fcs_card above_card = fcs_col_get_card(col, end_of_src_seq);
            if (!fcs_is_ss_false_parent(card, above_card))
            {
                /* Split the cards above it into false sequences */

                LOOK_FOR_TRUE_PARENT_AT_TOP__START(h_card)
                /* This is a suitable parent - let's check if we
                 * have enough empty stacks to make the move feasible */
                sequences_analysis seqs;

                if ((POPULATE_AND_CHECK_IF_FALSE_SEQ(
                         col, end_of_src_seq - 1, stack_idx, ds, FALSE) &&
                        (calc_max_simple_simon_seq_move(
                             seqs.after_junk_num_freestacks) > num_true_seqs)))
                {
                    /* Let's boogie - we can move everything */
                    sfs_check_state_begin();
                    copy_two_stacks(stack_idx, ds);

                    /* Move the junk cards to their place */
                    move_sequences_analysis_seqs_loop(
                        &pass_new_state SFS__PASS_MOVE_STACK(moves), &seqs,
                        stack_idx,
                        cards_num PASS_IND_BUF_T(indirect_stacks_buffer));

                    fcs_move_sequence(
                        ds, stack_idx, end_of_src_seq - src_card_height);

                    sfs_check_state_end();
                }
                LOOK_FOR_TRUE_PARENT_AT_TOP__END()
                break;
            }
            if (!fcs_is_ss_suit_true(card, above_card))
            {
                ++num_true_seqs;
            }
            card = above_card;
        }
    }
    STACK_SOURCE_LOOP_END()
}

/*
 * start, end, src_stack.
 * */
typedef struct
{
    int cards_num;
    int src_stack;
} s_e_src_type;

static inline s_e_src_type calc_start_end_src_stack(const int seq_index,
    const sequences_analysis *const seqs, const int after_end_of_junk,
    const int cards_num, const int stack_idx, const int ds,
    const int dest_cards_num)
{
    if ((size_t)seq_index == seqs->num_separate_false_seqs)
    {
        return (const s_e_src_type){
            .cards_num = cards_num - after_end_of_junk, .src_stack = stack_idx};
    }
    else
    {
        return (const s_e_src_type){
            .cards_num = (((seq_index == 0) ? dest_cards_num
                                            : seqs->seq_points[seq_index - 1]) -
                          seqs->seq_points[seq_index]),
            .src_stack = ds};
    }
}

DECLARE_MOVE_FUNCTION(
    fc_solve_sfs_simple_simon_move_sequence_with_junk_seq_above_to_true_parent_with_some_cards_above)
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
    CALC_POSITIONS_BY_RANK();

    STACK_SOURCE_LOOP_START(1)
    size_t num_src_junk_true_seqs;

    int h = get_seq_h(col, &num_src_junk_true_seqs);
    if (!h)
    {
        continue;
    }

    fcs_card card = fcs_col_get_card(col, h);

    const int after_end_of_junk = h;
    const int end_of_junk = (--h);
    size_t num_true_seqs = 1;

    for (; h > -1; h--)
    {
        const fcs_card next_card = fcs_col_get_card(col, h);
        if (!fcs_is_ss_false_parent(next_card, card))
        {
            card = next_card;
            break;
        }
        if (!fcs_is_ss_suit_true(next_card, card))
        {
            ++num_true_seqs;
        }
        card = next_card;
    }

    /* Start at the card below the top one, so we will
     * make sure there's at least some junk above it
     * */
    LOOK_FOR_TRUE_PARENT_with_ds_dc__START(card) if (dc <= dest_cards_num - 2)
    {
        /* This is a suitable parent - let's check if there's a sequence above
         * it. */
        sequences_analysis seqs;

        populate_seq_points(dest_col, dc, &seqs);

        if (generic_false_seq_index_loop(LOCAL_STACKS_NUM, raw_state_raw,
                num_vacant_stacks, dest_col, &seqs, stack_idx, ds, FALSE, TRUE,
                fcs_col_get_card(col, after_end_of_junk),
                num_src_junk_true_seqs) &&
            (calc_max_simple_simon_seq_move(seqs.after_junk_num_freestacks) >=
                num_true_seqs))
        {
            /* We can do it - so let's move everything */
            sfs_check_state_begin();
            copy_two_stacks(stack_idx, ds);
            /* Move the junk cards to their place */

            for (size_t seq_index = 0;
                 seq_index < seqs.num_separate_false_seqs + 1; seq_index++)
            {
                const s_e_src_type s_e = calc_start_end_src_stack(seq_index,
                    &seqs, after_end_of_junk, cards_num, stack_idx, ds,
                    dest_cards_num);
                const_AUTO(dest_col_i, seqs.junk_move_to_stacks[seq_index]);
                copy_two_stacks(s_e.src_stack, dest_col_i);
                fcs_move_sequence(dest_col_i, s_e.src_stack, s_e.cards_num);
            }

            /* Move the source seq on top of the dest seq */
            fcs_move_sequence(ds, stack_idx, end_of_junk - h + 1);

            sfs_check_state_end();
        }
    }
    LOOK_FOR_TRUE_PARENT_with_ds_dc__END() STACK_SOURCE_LOOP_END()

        return;
}

typedef fcs_pos_by_rank ds_dc_type;

static inline void sort_ds_dcs(ds_dc_type *const ds_dcs, const int len)
{
#define start ds_dcs
    ds_dc_type *const end = start + len;

    for (ds_dc_type *b = start + 1; b < end; b++)
    {
        for (ds_dc_type *c = b;
             (c > start) &&
             (c[0].col < c[-1].col ||
                 (c[0].col == c[-1].col && c[0].height > c[-1].height));
             c--)
        {
            const_AUTO(swap_temp, c[-1]);
            c[-1] = c[0];
            c[0] = swap_temp;
        }
    }
#undef start
}

DECLARE_MOVE_FUNCTION(
    fc_solve_sfs_simple_simon_move_whole_stack_sequence_to_false_parent_with_some_cards_above)
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
     * after_junk_num_freestacks - a variable that holds the number of stacks
     *      that are left unoccupied as part of the junk disposal process.
     *
     * */
    SIMPS_define_vacant_stacks_accessors();
    CALC_POSITIONS_BY_RANK();

    /*
     * We never fill empty stacks with junk cards in this move function,
     * so as a result, after_junk_num_freestacks == num_vacant_stacks and
     * is constant here.
     * */
    const size_t max_seq_move =
        calc_max_simple_simon_seq_move(num_vacant_stacks);

    STACK_SOURCE_LOOP_START(1)
    {
        size_t num_true_seqs;
        if (get_seq_h(col, &num_true_seqs) || (max_seq_move < num_true_seqs))
        {
            continue;
        }
    }
#define h 0
    const fcs_card card = fcs_col_get_card(col, h);

    if (fcs_card_is_king(card))
    {
        continue;
    }
    ds_dc_type ds_dcs[4];
    size_t len = 0;
    for (size_t parent_suit = 0; parent_suit < 4; parent_suit++)
    {
        const fcs_pos_by_rank pos = positions_by_rank[FCS_POS_IDX(
            fcs_card_rank(card) + 1, parent_suit)];

        if ((pos.col < 0) || (pos.col == stack_idx))
        {
            continue;
        }

        ds_dcs[len++] = pos;
    }

    /* This is done to preserve the original order in the solutions. */
    sort_ds_dcs(ds_dcs, len);

    for (size_t i = 0; i < len; i++)
    {
        const size_t ds = ds_dcs[i].col;
        const size_t dc = ds_dcs[i].height;
        const_AUTO(dest_col, fcs_state_get_col(state, ds));
        const int dest_cards_num = fcs_col_len(dest_col);

        /* This is a suitable parent - let's check if there's a sequence above
         * it. */
        sequences_analysis seqs;

        if (POPULATE_AND_CHECK_IF_FALSE_SEQ(dest_col, dc, stack_idx, ds, TRUE))
        {
            /* We can do it - so let's move */
            sfs_check_state_begin();
            copy_two_stacks(stack_idx, ds);
            /* Move the junk cards to their place */
            move_sequences_analysis_seqs_loop(
                &pass_new_state SFS__PASS_MOVE_STACK(moves), &seqs, ds,
                dest_cards_num PASS_IND_BUF_T(indirect_stacks_buffer));

            fcs_move_sequence(ds, stack_idx, cards_num - h);

            sfs_check_state_end();
#undef h
        }
    }
    STACK_SOURCE_LOOP_END()
}

DECLARE_MOVE_FUNCTION(
    fc_solve_sfs_simple_simon_move_sequence_to_parent_on_the_same_stack)
{
    SIMPS_define_vacant_stacks_accessors();

    STACK_SOURCE_LOOP_START(3)
    /* Search for a parent card */
    for (int parent_card_height = 0; parent_card_height < cards_num - 2;
         parent_card_height++)
    {
        const fcs_card parent_card = fcs_col_get_card(col, parent_card_height);
        if (fcs_is_ss_true_parent(
                parent_card, fcs_col_get_card(col, parent_card_height + 1)))
        {
            continue;
        }

        for (int child_card_height = parent_card_height + 2;
             child_card_height < cards_num; child_card_height++)
        {
            if (!fcs_is_ss_true_parent(
                    parent_card, fcs_col_get_card(col, child_card_height)))
            {
                continue;
            }
            // We have a matching parent and child cards
            // Now let's try to find stacks to place the cards above the child
            // card.

            int end_of_child_seq = child_card_height;
            size_t child_num_true_seqs = 1;
            while (
                (end_of_child_seq + 1 < cards_num) &&
                fcs_is_ss_false_parent(fcs_col_get_card(col, end_of_child_seq),
                    fcs_col_get_card(col, end_of_child_seq + 1)))
            {
                child_num_true_seqs += (!fcs_is_ss_true_parent(
                    fcs_col_get_card(col, end_of_child_seq),
                    fcs_col_get_card(col, end_of_child_seq + 1)));
                ++end_of_child_seq;
            }

            sequences_analysis seqs;
            populate_seq_points(col, end_of_child_seq, &seqs);

            /* Add the child to the seq_points */
            const_AUTO(child_seq_index, seqs.num_separate_false_seqs);
            seqs.above_num_true_seqs[seqs.num_separate_false_seqs] =
                child_num_true_seqs;
            seqs.seq_points[seqs.num_separate_false_seqs++] = child_card_height;

            /* Add the cards between the parent and the child to the seq_points
             */
            generic_populate_seq_points(
                col, parent_card_height, &seqs, child_card_height);

            /* Let's check if we can move the child after we are done
             * moving all the junk cards */
            if (!(IS_false_seq_index_loop(col, FALSE, stack_idx, stack_idx) &&
                    (calc_max_simple_simon_seq_move(
                         seqs.after_junk_num_freestacks) >=
                        child_num_true_seqs)))
            {
                continue;
            }
            /* We can do it - so let's move everything */
            sfs_check_state_begin();

            /* Move the junk cards to their place */

            my_copy_stack(stack_idx);
            move_sequences_analysis_seqs_loop(
                &pass_new_state SFS__PASS_MOVE_STACK(moves), &seqs, stack_idx,
                cards_num PASS_IND_BUF_T(indirect_stacks_buffer));
            const_AUTO(src_col_i, seqs.junk_move_to_stacks[child_seq_index]);
            my_copy_stack(src_col_i);
            fcs_move_sequence(
                stack_idx, src_col_i, end_of_child_seq - child_card_height + 1);

            sfs_check_state_end();
        }
    }
    STACK_SOURCE_LOOP_END()
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

    const size_t max_seq_move =
        calc_max_simple_simon_seq_move(num_vacant_stacks);

    STACK_SOURCE_LOOP_START(1)
    size_t num_true_seqs;
    const int h = get_seq_h(col, &num_true_seqs);
    /* Let's check if we have enough empty stacks to make the move
     * feasible.
     * */
    if (max_seq_move < num_true_seqs)
    {
        continue;
    }
    const fcs_card card = fcs_col_get_card(col, h);

    /* take the sequence and try and put it on another stack */
    STACK_DEST_LOOP_START(1)
    if (!fcs_is_ss_false_parent(
            fcs_col_get_card(dest_col, dest_cards_num - 1), card))
    {
        continue;
    }

    /* We can do it - so let's perform the move */
    sfs_check_state_begin();
    copy_two_stacks(stack_idx, ds);
    fcs_move_sequence(ds, stack_idx, cards_num - h);
    sfs_check_state_end();
    STACK_DEST_LOOP_END()
    STACK_SOURCE_LOOP_END()
}

#undef state

#endif /* #ifdef FCS_DISABLE_SIMPLE_SIMON */
