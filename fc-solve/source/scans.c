/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * scans.c - The code that relates to the various scans.
 * Currently Hard DFS, Soft-DFS, Random-DFS, BeFS and BFS are implemented.
 */

#include "dll_thunk.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <assert.h>

#include "config.h"
#include "state.h"
#include "scans.h"
#include "meta_alloc.h"
#include "instance.h"
#include "move_stack_compact_alloc.h"
#include "freecell.h"
#include "simpsim.h"
#include "move_funcs_maps.h"

#if 0
#define DEBUG 1
#endif

#if 1
#undef DEBUG
#endif

#include "scans_impl.h"

/* GCC does not handle inline functions as well as macros. */
#define kv_calc_depth(ptr_state)                                               \
    calc_depth(FCS_STATE_kv_to_collectible(ptr_state))

#define SOFT_DFS_DEPTH_GROW_BY 16
void fc_solve_increase_dfs_max_depth(fc_solve_soft_thread_t *const soft_thread)
{
    const_AUTO(new_dfs_max_depth,
        DFS_VAR(soft_thread, dfs_max_depth) + SOFT_DFS_DEPTH_GROW_BY);

    DFS_VAR(soft_thread, soft_dfs_info) =
        SREALLOC(DFS_VAR(soft_thread, soft_dfs_info), new_dfs_max_depth);

    fcs_soft_dfs_stack_item_t *soft_dfs_info =
        DFS_VAR(soft_thread, soft_dfs_info) +
        DFS_VAR(soft_thread, dfs_max_depth);

    fcs_soft_dfs_stack_item_t *const end_soft_dfs_info =
        soft_dfs_info + SOFT_DFS_DEPTH_GROW_BY;
    for (; soft_dfs_info < end_soft_dfs_info; soft_dfs_info++)
    {
        *soft_dfs_info = (fcs_soft_dfs_stack_item_t){
            .state = NULL,
            .tests_list_index = 0,
            .move_func_idx = 0,
            .current_state_index = 0,
            .derived_states_list =
                {
                    .num_states = 0, .states = NULL,
                },
            .derived_states_random_indexes = NULL,
            .derived_states_random_indexes_max_size = 0,
        };
    }

    DFS_VAR(soft_thread, dfs_max_depth) = new_dfs_max_depth;
}

#ifdef FCS_RCS_STATES

/* TODO : Unit-test this function as it had had a bug beforehand
 * because fcs_lru_side_t had been an unsigned long.
 * */
typedef const char *fcs_lru_side_t;

extern int fc_solve_compare_lru_cache_keys(const void *const void_a,
    const void *const void_b, void *const context GCC_UNUSED)
{
#define GET_PARAM(p)                                                           \
    ((fcs_lru_side_t)(((const fcs_cache_key_info_t *)(p))->val_ptr))
    fcs_lru_side_t a = GET_PARAM(void_a);
    fcs_lru_side_t b = GET_PARAM(void_b);

    return ((a > b) ? 1 : (a < b) ? (-1) : 0);
#undef GET_PARAM
}

#define NEXT_CACHE_STATE(s) ((s)->lower_pri)
fcs_state_t *fc_solve_lookup_state_key_from_val(
    fc_solve_instance_t *const instance,
    fcs_collectible_state_t *const orig_ptr_state_val)
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    PWord_t PValue;
#endif
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    fcs_lru_cache_t *cache = &(instance->rcs_states_cache);

    ssize_t parents_stack_len = 1;
    ssize_t parents_stack_max_len = 16;

    struct
    {
        fcs_cache_key_info_t *new_cache_state;
        fcs_collectible_state_t *state_val;
    } *parents_stack = SMALLOC(parents_stack, parents_stack_max_len);

    parents_stack[0].state_val = orig_ptr_state_val;

    fcs_cache_key_info_t *new_cache_state;
    while (1)
    {
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
        JLI(PValue, cache->states_values_to_keys_map,
            ((Word_t)parents_stack[parents_stack_len - 1].state_val));
        if (*PValue)
        {
            parents_stack[parents_stack_len - 1].new_cache_state =
                new_cache_state = (fcs_cache_key_info_t *)(*PValue);
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
                new_cache_state = fcs_compact_alloc_ptr(
                    &(cache->states_values_to_keys_allocator),
                    sizeof(*new_cache_state));
            }
        }
