// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// freecell.c - a module that contains moves of the Freecell-games family.

#include "scans.h"
#include "freecell.h"
#include "meta_move_funcs_helpers.h"

#define CALC_POSITIONS_BY_RANK()                                               \
    const int8_t *const positions_by_rank =                                    \
        fc_solve_calc_positions_by_rank_location(soft_thread);                 \
    const int suit_positions_by_rank_step = POS_BY_RANK_MAP(POS_BY_RANK_STEP)

#define POS_BY_RANK_MAP(x) ((x) << 1)

#ifdef FCS_BREAK_BACKWARD_COMPAT_2
#define RAR
#endif

static inline int find_empty_stack(fcs_kv_state raw_state_raw,
    const int start_from, const int local_stacks_num)
{
    for (int ret = start_from; ret < local_stacks_num; ret++)
    {
        if (fcs_state_col_is_empty(state_key, ret))
        {
            return ret;
        }
    }
    return -1;
}

static inline void sort_derived_states(
    fcs_derived_states_list *const d, const size_t derived_start_idx)
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
 * Throughout this code the following local variables are used to quickly
 * access the instance's members:
 *
 * LOCAL_STACKS_NUM - the number of stacks in the state
 * LOCAL_FREECELLS_NUM - the number of freecells in the state
 * sequences_are_built_by - the type of sequences of this board.
 * */

#if MAX_NUM_FREECELLS == 0
DECLARE_PURE_MOVE_FUNCTION(fc_solve_sfs_null_move_func) {}
#endif

/*
 * This function tries to move stack cards that are present at the
 * top of stacks to the foundations.
 * */
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_top_stack_cards_to_founds)
{
    tests_define_accessors();
    STACKS__SET_PARAMS();

    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
        const size_t cards_num = fcs_col_len(col);
        if (!cards_num)
        {
            continue;
        }
        /* Get the top card in the stack */
        const fcs_card card = fcs_col_get_card(col, cards_num - 1);
        for (size_t deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
        {
            if (fcs_foundation_value(state_key,
                    deck * 4 + fcs_card_suit(card)) != fcs_card_rank(card) - 1)
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
// #ifdef FCS_BREAK_BACKWARD_COMPAT_2
#if 0
            return;
#else
            break;
#endif
        }
    }
}

#if MAX_NUM_FREECELLS > 0
#ifdef FCS_BREAK_BACKWARD_COMPAT_2
#define RAR2
#endif
// This test moves single cards that are present in the freecells to
// the foundations.
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_to_founds)
{
    tests_define_accessors_no_stacks(tests_state_context_val);

#ifndef HARD_CODED_NUM_FREECELLS
    SET_GAME_PARAMS();
#endif

#ifdef RAR2
    for (int fc = LOCAL_FREECELLS_NUM - 1; fc >= 0; --fc)
#else
    for (int fc = 0; fc < LOCAL_FREECELLS_NUM; ++fc)
#endif
    {
        const fcs_card card = fcs_freecell_card(state_key, fc);

        if (fcs_card_is_empty(card))
        {
            continue;
        }
        for (stack_i deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
        {
            if (fcs_foundation_value(state_key,
                    deck * 4 + fcs_card_suit(card)) != fcs_card_rank(card) - 1)
            {
                continue;
            }
            /* We can put it there */
            sfs_check_state_begin();

            fcs_empty_freecell(new_state_key, fc);
            fcs_increment_foundation(
                new_state_key, deck * 4 + fcs_card_suit(card));
            fcs_move_stack_non_seq_push(moves,
                FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION, (size_t)fc,
                deck * 4 + fcs_card_suit(card));

            sfs_check_state_end();
// #ifdef FCS_BREAK_BACKWARD_COMPAT_2
#if 0
            return;
#else
            break;
#endif
        }
    }
}
#endif

typedef struct
{
    int src_idx;
    bool is_col;
} empty_two_cols_ret;
/*
 * This function empties two stacks from the new state
 * into freeeclls and empty columns
 */
static inline empty_two_cols_ret empty_two_cols_from_new_state(
    const fcs_soft_thread *const soft_thread GCC_UNUSED,
    fcs_kv_state *const kv_ptr_new_state SFS__PASS_MOVE_STACK(
        fcs_move_stack *const moves_ptr),
    const int cols_indexes[3], const int_fast16_t num_cards_1,
    const int_fast16_t num_cards_2)
{
    empty_two_cols_ret ret = {.src_idx = -1, .is_col = false};

    int_fast16_t num_cards_to_move_from_columns[3] = {
        num_cards_1, num_cards_2, -1};

    const int *col_idx = cols_indexes;
    int_fast16_t *col_num_cards = num_cards_to_move_from_columns;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_INSTANCE_GAME_PARAMS(fcs_st_instance(soft_thread));
#endif

#ifdef INDIRECT_STACK_STATES
    fcs_card *const indirect_stacks_buffer =
        HT_FIELD(soft_thread->hard_thread, indirect_stacks_buffer);
#endif

    const_AUTO(new_key, kv_ptr_new_state->key);
#if MAX_NUM_FREECELLS > 0
    {
        size_t dest_fc_idx = 0;

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

            const_AUTO(
                top_card, fcs_state_pop_col_card(new_key, (stack_i)*col_idx));
            fcs_put_card_in_freecell(*new_key, dest_fc_idx, top_card);

            fcs_move_stack_non_seq_push(moves_ptr,
                FCS_MOVE_TYPE_STACK_TO_FREECELL, (size_t)*col_idx, dest_fc_idx);

            ret = (empty_two_cols_ret){
                .src_idx = (int)dest_fc_idx, .is_col = false};
            --(*col_num_cards);
            ++dest_fc_idx;
        }
    }
#endif

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
            *kv_ptr_new_state, put_cards_in_col_idx, LOCAL_STACKS_NUM);

        fcs_copy_stack(*new_key, *(kv_ptr_new_state->val), put_cards_in_col_idx,
            indirect_stacks_buffer);

        const size_t col_idx_val = (size_t)*col_idx;
        const_AUTO(top_card, fcs_state_pop_col_card(new_key, col_idx_val));
        fcs_state_push(new_key, (size_t)put_cards_in_col_idx, top_card);

        fcs_push_1card_seq(
            moves_ptr, col_idx_val, (size_t)put_cards_in_col_idx);

        ret = (empty_two_cols_ret){
            .src_idx = put_cards_in_col_idx, .is_col = true};
        --(*col_num_cards);
        ++put_cards_in_col_idx;
    }
}

