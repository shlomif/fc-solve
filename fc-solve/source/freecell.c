/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// freecell.c - a module that contains moves of the Freecell-games family.

#include "dll_thunk.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include "config.h"
#include "scans.h"
#include "meta_move_funcs_helpers.h"
#include "freecell.h"

#ifdef FCS_FREECELL_ONLY

#define MOVE_FUNCS__define_seqs_built_by()
#define MOVE_FUNCS__define_empty_stacks_fill()
#define PASS_sequences_are_built_by(param)

#else

#define MOVE_FUNCS__define_seqs_built_by()                                     \
    const int sequences_are_built_by =                                         \
        GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance);
#define MOVE_FUNCS__define_empty_stacks_fill()                                 \
    const int empty_stacks_fill = INSTANCE_EMPTY_STACKS_FILL;
#define PASS_sequences_are_built_by(param) , param

#endif

#ifndef FCS_WITH_MOVES
#define moves NULL
#define moves_ptr NULL
#endif

static inline int find_empty_stack(fcs_kv_state_t *const raw_ptr_state_raw,
    const int start_from, const int local_stacks_num)
{
    for (int ret = start_from; ret < local_stacks_num; ret++)
    {
        if (fcs_col_len(fcs_state_get_col(state, ret)) == 0)
        {
            return ret;
        }
    }
    return -1;
}

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
    tests_define_accessors();

#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        const size_t cards_num = fcs_col_len(col);
        if (!cards_num)
        {
            continue;
        }
        /* Get the top card in the stack */
        const fcs_card_t card = fcs_col_get_card(col, cards_num - 1);
        for (int deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
        {
            if (fcs_foundation_value(state, deck * 4 + fcs_card_suit(card)) !=
                fcs_card_rank(card) - 1)
            {
                continue;
            }
            /* We can put it there */

            sfs_check_state_begin();

            my_copy_stack(stack_idx);
            fcs_state_pop_col_top(&new_state_key, stack_idx);
            fcs_increment_foundation(
                new_state_key, deck * 4 + fcs_card_suit(card));

            fcs_move_stack_non_seq_push(moves,
                FCS_MOVE_TYPE_STACK_TO_FOUNDATION, stack_idx,
                deck * 4 + fcs_card_suit(card));

            sfs_check_state_end();
            break;
        }
    }

    return;
}

static inline void sort_derived_states(
    fcs_derived_states_list_t *const d, const size_t derived_start_idx)
{
    var_AUTO(start, d->states + derived_start_idx);
    const_AUTO(limit, d->states + d->num_states);

    for (var_AUTO(b, start + 1); b < limit; b++)
    {
        for (var_AUTO(c, b); (c > start) && (c[0].context.i < c[-1].context.i);
             c--)
        {
            const_AUTO(swap_temp, c[-1]);
            c[-1] = c[0];
            c[0] = swap_temp;
        }
    }
}

