/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// scans_impl.h - code implementing the various scans as inline functions.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>

#include "scans.h"

#ifdef FCS_SINGLE_HARD_THREAD
#define check_if_limits_exceeded__num()                                        \
    ((*instance_num_checked_states_ptr) == max_num_states)
#else
#define check_if_limits_exceeded__num()                                        \
    ((*hard_thread_num_checked_states_ptr) == max_num_states)
#endif

#ifdef FCS_DISABLE_NUM_STORED_STATES
#define check_if_limits_exceeded__num_states()
#else
#define check_if_limits_exceeded__num_states()                                 \
    || (instance->num_states_in_collection >=                                  \
           effective_max_num_states_in_collection)
#endif

/*
 * This macro checks if we need to terminate from running this soft
 * thread and return to the soft thread manager with an
 * FCS_STATE_SUSPEND_PROCESS
 * */
#define check_if_limits_exceeded()                                             \
    (check_if_limits_exceeded__num() check_if_limits_exceeded__num_states())

#define BEFS_MAX_DEPTH 20000

#ifdef FCS_FREECELL_ONLY
#define is_filled_by_any_card() TRUE
#else
#define is_filled_by_any_card()                                                \
    (INSTANCE_EMPTY_STACKS_FILL == FCS_ES_FILLED_BY_ANY_CARD)
#endif
static inline void fc_solve_initialize_befs_rater(
    fcs_soft_thread *const soft_thread, fcs_state_weighting *const weighting)
{
    const fc_solve_weighting_float *const befs_weights =
        weighting->befs_weights.weights;
    /* Normalize the BeFS Weights, so the sum of all of them would be 1. */
    fc_solve_weighting_float sum = 0;
    for (int i = 0; i < FCS_NUM_BEFS_WEIGHTS; i++)
    {
        sum += befs_weights[i];
    }
    if (sum < 1e-6)
    {
        sum = 1;
    }
    const_AUTO(factor, INT_MAX / sum);
#define W(idx) (befs_weights[idx] * factor)
    fcs_hard_thread *const hard_thread = soft_thread->hard_thread;
    fcs_instance *const instance = HT_INSTANCE(hard_thread);
    HARD__SET_GAME_PARAMS();

#ifndef FCS_FREECELL_ONLY
    const bool unlimited_sequence_move_var = INSTANCE_UNLIMITED_SEQUENCE_MOVE;
#define unlimited_sequence_move unlimited_sequence_move_var
#else
#define unlimited_sequence_move FALSE
#endif

    const fc_solve_weighting_float num_cards_out_factor =
        W(FCS_BEFS_WEIGHT_CARDS_OUT) / (LOCAL_DECKS_NUM * 52);

    fc_solve_weighting_float out_sum = 0.0;
    const_PTR(
        num_cards_out_lookup_table, weighting->num_cards_out_lookup_table);
    for (int i = 0; i <= 13; i++, out_sum += num_cards_out_factor)
    {
        num_cards_out_lookup_table[i] = out_sum;
    }

    weighting->max_sequence_move_factor =
        W(FCS_BEFS_WEIGHT_MAX_SEQUENCE_MOVE) /
        (is_filled_by_any_card()
                ? (unlimited_sequence_move
                          ? (LOCAL_FREECELLS_NUM + INSTANCE_STACKS_NUM)
                          : ((LOCAL_FREECELLS_NUM + 1)
                                << (INSTANCE_STACKS_NUM)))
                : (unlimited_sequence_move ? LOCAL_FREECELLS_NUM : 1));

    weighting->cards_under_sequences_factor =
        W(FCS_BEFS_WEIGHT_CARDS_UNDER_SEQUENCES) /
        instance->initial_cards_under_sequences_value;

    weighting->seqs_over_renegade_cards_factor =
        W(FCS_BEFS_WEIGHT_SEQS_OVER_RENEGADE_CARDS) /
        FCS_SEQS_OVER_RENEGADE_POWER(LOCAL_DECKS_NUM * (13 * 4));

    weighting->depth_factor = W(FCS_BEFS_WEIGHT_DEPTH) / BEFS_MAX_DEPTH;

    weighting->num_cards_not_on_parents_factor =
        W(FCS_BEFS_WEIGHT_NUM_CARDS_NOT_ON_PARENTS) / (LOCAL_DECKS_NUM * 52);

    weighting->should_go_over_stacks =
        (weighting->max_sequence_move_factor ||
            weighting->cards_under_sequences_factor ||
            weighting->seqs_over_renegade_cards_factor);
}
#undef unlimited_sequence_move
#undef W

