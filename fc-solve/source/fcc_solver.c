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
#include <limits.h>
#include <signal.h>

/*
 * Define FCS_DBM_USE_RWLOCK to use the pthread FCFS RWLock which appears
 * to improve CPU utilisations of the various worker threads and possibly
 * overall performance.
 * #define FCS_DBM_USE_RWLOCK 1 */

#ifdef FCS_DBM_USE_RWLOCK
#include <pthread/rwlock_fcfs.h>
#endif

#include "config.h"
#undef FCS_RCS_STATES

#include "bool.h"
#include "inline.h"
#include "portable_time.h"

#include "dbm_calc_derived.h"

#include "delta_states_any.h"

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
    fcs_fcc_moves_seq_t moves_seq_to_min_by_absolute_depth;
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
    void * tree_recycle_bin;
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
    long FCCs_per_depth_milestone_step;
    long positions_milestone_step;
    FILE * out_fh;
    long num_FCCs_processed_for_depth;
    long num_unique_FCCs_for_depth;
} fcs_dbm_solver_instance_t;

static GCC_INLINE void instance_init(
    fcs_dbm_solver_instance_t * instance,
    long max_processed_positions_count,
    long positions_milestone_step,
    long FCCs_per_depth_milestone_step,
    FILE * out_fh
)
{
    instance->count_num_processed = 0;
    instance->max_processed_positions_count = max_processed_positions_count;
    instance->positions_milestone_step = positions_milestone_step;
    instance->FCCs_per_depth_milestone_step = FCCs_per_depth_milestone_step;
    instance->out_fh = out_fh;
}

static GCC_INLINE void instance_destroy(
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

    solver_state->tree_recycle_bin = NULL;

    for (i = 0 ; i < FCC_DEPTH ; i++)
    {
        fcs_fcc_collection_by_depth * fcc = &(solver_state->FCCs_by_depth[i]);
        fcc->queue = NULL;
        fcc->queue_recycle_bin = NULL;
        fc_solve_compact_allocator_init( &(fcc->queue_allocator), meta_alloc );
        fcc->does_min_by_sorting_exist
            = fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL, meta_alloc, &(solver_state->tree_recycle_bin));
        fcc->does_min_by_absolute_depth_exist
            = fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL, meta_alloc, &(solver_state->tree_recycle_bin));
    }
}

