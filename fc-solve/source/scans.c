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
 * scans.c - The code that relates to the various scans.
 * Currently Hard DFS, Soft-DFS, Random-DFS, BeFS and BFS are implemented.
 *
 */

#define BUILDING_DLL 1

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "config.h"

#include "state.h"
#include "card.h"
#include "scans.h"
#include "alloc.h"
#include "check_and_add_state.h"
#include "move_stack_compact_alloc.h"
#include "freecell.h"
#include "simpsim.h"
#include "move_funcs_maps.h"

#include "check_limits.h"
#include "inline.h"
#include "likely.h"
#include "bool.h"

#if 0
#define DEBUG 1
#endif

#define SOFT_DFS_DEPTH_GROW_BY 16
void fc_solve_increase_dfs_max_depth(
    fc_solve_soft_thread_t * soft_thread
    )
{
    int new_dfs_max_depth = soft_thread->method_specific.soft_dfs.dfs_max_depth + SOFT_DFS_DEPTH_GROW_BY;
    fcs_soft_dfs_stack_item_t * soft_dfs_info, * end_soft_dfs_info;

    soft_thread->method_specific.soft_dfs.soft_dfs_info = realloc(
        soft_thread->method_specific.soft_dfs.soft_dfs_info,
        sizeof(soft_thread->method_specific.soft_dfs.soft_dfs_info[0])*new_dfs_max_depth
        );

    soft_dfs_info = soft_thread->method_specific.soft_dfs.soft_dfs_info +
        soft_thread->method_specific.soft_dfs.dfs_max_depth;

    end_soft_dfs_info = soft_dfs_info + SOFT_DFS_DEPTH_GROW_BY;
    for(; soft_dfs_info < end_soft_dfs_info ; soft_dfs_info++)
    {
        soft_dfs_info->state = NULL;
        soft_dfs_info->tests_list_index = 0;
        soft_dfs_info->test_index = 0;
        soft_dfs_info->current_state_index = 0;
        soft_dfs_info->derived_states_list.num_states = 0;
        soft_dfs_info->derived_states_list.states = NULL;
        soft_dfs_info->derived_states_random_indexes = NULL;
        soft_dfs_info->derived_states_random_indexes_max_size = 0;
        soft_dfs_info->positions_by_rank = NULL;
    }

    soft_thread->method_specific.soft_dfs.dfs_max_depth = new_dfs_max_depth;
}

