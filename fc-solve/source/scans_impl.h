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
 * scans_impl.h - code implementing the various scans as inline functions.
 *
 */

#ifndef FC_SOLVE__SCANS_IMPL_H
#define FC_SOLVE__SCANS_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "config.h"

#include "inline.h"
#include "likely.h"
#include "bool.h"
#include "min_and_max.h"

static GCC_INLINE const fcs_bool_t check_num_states_in_collection(
    const fc_solve_instance_t * const instance
    )
{
    return (instance->active_num_states_in_collection >=
            instance->effective_trim_states_in_collection_from
            );
}

#ifdef FCS_SINGLE_HARD_THREAD
#define check_if_limits_exceeded__num() \
        ((*instance_num_checked_states_ptr) == hard_thread_max_num_checked_states)
#else
#define check_if_limits_exceeded__num() \
        ((*hard_thread_num_checked_states_ptr) == hard_thread_max_num_checked_states)
#endif

/*
 * This macro checks if we need to terminate from running this soft
 * thread and return to the soft thread manager with an
 * FCS_STATE_SUSPEND_PROCESS
 * */
#define check_if_limits_exceeded()                                    \
    (   \
        check_if_limits_exceeded__num() \
            || \
        (instance->num_states_in_collection >=   \
            effective_max_num_states_in_collection) \
    )

#define BEFS_MAX_DEPTH 20000

extern const double fc_solve_default_befs_weights[FCS_NUM_BEFS_WEIGHTS];

#ifdef FCS_FREECELL_ONLY
#define is_filled_by_any_card() TRUE
#else
#define is_filled_by_any_card() (INSTANCE_EMPTY_STACKS_FILL == FCS_ES_FILLED_BY_ANY_CARD)
#endif
static GCC_INLINE void fc_solve_initialize_befs_rater(
    fc_solve_soft_thread_t * const soft_thread,
    fc_solve_state_weighting_t * const weighting
)
{
    double * const befs_weights = weighting->befs_weights;
    double normalized_befs_weights[COUNT(weighting->befs_weights)];

    /* Normalize the BeFS Weights, so the sum of all of them would be 1. */
    double sum = 0;
    for (int i = 0 ; i < FCS_NUM_BEFS_WEIGHTS; i++)
    {
        if (befs_weights[i] < 0)
        {
            befs_weights[i] = fc_solve_default_befs_weights[i];
        }
        sum += befs_weights[i];
    }
    if (sum < 1e-6)
    {
        sum = 1;
    }
    for (int i=0 ; i < FCS_NUM_BEFS_WEIGHTS ; i++)
    {
        normalized_befs_weights[i] = ((befs_weights[i] /= sum) * INT_MAX);
    }
#ifndef HARD_CODED_NUM_STACKS
    fc_solve_hard_thread_t * const hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * const instance = HT_INSTANCE(hard_thread);
#endif

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif

#ifndef FCS_FREECELL_ONLY
    const fcs_bool_t bool_unlimited_sequence_move = INSTANCE_UNLIMITED_SEQUENCE_MOVE;
#define unlimited_sequence_move bool_unlimited_sequence_move
#else
#define unlimited_sequence_move FALSE
#endif

    const double num_cards_out_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_CARDS_OUT] / (LOCAL_DECKS_NUM*52);

    double out_sum = 0.0;
    typeof( weighting->num_cards_out_lookup_table[0] ) * const num_cards_out_lookup_table = weighting->num_cards_out_lookup_table;
    for (int i=0 ; i <= 13 ; i++, out_sum += num_cards_out_factor)
    {
        num_cards_out_lookup_table[i] = out_sum;
    }

    weighting->max_sequence_move_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_MAX_SEQUENCE_MOVE] /
        (is_filled_by_any_card()
         ? (unlimited_sequence_move
            ? (LOCAL_FREECELLS_NUM+INSTANCE_STACKS_NUM)
            : ((LOCAL_FREECELLS_NUM+1)<<(INSTANCE_STACKS_NUM))
         )
         :
           (unlimited_sequence_move
            ? LOCAL_FREECELLS_NUM
            : 1
           )
        );


    weighting->cards_under_sequences_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_CARDS_UNDER_SEQUENCES] / soft_thread->initial_cards_under_sequences_value;

    weighting->seqs_over_renegade_cards_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_SEQS_OVER_RENEGADE_CARDS] / FCS_SEQS_OVER_RENEGADE_POWER(LOCAL_DECKS_NUM*(13*4));

    weighting->depth_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_DEPTH] / BEFS_MAX_DEPTH;

    weighting->num_cards_not_on_parents_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_NUM_CARDS_NOT_ON_PARENTS] / (LOCAL_DECKS_NUM * 52);


    weighting->should_go_over_stacks =
    (
        weighting->max_sequence_move_factor
        || weighting->cards_under_sequences_factor
        || weighting->seqs_over_renegade_cards_factor
    );
}

#undef unlimited_sequence_move

static int compare_rating_with_index(const void * const void_a, const void * const void_b)
{
    const fcs_rating_with_index_t * const a = (const fcs_rating_with_index_t * const)void_a;
    const fcs_rating_with_index_t * const b = (const fcs_rating_with_index_t * const)void_b;

    return (
          (a->rating < b->rating) ? -1
        : (a->rating > b->rating) ? 1
        : (a->idx - b->idx)
    );
}

typedef int fcs_depth_t;