typedef int fcs_depth;

static inline fcs_depth calc_depth(fcs_collectible_state *ptr_state)
{
#ifdef FCS_WITHOUT_DEPTH_FIELD
    register fcs_depth ret = 0;
    while ((ptr_state = FCS_S_PARENT(ptr_state)) != NULL)
    {
        ++ret;
    }
    return ret;
#else
    return (FCS_S_DEPTH(ptr_state));
#endif
}

#ifdef DEBUG
static inline __attribute__((format(printf, 1, 2))) void fcs_trace(
    const char *const format, ...)
{
    va_list my_va_list;
    va_start(my_va_list, format);

    if (getenv("FCS_TRACE"))
    {
        vfprintf(stdout, format, my_va_list);
        fflush(stdout);
    }
    va_end(my_va_list);
}
#endif

static inline fcs_game_limit count_num_vacant_freecells(
    const fcs_game_limit freecells_num GCC_UNUSED,
    const fcs_state *const state_ptr GCC_UNUSED)
{
#if MAX_NUM_FREECELLS > 0
    fcs_game_limit num_vacant_freecells = 0;
    for (int i = 0; i < freecells_num; i++)
    {
        if (fcs_freecell_is_empty(*state_ptr, i))
        {
            num_vacant_freecells++;
        }
    }

    return num_vacant_freecells;
#else
    return 0;
#endif
}

static inline pq_rating befs_rate_state(
    const fcs_soft_thread *const soft_thread,
    const fcs_state_weighting *const weighting, const fcs_state *const state,
    const int negated_depth)
{
    const_AUTO(instance, fcs_st_instance(soft_thread));
    FCS_ON_NOT_FC_ONLY(const int sequences_are_built_by =
                           GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance));
    HARD__SET_GAME_PARAMS();

#ifndef FCS_FREECELL_ONLY
    const bool unlimited_sequence_move_var = INSTANCE_UNLIMITED_SEQUENCE_MOVE;
#define unlimited_sequence_move unlimited_sequence_move_var
#else
#define unlimited_sequence_move FALSE
#endif

    fcs_seq_cards_power_type cards_under_sequences = 0;
    fcs_seq_cards_power_type seqs_over_renegade_cards = 0;

    fc_solve_weighting_float sum =
        (max(0, negated_depth) * weighting->depth_factor);
    const_PTR(
        num_cards_out_lookup_table, weighting->num_cards_out_lookup_table);
    if (num_cards_out_lookup_table[1])
    {
        const int num_foundations = (LOCAL_DECKS_NUM << 2);
        for (int found_idx = 0; found_idx < num_foundations; ++found_idx)
        {
            sum += num_cards_out_lookup_table[(
                int)(fcs_foundation_value((*state), found_idx))];
        }
    }

    fcs_game_limit num_vacant_stacks = 0;
    if (weighting->should_go_over_stacks)
    {
        for (int a = 0; a < LOCAL_STACKS_NUM; ++a)
        {
            const_AUTO(col, fcs_state_get_col(*state, a));
            const int cards_num = fcs_col_len(col);

            if (cards_num <= 1)
            {
                if (cards_num == 0)
                {
                    ++num_vacant_stacks;
                }
                continue;
            }

            const int c = update_col_cards_under_sequences(
#ifndef FCS_FREECELL_ONLY
                sequences_are_built_by,
#endif
                col, cards_num - 1);

            cards_under_sequences += FCS_SEQS_OVER_RENEGADE_POWER(c);
            if (c > 0)
            {
                seqs_over_renegade_cards +=
                    ((unlimited_sequence_move)
                            ? 1
                            : FCS_SEQS_OVER_RENEGADE_POWER(cards_num - c));
            }
        }

        const fcs_game_limit num_vacant_freecells =
            count_num_vacant_freecells(LOCAL_FREECELLS_NUM, state);
#define CALC_VACANCY_VAL()                                                     \
    (is_filled_by_any_card()                                                   \
            ? (unlimited_sequence_move                                         \
                      ? (num_vacant_freecells + num_vacant_stacks)             \
                      : ((num_vacant_freecells + 1) << num_vacant_stacks))     \
            : (unlimited_sequence_move ? (num_vacant_freecells) : 0))
        sum += ((CALC_VACANCY_VAL() * weighting->max_sequence_move_factor) +
                ((instance->initial_cards_under_sequences_value -
                     cards_under_sequences) *
                    weighting->cards_under_sequences_factor) +
                (seqs_over_renegade_cards *
                    weighting->seqs_over_renegade_cards_factor));
    }

    const fc_solve_weighting_float num_cards_not_on_parents_weight =
        weighting->num_cards_not_on_parents_factor;
    if (num_cards_not_on_parents_weight)
    {
        int num_cards_not_on_parents = (LOCAL_DECKS_NUM * 52);
        for (int stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
        {
            const_AUTO(col, fcs_state_get_col(*state, stack_idx));
            const uint_fast16_t col_len = fcs_col_len(col);
            for (uint_fast16_t h = 1; h < col_len; h++)
            {
                if (!fcs_is_parent_card(
                        fcs_col_get_card(col, h - 1), fcs_col_get_card(col, h)))
                {
                    num_cards_not_on_parents--;
                }
            }
        }
        sum += num_cards_not_on_parents * num_cards_not_on_parents_weight;
    }

#ifdef DEBUG
    fcs_trace(
        "BestFS(rate_state) - %s ; rating=%.40f .\n", "Before return", 0.1);
#endif
    return ((int)sum);
#undef CALC_VACANCY_VAL
}

