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
 * depth_dbm_solver.c - a specialised solver that offloads the states'
 * collection to an on-disk DBM database such as Berkeley DB or Google's
 * LevelDB. Has been adapted to be completely in-memory. It makes use of
 * delta_states.c for a very compact storage.
 *
 * In addition, this solver implements the scheme in:
 * http://tech.groups.yahoo.com/group/fc-solve-discuss/message/1135
 *
 */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <limits.h>

#if 0
#define DEBUG_FOO
#endif

/*
 * Define FCS_DBM_SINGLE_THREAD to have single thread-per-instance traversal.
 */
#if 0
#define FCS_DBM_SINGLE_THREAD 1
#endif

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
#include "dbm_cache.h"

#define FCS_DBM_USE_OFFLOADING_QUEUE

#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
#include "offloading_queue.h"
#endif

#ifdef FCS_DBM_USE_OFFLOADING_QUEUE

#if (!defined(FCS_DBM_WITHOUT_CACHES))
#error FCS_DBM_USE_OFFLOADING_QUEUE requires FCS_DBM_WITHOUT_CACHES
#endif

#if (defined(FCS_DBM_CACHE_ONLY))
#error FCS_DBM_USE_OFFLOADING_QUEUE is not compatible with FCS_DBM_CACHE_ONLY
#endif

#endif

#define T

#ifdef T
#define TRACE0(message) \
        { \
            fprintf(out_fh, "%s\n", message); \
            fflush(out_fh); \
        }
#define TRACE1(my_format, arg1) \
        { \
            fprintf(out_fh, my_format, arg1); \
            fflush(out_fh); \
        }
#endif

#ifdef DEBUG_FOO
fc_solve_delta_stater_t * global_delta_stater;
#endif

#ifndef FCS_DBM_WITHOUT_CACHES