static GCC_INLINE fcs_depth_t calc_depth(fcs_collectible_state_t * ptr_state)
{
#ifdef FCS_WITHOUT_DEPTH_FIELD

    register fcs_depth_t ret = 0;
    while ((ptr_state = FCS_S_PARENT(ptr_state)) != NULL)
    {
        ret++;
    }
    return ret;

#else

    return (FCS_S_DEPTH(ptr_state));

#endif
}

#ifdef DEBUG

#define TRACE0(message) \
        { \
            if (getenv("FCS_TRACE")) \
            { \
            printf("BestFS(rate_state) - %s ; rating=%.40f .\n", \
                    message, \
                   0.1  \
                    );  \
            fflush(stdout); \
            } \
        }

#else

#define TRACE0(no_use) {}

#endif


static GCC_INLINE pq_rating_t befs_rate_state(
    const fc_solve_soft_thread_t * const soft_thread,
    const fc_solve_state_weighting_t * const weighting,
    const fcs_state_t * const state,
    const int negated_depth
)
{
#ifndef FCS_FREECELL_ONLY
    const fc_solve_instance_t * const instance = HT_INSTANCE(soft_thread->hard_thread);
    const int sequences_are_built_by =
        GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance)
        ;
#endif
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif

#ifndef FCS_FREECELL_ONLY
    const fcs_bool_t unlimited_sequence_move_var = INSTANCE_UNLIMITED_SEQUENCE_MOVE;
#define unlimited_sequence_move unlimited_sequence_move_var
#else
#define unlimited_sequence_move FALSE
#endif

    fc_solve_seq_cards_power_type_t cards_under_sequences = 0;
    fc_solve_seq_cards_power_type_t seqs_over_renegade_cards = 0;

    double sum = (max(0, negated_depth) * weighting->depth_factor);
    typeof( weighting->num_cards_out_lookup_table[0] ) * const num_cards_out_lookup_table = weighting->num_cards_out_lookup_table;
    if (num_cards_out_lookup_table[1])
    {
        const int num_founds = (LOCAL_DECKS_NUM<<2);
        for (int found_idx = 0 ; found_idx < num_founds ; found_idx++)
        {
            sum += num_cards_out_lookup_table[(int)(fcs_foundation_value((*state), found_idx))];
        }
    }

    fcs_game_limit_t num_vacant_stacks = 0;
    fcs_game_limit_t num_vacant_freecells = 0;
    if (weighting->should_go_over_stacks)
    {
        for (int a = 0 ; a < LOCAL_STACKS_NUM ; a++)
        {
            const fcs_const_cards_column_t col = fcs_state_get_col(*state, a);
            const int cards_num = fcs_col_len(col);

            if (cards_num <= 1)
            {
                if (cards_num == 0)
                {
                    num_vacant_stacks++;
                }
                continue;
            }

            const int c =
                update_col_cards_under_sequences(
#ifndef FCS_FREECELL_ONLY
                        sequences_are_built_by,
#endif
                        col,
                        cards_num-1
                        )
                ;

            cards_under_sequences += FCS_SEQS_OVER_RENEGADE_POWER(c);
            if (c > 0)
            {
                seqs_over_renegade_cards +=
                    ((unlimited_sequence_move) ?
                     1 :
                     FCS_SEQS_OVER_RENEGADE_POWER(cards_num-c)
                    );
            }
        }

        for (int freecell_idx = 0 ; freecell_idx < LOCAL_FREECELLS_NUM ; freecell_idx++)
        {
            if (fcs_freecell_is_empty((*state),freecell_idx))
            {
                num_vacant_freecells++;
            }
        }
#define CALC_VACANCY_VAL() \
    ( \
        is_filled_by_any_card() \
      ? \
        (unlimited_sequence_move \
         ? (num_vacant_freecells+num_vacant_stacks)  \
         : ((num_vacant_freecells+1)<<num_vacant_stacks) \
        ) \
      : (unlimited_sequence_move \
         ? (num_vacant_freecells) \
         : 0 \
        ) \
    )
        sum +=
        (
            (CALC_VACANCY_VAL() * weighting->max_sequence_move_factor)
            +
            (
                (soft_thread->initial_cards_under_sequences_value - cards_under_sequences)
                * weighting->cards_under_sequences_factor
            )
            +
            (seqs_over_renegade_cards * weighting->seqs_over_renegade_cards_factor)
        );
    }

    double num_cards_not_on_parents_weight = weighting->num_cards_not_on_parents_factor;
    if (num_cards_not_on_parents_weight)
    {
        int num_cards_not_on_parents = (LOCAL_DECKS_NUM*52);
        for (int stack_idx = 0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
        {
            const fcs_const_cards_column_t col =
                fcs_state_get_col(*state, stack_idx);

            const int col_len = fcs_col_len(col);
            fcs_card_t parent_card = fcs_col_get_card(col, 0);
            for (int h = 1 ; h < col_len ; h++)
            {
                const fcs_card_t child_card = fcs_col_get_card(col, h);

                if (! fcs_is_parent_card(parent_card, child_card))
                {
                    num_cards_not_on_parents--;
                }
                parent_card = child_card;
            }
        }
        sum += num_cards_not_on_parents * num_cards_not_on_parents_weight;
    }


    TRACE0("Before return");

    return ( (int)sum );
#undef CALC_VACANCY_VAL
}

#undef TRACE0

#define STATE_TO_PASS() (&(pass))

#ifdef FCS_RCS_STATES