#else
        {
            if (cache->recycle_bin)
            {
                new_cache_state = cache->recycle_bin;
                cache->recycle_bin = NEXT_CACHE_STATE(new_cache_state);
            }
            else
            {
                new_cache_state = fcs_compact_alloc_ptr(
                    &(cache->states_values_to_keys_allocator),
                    sizeof(*new_cache_state));
            }

            new_cache_state->val_ptr =
                parents_stack[parents_stack_len - 1].state_val;
            fcs_cache_key_info_t *const existing_cache_state =
                (fcs_cache_key_info_t *)fc_solve_kaz_tree_alloc_insert(
                    cache->kaz_tree, new_cache_state);

            if (existing_cache_state)
            {
                NEXT_CACHE_STATE(new_cache_state) = cache->recycle_bin;
                cache->recycle_bin = new_cache_state;

                parents_stack[parents_stack_len - 1].new_cache_state =
                    new_cache_state = existing_cache_state;
                break;
            }
        }
#endif

        parents_stack[parents_stack_len - 1].new_cache_state = new_cache_state;

#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
        *PValue = ((Word_t)new_cache_state);

        new_cache_state->val_ptr =
            parents_stack[parents_stack_len - 1].state_val;
#endif

        new_cache_state->lower_pri = new_cache_state->higher_pri = NULL;

        cache->count_elements_in_cache++;

        if (!FCS_S_PARENT(parents_stack[parents_stack_len - 1].state_val))
        {
            new_cache_state->key = instance->state_copy_ptr->s;
            break;
        }
        else
        {
            parents_stack[parents_stack_len].state_val =
                FCS_S_PARENT(parents_stack[parents_stack_len - 1].state_val);
            if (++parents_stack_len == parents_stack_max_len)
            {
                parents_stack_max_len += 16;
                parents_stack = SREALLOC(parents_stack, parents_stack_max_len);
            }
        }
    }

    for (parents_stack_len--; parents_stack_len > 0; parents_stack_len--)
    {

        new_cache_state = parents_stack[parents_stack_len - 1].new_cache_state;

        fcs_state_t *const pass_key = &(new_cache_state->key);
        *pass_key = parents_stack[parents_stack_len].new_cache_state->key;

        const fcs_move_stack_t *const stack_ptr__moves_to_parent =
            parents_stack[parents_stack_len - 1].state_val->moves_to_parent;
        const fcs_internal_move_t *next_move =
            stack_ptr__moves_to_parent->moves;
        const fcs_internal_move_t *const moves_end =
            (next_move + stack_ptr__moves_to_parent->num_moves);

        for (; next_move < moves_end; next_move++)
        {

            fc_solve_apply_move(pass_key, NULL,
                (*next_move)PASS_FREECELLS(LOCAL_FREECELLS_NUM)
                    PASS_STACKS(LOCAL_STACKS_NUM));
        }
        /* The state->parent_state moves stack has an implicit canonize
         * suffix move. */
        fc_solve_canonize_state(pass_key PASS_FREECELLS(LOCAL_FREECELLS_NUM)
                PASS_STACKS(LOCAL_STACKS_NUM));

        /* Promote new_cache_state to the head of the priority list. */
        if (!cache->lowest_pri)
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
            /* Bug fix: make sure that ->lowest_pri is always valid. */
            else if (new_cache_state->higher_pri)
            {
                cache->lowest_pri = new_cache_state->higher_pri;
            }

            /* Now promote it to be the highest. */
            cache->highest_pri->higher_pri = new_cache_state;
            new_cache_state->lower_pri = cache->highest_pri;
            new_cache_state->higher_pri = NULL;
            cache->highest_pri = new_cache_state;
        }
    }

    free(parents_stack);

    if (cache->count_elements_in_cache > cache->max_num_elements_in_cache)
    {
        long count = cache->count_elements_in_cache;
        long limit = cache->max_num_elements_in_cache;

        while (count > limit)
        {
            fcs_cache_key_info_t *lowest_pri = cache->lowest_pri;
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
            int rc_int;
            JLD(rc_int, cache->states_values_to_keys_map,
                (Word_t)(lowest_pri->val_ptr));
#else
            fc_solve_kaz_tree_delete_free(cache->kaz_tree,
                fc_solve_kaz_tree_lookup(cache->kaz_tree, lowest_pri));
#endif

            cache->lowest_pri = lowest_pri->higher_pri;
            cache->lowest_pri->lower_pri = NULL;

            NEXT_CACHE_STATE(lowest_pri) = cache->recycle_bin;

            cache->recycle_bin = lowest_pri;
            count--;
        }

        cache->count_elements_in_cache = count;
    }

    return &(new_cache_state->key);
}