#define FCS_IS_STATE_DEAD_END(ptr_state) \
    (FCS_S_VISITED(ptr_state) & FCS_VISITED_DEAD_END)

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH))
static fcs_bool_t free_states_should_delete(void * key, void * context)
{
    fc_solve_instance_t * instance = (fc_solve_instance_t *)context;
    fcs_collectible_state_t * ptr_state = (fcs_collectible_state_t *)key;

    if (FCS_IS_STATE_DEAD_END(ptr_state))
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

static GCC_INLINE void free_states_handle_soft_dfs_soft_thread(
        fc_solve_soft_thread_t * soft_thread
        )
{
    fcs_soft_dfs_stack_item_t * soft_dfs_info,
                              * end_soft_dfs_info;

    soft_dfs_info = soft_thread->method_specific.soft_dfs.soft_dfs_info;

    end_soft_dfs_info =
        soft_dfs_info + soft_thread->method_specific.soft_dfs.depth;

    for(;soft_dfs_info < end_soft_dfs_info; soft_dfs_info++)
    {
        int * rand_index_ptr, * dest_rand_index_ptr, * end_rand_index_ptr;

        /*
         * We start from current_state_index instead of current_state_index+1
         * because that is the next state to be checked - it is referenced
         * by current_state_index++ instead of ++current_state_index .
         * */
        dest_rand_index_ptr = rand_index_ptr =
            soft_dfs_info->derived_states_random_indexes
            + soft_dfs_info->current_state_index
            ;
        end_rand_index_ptr = soft_dfs_info->derived_states_random_indexes + soft_dfs_info->derived_states_list.num_states;

        for( ; rand_index_ptr < end_rand_index_ptr ; rand_index_ptr++ )
        {
            if (! FCS_IS_STATE_DEAD_END(soft_dfs_info->derived_states_list.states[*(rand_index_ptr)].state_ptr))
            {
                *(dest_rand_index_ptr++) = *(rand_index_ptr);
            }
        }
        soft_dfs_info->derived_states_list.num_states =
            dest_rand_index_ptr - soft_dfs_info->derived_states_random_indexes;
    }

    return;
}

#ifdef DEBUG

#include <assert.h>

static void verify_state_sanity(
        fcs_collectible_state_t * ptr_state
        )
{
    int i;

    for (i=0; i < 8 ; i++)
    {
        int l = fcs_col_len(fcs_state_get_col(ptr_state->s, i));
        assert ((l >= 0) && (l <= 7+12));
    }

    return;
}

#ifdef DEBUG_VERIFY_SOFT_DFS_STACK
static void verify_soft_dfs_stack(
    fc_solve_soft_thread_t * soft_thread
    )
{
    int depth = 0, i, num_states;
    for (depth = 0 ; depth < soft_thread->method_specific.soft_dfs.depth ; depth++)
    {
        fcs_soft_dfs_stack_item_t * soft_dfs_info;
        int * rand_indexes;
        
        soft_dfs_info = &(soft_thread->method_specific.soft_dfs.soft_dfs_info[depth]);
        rand_indexes = soft_dfs_info->derived_states_random_indexes;

        num_states = soft_dfs_info->derived_states_list.num_states;
        
        for ( i=soft_dfs_info->current_state_index ; i < num_states ; i++)
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

static void free_states(fc_solve_instance_t * instance)
{
#ifdef DEBUG
    printf("%s\n", "FREE_STATES HIT");
#endif
#if (! ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)))
    return;
#else
    /* First of all, let's make sure the soft_threads will no longer
     * traverse to the freed states that are currently dead end.
     * */

    HT_LOOP_DECLARE_VARS();

    HT_LOOP_START()
    {
        ST_LOOP_DECLARE_VARS();

        ST_LOOP_START()
        {
            switch (soft_thread->method)
            {
                case FCS_METHOD_SOFT_DFS:
                case FCS_METHOD_HARD_DFS:
                case FCS_METHOD_RANDOM_DFS:
                {
                    free_states_handle_soft_dfs_soft_thread(soft_thread);
                }
                break;

                case FCS_METHOD_A_STAR:
                {
                    PQUEUE new_pq;
                    int i, CurrentSize;
                    pq_element_t * Elements;

                    fc_solve_PQueueInitialise(
                        &(new_pq),
                        1024
                    );

                    CurrentSize = soft_thread->method_specific.befs.meth.befs.pqueue.CurrentSize;
                    Elements = soft_thread->method_specific.befs.meth.befs.pqueue.Elements;

                    for (i = PQ_FIRST_ENTRY ; i <= CurrentSize ; i++)
                    {
                        if (! FCS_IS_STATE_DEAD_END(Elements[i].val))
                        {
                            fc_solve_PQueuePush(
                                &new_pq,
                                Elements[i].val,
                                Elements[i].rating
                            );
                        }
                    }

                    fc_solve_PQueueFree(
                        &(soft_thread->method_specific.befs.meth.befs.pqueue)
                    );

                    soft_thread->method_specific.befs.meth.befs.pqueue = new_pq;
                }
                break;
                /* TODO : Implement for the BrFS/Optimize scans. */
            }
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
#endif
}

/*
    fc_solve_soft_dfs_do_solve is the event loop of the
    Random-DFS scan. DFS which is recursive in nature is handled here
    without procedural recursion
    by using some dedicated stacks for the traversal.
  */
#ifdef FCS_RCS_STATES
#define the_state (state_key)
#else
#define the_state (ptr_state->s)
#endif

#ifdef DEBUG
#define TRACE0(message) \
        { \
            if (getenv("FCS_TRACE")) \
            { \
            printf("%s. Depth=%d ; the_soft_Depth=%ld ; Iters=%d ; test_index=%d ; current_state_index=%d ; num_states=%d\n", \
                    message, \
                    soft_thread->method_specific.soft_dfs.depth, (the_soft_dfs_info-soft_thread->method_specific.soft_dfs.soft_dfs_info), \
                    instance->num_times, the_soft_dfs_info->test_index, \
                    the_soft_dfs_info->current_state_index, \
                    (derived_states_list ? derived_states_list->num_states : -1) \
                    );  \
            fflush(stdout); \
            } \
        }
#else
#define TRACE0(no_use) {}
#endif

#ifndef FCS_WITHOUT_DEPTH_FIELD
/*
 * This macro traces the path of the state up to the original state,
 * and thus calculates its real depth.
 *
 * It then assigns the newly updated depth throughout the path.
 *
 * */

static GCC_INLINE void calculate_real_depth(fcs_bool_t calc_real_depth, fcs_collectible_state_t * ptr_state_orig)
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

/*
 * This macro marks a state as a dead end, and afterwards propogates
 * this information to its parent and ancestor states.
 * */

static void GCC_INLINE mark_as_dead_end(fcs_bool_t scans_synergy, fcs_collectible_state_t * ptr_state_input)
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

#define BUMP_NUM_TIMES() \
{       \
    (*instance_num_times_ptr)++; \
    (*hard_thread_num_times_ptr)++; \
}

#define SHOULD_STATE_BE_PRUNED(enable_pruning, ptr_state) \
    ( \
        enable_pruning && \
        (! (FCS_S_VISITED(ptr_state) & \
            FCS_VISITED_GENERATED_BY_PRUNING \
            ) \
        ) \
    )

#ifdef FCS_RCS_STATES

typedef struct {
    fcs_cache_key_info_t * new_cache_state;
    fcs_collectible_state_t * state_val;
} cache_parents_stack_item_t;

/* TODO : Unit-test this function as it had had a bug beforehand 
 * because fcs_lru_side_t had been an unsigned long.
 * */
typedef const char * fcs_lru_side_t;

extern int fc_solve_compare_lru_cache_keys(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((fcs_lru_side_t)(((const fcs_cache_key_info_t *)(p))->val_ptr))
    fcs_lru_side_t a, b;

    a = GET_PARAM(void_a);
    b = GET_PARAM(void_b);

    return ((a > b) ? 1 : (a < b) ? (-1) : 0);
#undef GET_PARAM
}

#define NEXT_CACHE_STATE(s) ((s)->lower_pri)
fcs_state_t * fc_solve_lookup_state_key_from_val(
    fc_solve_instance_t * instance,
    fcs_collectible_state_t * orig_ptr_state_val
)
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    PWord_t PValue;
#endif
    fcs_lru_cache_t * cache;
    cache_parents_stack_item_t * parents_stack;
    int parents_stack_len, parents_stack_max_len;
    fcs_cache_key_info_t * new_cache_state;
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    cache = &(instance->rcs_states_cache);

    parents_stack_len = 1;
    parents_stack_max_len = 16;

    parents_stack = malloc(sizeof(parents_stack[0]) * parents_stack_max_len);

    parents_stack[0].state_val = orig_ptr_state_val;

    while (1)
    {
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
        JLI (
            PValue,
            cache->states_values_to_keys_map,
            ((Word_t)parents_stack[parents_stack_len-1].state_val)
        );
        if (*PValue)
        {
            parents_stack[parents_stack_len-1].new_cache_state
                = new_cache_state
                = (fcs_cache_key_info_t *)(*PValue);
            break;
        }
        else
        {
            /* A new state. */
            if (cache->recycle_bin)
            {
                new_cache_state = cache->recycle_bin;
                cache->recycle_bin = NEXT_CACHE_STATE(new_cache_state);
            }
            else
            {
                new_cache_state
                    = fcs_compact_alloc_ptr(
                        &(cache->states_values_to_keys_allocator),
                        sizeof(*new_cache_state)
                    );
            }
        }
#else
        {
            fcs_cache_key_info_t * existing_cache_state;

            if (cache->recycle_bin)
            {
                new_cache_state = cache->recycle_bin;
                cache->recycle_bin = NEXT_CACHE_STATE(new_cache_state);
            }
            else
            {
                new_cache_state
                    = fcs_compact_alloc_ptr(
                        &(cache->states_values_to_keys_allocator),
                        sizeof(*new_cache_state)
                    );
            }

            new_cache_state->val_ptr = parents_stack[parents_stack_len-1].state_val;
            existing_cache_state = (fcs_cache_key_info_t *)fc_solve_kaz_tree_alloc_insert(
                cache->kaz_tree,
                new_cache_state
            );

            if (existing_cache_state)
            {
                NEXT_CACHE_STATE( new_cache_state ) = cache->recycle_bin;
                cache->recycle_bin = new_cache_state;

                parents_stack[parents_stack_len-1].new_cache_state
                    = new_cache_state = existing_cache_state;
                break;
            }
        }
#endif

            parents_stack[parents_stack_len-1].new_cache_state
                = new_cache_state;

#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
            *PValue = ((Word_t)new_cache_state);

            new_cache_state->val_ptr
                = parents_stack[parents_stack_len-1].state_val;
#endif

            new_cache_state->lower_pri = new_cache_state->higher_pri = NULL;

            cache->count_elements_in_cache++;

            if (!FCS_S_PARENT(parents_stack[parents_stack_len-1].state_val))
            {
                new_cache_state->key = instance->state_copy_ptr->s;
                break;
            }
            else
            {
                parents_stack[parents_stack_len].state_val =
                    FCS_S_PARENT(parents_stack[parents_stack_len-1].state_val);
                if (++parents_stack_len == parents_stack_max_len)
                {
                    parents_stack_max_len += 16;
                    parents_stack =
                        realloc(
                            parents_stack,
                            sizeof(parents_stack[0]) * parents_stack_max_len
                        );
                }
            }
    }

    for (parents_stack_len--; parents_stack_len > 0; parents_stack_len--)
    {
        fcs_collectible_state_t temp_new_state_val;
        fcs_internal_move_t * next_move, * moves_end;

        new_cache_state = parents_stack[parents_stack_len-1].new_cache_state;

        fcs_collectible_state_t * stack_ptr_this_state_val =
            parents_stack[parents_stack_len-1].state_val;

        fcs_duplicate_state(
            &(new_cache_state->key),
            &(temp_new_state_val),
            &(parents_stack[parents_stack_len].new_cache_state->key),
            parents_stack[parents_stack_len].state_val
        );

        moves_end = 
        (
            (next_move = stack_ptr_this_state_val->moves_to_parent->moves)
            + 
            stack_ptr_this_state_val->moves_to_parent->num_moves
        );

        for ( ;
            next_move < moves_end
            ; next_move++)
        {
            fc_solve_apply_move(
                &(new_cache_state->key),
                &(temp_new_state_val),
#ifdef FCS_WITHOUT_LOCS_FIELDS
                NULL,
#endif
                (*next_move),
                LOCAL_FREECELLS_NUM,
                LOCAL_STACKS_NUM,
                LOCAL_DECKS_NUM
            );
        }
        /* The state->parent_state moves stack has an implicit canonize
         * suffix move. */
        fc_solve_canonize_state(
#ifdef FCS_RCS_STATES
            &(new_cache_state->key),
#endif
            &(temp_new_state_val),
            LOCAL_FREECELLS_NUM,
            LOCAL_STACKS_NUM
        );

        /* Promote new_cache_state to the head of the priority list. */
        if (! cache->lowest_pri)
        {
            /* It's the only state. */
            cache->lowest_pri = new_cache_state;
            cache->highest_pri = new_cache_state;
        }
        else
        {
            /* First remove the state from its place in the doubly-linked
             * list by linking its neighbours together.
             * */
            if (new_cache_state->higher_pri)
            {
                new_cache_state->higher_pri->lower_pri =
                    new_cache_state->lower_pri;
            }
            if (new_cache_state->lower_pri)
            {
                new_cache_state->lower_pri->higher_pri =
                    new_cache_state->higher_pri;
            }
            /* Now promote it to be the highest. */
            cache->highest_pri->higher_pri = new_cache_state;
            new_cache_state->lower_pri = cache->highest_pri;
            new_cache_state->higher_pri = NULL;
            cache->highest_pri = new_cache_state;
        }
    }

#ifdef DEBUG
    {
        fcs_state_t * state = &(((fcs_cache_key_info_t * )(*PValue))->key);

        int s=0;
        for (s=0;s<INSTANCE_STACKS_NUM; s++)
        {
            fcs_cards_column_t col = fcs_state_get_col(*state, s);
            int col_len = fcs_col_len(col);
            int c;
            for (c = col_len ; c < MAX_NUM_CARDS_IN_A_STACK ; c++)
            {
                assert (fcs_col_get_card(col, c) == 0);
            }
        }
    }
#endif

    free(parents_stack);

    if (cache->count_elements_in_cache > cache->max_num_elements_in_cache)
    {
        long count = cache->count_elements_in_cache;
        long limit = cache->max_num_elements_in_cache;

        while (count > limit)
        {
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
            int Rc_int;
#endif
            fcs_cache_key_info_t * lowest_pri = cache->lowest_pri;

#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
            JLD(
                Rc_int,
                cache->states_values_to_keys_map,
                (Word_t)(lowest_pri->val_ptr)
            );
#else
            fc_solve_kaz_tree_delete_free(
                cache->kaz_tree,
                fc_solve_kaz_tree_lookup(
                    cache->kaz_tree, lowest_pri
                )
            );
#endif

            cache->lowest_pri = lowest_pri->higher_pri;
            cache->lowest_pri->lower_pri = NULL;

            NEXT_CACHE_STATE(lowest_pri)
                = cache->recycle_bin;

            cache->recycle_bin = lowest_pri;
            count--;
        }

        cache->count_elements_in_cache = count;
    }

    return &(new_cache_state->key);
}

#undef NEXT_CACHE_STATE

#endif

#define ASSIGN_STATE_KEY() (state_key = (*(fc_solve_lookup_state_key_from_val(instance, ptr_state))))

int fc_solve_soft_dfs_do_solve(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;

#ifdef FCS_RCS_STATES
    fcs_state_t state_key;
#endif
    fcs_collectible_state_t * ptr_state;
    fcs_soft_dfs_stack_item_t * the_soft_dfs_info;
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif
    int dfs_max_depth, by_depth_max_depth, by_depth_min_depth;

#ifndef FCS_WITHOUT_DEPTH_FIELD
    fcs_runtime_flags_t calc_real_depth = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_CALC_REAL_DEPTH);
#endif
    fcs_runtime_flags_t scans_synergy = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY);

    fcs_runtime_flags_t is_a_complete_scan = STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN);
    int soft_thread_id = soft_thread->id;
    fcs_derived_states_list_t * derived_states_list;
    fcs_tests_by_depth_unit_t * by_depth_units, * curr_by_depth_unit;
    fcs_tests_list_of_lists * the_tests_list_ptr;
    fcs_rand_t * rand_gen;
    fcs_bool_t local_to_randomize = FALSE;
    int * depth_ptr;
    fcs_bool_t enable_pruning;
    int * instance_num_times_ptr, * hard_thread_num_times_ptr;
    int hard_thread_max_num_times;
    fcs_instance_debug_iter_output_func_t debug_iter_output_func;
    fcs_instance_debug_iter_output_context_t debug_iter_output_context;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif


    the_soft_dfs_info = &(soft_thread->method_specific.soft_dfs.soft_dfs_info[soft_thread->method_specific.soft_dfs.depth]);

    dfs_max_depth = soft_thread->method_specific.soft_dfs.dfs_max_depth;
    enable_pruning = soft_thread->enable_pruning;

    ptr_state = the_soft_dfs_info->state;
    derived_states_list = &(the_soft_dfs_info->derived_states_list);

#ifdef FCS_RCS_STATES
    ASSIGN_STATE_KEY();
#endif

    rand_gen = &(soft_thread->method_specific.soft_dfs.rand_gen);

    calculate_real_depth(calc_real_depth, ptr_state);

    by_depth_units = soft_thread->method_specific.soft_dfs.tests_by_depth_array.by_depth_units;

#define THE_TESTS_LIST (*the_tests_list_ptr)
    TRACE0("Before depth loop");

#define GET_DEPTH(ptr) ((ptr)->max_depth)

#define RECALC_BY_DEPTH_LIMITS() \
    { \
        by_depth_max_depth = GET_DEPTH(curr_by_depth_unit); \
        by_depth_min_depth = (curr_by_depth_unit == by_depth_units) ? 0 : GET_DEPTH(curr_by_depth_unit-1); \
        the_tests_list_ptr = &(curr_by_depth_unit->tests); \
    }

    depth_ptr = &(soft_thread->method_specific.soft_dfs.depth);

    instance_num_times_ptr = &(instance->num_times);
    hard_thread_num_times_ptr = &(hard_thread->num_times);

#define CALC_HARD_THREAD_MAX_NUM_TIMES() \
    hard_thread_max_num_times = hard_thread->max_num_times; \
                \
    {           \
        int lim = hard_thread->num_times       \
            + (instance->effective_max_num_times - *(instance_num_times_ptr)) \
            ; \
              \
        hard_thread_max_num_times = min(hard_thread_max_num_times, lim); \
    }

    CALC_HARD_THREAD_MAX_NUM_TIMES();

    debug_iter_output_func = instance->debug_iter_output_func;
    debug_iter_output_context = instance->debug_iter_output_context;

#define DEPTH() (*depth_ptr)

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

            /* Because the address of soft_thread->method_specific.soft_dfs.soft_dfs_info may
             * be changed
             * */
            the_soft_dfs_info = &(soft_thread->method_specific.soft_dfs.soft_dfs_info[DEPTH()]);
            dfs_max_depth = soft_thread->method_specific.soft_dfs.dfs_max_depth;
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
                    FCS_S_VISITED(ptr_state) |= FCS_VISITED_ALL_TESTS_DONE;
                    mark_as_dead_end (scans_synergy, ptr_state);
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

                    ptr_state = the_soft_dfs_info->state;


#ifdef DEBUG
                    TRACE0("Verify Foo");
                    verify_state_sanity(ptr_state);
                    VERIFY_SOFT_DFS_STACK(soft_thread);
#endif

#ifdef FCS_RCS_STATES
                    ASSIGN_STATE_KEY();
#endif

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
                int i;

                TRACE0("In iter_handler");

                if (debug_iter_output_func)
                {
#ifdef DEBUG
                    printf("ST Name: %s\n", soft_thread->name);
#endif
                    debug_iter_output_func(
                        debug_iter_output_context,
                        *(instance_num_times_ptr),
                        soft_thread->method_specific.soft_dfs.depth,
                        (void*)instance,
#ifdef FCS_RCS_STATES
                        &(state_key),
#endif
                        ptr_state,
#ifdef FCS_WITHOUT_VISITED_ITER
                        0
#else
                        ((soft_thread->method_specific.soft_dfs.depth == 0) ?
                            0 :
                            FCS_S_VISITED_ITER(soft_thread->method_specific.soft_dfs.soft_dfs_info[soft_thread->method_specific.soft_dfs.depth-1].state)
                        )
#endif
                        );
                }

                /* Count the free-cells */
                num_vacant_freecells = 0;
                for(i=0;i<LOCAL_FREECELLS_NUM;i++)
                {
                    if (fcs_freecell_card_num(the_state, i) == 0)
                    {
                        num_vacant_freecells++;
                    }
                }

                /* Count the number of unoccupied stacks */

                num_vacant_stacks = 0;
                for(i=0;i<LOCAL_STACKS_NUM;i++)
                {
                    if (fcs_col_len(fcs_state_get_col(the_state, i)) == 0)
                    {
                        num_vacant_stacks++;
                    }
                }

                /* Check if we have reached the empty state */
                if (unlikely((num_vacant_stacks == LOCAL_STACKS_NUM) &&
                    (num_vacant_freecells  == LOCAL_FREECELLS_NUM)))
                {
                    instance->final_state = ptr_state;

                    BUMP_NUM_TIMES();

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
                if (SHOULD_STATE_BE_PRUNED(enable_pruning, ptr_state))
                {
                    fcs_collectible_state_t * derived;
#ifdef FCS_RCS_STATES
                    fcs_state_t derived_key;
#endif

                    if (fc_solve_sfs_raymond_prune(
                        soft_thread,
#ifdef FCS_RCS_STATES
                        &(state_key),
#endif
                        ptr_state,
#ifdef FCS_RCS_STATES
                        &derived_key,
#endif
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
                        realloc(
                            the_soft_dfs_info->derived_states_random_indexes,
                            sizeof(the_soft_dfs_info->derived_states_random_indexes[0]) * the_soft_dfs_info->derived_states_random_indexes_max_size
                            );
                        }

                        the_soft_dfs_info->derived_states_random_indexes[0] = 0;
                    }
                }
            }

            TRACE0("After iter_handler");

            if (the_soft_dfs_info->tests_list_index < THE_TESTS_LIST.num_lists)
            {
                /* Always do the first test */
                local_to_randomize = THE_TESTS_LIST.lists[
                    the_soft_dfs_info->tests_list_index
                    ].to_randomize;
            do
            {

#ifdef DEBUG
                TRACE0("Verify Bar");
                verify_state_sanity(ptr_state);
                VERIFY_SOFT_DFS_STACK(soft_thread);
#endif

                THE_TESTS_LIST.lists[
                    the_soft_dfs_info->tests_list_index
                    ].tests[the_soft_dfs_info->test_index]
                    (
                        soft_thread,
#ifdef FCS_RCS_STATES
                        &(state_key),
#endif
                        ptr_state,
                        derived_states_list
                    );

#ifdef DEBUG
                TRACE0("Verify Glanko");
                verify_state_sanity(ptr_state);
                VERIFY_SOFT_DFS_STACK(soft_thread);
#endif


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
            } while (local_to_randomize);
            }

            {
                int a, j;
                int swap_save;
                int * rand_array, * ra_ptr;
                int num_states = derived_states_list->num_states;

                if (num_states >
                        the_soft_dfs_info->derived_states_random_indexes_max_size)
                {
                    the_soft_dfs_info->derived_states_random_indexes_max_size =
                        num_states;
                    the_soft_dfs_info->derived_states_random_indexes =
                        realloc(
                            the_soft_dfs_info->derived_states_random_indexes,
                            sizeof(the_soft_dfs_info->derived_states_random_indexes[0]) * the_soft_dfs_info->derived_states_random_indexes_max_size
                            );
                }
                rand_array = the_soft_dfs_info->derived_states_random_indexes;

#ifdef DEBUG
                TRACE0("Verify Panter");
                verify_state_sanity(ptr_state);
                VERIFY_SOFT_DFS_STACK(soft_thread);
#endif


                for(a=0, ra_ptr = rand_array; a < num_states ; a++)
                {
                    *(ra_ptr++) = a;
                }
                /* If we just conducted the tests for a random group -
                 * randomize. Else - keep those indexes as the unity vector.
                 *
                 * Also, do not randomize if this is a pure soft-DFS scan.
                 * */
                if (local_to_randomize)
                {
                    a = num_states-1;
                    while (a > 0)
                    {
                        j =
                            (
                                fc_solve_rand_get_random_number(
                                    rand_gen
                                )
                                % (a+1)
                            );

                        swap_save = rand_array[a];
                        rand_array[a] = rand_array[j];
                        rand_array[j] = swap_save;
                        a--;
                    }
                }
            }

#ifdef DEBUG
            TRACE0("Verify Rondora");
            verify_state_sanity(ptr_state);
            VERIFY_SOFT_DFS_STACK(soft_thread);
#endif

            /* We just performed a test, so the index of the first state that
               ought to be checked in this depth is 0.
               */
            the_soft_dfs_info->current_state_index = 0;
        }

        {
            int num_states = derived_states_list->num_states;
            fcs_derived_states_list_item_t * derived_states =
                derived_states_list->states;
            int * rand_array = the_soft_dfs_info->derived_states_random_indexes;
            fcs_collectible_state_t * single_derived_state;

#ifdef DEBUG
            TRACE0("Verify Klondike");
            verify_state_sanity(ptr_state);
            VERIFY_SOFT_DFS_STACK(soft_thread);
#endif

            while (the_soft_dfs_info->current_state_index <
                   num_states)
            {
                single_derived_state = derived_states[
                        rand_array[
                            the_soft_dfs_info->current_state_index++
                        ]
                    ].state_ptr;

#ifdef DEBUG
            TRACE0("Verify Seahaven");
            verify_state_sanity(ptr_state);
            verify_state_sanity(single_derived_state);
            VERIFY_SOFT_DFS_STACK(soft_thread);
#endif

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
                    BUMP_NUM_TIMES();

#ifdef DEBUG
                    TRACE0("Verify Gypsy");
                    verify_state_sanity(single_derived_state);
                    verify_state_sanity(ptr_state);
                    VERIFY_SOFT_DFS_STACK(soft_thread);
#endif

                    set_scan_visited(
                        single_derived_state,
                        soft_thread_id
                    );

#ifndef FCS_WITHOUT_VISITED_ITER
                    FCS_S_VISITED_ITER(single_derived_state) = instance->num_times;
#endif

#ifdef DEBUG
                    TRACE0("Verify Golf");
                    verify_state_sanity(single_derived_state);
                    verify_state_sanity(ptr_state);
                    VERIFY_SOFT_DFS_STACK(soft_thread);
#endif


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

                    the_soft_dfs_info->state =
                        ptr_state =
                        single_derived_state;

#ifdef DEBUG
                    TRACE0("Verify Zap");
                    verify_state_sanity(single_derived_state);
                    verify_state_sanity(ptr_state);
                    VERIFY_SOFT_DFS_STACK(soft_thread);
#endif

#ifdef FCS_RCS_STATES
                    ASSIGN_STATE_KEY();
#endif


                    the_soft_dfs_info->tests_list_index = 0;
                    the_soft_dfs_info->test_index = 0;
                    the_soft_dfs_info->current_state_index = 0;
                    the_soft_dfs_info->positions_by_rank = NULL;
                    derived_states_list = &(the_soft_dfs_info->derived_states_list);
                    derived_states_list->num_states = 0;

                    calculate_real_depth(calc_real_depth, ptr_state);

                    if (check_num_states_in_collection(instance))
                    {
#ifdef DEBUG
                        TRACE0("Verify Bakers_Game");
                        VERIFY_SOFT_DFS_STACK(soft_thread);
#endif

                        free_states(instance);
#ifdef DEBUG
                        TRACE0("Verify Penguin");
                        VERIFY_SOFT_DFS_STACK(soft_thread);
#endif

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
    BUMP_NUM_TIMES();

    DEPTH() = -1;

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


#undef state
#undef myreturn

#define FCS_BEFS_CARDS_UNDER_SEQUENCES_EXPONENT 1.3
#define FCS_BEFS_SEQS_OVER_RENEGADE_CARDS_EXPONENT 1.3

#define FCS_SEQS_OVER_RENEGADE_POWER(n) pow(n, FCS_BEFS_SEQS_OVER_RENEGADE_CARDS_EXPONENT)

static GCC_INLINE int update_col_cards_under_sequences(
        fc_solve_soft_thread_t * soft_thread,
        fcs_cards_column_t col,
        double * cards_under_sequences_ptr
        )
{
    int cards_num;
    int c;
    fcs_card_t this_card, prev_card;
#ifndef FCS_FREECELL_ONLY
    int sequences_are_built_by =
        GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(soft_thread->hard_thread->instance)
        ;
#endif

    cards_num = fcs_col_len(col);
    c = cards_num - 2;
    this_card = fcs_col_get_card(col, c+1);
    prev_card = fcs_col_get_card(col, c);
    while ((c >= 0) && fcs_is_parent_card(this_card,prev_card))
    {
        this_card = prev_card;
        if (--c>=0)
        {
            prev_card = fcs_col_get_card(col, c);
        }
    }
    *cards_under_sequences_ptr += pow(c+1, FCS_BEFS_CARDS_UNDER_SEQUENCES_EXPONENT);
    return c;
}

static GCC_INLINE void initialize_befs_rater(
    fc_solve_soft_thread_t * soft_thread,
#ifdef FCS_RCS_STATES
    fcs_state_t * ptr_state_key,
#define STATE() (*ptr_state_key)
#else
#define STATE() (ptr_state->s)
#endif
    fcs_collectible_state_t * ptr_state
    )
{
#ifndef HARD_CODED_NUM_STACKS
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;
#endif

    int a;
    double cards_under_sequences;

    cards_under_sequences = 0;
    for(a=0;a<INSTANCE_STACKS_NUM;a++)
    {
        update_col_cards_under_sequences(soft_thread, fcs_state_get_col(STATE(), a), &cards_under_sequences);
    }
    soft_thread->method_specific.befs.meth.befs.initial_cards_under_sequences_value = cards_under_sequences;
}
#undef STATE

#undef TRACE0

#ifdef DEBUG

#define TRACE0(message) \
        { \
            if (getenv("FCS_TRACE")) \
            { \
            printf("BestFS(rate_state) - %s ; rating=%.40f .\n", \
                    message, \
                    ret \
                    );  \
            fflush(stdout); \
            } \
        }

#else

#define TRACE0(no_use) {}

#endif


static GCC_INLINE pq_rating_t befs_rate_state(
    fc_solve_soft_thread_t * soft_thread,
#ifdef FCS_RCS_STATES
    fcs_state_t * ptr_state_key,
#endif
    fcs_collectible_state_t * ptr_state
    )
{
#ifdef FCS_RCS_STATES
#define state_key (*ptr_state_key)
#endif
#ifndef FCS_FREECELL_ONLY
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;
#endif

    double ret=0;
    int a, c, cards_num, num_cards_in_founds;
    fcs_game_limit_t num_vacant_stacks, num_vacant_freecells;
    double cards_under_sequences, temp;
    double seqs_over_renegade_cards;
    fcs_cards_column_t col;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    DECLARE_GAME_PARAMS();
#endif
#define my_befs_weights soft_thread->method_specific.befs.meth.befs.befs_weights
    double * befs_weights = my_befs_weights;
#ifndef FCS_FREECELL_ONLY
    int unlimited_sequence_move = INSTANCE_UNLIMITED_SEQUENCE_MOVE;
#else
    #define unlimited_sequence_move 0
#endif

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif

    cards_under_sequences = 0;
    num_vacant_stacks = 0;
    seqs_over_renegade_cards = 0;
    for(a=0;a<LOCAL_STACKS_NUM;a++)
    {
        col = fcs_state_get_col(the_state, a);
        cards_num = fcs_col_len(col);

        if (cards_num == 0)
        {
            num_vacant_stacks++;
        }

        if (cards_num <= 1)
        {
            continue;
        }

        c = update_col_cards_under_sequences(soft_thread, col, &cards_under_sequences);
        if (c >= 0)
        {
            seqs_over_renegade_cards +=
                ((unlimited_sequence_move) ?
                    1 :
                    FCS_SEQS_OVER_RENEGADE_POWER(cards_num-c-1)
                    );
        }
    }

    ret += ((soft_thread->method_specific.befs.meth.befs.initial_cards_under_sequences_value - cards_under_sequences)
            / soft_thread->method_specific.befs.meth.befs.initial_cards_under_sequences_value) * befs_weights[FCS_BEFS_WEIGHT_CARDS_UNDER_SEQUENCES];

    ret += (seqs_over_renegade_cards /
               FCS_SEQS_OVER_RENEGADE_POWER(LOCAL_DECKS_NUM*(13*4))
            )
           * befs_weights[FCS_BEFS_WEIGHT_SEQS_OVER_RENEGADE_CARDS];

    num_cards_in_founds = 0;
    for(a=0;a<(LOCAL_DECKS_NUM<<2);a++)
    {
        num_cards_in_founds += fcs_foundation_value((the_state), a);
    }

    ret += ((double)num_cards_in_founds/(LOCAL_DECKS_NUM*52)) * befs_weights[FCS_BEFS_WEIGHT_CARDS_OUT];

    num_vacant_freecells = 0;
    for(a=0;a<LOCAL_FREECELLS_NUM;a++)
    {
        if (fcs_freecell_card_num((the_state),a) == 0)
        {
            num_vacant_freecells++;
        }
    }

#ifdef FCS_FREECELL_ONLY
#define is_filled_by_any_card() 1
#else
#define is_filled_by_any_card() (INSTANCE_EMPTY_STACKS_FILL == FCS_ES_FILLED_BY_ANY_CARD)
#endif
    if (is_filled_by_any_card())
    {
        if (unlimited_sequence_move)
        {
            temp = (((double)num_vacant_freecells+num_vacant_stacks)/(LOCAL_FREECELLS_NUM+INSTANCE_STACKS_NUM));
        }
        else
        {
            temp = (((double)((num_vacant_freecells+1)<<num_vacant_stacks)) / ((LOCAL_FREECELLS_NUM+1)<<(INSTANCE_STACKS_NUM)));
        }
    }
    else
    {
        if (unlimited_sequence_move)
        {
            temp = (((double)num_vacant_freecells)/LOCAL_FREECELLS_NUM);
        }
        else
        {
            temp = 0;
        }
    }

    ret += (temp * befs_weights[FCS_BEFS_WEIGHT_MAX_SEQUENCE_MOVE]);

#ifdef FCS_WITHOUT_DEPTH_FIELD
    ret += befs_weights[FCS_BEFS_WEIGHT_DEPTH];
#else
    if (FCS_S_DEPTH(ptr_state) <= 20000)
    {
        ret += ((20000 - FCS_S_DEPTH(ptr_state))/20000.0) * befs_weights[FCS_BEFS_WEIGHT_DEPTH];
    }
#endif

    TRACE0("Before return");

    return (int)(ret*INT_MAX);
}
#ifdef FCS_RCS_STATES
#undef state_key
#endif


#ifdef FCS_FREECELL_ONLY
#undef unlimited_sequence_move
#endif

/*
    fc_solve_befs_or_bfs_do_solve() is the main event
    loop of the BeFS And BFS scans. It is quite simple as all it does is
    extract elements out of the queue or priority queue and run all the test
    of them.

    It goes on in this fashion until the final state was reached or
    there are no more states in the queue.
*/

#undef TRACE0

#ifdef DEBUG

#define TRACE0(message) \
        { \
            if (getenv("FCS_TRACE")) \
            { \
            printf("BestFS - %s ; Iters=%d.\n", \
                    message, \
                    instance->num_times \
                    );  \
            fflush(stdout); \
            } \
        }

#else

#define TRACE0(no_use) {}

#endif

#define my_brfs_queue (soft_thread->method_specific.befs.meth.brfs.bfs_queue)
#define my_brfs_queue_last_item \
    (soft_thread->method_specific.befs.meth.brfs.bfs_queue_last_item)
#define my_brfs_recycle_bin (soft_thread->method_specific.befs.meth.brfs.recycle_bin)

#define NEW_BRFS_QUEUE_ITEM() \
    ((fcs_states_linked_list_item_t *) \
    fcs_compact_alloc_ptr( \
        &(hard_thread->allocator), \
        sizeof(fcs_states_linked_list_item_t) \
    ));

static void GCC_INLINE fc_solve_initialize_bfs_queue(fc_solve_soft_thread_t * soft_thread)
{
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;

    /* Initialize the BFS queue. We have one dummy element at the beginning
       in order to make operations simpler. */
    my_brfs_queue =
        NEW_BRFS_QUEUE_ITEM();

    my_brfs_queue_last_item =
        my_brfs_queue->next =
        NEW_BRFS_QUEUE_ITEM();

    my_brfs_queue_last_item->next = NULL;

    my_brfs_recycle_bin = NULL;

    return;
}


static GCC_INLINE void normalize_befs_weights(
    fc_solve_soft_thread_t * soft_thread
    )
{
    /* Normalize the BeFS Weights, so the sum of all of them would be 1. */
    double sum;
    int a;
    sum = 0;
    for(a=0;a<(sizeof(my_befs_weights)/sizeof(my_befs_weights[0]));a++)
    {
        if (my_befs_weights[a] < 0)
        {
            my_befs_weights[a] = fc_solve_default_befs_weights[a];
        }
        sum += my_befs_weights[a];
    }
    if (sum < 1e-6)
    {
        sum = 1;
    }
    for(a=0;a<(sizeof(my_befs_weights)/sizeof(my_befs_weights[0]));a++)
    {
        my_befs_weights[a] /= sum;
    }
}

void fc_solve_soft_thread_init_befs_or_bfs(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_instance_t * instance = soft_thread->hard_thread->instance;
    fcs_collectible_state_t * ptr_orig_state;
#ifdef FCS_RCS_STATES
    ptr_orig_state = &(instance->state_copy_ptr->info);
#else
    ptr_orig_state = instance->state_copy_ptr;
#endif

    if (soft_thread->method == FCS_METHOD_A_STAR)
    {
        /* Initialize the priotity queue of the BeFS scan */
        fc_solve_PQueueInitialise(
            &(soft_thread->method_specific.befs.meth.befs.pqueue),
            1024
        );

        normalize_befs_weights(soft_thread);

        initialize_befs_rater(
            soft_thread,
#ifdef FCS_RCS_STATES
            &(instance->state_copy_ptr->s),
#endif
            ptr_orig_state
            );
    }
    else
    {
        fc_solve_initialize_bfs_queue(soft_thread);
    }

    if (! soft_thread->method_specific.befs.tests_list)
    {
        fc_solve_solve_for_state_test_t * tests_list, * next_test;
        int tests_order_num;
        int * tests_order_tests;
        int i;

        tests_order_tests = soft_thread->by_depth_tests_order.by_depth_tests[0].tests_order.tests;

        tests_order_num = soft_thread->by_depth_tests_order.by_depth_tests[0].tests_order.num;

        tests_list = malloc(sizeof(tests_list[0]) * tests_order_num);

        for (i = 0, next_test = tests_list ; i < tests_order_num ; i++)
        {
            *(next_test++) =
                    fc_solve_sfs_tests[
                        tests_order_tests[i] & FCS_TEST_ORDER_NO_FLAGS_MASK
                    ];
        }
        soft_thread->method_specific.befs.tests_list = tests_list;
        soft_thread->method_specific.befs.tests_list_end = next_test;
    }

    soft_thread->first_state_to_check = ptr_orig_state;

    return;
}

#ifdef DEBUG
#if 0
static void dump_pqueue (
    fc_solve_soft_thread_t * soft_thread,
    const char * stage_id,
    PQUEUE * pq
    )
{
    int i;
    char * s;
    fc_solve_instance_t * instance = soft_thread->hard_thread->instance;

    if (strcmp(soft_thread->name, "11"))
    {
        return;
    }

    printf("<pqueue_dump stage=\"%s\">\n\n", stage_id);

    for (i = PQ_FIRST_ENTRY ; i < pq->CurrentSize ; i++)
    {
        printf("Rating[%d] = %d\nState[%d] = <<<\n", i, pq->Elements[i].rating, i);
        s = fc_solve_state_as_string(pq->Elements[i].val,
                INSTANCE_FREECELLS_NUM,
                INSTANCE_STACKS_NUM,
                INSTANCE_DECKS_NUM,
                1,
                0,
                1
                );

        printf("%s\n>>>\n\n", s);

        free(s);
    }

    printf("\n\n</pqueue_dump>\n\n");
}
#else
#define dump_pqueue(a,b,c) {}
#endif
#endif

#ifdef FCS_WITHOUT_DEPTH_FIELD
static GCC_INLINE int calc_depth(fcs_collectible_state_t * ptr_state)
{
    register int ret = 0;
    while ((ptr_state = FCS_S_PARENT(ptr_state)) != NULL)
    {
        ret++;
    }
    return ret;
}
#endif
int fc_solve_befs_or_bfs_do_solve(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;

    fcs_collectible_state_t * ptr_state, * ptr_new_state;
#ifdef FCS_RCS_STATES
    fcs_state_t state_key;
#endif
    fcs_game_limit_t num_vacant_stacks, num_vacant_freecells;
    fcs_states_linked_list_item_t * save_item;
    int a;
    fcs_derived_states_list_t derived;
    int derived_index;
    fcs_bool_t enable_pruning;

    int method;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif

    fc_solve_solve_for_state_test_t * tests_list, * tests_list_end;
    fc_solve_solve_for_state_test_t * next_test;

#ifndef FCS_WITHOUT_DEPTH_FIELD
    fcs_runtime_flags_t calc_real_depth = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_CALC_REAL_DEPTH);
#endif
    fcs_runtime_flags_t scans_synergy = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY);
    int soft_thread_id = soft_thread->id;
    fcs_runtime_flags_t is_a_complete_scan = STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN);

    fcs_states_linked_list_item_t * queue = NULL;
    fcs_states_linked_list_item_t * queue_last_item = NULL;
    PQUEUE * pqueue = NULL;

    int error_code;

    int * instance_num_times_ptr, * hard_thread_num_times_ptr;

    int hard_thread_max_num_times;

    fcs_instance_debug_iter_output_func_t debug_iter_output_func;
    fcs_instance_debug_iter_output_context_t debug_iter_output_context;

    derived.num_states = 0;
    derived.states = NULL;

    tests_list = soft_thread->method_specific.befs.tests_list;
    tests_list_end = soft_thread->method_specific.befs.tests_list_end;

    ptr_state = soft_thread->first_state_to_check;
    enable_pruning = soft_thread->enable_pruning;

    method = soft_thread->method;
    instance_num_times_ptr = &(instance->num_times);
    hard_thread_num_times_ptr = &(hard_thread->num_times);

    if (method == FCS_METHOD_A_STAR)
    {
        pqueue = &(soft_thread->method_specific.befs.meth.befs.pqueue);
    }
    else
    {
        queue = my_brfs_queue;
        queue_last_item = my_brfs_queue_last_item;
    }

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    CALC_HARD_THREAD_MAX_NUM_TIMES();

    debug_iter_output_func = instance->debug_iter_output_func;
    debug_iter_output_context = instance->debug_iter_output_context;

    /* Continue as long as there are states in the queue or
       priority queue. */
    while ( ptr_state != NULL)
    {
        TRACE0("Start of loop");

#ifdef FCS_RCS_STATES
        ASSIGN_STATE_KEY();
#endif

#ifdef DEBUG
        dump_pqueue(soft_thread, "loop_start", scan_specific.pqueue);
#endif

        /*
         * If we do the pruning after checking for being visited, then
         * there's a risk of inconsistent result when being interrupted
         * because we check once for the pruned state (after the scan
         * was suspended) and another time for the uninterrupted state.
         *
         * Therefore, we prune before checking for the visited flags.
         * */
        TRACE0("Pruning");
        if (SHOULD_STATE_BE_PRUNED(enable_pruning, ptr_state))
        {
            fcs_collectible_state_t * derived;
#ifdef FCS_RCS_STATES
            fcs_state_t derived_key;
#endif

            if (fc_solve_sfs_raymond_prune(
                    soft_thread,
#ifdef FCS_RCS_STATES
                    (&state_key),
#endif
                    ptr_state,
#ifdef FCS_RCS_STATES
                    &derived_key,
#endif
                    &derived
                ) == PRUNE_RET_FOLLOW_STATE
            )
            {
                ptr_state = derived;

#ifdef FCS_RCS_STATES
                ASSIGN_STATE_KEY();
#endif

            }
        }

        {
             register int temp_visited = FCS_S_VISITED(ptr_state);

            /*
             * If this is an optimization scan and the state being checked is
             * not in the original solution path - move on to the next state
             * */
            /*
             * It the state has already been visited - move on to the next
             * state.
             * */
            if ((method == FCS_METHOD_OPTIMIZE) ?
                    (
                        (!(temp_visited & FCS_VISITED_IN_SOLUTION_PATH))
                            ||
                        (temp_visited & FCS_VISITED_IN_OPTIMIZED_PATH)
                    )
                    :
                    (
                        (temp_visited & FCS_VISITED_DEAD_END)
                            ||
                        (is_scan_visited(ptr_state, soft_thread_id))
                    )
                )
            {
                goto label_next_state;
            }
        }

        TRACE0("Counting cells");

        /* Count the free-cells */
        num_vacant_freecells = 0;
        for(a=0;a<LOCAL_FREECELLS_NUM;a++)
        {
            if (fcs_freecell_card_num(the_state, a) == 0)
            {
                num_vacant_freecells++;
            }
        }

        /* Count the number of unoccupied stacks */

        num_vacant_stacks = 0;
        for(a=0;a<LOCAL_STACKS_NUM;a++)
        {
            if (fcs_col_len(fcs_state_get_col(the_state, a)) == 0)
            {
                num_vacant_stacks++;
            }
        }


        if (check_if_limits_exceeded())
        {
            soft_thread->first_state_to_check = ptr_state;

            TRACE0("myreturn - FCS_STATE_SUSPEND_PROCESS");
            error_code = FCS_STATE_SUSPEND_PROCESS;
            goto my_return_label;
        }

        TRACE0("debug_iter_output");
        if (debug_iter_output_func)
        {
#ifdef DEBUG
            printf("ST Name: %s\n", soft_thread->name);
#endif
            debug_iter_output_func(
                    debug_iter_output_context,
                    *(instance_num_times_ptr),
#ifdef FCS_WITHOUT_DEPTH_FIELD
                    calc_depth(ptr_state),
#else
                    FCS_S_DEPTH(ptr_state),
#endif
                    (void*)instance,
#ifdef FCS_RCS_STATES
                    (&state_key),
#endif
                    ptr_state,
#ifdef FCS_WITHOUT_VISITED_ITER
                    0
#else
                    ((FCS_S_PARENT(ptr_state) == NULL) ?
                        0 :
                        FCS_S_VISITED_ITER(FCS_S_PARENT(ptr_state))
                    )
#endif
                    );
        }


        if ((num_vacant_stacks == LOCAL_STACKS_NUM) && (num_vacant_freecells == LOCAL_FREECELLS_NUM))
        {
            instance->final_state = ptr_state;

            BUMP_NUM_TIMES();

            error_code = FCS_STATE_WAS_SOLVED;
            goto my_return_label;
        }

        calculate_real_depth (calc_real_depth, ptr_state);

        soft_thread->num_vacant_freecells = num_vacant_freecells;
        soft_thread->num_vacant_stacks = num_vacant_stacks;

        if (soft_thread->method_specific.befs.befs_positions_by_rank)
        {
            free(soft_thread->method_specific.befs.befs_positions_by_rank);
            soft_thread->method_specific.befs.befs_positions_by_rank = NULL;
        }

        TRACE0("perform_tests");

        /*
         * Do all the tests at one go, because that is the way it should be
         * done for BFS and BeFS.
        */
        derived.num_states = 0;
        for(next_test = tests_list;
            next_test < tests_list_end;
            next_test++
           )
        {
            (*next_test)(
                soft_thread,
#ifdef FCS_RCS_STATES
                &(state_key),
#endif
                ptr_state,
                &derived
            );
        }

        if (is_a_complete_scan)
        {
            FCS_S_VISITED(ptr_state) |= FCS_VISITED_ALL_TESTS_DONE;
        }

        /* Increase the number of iterations by one .
         * */
        BUMP_NUM_TIMES();


        TRACE0("Insert all states");
        /* Insert all the derived states into the PQ or Queue */

        for(derived_index = 0 ; derived_index < derived.num_states ; derived_index++)
        {
            ptr_new_state = derived.states[derived_index].state_ptr;

            if (method == FCS_METHOD_A_STAR)
            {
                fc_solve_PQueuePush(
                    pqueue,
                    ptr_new_state,
                    befs_rate_state(
                        soft_thread,
#ifdef FCS_RCS_STATES
                        fc_solve_lookup_state_key_from_val(instance, ptr_new_state),
#endif
                        ptr_new_state
                        )
                    );
            }
            else
            {
                /* Enqueue the new state. */
                fcs_states_linked_list_item_t * last_item_next;

                if (my_brfs_recycle_bin)
                {
                    last_item_next = my_brfs_recycle_bin;
                    my_brfs_recycle_bin = my_brfs_recycle_bin->next;
                }
                else
                {
                    last_item_next = NEW_BRFS_QUEUE_ITEM();
                }

                queue_last_item->next = last_item_next;

                queue_last_item->s = ptr_new_state;
                last_item_next->next = NULL;
                queue_last_item = last_item_next;
            }
        }

        if (method == FCS_METHOD_OPTIMIZE)
        {
            FCS_S_VISITED(ptr_state) |= FCS_VISITED_IN_OPTIMIZED_PATH;
        }
        else
        {
            set_scan_visited(
                    ptr_state,
                    soft_thread_id
                    );

            if (derived.num_states == 0)
            {
                if (is_a_complete_scan)
                {
                    mark_as_dead_end(scans_synergy, ptr_state);
                }
            }
        }

#ifndef FCS_WITHOUT_VISITED_ITER
        FCS_S_VISITED_ITER(ptr_state) = *(instance_num_times_ptr)-1;
#endif

label_next_state:
        TRACE0("Label next state");
        /*
            Extract the next item in the queue/priority queue.
        */
        if (method == FCS_METHOD_A_STAR)
        {

#ifdef DEBUG
            dump_pqueue(soft_thread, "before_pop", scan_specific.pqueue);
#endif
            /* It is an BeFS scan */
            fc_solve_PQueuePop(
                pqueue,
                &ptr_state
                );
        }
        else
        {
            save_item = queue->next;
            if (save_item != queue_last_item)
            {
                ptr_state = save_item->s;
                queue->next = save_item->next;
                save_item->next = my_brfs_recycle_bin;
                my_brfs_recycle_bin = save_item;
            }
            else
            {
                ptr_state = NULL;
            }
        }
    }

    error_code = FCS_STATE_IS_NOT_SOLVEABLE;
my_return_label:
    /* Free the memory that was allocated by the
     * derived states list */
    if (derived.states != NULL)
    {
        free(derived.states);
    }

    if (method != FCS_METHOD_A_STAR)
    {
        my_brfs_queue_last_item = queue_last_item;
    }

    if (soft_thread->method_specific.befs.befs_positions_by_rank)
    {
        free(soft_thread->method_specific.befs.befs_positions_by_rank);
        soft_thread->method_specific.befs.befs_positions_by_rank = NULL;
    }

    return error_code;
}

#undef myreturn

/*
 * Calculate, cache and return the positions_by_rank meta-data
 * about the currently-evaluated state.
 */
extern char * fc_solve_get_the_positions_by_rank_data(
        fc_solve_soft_thread_t * soft_thread,
#ifdef FCS_RCS_STATES
        fcs_state_t * ptr_state_key,
#endif
        fcs_collectible_state_t * ptr_state
        )
{
#ifdef FCS_RCS_STATES
#define state_key (*ptr_state_key)
#endif

    char * * positions_by_rank_location;

#ifdef DEBUG
    if (getenv("FCS_TRACE"))
    {
        printf("%s\n", "Verify Quux");
        fflush(stdout);
    }
    verify_state_sanity(ptr_state);
#endif

    switch(soft_thread->method)
    {
        case FCS_METHOD_SOFT_DFS:
        case FCS_METHOD_RANDOM_DFS:
            {
                positions_by_rank_location = &(
                    soft_thread->method_specific.soft_dfs.soft_dfs_info[
                        soft_thread->method_specific.soft_dfs.depth
                    ].positions_by_rank
                    );
            }
            break;
        default:
            {
                positions_by_rank_location = &(
                        soft_thread->method_specific.befs.befs_positions_by_rank
                        );
            }
            break;
    }

    if (unlikely(! *positions_by_rank_location))
    {
        char * positions_by_rank;
#if (!(defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
        fc_solve_instance_t * instance;
        DECLARE_GAME_PARAMS();
#endif

#ifndef FCS_FREECELL_ONLY
        int sequences_are_built_by;
#endif

#if (!(defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
        instance = soft_thread->hard_thread->instance;
        SET_GAME_PARAMS();
#endif

#ifndef FCS_FREECELL_ONLY
        sequences_are_built_by = GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance);
#endif

        /* We don't keep track of kings (rank == 13). */
#define NUM_POS_BY_RANK_SLOTS 13
        /* We need 2 chars per card - one for the column_idx and one
         * for the card_idx.
         *
         * We also need it times 13 for each of the ranks.
         *
         * We need (4*LOCAL_DECKS_NUM+1) slots to hold the cards plus a
         * (-1,-1) (= end) padding.             * */
#define FCS_POS_BY_RANK_SIZE (sizeof(positions_by_rank[0]) * NUM_POS_BY_RANK_SLOTS * FCS_POS_BY_RANK_WIDTH)

        positions_by_rank = malloc(FCS_POS_BY_RANK_SIZE);

        memset(positions_by_rank, -1, FCS_POS_BY_RANK_SIZE);

        {
            /* Populate positions_by_rank by looping over the stacks and
             * indices looking for the cards and filling them. */

            {
                int ds;
                char * ptr;

                for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
                {
                    fcs_cards_column_t dest_col;
                    int top_card_idx;
                    fcs_card_t dest_card;

                    dest_col = fcs_state_get_col(the_state, ds);
                    top_card_idx = fcs_col_len(dest_col);

                    if (unlikely((top_card_idx--) == 0))
                    {
                        continue;
                    }

                    {
                        fcs_card_t dest_below_card;
                        int dc;
                        for (
                              dc=0,
                              dest_card = fcs_col_get_card(dest_col, 0)
                                ;
                              dc < top_card_idx
                                ;
                              dc++,
                              dest_card = dest_below_card
                            )
                        {
                            dest_below_card = fcs_col_get_card(dest_col, dc+1);
                            if (!fcs_is_parent_card(dest_below_card, dest_card))
                            {
#if (!defined(HARD_CODED_NUM_DECKS) || (HARD_CODED_NUM_DECKS == 1))
#define INCREMENT_PTR_BY_NUM_DECKS() \
                                for(;(*ptr) != -1;ptr += (4 << 1)) \
                                { \
                                }
#else
#define INCREMENT_PTR_BY_NUM_DECKS() \ {}
#endif

#define ASSIGN_PTR(dest_stack, dest_col) \
                                ptr = &positions_by_rank[ \
                                    (FCS_POS_BY_RANK_WIDTH * \
                                     (fcs_card_card_num(dest_card)-1) \
                                    ) \
                                    + \
                                    (fcs_card_suit(dest_card)<<1) \
                                ]; \
                                INCREMENT_PTR_BY_NUM_DECKS() \
                                *(ptr++) = (char)(dest_stack); \
                                *(ptr) = (char)(dest_col)


                                ASSIGN_PTR(ds, dc);
                            }
                        }
                    }
                    ASSIGN_PTR(ds, top_card_idx);
                }
            }
        }

        *positions_by_rank_location = positions_by_rank;
    }

    return *positions_by_rank_location;
}
#ifdef FCS_RCS_STATES
#undef state_key
#endif