static GCC_INLINE void solver_state__free_dcc_depth(
    fcs_fcc_solver_state * solver_state,
    int depth,
    fcs_fcc_moves_seq_allocator_t * moves_list_allocator
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
        if (iter->moves_seq_to_min_by_absolute_depth.moves_list)
        {
            fc_solve_fcc_release_moves_seq(
                &(iter->moves_seq_to_min_by_absolute_depth),
                moves_list_allocator
            );
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
    fcs_fcc_solver_state * solver_state,
    fcs_fcc_moves_seq_allocator_t * moves_list_allocator
)
{
    int depth;
    for (depth = 0; depth < FCC_DEPTH ; depth++)
    {
        solver_state__free_dcc_depth(solver_state, depth, moves_list_allocator);
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

static void instance_time_printf(
    fcs_dbm_solver_instance_t * instance,
    const char * format,
    ...
)
{
    fcs_portable_time_t mytime;
    va_list ap;
    FILE * fh = instance->out_fh;

    FCS_GET_TIME(mytime);
    fprintf(fh, "[T=%li.%.6li] ", FCS_TIME_GET_SEC(mytime), FCS_TIME_GET_USEC(mytime));
    va_start (ap, format);
    vfprintf(fh, format, ap);
    va_end(ap);
    fflush(fh);
}

#define STEP (instance->positions_milestone_step)

static GCC_INLINE void instance_print_processed_FCCs(
    fcs_dbm_solver_instance_t * instance
    )
{
    instance_time_printf(
        instance,
        "Processed %ld FCCs (%ld unique) for depth %d\n",
        instance->num_FCCs_processed_for_depth, instance->num_unique_FCCs_for_depth,
        instance->solver_state.curr_depth
    );
}

static GCC_INLINE void instance_print_reached(
    fcs_dbm_solver_instance_t * instance
    )
{
    instance_time_printf(
        instance,
        "Reached %li positions\n", instance->count_num_processed
        );
}

static int instance_run_solver(
    fcs_dbm_solver_instance_t * instance,
    long max_num_elements_in_cache,
    fcs_state_keyval_pair_t * init_state,
    fcs_fcc_moves_seq_t * out_moves_seq,
    fcs_fcc_moves_seq_allocator_t * moves_list_allocator,
    fcs_meta_compact_allocator_t * meta_alloc
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
    long next_count_num_processed_landmark = STEP;
    long FCCs_per_depth_milestone_step;
    fcs_fcc_moves_seq_t ret_moves_seq;

    /* Initialize the state. */
    solver_state = &(instance->solver_state);
    init_solver_state(solver_state, max_num_elements_in_cache, meta_alloc);
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

    fcs_init_and_encode_state(
        delta, init_state, &init_state_enc
    );

    ret = FCC_IMPOSSIBLE;
    ret_moves_seq.count = 0;
    ret_moves_seq.moves_list = NULL;

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
    fcc->moves_seq_to_min_by_absolute_depth.count= 0;
    fcc->moves_seq_to_min_by_absolute_depth.moves_list = NULL;
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
        instance->num_FCCs_processed_for_depth = 0;
        instance->num_unique_FCCs_for_depth = 0;

        do_next_fcc_start_points_exist
            = fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL, meta_alloc, &(solver_state->tree_recycle_bin));

        while ((ret == FCC_IMPOSSIBLE) && fcc_stage->queue)
        {
            fcs_FCC_start_point_t * start_point_iter;
            fcs_FCC_start_points_list_t next_start_points_list;
            fcs_bool_t is_fcc_new;
            fcs_encoded_state_buffer_t min_by_sorting;
            long num_new_positions;
            add_start_point_context_t add_start_point_context;

            fcc_stage->queue = ((fcc = fcc_stage->queue)->next);

            next_start_points_list.list = NULL;
            next_start_points_list.recycle_bin = NULL;
            fc_solve_compact_allocator_init(&(next_start_points_list.allocator), meta_alloc);

#if 0
            instance_time_printf (instance, "Before perform_FCC_brfs\n");
#endif

            add_start_point_context.do_next_fcc_start_points_exist = do_next_fcc_start_points_exist;
            add_start_point_context.next_start_points_list = &next_start_points_list;
            add_start_point_context.moves_list_allocator = moves_list_allocator;

            /* Now scan the new fcc */
            perform_FCC_brfs(
                init_state,
                fcc->min_by_absolute_depth,
                &(fcc->moves_seq_to_min_by_absolute_depth),
                fc_solve_add_start_point_in_mem,
                &add_start_point_context,
                &is_fcc_new,
                &min_by_sorting,
                fcc_stage->does_min_by_sorting_exist,
                cache,
                &num_new_positions,
                moves_list_allocator,
                meta_alloc
            );

            start_point_iter = next_start_points_list.list;

#if 0
            instance_time_printf (instance, "After perform_FCC_brfs\n");
#endif

            if (( (++instance->num_FCCs_processed_for_depth)
                % FCCs_per_depth_milestone_step )
                    == 0
            )
            {
                instance_print_processed_FCCs(instance);
            }

            if (num_new_positions)
            {
                if ((instance->count_num_processed += num_new_positions) >= next_count_num_processed_landmark)
                {
                    instance_print_reached(instance);
                    next_count_num_processed_landmark = instance->count_num_processed;
                    next_count_num_processed_landmark += (STEP - (next_count_num_processed_landmark % STEP));
                    if (instance->count_num_processed >=
                        instance->max_processed_positions_count)
                    {
                        ret = FCC_INTERRUPTED;

                        goto fcc_loop_cleanup;
                    }
                }
            }

            if (is_fcc_new)
            {
                fcs_encoded_state_buffer_t * min_by_sorting_copy_ptr;

                instance->num_unique_FCCs_for_depth++;

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

            /* Free fcc's resources. */
            fc_solve_fcc_release_moves_seq(
                &(fcc->moves_seq_to_min_by_absolute_depth),
                moves_list_allocator
            );

            /* Now: iterate over the next_start_points_list . */
            for (
                    ;
                 start_point_iter
                    ;
                 start_point_iter = start_point_iter->next
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
                /*  TODO : convert horne_prune to fcs_fcc_moves_seq_t . */
                num_additional_moves =
                    horne_prune(
                        &state,
                        &(start_point_iter->moves_seq),
                        moves_list_allocator
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
                    ret_moves_seq = start_point_iter->moves_seq;
                    /* Invalidate the existing ones so they won't be
                     * freed by accident.
                     * */
                    start_point_iter->moves_seq.moves_list = NULL;
                    start_point_iter->moves_seq.count = -1;

                    goto fcc_loop_cleanup;
                }
#undef SUIT_LIMIT

                fcs_init_and_encode_state(
                    delta,
                    &(state),
                    &enc_state
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
                        next_fcc->moves_seq_to_min_by_absolute_depth = start_point_iter->moves_seq;
                    }
                    else
                    {
                        fc_solve_fcc_release_moves_seq(
                            &(start_point_iter->moves_seq),
                            moves_list_allocator
                        );
                    }
                }
            }

fcc_loop_cleanup:
            {
                fcs_FCC_start_point_t * more_start_point_iter;
                for(
                    more_start_point_iter = start_point_iter;
                    more_start_point_iter;
                    more_start_point_iter = more_start_point_iter->next
                   )
                {
                    fc_solve_fcc_release_moves_seq(
                        &(more_start_point_iter->moves_seq),
                        moves_list_allocator
                        );
                }
            }

            /* Free the next_start_points_list */
            fc_solve_compact_allocator_finish(&(next_start_points_list.allocator));

            /* Free fcc's resources. */
            fc_solve_fcc_release_moves_seq(
                &(fcc->moves_seq_to_min_by_absolute_depth),
                moves_list_allocator
            );

            /* -> Put it in the queue's recycle bin. */
            fcc->next = fcc_stage->queue_recycle_bin;
            fcc_stage->queue_recycle_bin = fcc;
        }

        /* Now ascend to the next FCC depth. */
        fc_solve_kaz_tree_destroy(do_next_fcc_start_points_exist);
        solver_state__free_dcc_depth(solver_state, curr_depth, moves_list_allocator);
        /* -> Refresh the cache, because it may hold pointers that are
         * out-of-date.
         * */
        cache_destroy(cache);
        cache_init (cache, max_num_elements_in_cache, meta_alloc);
        /*
         * A trace for keeping track of the solver's progress.
         * TODO : make it optional/abstract and add more traces.
         */
        instance_time_printf (instance, "Finished checking FCC-depth %d (Total processed FCCs for depth - %ld)\n", curr_depth, instance->num_FCCs_processed_for_depth);
    }

free_resources:
    solver_state_free(solver_state, moves_list_allocator);
    fc_solve_delta_stater_free(delta);

    *(out_moves_seq) = ret_moves_seq;

    return ret;
}

typedef struct {
    fcs_dbm_solver_thread_t thread;
    thread_arg_t arg;
    pthread_t id;
} main_thread_item_t;

#define USER_STATE_SIZE 2000

static const char * move_to_string(unsigned char move, char * move_buffer)
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

static fcs_dbm_solver_instance_t * global_instance_ptr;

static void command_signal_handler(int signal_num GCC_UNUSED)
{
    instance_print_processed_FCCs( global_instance_ptr );
    instance_print_reached( global_instance_ptr );
}

int main(int argc, char * argv[])
{
    /* Temporarily #if'ed away until we finish working on instance_run_solver
     * */
    fcs_dbm_solver_instance_t instance;
    long caches_delta;
    long max_processed_positions_count = LONG_MAX;
    long positions_milestone_step = 100000;
    int num_threads;
    int arg;
    const char * filename;
    const char * out_filename = NULL;
    FILE * fh, * out_fh;
    char user_state[USER_STATE_SIZE];
    fcs_state_keyval_pair_t init_state;
    fcs_fcc_moves_seq_t ret_moves_seq, init_moves_seq;
    long FCCs_per_depth_milestone_step;
    int ret_code;
    fcs_fcc_moves_seq_allocator_t moves_list_allocator;
    fcs_compact_allocator_t moves_list_compact_alloc;
    fcs_meta_compact_allocator_t meta_alloc;
    DECLARE_IND_BUF_T(init_indirect_stacks_buffer)

    caches_delta = 1000000;
    num_threads = 2;
    FCCs_per_depth_milestone_step = 10000;
    global_instance_ptr = &instance;

    for (arg=1;arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--caches-delta"))
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
        else if (!strcmp(argv[arg], "-o"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "-o requires an argument!\n");
                exit(-1);
            }
            out_filename = argv[arg];
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

    if (out_filename)
    {
        if (!(out_fh = fopen(out_filename, "wt")))
        {
            fprintf(stderr, "Cannot open '%s' for writing.", out_filename);
            exit(-1);
        }
    }
    else
    {
        out_fh = stdout;
    }

    instance_init(
        &instance,
        max_processed_positions_count,
        positions_milestone_step,
        FCCs_per_depth_milestone_step,
        out_fh
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
        DECKS_NUM,
        init_indirect_stacks_buffer
    );

    fc_solve_meta_compact_allocator_init(&meta_alloc);
    fc_solve_compact_allocator_init(&(moves_list_compact_alloc), &(meta_alloc));
    moves_list_allocator.recycle_bin = NULL;
    moves_list_allocator.allocator = &(moves_list_compact_alloc);
    init_moves_seq.moves_list = NULL;
    init_moves_seq.count = 0;
    horne_prune(&init_state, &init_moves_seq, &moves_list_allocator);

#ifndef WIN32
    signal(SIGUSR1, command_signal_handler);
#endif

    ret_code = instance_run_solver(
        &instance,
        caches_delta,
        &init_state,
        &ret_moves_seq,
        &moves_list_allocator,
        &meta_alloc
    );

    if (ret_code == FCC_SOLVED)
    {
        int i;
        char move_buffer[500];
        const fcs_fcc_moves_list_item_t * iter;

        fprintf (out_fh, "VERDICT: %s\n", "Success!");
        /* Now trace the solution */
        iter = init_moves_seq.moves_list;
        for (i = 0 ; i < init_moves_seq.count ;)
        {
            fprintf(out_fh, "==\n%s\n",
                   move_to_string(
                       iter->data.s[i%FCS_FCC_NUM_MOVES_IN_ITEM],
                       move_buffer
                   )
            );
            if ((++i) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                iter = iter->next;
            }
        }
        iter = ret_moves_seq.moves_list;
        for (i = 0 ; i < ret_moves_seq.count ;)
        {
             fprintf(out_fh, "==\n%s\n",
                   move_to_string(
                       iter->data.s[i%FCS_FCC_NUM_MOVES_IN_ITEM],
                       move_buffer
                   )
            );
            if ((++i) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                iter = iter->next;
            }
        }
        fprintf (out_fh, "==\nEND\n");
    }
    else if (ret_code == FCC_IMPOSSIBLE)
    {
        fprintf (out_fh, "VERDICT: %s\n", "Could not solve successfully.");
    }
    else if (ret_code == FCC_INTERRUPTED)
    {
        fprintf (out_fh, "VERDICT: %s\n", "Interrupted run after limit reached.");
    }
    else
    {
        fprintf (out_fh, "VERDICT: %s\n", "Unknown return code. ERROR.");
    }

    instance_time_printf(
        &instance,
        "{FINAL} Reached %li positions in total.\n", instance.count_num_processed
        );

    instance_destroy(&instance);

    fc_solve_compact_allocator_finish(&moves_list_compact_alloc);
    fc_solve_meta_compact_allocator_finish(&meta_alloc);

    fclose(out_fh);

    return 0;
}