#define FCS_SCANS_the_state (state_key)
#define VERIFY_DERIVED_STATE() {}
#define FCS_ASSIGN_STATE_KEY() (state_key = (*(fc_solve_lookup_state_key_from_val(instance, PTR_STATE))))
#define PTR_STATE (pass.val)
#define DECLARE_STATE() fcs_state_t state_key; fcs_kv_state_t pass = {.key = &(state_key)}
#define DECLARE_NEW_STATE() fcs_kv_state_t new_pass
#define FCS_SCANS_ptr_new_state (new_pass.val)

#else

#define FCS_SCANS_the_state (PTR_STATE->s)
#define VERIFY_DERIVED_STATE() verify_state_sanity(&(single_derived_state->s))
#define FCS_ASSIGN_STATE_KEY() { pass.key = &(FCS_SCANS_the_state); pass.val = &(PTR_STATE->info); }
#define PTR_STATE (ptr_state_raw)
#define DECLARE_STATE() fcs_collectible_state_t * ptr_state_raw; fcs_kv_state_t pass
#define DECLARE_NEW_STATE() fcs_collectible_state_t * FCS_SCANS_ptr_new_state; fcs_kv_state_t new_pass
#endif

#define ASSIGN_ptr_state(my_value) { if ((PTR_STATE = my_value)) { FCS_ASSIGN_STATE_KEY(); } }

#ifndef FCS_WITHOUT_DEPTH_FIELD
/*
 * The calculate_real_depth() inline function traces the path of the state up
 * to the original state, and thus calculates its real depth.
 *
 * It then assigns the newly updated depth throughout the path.
 *
 * */

static GCC_INLINE void calculate_real_depth(
    const fcs_bool_t calc_real_depth,
    fcs_collectible_state_t * const ptr_state_orig
)
{
    if (calc_real_depth)
    {
        int this_real_depth = 0;
        fcs_collectible_state_t * temp_state = ptr_state_orig;
        /* Count the number of states until the original state. */
        while(temp_state != NULL)
        {
            temp_state = FCS_S_PARENT(temp_state);
            this_real_depth++;
        }
        this_real_depth--;
        temp_state = (ptr_state_orig);
        /* Assign the new depth throughout the path */
        while (FCS_S_DEPTH(temp_state) != this_real_depth)
        {
            FCS_S_DEPTH(temp_state) = this_real_depth;
            this_real_depth--;
            temp_state = FCS_S_PARENT(temp_state);
        }
    }

    return;
}
#else
#define calculate_real_depth(calc_real_depth, ptr_state_orig) {}
#endif

#ifdef DEBUG
#define TRACE0(message) \
        { \
            if (getenv("FCS_TRACE")) \
            { \
            printf("%s. Depth=%ld ; the_soft_Depth=%ld ; Iters=%ld ; tests_list_index=%d ; test_index=%d ; current_state_index=%d ; num_states=%d\n", \
                    message, \
                    (long int)DFS_VAR(soft_thread, depth), \
                    (long int)(the_soft_dfs_info-DFS_VAR(soft_thread, soft_dfs_info)), \
                    (long int)(instance->i__num_checked_states), \
                    the_soft_dfs_info->tests_list_index, \
                    the_soft_dfs_info->test_index, \
                    the_soft_dfs_info->current_state_index, \
                    (derived_states_list ? derived_states_list->num_states : -1) \
                    );  \
            fflush(stdout); \
            } \
        }

#define VERIFY_STATE_SANITY() verify_state_sanity(&FCS_SCANS_the_state)

#define VERIFY_PTR_STATE_TRACE0(string) \
{ \
    TRACE0(string); \
    VERIFY_STATE_SANITY(); \
    VERIFY_SOFT_DFS_STACK(soft_thread); \
}


#define VERIFY_PTR_STATE_AND_DERIVED_TRACE0(string) \
{ \
    TRACE0(string); \
    VERIFY_STATE_SANITY(); \
    VERIFY_DERIVED_STATE(); \
    VERIFY_SOFT_DFS_STACK(soft_thread); \
}

#else

#define TRACE0(no_use) {}
#define VERIFY_PTR_STATE_TRACE0(no_use) {}
#define VERIFY_PTR_STATE_AND_DERIVED_TRACE0(no_use) {}

#endif

#ifdef DEBUG

static void verify_state_sanity(
        fcs_state_t * ptr_state
        )
{
#ifndef NDEBUG
    for (int i=0; i < 8 ; i++)
    {
        const int l = fcs_col_len(fcs_state_get_col(*(ptr_state), i));
        assert ((l >= 0) && (l <= 7+12));
    }
#endif

    return;
}

#ifdef DEBUG_VERIFY_SOFT_DFS_STACK
static void verify_soft_dfs_stack(
    fc_solve_soft_thread_t * soft_thread
    )
{
    for (int depth = 0 ; depth < DFS_VAR(soft_thread, depth) ; depth++)
    {
        fcs_soft_dfs_stack_item_t * soft_dfs_info;
        soft_dfs_info = &(DFS_VAR(soft_thread, soft_dfs_info)[depth]);
        int * const rand_indexes = soft_dfs_info->derived_states_random_indexes;

        const int num_states = soft_dfs_info->derived_states_list.num_states;

        for ( int i=soft_dfs_info->current_state_index ; i < num_states ; i++)
        {
            verify_state_sanity(soft_dfs_info->derived_states_list.states[rand_indexes[i]].state_ptr);
        }
    }

    return;
}
#define VERIFY_SOFT_DFS_STACK(soft_thread) verify_soft_dfs_stack(soft_thread)
#else
#define VERIFY_SOFT_DFS_STACK(soft_thread)
#endif