/*
 * This test moves single cards that are present in the freecells to
 * the foundations.
 * */
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_to_founds)
{
    tests_define_accessors_no_stacks();

#ifndef HARD_CODED_NUM_FREECELLS
    SET_GAME_PARAMS();
#endif

    /* Now check the same for the free cells */
    for (int fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
    {
        const fcs_card_t card = fcs_freecell_card(state, fc);

        if (fcs_card_is_empty(card))
        {
            continue;
        }
        for (int deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
        {
            if (fcs_foundation_value(state, deck * 4 + fcs_card_suit(card)) !=
                fcs_card_rank(card) - 1)
            {
                continue;
            }
            /* We can put it there */
            sfs_check_state_begin();

            fcs_empty_freecell(new_state_key, fc);

            fcs_increment_foundation(
                new_state_key, deck * 4 + fcs_card_suit(card));

            fcs_move_stack_non_seq_push(moves,
                FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION, fc,
                deck * 4 + fcs_card_suit(card));

            sfs_check_state_end();

            break;
        }
    }

    return;
}

typedef struct
{
    int src_idx;
    fcs_bool_t is_col;
} empty_two_cols_ret_t;
/*
 * This function empties two stacks from the new state
 * into freeeclls and empty columns
 */
static inline empty_two_cols_ret_t empty_two_cols_from_new_state(
    const fc_solve_soft_thread_t *const soft_thread,
    fcs_kv_state_t *const kv_ptr_new_state SFS__PASS_MOVE_STACK(
        fcs_move_stack_t *const moves_ptr),
    const int cols_indexes[3], const int num_cards_1, const int num_cards_2)
{
    empty_two_cols_ret_t ret = {.src_idx = -1, .is_col = FALSE};

    int num_cards_to_move_from_columns[3] = {num_cards_1, num_cards_2, -1};

    const int *col_idx = cols_indexes;
    int *col_num_cards = num_cards_to_move_from_columns;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_INSTANCE_GAME_PARAMS(fcs_st_instance(soft_thread));
#endif

#ifdef INDIRECT_STACK_STATES
    char *const indirect_stacks_buffer =
        HT_FIELD(soft_thread->hard_thread, indirect_stacks_buffer);
#endif

    const_AUTO(new_key, kv_ptr_new_state->key);
    {
        int dest_fc_idx = 0;

        while (1)
        {
            while ((*col_num_cards) == 0)
            {
                ++col_num_cards;
                if (*(++col_idx) == -1)
                {
                    return ret;
                }
            }

            /* Find a vacant freecell */
            for (; dest_fc_idx < LOCAL_FREECELLS_NUM; dest_fc_idx++)
            {
                if (fcs_freecell_is_empty(*new_key, dest_fc_idx))
                {
                    break;
                }
            }
            if (dest_fc_idx == LOCAL_FREECELLS_NUM)
            {
                /*  Move on to the stacks. */
                break;
            }

            const fcs_card_t top_card =
                fcs_state_pop_col_card(new_key, *col_idx);

            fcs_put_card_in_freecell(*new_key, dest_fc_idx, top_card);

            fcs_move_stack_non_seq_push(moves_ptr,
                FCS_MOVE_TYPE_STACK_TO_FREECELL, *col_idx, dest_fc_idx);

            ret =
                (empty_two_cols_ret_t){.src_idx = dest_fc_idx, .is_col = FALSE};
            --(*col_num_cards);
            ++dest_fc_idx;
        }
    }

    while ((*col_num_cards) == 0)
    {
        ++col_num_cards;
        if (*(++col_idx) == -1)
        {
            return ret;
        }
    }

    int put_cards_in_col_idx = 0;
    /* Fill the free stacks with the cards below them */
    while (1)
    {
        while ((*col_num_cards) == 0)
        {
            ++col_num_cards;
            if (*(++col_idx) == -1)
            {
                return ret;
            }
        }

        /*  Find a vacant stack */
        put_cards_in_col_idx = find_empty_stack(
            kv_ptr_new_state, put_cards_in_col_idx, LOCAL_STACKS_NUM);
        assert(put_cards_in_col_idx < LOCAL_STACKS_NUM);

        fcs_copy_stack(*new_key, *(kv_ptr_new_state->val), put_cards_in_col_idx,
            indirect_stacks_buffer);

        const int col_idx_val = *col_idx;
        const fcs_card_t top_card =
            fcs_state_pop_col_card(new_key, col_idx_val);
        const fcs_cards_column_t new_b_col =
            fcs_state_get_col(*new_key, put_cards_in_col_idx);
        fcs_col_push_card(new_b_col, top_card);

        fcs_push_1card_seq(moves_ptr, col_idx_val, put_cards_in_col_idx);

        ret = (empty_two_cols_ret_t){
            .src_idx = put_cards_in_col_idx, .is_col = TRUE};
        --(*col_num_cards);
        ++put_cards_in_col_idx;
    }
}

#define CALC_POSITIONS_BY_RANK()                                               \
    const int8_t *const positions_by_rank =                                    \
        fc_solve_calc_positions_by_rank_location(soft_thread)

#define FCS_POS_BY_RANK_MAP(x) ((x) << 1)

#ifdef FCS_FREECELL_ONLY

#define FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(card)             \
    ((fcs_card_suit(card) ^ 0x1) & (0x2 - 1))

#define FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_STEP() (2)

#else

#define FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(card)             \
    ((sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK)                         \
            ? 0                                                                \
            : (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT)                \
                  ? fcs_card_suit(card)                                        \
                  : ((fcs_card_suit(card) ^ 0x1) & (0x2 - 1)))

#define FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_STEP()                           \
    ((sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK)                         \
            ? 1                                                                \
            : (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT) ? 4 : 2)

#endif

#define FCS_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(card)                   \
    FCS_POS_BY_RANK_MAP(                                                       \
        FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(card))
#define FCS_CARD_SUIT_POSITIONS_BY_RANK_STEP()                                 \
    FCS_POS_BY_RANK_MAP(FCS_PROTO_CARD_SUIT_POSITIONS_BY_RANK_STEP())

#define FCS_POS_IDX_TO_CHECK__INIT_CONSTANTS()                                 \
    const int suit_positions_by_rank_step =                                    \
        (FCS_CARD_SUIT_POSITIONS_BY_RANK_STEP())

#define FCS_POS_IDX_TO_CHECK_START_LOOP(src_card)                              \
    const int8_t *pos_idx_to_check = &positions_by_rank[(                      \
        FCS_POS_BY_RANK_WIDTH * (fcs_card_rank(src_card)))];                   \
    const int8_t *const last_pos_idx =                                         \
        pos_idx_to_check + FCS_POS_BY_RANK_WIDTH;                              \
                                                                               \
    for (pos_idx_to_check +=                                                   \
         FCS_CARD_SUIT_POSITIONS_BY_RANK_INITIAL_OFFSET(src_card);             \
         pos_idx_to_check < last_pos_idx;                                      \
         pos_idx_to_check += suit_positions_by_rank_step)

static inline fcs_game_limit_t calc_num_vacant_slots(
    const fc_solve_soft_thread_t *const soft_thread,
    const fcs_bool_t is_filled_by_any_card)
{
    return (soft_thread->num_vacant_freecells +
            (is_filled_by_any_card ? soft_thread->num_vacant_stacks : 0));
}

#define MOVE_FUNCS__define_common()                                            \
    tests_define_accessors();                                                  \
    MOVE_FUNCS__define_seqs_built_by();                                        \
    MOVE_FUNCS__define_empty_stacks_fill()

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_on_top_of_stacks)
{
    MOVE_FUNCS__define_common();
#if ((!defined(HARD_CODED_NUM_FREECELLS)) ||                                   \
     (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif

    const fcs_game_limit_t num_vacant_slots =
        calc_num_vacant_slots(soft_thread, tests__is_filled_by_any_card());

    const size_t derived_start_idx = derived_states_list->num_states;

    CALC_POSITIONS_BY_RANK();
    FCS_POS_IDX_TO_CHECK__INIT_CONSTANTS();

    /* Let's try to put cards in the freecells on top of stacks */

    /* Scan the freecells */
    for (int fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
    {
        const fcs_card_t src_card = fcs_freecell_card(state, fc);

        /* If the freecell is not empty and dest_card is its parent
         * */
        if (!(
                /* The Cell should not be empty. */
                fcs_card_is_valid(src_card) &&
                /* We cannot put a king anywhere. */
                (fcs_card_rank(src_card) != 13)))
        {
            continue;
        }
        FCS_POS_IDX_TO_CHECK_START_LOOP(src_card)
        {
            const int_fast32_t ds = pos_idx_to_check[0];
            if (ds == -1)
            {
                continue;
            }
            const int_fast32_t dc = pos_idx_to_check[1];

            const fcs_const_cards_column_t dest_col =
                fcs_state_get_col(state, ds);

            const int dest_cards_num = fcs_col_len(dest_col);
            /* Let's check if we can put it there */

            /* Check if the destination card is already below a
             * suitable card */
            const int next_dc = dc + 1;
/* If dest_cards_num == next_dc then
 * dest_cards_num - next_dc == 0 <= 0 so the other check
 * cab be skipped.
 * */

/*
 * We don't need this check because the positions_by_rank
 * already filters the fcs_is_parent_card check for us.
 * */
#ifdef FCS_POS_BY_RANK__ENABLE_PARENT_CHECK
            const fcs_card_t dest_card = fcs_col_get_card(dest_col, dc);
            if (!((dest_cards_num == next_dc) ||
                    ((!fcs_is_parent_card(
                         fcs_col_get_card(dest_col, next_dc), dest_card)) &&
                        (dest_cards_num <= next_dc + num_vacant_slots)

                            )))
#else
            if (!(dest_cards_num <= next_dc + num_vacant_slots))
#endif
            {
                continue;
            }
            /* We can move it */
            sfs_check_state_begin();

            /* Fill the freecells with the top cards */
            my_copy_stack(ds);
            const int cols_indexes[3] = {ds, -1, -1};
            empty_two_cols_from_new_state(soft_thread,
                ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                dest_cards_num - dc - 1, 0);

            fcs_cards_column_t new_dest_col =
                fcs_state_get_col(new_state_key, ds);

            /* Now put the freecell card on top of the stack */
            fcs_col_push_card(new_dest_col, src_card);
            fcs_empty_freecell(new_state_key, fc);

            fcs_move_stack_non_seq_push(
                moves, FCS_MOVE_TYPE_FREECELL_TO_STACK, fc, ds);

            /*
             * This is to preserve the order that the
             * initial (non-optimized) version of the
             * function used - for backwards-compatibility
             * and consistency.
             * */
            state_context_value = ((ds << 16) | ((255 - dc) << 8) | fc);

            sfs_check_state_end();
        }
    }

    sort_derived_states(derived_states_list, derived_start_idx);

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_non_top_stack_cards_to_founds)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif
    const fcs_game_limit_t num_vacant_slots =
        calc_num_vacant_slots(soft_thread, tests__is_filled_by_any_card());

    /* Now let's check if a card that is under some other cards can be placed
     * in the foundations. */

    const_AUTO(num_vacant_slots_plus_1, num_vacant_slots + 1);

    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        const int cards_num = fcs_col_len(col);
        /*
         * We starts from cards_num-2 because the top card is already covered
         * by move_top_stack_cards_to_founds.
         *
         * Some math:
         * num_vacant_slots >= cards_num - (c + 1)
         * c >= cards_num - num_vacant_slots - 1
         * */
        const int c_bottom = max0(cards_num - num_vacant_slots_plus_1);
        for (int c = cards_num - 2; c >= c_bottom; c--)
        {
            const fcs_card_t card = fcs_col_get_card(col, c);

            for (int deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
            {
                const int dest_found = deck * 4 + fcs_card_suit(card);
                if (fcs_foundation_value(state, dest_found) !=
                    fcs_card_rank(card) - 1)
                {
                    continue;
                }
                /* We can move it */
                sfs_check_state_begin();
                my_copy_stack(stack_idx);
                const int cols_indexes[3] = {stack_idx, -1, -1};

                empty_two_cols_from_new_state(soft_thread,
                    ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                    cards_num - (c + 1), 0);
                fcs_state_pop_col_top(&new_state_key, stack_idx);
                fcs_increment_foundation(new_state_key, dest_found);
                fcs_move_stack_non_seq_push(moves,
                    FCS_MOVE_TYPE_STACK_TO_FOUNDATION, stack_idx, dest_found);
                sfs_check_state_end();
                break;
            }
        }
    }

    return;
}

DECLARE_MOVE_FUNCTION(
    fc_solve_sfs_move_stack_cards_to_a_parent_on_the_same_stack)
{
    MOVE_FUNCS__define_common();
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    const fcs_game_limit_t num_vacant_slots_plus_1 =
        calc_num_vacant_slots(soft_thread, tests__is_filled_by_any_card()) + 1;

    /*
     * Now let's try to move a stack card to a parent card which is found
     * on the same stack.
     * */
    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        const int cards_num = fcs_col_len(col);
#define dest_cards_num cards_num
        const int start_dc = max0(dest_cards_num - num_vacant_slots_plus_1);

        for (int c = start_dc + 2; c < cards_num; c++)
        {
            /* Find a card which this card can be put on; */

            const fcs_card_t card = fcs_col_get_card(col, c);

            /* Do not move cards that are already found above a suitable
             * parent */
            const int below_c = c - 1;
            if (fcs_is_parent_card(card, fcs_col_get_card(col, below_c)))
            {
                continue;
            }
/*
 * Some math:
 * (dest_cards_num - next_dc <= num_vacant_slots)
 * dest_cards_num - dc - 1 <= num_vacant_slots
 * dc >= dest_cards_num - num_vacant_slots - 1
 * */
#define ds stack_idx
#define dest_col col
            /* Check if it can be moved to something on the same stack */
            for (int dc = start_dc; dc < below_c; dc++)
            {
                const fcs_card_t dest_card = fcs_col_get_card(dest_col, dc);
                const int next_dc = dc + 1;
                if ((!fcs_is_parent_card(card, dest_card)) ||
                    /* Corresponding cards - see if it is feasible to move
                       the source to the destination. */
                    fcs_is_parent_card(
                        fcs_col_get_card(dest_col, next_dc), dest_card))
                {
                    continue;
                }
                /* We can move it */
                sfs_check_state_begin();

                my_copy_stack(ds);

                fcs_cards_column_t new_dest_col =
                    fcs_state_get_col(new_state_key, ds);

                const int cols_indexes[3] = {ds, -1, -1};

                const empty_two_cols_ret_t last_dest =
                    empty_two_cols_from_new_state(soft_thread,
                        ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                        /* We're moving one extra card */
                        cards_num - c, 0);

                empty_two_cols_from_new_state(soft_thread,
                    ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                    below_c - dc, 0);

                fcs_card_t moved_card;
#define src_idx last_dest.src_idx
                if (last_dest.is_col)
                {
                    fcs_cards_column_t new_source_col =
                        fcs_state_get_col(new_state_key, src_idx);

                    fcs_col_pop_card(new_source_col, moved_card);

                    fcs_push_1card_seq(moves, src_idx, ds);
                }
                else
                {
                    moved_card = fcs_freecell_card(new_state_key, src_idx);
                    fcs_empty_freecell(new_state_key, src_idx);

                    fcs_move_stack_non_seq_push(
                        moves, FCS_MOVE_TYPE_FREECELL_TO_STACK, src_idx, ds);
                }
#undef src_idx

                fcs_col_push_card(new_dest_col, moved_card);

                sfs_check_state_end();
            }
        }
    }

    return;
}
#undef ds
#undef dest_col
#undef dest_cards_num

#define CALC_num_virtual_vacant_stacks()                                       \
    (tests__is_filled_by_any_card() ? num_vacant_stacks : 0)

typedef struct
{
    fcs_cards_column_t col;
    int_fast16_t col_len, col_len_minus_1, c, seq_end;
#ifndef FCS_FREECELL_ONLY
    int sequences_are_built_by;
#endif
} col_seqs_iter_t;

static inline void col_seqs_iter__calc_end(col_seqs_iter_t *const iter)
{
#ifndef FCS_FREECELL_ONLY
    const_SLOT(sequences_are_built_by, iter);
#endif
    for ((*iter).seq_end = (*iter).c; (*iter).seq_end < (*iter).col_len_minus_1;
         (*iter).seq_end++)
    {
        if (!fcs_is_parent_card(
                fcs_col_get_card((*iter).col, (*iter).seq_end + 1),
                fcs_col_get_card((*iter).col, (*iter).seq_end)))
        {
            break;
        }
    }
}

static inline col_seqs_iter_t col_seqs_iter__create(
    fcs_state_t *const s, const int stack_idx PASS_sequences_are_built_by(
                              const int sequences_are_built_by))
{
    col_seqs_iter_t ret;
#ifndef FCS_FREECELL_ONLY
    ret.sequences_are_built_by = sequences_are_built_by;
#endif
    ret.col = fcs_state_get_col(*s, stack_idx);
    ret.col_len_minus_1 = (ret.col_len = fcs_col_len(ret.col)) - 1;
    ret.c = 0;
    col_seqs_iter__calc_end(&ret);
    return ret;
}

static inline void col_seqs_iter__advance(col_seqs_iter_t *const iter)
{
    iter->c = iter->seq_end + 1;
    col_seqs_iter__calc_end(iter);
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_stack_cards_to_different_stacks)
{
    MOVE_FUNCS__define_common();
#if ((!defined(HARD_CODED_NUM_FREECELLS)) ||                                   \
     (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif
    const_SLOT(num_vacant_freecells, soft_thread);
    const_SLOT(num_vacant_stacks, soft_thread);
    const fcs_game_limit_t num_virtual_vacant_stacks =
        CALC_num_virtual_vacant_stacks();
    const size_t derived_start_idx = derived_states_list->num_states;

    CALC_POSITIONS_BY_RANK();

    FCS_POS_IDX_TO_CHECK__INIT_CONSTANTS();

    /* Now let's try to move a card from one stack to the other     *
     * Note that it does not involve moving cards lower than king   *
     * to empty stacks                                              */

    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        col_seqs_iter_t iter = col_seqs_iter__create(&state,
            stack_idx PASS_sequences_are_built_by(sequences_are_built_by));
        for (; iter.c < iter.col_len; col_seqs_iter__advance(&iter))
        {
            if (MOVE_FUNCS__should_not_empty_columns() && (iter.c == 0))
            {
                continue;
            }
            const int col_num_cards = iter.col_len_minus_1 - iter.seq_end;
            /* Find a card which this card can be put on; */

            const fcs_card_t card = fcs_col_get_card(iter.col, iter.c);
            /* Skip if it's a King - nothing to put it on. */
            if (unlikely(fcs_card_rank(card) == 13))
            {
                continue;
            }

            FCS_POS_IDX_TO_CHECK_START_LOOP(card)
            {
                const int_fast32_t ds = pos_idx_to_check[0];

                if ((ds < 0) || (ds == stack_idx))
                {
                    continue;
                }
                const int_fast32_t dc = pos_idx_to_check[1];
                const fcs_const_cards_column_t dest_col =
                    fcs_state_get_col(state, ds);
                const int dest_num_cards = fcs_col_len(dest_col) - dc - 1;

                int num_cards_to_relocate = dest_num_cards + col_num_cards;

                const int freecells_to_fill =
                    min(num_cards_to_relocate, num_vacant_freecells);

                num_cards_to_relocate -= freecells_to_fill;

                const int freestacks_to_fill =
                    min(num_cards_to_relocate, num_virtual_vacant_stacks);
                num_cards_to_relocate -= freestacks_to_fill;

                if (!unlikely(
                        (num_cards_to_relocate == 0) &&
                        (calc_max_sequence_move(
                             num_vacant_freecells - freecells_to_fill,
                             num_virtual_vacant_stacks - freestacks_to_fill) >=
                            iter.seq_end - iter.c + 1)))
                {
                    continue;
                }
                sfs_check_state_begin();
                my_copy_stack(stack_idx);
                my_copy_stack(ds);
                const int cols_indexes[3] = {ds, stack_idx, -1};
                empty_two_cols_from_new_state(soft_thread,
                    ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                    dest_num_cards, col_num_cards);
                fcs_move_sequence(ds, stack_idx, iter.seq_end - iter.c + 1);
                /*
                 * This is to preserve the order that the
                 * initial (non-optimized) version of the
                 * function used - for backwards-compatibility
                 * and consistency.
                 * */
                state_context_value =
                    ((((((stack_idx << 8) | iter.c) << 8) | ds) << 8) | dc);

                sfs_check_state_end();
            }
        }
    }
    sort_derived_states(derived_states_list, derived_start_idx);
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_sequences_to_free_stacks)
{
    MOVE_FUNCS__define_common();
    if (IS_FILLED_BY_NONE())
    {
        return;
    }
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif
    const fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks;
    if (num_vacant_stacks == 0)
    {
        return;
    }
    const fcs_game_limit_t num_vacant_freecells =
        soft_thread->num_vacant_freecells;
    const fcs_game_limit_t num_virtual_vacant_stacks =
        CALC_num_virtual_vacant_stacks();
    const int max_sequence_len =
        calc_max_sequence_move(num_vacant_freecells, num_vacant_stacks - 1);

    /* Now try to move sequences to empty stacks */
    const int ds = find_empty_stack(raw_ptr_state_raw, 0, LOCAL_STACKS_NUM);
    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        col_seqs_iter_t iter = col_seqs_iter__create(&state,
            stack_idx PASS_sequences_are_built_by(sequences_are_built_by));
        for (; iter.c < iter.col_len; col_seqs_iter__advance(&iter))
        {
            if (IS_FILLED_BY_KINGS_ONLY() &&
                (!fcs_col_is_king(iter.col, iter.c)))
            {
                continue;
            }

            if (iter.seq_end == iter.col_len_minus_1)
            {
                int c = iter.c;
                /* One stack is the destination stack, so we have one     *
                 * less stack in that case                                */
                while ((max_sequence_len < iter.col_len - c) && (c > 0))
                {
                    c--;
                }

                if (!((c > 0) && ((IS_FILLED_BY_KINGS_ONLY())
                                         ? fcs_col_is_king(iter.col, c)
                                         : TRUE)))
                {
                    continue;
                }
                sfs_check_state_begin();

                my_copy_stack(ds);
                my_copy_stack(stack_idx);

                fcs_move_sequence(ds, stack_idx, iter.col_len - c);

                sfs_check_state_end();
            }
            else
            {
                int num_cards_to_relocate = iter.col_len_minus_1 - iter.seq_end;

                const int freecells_to_fill =
                    min(num_cards_to_relocate, num_vacant_freecells);

                num_cards_to_relocate -= freecells_to_fill;

                const int freestacks_to_fill =
                    min(num_cards_to_relocate, num_virtual_vacant_stacks);
                num_cards_to_relocate -= freestacks_to_fill;

                if (!((num_cards_to_relocate == 0) &&
                        (num_vacant_stacks - freestacks_to_fill > 0)))
                {
                    continue;
                }
                /* We can move it */
                const int seq_start = ({
                    const int max_seq_move = calc_max_sequence_move(
                        num_vacant_freecells - freecells_to_fill,
                        num_vacant_stacks - freestacks_to_fill - 1);
                    const int m = iter.seq_end + 1 - max_seq_move;
                    max(m, iter.c);
                });
                if (!((seq_start <= iter.seq_end) &&
                        ((IS_FILLED_BY_KINGS_ONLY())
                                ? fcs_col_is_king(iter.col, seq_start)
                                : TRUE)))
                {
                    continue;
                }
                sfs_check_state_begin();

                /* Fill the freecells with the top cards */

                my_copy_stack(stack_idx);

                const int cols_indexes[3] = {stack_idx, -1, -1};
                const empty_two_cols_ret_t empty_ret =
                    empty_two_cols_from_new_state(soft_thread,
                        ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                        freecells_to_fill + freestacks_to_fill, 0);

                const int b = find_empty_stack(raw_ptr_state_raw,
                    (empty_ret.is_col ? empty_ret.src_idx + 1 : 0),
                    LOCAL_STACKS_NUM);
                my_copy_stack(b);

                fcs_move_sequence(b, stack_idx, iter.seq_end - seq_start + 1);

                sfs_check_state_end();
            }
        }
    }

    return;
}