static inline fcs_game_limit calc_num_vacant_slots(
    const fcs_soft_thread *const soft_thread, const bool is_filled_by_any_card)
{
    return (soft_thread->num_vacant_freecells +
            (is_filled_by_any_card ? soft_thread->num_vacant_stacks : 0));
}

#if MAX_NUM_FREECELLS > 0
#ifndef FCS_BREAK_BACKWARD_COMPAT_2
#define FLUT
#endif
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_on_top_of_stacks)
{
    MOVE_FUNCS__define_common();
    HARD__SET_GAME_PARAMS();

    const fcs_game_limit num_vacant_slots =
        calc_num_vacant_slots(soft_thread, tests__is_filled_by_any_card());

#ifdef FLUT
    const size_t derived_start_idx = derived_states_list->num_states;
#endif

    CALC_POSITIONS_BY_RANK();
    /* Let's try to put cards in the freecells on top of stacks */

    /* Scan the freecells */
    for (stack_i fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
    {
        const fcs_card src_card = fcs_freecell_card(state_key, fc);

        /* If the freecell is not empty and dest_card is its parent
         * */
        if (!(
                /* The Cell should not be empty. */
                fcs_card_is_valid(src_card) &&
                /* We cannot put a king anywhere. */
                (!fcs_card_is_king(src_card))))
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

            const_AUTO(dest_col, fcs_state_get_col(state_key, ds));
            const int dest_cards_num = fcs_col_len(dest_col);
            /* Let's check if we can put it there */

            /* Check if the destination card is already below a
             * suitable card */
            const int_fast32_t next_dc = dc + 1;
/* If dest_cards_num == next_dc then
 * dest_cards_num - next_dc == 0 <= 0 so the other check
 * cab be skipped.
 * */

// We don't need this check because the positions_by_rank
// already filters the fcs_is_parent_card check for us.
#ifdef FCS_POS_BY_RANK__ENABLE_PARENT_CHECK
            const fcs_card dest_card = fcs_col_get_card(dest_col, dc);
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
            const int cols_indexes[3] = {(int)ds, -1, -1};
            empty_two_cols_from_new_state(soft_thread,
                ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                (int)(dest_cards_num - dc - 1), 0);

            /* Now put the freecell card on top of the stack */
            fcs_state_push(&new_state_key, (stack_i)ds, src_card);
            fcs_empty_freecell(new_state_key, fc);
            fcs_move_stack_non_seq_push(
                moves, FCS_MOVE_TYPE_FREECELL_TO_STACK, fc, (stack_i)ds);

#ifdef FLUT
            /*
             * This is to preserve the order that the
             * initial (non-optimized) version of the
             * function used - for backwards-compatibility
             * and consistency.
             * */
            state_context_value =
                (int)(((stack_i)ds << 16U) | ((255 - (stack_i)dc) << 8U) | fc);
#endif

            sfs_check_state_end();
        }
    }

#ifdef FLUT
    sort_derived_states(derived_states_list, derived_start_idx);
#endif
}
#endif

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_non_top_stack_cards_to_founds)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
    FC__STACKS__SET_PARAMS();
    const fcs_game_limit num_vacant_slots =
        calc_num_vacant_slots(soft_thread, tests__is_filled_by_any_card());
#ifdef RAR
    const size_t derived_start_idx = derived_states_list->num_states;
#endif

    /* Now let's check if a card that is under some other cards can be placed
     * in the foundations. */

    const_AUTO(num_vacant_slots_plus_1, num_vacant_slots + 1);

    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
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
            const fcs_card card = fcs_col_get_card(col, c);

            for (stack_i deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
            {
                const stack_i dest_found = deck * 4 + fcs_card_suit(card);
                if (fcs_foundation_value(state_key, dest_found) !=
                    fcs_card_rank(card) - 1)
                {
                    continue;
                }
                /* We can move it */
                sfs_check_state_begin();
                my_copy_stack(stack_idx);
                const int cols_indexes[3] = {(int)stack_idx, -1, -1};

                empty_two_cols_from_new_state(soft_thread,
                    ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                    cards_num - (c + 1), 0);
                fcs_state_pop_col_top(&new_state_key, stack_idx);
                fcs_increment_foundation(new_state_key, dest_found);
                fcs_move_stack_non_seq_push(moves,
                    FCS_MOVE_TYPE_STACK_TO_FOUNDATION, stack_idx, dest_found);
#ifdef RAR
#if 1
                state_context_value =
                    (int)(((size_t)(fcs_card_rank(card)) << 8U) |
                          fcs_card_suit(card));
#else
                state_context_value = card;
#endif
#endif
                sfs_check_state_end();
#ifdef FCS_BREAK_BACKWARD_COMPAT_2
                goto end_loop;
#else
                break;
#endif
            }
        }
