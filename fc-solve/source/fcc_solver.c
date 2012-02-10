/* Copyright (c) 2012 Shlomi Fish
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
 * fcc_solver.c - a specialised solver that works by analyzing fully-connected
 * components (FCCs) of the Freecell graph.
 *
 * For more information see:
 *
 * ../docs/fully-connected-components-based-solver-planning.txt
 *
 */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <limits.h>

/* 
 * Define FCS_DBM_USE_RWLOCK to use the pthread FCFS RWLock which appears
 * to improve CPU utilisations of the various worker threads and possibly
 * overall performance.
 * #define FCS_DBM_USE_RWLOCK 1 */

#ifdef FCS_DBM_USE_RWLOCK
#include <pthread/rwlock_fcfs.h>
#endif

#include "config.h"

#include "bool.h"
#include "inline.h"
#include "portable_time.h"

#include "dbm_calc_derived.h"

#include "delta_states.c"

#include "dbm_common.h"
#include "dbm_solver.h"

#include "dbm_lru_cache.h"
#include "fcc_brfs.h"

struct fcs_fully_connected_component_struct
{
    /* The minimal state in the fully-connected component, according to
    the lexical sorting of the encoded state keys. This is used to identify
    it and avoid collisions and re-processing. 
    */
    fcs_encoded_state_buffer_t min_by_sorting;
    /* The minimal state by absolute depth (including that of
    reversible moves). Not absolutely minimal, because the first
    $depth-1 FCC that reaches it, wins the jackpot.
    */
    fcs_encoded_state_buffer_t min_by_absolute_depth;
    /* Moves to the min_by_absolute_depth from the initial state. 
    (accumlative).
    */
    int count_moves_to_min_by_absolute_depth;
    fcs_fcc_move_t * moves_to_min_by_absolute_depth;
    struct fcs_fully_connected_component_struct * next;
};

typedef struct fcs_fully_connected_component_struct fcs_fully_connected_component_t;

typedef struct {
    /* All of these get recycled (and their memory reclaimed by the heap)
     * when we ascend to a new depth. */
    fcs_fully_connected_component_t * queue;
    fcs_fully_connected_component_t * queue_recycle_bin;
    fcs_compact_allocator_t queue_allocator;
    dict_t * does_min_by_sorting_exist;
    dict_t * does_min_by_absolute_depth_exist;
} fcs_fcc_collection_by_depth;

#define FCC_DEPTH (RANK_KING * 4 * DECKS_NUM * 2)
typedef struct {
    int curr_depth;
    long max_num_elements_in_cache;
    fcs_fcc_collection_by_depth FCCs_by_depth[FCC_DEPTH];
    /* No need to reset when we ascend to a new depth. 
     * TODO : make sure the pointers to the encoded states 
     * inside the cache are always valid.
     * TODO : might be easier to reset during every ascension.
     * */
    fcs_lru_cache_t cache;
} fcs_fcc_solver_state;

static const pthread_mutex_t initial_mutex_constant =
    PTHREAD_MUTEX_INITIALIZER
    ;

#ifdef FCS_DBM_USE_RWLOCK
typedef pthread_rwlock_fcfs_t * fcs_lock_t;
#define FCS_LOCK(lock) pthread_rwlock_fcfs_gain_write(lock)
#define FCS_UNLOCK(lock) pthread_rwlock_fcfs_release(lock)
#define FCS_INIT_LOCK(lock) ((lock) = pthread_rwlock_fcfs_alloc())
#define FCS_DESTROY_LOCK(lock) pthread_rwlock_fcfs_destroy(lock)
#else
typedef pthread_mutex_t fcs_lock_t;
#define FCS_LOCK(lock) pthread_mutex_lock(&(lock))
#define FCS_UNLOCK(lock) pthread_mutex_unlock(&(lock))
#define FCS_INIT_LOCK(lock) ((lock) = initial_mutex_constant)
#define FCS_DESTROY_LOCK(lock) {}
#endif