/* Let's try to put cards that occupy freecells on an empty stack */
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_to_empty_stack)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
    if (IS_FILLED_BY_NONE())
    {
        return;
    }
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    if (soft_thread->num_vacant_stacks)
    {
        int stack_idx;
        for (stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
        {
            if (fcs_col_len(fcs_state_get_col(state, stack_idx)) == 0)
            {
                break;
            }
        }

        for (int fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
        {
            const fcs_card_t card = fcs_freecell_card(state, fc);

            if ((IS_FILLED_BY_KINGS_ONLY()) ? (fcs_card_rank(card) != 13)
                                            : fcs_card_is_empty(card))
            {
                continue;
            }
            /* We can move it */

            sfs_check_state_begin();

            my_copy_stack(stack_idx);

            fcs_cards_column_t new_src_col =
                fcs_state_get_col(new_state_key, stack_idx);
            fcs_col_push_card(new_src_col, card);
            fcs_empty_freecell(new_state_key, fc);
            fcs_move_stack_non_seq_push(
                moves, FCS_MOVE_TYPE_FREECELL_TO_STACK, fc, stack_idx);
            sfs_check_state_end();
        }
    }

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_cards_to_a_different_parent)
{
    MOVE_FUNCS__define_common();
#if ((!defined(HARD_CODED_NUM_FREECELLS)) ||                                   \
     (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif
    const fcs_game_limit_t num_vacant_freecells =
        soft_thread->num_vacant_freecells;
    const fcs_game_limit_t num_vacant_stacks = soft_thread->num_vacant_stacks;
    const fcs_game_limit_t num_virtual_vacant_stacks =
        CALC_num_virtual_vacant_stacks();
    const int derived_start_idx = derived_states_list->num_states;

    CALC_POSITIONS_BY_RANK();

    FCS_POS_IDX_TO_CHECK__INIT_CONSTANTS();

    /* This time try to move cards that are already on top of a parent to a
     * different parent */

    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        const int cards_num = fcs_col_len(col);

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

        fcs_card_t upper_card = fcs_col_get_card(col, cards_num - 1);

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
        for (min_card_height = cards_num - 2; min_card_height >= 0;
             upper_card = lower_card, min_card_height--)
        {
            lower_card = fcs_col_get_card(col, min_card_height);
            if (!fcs_is_parent_card(upper_card, lower_card))
            {
                break;
            }
        }
        min_card_height += 2;

        for (int c = min_card_height; c < cards_num; c++)
        {
            /* Find a card which this card can be put on; */

            const fcs_card_t card = fcs_col_get_card(col, c);
            FCS_POS_IDX_TO_CHECK_START_LOOP(card)
            {
                const int_fast32_t ds = pos_idx_to_check[0];
                if ((ds == -1) || (ds == stack_idx))
                {
                    continue;
                }
                const int_fast32_t dc = pos_idx_to_check[1];

                fcs_cards_column_t dest_col = fcs_state_get_col(state, ds);
                int dest_cards_num = fcs_col_len(dest_col);

/* Corresponding cards - see if it is feasible to move
   the source to the destination. */

/*
 * We don't need this check because the positions_by_rank
 * already filters the fcs_is_parent_card check for us.
 * */
#ifdef FCS_POS_BY_RANK__ENABLE_PARENT_CHECK
                fcs_card_t dest_card = fcs_col_get_card(dest_col, dc);
                /* Don't move if there's a sequence of cards in the
                 * destination.
                 * */
                if ((dc + 1 < dest_cards_num) &&
                    fcs_is_parent_card(
                        fcs_col_get_card(dest_col, dc + 1), dest_card))
                {
                    continue;
                }
#endif

                int num_cards_to_relocate = dest_cards_num - dc - 1;

                const int freecells_to_fill =
                    min(num_cards_to_relocate, num_vacant_freecells);

                num_cards_to_relocate -= freecells_to_fill;

                const int freestacks_to_fill =
                    min(num_cards_to_relocate, num_virtual_vacant_stacks);
                num_cards_to_relocate -= freestacks_to_fill;

                if (!((num_cards_to_relocate == 0) &&
                        (calc_max_sequence_move(
                             num_vacant_freecells - freecells_to_fill,
                             num_vacant_stacks - freestacks_to_fill) >=
                            cards_num - c)))
                {
                    continue;
                }

                /* We can move it */

                {
                    sfs_check_state_begin();
                    // Fill the freecells with the top cards
                    my_copy_stack(ds);

                    const int cols_indexes[3] = {ds, -1, -1};
                    empty_two_cols_from_new_state(soft_thread,
                        ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                        freestacks_to_fill + freecells_to_fill, 0);

                    my_copy_stack(stack_idx);

                    fcs_move_sequence(ds, stack_idx, cards_num - c);

                    state_context_value =
                        ((((((stack_idx << 8) | c) << 8) | ds) << 8) | dc);

                    sfs_check_state_end();
                }
            }
        }
    }

    sort_derived_states(derived_states_list, derived_start_idx);
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_empty_stack_into_freecells)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
    if (IS_FILLED_BY_NONE())
    {
        return;
    }
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    const fcs_game_limit_t num_vacant_freecells =
        soft_thread->num_vacant_freecells;

    /* Now, let's try to empty an entire stack into the freecells, so other
     * cards can
     * inhabit it */

    if ((!num_vacant_freecells) || soft_thread->num_vacant_stacks)
    {
        return;
    }
    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        const int cards_num = fcs_col_len(col);

        if ((!cards_num) || (cards_num > num_vacant_freecells))
        {
            continue;
        }
        /* We can empty it */
        sfs_check_state_begin();

        my_copy_stack(stack_idx);

        const fcs_cards_column_t new_src_col =
            fcs_state_get_col(new_state_key, stack_idx);

        int b = 0;
        for (int c = 0; c < cards_num; ++c, ++b)
        {
            /* Find a vacant freecell */
            for (; b < LOCAL_FREECELLS_NUM; ++b)
            {
                if (fcs_freecell_is_empty(new_state_key, b))
                {
                    break;
                }
            }
            fcs_card_t top_card;
            fcs_col_pop_card(new_src_col, top_card);

            fcs_put_card_in_freecell(new_state_key, b, top_card);

            fcs_move_stack_non_seq_push(
                moves, FCS_MOVE_TYPE_STACK_TO_FREECELL, stack_idx, b);
        }

        sfs_check_state_end();
    }
}