#ifdef FCS_BREAK_BACKWARD_COMPAT_2
    end_loop:;
#endif
    }
#ifdef RAR
    sort_derived_states(derived_states_list, derived_start_idx);
#endif
}

DECLARE_MOVE_FUNCTION(
    fc_solve_sfs_move_stack_cards_to_a_parent_on_the_same_stack)
{
    MOVE_FUNCS__define_common();
    FC__STACKS__SET_PARAMS();
    const fcs_game_limit num_vacant_slots_plus_1 =
        calc_num_vacant_slots(soft_thread, tests__is_filled_by_any_card()) + 1;

    /*
     * Now let's try to move a stack card to a parent card which is found
     * on the same stack.
     * */
    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
        const int cards_num = fcs_col_len(col);
        const int start_dc = max0(cards_num - num_vacant_slots_plus_1);

        for (int c = start_dc + 2; c < cards_num; c++)
        {
            /* Find a card which this card can be put on; */

            const fcs_card card = fcs_col_get_card(col, c);

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
            /* Check if it can be moved to something on the same stack */
            for (int dc = start_dc; dc < below_c; dc++)
            {
                const fcs_card dest_card = fcs_col_get_card(col, dc);
                const int next_dc = dc + 1;
                if ((!fcs_is_parent_card(card, dest_card)) ||
                    /* Corresponding cards - see if it is feasible to move
                       the source to the destination. */
                    fcs_is_parent_card(
                        fcs_col_get_card(col, next_dc), dest_card))
                {
                    continue;
                }
                /* We can move it */
                sfs_check_state_begin();

                my_copy_stack(ds);
                const int cols_indexes[3] = {(int)ds, -1, -1};
                const empty_two_cols_ret last_dest =
                    empty_two_cols_from_new_state(soft_thread,
                        ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                        /* We're moving one extra card */
                        cards_num - c, 0);

                empty_two_cols_from_new_state(soft_thread,
                    ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                    below_c - dc, 0);

                fcs_card moved_card;
#define s_idx ((stack_i)last_dest.src_idx)
#if MAX_NUM_FREECELLS > 0
                if (last_dest.is_col)
#endif
                {
                    var_AUTO(new_source_col,
                        fcs_state_get_col(new_state_key, s_idx));
                    fcs_col_pop_card(new_source_col, moved_card);
                    fcs_push_1card_seq(moves, s_idx, ds);
                }
#if MAX_NUM_FREECELLS > 0
                else
                {
                    moved_card = fcs_freecell_card(new_state_key, s_idx);
                    fcs_empty_freecell(new_state_key, s_idx);
                    fcs_move_stack_non_seq_push(
                        moves, FCS_MOVE_TYPE_FREECELL_TO_STACK, s_idx, ds);
                }
#endif
#undef s_idx
                fcs_state_push(&new_state_key, ds, moved_card);

                sfs_check_state_end();
            }
        }
    }
}
#undef ds

#define CALC_num_virtual_vacant_stacks()                                       \
    (tests__is_filled_by_any_card() ? num_vacant_stacks : 0)

typedef struct
{
    fcs_cards_column col;
    int_fast16_t col_len, col_len_minus_1, c, seq_end;
    FCS_ON_NOT_FC_ONLY(int sequences_are_built_by;)
} col_seqs_iter;

static inline void col_seqs_iter__calc_end(col_seqs_iter *const iter)
{
    FCS_ON_NOT_FC_ONLY(const_SLOT(sequences_are_built_by, iter));
    const_SLOT(col, iter);
    for ((*iter).seq_end = (*iter).c; (*iter).seq_end < (*iter).col_len_minus_1;
         ++((*iter).seq_end))
    {
        if (!fcs_is_parent_card(fcs_col_get_card(col, (*iter).seq_end + 1),
                fcs_col_get_card(col, (*iter).seq_end)))
        {
            break;
        }
    }
}

static inline col_seqs_iter col_seqs_iter__create(
    fcs_state *const s, const stack_i stack_idx PASS_sequences_are_built_by(
                            const int sequences_are_built_by))
{
    col_seqs_iter ret;
    FCS_ON_NOT_FC_ONLY(ret.sequences_are_built_by = sequences_are_built_by);
    ret.col = fcs_state_get_col(*s, stack_idx);
    ret.col_len_minus_1 = (ret.col_len = fcs_col_len(ret.col)) - 1;
    ret.c = 0;
    col_seqs_iter__calc_end(&ret);
    return ret;
}

static inline void col_seqs_iter__advance(col_seqs_iter *const iter)
{
    iter->c = iter->seq_end + 1;
    col_seqs_iter__calc_end(iter);
}

static inline bool check_if_can_relocate(const fcs_game_limit start,
    const fcs_game_limit num_virtual_vacant_freecells,
    const fcs_game_limit num_virtual_vacant_stacks,
    const col_seqs_iter *const iter PASS_sequences_are_built_by(
        const fcs_instance *const instance))
{
    MOVE_FUNCS__define_empty_stacks_fill();
    fcs_game_limit num_cards_to_relocate = start;
    const fcs_game_limit freecells_to_fill =
        min(num_cards_to_relocate, num_virtual_vacant_freecells);

    num_cards_to_relocate -= freecells_to_fill;

    const fcs_game_limit freestacks_to_fill =
        min(num_cards_to_relocate, num_virtual_vacant_stacks);
    num_cards_to_relocate -= freestacks_to_fill;

    return ((num_cards_to_relocate == 0) &&
            (calc_max_sequence_move(
                 num_virtual_vacant_freecells - freecells_to_fill,
                 num_virtual_vacant_stacks - freestacks_to_fill) >=
                iter->seq_end - iter->c + 1));
}