typedef struct {
    fcs_fcc_solver_state solver_state;
    long count_num_processed;
    long max_processed_positions_count;
    long positions_milestone_step;
    long FCCs_per_depth_milestone_step;
    fcs_meta_compact_allocator_t meta_alloc;
} fcs_dbm_solver_instance_t;

static void GCC_INLINE instance_init(
    fcs_dbm_solver_instance_t * instance,
    long max_processed_positions_count,
    long positions_milestone_step,
    long FCCs_per_depth_milestone_step
)
{
    instance->count_num_processed = 0;
    instance->max_processed_positions_count = max_processed_positions_count;
    instance->positions_milestone_step = positions_milestone_step;
    instance->FCCs_per_depth_milestone_step = FCCs_per_depth_milestone_step;
}

static void GCC_INLINE instance_destroy(
    fcs_dbm_solver_instance_t * instance
    )
{
    return;
}

typedef struct {
    fcs_dbm_solver_instance_t * instance;
    fc_solve_delta_stater_t * delta_stater;
} fcs_dbm_solver_thread_t;

typedef struct {
    fcs_dbm_solver_thread_t * thread;
} thread_arg_t;

static GCC_INLINE void init_solver_state(
    fcs_fcc_solver_state * solver_state,
    long max_num_elements_in_cache,
    fcs_meta_compact_allocator_t * meta_alloc
    )
{
    int i;

    solver_state->curr_depth = 0;
    solver_state->max_num_elements_in_cache = max_num_elements_in_cache;

    cache_init (
        &(solver_state->cache),
        max_num_elements_in_cache,
        meta_alloc
    );
    
    for (i = 0 ; i < FCC_DEPTH ; i++)
    {
        fcs_fcc_collection_by_depth * fcc = &(solver_state->FCCs_by_depth[i]);
        fcc->queue = NULL;
        fcc->queue_recycle_bin = NULL;
        fc_solve_compact_allocator_init( &(fcc->queue_allocator), meta_alloc );
        fcc->does_min_by_sorting_exist
            = fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL, meta_alloc);
        fcc->does_min_by_absolute_depth_exist
            = fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL, meta_alloc);
    }
}

static GCC_INLINE void solver_state__free_dcc_depth(
    fcs_fcc_solver_state * solver_state,
    int depth
    )
{
    fcs_fcc_collection_by_depth * fcc = &(solver_state->FCCs_by_depth[depth]);
    fcs_fully_connected_component_t * iter;

    if (! fcc->does_min_by_sorting_exist)
    {
        return;
    }
    else
    {
        fc_solve_kaz_tree_destroy(fcc->does_min_by_sorting_exist);
        fcc->does_min_by_sorting_exist = NULL;
    }
    /* We need to iterate over the queue before we dellocate its
     * memory by freeing the compact_allocator.
     * */
    for(
        iter = fcc->queue;
        iter;
        iter = iter->next
       )
    {
        if (iter->moves_to_min_by_absolute_depth)
        {
            free(iter->moves_to_min_by_absolute_depth);
            iter->moves_to_min_by_absolute_depth = NULL;
        }
    }
    fcc->queue = NULL;
    if (fcc->does_min_by_absolute_depth_exist)
    {
        fc_solve_kaz_tree_destroy(fcc->does_min_by_absolute_depth_exist);
        fcc->does_min_by_absolute_depth_exist = NULL;
    }
    fc_solve_compact_allocator_finish(&(fcc->queue_allocator));

}

static GCC_INLINE void solver_state_free(
    fcs_fcc_solver_state * solver_state
)
{
    int depth;
    for (depth = 0; depth < FCC_DEPTH ; depth++)
    {
        solver_state__free_dcc_depth(solver_state, depth);
    }
    solver_state->curr_depth = depth;
    cache_destroy(&(solver_state->cache));
}