/*
 * These functions are used by the move functions in freecell.c and
 * simpsim.c.
 * */
int fc_solve_sfs_check_state_begin(
    fc_solve_hard_thread_t * hard_thread,
#ifdef FCS_RCS_STATES
    fcs_state_t * out_new_state_key,
#endif
    fcs_collectible_state_t * *  out_ptr_new_state,
#ifdef FCS_RCS_STATES
    fcs_state_t * ptr_state_key,
#endif
    fcs_collectible_state_t * ptr_state,
    fcs_move_stack_t * moves
    )
{
    fcs_collectible_state_t * ptr_new_state;
    fc_solve_instance_t * instance;

    instance = hard_thread->instance;

    if ((hard_thread->allocated_from_list =
        (instance->list_of_vacant_states != NULL)))
    {
        ptr_new_state = instance->list_of_vacant_states;
        instance->list_of_vacant_states = FCS_S_NEXT(instance->list_of_vacant_states);
    }
    else
    {
        ptr_new_state =
            fcs_state_ia_alloc_into_var(
                &(hard_thread->allocator)
            );
    }

    fcs_duplicate_state(
#ifdef FCS_RCS_STATES
            out_new_state_key,
#endif
            ptr_new_state,
#ifdef FCS_RCS_STATES
            ptr_state_key,
#endif
            ptr_state
    );
    /* Some BeFS and BFS parameters that need to be initialized in
     * the derived state.
     * */
    FCS_S_PARENT(ptr_new_state) = ptr_state;
    FCS_S_MOVES_TO_PARENT(ptr_new_state) = moves;
    /* Make sure depth is consistent with the game graph.
     * I.e: the depth of every newly discovered state is derived from
     * the state from which it was discovered. */
#ifndef FCS_WITHOUT_DEPTH_FIELD
    (FCS_S_DEPTH(ptr_new_state))++;
#endif
    /* Mark this state as a state that was not yet visited */
    FCS_S_VISITED(ptr_new_state) = 0;
    /* It's a newly created state which does not have children yet. */
    FCS_S_NUM_ACTIVE_CHILDREN(ptr_new_state) = 0;
    memset(&(FCS_S_SCAN_VISITED(ptr_new_state)), '\0',
        sizeof(FCS_S_SCAN_VISITED(ptr_new_state))
        );
    fcs_move_stack_reset(moves);

    *out_ptr_new_state = ptr_new_state;

    return 0;
}