#ifdef FCS_BREAK_BACKWARD_COMPAT_2
#define CLAP
#endif
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_stack_cards_to_different_stacks)
{
    MOVE_FUNCS__define_common();
    HARD__SET_GAME_PARAMS();
    const_SLOT(num_vacant_freecells, soft_thread);
    const_SLOT(num_vacant_stacks, soft_thread);
    const fcs_game_limit num_virtual_vacant_stacks =
        CALC_num_virtual_vacant_stacks();
#ifndef CLAP
    const size_t derived_start_idx = derived_states_list->num_states;
#endif

    CALC_POSITIONS_BY_RANK();
    /* Now let's try to move a card from one stack to the other     *
     * Note that it does not involve moving cards lower than king   *
     * to empty stacks                                              */

#ifdef RAR2__cancelled
    for (stack_i stack_idx = LOCAL_STACKS_NUM - 1; stack_idx >= 0; --stack_idx)
#else
    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
#endif
    {
        col_seqs_iter iter = col_seqs_iter__create(&state_key,
            stack_idx PASS_sequences_are_built_by(sequences_are_built_by));
        for (; iter.c < iter.col_len; col_seqs_iter__advance(&iter))
        {
            if (MOVE_FUNCS__should_not_empty_columns() && (iter.c == 0))
            {
                continue;
            }
            const int_fast16_t col_num_cards =
                iter.col_len_minus_1 - iter.seq_end;
            /* Find a card which this card can be put on; */

            const fcs_card card = fcs_col_get_card(iter.col, iter.c);
            /* Skip if it's a King - nothing to put it on. */
            if (unlikely(fcs_card_is_king(card)))
            {
                continue;
            }

            FCS_POS_IDX_TO_CHECK_START_LOOP(card)
            {
                const int_fast32_t ds = pos_idx_to_check[0];

                if ((ds < 0) || ((stack_i)ds == stack_idx))
                {
                    continue;
                }
                const int_fast32_t dc = pos_idx_to_check[1];
                const stack_i dest_cards_num =
                    fcs_state_col_len(state_key, ds) - (stack_i)dc - 1;
                if (!unlikely(check_if_can_relocate(
                        (fcs_game_limit)(
                            dest_cards_num + (stack_i)col_num_cards),
                        num_vacant_freecells, num_virtual_vacant_stacks,
                        &iter PASS_sequences_are_built_by(instance))))
                {
                    continue;
                }
                sfs_check_state_begin();
                copy_two_stacks(stack_idx, ds);
                const int cols_indexes[3] = {(int)ds, (int)stack_idx, -1};
                empty_two_cols_from_new_state(soft_thread,
                    ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                    (int)dest_cards_num, col_num_cards);
                fcs_move_sequence((stack_i)ds, stack_idx,
                    (stack_i)(iter.seq_end - iter.c + 1));
                /*
                 * This is to preserve the order that the
                 * initial (non-optimized) version of the
                 * function used - for backwards-compatibility
                 * and consistency.
                 * */
#ifndef CLAP
                state_context_value =
                    (int)((((((stack_idx << 8) | (stack_i)iter.c) << 8) |
                               (stack_i)ds)
                              << 8) |
                          (stack_i)dc);
#endif

                sfs_check_state_end();
            }
        }
    }
#ifndef CLAP
    sort_derived_states(derived_states_list, derived_start_idx);
#endif
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_sequences_to_free_stacks)
{
    MOVE_FUNCS__define_common();
    if (IS_FILLED_BY_NONE())
    {
        return;
    }
    FC__STACKS__SET_PARAMS();
    const fcs_game_limit num_vacant_stacks = soft_thread->num_vacant_stacks;
    if (num_vacant_stacks == 0)
    {
        return;
    }
    const fcs_game_limit num_vacant_freecells =
        soft_thread->num_vacant_freecells;
    const fcs_game_limit num_virtual_vacant_stacks =
        CALC_num_virtual_vacant_stacks();
    const size_t max_sequence_len = (size_t)calc_max_sequence_move(
        num_vacant_freecells, num_vacant_stacks - 1);

    /* Now try to move sequences to empty stacks */
    const int ds = find_empty_stack(raw_state_raw, 0, LOCAL_STACKS_NUM);
    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        col_seqs_iter iter = col_seqs_iter__create(&state_key,
            stack_idx PASS_sequences_are_built_by(sequences_are_built_by));
        for (; iter.c < iter.col_len; col_seqs_iter__advance(&iter))
        {
            if (IS_FILLED_BY_KINGS_ONLY() &&
                (!fcs_col_is_king(iter.col, (stack_i)iter.c)))
            {
                continue;
            }

            if (iter.seq_end == iter.col_len_minus_1)
            {
                stack_i c = (stack_i)iter.c;
                /* One stack is the destination stack, so we have one     *
                 * less stack in that case                                */
                while (
                    (max_sequence_len < (stack_i)iter.col_len - c) && (c > 0))
                {
                    c--;
                }

                if (!((c > 0) && ((IS_FILLED_BY_KINGS_ONLY())
                                         ? fcs_col_is_king(iter.col, c)
                                         : true)))
                {
                    continue;
                }
                sfs_check_state_begin();
                copy_two_stacks(stack_idx, ds);
                fcs_move_sequence(
                    (stack_i)ds, stack_idx, (stack_i)iter.col_len - c);

                sfs_check_state_end();
            }
            else
            {
                long num_cards_to_relocate =
                    iter.col_len_minus_1 - iter.seq_end;

                const long freecells_to_fill =
                    min(num_cards_to_relocate, (long)num_vacant_freecells);

                num_cards_to_relocate -= freecells_to_fill;

                const long freestacks_to_fill =
                    min(num_cards_to_relocate, num_virtual_vacant_stacks);
                num_cards_to_relocate -= freestacks_to_fill;

                if (!((num_cards_to_relocate == 0) &&
                        (num_vacant_stacks - freestacks_to_fill > 0)))
                {
                    continue;
                }
                /* We can move it */
                const long seq_start = ({
                    const long max_seq_move = (long)calc_max_sequence_move(
                        num_vacant_freecells - freecells_to_fill,
                        num_vacant_stacks - freestacks_to_fill - 1);
                    const long m = iter.seq_end + 1 - max_seq_move;
                    max(m, iter.c);
                });
                if (!((seq_start <= iter.seq_end) &&
                        ((IS_FILLED_BY_KINGS_ONLY())
                                ? fcs_col_is_king(iter.col, (stack_i)seq_start)
                                : true)))
                {
                    continue;
                }
                sfs_check_state_begin();

                /* Fill the freecells with the top cards */
                my_copy_stack(stack_idx);
                const int cols_indexes[3] = {(int)stack_idx, -1, -1};
                const empty_two_cols_ret empty_ret =
                    empty_two_cols_from_new_state(soft_thread,
                        ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                        freecells_to_fill + freestacks_to_fill, 0);

                const stack_i b = (stack_i)find_empty_stack(raw_state_raw,
                    (empty_ret.is_col ? empty_ret.src_idx + 1 : 0),
                    LOCAL_STACKS_NUM);
                my_copy_stack(b);

                fcs_move_sequence(
                    b, stack_idx, (stack_i)(iter.seq_end - seq_start + 1));

                sfs_check_state_end();
            }
        }
    }
}