/* Disabling Yukon solving for the time being. */
#if 0
DECLARE_MOVE_FUNCTION(fc_solve_sfs_yukon_move_card_to_parent)
{
    int stack_idx, cards_num, c, ds;
    int dest_cards_num;
    fcs_card_t card;
    fcs_card_t dest_card;
    fcs_cards_column_t dest_col, col;

#ifndef HARD_CODED_NUM_STACKS
    const int stacks_num = INSTANCE_STACKS_NUM;
#endif
    tests_define_accessors();

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
    int stack_idx, cards_num, c, ds;
    fcs_card_t card;
    fcs_cards_column_t col;

#ifndef HARD_CODED_NUM_STACKS
    const int stacks_num = INSTANCE_STACKS_NUM;
#endif

    fcs_game_limit_t num_vacant_stacks;
    tests_define_accessors();

    num_vacant_stacks = soft_thread->num_vacant_stacks;
    if (num_vacant_stacks == 0)
    {
        return;
    }

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
            if (fcs_card_rank(card) == 13)
            {
                /* It's a King - so let's move it */
                sfs_check_state_begin();
                my_copy_stack(stack_idx);
                my_copy_stack(ds);
                fcs_move_sequence(ds, stack_idx, c, cards_num-1);
                sfs_check_state_end();
            }
        }
    }

    return;
}
#endif

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_empty_stack)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
    if (IS_FILLED_BY_NONE())
    {
        return;
    }
    if (soft_thread->num_vacant_stacks == 0)
    {
        return;
    }