enum STATUS
{
    FCC_SOLVED = 0,
    FCC_IMPOSSIBLE,
    FCC_INTERRUPTED
};

static GCC_INLINE void instance_time_printf(
    fcs_dbm_solver_instance_t * instance,
    const char * format,
    ...
)
{
    fcs_portable_time_t mytime;
    va_list ap;
    FILE * fh = stdout;

    FCS_GET_TIME(mytime);
    fprintf(fh, "[T=%li.%.6li] ", FCS_TIME_GET_SEC(mytime), FCS_TIME_GET_USEC(mytime));
    va_start (ap, format);
    vfprintf(fh, format, ap);
    va_end(ap);
    fflush(fh);
}

#define STEP (instance->positions_milestone_step)

static GCC_INLINE int fc_solve_compare_fcs_FCC_start_points(const void * a, const void * b, void * context)
{
#define DEREF(x) (&(((fcs_FCC_start_point_t *)(x))->enc_state))
    return fc_solve_compare_encoded_states(DEREF(a), DEREF(b), context);
#undef DEREF
}

int instance_run_solver(
    fcs_dbm_solver_instance_t * instance,
    long max_num_elements_in_cache,
    fcs_state_keyval_pair_t * init_state,
    int * out_count_moves,
    fcs_fcc_move_t * * out_moves
)
{
    fc_solve_delta_stater_t * delta;
    fcs_encoded_state_buffer_t init_state_enc;
    fcs_fcc_solver_state * solver_state;
    fcs_fcc_collection_by_depth * fcc_stage;
    fcs_fully_connected_component_t * fcc;
    int curr_depth;
    fcs_lru_cache_t * cache;
    int ret;
    fcs_fcc_move_t * ret_moves;
    int ret_count_moves;
    long next_count_num_processed_landmark = STEP;
    fcs_meta_compact_allocator_t * meta_alloc;
    long FCCs_per_depth_milestone_step;

    fc_solve_meta_compact_allocator_init(meta_alloc = &(instance->meta_alloc));
    /* Initialize the state. */
    solver_state = &(instance->solver_state);
    init_solver_state(solver_state, max_num_elements_in_cache, &(instance->meta_alloc));
    cache = &(solver_state->cache);

    instance->count_num_processed = 0;
    FCCs_per_depth_milestone_step = instance->FCCs_per_depth_milestone_step;

    /* Initialize local variables. */
    delta = fc_solve_delta_stater_alloc(
        &(init_state->s),
        STACKS_NUM,
        FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
        , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
    );

    fc_solve_delta_stater_encode_into_buffer(
        delta, init_state, init_state_enc.s
    );

    ret = FCC_IMPOSSIBLE;
    ret_moves = NULL;
    ret_count_moves = 0;

    {
#define SUIT_LIMIT ( DECKS_NUM * 4 )
        int suit;

        for (suit = 0 ; suit < SUIT_LIMIT ; suit++)
        {
            if (fcs_foundation_value(init_state->s, suit) < RANK_KING)
            {
                break;
            }
        }

        if (suit == SUIT_LIMIT)
        {
            ret = FCC_SOLVED;
            goto free_resources;
        }
    }
#undef SUIT_LIMIT

    /* Bootstrap FCC depth 0 with the initial state. */
    fcc_stage = &(solver_state->FCCs_by_depth[0]);
    fcc_stage->queue = fcc =
        fcs_compact_alloc_ptr(
            &(fcc_stage->queue_allocator),
            sizeof(*(fcc_stage->queue))
        );

    fcc->min_by_absolute_depth = init_state_enc;
    fcc->count_moves_to_min_by_absolute_depth = 0;
    fcc->moves_to_min_by_absolute_depth = NULL;
    fcc->next = NULL;

    instance->count_num_processed++;

    /* Now: iterate over the depths and generate new states. */
    for (curr_depth=0 
         ; 
         (ret == FCC_IMPOSSIBLE) && (curr_depth < FCC_DEPTH )
         ; 
         solver_state->curr_depth = ++curr_depth,
         fcc_stage++
         )
    {
        dict_t * do_next_fcc_start_points_exist;
        fcs_FCC_start_points_list_t next_start_points_list;
        long num_FCCs_processed_for_depth = 0, num_unique_FCCs_for_depth = 0;
        struct avl_traverser trav;
        fcs_FCC_start_point_t * start_point_iter;
        fcs_FCC_start_point_t * more_start_point_iter;


        next_start_points_list.recycle_bin = NULL;
        fc_solve_compact_allocator_init(&(next_start_points_list.allocator), meta_alloc);

        do_next_fcc_start_points_exist
            = fc_solve_kaz_tree_create(fc_solve_compare_fcs_FCC_start_points, NULL, meta_alloc);
        while ((ret == FCC_IMPOSSIBLE) && fcc_stage->queue)
        {
            fcs_bool_t is_fcc_new;
            fcs_encoded_state_buffer_t min_by_sorting;
            long num_new_positions;

            fcc_stage->queue = ((fcc = fcc_stage->queue)->next);

#if 0
            instance_time_printf (instance, "Before perform_FCC_brfs\n");
#endif

            /* Now scan the new fcc */
            perform_FCC_brfs(
                init_state,
                fcc->min_by_absolute_depth,
                fcc->count_moves_to_min_by_absolute_depth,
                fcc->moves_to_min_by_absolute_depth,
                &next_start_points_list,
                do_next_fcc_start_points_exist,
                &is_fcc_new,
                &min_by_sorting,
                fcc_stage->does_min_by_sorting_exist,
                cache,
                &num_new_positions,
                meta_alloc
            );

#if 0
            instance_time_printf (instance, "After perform_FCC_brfs\n");
#endif

            if (( (++num_FCCs_processed_for_depth) 
                % FCCs_per_depth_milestone_step ) 
                    == 0
            )
            {
                instance_time_printf(
                    instance, 
                    "Processed %ld FCCs (%ld unique) for depth %d\n", 
                    num_FCCs_processed_for_depth, num_unique_FCCs_for_depth, curr_depth
                );
            }

            if (num_new_positions)
            {
                if ((instance->count_num_processed += num_new_positions) >= next_count_num_processed_landmark)
                {
                    instance_time_printf(
                        instance,
                        "Reached %li positions\n", instance->count_num_processed
                    );
                    next_count_num_processed_landmark = instance->count_num_processed;
                    next_count_num_processed_landmark += (STEP - (next_count_num_processed_landmark % STEP));
                    if (instance->count_num_processed >=
                        instance->max_processed_positions_count)
                    {
                        ret = FCC_INTERRUPTED;

                        avl_t_init(&trav, do_next_fcc_start_points_exist);
                        more_start_point_iter = avl_t_first(&trav, do_next_fcc_start_points_exist);

                        goto fcc_loop_cleanup;
                    }
                }
            }

            if (is_fcc_new)
            {
                fcs_encoded_state_buffer_t * min_by_sorting_copy_ptr;
                
                num_unique_FCCs_for_depth++;

                /* First of all, add min_by_sorting to 
                 * fcc_stage->does_min_by_sorting_exist, so it won't be
                 * traversed again. We need to allocate a new copy because
                 * min_by_sorting is a temporary variable and the tree
                 * holds pointers.
                 * */
                min_by_sorting_copy_ptr = fcs_compact_alloc_ptr(
                    &(fcc_stage->does_min_by_sorting_exist->dict_allocator),
                    sizeof(*min_by_sorting_copy_ptr)
                    );

                *min_by_sorting_copy_ptr = min_by_sorting;

                fc_solve_kaz_tree_alloc_insert(
                    fcc_stage->does_min_by_sorting_exist,
                    min_by_sorting_copy_ptr
                );
            }
fcc_loop_cleanup:
            /* Free fcc's resources. */
            free (fcc->moves_to_min_by_absolute_depth);
            fcc->moves_to_min_by_absolute_depth = NULL;
            fcc->count_moves_to_min_by_absolute_depth = 0;

            /* -> Put it in the queue's recycle bin. */
            fcc->next = fcc_stage->queue_recycle_bin;
            fcc_stage->queue_recycle_bin = fcc;
        }

        avl_t_init(&trav, do_next_fcc_start_points_exist);
        start_point_iter =
            (fcs_FCC_start_point_t *)avl_t_first(&trav, do_next_fcc_start_points_exist);

        if (ret == FCC_IMPOSSIBLE)
        {
            /* Now: iterate over the next_start_points_list . */
            for (
                ;
                start_point_iter
                ;
                start_point_iter = avl_t_next(&trav)
                )
            {
                int num_additional_moves;
                fcs_state_keyval_pair_t state;
                int start_point_new_FCC_depth;
                fcs_encoded_state_buffer_t enc_state;
                fcs_fcc_collection_by_depth * next_fcc_stage;

                DECLARE_IND_BUF_T(indirect_stacks_buffer)

                    fc_solve_delta_stater_decode_into_state(
                        delta,
                        start_point_iter->enc_state.s,
                        &(state),
                        indirect_stacks_buffer
                        );
                /* Perform Horne's Prune on the position to see where it ends up at. */
                num_additional_moves =
                    horne_prune(
                        &state,
                        &(start_point_iter->count_moves),
                        &(start_point_iter->moves)
                        );
                /* TODO : check that it's the final state. If
                 * so, do the cleanup and return the solution.
                 * */
                {
#define SUIT_LIMIT ( DECKS_NUM * 4 )
                    int suit;

                    for (suit = 0 ; suit < SUIT_LIMIT ; suit++)
                    {
                        if (fcs_foundation_value(state.s, suit) < RANK_KING)
                        {
                            break;
                        }
                    }

                    if (suit == SUIT_LIMIT)
                    {
                        /* State is solved! Yay! Cleanup and return. */
                        ret = FCC_SOLVED;
                        ret_moves = start_point_iter->moves;
                        ret_count_moves = start_point_iter->count_moves;
                        /* Invalidate the existing ones so they won't be
                         * freed by accident.
                         * */
                        start_point_iter->moves = NULL;
                        start_point_iter->count_moves = -1;

                        goto second_stage_cleanup;
                    }
                }
#undef SUIT_LIMIT

                fcs_init_encoded_state(&(enc_state));
                fc_solve_delta_stater_encode_into_buffer(
                    delta,
                    &(state),
                    enc_state.s
                    );

                next_fcc_stage = &(solver_state->FCCs_by_depth[
                                   start_point_new_FCC_depth =
                                   curr_depth + 1 + num_additional_moves
                                   ]);

                if (!
                    fc_solve_kaz_tree_lookup_value(
                        next_fcc_stage->does_min_by_absolute_depth_exist,
                        &enc_state
                        )
                   )
                {
                    fcs_fully_connected_component_t * next_fcc;
                    fcs_encoded_state_buffer_t * enc_state_copy_ptr;

                    enc_state_copy_ptr = fcs_compact_alloc_ptr(
                        &(next_fcc_stage->does_min_by_absolute_depth_exist->dict_allocator),
                        sizeof(*enc_state_copy_ptr)
                        );
                    *enc_state_copy_ptr = enc_state;

                    fc_solve_kaz_tree_alloc_insert(
                        next_fcc_stage->does_min_by_absolute_depth_exist,
                        enc_state_copy_ptr
                        );

                    if (next_fcc_stage->queue_recycle_bin)
                    {
                        fcs_fully_connected_component_t * temp_fcc;
                        temp_fcc = next_fcc_stage->queue_recycle_bin->next;
                        next_fcc_stage->queue_recycle_bin->next = 
                            next_fcc_stage->queue;
                        next_fcc_stage->queue = next_fcc_stage->queue_recycle_bin;
                        next_fcc_stage->queue_recycle_bin = temp_fcc;
                    }
                    else
                    {
                        fcs_fully_connected_component_t * temp_fcc;
                        temp_fcc =
                            fcs_compact_alloc_ptr(
                                &(next_fcc_stage->queue_allocator),
                                sizeof(*(next_fcc_stage->queue))
                                );
                        temp_fcc->next = next_fcc_stage->queue;
                        next_fcc_stage->queue = temp_fcc;
                    }
                    next_fcc = next_fcc_stage->queue;

                    next_fcc->min_by_absolute_depth = enc_state;
                    next_fcc->count_moves_to_min_by_absolute_depth = 
                        start_point_iter->count_moves;
                    next_fcc->moves_to_min_by_absolute_depth = 
                        start_point_iter->moves;
                }
                else
                {
                    free(start_point_iter->moves);
                }
                start_point_iter->moves = NULL;
                start_point_iter->count_moves = -1;
            }
        }

second_stage_cleanup:
        more_start_point_iter = start_point_iter;
        {
            for(
                ;
                more_start_point_iter;
                more_start_point_iter = avl_t_next(&trav)
               )
            {
                free (more_start_point_iter->moves);
            }
        }

        /* Free the next_start_points_list */
        fc_solve_compact_allocator_finish(&(next_start_points_list.allocator));


        /* Now ascend to the next FCC depth. */
        fc_solve_kaz_tree_destroy(do_next_fcc_start_points_exist);
        solver_state__free_dcc_depth(solver_state, curr_depth);
        /* -> Refresh the cache, because it may hold pointers that are
         * out-of-date.
         * */
        cache_destroy(cache);
        cache_init (cache, max_num_elements_in_cache, meta_alloc);
        /*
         * A trace for keeping track of the solver's progress.
         * TODO : make it optional/abstract and add more traces.
         */
        instance_time_printf (instance, "Finished checking FCC-depth %d (Total processed FCCs for depth - %ld ; unique FCCs - %ld)\n", curr_depth, num_FCCs_processed_for_depth, num_unique_FCCs_for_depth);
    }

free_resources:
    solver_state_free(solver_state);
    fc_solve_delta_stater_free(delta);
    fc_solve_meta_compact_allocator_finish(meta_alloc);

    *out_moves = ret_moves;
    *out_count_moves = ret_count_moves;

    return ret;
}