#if MAX_NUM_FREECELLS > 0
/* Let's try to put cards that occupy freecells on an empty stack */
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_to_empty_stack)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
    if (IS_FILLED_BY_NONE())
    {
        return;
    }
    FC__STACKS__SET_PARAMS();
    if (!soft_thread->num_vacant_stacks)
    {
        return;
    }
    SET_empty_stack_idx(empty_stack_idx);

    for (int fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
    {
        const fcs_card card = fcs_freecell_card(state_key, fc);
#define SHOULD_SKIP_FC_CARD(card)                                              \
    (fcs_card_is_empty(card) ||                                                \
        (IS_FILLED_BY_KINGS_ONLY() && !fcs_card_is_king(card)))
        if (SHOULD_SKIP_FC_CARD(card))
        {
            continue;
        }
        /* We can move it */
        sfs_check_state_begin();

        my_copy_stack(empty_stack_idx);

        fcs_state_push(&new_state_key, (stack_i)empty_stack_idx, card);
        fcs_empty_freecell(new_state_key, fc);
        fcs_move_stack_non_seq_push(moves, FCS_MOVE_TYPE_FREECELL_TO_STACK,
            (stack_i)fc, (stack_i)empty_stack_idx);
        sfs_check_state_end();
    }
}

/* Let's try to put cards that occupy freecells on an empty stack and
 * immediately put a child card on top. See:
https://groups.yahoo.com/neo/groups/fc-solve-discuss/conversations/messages/584
 * */
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_fc_to_empty_and_put_on_top)
{
    MOVE_FUNCS__define_common();
    if (IS_FILLED_BY_NONE())
    {
        return;
    }
    FC__STACKS__SET_PARAMS();
    const fcs_game_limit num_vacant_freecells =
        soft_thread->num_vacant_freecells;
    const fcs_game_limit num_virtual_vacant_freecells =
        num_vacant_freecells + 1;
    const fcs_game_limit num_vacant_stacks = soft_thread->num_vacant_stacks;
    const fcs_game_limit num_virtual_vacant_stacks = num_vacant_stacks - 1;
    if (!num_vacant_stacks)
    {
        return;
    }
    SET_empty_stack_idx(empty_stack_idx);

#ifdef RAR2
    for (int fc = LOCAL_FREECELLS_NUM - 1; fc >= 0; --fc)
#else
    for (int fc = 0; fc < LOCAL_FREECELLS_NUM; ++fc)
#endif
    {
        const fcs_card src_card = fcs_freecell_card(state_key, fc);
        if (SHOULD_SKIP_FC_CARD(src_card))
        {
            continue;
        }
#ifdef RAR2__cancelled
        for (int dest_fc = LOCAL_FREECELLS_NUM - 1; dest_fc >= 0; --dest_fc)
#else
        for (int dest_fc = 0; dest_fc < LOCAL_FREECELLS_NUM; ++dest_fc)
#endif
        {
            if (dest_fc == fc)
            {
                continue;
            }
            const fcs_card dest_card = fcs_freecell_card(state_key, dest_fc);
            if (fcs_card_is_empty(dest_card))
            {
                continue;
            }
            if (!unlikely(fcs_is_parent_card(dest_card, src_card)))
            {
                continue;
            }
            sfs_check_state_begin();

            my_copy_stack(empty_stack_idx);

            fcs_state_push(&new_state_key, (stack_i)empty_stack_idx, src_card);
            fcs_state_push(&new_state_key, (stack_i)empty_stack_idx, dest_card);
            fcs_empty_freecell(new_state_key, fc);
            fcs_empty_freecell(new_state_key, dest_fc);
            fcs_move_stack_non_seq_push(moves, FCS_MOVE_TYPE_FREECELL_TO_STACK,
                (stack_i)fc, (stack_i)empty_stack_idx);
            fcs_move_stack_non_seq_push(moves, FCS_MOVE_TYPE_FREECELL_TO_STACK,
                (stack_i)dest_fc, (stack_i)empty_stack_idx);
            sfs_check_state_end();
        }
#ifdef RAR2__cancelled
        for (stack_i stack_idx = LOCAL_STACKS_NUM - 1; stack_idx >= 0;
             --stack_idx)
#else
        for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; ++stack_idx)
#endif
        {
            col_seqs_iter iter = col_seqs_iter__create(&state_key,
                stack_idx PASS_sequences_are_built_by(sequences_are_built_by));
            for (; iter.c < iter.col_len; col_seqs_iter__advance(&iter))
            {
                if (MOVE_FUNCS__should_not_empty_columns() && (iter.c == 0))
                {
                    continue;
                }
                const long col_num_cards = iter.col_len_minus_1 - iter.seq_end;
                /* Find a card which this card can be put on; */

                const fcs_card card = fcs_col_get_card(iter.col, iter.c);
                if (!unlikely(fcs_is_parent_card(card, src_card)))
                {
                    continue;
                }
                if (!unlikely(check_if_can_relocate(
                        (fcs_game_limit)col_num_cards,
                        num_virtual_vacant_freecells, num_virtual_vacant_stacks,
                        &iter PASS_sequences_are_built_by(instance))))
                {
                    continue;
                }
                sfs_check_state_begin();
                copy_two_stacks(stack_idx, empty_stack_idx);
                fcs_state_push(
                    &new_state_key, (stack_i)empty_stack_idx, src_card);
                fcs_empty_freecell(new_state_key, fc);
                fcs_move_stack_non_seq_push(moves,
                    FCS_MOVE_TYPE_FREECELL_TO_STACK, (stack_i)fc,
                    (stack_i)empty_stack_idx);
                const int cols_indexes[3] = {(int)stack_idx, -1, -1};
                empty_two_cols_from_new_state(soft_thread,
                    ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                    col_num_cards, 0);
                fcs_move_sequence((stack_i)empty_stack_idx, stack_idx,
                    (stack_i)(iter.seq_end - iter.c + 1));
                sfs_check_state_end();
            }
        }
    }
}
#endif

DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_cards_to_a_different_parent)
{
    MOVE_FUNCS__define_common();
    HARD__SET_GAME_PARAMS();
    const fcs_game_limit num_vacant_freecells =
        soft_thread->num_vacant_freecells;
    const fcs_game_limit num_vacant_stacks = soft_thread->num_vacant_stacks;
    const fcs_game_limit num_virtual_vacant_stacks =
        CALC_num_virtual_vacant_stacks();
#ifndef FCS_BREAK_BACKWARD_COMPAT_2
#define BUTTER
#endif
#ifndef BUTTER
    const size_t derived_start_idx = derived_states_list->num_states;
#endif

    CALC_POSITIONS_BY_RANK();
    /* This time try to move cards that are already on top of a parent to a
     * different parent */

    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
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

        fcs_card upper_card = fcs_col_get_card(col, cards_num - 1);

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
        fcs_card lower_card;
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

            const fcs_card card = fcs_col_get_card(col, c);
            FCS_POS_IDX_TO_CHECK_START_LOOP(card)
            {
                const int_fast32_t ds = pos_idx_to_check[0];
                if ((ds == -1) || ((typeof(stack_idx))ds == stack_idx))
                {
                    continue;
                }
                const int_fast32_t dc = pos_idx_to_check[1];

                var_AUTO(dest_col, fcs_state_get_col(state_key, ds));
                const int dest_cards_num = fcs_col_len(dest_col);

/* Corresponding cards - see if it is feasible to move
   the source to the destination. */

// We don't need this check because the positions_by_rank
// already filters the fcs_is_parent_card check for us.
#ifdef FCS_POS_BY_RANK__ENABLE_PARENT_CHECK
                const_AUTO(dest_card, fcs_col_get_card(dest_col, dc));
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

                long num_cards_to_relocate = dest_cards_num - dc - 1;

                const long freecells_to_fill =
                    min(num_cards_to_relocate, num_vacant_freecells);

                num_cards_to_relocate -= freecells_to_fill;

                const long freestacks_to_fill =
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
                sfs_check_state_begin();
                // Fill the freecells with the top cards
                copy_two_stacks(stack_idx, ds);
                const int cols_indexes[3] = {(int)ds, -1, -1};
                empty_two_cols_from_new_state(soft_thread,
                    ptr_new_state SFS__PASS_MOVE_STACK(moves), cols_indexes,
                    freestacks_to_fill + freecells_to_fill, 0);
                fcs_move_sequence(
                    (stack_i)ds, stack_idx, (stack_i)(cards_num - c));

#ifndef BUTTER
                state_context_value =
                    (int)((((((stack_idx << 8) | (size_t)c) << 8) | (size_t)ds)
                              << 8) |
                          (size_t)dc);
#endif

                sfs_check_state_end();
            }
        }
    }

#ifndef BUTTER
    sort_derived_states(derived_states_list, derived_start_idx);
#endif
}