#undef NEXT_CACHE_STATE

#endif

#ifdef DEBUG

#define TRACE0(message)                                                        \
    {                                                                          \
        if (getenv("FCS_TRACE"))                                               \
        {                                                                      \
            printf("BestFS - %s ; Iters=%ld.\n", message,                      \
                (long)(*instance_num_checked_states_ptr));                     \
            fflush(stdout);                                                    \
        }                                                                      \
    }

#else

#define TRACE0(no_use)                                                         \
    {                                                                          \
    }

#endif

#define my_brfs_queue (BRFS_VAR(soft_thread, bfs_queue))
#define my_brfs_queue_last_item (BRFS_VAR(soft_thread, bfs_queue_last_item))
#define my_brfs_recycle_bin (BRFS_VAR(soft_thread, recycle_bin))

#define NEW_BRFS_QUEUE_ITEM()                                                  \
    ((fcs_states_linked_list_item_t *)fcs_compact_alloc_ptr(                   \
        &(HT_FIELD(hard_thread, allocator)),                                   \
        sizeof(fcs_states_linked_list_item_t)));

static inline void fc_solve_initialize_bfs_queue(
    fc_solve_soft_thread_t *const soft_thread)
{
    fc_solve_hard_thread_t *const hard_thread = soft_thread->hard_thread;

    /* Initialize the BFS queue. We have one dummy element at the beginning
       in order to make operations simpler. */
    my_brfs_queue = NEW_BRFS_QUEUE_ITEM();

    my_brfs_queue_last_item = my_brfs_queue->next = NEW_BRFS_QUEUE_ITEM();

    my_brfs_queue_last_item->next = NULL;

    my_brfs_recycle_bin = NULL;

    return;
}

void fc_solve_soft_thread_init_befs_or_bfs(
    fc_solve_soft_thread_t *const soft_thread)
{
    fc_solve_soft_thread_update_initial_cards_val(soft_thread);

    if (soft_thread->is_befs)
    {
#define WEIGHTING(soft_thread) (&(BEFS_VAR(soft_thread, weighting)))
        /* Initialize the priotity queue of the BeFS scan */
        fc_solve_pq_init(&(BEFS_VAR(soft_thread, pqueue)));
        fc_solve_initialize_befs_rater(soft_thread, WEIGHTING(soft_thread));
    }
    else
    {
        fc_solve_initialize_bfs_queue(soft_thread);
    }

    if (!BEFS_M_VAR(soft_thread, tests_list))
    {
        size_t num = 0;
        fc_solve_solve_for_state_move_func_t *tests_list = NULL;

        for (size_t group_idx = 0;
             group_idx < soft_thread->by_depth_tests_order.by_depth_tests[0]
                             .tests_order.num_groups;
             group_idx++)
        {
            add_to_move_funcs_list(&tests_list, &num,
                soft_thread->by_depth_tests_order.by_depth_tests[0]
                    .tests_order.groups[group_idx]
                    .order_group_tests,
                soft_thread->by_depth_tests_order.by_depth_tests[0]
                    .tests_order.groups[group_idx]
                    .num);
        }
        BEFS_M_VAR(soft_thread, tests_list) = tests_list;
        BEFS_M_VAR(soft_thread, tests_list_end) = tests_list + num;
    }
    BEFS_M_VAR(soft_thread, first_state_to_check) =
        FCS_STATE_keyval_pair_to_collectible(
            HT_INSTANCE(soft_thread->hard_thread)->state_copy_ptr);
}