extern void fc_solve_sfs_check_state_end(
    fc_solve_soft_thread_t * soft_thread,
#ifdef FCS_RCS_STATES
    fcs_state_t * ptr_state_key,
#endif
    fcs_collectible_state_t * ptr_state,
#ifdef FCS_RCS_STATES
    fcs_state_t * ptr_new_state_key,
#endif
    fcs_collectible_state_t * ptr_new_state,
    int state_context_value,
    fcs_move_stack_t * moves,
    fcs_derived_states_list_t * derived_states_list
    )
{
    fcs_internal_move_t temp_move;
    fc_solve_hard_thread_t * hard_thread;
    fc_solve_instance_t * instance;
    fcs_runtime_flags_t calc_real_depth;
    fcs_runtime_flags_t scans_synergy;
    fcs_collectible_state_t * existing_state;

#ifdef FCS_RCS_STATES
    fcs_state_t * existing_state_key;
#endif

    temp_move = fc_solve_empty_move;

    hard_thread = soft_thread->hard_thread;
    instance = hard_thread->instance;

    calc_real_depth = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_CALC_REAL_DEPTH);
    scans_synergy = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY);

    if (! fc_solve_check_and_add_state(
        hard_thread,
#ifdef FCS_RCS_STATES
        ptr_new_state_key,
#endif
        ptr_new_state,
#ifdef FCS_RCS_STATES
        &existing_state_key,
#endif
        &existing_state
        ))
    {
        if (hard_thread->allocated_from_list)
        {
            FCS_S_NEXT(ptr_new_state) = instance->list_of_vacant_states;
            instance->list_of_vacant_states = ptr_new_state;
        }
        else
        {
            fcs_compact_alloc_release(&(hard_thread->allocator));
        }

#ifndef FCS_WITHOUT_DEPTH_FIELD
        calculate_real_depth (calc_real_depth, existing_state);

        /* Re-parent the existing state to this one.
         *
         * What it means is that if the depth of the state if it
         * can be reached from this one is lower than what it
         * already have, then re-assign its parent to this state.
         * */
        if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_TO_REPARENT_STATES_REAL) &&
           (FCS_S_DEPTH(existing_state) > FCS_S_DEPTH(ptr_state)+1))
        {
            /* Make a copy of "moves" because "moves" will be destroyed */
            FCS_S_MOVES_TO_PARENT(existing_state) =
                fc_solve_move_stack_compact_allocate(
                    hard_thread, moves
                    );
            if (!(FCS_S_VISITED(existing_state) & FCS_VISITED_DEAD_END))
            {
                if ((--(FCS_S_NUM_ACTIVE_CHILDREN(FCS_S_PARENT(existing_state)))) == 0)
                {
                    mark_as_dead_end(scans_synergy, FCS_S_PARENT(existing_state));
                }
                FCS_S_NUM_ACTIVE_CHILDREN(ptr_state)++;
            }
            FCS_S_PARENT(existing_state) = ptr_state;
            FCS_S_DEPTH(existing_state) = FCS_S_DEPTH(ptr_state) + 1;
        }

#endif

        fc_solve_derived_states_list_add_state(
            derived_states_list,
            existing_state,
            state_context_value
        );

    }
    else
    {
        fc_solve_derived_states_list_add_state(
            derived_states_list,
            ptr_new_state,
            state_context_value
        );
    }

    return;
}