#if MAX_NUM_FREECELLS > 0
DECLARE_MOVE_FUNCTION(fc_solve_sfs_empty_stack_into_freecells)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
    if (IS_FILLED_BY_NONE())
    {
        return;
    }
    FC__STACKS__SET_PARAMS();
    const fcs_game_limit num_vacant_freecells =
        soft_thread->num_vacant_freecells;

    /* Now, let's try to empty an entire stack into the freecells, so other
     * cards can
     * inhabit it */

    if ((!num_vacant_freecells) || soft_thread->num_vacant_stacks)
    {
        return;
    }
    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
        const int cards_num = fcs_col_len(col);

        if ((!cards_num) || (cards_num > num_vacant_freecells))
        {
            continue;
        }
        /* We can empty it */
        sfs_check_state_begin();

        my_copy_stack(stack_idx);

        const_AUTO(new_src_col, fcs_state_get_col(new_state_key, stack_idx));

        stack_i b = 0;
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
            fcs_card top_card;
            fcs_col_pop_card(new_src_col, top_card);

            fcs_put_card_in_freecell(new_state_key, b, top_card);

            fcs_move_stack_non_seq_push(
                moves, FCS_MOVE_TYPE_STACK_TO_FREECELL, stack_idx, b);
        }

        sfs_check_state_end();
    }
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
    STACKS__SET_PARAMS();
    SET_empty_stack_idx(empty_stack_idx);

    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
        const int cards_num = fcs_col_len(col);

        /* Bug fix: if there's only one card in a column, there's no
         * point moving it to a new empty column.
         * */
        if (cards_num <= 1)
        {
            continue;
        }
        const fcs_card card = fcs_col_get_card(col, cards_num - 1);

        if (IS_FILLED_BY_KINGS_ONLY() && (!fcs_card_is_king(card)))
        {
            continue;
        }
        /* Let's move it */
        sfs_check_state_begin();
        copy_two_stacks(stack_idx, empty_stack_idx);
        fcs_state_pop_col_top(&new_state_key, stack_idx);
        fcs_state_push(&new_state_key, empty_stack_idx, card);
        fcs_push_1card_seq(moves, stack_idx, empty_stack_idx);

        sfs_check_state_end();
    }
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_parent)
{
    MOVE_FUNCS__define_common();
    STACKS__SET_PARAMS();
    const int num_cards_in_col_threshold = CALC_num_cards_in_col_threshold();

    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
        const int cards_num = fcs_col_len(col);

        if (cards_num <= num_cards_in_col_threshold)
        {
            continue;
        }
        const fcs_card card = fcs_col_get_card(col, cards_num - 1);

        for (int ds = 0; ds < LOCAL_STACKS_NUM; ds++)
        {
            if ((stack_i)ds == stack_idx)
            {
                continue;
            }

            var_AUTO(dest_col, fcs_state_get_col(state_key, ds));
            const int dest_cards_num = fcs_col_len(dest_col);

            if (!dest_cards_num)
            {
                continue;
            }
            const fcs_card dest_card =
                fcs_col_get_card(dest_col, fcs_col_len(dest_col) - 1);
            if (!fcs_is_parent_card(card, dest_card))
            {
                continue;
            }
            sfs_check_state_begin();
            copy_two_stacks(stack_idx, ds);
            fcs_state_pop_col_top(&new_state_key, stack_idx);
            fcs_state_push(&new_state_key, (stack_i)ds, card);
            fcs_push_1card_seq(moves, stack_idx, (stack_i)ds);

            sfs_check_state_end();
        }
    }
}