#endif
/*
 * The mark_as_dead_end() inline function marks a state as a dead end, and
 * afterwards propogates this information to its parent and ancestor states.
 *
 * */

static GCC_INLINE void mark_as_dead_end(
    const fcs_bool_t scans_synergy,
    fcs_collectible_state_t * const ptr_state_input
)
{
    if (scans_synergy)
    {
        fcs_collectible_state_t * temp_state = (ptr_state_input);
        /* Mark as a dead end */
        FCS_S_VISITED(temp_state)|= FCS_VISITED_DEAD_END;
        temp_state = FCS_S_PARENT(temp_state);
        if (temp_state != NULL)
        {
            /* Decrease the refcount of the state */
            (FCS_S_NUM_ACTIVE_CHILDREN(temp_state))--;
            while((FCS_S_NUM_ACTIVE_CHILDREN(temp_state) == 0) && (FCS_S_VISITED(temp_state) & FCS_VISITED_ALL_TESTS_DONE))
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

    return;
}

#ifdef FCS_SINGLE_HARD_THREAD
#define BUMP_NUM_CHECKED_STATES__HT()
#else
#define BUMP_NUM_CHECKED_STATES__HT() \
    (*hard_thread_num_checked_states_ptr)++;
#endif

#define BUMP_NUM_CHECKED_STATES() \
{       \
    (*instance_num_checked_states_ptr)++; \
    BUMP_NUM_CHECKED_STATES__HT() \
}

static GCC_INLINE fcs_game_limit_t count_num_vacant_freecells(
    const fcs_game_limit_t freecells_num,
    const fcs_state_t * const state_ptr
)
{
    fcs_game_limit_t num_vacant_freecells = 0;
    for(int i=0;i<freecells_num;i++)
    {
        if (fcs_freecell_is_empty(*state_ptr, i))
        {
            num_vacant_freecells++;
        }
    }

    return num_vacant_freecells;
}

static GCC_INLINE fcs_game_limit_t count_num_vacant_stacks(
    const fcs_game_limit_t stacks_num,
    const fcs_state_t * const state_ptr
)
{
    fcs_game_limit_t num_vacant_stacks = 0;

    for (int  i=0 ; i < stacks_num ; i++ )
    {
        if (fcs_col_len(fcs_state_get_col(*state_ptr, i)) == 0)
        {
            num_vacant_stacks++;
        }
    }

    return num_vacant_stacks;
}

static GCC_INLINE const fcs_bool_t fcs__should_state_be_pruned(const fcs_bool_t enable_pruning, const fcs_collectible_state_t * const ptr_state)
{
    return
    (
        enable_pruning
            &&
        (! (FCS_S_VISITED(ptr_state) &
            FCS_VISITED_GENERATED_BY_PRUNING
            )
        )
    );
}

static GCC_INLINE const fcs_bool_t fcs__is_state_a_dead_end( const fcs_collectible_state_t * const ptr_state)
{
    return (FCS_S_VISITED(ptr_state) & FCS_VISITED_DEAD_END);
}

static GCC_INLINE void free_states_handle_soft_dfs_soft_thread(
        fc_solve_soft_thread_t * const soft_thread
        )
{
    fcs_soft_dfs_stack_item_t * soft_dfs_info =
        DFS_VAR(soft_thread, soft_dfs_info);
    fcs_soft_dfs_stack_item_t * const end_soft_dfs_info =
        soft_dfs_info + DFS_VAR(soft_thread, depth);

    for(;soft_dfs_info < end_soft_dfs_info; soft_dfs_info++)
    {
        fcs_rating_with_index_t * rand_index_ptr, * dest_rand_index_ptr;
        fcs_derived_states_list_item_t * const states =
            soft_dfs_info->derived_states_list.states;
        /*
         * We start from current_state_index instead of current_state_index+1
         * because that is the next state to be checked - it is referenced
         * by current_state_index++ instead of ++current_state_index .
         * */
        dest_rand_index_ptr = rand_index_ptr =
            soft_dfs_info->derived_states_random_indexes
            + soft_dfs_info->current_state_index
            ;
        fcs_rating_with_index_t * const end_rand_index_ptr =
            soft_dfs_info->derived_states_random_indexes
            + soft_dfs_info->derived_states_list.num_states
            ;

        for( ; rand_index_ptr < end_rand_index_ptr ; rand_index_ptr++ )
        {
            if (! fcs__is_state_a_dead_end(states[rand_index_ptr->idx].state_ptr))
            {
                *(dest_rand_index_ptr++) = *(rand_index_ptr);
            }
        }
        soft_dfs_info->derived_states_list.num_states =
            dest_rand_index_ptr - soft_dfs_info->derived_states_random_indexes;
    }

    return;
}

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH))
static const fcs_bool_t free_states_should_delete(void * const key, void * const context)
{
    fc_solve_instance_t * const instance = (fc_solve_instance_t * const)context;
    fcs_collectible_state_t * const ptr_state = (fcs_collectible_state_t * const)key;

    if (fcs__is_state_a_dead_end(ptr_state))
    {
        FCS_S_NEXT(ptr_state) = instance->list_of_vacant_states;
        instance->list_of_vacant_states = ptr_state;

        instance->active_num_states_in_collection--;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif

static GCC_INLINE void free_states(fc_solve_instance_t * const instance)
{
#ifdef DEBUG
    printf("%s\n", "FREE_STATES HIT");
#endif
#if (! ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)))
    return;
#else
    {
    /* First of all, let's make sure the soft_threads will no longer
     * traverse to the freed states that are currently dead end.
     * */


    HT_LOOP_START()
    {
        ST_LOOP_START()
        {
            if (soft_thread->super_method_type == FCS_SUPER_METHOD_DFS)
            {
                free_states_handle_soft_dfs_soft_thread(soft_thread);
            }
            else if (soft_thread->method == FCS_METHOD_A_STAR)
            {
                PQUEUE new_pq;
                fc_solve_pq_init(
                    &(new_pq),
                    1024
                );

                pq_element_t * const Elements = BEFS_VAR(soft_thread, pqueue).Elements;
                pq_element_t * const end_element = Elements + BEFS_VAR(soft_thread, pqueue).CurrentSize;

                pq_element_t * next_element = BEFS_VAR(soft_thread, pqueue).Elements + PQ_FIRST_ENTRY;

                for (; next_element <= end_element ; next_element++)
                {
                    if (! fcs__is_state_a_dead_end((*next_element).val))
                    {
                        fc_solve_pq_push(
                            &new_pq,
                            (*next_element).val,
                            (*next_element).rating
                        );
                    }
                }

                fc_solve_PQueueFree(
                    &(BEFS_VAR(soft_thread, pqueue))
                );

                BEFS_VAR(soft_thread, pqueue) = new_pq;
            }
            /* TODO : Implement for the BrFS/Optimize scans. */
        }
    }

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    /* Now let's recycle the states. */
    fc_solve_hash_foreach(
        &(instance->hash),
        free_states_should_delete,
        ((void *)instance)
    );
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)
    /* Now let's recycle the states. */
    fc_solve_states_google_hash_foreach(
        instance->hash,
        free_states_should_delete,
        ((void *)instance)
    );
#endif
    }
#endif
}
/*
 * fc_solve_soft_dfs_do_solve() is the event loop of the
 * Random-DFS scan. DFS which is recursive in nature is handled here
 * without procedural recursion by using some dedicated stacks for
 * the traversal.
 */
static GCC_INLINE int fc_solve_soft_dfs_do_solve(
    fc_solve_soft_thread_t * const soft_thread
    )
{
    fc_solve_hard_thread_t * const hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * const instance = HT_INSTANCE(hard_thread);

    int by_depth_max_depth, by_depth_min_depth;

#ifndef FCS_WITHOUT_DEPTH_FIELD
    const fcs_runtime_flags_t calc_real_depth = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_CALC_REAL_DEPTH);
#endif
    const fcs_runtime_flags_t scans_synergy = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY);

    const fcs_runtime_flags_t is_a_complete_scan = STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN);
    const int soft_thread_id = soft_thread->id;
    const fcs_tests_list_of_lists * the_tests_list_ptr;
    fcs_tests_group_type_t local_shuffling_type = FCS_NO_SHUFFLING;
    fcs_int_limit_t hard_thread_max_num_checked_states;
    const typeof(instance->effective_max_num_states_in_collection) effective_max_num_states_in_collection = instance->effective_max_num_states_in_collection;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