#ifdef FCS_RCS_STATES

#define FCS_SCANS_the_state (state_key)
#define VERIFY_DERIVED_STATE()                                                 \
    {                                                                          \
    }
#define FCS_ASSIGN_STATE_KEY()                                                 \
    (state_key = (*(fc_solve_lookup_state_key_from_val(instance, PTR_STATE))))
#define PTR_STATE (pass.val)
#define DECLARE_STATE()                                                        \
    fcs_state state_key;                                                       \
    fcs_kv_state pass = {.key = &(state_key)}

#else

#define FCS_SCANS_the_state (PTR_STATE->s)
#define VERIFY_DERIVED_STATE() verify_state_sanity(&(single_derived_state->s))
#define FCS_ASSIGN_STATE_KEY()                                                 \
    (pass = (typeof(pass)){                                                    \
         .key = &FCS_SCANS_the_state, .val = &(PTR_STATE->info)})
#define PTR_STATE (ptr_state_raw)
#define DECLARE_STATE()                                                        \
    fcs_collectible_state *ptr_state_raw;                                      \
    fcs_kv_state pass
#endif

#define ASSIGN_ptr_state(my_value)                                             \
    if ((PTR_STATE = my_value))                                                \
    {                                                                          \
        FCS_ASSIGN_STATE_KEY();                                                \
    }

#if !defined(FCS_WITHOUT_DEPTH_FIELD) &&                                       \
    !defined(FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE)
/*
 * The calculate_real_depth() inline function traces the path of the state
 * up to the original state, and thus calculates its real depth.
 *
 * It then assigns the newly updated depth throughout the path.
 *
 * */

static inline void calculate_real_depth(
    const bool calc_real_depth, fcs_collectible_state *const ptr_state_orig)
{
    if (calc_real_depth)
    {
        int_fast32_t this_real_depth = -1;
        fcs_collectible_state *temp_state = ptr_state_orig;
        /* Count the number of states until the original state. */
        while (temp_state != NULL)
        {
            temp_state = FCS_S_PARENT(temp_state);
            ++this_real_depth;
        }
        temp_state = ptr_state_orig;
        /* Assign the new depth throughout the path */
        while (FCS_S_DEPTH(temp_state) != this_real_depth)
        {
            FCS_S_DEPTH(temp_state) = this_real_depth;
            --this_real_depth;
            temp_state = FCS_S_PARENT(temp_state);
        }
    }
}
#else
#define calculate_real_depth(calc_real_depth, ptr_state_orig)
#endif

/*
 * The mark_as_dead_end() inline function marks a state as a dead end, and
 * afterwards propagates this information to its parent and ancestor states.
 *
 * */

