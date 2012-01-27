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

/* TODO: make sure the key is '\0'-padded. */
static int fc_solve_compare_lru_cache_keys(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((((const fcs_cache_key_info_t *)(p))->key))
    return memcmp(GET_PARAM(void_a), GET_PARAM(void_b), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static void GCC_INLINE cache_init(fcs_lru_cache_t * cache, long max_num_elements_in_cache)
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    cache->states_values_to_keys_map = ((Pvoid_t) NULL);
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
    cache->kaz_tree = fc_solve_kaz_tree_create(fc_solve_compare_lru_cache_keys, NULL);
#else
#error Unknown FCS_RCS_CACHE_STORAGE
#endif

    fc_solve_compact_allocator_init(
        &(cache->states_values_to_keys_allocator)
    );
    cache->lowest_pri = NULL;
    cache->highest_pri = NULL;
    cache->recycle_bin = NULL;
    cache->count_elements_in_cache = 0;
    cache->max_num_elements_in_cache = max_num_elements_in_cache;
}

static void GCC_INLINE cache_destroy(fcs_lru_cache_t * cache)
{
    fc_solve_kaz_tree_destroy(cache->kaz_tree);
    fc_solve_compact_allocator_finish(&(cache->states_values_to_keys_allocator));
}

static fcs_bool_t GCC_INLINE cache_does_key_exist(fcs_lru_cache_t * cache, unsigned char * key)
{
    fcs_cache_key_info_t to_check;
    dnode_t * node;

    memcpy(to_check.key, key, sizeof(to_check.key));

    node = fc_solve_kaz_tree_lookup(cache->kaz_tree, &to_check);

    if (! node)
    {
        return FALSE;
    }
    else
    {
        /* First - promote this key to the top of the cache. */
        fcs_cache_key_info_t * existing;

        existing = (fcs_cache_key_info_t *)(node->dict_key);

        if (existing->higher_pri)
        {
            existing->higher_pri->lower_pri =
                existing->lower_pri;
            if (existing->lower_pri)
            {
                existing->lower_pri->higher_pri =
                    existing->higher_pri;
            }
            else
            {
                cache->lowest_pri = existing->higher_pri;
                /* Bug fix: keep the chain intact. */
                existing->higher_pri->lower_pri = NULL;
            }
            cache->highest_pri->higher_pri = existing;
            existing->lower_pri = cache->highest_pri;
            cache->highest_pri = existing;
            existing->higher_pri = NULL;
        }


        return TRUE;
    }
}

static void GCC_INLINE cache_insert(fcs_lru_cache_t * cache, unsigned char * key)
{
    fcs_cache_key_info_t * cache_key;
    dict_t * kaz_tree;

    kaz_tree = cache->kaz_tree;

    if (cache->count_elements_in_cache >= cache->max_num_elements_in_cache)
    {
        fc_solve_kaz_tree_delete_free(
            kaz_tree,
            fc_solve_kaz_tree_lookup(
                kaz_tree, (cache_key = cache->lowest_pri)
                )
            );
            
        cache->lowest_pri = cache->lowest_pri->higher_pri;
        cache->lowest_pri->lower_pri = NULL;
    }
    else
    {
        cache_key =
            (fcs_cache_key_info_t *)
            fcs_compact_alloc_ptr(
                &(cache->states_values_to_keys_allocator),
                sizeof(*cache_key)
            );
        cache->count_elements_in_cache++;
    }

    memcpy(cache_key->key, key, sizeof(cache_key->key));

    if (cache->highest_pri)
    {
        cache_key->lower_pri = cache->highest_pri;
        cache_key->higher_pri = NULL;
        cache->highest_pri->higher_pri = cache_key;
        cache->highest_pri = cache_key;
    }
    else
    {
        cache->highest_pri = cache->lowest_pri = cache_key;
        cache_key->higher_pri = cache_key->lower_pri = NULL;
    }

    fc_solve_kaz_tree_alloc_insert(kaz_tree, cache_key);

}

#ifndef FCS_DBM_WITHOUT_CACHES
static int fc_solve_compare_pre_cache_keys(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((((const fcs_pre_cache_key_val_pair_t *)(p))->key))
    return memcmp(GET_PARAM(void_a), GET_PARAM(void_b), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static void GCC_INLINE pre_cache_init(fcs_pre_cache_t * pre_cache_ptr)
{
    pre_cache_ptr->kaz_tree =
        fc_solve_kaz_tree_create(fc_solve_compare_pre_cache_keys, NULL);

    fc_solve_compact_allocator_init(&(pre_cache_ptr->kv_allocator));
    pre_cache_ptr->kv_recycle_bin = NULL;
    pre_cache_ptr->count_elements = 0;
}

static fcs_bool_t GCC_INLINE pre_cache_does_key_exist(
    fcs_pre_cache_t * pre_cache,
    unsigned char * key
    )
{
    fcs_pre_cache_key_val_pair_t to_check;

    memcpy(to_check.key, key, sizeof(to_check.key));

    return (fc_solve_kaz_tree_lookup(pre_cache->kaz_tree, &to_check) != NULL);
}

static fcs_bool_t GCC_INLINE pre_cache_lookup_parent_and_move(
    fcs_pre_cache_t * pre_cache,
    unsigned char * key,
    unsigned char * parent_and_move
    )
{
    fcs_pre_cache_key_val_pair_t to_check;
    dnode_t * node;

    memcpy(to_check.key, key, sizeof(to_check.key));

    node = fc_solve_kaz_tree_lookup(pre_cache->kaz_tree, &to_check);

    if (node)
    {
        memcpy(
            parent_and_move,
            ((fcs_pre_cache_key_val_pair_t *)(node->dict_key))->parent_and_move,
            sizeof(fcs_encoded_state_buffer_t)
        );
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void GCC_INLINE pre_cache_insert(
    fcs_pre_cache_t * pre_cache,
    unsigned char * key,
    unsigned char * parent_and_move
    )
{
    fcs_pre_cache_key_val_pair_t * to_insert;

    if (pre_cache->kv_recycle_bin)
    {
        pre_cache->kv_recycle_bin =
            (to_insert = pre_cache->kv_recycle_bin)->next;
    }
    else
    {
        to_insert =
            fcs_compact_alloc_ptr(
                &(pre_cache->kv_allocator),
                sizeof(*to_insert)
            );
    }
    memcpy(to_insert->key, key, sizeof(to_insert->key));
    memcpy(to_insert->parent_and_move, parent_and_move, 
            sizeof(to_insert->parent_and_move));

    fc_solve_kaz_tree_alloc_insert(pre_cache->kaz_tree, to_insert);
    pre_cache->count_elements++;
}

static void GCC_INLINE cache_populate_from_pre_cache(
    fcs_lru_cache_t * cache,
    fcs_pre_cache_t * pre_cache
)
{
    dnode_t * node;
    dict_t * kaz_tree;

    kaz_tree = pre_cache->kaz_tree;

    for (node = fc_solve_kaz_tree_first(kaz_tree);
            node ;
            node = fc_solve_kaz_tree_next(kaz_tree, node)
            )
    {
        cache_insert(
            cache, 
            ((fcs_pre_cache_key_val_pair_t *)(node->dict_key))->key
        );
    }
}

static void GCC_INLINE pre_cache_offload_and_destroy(
    fcs_pre_cache_t * pre_cache,
    fcs_dbm_store_t store,
    fcs_lru_cache_t * cache
)
{
    fc_solve_dbm_store_offload_pre_cache(store, pre_cache);
    cache_populate_from_pre_cache(cache, pre_cache);

    /* Now reset the pre_cache. */
    fc_solve_kaz_tree_destroy(pre_cache->kaz_tree);
    fc_solve_compact_allocator_finish(&(pre_cache->kv_allocator));
}

static void GCC_INLINE pre_cache_offload_and_reset(
    fcs_pre_cache_t * pre_cache,
    fcs_dbm_store_t store,
    fcs_lru_cache_t * cache
)
{
    pre_cache_offload_and_destroy(pre_cache, store, cache);
    pre_cache_init(pre_cache);
}

#endif /* FCS_DBM_WITHOUT_CACHES */

typedef unsigned char fcs_fcc_move_t;

struct fcs_dbm_queue_item_struct
{
    fcs_encoded_state_buffer_t key;
    int count_moves;
    fcs_fcc_move_t * moves;
    struct fcs_dbm_queue_item_struct * next;
};

typedef struct fcs_dbm_queue_item_struct fcs_dbm_queue_item_t;

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
    fcs_lock_t storage_lock;
#ifndef FCS_DBM_WITHOUT_CACHES
    fcs_pre_cache_t pre_cache;
    fcs_lru_cache_t cache;
#endif
    fcs_dbm_store_t store;

    long pre_cache_max_count;
    /* The queue */
    
    fcs_lock_t queue_lock;
    long count_num_processed;
    fcs_bool_t queue_solution_was_found;
    fcs_encoded_state_buffer_t queue_solution;
    fcs_compact_allocator_t queue_allocator;
    int queue_num_extracted_and_processed;
    /* TODO : offload the queue to the hard disk. */
    fcs_dbm_queue_item_t * queue_head, * queue_tail, * queue_recycle_bin;
    long num_states_in_collection;
} fcs_dbm_solver_instance_t;

static void GCC_INLINE instance_init(
    fcs_dbm_solver_instance_t * instance,
    long pre_cache_max_count,
    long caches_delta,
    const char * dbm_store_path
)
{
    FCS_INIT_LOCK(instance->queue_lock);
    FCS_INIT_LOCK(instance->storage_lock);

    fc_solve_compact_allocator_init(
        &(instance->queue_allocator)
    );
    instance->queue_solution_was_found = FALSE;
    instance->queue_num_extracted_and_processed = 0;
    instance->num_states_in_collection = 0;
    instance->count_num_processed = 0;
    instance->queue_head =
        instance->queue_tail =
        instance->queue_recycle_bin =
        NULL;

#ifndef FCS_DBM_WITHOUT_CACHES
    pre_cache_init (&(instance->pre_cache));
    instance->pre_cache_max_count = pre_cache_max_count;
    cache_init (&(instance->cache), pre_cache_max_count+caches_delta);
#endif
    fc_solve_dbm_store_init(&(instance->store), dbm_store_path);
}

static void GCC_INLINE instance_destroy(
    fcs_dbm_solver_instance_t * instance
    )
{
    /* TODO : store what's left on the queue on the hard-disk. */
    fc_solve_compact_allocator_finish(&(instance->queue_allocator));

#ifndef FCS_DBM_WITHOUT_CACHES
    pre_cache_offload_and_destroy(
        &(instance->pre_cache),
        instance->store,
        &(instance->cache)
    );

    cache_destroy(&(instance->cache));
#endif

    fc_solve_dbm_store_destroy(instance->store);

    FCS_DESTROY_LOCK(instance->queue_lock);
    FCS_DESTROY_LOCK(instance->storage_lock);
}

static void GCC_INLINE instance_check_key(
    fcs_dbm_solver_instance_t * instance,
    unsigned char * key,
    unsigned char * parent_and_move
)
{
#ifndef FCS_DBM_WITHOUT_CACHES
    fcs_lru_cache_t * cache;
    fcs_pre_cache_t * pre_cache;

    cache = &(instance->cache);
    pre_cache = &(instance->pre_cache);

    if (cache_does_key_exist(cache, key))
    {
        return;
    }
    else if (pre_cache_does_key_exist(pre_cache, key))
    {
        return;
    }
    else if (fc_solve_dbm_store_does_key_exist(instance->store, key))
    {
        cache_insert(cache, key);
        return;
    }
    else
#else
    if (fc_solve_dbm_store_insert_key_value(instance->store, key, parent_and_move))
#endif
    {
        fcs_dbm_queue_item_t * new_item;

#ifndef FCS_DBM_WITHOUT_CACHES
        pre_cache_insert(pre_cache, key, parent_and_move);
#endif

        /* Now insert it into the queue. */

        FCS_LOCK(instance->queue_lock);

        instance->num_states_in_collection++;
        
        if (instance->queue_recycle_bin)
        {
            instance->queue_recycle_bin = 
            (new_item = instance->queue_recycle_bin)->next;
        }
        else
        {
            new_item =
                (fcs_dbm_queue_item_t *)
                fcs_compact_alloc_ptr(
                    &(instance->queue_allocator),
                    sizeof(*new_item)
                );
        }

        memcpy(new_item->key, key, sizeof(new_item->key));
        new_item->next = NULL;

        if (instance->queue_tail)
        {
            instance->queue_tail = instance->queue_tail->next = new_item;
        }
        else
        {
            instance->queue_head = instance->queue_tail = new_item;
        }
        FCS_UNLOCK(instance->queue_lock);
    }
}

static void GCC_INLINE instance_check_multiple_keys(
    fcs_dbm_solver_instance_t * instance,
    fcs_derived_state_t * list
)
{
    /* Small optimization in case the list is empty. */
    if (!list)
    {
        return;
    }
    FCS_LOCK(instance->storage_lock);
    for (; list ; list = list->next)
    {
        instance_check_key(instance, list->key, list->parent_and_move);
    }
#ifndef FCS_DBM_WITHOUT_CACHES
    if (instance->pre_cache.count_elements >= instance->pre_cache_max_count)
    {
        pre_cache_offload_and_reset(
            &(instance->pre_cache),
            instance->store,
            &(instance->cache)
        );
    }
#endif
    FCS_UNLOCK(instance->storage_lock);
}

typedef struct {
    fcs_dbm_solver_instance_t * instance;
    fc_solve_delta_stater_t * delta_stater;
} fcs_dbm_solver_thread_t;

typedef struct {
    fcs_dbm_solver_thread_t * thread;
} thread_arg_t;

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
#ifdef INDIRECT_STACK_STATES
    dll_ind_buf_t indirect_stacks_buffer;
#endif
    fc_solve_bit_reader_t bit_r;
    fc_solve_bit_writer_t bit_w;

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
        fc_solve_bit_reader_init(&bit_r, item->key + 1);

        fc_solve_state_init(&state, STACKS_NUM
#ifdef INDIRECT_STACK_STATES
            , indirect_stacks_buffer
#endif
        );

        fc_solve_delta_stater_decode(
            delta_stater,
            &bit_r,
            &(state.s)
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
            memset(derived_iter->key, '\0', sizeof(derived_iter->key));
            fc_solve_bit_writer_init(&bit_w, derived_iter->key+1);
            fc_solve_delta_stater_set_derived(
                delta_stater, &(derived_iter->state.s)
            );
            fc_solve_delta_stater_encode_composite(delta_stater, &bit_w);
            derived_iter->key[0] =
                bit_w.current - bit_w.start + (bit_w.bit_in_char_idx > 0)
                ;
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

enum FCC_brfs_scan_type
{
    FIND_FCC_START_POINTS,
    FIND_MIN_BY_SORTING
};


struct fcs_FCC_start_point_struct
{
    fcs_encoded_state_buffer_t enc_state;
    int count_moves;
    /* Dynamically allocated. */
    fcs_fcc_move_t * moves;
    struct fcs_FCC_start_point_struct * next;
};

typedef struct fcs_FCC_start_point_struct fcs_FCC_start_point_t;

typedef struct {
    fcs_FCC_start_point_t * list;
    fcs_compact_allocator_t allocator;
    fcs_FCC_start_point_t * recycle_bin;
} fcs_FCC_start_points_list_t;

static void perform_FCC_brfs(
    /* The first state in the game, from which all states are encoded. */
    fcs_state_keyval_pair_t * init_state,
    /* The start state. */
    fcs_encoded_state_buffer_t start_state,
    /* The moves leading up to the state. */
    const int count_start_state_moves,
    const fcs_fcc_move_t * const start_state_moves,
    /* Type of scan. */
    enum FCC_brfs_scan_type scan_type,
    /* [Output]: FCC start points. 
     * Will be NULL if scan_type is not FIND_FCC_START_POINTS
     * */
    fcs_FCC_start_points_list_t * fcc_start_points,
    /* [Output]: Is the min_by_sorting new.
     * Will be NULL if scan_type is not FIND_MIN_BY_SORTING
     * */
    fcs_bool_t * is_min_by_sorting_new,
    /* [Output]: The min_by_sorting.
     * Will be NULL if scan_type is not FIND_MIN_BY_SORTING
     * */
    fcs_encoded_state_buffer_t * min_by_sorting,
    /* [Input/Output]: The ${next}_depth_FCCs.DoesExist
     * (for the right depth based on the current depth and pruning.)
     * Of type Map{min_by_sorting => Bool Exists} DoesExist.
     * Will be NULL if scan_type is not FIND_MIN_BY_SORTING
    */
    dict_t * does_min_by_sorting_exist,
    /* [Input/Output]: The LRU.
     * Of type <<LRU_Map{any_state_in_the_FCCs => Bool Exists} Cache>>
     * Will be NULL if scan_type is not FIND_MIN_BY_SORTING
     * */
    fcs_lru_cache_t * does_state_exist_in_any_FCC_cache
)
{
    fcs_dbm_queue_item_t * queue_head, * queue_tail, * queue_recycle_bin, * new_item, * extracted_item;
    fcs_compact_allocator_t queue_allocator, derived_list_allocator;
    fcs_derived_state_t * derived_list, * derived_list_recycle_bin,
                        * derived_iter, * next_derived_iter;
    dict_t * traversed_states, * found_new_start_points;
    fc_solve_bit_reader_t bit_r;
    fc_solve_bit_writer_t bit_w;
    fc_solve_delta_stater_t * delta_stater;
    fcs_state_keyval_pair_t state;
    fcs_bool_t running_min_was_assigned = FALSE;
    fcs_encoded_state_buffer_t running_min;

#ifdef INDIRECT_STACK_STATES
    dll_ind_buf_t indirect_stacks_buffer;
#endif

    /* Some sanity checks. */
#ifndef NDEBUG
    if (scan_type == FIND_FCC_START_POINTS)
    {
        assert(fcc_start_points);
        assert(! is_min_by_sorting_new);
        assert(! min_by_sorting);
        assert(! does_min_by_sorting_exist);
        assert(! does_state_exist_in_any_FCC_cache);
    }
    else
    {
        assert(! fcc_start_points);
        assert(is_min_by_sorting_new);
        assert(min_by_sorting);
        assert(does_min_by_sorting_exist);
        assert(does_state_exist_in_any_FCC_cache);
    }
#endif

    /* Initialize the queue_allocator. */
    fc_solve_compact_allocator_init( &(queue_allocator) );
    fc_solve_compact_allocator_init( &(derived_list_allocator) );
    queue_recycle_bin = NULL;

    /* TODO : maybe pass delta_stater as an argument  */
    delta_stater = fc_solve_delta_stater_alloc(
        &(init_state->s),
        STACKS_NUM,
        FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
        , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
    );

    new_item =
        (fcs_dbm_queue_item_t *)
        fcs_compact_alloc_ptr(
            &(queue_allocator),
            sizeof(*new_item)
        );

    memcpy(new_item->key, start_state, sizeof(new_item->key));
    new_item->next = NULL;

    queue_head = queue_tail = new_item;

    derived_list_recycle_bin = NULL;

    /* Extract an item from the queue. */
    while ((extracted_item = queue_head))
    {
        if (! (queue_head = extracted_item->next) )
        {
            queue_tail = NULL;
        }

        /* Calculate the derived list. */
        derived_list = NULL;

        /* Handle item. */
        fc_solve_bit_reader_init(&bit_r, extracted_item->key + 1);

        fc_solve_state_init(&state, STACKS_NUM
#ifdef INDIRECT_STACK_STATES
            , indirect_stacks_buffer
#endif
        );

        fc_solve_delta_stater_decode(
            delta_stater,
            &bit_r,
            &(state.s)
        );

        instance_solver_thread_calc_derived_states(
            &state,
            &(extracted_item->key),
            &derived_list,
            &derived_list_recycle_bin,
            &derived_list_allocator,
            TRUE
        );

        /* Handle the min_by_sorting scan. */
        if (scan_type == FIND_MIN_BY_SORTING)
        {
            if (cache_does_key_exist(
                does_state_exist_in_any_FCC_cache, 
                extracted_item->key
            ))
            {
                *is_min_by_sorting_new = FALSE;
                goto free_resources;
            }
            else
            {
                cache_insert(does_state_exist_in_any_FCC_cache, extracted_item->key);
            }

            if (! running_min_was_assigned)
            {
                running_min_was_assigned = TRUE;
                memcpy(running_min, extracted_item->key, sizeof(running_min));
            }
            else
            {
                if (memcmp(extracted_item->key, running_min, sizeof(running_min)) < 0)
                {
                    memcpy(running_min, extracted_item->key, sizeof(running_min));
                }
            }
        }

        /* Allocate a spare 'new_item'. */
        if (queue_recycle_bin)
        {
            new_item = queue_recycle_bin;
            queue_recycle_bin = queue_recycle_bin->next;
        }
        else
        {
            new_item =
                (fcs_dbm_queue_item_t *)
                fcs_compact_alloc_ptr(
                    &(queue_allocator),
                    sizeof(*new_item)
                );
        }

        /* Enqueue the new elements to the queue. */
        for (derived_iter = derived_list;
                derived_iter ;
                derived_iter = next_derived_iter
        )
        {
            fcs_bool_t is_reversible = derived_iter->is_reversible_move;
            if (is_reversible || (scan_type == FIND_FCC_START_POINTS))
            {
                dict_t * right_tree;

                right_tree = (is_reversible ? traversed_states : found_new_start_points);

                memset(new_item->key, '\0', sizeof(new_item->key));
                fc_solve_bit_writer_init(&bit_w, new_item->key+1);
                fc_solve_delta_stater_set_derived(
                        delta_stater, &(derived_iter->state.s)
                        );
                fc_solve_delta_stater_encode_composite(delta_stater, &bit_w);
                new_item->key[0] =
                    bit_w.current - bit_w.start + (bit_w.bit_in_char_idx > 0)
                    ;
                if (! fc_solve_kaz_tree_lookup(
                    right_tree,
                    &(new_item->key)
                    )
                )
                {
                    fc_solve_kaz_tree_alloc_insert(
                        right_tree,
                        &(new_item->key)
                    );
                    if (scan_type == FIND_FCC_START_POINTS)
                    {
                        int count_moves;
                        fcs_fcc_move_t * moves;
                        /* Fill in the moves. */
                        moves = malloc(
                                sizeof(new_item->moves[0])
                                * (count_moves = (extracted_item->count_moves + 1))
                                );
                        memcpy(moves, extracted_item->moves, extracted_item->count_moves * sizeof(new_item->moves[0]));
                        moves[extracted_item->count_moves]
                            = derived_iter->parent_and_move[
                            derived_iter->parent_and_move[0]
                            ];

                        if (is_reversible)
                        {
                            new_item->moves = moves;
                            new_item->count_moves = count_moves;
                        }
                        else
                        {
                            fcs_FCC_start_point_t * new_start_point;
                            /* Enqueue the new FCC start point. */
                            if (fcc_start_points->recycle_bin)
                            {
                                new_start_point = fcc_start_points->recycle_bin;
                                fcc_start_points->recycle_bin = fcc_start_points->recycle_bin->next;
                            }
                            else
                            {
                                new_start_point = (fcs_FCC_start_point_t *)
                                    fcs_compact_alloc_ptr(
                                        &(fcc_start_points->allocator),
                                        sizeof (*new_start_point)
                                    );
                            }
                            memcpy(
                                new_start_point->enc_state,
                                new_item->key,
                                sizeof(new_item->key)
                            );
                            new_start_point->count_moves = count_moves;
                            new_start_point->moves = moves;
                        }
                    }

                    if (is_reversible)
                    {
                        /* Enqueue the item in the queue. */
                        new_item->next = NULL;
                        if (queue_tail)
                        {
                            queue_tail = queue_tail->next = new_item;
                        }
                        else
                        {
                            queue_head = queue_tail = new_item;
                        }
                    }
                    /* Allocate a new new_item */
                    if (queue_recycle_bin)
                    {
                        new_item = queue_recycle_bin;
                        queue_recycle_bin = queue_recycle_bin->next;
                    }
                    else
                    {
                        new_item =
                            (fcs_dbm_queue_item_t *)
                            fcs_compact_alloc_ptr(
                                &(queue_allocator),
                                sizeof(*new_item)
                            );
                    }
                }
            }
            /* Recycle derived_iter.  */
            next_derived_iter = derived_iter->next;
            derived_iter->next = derived_list_recycle_bin;
            derived_list_recycle_bin = derived_iter;
        }

        /* We always hold one new_item spare, so let's recycle i now. */
        new_item->next = queue_recycle_bin;
        queue_recycle_bin = new_item;

        /* Clean up the extracted_item's resources. We no longer need them
         * because we are interested only in those of the derived items.
         * */
        free(extracted_item->moves);
        extracted_item->moves = NULL;
    }

    if (scan_type == FIND_MIN_BY_SORTING)
    {
        if ((*is_min_by_sorting_new = (!fc_solve_kaz_tree_lookup(does_min_by_sorting_exist, running_min))))
        {
            memcpy((*min_by_sorting), running_min, sizeof(running_min));
        }
    }

    /* Free the allocated resources. */
free_resources:
    fc_solve_compact_allocator_finish(&(queue_allocator));
    fc_solve_compact_allocator_finish(&(derived_list_allocator));
    fc_solve_delta_stater_free(delta_stater);
}

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
    fcs_dbm_solver_instance_t instance;
    long pre_cache_max_count;
    long caches_delta;
    const char * dbm_store_path;
    int num_threads;
    int arg;
    const char * filename;
    FILE * fh;
    char user_state[USER_STATE_SIZE];
    fc_solve_delta_stater_t * delta;
    fcs_state_keyval_pair_t init_state;
#ifdef INDIRECT_STACK_STATES
    dll_ind_buf_t init_indirect_stacks_buffer;
#endif
    fc_solve_bit_writer_t bit_w;

    pre_cache_max_count = 1000000;
    caches_delta = 1000000;
    dbm_store_path = "./fc_solve_dbm_store";
    num_threads = 2;

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
        else if (!strcmp(argv[arg], "--dbm-store-path"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--dbm-store-path came without an argument.\n");
                exit(-1);
            }
            dbm_store_path = argv[arg];
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

    instance_init(&instance, pre_cache_max_count, caches_delta, dbm_store_path);
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

    horne_prune(&init_state);

    delta = fc_solve_delta_stater_alloc(
            &init_state.s,
            STACKS_NUM,
            FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
            , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
    );

    fc_solve_delta_stater_set_derived(delta, &(init_state.s));
    
    {
        fcs_dbm_queue_item_t * first_item;
        fcs_encoded_state_buffer_t parent_and_move;

        first_item =
            (fcs_dbm_queue_item_t *)
                fcs_compact_alloc_ptr(
                    &(instance.queue_allocator),
                    sizeof(*first_item)
                );

        first_item->next = NULL;
        memset(first_item->key, '\0', sizeof(first_item->key));

        fc_solve_bit_writer_init(&bit_w, first_item->key+1);
        fc_solve_delta_stater_encode_composite(delta, &bit_w);
        first_item->key[0] =
            bit_w.current - bit_w.start + (bit_w.bit_in_char_idx > 0)
            ;

        /* The NULL parent and move for indicating this is the initial
         * state. */
        memset(parent_and_move, '\0', sizeof(parent_and_move));

#ifndef FCS_DBM_WITHOUT_CACHES
        pre_cache_insert(&(instance.pre_cache), first_item->key, parent_and_move);
#else
        fc_solve_dbm_store_insert_key_value(instance.store, first_item->key, parent_and_move);
#endif
        instance.num_states_in_collection++;
        instance.queue_head = instance.queue_tail = first_item;
    }
    {
        int i, check;
        main_thread_item_t * threads;

        threads = malloc(sizeof(threads[0]) * num_threads);

        for (i=0; i < num_threads ; i++)
        {
            threads[i].thread.instance = &(instance);
            threads[i].thread.delta_stater =
                fc_solve_delta_stater_alloc(
                    &(init_state.s),
                    STACKS_NUM,
                    FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
                    , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
                );
            threads[i].arg.thread = &(threads[i].thread);
            check = pthread_create(
                &(threads[i].id),
                NULL,
                instance_run_solver_thread,
                &(threads[i].arg)
            );

            if (check)
            {
                fprintf(stderr,
                        "Worker Thread No. %d Initialization failed!\n",
                        i
                       );
                exit(-1);
            }
        }

        for (i=0; i < num_threads ; i++)
        {
            pthread_join(threads[i].id, NULL);
            fc_solve_delta_stater_free(threads[i].thread.delta_stater);
        }
        free(threads);
    }

    if (instance.queue_solution_was_found)
    {
        fcs_encoded_state_buffer_t * trace;
        fcs_encoded_state_buffer_t key;
        int trace_num, trace_max_num;
        int i;
        fcs_state_keyval_pair_t state;
        fc_solve_bit_reader_t bit_r;
        unsigned char move;
        char * state_as_str;
        char move_buffer[500];
#ifdef INDIRECT_STACK_STATES
        dll_ind_buf_t indirect_stacks_buffer;
#endif
#ifdef FCS_WITHOUT_LOCS_FIELDS
        fcs_state_locs_struct_t locs;
#endif
        printf ("%s\n", "Success!");
        /* Now trace the solution */
#define GROW_BY 100
        trace_num = 0;
        trace = malloc(sizeof(trace[0]) * (trace_max_num = GROW_BY));
        memcpy(trace[trace_num], instance.queue_solution,
               sizeof(trace[0]));
        while (trace[trace_num][0])
        {
            memset(key, '\0', sizeof(key));
            /* Omit the move. */
            memcpy(key, trace[trace_num], trace[trace_num][0]+1);

            if ((++trace_num) == trace_max_num)
            {
                trace = realloc(trace, sizeof(trace[0]) * (trace_max_num += GROW_BY));
            }
#ifndef FCS_DBM_WITHOUT_CACHES
            if (! pre_cache_lookup_parent_and_move(
                &(instance.pre_cache),
                key,
                trace[trace_num]
                ))
            {
#endif
                assert(fc_solve_dbm_store_lookup_parent_and_move(
                    instance.store,
                    key,
                    trace[trace_num]
                    ));
#ifndef FCS_DBM_WITHOUT_CACHES
            }
#endif
        }
#ifdef FCS_WITHOUT_LOCS_FIELDS
        for (i=0 ; i < MAX_NUM_STACKS ; i++)
        {
            locs.stack_locs[i] = i;
        }
        for (i=0 ; i < MAX_NUM_FREECELLS ; i++)
        {
            locs.fc_locs[i] = i;
        }
#endif
        for (i = trace_num-1 ; i >= 0; i--)
        {
            fc_solve_state_init(&state, STACKS_NUM
#ifdef INDIRECT_STACK_STATES
                , indirect_stacks_buffer
#endif
            );

            fc_solve_bit_reader_init(&bit_r, &(trace[i][1]));
            fc_solve_delta_stater_decode(
                delta,
                &bit_r,
                &(state.s)
            );
            if (i > 0)
            {
                move = trace[i][1+trace[i][0]];
            }

            state_as_str =
                fc_solve_state_as_string(
#ifdef FCS_RCS_STATES
                        &(state.s),
                        &(state.info),
#else
                        &state,
#endif
#ifdef FCS_WITHOUT_LOCS_FIELDS
                        &locs,
#endif
                        FREECELLS_NUM,
                        STACKS_NUM,
                        DECKS_NUM,
                        1,
                        0,
                        1
                );

            printf("--------\n%s\n==\n%s\n",
                    state_as_str,
                    (i > 0 )
                        ? move_to_string(move, move_buffer)
                        : "END"
                  );

            free(state_as_str);
        }
        free (trace);
    }
    else
    {
        printf ("%s\n", "Could not solve successfully.");
    }
    
    instance_destroy(&instance);

    return 0;
}