static int fc_solve_compare_pre_cache_keys(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((((const fcs_pre_cache_key_val_pair_t *)(p))->key))
    return memcmp(&(GET_PARAM(void_a)), &(GET_PARAM(void_b)), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static GCC_INLINE void pre_cache_init(fcs_pre_cache_t * pre_cache_ptr, fcs_meta_compact_allocator_t * meta_alloc)
{
    pre_cache_ptr->kaz_tree =
        fc_solve_kaz_tree_create(fc_solve_compare_pre_cache_keys, NULL, meta_alloc);

    fc_solve_compact_allocator_init(&(pre_cache_ptr->kv_allocator), meta_alloc);
    pre_cache_ptr->kv_recycle_bin = NULL;
    pre_cache_ptr->count_elements = 0;
}

static GCC_INLINE fcs_bool_t pre_cache_does_key_exist(
    fcs_pre_cache_t * pre_cache,
    fcs_encoded_state_buffer_t * key
    )
{
    fcs_pre_cache_key_val_pair_t to_check;

    to_check.key = *key;

    return (fc_solve_kaz_tree_lookup_value(pre_cache->kaz_tree, &to_check) != NULL);
}

static GCC_INLINE fcs_bool_t pre_cache_lookup_parent(
    fcs_pre_cache_t * pre_cache,
    fcs_encoded_state_buffer_t * key,
    fcs_encoded_state_buffer_t * parent
    )
{
    fcs_pre_cache_key_val_pair_t to_check;
    dict_key_t found_key;

    to_check.key = *key;

    found_key = fc_solve_kaz_tree_lookup_value(pre_cache->kaz_tree, &to_check);

    if (found_key)
    {
        *parent =
            ((fcs_pre_cache_key_val_pair_t *)(found_key))->parent;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static GCC_INLINE void pre_cache_insert(
    fcs_pre_cache_t * pre_cache,
    fcs_encoded_state_buffer_t * key,
    fcs_encoded_state_buffer_t * parent
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
    to_insert->key = *key;
    to_insert->parent = *parent;

    fc_solve_kaz_tree_alloc_insert(pre_cache->kaz_tree, to_insert);
    pre_cache->count_elements++;
}

#ifndef FCS_DBM_CACHE_ONLY
static GCC_INLINE void cache_populate_from_pre_cache(
    fcs_lru_cache_t * cache,
    fcs_pre_cache_t * pre_cache
)
{
#ifdef FCS_DBM_USE_LIBAVL
    dict_t * kaz_tree;
    struct avl_traverser trav;
    dict_key_t item;

    kaz_tree = pre_cache->kaz_tree;
    avl_t_init(&trav, kaz_tree);

    for (
        item = avl_t_first(&trav, kaz_tree)
            ;
        item
            ;
        item = avl_t_next(&trav)
        )
    {
        cache_insert(
            cache,
            &(((fcs_pre_cache_key_val_pair_t *)(item))->key),
            NULL,
            '\0'
        );
    }
#else
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
            &(((fcs_pre_cache_key_val_pair_t *)(node->dict_key))->key)
        );
    }
#endif
}

static GCC_INLINE void pre_cache_offload_and_destroy(
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

static GCC_INLINE void pre_cache_offload_and_reset(
    fcs_pre_cache_t * pre_cache,
    fcs_dbm_store_t store,
    fcs_lru_cache_t * cache,
    fcs_meta_compact_allocator_t * meta_alloc
)
{
    pre_cache_offload_and_destroy(pre_cache, store, cache);
    pre_cache_init(pre_cache, meta_alloc);
}

#endif

#endif /* FCS_DBM_WITHOUT_CACHES */
static const pthread_mutex_t initial_mutex_constant =
    PTHREAD_MUTEX_INITIALIZER
    ;

#ifdef FCS_DBM_SINGLE_THREAD
typedef fcs_bool_t fcs_lock_t;
#define FCS_LOCK(lock) {}
#define FCS_UNLOCK(lock) {}
#define FCS_INIT_LOCK(lock) {}
#define FCS_DESTROY_LOCK(lock) {}
#elif defined(FCS_DBM_USE_RWLOCK)
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

enum TERMINATE_REASON
{
    DONT_TERMINATE = 0,
    QUEUE_TERMINATE,
    MAX_ITERS_TERMINATE,
    SOLUTION_FOUND_TERMINATE
};

#define FCC_DEPTH (RANK_KING * 4 * DECKS_NUM * 2)
typedef struct
{
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
    fcs_pre_cache_t pre_cache;
#endif
    fcs_lru_cache_t cache;
#endif
#ifndef FCS_DBM_CACHE_ONLY
    fcs_dbm_store_t store;
#endif
    fcs_lock_t queue_lock;
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
    fcs_offloading_queue_t queue;
#else
    fcs_compact_allocator_t queue_allocator;
    fcs_dbm_queue_item_t * queue_head, * queue_tail, * queue_recycle_bin;
#endif
} fcs_dbm_collection_by_depth_t;

typedef struct
{
    fcs_lock_t global_lock;
    void * tree_recycle_bin;
    fcs_lock_t storage_lock;
    const char * offload_dir_path;
    int curr_depth;
    fcs_dbm_collection_by_depth_t colls_by_depth[FCC_DEPTH];
    long pre_cache_max_count;
    /* The queue */
    long count_num_processed, count_of_items_in_queue, max_count_num_processed;
    long max_count_of_items_in_queue;
    fcs_bool_t queue_solution_was_found;
    enum TERMINATE_REASON should_terminate;
#ifdef FCS_DBM_WITHOUT_CACHES
    fcs_dbm_record_t * queue_solution_ptr;
    fcs_dbm_record_t physical_queue_solution;
#else
    fcs_encoded_state_buffer_t queue_solution;

#endif
    fcs_meta_compact_allocator_t instance_meta_alloc;
    int queue_num_extracted_and_processed;
    long num_states_in_collection;
    FILE * out_fh;
    fcs_encoded_state_buffer_t first_key;
} fcs_dbm_solver_instance_t;

static GCC_INLINE void instance_init(
    fcs_dbm_solver_instance_t * instance,
    long pre_cache_max_count,
    long caches_delta,
    const char * dbm_store_path,
    long max_count_of_items_in_queue,
    long iters_delta_limit,
    const char * offload_dir_path,
    FILE * out_fh
)
{
    int depth;
    fcs_dbm_collection_by_depth_t * coll;

    instance->curr_depth = 0;
    FCS_INIT_LOCK(instance->global_lock);
    instance->offload_dir_path = offload_dir_path;

    instance->out_fh = out_fh;

    fc_solve_meta_compact_allocator_init(
        &(instance->instance_meta_alloc)
    );
    instance->queue_solution_was_found = FALSE;
    instance->should_terminate = DONT_TERMINATE;
    instance->queue_num_extracted_and_processed = 0;
    instance->num_states_in_collection = 0;
    instance->count_num_processed = 0;
    if (iters_delta_limit >= 0)
    {
        instance->max_count_num_processed =
            instance->count_num_processed + iters_delta_limit;
    }
    else
    {
        instance->max_count_num_processed = LONG_MAX;
    }
    instance->count_of_items_in_queue = 0;
    instance->max_count_of_items_in_queue = max_count_of_items_in_queue;
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
    instance->queue_head =
        instance->queue_tail =
        instance->queue_recycle_bin =
        NULL;
#endif
    instance->tree_recycle_bin = NULL;

    FCS_INIT_LOCK(instance->storage_lock);
    for (depth = 0 ; depth < FCC_DEPTH ; depth++)
    {
        coll = &(instance->colls_by_depth[depth]);
        FCS_INIT_LOCK(coll->queue_lock);
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
#define NUM_ITEMS_PER_PAGE (128 * 1024)
        fcs_offloading_queue__init(&(coll->queue), NUM_ITEMS_PER_PAGE, offload_dir_path, depth);
#else
        fc_solve_compact_allocator_init(
            &(coll->queue_allocator), &(instance->meta_alloc)
        );
#endif
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
        pre_cache_init (&(coll->pre_cache), &(coll->meta_alloc));
#endif
        coll->pre_cache_max_count = pre_cache_max_count;
        cache_init (&(coll->cache), pre_cache_max_count+caches_delta, &(coll->meta_alloc));
#endif
#ifndef FCS_DBM_CACHE_ONLY
        fc_solve_dbm_store_init(&(coll->store), dbm_store_path, &(instance->tree_recycle_bin));
#endif
    }

}

#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
static GCC_INLINE void instance_dealloc_queue_moves_to_key(
    fcs_dbm_solver_instance_t * instance
)
{
    int i;
#define NUM_CHAINS_TO_RELEASE 2
    fcs_dbm_queue_item_t * to_release[NUM_CHAINS_TO_RELEASE];

    to_release[0] = instance->queue_recycle_bin;
    to_release[1] = instance->queue_head;

    for (i=0 ; i < NUM_CHAINS_TO_RELEASE ; i++)
    {
        fcs_dbm_queue_item_t * item;

        for (item = to_release[i] ;
             item ;
             item = item->next)
        {
            free(item->moves_to_key);
            item->moves_to_key = NULL;
        }
    }
#undef NUM_CHAINS_TO_RELEASE
}
#endif

static GCC_INLINE void instance_recycle(
    fcs_dbm_solver_instance_t * instance
    )
{
     int depth;

     for (depth = instance->curr_depth ; depth < FCC_DEPTH ; depth++)
     {
         fcs_dbm_collection_by_depth_t * coll;

         coll = &(instance->colls_by_depth[depth]);
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
         fcs_offloading_queue__destroy(&(coll->queue));
         fcs_offloading_queue__init(&(coll->queue), NUM_ITEMS_PER_PAGE, instance->offload_dir_path, depth);
#else
         instance_dealloc_queue_moves_to_key(instance);
         /* TODO : store what's left on the queue on the hard-disk. */
         fc_solve_compact_allocator_finish(&(coll->queue_allocator));
         fc_solve_compact_allocator_init(
             &(coll->queue_allocator), &(coll->meta_alloc)
             );
#endif
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
    coll->queue_head =
        coll->queue_tail =
        coll->queue_recycle_bin =
        NULL;
#endif
     }

    instance->should_terminate = DONT_TERMINATE;
    instance->queue_num_extracted_and_processed = 0;
    instance->num_states_in_collection = 0;
    instance->count_num_processed = 0;
    instance->count_of_items_in_queue = 0;
}

static GCC_INLINE void instance_destroy(
    fcs_dbm_solver_instance_t * instance
    )
{
    int depth;
    fcs_dbm_collection_by_depth_t * coll;

    for (depth = 0 ; depth < FCC_DEPTH ; depth++)
    {
        coll = &(instance->colls_by_depth[depth]);
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
        fcs_offloading_queue__destroy(&(coll->queue));
#else
        instance_dealloc_queue_moves_to_key(coll);
        /* TODO : store what's left on the queue on the hard-disk. */
        fc_solve_compact_allocator_finish(&(coll->queue_allocator));
#endif

#ifndef FCS_DBM_WITHOUT_CACHES

#ifndef FCS_DBM_CACHE_ONLY
        pre_cache_offload_and_destroy(
            &(coll->pre_cache),
            coll->store,
            &(coll->cache)
            );
#endif

        cache_destroy(&(coll->cache));
#endif

#ifndef FCS_DBM_CACHE_ONLY
        fc_solve_dbm_store_destroy(coll->store);
#endif
        FCS_DESTROY_LOCK(coll->queue_lock);
    }
    FCS_DESTROY_LOCK(instance->storage_lock);

    fc_solve_meta_compact_allocator_finish(
        &(instance->instance_meta_alloc)
    );
}

static GCC_INLINE void instance_check_key(
    fcs_dbm_solver_instance_t * instance,
    int key_depth,
    fcs_encoded_state_buffer_t * key,
    fcs_dbm_record_t * parent,
    unsigned char move
#ifdef FCS_DBM_CACHE_ONLY
    , const fcs_fcc_move_t * moves_to_parent
#endif
)
{
    fcs_dbm_collection_by_depth_t * coll;
    coll = &(instance->colls_by_depth[key_depth]);
    {
#ifdef FCS_DBM_WITHOUT_CACHES
        fcs_dbm_record_t * token;
#endif
#ifndef FCS_DBM_WITHOUT_CACHES
        fcs_lru_cache_t * cache;
#ifndef FCS_DBM_CACHE_ONLY
        fcs_pre_cache_t * pre_cache;
#endif

        cache = &(instance->cache);
#ifndef FCS_DBM_CACHE_ONLY
        pre_cache = &(instance->pre_cache);
#endif

        if (cache_does_key_exist(cache, key))
        {
            return;
        }
#ifndef FCS_DBM_CACHE_ONLY
        else if (pre_cache_does_key_exist(pre_cache, key))
        {
            return;
        }
#endif
#ifndef FCS_DBM_CACHE_ONLY
        else if (fc_solve_dbm_store_does_key_exist(instance->store, key->s))
        {
            cache_insert(cache, key, NULL, '\0');
            return;
        }
#endif
        else
#else
            if ((token = fc_solve_dbm_store_insert_key_value(coll->store, key, parent)))
#endif
            {
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
                fcs_dbm_queue_item_t * new_item;
#endif
#ifdef FCS_DBM_CACHE_ONLY
                fcs_cache_key_info_t * cache_key;
#endif

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
                pre_cache_insert(pre_cache, key, parent);
#else
                cache_key = cache_insert(cache, key, moves_to_parent, move);
#endif
#endif

                /* Now insert it into the queue. */

                FCS_LOCK(coll->queue_lock);


#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
                fcs_offloading_queue__insert(
                    &(coll->queue),
                    (const fcs_offloading_queue_item_t *)(&token)
                    );
#else
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
#ifdef FCS_DBM_CACHE_ONLY
                    new_item->moves_to_key = NULL;
#endif
                }

                new_item->key = (*key);
                new_item->next = NULL;
#ifdef FCS_DBM_CACHE_ONLY
                new_item->moves_to_key = realloc(
                    new_item->moves_to_key,
                    strlen((const char *)cache_key->moves_to_key)+1
                    );
                strcpy((char *)new_item->moves_to_key, (const char *)cache_key->moves_to_key);
#endif

                if (coll->queue_tail)
                {
                    coll->queue_tail = coll->queue_tail->next = new_item;
                }
                else
                {
                    coll->queue_head = coll->queue_tail = new_item;
                }
#endif
                FCS_UNLOCK(coll->queue_lock);

                FCS_LOCK(instance->global_lock);
                instance->count_of_items_in_queue++;
#ifdef DEBUG_FOO
                {
                    char * state_str;
                    fcs_state_keyval_pair_t state;
                    fcs_state_locs_struct_t locs;
                    DECLARE_IND_BUF_T(indirect_stacks_buffer)

                    fc_solve_init_locs(&locs);
                    /* Handle item. */
                    fc_solve_delta_stater_decode_into_state(
                        global_delta_stater,
                        token->key.s,
                        &state,
                        indirect_stacks_buffer
                    );

                    state_str = fc_solve_state_as_string(
                        &(state.s),
                        &(state.info),
                        &locs,
                        2,
                        8,
                        1,
                        1,
                        0,
                        1
                        );

#if 1
                    fprintf(instance->out_fh, "Found State:\n<<<\n%s>>>\n", state_str);
                    fflush(instance->out_fh);
#endif
                    free(state_str);
                }
#endif
                instance->num_states_in_collection++;
                FCS_UNLOCK(instance->global_lock);
            }
    }
}

static GCC_INLINE void instance_check_multiple_keys(
    fcs_dbm_solver_instance_t * instance,
    fcs_derived_state_t * list
#ifdef FCS_DBM_CACHE_ONLY
    , const fcs_fcc_move_t * moves_to_parent
#endif
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
        instance_check_key(
            instance,
            ( instance->curr_depth +
              list->num_non_reversible_moves_including_prune),
            &(list->key), list->parent, list->move
#ifdef FCS_DBM_CACHE_ONLY
            , moves_to_parent
#endif
        );
    }
    FCS_UNLOCK(instance->storage_lock);
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
    if (instance->pre_cache.count_elements >= instance->pre_cache_max_count)
    {
        pre_cache_offload_and_reset(
            &(instance->pre_cache),
            instance->store,
            &(instance->cache),
            &(instance->meta_alloc)
        );
    }
#endif
#endif
}

typedef struct {
    fcs_dbm_solver_instance_t * instance;
    fc_solve_delta_stater_t * delta_stater;
    fcs_meta_compact_allocator_t thread_meta_alloc;
} fcs_dbm_solver_thread_t;

typedef struct {
    fcs_dbm_solver_thread_t * thread;
} thread_arg_t;

static void instance_print_stats(
    fcs_dbm_solver_instance_t * instance,
    FILE * out_fh
    )
{
    fcs_portable_time_t mytime;
    FCS_GET_TIME(mytime);

    fprintf (out_fh, "Reached %ld ; States-in-collection: %ld ; Time: %li.%.6li\n",
             instance->count_num_processed,
             instance->num_states_in_collection,
             FCS_TIME_GET_SEC(mytime),
             FCS_TIME_GET_USEC(mytime)
            );
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
    fprintf (out_fh, ">>>Queue Stats: inserted=%ld items_in_queue=%ld extracted=%ld\n",
             instance->num_states_in_collection,
             instance->num_states_in_collection - instance->count_num_processed,
             instance->count_num_processed
            );
#endif
    fflush(out_fh);
}

static void * instance_run_solver_thread(void * void_arg)
{
    thread_arg_t * arg;
    int curr_depth;
    fcs_dbm_collection_by_depth_t * coll;

    enum TERMINATE_REASON should_terminate;
    fcs_dbm_solver_thread_t * thread;
    fcs_dbm_solver_instance_t * instance;
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
    fcs_dbm_queue_item_t physical_item;
    fcs_dbm_record_t * token;
#endif
    fcs_dbm_queue_item_t * item, * prev_item;
    int queue_num_extracted_and_processed;
    fcs_derived_state_t * derived_list, * derived_list_recycle_bin,
                        * derived_iter;
    fcs_compact_allocator_t derived_list_allocator;
    fc_solve_delta_stater_t * delta_stater;
    fcs_state_keyval_pair_t state;
    FILE * out_fh;
#ifdef DEBUG_OUT
    fcs_state_locs_struct_t locs;
#endif
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    arg = (thread_arg_t *)void_arg;
    thread = arg->thread;
    instance = thread->instance;
    delta_stater = thread->delta_stater;

    prev_item = item = NULL;
    queue_num_extracted_and_processed = 0;

    fc_solve_compact_allocator_init(&(derived_list_allocator), &(thread->thread_meta_alloc));
    derived_list_recycle_bin = NULL;
    derived_list = NULL;
    out_fh = instance->out_fh;

#ifdef T
    TRACE0("instance_run_solver_thread start");
#endif
#ifdef DEBUG_OUT
    fc_solve_init_locs(&locs);
#endif

    curr_depth = instance->curr_depth;
    coll = &(instance->colls_by_depth[curr_depth]);

    while (1)
    {
        /* First of all extract an item. */
        FCS_LOCK(coll->queue_lock);

        if (prev_item)
        {
            FCS_LOCK(instance->global_lock);
            instance->queue_num_extracted_and_processed--;
            FCS_UNLOCK(instance->global_lock);
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
            prev_item->next = coll->queue_recycle_bin;
            coll->queue_recycle_bin = prev_item;
#endif
        }

        if ((should_terminate = instance->should_terminate) == DONT_TERMINATE)
        {
            if (instance->count_of_items_in_queue >= instance->max_count_of_items_in_queue)
            {
                instance->should_terminate = should_terminate = QUEUE_TERMINATE;
                /* TODO :
                 * Implement dumping the queue to the output filehandle.
                 * */
            }
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
            else if (fcs_offloading_queue__extract(&(coll->queue), (fcs_offloading_queue_item_t *)(&token)))
#else
            else if ((item = coll->queue_head))
#endif
            {
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
                physical_item.key = token->key;
                item = &physical_item;
#else
                if (!(instance->queue_head = item->next))
                {
                    instance->queue_tail = NULL;
                }
#endif
                instance->count_of_items_in_queue--;
                instance->queue_num_extracted_and_processed++;
                if (++instance->count_num_processed % 100000 == 0)
                {
                    instance_print_stats(instance, out_fh);
                }
                if (instance->count_num_processed >=
                    instance->max_count_num_processed)
                {
                    instance->should_terminate = should_terminate = MAX_ITERS_TERMINATE;
                }
            }
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
            else
            {
                item = NULL;
            }
#endif

            queue_num_extracted_and_processed =
                instance->queue_num_extracted_and_processed;
        }
        FCS_UNLOCK(coll->queue_lock);

        if ((should_terminate != DONT_TERMINATE)
            || (! queue_num_extracted_and_processed)
        )
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
            &state,
            indirect_stacks_buffer
        );

        /* A section for debugging. */
#ifdef DEBUG_OUT
        {
            char * state_str;
            state_str = fc_solve_state_as_string(
                &(state.s),
                &(state.info),
                &locs,
                2,
                8,
                1,
                1,
                0,
                1
            );

            fprintf(out_fh, "<<<\n%s>>>\n", state_str);
            fflush(out_fh);
            free(state_str);
        }
#endif

        if (instance_solver_thread_calc_derived_states(
            &state,
            &(item->key),
            token,
            &derived_list,
            &derived_list_recycle_bin,
            &derived_list_allocator,
            TRUE
        ))
        {
            FCS_LOCK(instance->global_lock);
            instance->should_terminate = SOLUTION_FOUND_TERMINATE;
            instance->queue_solution_was_found = TRUE;
#ifdef FCS_DBM_WITHOUT_CACHES
            instance->queue_solution_ptr = token;
#else
            instance->queue_solution = item->key;
#endif
            FCS_UNLOCK(instance->global_lock);
            break;
        }

        /* Encode all the states. */
        for (derived_iter = derived_list;
                derived_iter ;
                derived_iter = derived_iter->next
        )
        {
            fcs_init_and_encode_state(
                delta_stater,
                &(derived_iter->state),
                &(derived_iter->key)
            );
        }

        instance_check_multiple_keys(instance, derived_list
#ifdef FCS_DBM_CACHE_ONLY
            , item->moves_to_key
#endif
        );

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

#ifdef T
    TRACE0("instance_run_solver_thread end");
#endif

    return NULL;
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

static void calc_trace(
    fcs_dbm_solver_instance_t * instance,
    fcs_dbm_record_t * ptr_initial_record,
    fcs_encoded_state_buffer_t * * ptr_trace,
    int * ptr_trace_num
    )
{
    int trace_num;
    int trace_max_num;
    fcs_encoded_state_buffer_t * trace;
    fcs_encoded_state_buffer_t * key_ptr;
    fcs_dbm_record_t * record;

#define GROW_BY 100
    trace_num = 0;
    trace = malloc(sizeof(trace[0]) * (trace_max_num = GROW_BY));
    key_ptr = trace;
    record = ptr_initial_record;

    while (record)
    {
#if 1
        *(key_ptr) = record->key;
        if ((++trace_num) == trace_max_num)
        {
            trace = realloc(trace, sizeof(trace[0]) * (trace_max_num += GROW_BY));
            key_ptr = &(trace[trace_num-1]);
        }
        record = fcs_dbm_record_get_parent_ptr(record);
        key_ptr++;
#else
#ifndef FCS_DBM_CACHE_ONLY
#ifndef FCS_DBM_WITHOUT_CACHES
        if (! pre_cache_lookup_parent(
            &(instance->pre_cache),
            key_ptr,
            (key_ptr+1)
            ))
#endif
        {
            if (!fc_solve_dbm_store_lookup_parent(
                instance->store,
                key_ptr->s,
                key_ptr[1].s
                ))
            {
                fprintf(stderr, "Trace problem in trace No. %d. Exiting\n", trace_num);
                exit(-1);
            }
        }
#endif
#endif
    }
#undef GROW_BY
    *ptr_trace_num = trace_num;
    *ptr_trace = trace;

    return;
}

static void instance_run_all_threads(
    fcs_dbm_solver_instance_t * instance,
    fcs_state_keyval_pair_t * init_state,
    int num_threads)
{
    int i, check;
    main_thread_item_t * threads;

#ifdef T
    FILE * out_fh = instance->out_fh;
#endif

    threads = malloc(sizeof(threads[0]) * num_threads);

#ifdef T
    TRACE0("instance_run_all_threads start");
#endif

#ifdef DEBUG_FOO
    global_delta_stater =
            fc_solve_delta_stater_alloc(
                &(init_state->s),
                STACKS_NUM,
                FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
                , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
            );
#endif
    for (i=0; i < num_threads ; i++)
    {
        threads[i].thread.instance = instance;
        threads[i].thread.delta_stater =
            fc_solve_delta_stater_alloc(
                &(init_state->s),
                STACKS_NUM,
                FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
                , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
            );

        fc_solve_meta_compact_allocator_init(
            &(threads[i].thread.thread_meta_alloc)
        );
        threads[i].arg.thread = &(threads[i].thread);
    }

    while (instance->curr_depth < FCC_DEPTH)
    {
#ifdef T
    TRACE1("Running threads for curr_depth=%d\n", instance->curr_depth);
#endif
        for (i=0; i < num_threads ; i++)
        {
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
        }
#ifdef T
        TRACE1("Finished running threads for curr_depth=%d\n", instance->curr_depth);
#endif
        if (instance->queue_solution_was_found)
        {
            break;
        }
        /* Now that we are about to ascend to a new depth, let's mark-and-sweep
         * the old states, some of which are no longer of interest.
         * */
        {
            dict_t * kaz_tree;
            struct avl_traverser trav;
            dict_key_t item;
            struct avl_node * ancestor;
            struct avl_node * * tree_recycle_bin;
            size_t items_count, idx;

#ifdef T
            TRACE1("Start mark-and-sweep cleanup for curr_depth=%d\n", instance->curr_depth);
#endif
            tree_recycle_bin =
            (
                (struct avl_node * *)(&(instance->tree_recycle_bin))
            );

            kaz_tree = fc_solve_dbm_store_get_dict(
                instance->colls_by_depth[instance->curr_depth].store
            );
            avl_t_init(&trav, kaz_tree);

            items_count = kaz_tree->avl_count;
            for (
                idx = 0,
                item = avl_t_first(&trav, kaz_tree)
                ;
                item
                ;
                item = avl_t_next(&trav)
            )
            {
                if (! avl_get_decommissioned_flag(item))
                {
                    ancestor = (struct avl_node *)item;
                    while (fcs_dbm_record_get_refcount(&(ancestor->avl_data)) == 0)
                    {
                        avl_set_decommissioned_flag(ancestor, 1);

                        AVL_SET_NEXT(ancestor, *(tree_recycle_bin));
                        *(tree_recycle_bin) = ancestor;

                        if (!(ancestor = (struct avl_node *)fcs_dbm_record_get_parent_ptr(&(ancestor->avl_data))))
                        {
                            break;
                        }
                        fcs_dbm_record_decrement_refcount(&(ancestor->avl_data));
                    }
                }
                if (((++idx) % 100000) == 0)
                {
                    fprintf(out_fh, "Mark+Sweep Progress - %ld/%ld\n",
                            ((long)idx), ((long)items_count));
                }
            }
#ifdef T
            TRACE1("Finish mark-and-sweep cleanup for curr_depth=%d\n", instance->curr_depth);
#endif
        }
        instance->curr_depth++;
    }

    for (i=0; i < num_threads ; i++)
    {
        fc_solve_delta_stater_free(threads[i].thread.delta_stater);
    }

    free(threads);

#ifdef T
    TRACE0("instance_run_all_threads end");
#endif
    return;
}

#ifdef FCS_DEBONDT_DELTA_STATES

static int compare_enc_states(
    const fcs_encoded_state_buffer_t * a, const fcs_encoded_state_buffer_t * b
)
{
    return memcmp(a,b, sizeof(*a));
}

#else

static int compare_enc_states(
    const fcs_encoded_state_buffer_t * a, const fcs_encoded_state_buffer_t * b
)
{
    if (a->s[0] < b->s[0])
    {
        return -1;
    }
    else if (a->s[0] > b->s[0])
    {
        return 1;
    }
    else
    {
        return memcmp(a->s, b->s, a->s[0]+1);
    }
}

#endif


static unsigned char get_move_from_parent_to_child(
    fcs_dbm_solver_instance_t * instance,
    fc_solve_delta_stater_t * delta,
    fcs_encoded_state_buffer_t parent,
    fcs_encoded_state_buffer_t child)
{
    unsigned char move_to_return;
    fcs_encoded_state_buffer_t got_child;
    fcs_state_keyval_pair_t parent_state;
    fcs_derived_state_t * derived_list, * derived_list_recycle_bin,
                        * derived_iter;
    fcs_compact_allocator_t derived_list_allocator;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    fc_solve_compact_allocator_init(&(derived_list_allocator), &(instance->instance_meta_alloc));
    fc_solve_delta_stater_decode_into_state(
        delta,
        parent.s,
        &parent_state,
        indirect_stacks_buffer
        );

    derived_list = NULL;
    derived_list_recycle_bin = NULL;

    instance_solver_thread_calc_derived_states(
        &parent_state,
        &parent,
        NULL,
        &derived_list,
        &derived_list_recycle_bin,
        &derived_list_allocator,
        TRUE
    );

    for (derived_iter = derived_list;
            derived_iter ;
            derived_iter = derived_iter->next
    )
    {
        fcs_init_and_encode_state(
            delta,
            &(derived_iter->state),
            &got_child
        );

        if (compare_enc_states(&got_child, &child) == 0)
        {
            break;
        }
    }

    if (! derived_iter)
    {
        fprintf(stderr, "%s\n", "Failed to find move. Terminating.");
        exit(-1);
    }
    move_to_return = derived_iter->move;

    fc_solve_compact_allocator_finish(&(derived_list_allocator));

    return move_to_return;
}

static void trace_solution(
    fcs_dbm_solver_instance_t * instance,
    FILE * out_fh,
    fc_solve_delta_stater_t * delta
)
{
    fcs_encoded_state_buffer_t * trace;
    int trace_num;
    int i;
    fcs_state_keyval_pair_t state;
    unsigned char move;
    char * state_as_str;
    char move_buffer[500];
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    fcs_state_locs_struct_t locs;

    fprintf (out_fh, "%s\n", "Success!");
    fflush (out_fh);
    /* Now trace the solution */

    calc_trace(instance, instance->queue_solution_ptr, &trace, &trace_num);

    fc_solve_init_locs(&locs);

    for (i = trace_num-1 ; i >= 0 ; i--)
    {
        fc_solve_delta_stater_decode_into_state(
            delta,
            trace[i].s,
            &state,
            indirect_stacks_buffer
            );
        if (i > 0)
        {
            move = get_move_from_parent_to_child(
                instance,
                delta,
                trace[i],
                trace[i-1]
            );
        }

        state_as_str =
            fc_solve_state_as_string(
                &(state.s),
                &(state.info),
                &locs,
                FREECELLS_NUM,
                STACKS_NUM,
                DECKS_NUM,
                1,
                0,
                1
                );

        fprintf(out_fh, "--------\n%s\n==\n%s\n",
                state_as_str,
                (i > 0 )
                ? move_to_string(move, move_buffer)
                : "END"
               );
        fflush (out_fh);

        free(state_as_str);
    }
    free (trace);
}

/* Returns if the process should terminate. */
static fcs_bool_t handle_and_destroy_instance_solution(
    fcs_dbm_solver_instance_t * instance,
    FILE * out_fh,
    fc_solve_delta_stater_t * delta
)
{
    fcs_bool_t ret = FALSE;

#ifdef T
    TRACE0("handle_and_destroy_instance_solution start");
#endif
    instance_print_stats(instance, out_fh);

    if (instance->queue_solution_was_found)
    {
        trace_solution(instance, out_fh, delta);
        ret = TRUE;
    }
    else if (instance->should_terminate != DONT_TERMINATE)
    {
        fprintf (out_fh, "%s\n", "Intractable.");
        fflush (out_fh);
        if (instance->should_terminate == QUEUE_TERMINATE)
        {
#if 0
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
            fcs_dbm_queue_item_t physical_item;
#endif
            fcs_dbm_queue_item_t * item;

#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
            item = &physical_item;
#endif

#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
            while (fcs_offloading_queue__extract(&(instance->queue), &token))
#else
            for (
                item = instance->queue_head ;
                item ;
                item = item->next
                )
#endif
            {
                int i;
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
                physical_item.key = *(const fcs_encoded_state_buffer_t *)token;
#endif

                for (i=0; i < item->key.s[0] ; i++)
                {
                    fprintf(out_fh, "%.2X", (int)item->key.s[1 + i]);
                }

                fprintf (out_fh, "%s", "|");
                fflush(out_fh);

#ifdef FCS_DBM_CACHE_ONLY
                {
                    fcs_fcc_move_t * move_ptr;
                    if ((move_ptr = item->moves_to_key))
                    {
                        while (*(move_ptr))
                        {
                            fprintf(out_fh, "%.2X,", *(move_ptr));
                            move_ptr++;
                        }
                    }
                }
#else
                {
                    int trace_num;
                    fcs_encoded_state_buffer_t * trace;

                    calc_trace(instance, &(item->key), &trace, &trace_num);

                    /*
                     * We stop at 1 because the deepest state does not contain
                     * a move (as it is the ultimate state).
                     * */
#define PENULTIMATE_DEPTH 1
                    for (i = trace_num-1 ; i >= PENULTIMATE_DEPTH ; i--)
                    {
                        fprintf(out_fh, "%.2X,", get_move_from_parent_to_child(instance, delta, trace[i], trace[i-1]));
                    }
#undef PENULTIMATE_DEPTH
                    free(trace);
                }
#endif
                fprintf (out_fh, "\n");
                fflush(out_fh);
#ifdef DEBUG_OUT
                {
                    char * state_str;
                    fcs_state_keyval_pair_t state;
                    fcs_state_locs_struct_t locs;
                    DECLARE_IND_BUF_T(indirect_stacks_buffer)

                    fc_solve_init_locs(&locs);

                    fc_solve_delta_stater_decode_into_state(
                        delta,
                        item->key.s,
                        &state,
                        indirect_stacks_buffer
                        );

                    state_str = fc_solve_state_as_string(
                        &(state.s),
                        &(state.info),
                        &locs,
                        2,
                        8,
                        1,
                        1,
                        0,
                        1
                    );

                    fprintf(out_fh, "<<<\n%s>>>\n", state_str);
                    fflush(out_fh);
                    free(state_str);
                }
#endif
            }
#endif
        }
        else if (instance->should_terminate == MAX_ITERS_TERMINATE)
        {
            fprintf(out_fh, "Reached Max-or-more iterations of %ld.\n", instance->max_count_num_processed);
        }
    }
    else
    {
        fprintf (out_fh, "%s\n", "Could not solve successfully.");
    }

#ifdef T
    TRACE0("handle_and_destroy_instance_solution end");
#endif

    instance_destroy(instance);

    return ret;
}

int main(int argc, char * argv[])
{
    long pre_cache_max_count;
    long caches_delta;
    long max_count_of_items_in_queue = LONG_MAX;
    long iters_delta_limit = -1;
#if 0
    long start_line = 1;
#endif
    const char * dbm_store_path;
    int num_threads;
    int arg;
    const char * filename = NULL, * out_filename = NULL,
          * intermediate_input_filename = NULL, * offload_dir_path = NULL;
    FILE * fh = NULL, * out_fh = NULL, * intermediate_in_fh = NULL;
    char user_state[USER_STATE_SIZE];
    fc_solve_delta_stater_t * delta;
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
    fcs_dbm_record_t * token;
#endif

    fcs_state_keyval_pair_t init_state;
#if 0
    fcs_bool_t skip_queue_output = FALSE;
#endif
    DECLARE_IND_BUF_T(init_indirect_stacks_buffer)

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
        else if (!strcmp(argv[arg], "--max-count-of-items-in-queue"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--max-count-of-items-in-queue came without an argument.\n");
                exit(-1);
            }
            max_count_of_items_in_queue = atol(argv[arg]);
        }
#if 0
        else if (!strcmp(argv[arg], "--start-line"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--start-line came without an argument.\n");
                exit(-1);
            }
            start_line = atol(argv[arg]);
        }
#endif
        else if (!strcmp(argv[arg], "--iters-delta-limit"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--iters-delta-limit came without an argument.\n");
                exit(-1);
            }
            iters_delta_limit = atol(argv[arg]);
        }
        else if (!strcmp(argv[arg], "-o"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "-o came without an argument.\n");
                exit(-1);
            }
            out_filename = argv[arg];
        }
        else if (!strcmp(argv[arg], "--intermediate-input"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--intermediate-input came without an argument.\n");
                exit(-1);
            }
            intermediate_input_filename = argv[arg];
        }
        else if (!strcmp(argv[arg], "--offload-dir-path"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--offload-dir-path came without an argument.\n");
                exit(-1);
            }
            offload_dir_path = argv[arg];
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

    if (out_filename)
    {
        out_fh = fopen(out_filename, "at");
        if (! out_fh)
        {
            fprintf (stderr, "Cannot open '%s' for output.\n",
                     "out_filename");
            exit(-1);
        }
    }
    else
    {
        out_fh = stdout;
    }

    filename = argv[arg];

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

    horne_prune(&init_state, NULL, NULL);

    delta = fc_solve_delta_stater_alloc(
            &init_state.s,
            STACKS_NUM,
            FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
            , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
    );

    if (intermediate_input_filename)
    {
        intermediate_in_fh = fopen(intermediate_input_filename, "rt");
        if (! intermediate_in_fh)
        {
            fprintf (stderr,
                     "Could not open file '%s' as --intermediate-input-filename.\n",
                     intermediate_input_filename);
            exit(-1);
        }
    }

#ifdef FCS_DBM_SINGLE_THREAD
#define NUM_THREADS() 1
#else
#define NUM_THREADS() num_threads
#endif
#if 0
    if (intermediate_in_fh)
    {
        fcs_bool_t found_line;
        char * line = NULL;
        size_t line_size = 0;
        long line_num = 0;
        fcs_bool_t queue_solution_was_found = FALSE;
        fcs_dbm_solver_instance_t queue_instance;
        fcs_dbm_solver_instance_t limit_instance;

        instance_init(&queue_instance, pre_cache_max_count, caches_delta,
                      dbm_store_path, max_count_of_items_in_queue,
                      -1, offload_dir_path, out_fh);

        instance_init(
            &limit_instance, pre_cache_max_count, caches_delta,
            dbm_store_path, LONG_MAX,
            iters_delta_limit, offload_dir_path, out_fh
            );

        do
        {
            line_num++;
            found_line = FALSE;
            while (getline(&line, &line_size, intermediate_in_fh) >= 0)
            {
                if (strchr(line, '|') != NULL)
                {
                    found_line = TRUE;
                    break;
                }
                line_num++;
            }

            if (found_line)
            {
                if (line_num < start_line)
                {
                    continue;
                }
                skip_queue_output = FALSE;
                {
                    populate_instance_with_intermediate_input_line(
                        &limit_instance,
                        delta,
                        &init_state,
                        line,
                        line_num
                        );

                    instance_run_all_threads(
                        &limit_instance, &init_state, NUM_THREADS()
                        );

                    if (limit_instance.queue_solution_was_found)
                    {
                        trace_solution(&limit_instance, out_fh, delta);
                        skip_queue_output = TRUE;
                        queue_solution_was_found = TRUE;
                    }
                    else if (limit_instance.should_terminate == MAX_ITERS_TERMINATE)
                    {
                        fprintf(
                            out_fh,
                            "Reached Max-or-more iterations of %ld in intermediate-input line No. %ld.\n",
                            limit_instance.max_count_num_processed,
                            line_num
                            );
                    }
                    else if (limit_instance.should_terminate == DONT_TERMINATE)
                    {
                        fprintf(
                            out_fh,
                            "Pruning due to unsolvability in intermediate-input line No. %ld\n",
                            line_num
                            );
                        skip_queue_output = TRUE;
                    }
                }

                if (!skip_queue_output)
                {
                    populate_instance_with_intermediate_input_line(
                        &queue_instance,
                        delta,
                        &init_state,
                        line,
                        line_num
                        );

                    instance_run_all_threads(
                        &queue_instance, &init_state, NUM_THREADS()
                        );

                    if (handle_and_destroy_instance_solution(
                        &queue_instance,
                        out_fh,
                        delta
                    ))
                    {
                        queue_solution_was_found = TRUE;
                    }
                }
            }

            if (!queue_solution_was_found)
            {
                /*
                 * This recycles the instances by keeping the cache,
                 * but making sure that the statistics and the queue are reset.
                 * */
                instance_recycle(&limit_instance);
                instance_recycle(&queue_instance);
            }
        } while (found_line && (!queue_solution_was_found));

        instance_destroy(&limit_instance);
        if (! queue_solution_was_found)
        {
            instance_destroy(&queue_instance);
        }

        free(line);
        line = NULL;
    }
    else
#endif
    {
        fcs_dbm_solver_instance_t instance;
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
        fcs_encoded_state_buffer_t * key_ptr;
#define KEY_PTR() (key_ptr)
#else
        fcs_dbm_queue_item_t * first_item;
#define KEY_PTR() &(first_item->key)
#endif

        fcs_encoded_state_buffer_t parent;

        instance_init(&instance, pre_cache_max_count, caches_delta,
                      dbm_store_path, max_count_of_items_in_queue,
                      iters_delta_limit, offload_dir_path, out_fh);

#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
    key_ptr = &(instance.first_key);
#else
        first_item =
            (fcs_dbm_queue_item_t *)
            fcs_compact_alloc_ptr(
                &(instance.queue_allocator),
                sizeof(*first_item)
                );

        first_item->next = NULL;
        first_item->moves_to_key = NULL;
#endif
        fcs_init_and_encode_state(delta, &(init_state), KEY_PTR());
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
#endif

        /* The NULL parent and move for indicating this is the initial
         * state. */
        fcs_init_encoded_state(&(parent));

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
        pre_cache_insert(&(instance.pre_cache), KEY_PTR(), &parent);
#else
        cache_insert(&(instance.cache), KEY_PTR(), NULL, '\0');
#endif
#else
        token = fc_solve_dbm_store_insert_key_value(instance.colls_by_depth[0].store, KEY_PTR(), NULL);
#endif

#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
        fcs_offloading_queue__insert(&(instance.colls_by_depth[0].queue),
            (const fcs_offloading_queue_item_t *)(&token));
#else
        instance.queue_head = instance.queue_tail = first_item;
#endif
        instance.num_states_in_collection++;
        instance.count_of_items_in_queue++;

        instance_run_all_threads(&instance, &init_state, NUM_THREADS());
        handle_and_destroy_instance_solution(&instance, out_fh, delta);
    }

    fc_solve_delta_stater_free(delta);
    delta = NULL;

    if (out_filename)
    {
        fclose(out_fh);
        out_fh = NULL;
    }

    if (intermediate_in_fh)
    {
        fclose(intermediate_in_fh);
        intermediate_in_fh = NULL;
    }

    return 0;
}