#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    int empty_stack_idx;
    for (empty_stack_idx = 0; empty_stack_idx < LOCAL_STACKS_NUM;
         empty_stack_idx++)
    {
        if (fcs_col_len(fcs_state_get_col(state, empty_stack_idx)) == 0)
        {
            break;
        }
    }

    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        const int cards_num = fcs_col_len(col);

        /* Bug fix: if there's only one card in a column, there's no
         * point moving it to a new empty column.
         * */
        if (cards_num <= 1)
        {
            continue;
        }
        const fcs_card_t card = fcs_col_get_card(col, cards_num - 1);

        if (IS_FILLED_BY_KINGS_ONLY() && (fcs_card_rank(card) != 13))
        {
            continue;
        }
        /* Let's move it */
        sfs_check_state_begin();

        my_copy_stack(stack_idx);
        my_copy_stack(empty_stack_idx);
        fcs_state_pop_col_top(&new_state_key, stack_idx);

        fcs_cards_column_t empty_stack_col =
            fcs_state_get_col(new_state_key, empty_stack_idx);
        fcs_col_push_card(empty_stack_col, card);

        fcs_push_1card_seq(moves, stack_idx, empty_stack_idx);

        sfs_check_state_end();
    }
}

#define CALC_num_cards_in_col_threshold()                                      \
    (MOVE_FUNCS__should_not_empty_columns() ? 1 : 0)

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_parent)
{
    MOVE_FUNCS__define_common();
#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    const int num_cards_in_col_threshold = CALC_num_cards_in_col_threshold();

    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        const int cards_num = fcs_col_len(col);

        if (cards_num <= num_cards_in_col_threshold)
        {
            continue;
        }
        const fcs_card_t card = fcs_col_get_card(col, cards_num - 1);

        for (int ds = 0; ds < LOCAL_STACKS_NUM; ds++)
        {
            if (ds == stack_idx)
            {
                continue;
            }

            fcs_cards_column_t dest_col = fcs_state_get_col(state, ds);
            const int dest_cards_num = fcs_col_len(dest_col);

            if (!dest_cards_num)
            {
                continue;
            }
            const fcs_card_t dest_card =
                fcs_col_get_card(dest_col, fcs_col_len(dest_col) - 1);
            if (!fcs_is_parent_card(card, dest_card))
            {
                continue;
            }
            sfs_check_state_begin();

            my_copy_stack(stack_idx);
            my_copy_stack(ds);

            fcs_state_pop_col_top(&new_state_key, stack_idx);
            fcs_cards_column_t new_dest_col =
                fcs_state_get_col(new_state_key, ds);
            fcs_col_push_card(new_dest_col, card);
            fcs_push_1card_seq(moves, stack_idx, ds);

            sfs_check_state_end();
        }
    }
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_freecell)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    const fcs_game_limit_t num_vacant_freecells =
        soft_thread->num_vacant_freecells;

    if (num_vacant_freecells == 0)
    {
        return;
    }

    const int num_cards_in_col_threshold = CALC_num_cards_in_col_threshold();

    int ds;
    for (ds = 0; ds < LOCAL_FREECELLS_NUM; ds++)
    {
        if (fcs_freecell_is_empty(state, ds))
        {
            break;
        }
    }

    for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        fcs_cards_column_t col = fcs_state_get_col(state, stack_idx);
        const int cards_num = fcs_col_len(col);
        if (cards_num <= num_cards_in_col_threshold)
        {
            continue;
        }
        const fcs_card_t card = fcs_col_get_card(col, cards_num - 1);

        /* Let's move it */
        sfs_check_state_begin();

        my_copy_stack(stack_idx);
        fcs_state_pop_col_top(&new_state_key, stack_idx);
        fcs_put_card_in_freecell(new_state_key, ds, card);

        fcs_move_stack_non_seq_push(
            moves, FCS_MOVE_TYPE_STACK_TO_FREECELL, stack_idx, ds);

        sfs_check_state_end();
    }
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_freecell_card_to_parent)
{
    tests_define_accessors();
    MOVE_FUNCS__define_seqs_built_by();

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    for (int fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
    {
        const fcs_card_t card = fcs_freecell_card(state, fc);

        if (fcs_card_is_empty(card))
        {
            continue;
        }

        for (int ds = 0; ds < LOCAL_STACKS_NUM; ds++)
        {
            fcs_cards_column_t dest_col = fcs_state_get_col(state, ds);
            if (!fcs_col_len(dest_col))
            {
                continue;
            }
            const fcs_card_t dest_card =
                fcs_col_get_card(dest_col, fcs_col_len(dest_col) - 1);
            if (!fcs_is_parent_card(card, dest_card))
            {
                continue;
            }
            /* Let's move it */
            sfs_check_state_begin();

            my_copy_stack(ds);
            fcs_cards_column_t new_dest_col =
                fcs_state_get_col(new_state_key, ds);
            fcs_empty_freecell(new_state_key, fc);
            fcs_col_push_card(new_dest_col, card);
            fcs_move_stack_non_seq_push(
                moves, FCS_MOVE_TYPE_FREECELL_TO_STACK, fc, ds);

            sfs_check_state_end();
        }
    }

    return;
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_freecell_card_to_empty_stack)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif
    if (IS_FILLED_BY_NONE())
    {
        return;
    }

    if (soft_thread->num_vacant_stacks == 0)
    {
        return;
    }

    /* Find a vacant stack */
    const int ds = find_empty_stack(raw_ptr_state_raw, 0, LOCAL_STACKS_NUM);

    for (int fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
    {
        const fcs_card_t card = fcs_freecell_card(state, fc);

        if (fcs_card_is_empty(card) ||
            (IS_FILLED_BY_KINGS_ONLY() && (fcs_card_rank(card) != 13)))
        {
            continue;
        }

        sfs_check_state_begin();

        my_copy_stack(ds);
        fcs_cards_column_t new_dest_col = fcs_state_get_col(new_state_key, ds);
        fcs_empty_freecell(new_state_key, fc);
        fcs_col_push_card(new_dest_col, card);
        fcs_move_stack_non_seq_push(
            moves, FCS_MOVE_TYPE_FREECELL_TO_STACK, fc, ds);

        sfs_check_state_end();
    }

    return;
}

#define CALC_FOUNDATION_TO_PUT_CARD_ON()                                       \
    calc_foundation_to_put_card_on(soft_thread, pass_new_state.key, card)

static inline int calc_foundation_to_put_card_on(
    const fc_solve_soft_thread_t *const soft_thread,
    const fcs_state_t *const ptr_state, const fcs_card_t card)
{
#ifndef FCS_FREECELL_ONLY
    const_AUTO(instance, fcs_st_instance(soft_thread));
#endif
    MOVE_FUNCS__define_seqs_built_by();

    for (int deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
    {
        const int ret_val = ((deck << 2) | fcs_card_suit(card));
        if (fcs_foundation_value(*ptr_state, ret_val) ==
            fcs_card_rank(card) - 1)
        {
/* Always put on the foundation if it is built-by-suit */
#ifndef FCS_FREECELL_ONLY
            if (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT)
            {
                return ret_val;
            }
#endif

            int other_deck_idx;
            for (other_deck_idx = 0; other_deck_idx < (INSTANCE_DECKS_NUM << 2);
                 other_deck_idx++)
            {
                if (fcs_foundation_value(*ptr_state, other_deck_idx) <
                    fcs_card_rank(card) - 2 -
                        (FCS__SEQS_ARE_BUILT_BY_RANK()
                                ? 0
                                : ((other_deck_idx & 0x1) ==
                                      (fcs_card_suit(card) & 0x1))))
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

extern fcs_collectible_state_t *fc_solve_sfs_raymond_prune(
    fc_solve_soft_thread_t *const soft_thread,
    fcs_kv_state_t *const raw_ptr_state_raw)
{
    tests_define_accessors();
#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif

    fcs_derived_states_list_t derived_states_list_struct = {
        .states = NULL, .num_states = 0};

    sfs_check_state_begin();

    fcs_bool_t cards_were_moved = FALSE;
    uint_fast32_t num_cards_moved;
    do
    {
        num_cards_moved = 0;
        for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
        {
            const fcs_cards_column_t col =
                fcs_state_get_col(new_state_key, stack_idx);
            const int cards_num = fcs_col_len(col);

            if (cards_num)
            {
                /* Get the top card in the stack */
                const fcs_card_t card = fcs_col_get_card(col, cards_num - 1);

                const int dest_foundation = CALC_FOUNDATION_TO_PUT_CARD_ON();
                if (dest_foundation >= 0)
                {
                    /* We can safely move it. */
                    num_cards_moved++;

                    my_copy_stack(stack_idx);
                    fcs_state_pop_col_top(&new_state_key, stack_idx);
                    fcs_increment_foundation(new_state_key, dest_foundation);
                    fcs_move_stack_non_seq_push(moves,
                        FCS_MOVE_TYPE_STACK_TO_FOUNDATION, stack_idx,
                        dest_foundation);
                }
            }
        }

        /* Now check the same for the free cells */
        for (int fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
        {
            const fcs_card_t card = fcs_freecell_card(new_state_key, fc);
            if (fcs_card_is_valid(card))
            {
                const int dest_foundation = CALC_FOUNDATION_TO_PUT_CARD_ON();
                if (dest_foundation >= 0)
                {
                    num_cards_moved++;

                    /* We can put it there */

                    fcs_empty_freecell(new_state_key, fc);

                    fcs_increment_foundation(new_state_key, dest_foundation);
                    fcs_move_stack_non_seq_push(moves,
                        FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION, fc,
                        dest_foundation);
                }
            }
        }
        if (num_cards_moved)
        {
            cards_were_moved = TRUE;
        }
    } while (num_cards_moved);

#define derived_states_list (&derived_states_list_struct)
    sfs_check_state_end();
#undef derived_states_list

    register fcs_collectible_state_t *ptr_next_state;
    if (cards_were_moved)
    {
        ptr_next_state = derived_states_list_struct.states[0].state_ptr;

        /*
         * Set the GENERATED_BY_PRUNING flag uncondtionally. It won't
         * hurt if it's already there, and if it's a state that was
         * found by other means, we still shouldn't prune it, because
         * it is already "prune-perfect".
         * */
        FCS_S_VISITED(ptr_next_state) |= FCS_VISITED_GENERATED_BY_PRUNING;
    }
    else
    {
        ptr_next_state = NULL;
    }

    free(derived_states_list_struct.states);
    return ptr_next_state;
}