#define DEPTH() (*depth_ptr)
    int * const depth_ptr = &(DFS_VAR(soft_thread, depth));

    fcs_soft_dfs_stack_item_t * the_soft_dfs_info = &(DFS_VAR(soft_thread, soft_dfs_info)[DEPTH()]);

    int dfs_max_depth = DFS_VAR(soft_thread, dfs_max_depth);
    fcs_bool_t enable_pruning = soft_thread->enable_pruning;

    DECLARE_STATE();
    ASSIGN_ptr_state (the_soft_dfs_info->state);
    fcs_derived_states_list_t * derived_states_list = &(the_soft_dfs_info->derived_states_list);

    fcs_rand_t * const rand_gen = &(DFS_VAR(soft_thread, rand_gen));

    calculate_real_depth(calc_real_depth, PTR_STATE);

    fcs_tests_by_depth_unit_t * by_depth_units = DFS_VAR(soft_thread, tests_by_depth_array).by_depth_units;

#define THE_TESTS_LIST (*the_tests_list_ptr)
    TRACE0("Before depth loop");

#define GET_DEPTH(ptr) ((ptr)->max_depth)

#define RECALC_BY_DEPTH_LIMITS() \
    { \
        by_depth_max_depth = GET_DEPTH(curr_by_depth_unit); \
        by_depth_min_depth = (curr_by_depth_unit == by_depth_units) ? 0 : GET_DEPTH(curr_by_depth_unit-1); \
        the_tests_list_ptr = &(curr_by_depth_unit->tests); \
    }


    fcs_int_limit_t * const instance_num_checked_states_ptr = &(instance->i__num_checked_states);
#ifndef FCS_SINGLE_HARD_THREAD
    fcs_int_limit_t * const hard_thread_num_checked_states_ptr
        = &(HT_FIELD( hard_thread, ht__num_checked_states));
#endif

#ifdef FCS_SINGLE_HARD_THREAD
#define CALC_HARD_THREAD_MAX_NUM_CHECKED_STATES__HELPER() \
    (instance->effective_max_num_checked_states)