#if MAX_NUM_FREECELLS > 0
DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_freecell)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
    FC__STACKS__SET_PARAMS();
    const fcs_game_limit num_vacant_freecells =
        soft_thread->num_vacant_freecells;

    if (num_vacant_freecells == 0)
    {
        return;
    }

    const int num_cards_in_col_threshold = CALC_num_cards_in_col_threshold();

    int ds;
    for (ds = 0; ds < LOCAL_FREECELLS_NUM; ds++)
    {
        if (fcs_freecell_is_empty(state_key, ds))
        {
            break;
        }
    }

    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        var_AUTO(col, fcs_state_get_col(state_key, stack_idx));
        const int cards_num = fcs_col_len(col);
        if (cards_num <= num_cards_in_col_threshold)
        {
            continue;
        }
        const fcs_card card = fcs_col_get_card(col, cards_num - 1);

        /* Let's move it */
        sfs_check_state_begin();

        my_copy_stack(stack_idx);
        fcs_state_pop_col_top(&new_state_key, stack_idx);
        fcs_put_card_in_freecell(new_state_key, ds, card);

        fcs_move_stack_non_seq_push(
            moves, FCS_MOVE_TYPE_STACK_TO_FREECELL, stack_idx, (stack_i)ds);

        sfs_check_state_end();
    }
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_freecell_card_to_parent)
{
    tests_define_accessors();
    MOVE_FUNCS__define_seqs_built_by();
    FC__STACKS__SET_PARAMS();
    for (int fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
    {
        const fcs_card card = fcs_freecell_card(state_key, fc);

        if (fcs_card_is_empty(card))
        {
            continue;
        }

        for (int ds = 0; ds < LOCAL_STACKS_NUM; ds++)
        {
            var_AUTO(dest_col, fcs_state_get_col(state_key, ds));
            const_AUTO(l, fcs_col_len(dest_col));
            if (!l)
            {
                continue;
            }
            const fcs_card dest_card = fcs_col_get_card(dest_col, l - 1);
            if (!fcs_is_parent_card(card, dest_card))
            {
                continue;
            }
            /* Let's move it */
            sfs_check_state_begin();

            my_copy_stack(ds);
            fcs_empty_freecell(new_state_key, fc);
            fcs_state_push(&new_state_key, (stack_i)ds, card);
            fcs_move_stack_non_seq_push(moves, FCS_MOVE_TYPE_FREECELL_TO_STACK,
                (stack_i)fc, (stack_i)ds);

            sfs_check_state_end();
        }
    }
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_freecell_card_to_empty_stack)
{
    tests_define_accessors();
    MOVE_FUNCS__define_empty_stacks_fill();
    FC__STACKS__SET_PARAMS();
    if (IS_FILLED_BY_NONE())
    {
        return;
    }
    if (soft_thread->num_vacant_stacks == 0)
    {
        return;
    }

    /* Find a vacant stack */
    const int ds = find_empty_stack(raw_state_raw, 0, LOCAL_STACKS_NUM);

    for (int fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
    {
        const fcs_card card = fcs_freecell_card(state_key, fc);

        if (fcs_card_is_empty(card) ||
            (IS_FILLED_BY_KINGS_ONLY() && (!fcs_card_is_king(card))))
        {
            continue;
        }

        sfs_check_state_begin();

        my_copy_stack(ds);
        fcs_empty_freecell(new_state_key, fc);
        fcs_state_push(&new_state_key, (stack_i)ds, card);
        fcs_move_stack_non_seq_push(
            moves, FCS_MOVE_TYPE_FREECELL_TO_STACK, (stack_i)fc, (stack_i)ds);

        sfs_check_state_end();
    }
}
#endif

#define CALC_FOUNDATION_TO_PUT_CARD_ON()                                       \
    calc_foundation_to_put_card_on(soft_thread, pass_new_state.key, card)

static inline int_fast32_t __attribute__((pure)) calc_foundation_to_put_card_on(
    const fcs_soft_thread *const soft_thread GCC_UNUSED,
    const fcs_state *const ptr_state, const fcs_card card)
{
    FCS_ON_NOT_FC_ONLY(const_AUTO(instance, fcs_st_instance(soft_thread)));
    MOVE_FUNCS__define_seqs_built_by();
    const fcs_card rank = fcs_card_rank(card);
    const fcs_card suit = fcs_card_suit(card);
    const int_fast32_t rank_min_1 = rank - 1;
    const int_fast32_t rank_min_2 = rank - 2;

    for (stack_i deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
    {
        const stack_i ret_val = ((deck << 2) | suit);
        if (fcs_foundation_value(*ptr_state, ret_val) == rank_min_1)
        {
/* Always put on the foundation if it is built-by-suit */
#ifndef FCS_FREECELL_ONLY
            if (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT)
            {
                return (int_fast32_t)ret_val;
            }
#endif
            const size_t lim_ = ((size_t)INSTANCE_DECKS_NUM << 2U);
            for (size_t f = 0; f < lim_; ++f)
            {
                if (fcs_foundation_value(*ptr_state, f) <
                    rank_min_2 -
                        (FCS__SEQS_ARE_BUILT_BY_RANK()
                                ? 0
                                : ((f & 0x1) == (fcs_card_suit(card) & 0x1))))
                {
                    goto next_iter;
                }
            }
            return (int_fast32_t)ret_val;
        next_iter:;
        }
    }

    return -1;
}

extern fcs_collectible_state *fc_solve_sfs_raymond_prune(
    fcs_soft_thread *const soft_thread, fcs_kv_state raw_state_raw)
{
#define EMPTY
    tests_define_accessors__generic(EMPTY);
    STACKS__SET_PARAMS();

    sfs_check_state_begin();
    bool cards_were_moved = false;
    bool num_cards_moved;
    do
    {
        num_cards_moved = false;
        for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
        {
            const_AUTO(col, fcs_state_get_col(new_state_key, stack_idx));
            const int cards_num = fcs_col_len(col);

            if (!cards_num)
            {
                continue;
            }
            /* Get the top card in the stack */
            const fcs_card card = fcs_col_get_card(col, cards_num - 1);

            const_AUTO(dest_foundation, CALC_FOUNDATION_TO_PUT_CARD_ON());
            if (dest_foundation < 0)
            {
                continue;
            }
            /* We can safely move it. */
            num_cards_moved = true;

            my_copy_stack(stack_idx);
            fcs_state_pop_col_top(&new_state_key, stack_idx);
            fcs_increment_foundation(new_state_key, dest_foundation);
            fcs_move_stack_non_seq_push(moves,
                FCS_MOVE_TYPE_STACK_TO_FOUNDATION, stack_idx,
                (stack_i)dest_foundation);
        }

#if MAX_NUM_FREECELLS > 0
        /* Now check the same for the free cells */
        for (int fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
        {
            const fcs_card card = fcs_freecell_card(new_state_key, fc);
            if (fcs_card_is_empty(card))
            {
                continue;
            }
            const_AUTO(dest_foundation, CALC_FOUNDATION_TO_PUT_CARD_ON());
            if (dest_foundation < 0)
            {
                continue;
            }
            num_cards_moved = true;

            fcs_empty_freecell(new_state_key, fc);
            fcs_increment_foundation(new_state_key, dest_foundation);
            fcs_move_stack_non_seq_push(moves,
                FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION, (stack_i)fc,
                (stack_i)dest_foundation);
        }
#endif
        if (num_cards_moved)
        {
            cards_were_moved = true;
        }
    } while (num_cards_moved);

    if (!cards_were_moved)
    {
        return NULL;
    }
    register const_AUTO(ptr_next_state,
        fc_solve_sfs_check_state_end(soft_thread,
            sfs_check_state_end__arg & pass_new_state FCS__pass_moves(moves)));
    /*
     * Set the GENERATED_BY_PRUNING flag uncondtionally. It won't
     * hurt if it's already there, and if it's a state that was
     * found by other means, we still shouldn't prune it, because
     * it is already "prune-perfect".
     * */
    FCS_S_VISITED(ptr_next_state) |= FCS_VISITED_GENERATED_BY_PRUNING;
    return ptr_next_state;
}
