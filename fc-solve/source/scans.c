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
#include "scans.h"
#include "meta_alloc.h"
#include "instance.h"
#include "move_stack_compact_alloc.h"
#include "freecell.h"
#include "simpsim.h"
#include "move_funcs_maps.h"

#include "inline.h"
#include "likely.h"
#include "bool.h"
#include "min_and_max.h"

#if 0
#define DEBUG 1
#endif

#if 1
#undef DEBUG
#endif

#include "scans_impl.h"


static GCC_INLINE const fcs_depth_t kv_calc_depth(fcs_kv_state_t * const ptr_state)
{
    return calc_depth(FCS_STATE_kv_to_collectible(ptr_state));
}

#define SOFT_DFS_DEPTH_GROW_BY 16
void fc_solve_increase_dfs_max_depth(
    fc_solve_soft_thread_t * const soft_thread
    )
{
    const int new_dfs_max_depth = DFS_VAR(soft_thread, dfs_max_depth) + SOFT_DFS_DEPTH_GROW_BY;

    DFS_VAR(soft_thread, soft_dfs_info) = SREALLOC(
        DFS_VAR(soft_thread, soft_dfs_info),
        new_dfs_max_depth
        );

    fcs_soft_dfs_stack_item_t * soft_dfs_info = DFS_VAR(soft_thread, soft_dfs_info) +
        DFS_VAR(soft_thread, dfs_max_depth);

    fcs_soft_dfs_stack_item_t * const end_soft_dfs_info = soft_dfs_info + SOFT_DFS_DEPTH_GROW_BY;
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

    DFS_VAR(soft_thread, dfs_max_depth) = new_dfs_max_depth;
}


#ifdef FCS_RCS_STATES

/* TODO : Unit-test this function as it had had a bug beforehand
 * because fcs_lru_side_t had been an unsigned long.
 * */
typedef const char * fcs_lru_side_t;

extern int fc_solve_compare_lru_cache_keys(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((fcs_lru_side_t)(((const fcs_cache_key_info_t *)(p))->val_ptr))
    fcs_lru_side_t a = GET_PARAM(void_a);
    fcs_lru_side_t b = GET_PARAM(void_b);

    return ((a > b) ? 1 : (a < b) ? (-1) : 0);
#undef GET_PARAM
}