#else
#define CALC_HARD_THREAD_MAX_NUM_CHECKED_STATES__HELPER() \
    ( HT_FIELD(hard_thread, ht__num_checked_states)       \
      + (instance->effective_max_num_checked_states - *(instance_num_checked_states_ptr)) )
#endif

#define CALC_HARD_THREAD_MAX_NUM_CHECKED_STATES() \
    hard_thread_max_num_checked_states = HT_FIELD(hard_thread, ht__max_num_checked_states); \
                \
    {           \
        const fcs_int_limit_t lim = CALC_HARD_THREAD_MAX_NUM_CHECKED_STATES__HELPER(); \
        \
        hard_thread_max_num_checked_states = min(hard_thread_max_num_checked_states, lim); \
    }

    CALC_HARD_THREAD_MAX_NUM_CHECKED_STATES();

    const fcs_instance_debug_iter_output_func_t debug_iter_output_func = instance->debug_iter_output_func;
    const fcs_instance_debug_iter_output_context_t debug_iter_output_context = instance->debug_iter_output_context;

    fcs_tests_by_depth_unit_t * curr_by_depth_unit;
    {
        for (
            curr_by_depth_unit = by_depth_units
                ;
            (
                DEPTH()
                >= GET_DEPTH(curr_by_depth_unit)
            )
                ;
            curr_by_depth_unit++
            )
        {
        }
        RECALC_BY_DEPTH_LIMITS();
    }

    /*
        The main loop.
        We exit out of it when DEPTH() is decremented below zero.
    */
    while (1)
    {
        /*
            Increase the "maximal" depth if it is about to be exceeded.
        */
        if (unlikely(DEPTH()+1 >= dfs_max_depth))
        {
            fc_solve_increase_dfs_max_depth(soft_thread);

            /* Because the address of DFS_VAR(soft_thread, soft_dfs_info) may
             * be changed
             * */
            the_soft_dfs_info = &(DFS_VAR(soft_thread, soft_dfs_info)[DEPTH()]);
            dfs_max_depth = DFS_VAR(soft_thread, dfs_max_depth);
            /* This too has to be re-synced */
            derived_states_list = &(the_soft_dfs_info->derived_states_list);
        }

        TRACE0("Before current_state_index check");
        /* All the resultant states in the last test conducted were covered */
        if (the_soft_dfs_info->current_state_index ==
            derived_states_list->num_states
           )
        {
            /* Check if we already tried all the tests here. */
            if (the_soft_dfs_info->tests_list_index == THE_TESTS_LIST.num_lists)
            {
                /* Backtrack to the previous depth. */

                if (is_a_complete_scan)
                {
                    FCS_S_VISITED(PTR_STATE) |= FCS_VISITED_ALL_TESTS_DONE;
                    mark_as_dead_end (scans_synergy, PTR_STATE);
                }

                free(the_soft_dfs_info->positions_by_rank);
                if (unlikely(--DEPTH() < 0))
                {
                    break;
                }
                else
                {
                    the_soft_dfs_info--;
                    derived_states_list = &(the_soft_dfs_info->derived_states_list);

                    ASSIGN_ptr_state(the_soft_dfs_info->state);

                    VERIFY_PTR_STATE_TRACE0("Verify Foo");

                    soft_thread->num_vacant_freecells = the_soft_dfs_info->num_vacant_freecells;
                    soft_thread->num_vacant_stacks = the_soft_dfs_info->num_vacant_stacks;

                    if (unlikely(DEPTH() < by_depth_min_depth))
                    {
                        curr_by_depth_unit--;
                        RECALC_BY_DEPTH_LIMITS();
                    }
                }

                continue; /* Just to make sure depth is not -1 now */
            }

            derived_states_list->num_states = 0;

            TRACE0("Before iter_handler");
            /* If this is the first test, then count the number of unoccupied
               freecells and stacks and check if we are done. */
            if (   (the_soft_dfs_info->test_index == 0)
                && (the_soft_dfs_info->tests_list_index == 0)
               )
            {
                fcs_game_limit_t num_vacant_stacks, num_vacant_freecells;

                TRACE0("In iter_handler");

                if (debug_iter_output_func)
                {
                    debug_iter_output_func(
                        debug_iter_output_context,
                        *(instance_num_checked_states_ptr),
                        DEPTH(),
                        (void*)instance,
                        STATE_TO_PASS(),
#ifdef FCS_WITHOUT_VISITED_ITER
                        0
#else
                        ((DEPTH() == 0) ?
                            0 :
                            FCS_S_VISITED_ITER(DFS_VAR(soft_thread, soft_dfs_info)[DEPTH()-1].state)
                        )
#endif
                        );
                }

                num_vacant_freecells =
                    count_num_vacant_freecells(LOCAL_FREECELLS_NUM, &FCS_SCANS_the_state);

                num_vacant_stacks =
                    count_num_vacant_stacks(LOCAL_STACKS_NUM, &FCS_SCANS_the_state);

                /* Check if we have reached the empty state */
                if (unlikely((num_vacant_stacks == LOCAL_STACKS_NUM) &&
                    (num_vacant_freecells  == LOCAL_FREECELLS_NUM)))
                {
                    instance->final_state = PTR_STATE;

                    BUMP_NUM_CHECKED_STATES();

                    TRACE0("Returning FCS_STATE_WAS_SOLVED");
                    return FCS_STATE_WAS_SOLVED;
                }
                /*
                    Cache num_vacant_freecells and num_vacant_stacks in their
                    appropriate stacks, so they won't be calculated over and over
                    again.
                  */
                soft_thread->num_vacant_freecells =
                    the_soft_dfs_info->num_vacant_freecells =
                    num_vacant_freecells;
                soft_thread->num_vacant_stacks =
                    the_soft_dfs_info->num_vacant_stacks =
                    num_vacant_stacks;

                /* Perform the pruning. */
                if (fcs__should_state_be_pruned(enable_pruning, PTR_STATE))
                {
                    fcs_collectible_state_t * derived;

                    if (fc_solve_sfs_raymond_prune(
                        soft_thread,
                        STATE_TO_PASS(),
                        &derived
                        ) == PRUNE_RET_FOLLOW_STATE
                    )
                    {
                        the_soft_dfs_info->tests_list_index =
                            THE_TESTS_LIST.num_lists;
                        fc_solve_derived_states_list_add_state(
                            derived_states_list,
                            derived,
                            0
                        );
                        if (the_soft_dfs_info->derived_states_random_indexes_max_size < 1)
                        {
                            the_soft_dfs_info->derived_states_random_indexes_max_size = 1;
                    the_soft_dfs_info->derived_states_random_indexes =
                        SREALLOC(
                            the_soft_dfs_info->derived_states_random_indexes,
                            the_soft_dfs_info->derived_states_random_indexes_max_size
                            );
                        }

                        the_soft_dfs_info->derived_states_random_indexes[0].idx = 0;
                    }
                }
            }

            TRACE0("After iter_handler");

            const int orig_tests_list_index =
                the_soft_dfs_info->tests_list_index;

            const fc_solve_state_weighting_t * const weighting =
                &(THE_TESTS_LIST.lists[
                  orig_tests_list_index
                  ].weighting);

            if (the_soft_dfs_info->tests_list_index < THE_TESTS_LIST.num_lists)
            {
                /* Always do the first test */
                local_shuffling_type = THE_TESTS_LIST.lists[
                    the_soft_dfs_info->tests_list_index
                    ].shuffling_type;

            do
            {
                VERIFY_PTR_STATE_TRACE0("Verify Bar");

                THE_TESTS_LIST.lists[
                    the_soft_dfs_info->tests_list_index
                    ].tests[the_soft_dfs_info->test_index]
                    (
                        soft_thread,
                        STATE_TO_PASS(),
                        derived_states_list
                    );

                VERIFY_PTR_STATE_TRACE0("Verify Glanko");

                /* Move the counter to the next test */
                if ((++the_soft_dfs_info->test_index) ==
                        THE_TESTS_LIST.lists[
                            the_soft_dfs_info->tests_list_index
                        ].num_tests
                   )
                {
                    the_soft_dfs_info->tests_list_index++;
                    the_soft_dfs_info->test_index = 0;
                    break;
                }
            } while (local_shuffling_type != FCS_NO_SHUFFLING);
            }

            {
                const int num_states = derived_states_list->num_states;

                if (num_states >
                        the_soft_dfs_info->derived_states_random_indexes_max_size)
                {
                    the_soft_dfs_info->derived_states_random_indexes_max_size =
                        num_states;
                    the_soft_dfs_info->derived_states_random_indexes =
                        SREALLOC(
                            the_soft_dfs_info->derived_states_random_indexes,
                            the_soft_dfs_info->derived_states_random_indexes_max_size
                            );
                }
                fcs_rating_with_index_t * const rand_array = the_soft_dfs_info->derived_states_random_indexes;

                VERIFY_PTR_STATE_TRACE0("Verify Panter");

                for (int i=0 ; i < num_states ; i++)
                {
                    rand_array[i].idx = i;
                }
                /* If we just conducted the tests for a random group -
                 * randomize. Else - keep those indexes as the unity vector.
                 *
                 * Also, do not randomize if this is a pure soft-DFS scan.
                 *
                 * Also, do not randomize/sort if there's only one derived
                 * state or less, because in that case, there is nothing
                 * to reorder.
                 * */
                if (num_states > 1)
                {
                    switch (local_shuffling_type)
                    {
                        case FCS_RAND:
                            {
                                for (int i = num_states-1; i > 0; i--)
                                {
                                    const typeof(i) j =
                                        (
                                            fc_solve_rand_get_random_number(
                                                rand_gen
                                                )
                                            % (i+1)
                                        );

                                    const typeof(rand_array[i]) swap_save = rand_array[i];
                                    rand_array[i] = rand_array[j];
                                    rand_array[j] = swap_save;
                                }
                            }
                            break;

                        case FCS_WEIGHTING:
                            {
                                if (orig_tests_list_index < THE_TESTS_LIST.num_lists)
                                {
                                    fcs_derived_states_list_item_t * derived_states =
                                        derived_states_list->states;
                                    /* TODO : avoid excessive mallocing. */
                                    for (int i = 0 ; i < num_states ; i++)
                                    {
                                        rand_array[i].rating = befs_rate_state(
                                            soft_thread,
                                            weighting,
#ifdef FCS_RCS_STATES
                                            fc_solve_lookup_state_key_from_val(
                                                instance,
                                                derived_states[rand_array[i].idx].state_ptr
                                            ),
#else
                                            &(derived_states[rand_array[i].idx].state_ptr->s),
#endif
                                            BEFS_MAX_DEPTH - calc_depth(derived_states[rand_array[i].idx].state_ptr)
                                            );
                                    }

                                    qsort(
                                        rand_array,
                                        num_states,
                                        sizeof(rand_array[0]),
                                        compare_rating_with_index
                                        );
                                }
                            }
                            break;

                        case FCS_NO_SHUFFLING:
                            break;
                    }
                }
            }

            VERIFY_PTR_STATE_TRACE0("Verify Rondora");

            /* We just performed a test, so the index of the first state that
               ought to be checked in this depth is 0.
               */
            the_soft_dfs_info->current_state_index = 0;
        }

        {
            const int num_states = derived_states_list->num_states;
            fcs_derived_states_list_item_t * const derived_states =
                derived_states_list->states;
            const fcs_rating_with_index_t * rand_int_ptr =
                the_soft_dfs_info->derived_states_random_indexes +
                the_soft_dfs_info->current_state_index
                ;
            VERIFY_PTR_STATE_TRACE0("Verify Klondike");

            while (the_soft_dfs_info->current_state_index <
                   num_states)
            {
                fcs_collectible_state_t * const single_derived_state = derived_states[
                    (*(rand_int_ptr++)).idx
                ].state_ptr;

                the_soft_dfs_info->current_state_index++;
                VERIFY_PTR_STATE_AND_DERIVED_TRACE0("Verify Seahaven");

                if (
                    (! (FCS_S_VISITED(single_derived_state) &
                        FCS_VISITED_DEAD_END)
                    ) &&
                    (! is_scan_visited(
                        single_derived_state,
                        soft_thread_id)
                    )
                   )
                {
                    BUMP_NUM_CHECKED_STATES();

                    VERIFY_PTR_STATE_AND_DERIVED_TRACE0("Verify Gypsy");

                    set_scan_visited(
                        single_derived_state,
                        soft_thread_id
                    );

#ifndef FCS_WITHOUT_VISITED_ITER
                    FCS_S_VISITED_ITER(single_derived_state) = instance->i__num_checked_states;
#endif

                    VERIFY_PTR_STATE_AND_DERIVED_TRACE0("Verify Golf");

                    /*
                        I'm using current_state_indexes[depth]-1 because we already
                        increased it by one, so now it refers to the next state.
                    */
                    if (unlikely(++DEPTH() >= by_depth_max_depth))
                    {
                        curr_by_depth_unit++;
                        RECALC_BY_DEPTH_LIMITS();
                    }
                    the_soft_dfs_info++;

                    ASSIGN_ptr_state(single_derived_state);
                    the_soft_dfs_info->state = PTR_STATE;

                    VERIFY_PTR_STATE_AND_DERIVED_TRACE0("Verify Zap");

                    the_soft_dfs_info->tests_list_index = 0;
                    the_soft_dfs_info->test_index = 0;
                    the_soft_dfs_info->current_state_index = 0;
                    the_soft_dfs_info->positions_by_rank = NULL;
                    derived_states_list = &(the_soft_dfs_info->derived_states_list);
                    derived_states_list->num_states = 0;

                    calculate_real_depth(calc_real_depth, PTR_STATE);

                    if (check_num_states_in_collection(instance))
                    {
                        VERIFY_PTR_STATE_TRACE0("Verify Bakers_Game");

                        free_states(instance);

                        VERIFY_PTR_STATE_TRACE0("Verify Penguin");
                    }

                    if (check_if_limits_exceeded())
                    {
                        TRACE0("Returning FCS_STATE_SUSPEND_PROCESS (inside current_state_index)");
                        return FCS_STATE_SUSPEND_PROCESS;
                    }

                    break;
                }
            }
        }
    }

    /*
     * We need to bump the number of iterations so it will be ready with
     * a fresh iterations number for the next scan that takes place.
     * */
    BUMP_NUM_CHECKED_STATES();

    DEPTH() = -1;

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