#if 0
void * instance_run_solver_thread(void * void_arg)
{
    thread_arg_t * arg;
    fcs_bool_t queue_solution_was_found;
    fcs_dbm_solver_thread_t * thread;
    fcs_dbm_solver_instance_t * instance;
    fcs_dbm_queue_item_t * item, * prev_item;
    int queue_num_extracted_and_processed;
    fcs_derived_state_t * derived_list, * derived_list_recycle_bin, 
                        * derived_iter;
    fcs_compact_allocator_t derived_list_allocator;
    fc_solve_delta_stater_t * delta_stater;
    fcs_state_keyval_pair_t state;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    arg = (thread_arg_t *)void_arg;
    thread = arg->thread;
    instance = thread->instance;
    delta_stater = thread->delta_stater;

    prev_item = item = NULL;
    queue_num_extracted_and_processed = 0;

    fc_solve_compact_allocator_init(&(derived_list_allocator));
    derived_list_recycle_bin = NULL;
    derived_list = NULL;

    while (1)
    {
        /* First of all extract an item. */
        FCS_LOCK(instance->queue_lock);

        if (prev_item)
        {
            instance->queue_num_extracted_and_processed--;
            prev_item->next = instance->queue_recycle_bin;
            instance->queue_recycle_bin = prev_item;
        }

        if (! (queue_solution_was_found = instance->queue_solution_was_found))
        {
            if ((item = instance->queue_head))
            {
                if (!(instance->queue_head = item->next))
                {
                    instance->queue_tail = NULL;
                }
                instance->queue_num_extracted_and_processed++;
                if (++instance->count_num_processed % 100000 == 0)
                {
                    fcs_portable_time_t mytime;
                    FCS_GET_TIME(mytime);

                    printf ("Reached %ld ; States-in-collection: %ld ; Time: %li.%.6li\n",
                        instance->count_num_processed,
                        instance->num_states_in_collection,
                        FCS_TIME_GET_SEC(mytime),
                        FCS_TIME_GET_USEC(mytime)
                    );
                    fflush(stdout);
                }
            }

            queue_num_extracted_and_processed =
                instance->queue_num_extracted_and_processed;
        }
        FCS_UNLOCK(instance->queue_lock);

        if (queue_solution_was_found || (! queue_num_extracted_and_processed))
        {
            break;
        }

        if (! item)
        {
            /* Sleep until more items become available in the
             * queue. */
            usleep(5000);
        }
        else
        {
        /* Handle item. */
        fc_solve_delta_stater_decode_into_state(
            delta_stater,
            item->key.s,
            &(state),
            indirect_stacks_buffer
        );

        if (instance_solver_thread_calc_derived_states(
            &state,
            &(item->key),
            &derived_list,
            &derived_list_recycle_bin,
            &derived_list_allocator,
            TRUE
        ))
        {
            FCS_LOCK(instance->queue_lock);
            instance->queue_solution_was_found = TRUE;
            memcpy(&(instance->queue_solution), &(item->key),
                   sizeof(instance->queue_solution));
            FCS_UNLOCK(instance->queue_lock);
            break;
        }

        /* Encode all the states. */
        for (derived_iter = derived_list;
                derived_iter ;
                derived_iter = derived_iter->next
        )
        {
            fcs_init_encoded_state(&(derived_iter->key));
            fc_solve_delta_stater_encode_into_buffer(
                delta_stater,
                &(derived_iter->state),
                derived_iter->key.s
                );
        }

        instance_check_multiple_keys(instance, derived_list);

        /* Now recycle the derived_list */
        while (derived_list)
        {
#define derived_list_next derived_iter
            derived_list_next = derived_list->next;
            derived_list->next = derived_list_recycle_bin;
            derived_list_recycle_bin = derived_list;
            derived_list = derived_list_next;
#undef derived_list_next
        }
        /* End handle item. */
        }
        /* End of main thread loop */
        prev_item = item;
    }

    fc_solve_compact_allocator_finish(&(derived_list_allocator));

    return NULL;
}
#endif