#define NEXT_CACHE_STATE(s) ((s)->lower_pri)
fcs_state_t * fc_solve_lookup_state_key_from_val(
    fc_solve_instance_t * const instance,
    fcs_collectible_state_t * const orig_ptr_state_val
)
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    PWord_t PValue;
#endif
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    fcs_lru_cache_t * cache = &(instance->rcs_states_cache);

    int parents_stack_len = 1;
    int parents_stack_max_len = 16;

    struct
    {
        fcs_cache_key_info_t * new_cache_state;
        fcs_collectible_state_t * state_val;
    } * parents_stack = SMALLOC(parents_stack, parents_stack_max_len);

    parents_stack[0].state_val = orig_ptr_state_val;

    fcs_cache_key_info_t * new_cache_state;
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
                        SREALLOC(
                            parents_stack,
                            parents_stack_max_len
                        );
                }
            }
    }

    for (parents_stack_len--; parents_stack_len > 0; parents_stack_len--)
    {
        fcs_kv_state_t pass, src_pass;

        fcs_collectible_state_t temp_new_state_val;
        fcs_internal_move_t * next_move;

        new_cache_state = parents_stack[parents_stack_len-1].new_cache_state;


        pass.key = &(new_cache_state->key);
        pass.val = &(temp_new_state_val);
        src_pass.key = &(parents_stack[parents_stack_len].new_cache_state->key);
        src_pass.val = parents_stack[parents_stack_len].state_val;

        fcs_duplicate_state( &pass, &src_pass);

        fcs_move_stack_t * const stack_ptr__moves_to_parent =
            parents_stack[parents_stack_len-1].state_val->moves_to_parent;
        fcs_internal_move_t * const moves_end =
        (
            (next_move = stack_ptr__moves_to_parent->moves)
            +
            stack_ptr__moves_to_parent->num_moves
        );


        for ( ;
            next_move < moves_end
            ; next_move++)
        {

            fc_solve_apply_move(
                &pass,
                NULL,
                (*next_move),
                LOCAL_FREECELLS_NUM,
                LOCAL_STACKS_NUM,
                LOCAL_DECKS_NUM
            );
        }
        /* The state->parent_state moves stack has an implicit canonize
         * suffix move. */
        fc_solve_canonize_state(
            &(pass),
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



#ifdef DEBUG

#define TRACE0(message) \
        { \
            if (getenv("FCS_TRACE")) \
            { \
            printf("BestFS - %s ; Iters=%ld.\n", \
                    message, \
                    (long)(instance->num_checked_states) \
                    );  \
            fflush(stdout); \
            } \
        }

#else

#define TRACE0(no_use) {}

#endif

#define my_brfs_queue (BRFS_VAR(soft_thread, bfs_queue))
#define my_brfs_queue_last_item \
    (BRFS_VAR(soft_thread, bfs_queue_last_item))
#define my_brfs_recycle_bin (BRFS_VAR(soft_thread, recycle_bin))

#define NEW_BRFS_QUEUE_ITEM() \
    ((fcs_states_linked_list_item_t *) \
    fcs_compact_alloc_ptr( \
        &(HT_FIELD(hard_thread, allocator)), \
        sizeof(fcs_states_linked_list_item_t) \
    ));

static GCC_INLINE void fc_solve_initialize_bfs_queue(fc_solve_soft_thread_t * const soft_thread)
{
    fc_solve_hard_thread_t * const hard_thread = soft_thread->hard_thread;

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



void fc_solve_soft_thread_init_befs_or_bfs(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_soft_thread_update_initial_cards_val(soft_thread);

    fc_solve_instance_t * const instance = HT_INSTANCE(soft_thread->hard_thread);

    if (soft_thread->method == FCS_METHOD_A_STAR)
    {
        /* Initialize the priotity queue of the BeFS scan */
        fc_solve_pq_init(
            &(BEFS_VAR(soft_thread, pqueue)),
            1024
        );

#define WEIGHTING(soft_thread) (&(BEFS_VAR(soft_thread, weighting)))

        fc_solve_initialize_befs_rater(
            soft_thread,
            WEIGHTING(soft_thread)
            );
    }
    else
    {
        fc_solve_initialize_bfs_queue(soft_thread);
    }

    if (! BEFS_M_VAR(soft_thread, tests_list))
    {
        int num = 0;
        fc_solve_solve_for_state_move_func_t * tests_list =
            SMALLOC(tests_list, 1);

        for (int group_idx = 0 ; group_idx < soft_thread->by_depth_tests_order.by_depth_tests[0].tests_order.num_groups ; group_idx++)
        {
            int * const tests_order_tests = soft_thread->by_depth_tests_order.by_depth_tests[0].tests_order.groups[group_idx].order_group_tests;

            const int tests_order_num = soft_thread->by_depth_tests_order.by_depth_tests[0].tests_order.groups[group_idx].num;


            {
                for (int i = 0 ; i < tests_order_num ; i++)
                {
                    tests_list = SREALLOC(tests_list, ++num);
                    tests_list[num-1] =
                        fc_solve_sfs_move_funcs[
                            tests_order_tests[i]
                        ];
                }
            }
        }
        BEFS_M_VAR(soft_thread, tests_list) = tests_list;
        BEFS_M_VAR(soft_thread, tests_list_end) = tests_list+num;
    }

    soft_thread->first_state_to_check =
        FCS_STATE_keyval_pair_to_collectible(instance->state_copy_ptr);

    return;
}

#ifdef DEBUG
#if 0
static void dump_pqueue (
    fc_solve_soft_thread_t * const soft_thread,
    const char * const stage_id,
    PQUEUE * const pq
    )
{
    int i;
    char * s;
    fc_solve_instance_t * instance = HT_INSTANCE(soft_thread->hard_thread);

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


/*
 *  fc_solve_befs_or_bfs_do_solve() is the main event
 *  loop of the BeFS And BFS scans. It is quite simple as all it does is
 *  extract elements out of the queue or priority queue and run all the test
 *  of them.
 *
 *  It goes on in this fashion until the final state was reached or
 *  there are no more states in the queue.
*/
int fc_solve_befs_or_bfs_do_solve( fc_solve_soft_thread_t * const soft_thread )
{
    fc_solve_hard_thread_t * const hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * const instance = HT_INSTANCE(hard_thread);

    DECLARE_NEW_STATE();

#ifndef FCS_WITHOUT_DEPTH_FIELD
    const fcs_runtime_flags_t calc_real_depth = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_CALC_REAL_DEPTH);
#endif
    const fcs_runtime_flags_t scans_synergy = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY);
    const int soft_thread_id = soft_thread->id;
    const fcs_runtime_flags_t is_a_complete_scan = STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN);
    const typeof(instance->effective_max_num_states_in_collection) effective_max_num_states_in_collection = instance->effective_max_num_states_in_collection;

    fcs_states_linked_list_item_t * queue = NULL;
    fcs_states_linked_list_item_t * queue_last_item = NULL;
    PQUEUE * pqueue = NULL;

    int error_code;

    fcs_int_limit_t hard_thread_max_num_checked_states;

    fcs_derived_states_list_t derived;
    derived.num_states = 0;
    derived.states = NULL;

    const fc_solve_solve_for_state_move_func_t * const tests_list
        = BEFS_M_VAR(soft_thread, tests_list);
    const fc_solve_solve_for_state_move_func_t * const tests_list_end
        = BEFS_M_VAR(soft_thread, tests_list_end);

    DECLARE_STATE();
    ASSIGN_ptr_state(soft_thread->first_state_to_check);
    const fcs_bool_t enable_pruning = soft_thread->enable_pruning;

    const int method = soft_thread->method;
    fcs_int_limit_t * const instance_num_checked_states_ptr = &(instance->i__num_checked_states);
#ifndef FCS_SINGLE_HARD_THREAD
    fcs_int_limit_t * const hard_thread_num_checked_states_ptr = &(HT_FIELD(hard_thread, ht__num_checked_states));
#endif

    if (method == FCS_METHOD_A_STAR)
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

    const fcs_instance_debug_iter_output_func_t debug_iter_output_func = instance->debug_iter_output_func;
    const fcs_instance_debug_iter_output_context_t debug_iter_output_context = instance->debug_iter_output_context;

    /* Continue as long as there are states in the queue or
       priority queue. */
    fcs_states_linked_list_item_t * save_item;
    while ( PTR_STATE != NULL)
    {
        TRACE0("Start of loop");

#ifdef DEBUG
        dump_pqueue(soft_thread, "loop_start", pqueue);
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
        if (fcs__should_state_be_pruned(enable_pruning, PTR_STATE))
        {
            fcs_collectible_state_t * after_pruning_state;

            if (fc_solve_sfs_raymond_prune(
                    soft_thread,
                    STATE_TO_PASS(),
                    &after_pruning_state
                ) == PRUNE_RET_FOLLOW_STATE
            )
            {
                ASSIGN_ptr_state(after_pruning_state);
            }
        }

        {
             register int temp_visited = FCS_S_VISITED(PTR_STATE);

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
                        (is_scan_visited(PTR_STATE, soft_thread_id))
                    )
                )
            {
                goto next_state;
            }
        }

        TRACE0("Counting cells");

        const fcs_game_limit_t num_vacant_freecells =
            count_num_vacant_freecells(LOCAL_FREECELLS_NUM, &FCS_SCANS_the_state);

        const fcs_game_limit_t num_vacant_stacks =
            count_num_vacant_stacks(LOCAL_STACKS_NUM, &FCS_SCANS_the_state);

        if (check_if_limits_exceeded())
        {
            soft_thread->first_state_to_check = PTR_STATE;

            TRACE0("error_code - FCS_STATE_SUSPEND_PROCESS");
            error_code = FCS_STATE_SUSPEND_PROCESS;
            goto my_return_label;
        }

        TRACE0("debug_iter_output");
        if (debug_iter_output_func)
        {
            debug_iter_output_func(
                    debug_iter_output_context,
                    *(instance_num_checked_states_ptr),
                    calc_depth(PTR_STATE),
                    (void*)instance,
                    STATE_TO_PASS(),
#ifdef FCS_WITHOUT_VISITED_ITER
                    0
#else
                    ((FCS_S_PARENT(PTR_STATE) == NULL) ?
                        0 :
                        FCS_S_VISITED_ITER(FCS_S_PARENT(PTR_STATE))
                    )
#endif
                    );
        }


        if ((num_vacant_stacks == LOCAL_STACKS_NUM) && (num_vacant_freecells == LOCAL_FREECELLS_NUM))
        {
            instance->final_state = PTR_STATE;

            BUMP_NUM_CHECKED_STATES();

            error_code = FCS_STATE_WAS_SOLVED;
            goto my_return_label;
        }

        calculate_real_depth (calc_real_depth, PTR_STATE);

        soft_thread->num_vacant_freecells = num_vacant_freecells;
        soft_thread->num_vacant_stacks = num_vacant_stacks;

        if (BEFS_M_VAR(soft_thread, befs_positions_by_rank))
        {
            free(BEFS_M_VAR(soft_thread, befs_positions_by_rank));
            BEFS_M_VAR(soft_thread, befs_positions_by_rank) = NULL;
        }

        TRACE0("perform_tests");

        /*
         * Do all the tests at one go, because that is the way it should be
         * done for BFS and BeFS.
        */
        derived.num_states = 0;
        for(const fc_solve_solve_for_state_move_func_t * next_test = tests_list;
            next_test < tests_list_end;
            next_test++
           )
        {
            (*next_test)(
                soft_thread,
                STATE_TO_PASS(),
                &derived
            );
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
        fcs_derived_states_list_item_t * derived_iter;
        fcs_derived_states_list_item_t * derived_end;
        for (
            derived_end = (derived_iter = derived.states) + derived.num_states
                ;
            derived_iter < derived_end
                ;
            derived_iter++
        )
        {
#ifdef FCS_RCS_STATES
            new_pass.key =
                fc_solve_lookup_state_key_from_val(instance,
                        new_pass.val = derived_iter->state_ptr
                );
#else
            new_pass = FCS_STATE_keyval_pair_to_kv(FCS_SCANS_ptr_new_state = derived_iter->state_ptr);
#endif

            if (method == FCS_METHOD_A_STAR)
            {
                fc_solve_pq_push(
                    pqueue,
                    FCS_SCANS_ptr_new_state,
                    befs_rate_state(
                        soft_thread,
                        WEIGHTING(soft_thread),
                        new_pass.key,
                        BEFS_MAX_DEPTH - kv_calc_depth(&(new_pass))
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

                queue_last_item->s = FCS_SCANS_ptr_new_state;
                last_item_next->next = NULL;
                queue_last_item = last_item_next;
            }
        }

        if (method == FCS_METHOD_OPTIMIZE)
        {
            FCS_S_VISITED(PTR_STATE) |= FCS_VISITED_IN_OPTIMIZED_PATH;
        }
        else
        {
            set_scan_visited(
                    PTR_STATE,
                    soft_thread_id
                    );

            if (derived.num_states == 0)
            {
                if (is_a_complete_scan)
                {
                    mark_as_dead_end(scans_synergy, PTR_STATE);
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
        if (method == FCS_METHOD_A_STAR)
        {
            fcs_collectible_state_t * new_ptr_state;
#ifdef DEBUG
            dump_pqueue(soft_thread, "before_pop", pqueue);
#endif
            /* It is an BeFS scan */
            fc_solve_pq_pop(
                pqueue,
                &(new_ptr_state)
                );

            ASSIGN_ptr_state(new_ptr_state);
        }
        else
        {
            save_item = queue->next;
            if (save_item != queue_last_item)
            {
                ASSIGN_ptr_state(save_item->s);
                queue->next = save_item->next;
                save_item->next = my_brfs_recycle_bin;
                my_brfs_recycle_bin = save_item;
            }
            else
            {
                ASSIGN_ptr_state(NULL);
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

    if (BEFS_M_VAR(soft_thread, befs_positions_by_rank))
    {
        free(BEFS_M_VAR(soft_thread, befs_positions_by_rank));
        BEFS_M_VAR(soft_thread, befs_positions_by_rank) = NULL;
    }

    return error_code;
}


static GCC_INLINE void assign_dest_stack_and_col_ptr(
    char * const positions_by_rank,
    const char dest_stack,
    const char dest_col,
    const fcs_card_t dest_card
)
{
    char * ptr = &positions_by_rank[
        (FCS_POS_BY_RANK_WIDTH *
         (fcs_card_rank(dest_card)-1)
        )
        +
        (fcs_card_suit(dest_card)<<1)
        ];

#if (!defined(HARD_CODED_NUM_DECKS) || (HARD_CODED_NUM_DECKS == 1))
    for(;(*ptr) != -1;ptr += (4 << 1))
    {
    }
#endif

    *(ptr++) = dest_stack;
    *(ptr) = dest_col;
}

char * fc_solve_get_the_positions_by_rank_data__freecell_generator(
    fc_solve_soft_thread_t * const soft_thread,
    const fcs_state_t * const ptr_state_key
)
{
#define state_key (*ptr_state_key)
#undef FCS_SCANS_the_state
#define FCS_SCANS_the_state state_key

#if (!(defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
    fc_solve_instance_t * const instance = HT_INSTANCE(soft_thread->hard_thread);
    SET_GAME_PARAMS();
#endif

#ifndef FCS_FREECELL_ONLY
    const int sequences_are_built_by = GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance);
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
#define FCS_POS_BY_RANK_LEN ( NUM_POS_BY_RANK_SLOTS * FCS_POS_BY_RANK_WIDTH )
#define FCS_POS_BY_RANK_SIZE (sizeof(positions_by_rank[0]) * FCS_POS_BY_RANK_LEN)

    char * const positions_by_rank = SMALLOC(positions_by_rank, FCS_POS_BY_RANK_LEN);

    memset(positions_by_rank, -1, FCS_POS_BY_RANK_SIZE);

    {
        /* Populate positions_by_rank by looping over the stacks and
         * indices looking for the cards and filling them. */

        for (int ds = 0 ; ds < LOCAL_STACKS_NUM ; ds++)
        {
            const fcs_const_cards_column_t dest_col = fcs_state_get_col(FCS_SCANS_the_state, ds);
            int top_card_idx = fcs_col_len(dest_col);

            if (unlikely((top_card_idx--) == 0))
            {
                continue;
            }

            fcs_card_t dest_card;
            {
                fcs_card_t dest_below_card;
                dest_card = fcs_col_get_card(dest_col, 0);
                for (
                    int dc=0
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
                        assign_dest_stack_and_col_ptr(
                            positions_by_rank,
                            ds,
                            dc,
                            dest_card
                        );
                    }
                }
            }
            assign_dest_stack_and_col_ptr(
                positions_by_rank,
                ds,
                top_card_idx,
                dest_card
            );
        }
    }

    return positions_by_rank;
}
#undef state_key
#undef ptr_state_key


/*
 * These functions are used by the move functions in freecell.c and
 * simpsim.c.
 * */
int fc_solve_sfs_check_state_begin(
    fc_solve_hard_thread_t * const hard_thread,
    fcs_kv_state_t * const out_new_state_out,
    fcs_kv_state_t * const raw_ptr_state_raw,
    fcs_move_stack_t * const moves
    )
{
#define ptr_state (raw_ptr_state_raw->val)
    fcs_collectible_state_t * raw_ptr_new_state;
    fc_solve_instance_t * const instance = HT_INSTANCE(hard_thread);

    if ((HT_FIELD(hard_thread, allocated_from_list) =
        (instance->list_of_vacant_states != NULL)))
    {
        raw_ptr_new_state = instance->list_of_vacant_states;
        instance->list_of_vacant_states = FCS_S_NEXT(instance->list_of_vacant_states);
    }
    else
    {
        raw_ptr_new_state =
            fcs_state_ia_alloc_into_var(
                &(HT_FIELD(hard_thread, allocator))
            );
    }

    FCS_STATE_collectible_to_kv(out_new_state_out, raw_ptr_new_state);
    fcs_duplicate_kv_state(
        out_new_state_out,
        raw_ptr_state_raw
    );
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
    FCS_S_MOVES_TO_PARENT(raw_ptr_new_state) = moves;
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
       sizeof(FCS_S_SCAN_VISITED(raw_ptr_new_state))
        );
    fcs_move_stack_reset(moves);

    return 0;
}
#undef ptr_state

extern void fc_solve_sfs_check_state_end(
    fc_solve_soft_thread_t * const soft_thread,
    fcs_kv_state_t * const raw_ptr_state_raw,
    fcs_kv_state_t * const raw_ptr_new_state_raw,
    const int state_context_value,
    fcs_move_stack_t * const moves,
    fcs_derived_states_list_t * const derived_states_list
    )
{
    fc_solve_hard_thread_t * const hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * const instance = HT_INSTANCE(hard_thread);
#ifndef FCS_WITHOUT_DEPTH_FIELD
    const fcs_runtime_flags_t calc_real_depth
        = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_CALC_REAL_DEPTH);
#endif
    const fcs_runtime_flags_t scans_synergy
        = STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY);
    fcs_kv_state_t existing_state;

#define ptr_new_state_foo (raw_ptr_new_state_raw->val)
#define ptr_state (raw_ptr_state_raw->val)

    if (! fc_solve_check_and_add_state(
        hard_thread,
        raw_ptr_new_state_raw,
        &existing_state
        ))
    {
#define existing_state_val (existing_state.val)
        if (HT_FIELD(hard_thread, allocated_from_list))
        {
            ptr_new_state_foo->parent = instance->list_of_vacant_states;
            instance->list_of_vacant_states = INFO_STATE_PTR(raw_ptr_new_state_raw);
        }
        else
        {
            fcs_compact_alloc_release(&(HT_FIELD(hard_thread, allocator)));
        }

#ifndef FCS_WITHOUT_DEPTH_FIELD
        calculate_real_depth (calc_real_depth, FCS_STATE_kv_to_collectible(&existing_state));
#endif

        /* Re-parent the existing state to this one.
         *
         * What it means is that if the depth of the state if it
         * can be reached from this one is lower than what it
         * already have, then re-assign its parent to this state.
         * */
        if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_TO_REPARENT_STATES_REAL) &&
           (kv_calc_depth(&existing_state) > kv_calc_depth(raw_ptr_state_raw)+1)
        )
        {
            /* Make a copy of "moves" because "moves" will be destroyed */
            existing_state_val->moves_to_parent =
                fc_solve_move_stack_compact_allocate(
                    hard_thread, moves
                    );
            if (!(existing_state_val->visited & FCS_VISITED_DEAD_END))
            {
                if ((--(FCS_S_NUM_ACTIVE_CHILDREN(existing_state_val->parent))) == 0)
                {
                    mark_as_dead_end(scans_synergy, existing_state_val->parent);
                }
                ptr_state->num_active_children++;
            }
            existing_state_val->parent = INFO_STATE_PTR(raw_ptr_state_raw);
#ifndef FCS_WITHOUT_DEPTH_FIELD
            existing_state_val->depth = ptr_state->depth + 1;
#endif
        }


        fc_solve_derived_states_list_add_state(
            derived_states_list,
            FCS_STATE_kv_to_collectible(&existing_state),
            state_context_value
        );

    }
    else
    {
        fc_solve_derived_states_list_add_state(
            derived_states_list,
            INFO_STATE_PTR(raw_ptr_new_state_raw),
            state_context_value
        );
    }

    return;
}