/*
 *  fc_solve_befs_or_bfs_do_solve() is the main event
 *  loop of the BeFS And BFS scans. It is quite simple as all it does is
 *  extract elements out of the queue or priority queue and run all the test
 *  of them.
 *
 *  It goes on in this fashion until the final state was reached or
 *  there are no more states in the queue.
*/
int fc_solve_befs_or_bfs_do_solve(fc_solve_soft_thread_t *const soft_thread)
{
    fc_solve_hard_thread_t *const hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t *const instance = HT_INSTANCE(hard_thread);

#ifndef FCS_DISABLE_SIMPLE_SIMON
    const fcs_bool_t is_simple_simon = instance->is_simple_simon;
#endif
#if !defined(FCS_WITHOUT_DEPTH_FIELD) &&                                       \
    !defined(FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE)
    const fcs_bool_t calc_real_depth = fcs_get_calc_real_depth(instance);
#endif
#ifndef FCS_HARD_CODE_SCANS_SYNERGY_AS_TRUE
    const fcs_bool_t scans_synergy =
        STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY);
#endif
    const_AUTO(soft_thread_id, soft_thread->id);
    const fcs_bool_t is_a_complete_scan =
        STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN);
    const_SLOT(effective_max_num_states_in_collection, instance);

    fcs_states_linked_list_item_t *queue = NULL;
    fcs_states_linked_list_item_t *queue_last_item = NULL;
    pri_queue_t *pqueue = NULL;

    int error_code;

    fcs_int_limit_t hard_thread_max_num_checked_states;

    fcs_derived_states_list_t derived;
    derived.num_states = 0;
    derived.states = NULL;

    const fc_solve_solve_for_state_move_func_t *const tests_list =
        BEFS_M_VAR(soft_thread, tests_list);
    const fc_solve_solve_for_state_move_func_t *const tests_list_end =
        BEFS_M_VAR(soft_thread, tests_list_end);

    DECLARE_STATE();
    ASSIGN_ptr_state(BEFS_M_VAR(soft_thread, first_state_to_check));
    const fcs_bool_t enable_pruning = soft_thread->enable_pruning;

    fcs_int_limit_t *const instance_num_checked_states_ptr =
        &(instance->i__num_checked_states);
#ifndef FCS_SINGLE_HARD_THREAD
    fcs_int_limit_t *const hard_thread_num_checked_states_ptr =
        &(HT_FIELD(hard_thread, ht__num_checked_states));
#endif
    const_SLOT(is_befs, soft_thread);
#ifdef FCS_WITH_MOVES
    const_SLOT(is_optimize_scan, soft_thread);
#endif

    if (is_befs)
    {
        pqueue = &(BEFS_VAR(soft_thread, pqueue));
    }
    else
    {
        queue = my_brfs_queue;
        queue_last_item = my_brfs_queue_last_item;
    }

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    CALC_HARD_THREAD_MAX_NUM_CHECKED_STATES();

#ifndef FCS_WITHOUT_ITER_HANDLER
    const fcs_instance_debug_iter_output_func_t debug_iter_output_func =
        instance->debug_iter_output_func;
    const fcs_instance_debug_iter_output_context_t debug_iter_output_context =
        instance->debug_iter_output_context;