typedef struct {
    fcs_dbm_solver_thread_t thread;
    thread_arg_t arg;
    pthread_t id;
} main_thread_item_t;

#define USER_STATE_SIZE 2000

const char * move_to_string(unsigned char move, char * move_buffer)
{
    int iter, inspect;
    char * s;

    s = move_buffer;

    for (iter=0 ; iter < 2 ; iter++)
    {
        inspect = (move & 0xF);
        move >>= 4;

        if (inspect < 8)
        {
            s += sprintf(s, "Column %d", inspect);
        }
        else
        {
            inspect -= 8;
            if (inspect < 4)
            {
                s += sprintf(s, "Freecell %d", inspect);
            }
            else
            {
                inspect -= 4;
                s += sprintf(s, "Foundation %d", inspect);
            }
        }
        if (iter == 0)
        {
            s += sprintf(s, "%s", " -> ");
        }
    }

    return move_buffer;
}

int main(int argc, char * argv[])
{
    /* Temporarily #if'ed away until we finish working on instance_run_solver
     * */
    fcs_dbm_solver_instance_t instance;
    long pre_cache_max_count;
    long caches_delta;
    long max_processed_positions_count = LONG_MAX;
    long positions_milestone_step = 100000;
    int num_threads;
    int arg;
    const char * filename;
    FILE * fh;
    char user_state[USER_STATE_SIZE];
    fcs_state_keyval_pair_t init_state;
    int ret_count_moves;
    long FCCs_per_depth_milestone_step;
    fcs_fcc_move_t * ret_moves;
    int ret_code;
    DECLARE_IND_BUF_T(init_indirect_stacks_buffer)

    pre_cache_max_count = 1000000;
    caches_delta = 1000000;
    num_threads = 2;
    FCCs_per_depth_milestone_step = 10000;

    for (arg=1;arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--pre-cache-max-count"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--pre-cache-max-count came without an argument!\n");
                exit(-1);
            }
            pre_cache_max_count = atol(argv[arg]);
            if (pre_cache_max_count < 1000)
            {
                fprintf(stderr, "--pre-cache-max-count must be at least 1,000.\n");
                exit(-1);
            }
        }
        else if (!strcmp(argv[arg], "--caches-delta"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--caches-delta came without an argument!\n");
                exit(-1);
            }
            caches_delta = atol(argv[arg]);
            if (caches_delta < 1000)
            {
                fprintf(stderr, "--caches-delta must be at least 1,000.\n");
                exit(-1);
            }
        }
        else if (!strcmp(argv[arg], "--max-iters"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--max-iters came without an argument!\n");
                exit(-1);
            }
            max_processed_positions_count = atol(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--positions-milestone-step"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--positions-milestone-step came without an argument!\n");
                exit(-1);
            }
            positions_milestone_step = atol(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--fccs-milestone-step"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--fcc-milestone-step came without an argument!\n");
                exit(-1);
            }
            FCCs_per_depth_milestone_step = atol(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--num-threads"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--num-threads came without an argument!\n");
                exit(-1);
            }
            num_threads = atoi(argv[arg]);
            if (num_threads < 1)
            {
                fprintf(stderr, "--num-threads must be at least 1.\n");
                exit(-1);
            }
        }
        else
        {
            break;
        }
    }

    if (arg < argc-1)
    {
        fprintf (stderr, "%s\n", "Junk arguments!");
        exit(-1);
    }
    else if (arg == argc)
    {
        fprintf (stderr, "%s\n", "No board specified.");
        exit(-1);
    }

    filename = argv[arg];

    instance_init(
        &instance,
        max_processed_positions_count,
        positions_milestone_step,
        FCCs_per_depth_milestone_step
        );
    fh = fopen(filename, "r");
    if (fh == NULL)
    {
        fprintf (stderr, "Could not open file '%s' for input.\n", filename);
        exit(-1);
    }
    memset(user_state, '\0', sizeof(user_state));
    fread(user_state, sizeof(user_state[0]), USER_STATE_SIZE-1, fh);
    fclose(fh);

    fc_solve_initial_user_state_to_c(
        user_state,
        &init_state,
        FREECELLS_NUM,
        STACKS_NUM,
        DECKS_NUM
#ifdef INDIRECT_STACK_STATES
        , init_indirect_stacks_buffer
#endif
    );

    horne_prune(&init_state, NULL, NULL);

    ret_code = instance_run_solver(
        &instance,
        caches_delta,
        &init_state,
        &ret_count_moves,
        &ret_moves
    );

    if (ret_code == FCC_SOLVED)
    {
        int i;
        char move_buffer[500];
        printf ("%s\n", "Success!");
        /* Now trace the solution */
        for (i = 0 ; i < ret_count_moves ; i++)
        {
            printf("==\n%s\n",
                   move_to_string(ret_moves[i], move_buffer)
           );
        }
        printf ("==\nEND\n");
        free(ret_moves);
    }
    else if (ret_code == FCC_IMPOSSIBLE)
    {
        printf ("%s\n", "Could not solve successfully.");
    }
    else if (ret_code == FCC_INTERRUPTED)
    {
        printf ("%s\n", "Interrupted run after limit reached.");
    }
    else
    {
        printf ("%s\n", "Unknown return code. ERROR.");
    }
    
    instance_destroy(&instance);

    return 0;
}