#undef state


#undef pass
#undef ptr_state_key

#ifdef FCS_FREECELL_ONLY
#undef unlimited_sequence_move
#endif


#undef TRACE0

#ifndef FCS_DISABLE_PATSOLVE
/*
 * fc_solve_patsolve_do_solve() is the event loop of the
 * Patsolve scan.
 */
static GCC_INLINE int fc_solve_patsolve_do_solve(
    fc_solve_soft_thread_t * const soft_thread
    )
{
    fc_solve_hard_thread_t * const hard_thread = soft_thread->hard_thread;
    struct fc_solve__patsolve_thread_struct * const pats_scan
        = soft_thread->pats_scan;

    const typeof (pats_scan->num_checked_states) start_from = pats_scan->num_checked_states;


    pats_scan->max_num_checked_states = start_from + (HT_FIELD(hard_thread, ht__max_num_checked_states) - NUM_CHECKED_STATES);

    pats_scan->status = FCS_PATS__NOSOL;

    fc_solve_pats__do_it(pats_scan);

    {
        const typeof(start_from) after_scan_delta = pats_scan->num_checked_states - start_from;
#ifndef FCS_SINGLE_HARD_THREAD
        HT_FIELD(hard_thread, ht__num_checked_states) += after_scan_delta;
#endif
        HT_INSTANCE(hard_thread)->i__num_checked_states += after_scan_delta;
    }

    const typeof(pats_scan->status) status = pats_scan->status;
    return
    (
        (status == FCS_PATS__WIN) ? FCS_STATE_WAS_SOLVED
        : (status == FCS_PATS__NOSOL) ? FCS_STATE_IS_NOT_SOLVEABLE
        : FCS_STATE_SUSPEND_PROCESS
    );
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__SCANS_IMPL_H */