static inline void mark_as_dead_end__proto(
    fcs_collectible_state *const ptr_state_input)
{
    fcs_collectible_state *temp_state = (ptr_state_input);
    /* Mark as a dead end */
    FCS_S_VISITED(temp_state) |= FCS_VISITED_DEAD_END;
    temp_state = FCS_S_PARENT(temp_state);
    if (temp_state != NULL)
    {
        /* Decrease the refcount of the state */
        --(FCS_S_NUM_ACTIVE_CHILDREN(temp_state));
        while ((FCS_S_NUM_ACTIVE_CHILDREN(temp_state) == 0) &&
               (FCS_S_VISITED(temp_state) & FCS_VISITED_ALL_TESTS_DONE))
        {
            /* Mark as dead end */
            FCS_S_VISITED(temp_state) |= FCS_VISITED_DEAD_END;
            /* Go to its parent state */
            temp_state = FCS_S_PARENT(temp_state);
            if (temp_state == NULL)
            {
                break;
            }
            /* Decrease the refcount */
            (FCS_S_NUM_ACTIVE_CHILDREN(temp_state))--;
        }
    }
}

#ifdef FCS_HARD_CODE_SCANS_SYNERGY_AS_TRUE
#define MARK_AS_DEAD_END(state)                                                \
    {                                                                          \
        mark_as_dead_end__proto(state);                                        \
    }
#else
#define MARK_AS_DEAD_END(state)                                                \
    {                                                                          \
        if (scans_synergy)                                                     \
        {                                                                      \
            mark_as_dead_end__proto(state);                                    \
        }                                                                      \
    }
#endif

#ifdef FCS_SINGLE_HARD_THREAD
#define BUMP_NUM_CHECKED_STATES__HT()
#else
#define BUMP_NUM_CHECKED_STATES__HT() (*hard_thread_num_checked_states_ptr)++;
#endif

#define BUMP_NUM_CHECKED_STATES()                                              \
    {                                                                          \
        (*instance_num_checked_states_ptr)++;                                  \
        BUMP_NUM_CHECKED_STATES__HT()                                          \
    }

static inline fcs_game_limit count_num_vacant_stacks(
    const fcs_game_limit stacks_num, const fcs_state *const state_ptr)
{
    fcs_game_limit num_vacant_stacks = 0;

    for (int i = 0; i < stacks_num; i++)
    {
        if (fcs_state_col_is_empty(*state_ptr, i))
        {
            ++num_vacant_stacks;
        }
    }

    return num_vacant_stacks;
}

static inline bool fcs__should_state_be_pruned__state(
    const fcs_collectible_state *const ptr_state)
{
    return (!(FCS_S_VISITED(ptr_state) & FCS_VISITED_GENERATED_BY_PRUNING));
}

#ifdef FCS_ENABLE_PRUNE__R_TF__UNCOND
#define fcs__should_state_be_pruned(enable_pruning, ptr_state)                 \
    fcs__should_state_be_pruned__state(ptr_state)
#else
static inline bool fcs__should_state_be_pruned(
    const bool enable_pruning, const fcs_collectible_state *const ptr_state)
{
    return (enable_pruning && fcs__should_state_be_pruned__state(ptr_state));
}
#endif

#ifdef FCS_SINGLE_HARD_THREAD
#define CALC_HARD_THREAD_MAX_NUM_CHECKED_STATES__HELPER()                      \
    (instance->effective_max_num_checked_states)
#else
#define CALC_HARD_THREAD_MAX_NUM_CHECKED_STATES__HELPER()                      \
    (HT_FIELD(hard_thread, ht__num_checked_states) +                           \
        (instance->effective_max_num_checked_states -                          \
            (instance->i__num_checked_states)))
#endif
static inline fcs_int_limit_t calc_ht_max_num_states(
    const fcs_instance *const instance GCC_UNUSED,
    const fcs_hard_thread *const hard_thread)
{
    const_AUTO(a, HT_FIELD(hard_thread, ht__max_num_checked_states));
#ifdef FCS_WITHOUT_MAX_NUM_STATES
    return a;
#else
    const_AUTO(b, CALC_HARD_THREAD_MAX_NUM_CHECKED_STATES__HELPER());
    return min(a, b);
#endif
}

#undef state
#undef ptr_state_key
#undef unlimited_sequence_move

#ifdef __cplusplus
}
#endif