#endif

    /* Continue as long as there are states in the queue or
       priority queue. */
    int8_t *const befs_positions_by_rank =
        (BEFS_M_VAR(soft_thread, befs_positions_by_rank));

    while (PTR_STATE != NULL)
    {
        TRACE0("Start of loop");
        /*
         * If we do the pruning after checking for being visited, then
         * there's a risk of inconsistent result when being interrupted
         * because we check once for the pruned state (after the scan
         * was suspended) and another time for the uninterrupted state.
         *
         * Therefore, we prune before checking for the visited flags.
         * */
        TRACE0("Pruning");
        if (fcs__should_state_be_pruned(enable_pruning, PTR_STATE))
        {
            fcs_collectible_state_t *const after_pruning_state =
                fc_solve_sfs_raymond_prune(soft_thread, &pass);
            if (after_pruning_state)
            {
                ASSIGN_ptr_state(after_pruning_state);
            }
        }

        {
            register const int temp_visited = FCS_S_VISITED(PTR_STATE);

            /*
             * If this is an optimization scan and the state being checked is
             * not in the original solution path - move on to the next state
             * */
            /*
             * It the state has already been visited - move on to the next
             * state.
             * */
            if (
#ifdef FCS_WITH_MOVES
                is_optimize_scan
                    ? ((!(temp_visited & FCS_VISITED_IN_SOLUTION_PATH)) ||
                          (temp_visited & FCS_VISITED_IN_OPTIMIZED_PATH))
                    :
#endif
                    ((temp_visited & FCS_VISITED_DEAD_END) ||
                        (is_scan_visited(PTR_STATE, soft_thread_id))))
            {
                goto next_state;
            }
        }

        TRACE0("Counting cells");

        if (check_if_limits_exceeded())
        {
            BEFS_M_VAR(soft_thread, first_state_to_check) = PTR_STATE;

            TRACE0("error_code - FCS_STATE_SUSPEND_PROCESS");
            error_code = FCS_STATE_SUSPEND_PROCESS;
            goto my_return_label;
        }

#ifndef FCS_WITHOUT_ITER_HANDLER
        TRACE0("debug_iter_output");
        if (debug_iter_output_func)
        {
            debug_iter_output_func(
                debug_iter_output_context, *(instance_num_checked_states_ptr),
                calc_depth(PTR_STATE), (void *)instance, &pass,
#ifdef FCS_WITHOUT_VISITED_ITER
                0
#else
                ((FCS_S_PARENT(PTR_STATE) == NULL)
                        ? 0
                        : FCS_S_VISITED_ITER(FCS_S_PARENT(PTR_STATE)))
#endif
                );
        }
#endif

        const fcs_game_limit_t num_vacant_freecells =
            count_num_vacant_freecells(
                LOCAL_FREECELLS_NUM, &FCS_SCANS_the_state);
        const fcs_game_limit_t num_vacant_stacks =
            count_num_vacant_stacks(LOCAL_STACKS_NUM, &FCS_SCANS_the_state);
        if ((num_vacant_stacks == LOCAL_STACKS_NUM) &&
            (num_vacant_freecells == LOCAL_FREECELLS_NUM))
        {
            instance->final_state = PTR_STATE;

            BUMP_NUM_CHECKED_STATES();

            error_code = FCS_STATE_WAS_SOLVED;
            goto my_return_label;
        }

        calculate_real_depth(calc_real_depth, PTR_STATE);

        soft_thread->num_vacant_freecells = num_vacant_freecells;
        soft_thread->num_vacant_stacks = num_vacant_stacks;
        fc_solve__calc_positions_by_rank_data(
            soft_thread, &FCS_SCANS_the_state, befs_positions_by_rank
#ifndef FCS_DISABLE_SIMPLE_SIMON
            ,
            is_simple_simon
#endif
            );

        TRACE0("perform_tests");

        /*
         * Do all the tests at one go, because that is the way it should be
         * done for BFS and BeFS.
        */
        derived.num_states = 0;
        for (const fc_solve_solve_for_state_move_func_t *move_func_ptr =
                 tests_list;
             move_func_ptr < tests_list_end; move_func_ptr++)
        {
            (*move_func_ptr)(soft_thread, &pass, &derived);
        }

        if (is_a_complete_scan)
        {
            FCS_S_VISITED(PTR_STATE) |= FCS_VISITED_ALL_TESTS_DONE;
        }

        /* Increase the number of iterations by one .
         * */
        BUMP_NUM_CHECKED_STATES();

        TRACE0("Insert all states");
        /* Insert all the derived states into the PQ or Queue */
        fcs_derived_states_list_item_t *derived_iter;
        fcs_derived_states_list_item_t *derived_end;
        for (derived_end = (derived_iter = derived.states) + derived.num_states;
             derived_iter < derived_end; derived_iter++)
        {
            const_AUTO(scans_ptr_new_state, derived_iter->state_ptr);
            if (is_befs)
            {
#ifdef FCS_RCS_STATES
                fcs_kv_state_t new_pass = {
                    .key = fc_solve_lookup_state_key_from_val(
                        instance, scans_ptr_new_state),
                    .val = scans_ptr_new_state};
#else
                fcs_kv_state_t new_pass =
                    FCS_STATE_keyval_pair_to_kv(scans_ptr_new_state);
#endif
                fc_solve_pq_push(pqueue, scans_ptr_new_state,
                    befs_rate_state(soft_thread, WEIGHTING(soft_thread),
                        new_pass.key,
                        BEFS_MAX_DEPTH - kv_calc_depth(&(new_pass))));
            }
            else
            {
                /* Enqueue the new state. */
                fcs_states_linked_list_item_t *last_item_next;

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

                queue_last_item->s = scans_ptr_new_state;
                last_item_next->next = NULL;
                queue_last_item = last_item_next;
            }
        }

#ifdef FCS_WITH_MOVES
        if (is_optimize_scan)
        {
            FCS_S_VISITED(PTR_STATE) |= FCS_VISITED_IN_OPTIMIZED_PATH;
        }
        else
#endif
        {
            set_scan_visited(PTR_STATE, soft_thread_id);

            if (derived.num_states == 0)
            {
                if (is_a_complete_scan)
                {
                    MARK_AS_DEAD_END(PTR_STATE);
                }
            }
        }

#ifndef FCS_WITHOUT_VISITED_ITER
        FCS_S_VISITED_ITER(PTR_STATE) = *(instance_num_checked_states_ptr)-1;
#endif

    next_state:
        TRACE0("Label next state");
        /*
            Extract the next item in the queue/priority queue.
        */
        {
            fcs_collectible_state_t *new_ptr_state;
            if (is_befs)
            {
                /* It is an BeFS scan */
                fc_solve_pq_pop(pqueue, &(new_ptr_state));
            }
            else
            {
                const_AUTO(save_item, queue->next);
                if (save_item != queue_last_item)
                {
                    new_ptr_state = save_item->s;
                    queue->next = save_item->next;
                    save_item->next = my_brfs_recycle_bin;
                    my_brfs_recycle_bin = save_item;
                }
                else
                {
                    new_ptr_state = NULL;
                }
            }
            ASSIGN_ptr_state(new_ptr_state);
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

    if (!is_befs)
    {
        my_brfs_queue_last_item = queue_last_item;
    }

    return error_code;
}

/*
 * These functions are used by the move functions in freecell.c and
 * simpsim.c.
 * */
int fc_solve_sfs_check_state_begin(fc_solve_hard_thread_t *const hard_thread,
    fcs_kv_state_t *const out_new_state_out,
    fcs_kv_state_t *const raw_ptr_state_raw SFS__PASS_MOVE_STACK(
        fcs_move_stack_t *const moves))
{
    fcs_collectible_state_t *raw_ptr_new_state;
    fc_solve_instance_t *const instance = HT_INSTANCE(hard_thread);

    if ((HT_FIELD(hard_thread, allocated_from_list) =
                (instance->list_of_vacant_states != NULL)))
    {
        raw_ptr_new_state = instance->list_of_vacant_states;
        instance->list_of_vacant_states =
            FCS_S_NEXT(instance->list_of_vacant_states);
    }
    else
    {
        raw_ptr_new_state =
            fcs_state_ia_alloc_into_var(&(HT_FIELD(hard_thread, allocator)));
    }

    FCS_STATE_collectible_to_kv(out_new_state_out, raw_ptr_new_state);
    fcs_duplicate_kv_state(out_new_state_out, raw_ptr_state_raw);
#ifdef FCS_RCS_STATES
#define INFO_STATE_PTR(kv_ptr) ((kv_ptr)->val)
#else
/* TODO : That's very hacky - get rid of it. */
#define INFO_STATE_PTR(kv_ptr) ((fcs_state_keyval_pair_t *)((kv_ptr)->key))
#endif
    /* Some BeFS and BFS parameters that need to be initialized in
     * the derived state.
     * */
    FCS_S_PARENT(raw_ptr_new_state) = INFO_STATE_PTR(raw_ptr_state_raw);
#ifdef FCS_WITH_MOVES
    FCS_S_MOVES_TO_PARENT(raw_ptr_new_state) = moves;
#endif
/* Make sure depth is consistent with the game graph.
 * I.e: the depth of every newly discovered state is derived from
 * the state from which it was discovered. */
#ifndef FCS_WITHOUT_DEPTH_FIELD
    (FCS_S_DEPTH(raw_ptr_new_state))++;
#endif
    /* Mark this state as a state that was not yet visited */
    FCS_S_VISITED(raw_ptr_new_state) = 0;
    /* It's a newly created state which does not have children yet. */
    FCS_S_NUM_ACTIVE_CHILDREN(raw_ptr_new_state) = 0;
    memset(&(FCS_S_SCAN_VISITED(raw_ptr_new_state)), '\0',
        sizeof(FCS_S_SCAN_VISITED(raw_ptr_new_state)));
    fcs_move_stack_reset(moves);

    return 0;
}

extern void fc_solve_sfs_check_state_end(
    fc_solve_soft_thread_t *const soft_thread,
    fcs_kv_state_t *const raw_ptr_state_raw,
    fcs_kv_state_t *const raw_ptr_new_state_raw, const int state_context_value,
    fcs_move_stack_t *const moves,
    fcs_derived_states_list_t *const derived_states_list)
{
    const_SLOT(hard_thread, soft_thread);
    const_AUTO(instance, HT_INSTANCE(hard_thread));
#if !defined(FCS_WITHOUT_DEPTH_FIELD) &&                                       \
    !defined(FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE)
    const fcs_bool_t calc_real_depth = fcs_get_calc_real_depth(instance);
#endif
#if !defined(FCS_HARD_CODE_REPARENT_STATES_AS_FALSE) &&                        \
    !defined(FCS_HARD_CODE_SCANS_SYNERGY_AS_TRUE)
    const fcs_bool_t scans_synergy =
        STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY);
#endif
    fcs_kv_state_t existing_state;

#define ptr_new_state_foo (raw_ptr_new_state_raw->val)
#define ptr_state (raw_ptr_state_raw->val)

    if (!fc_solve_check_and_add_state(
            hard_thread, raw_ptr_new_state_raw, &existing_state))
    {
#define existing_state_val (existing_state.val)
        if (HT_FIELD(hard_thread, allocated_from_list))
        {
            ptr_new_state_foo->parent = instance->list_of_vacant_states;
            instance->list_of_vacant_states =
                INFO_STATE_PTR(raw_ptr_new_state_raw);
        }
        else
        {
            fcs_compact_alloc_release(&(HT_FIELD(hard_thread, allocator)));
        }

#ifndef FCS_WITHOUT_DEPTH_FIELD
        calculate_real_depth(
            calc_real_depth, FCS_STATE_kv_to_collectible(&existing_state));
#endif

/* Re-parent the existing state to this one.
 *
 * What it means is that if the depth of the state if it
 * can be reached from this one is lower than what it
 * already have, then re-assign its parent to this state.
 * */
#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
        if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_TO_REPARENT_STATES_REAL) &&
            (kv_calc_depth(&existing_state) >
                kv_calc_depth(raw_ptr_state_raw) + 1))
        {
#ifdef DEBUG
            if (getenv("FCS_TRACE"))
            {
                printf("AppleBloom %ld %ld\n",
                    (long)kv_calc_depth(&existing_state),
                    (long)kv_calc_depth(raw_ptr_state_raw));
            }
#endif
#ifdef FCS_WITH_MOVES
            /* Make a copy of "moves" because "moves" will be destroyed */
            existing_state_val->moves_to_parent =
                fc_solve_move_stack_compact_allocate(hard_thread, moves);
#endif
            if (!(existing_state_val->visited & FCS_VISITED_DEAD_END))
            {
                if ((--(FCS_S_NUM_ACTIVE_CHILDREN(
                        existing_state_val->parent))) == 0)
                {
                    MARK_AS_DEAD_END(existing_state_val->parent);
                }
                ptr_state->num_active_children++;
            }
            existing_state_val->parent = INFO_STATE_PTR(raw_ptr_state_raw);
#ifndef FCS_WITHOUT_DEPTH_FIELD
            existing_state_val->depth = ptr_state->depth + 1;
#endif
        }
#endif

        fc_solve_derived_states_list_add_state(derived_states_list,
            FCS_STATE_kv_to_collectible(&existing_state), state_context_value);
    }
    else
    {
        fc_solve_derived_states_list_add_state(derived_states_list,
            INFO_STATE_PTR(raw_ptr_new_state_raw), state_context_value);
    }

    return;
}
