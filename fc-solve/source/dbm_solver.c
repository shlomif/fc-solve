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
 * dbm_solver.c - a specialised solver that offloads the states' collection
 * to an on-disk DBM database such as Berkeley DB or Google's LevelDB. Has
 * been adapted to be completely in-memory. It makes use of delta_states.c
 * for a very compact storage.
 *
 */
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <limits.h>

/*
 * Define FCS_DBM_SINGLE_THREAD to have single thread-per-instance traversal.
 */
#define FCS_DBM_SINGLE_THREAD 1

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

#include "delta_states.c"

#include "dbm_common.h"
#include "dbm_solver.h"
#include "dbm_cache.h"

#ifndef FCS_DBM_WITHOUT_CACHES

static int fc_solve_compare_pre_cache_keys(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((((const fcs_pre_cache_key_val_pair_t *)(p))->key))
    return memcmp(&(GET_PARAM(void_a)), &(GET_PARAM(void_b)), sizeof(GET_PARAM(void_a)));
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
    fcs_encoded_state_buffer_t * key
    )
{
    fcs_pre_cache_key_val_pair_t to_check;

    to_check.key = *key;

    return (fc_solve_kaz_tree_lookup(pre_cache->kaz_tree, &to_check) != NULL);
}

static fcs_bool_t GCC_INLINE pre_cache_lookup_parent_and_move(
    fcs_pre_cache_t * pre_cache,
    fcs_encoded_state_buffer_t * key,
    fcs_encoded_state_buffer_t * parent_and_move
    )
{
    fcs_pre_cache_key_val_pair_t to_check;
    dnode_t * node;

    to_check.key = *key;

    node = fc_solve_kaz_tree_lookup(pre_cache->kaz_tree, &to_check);

    if (node)
    {
        *parent_and_move = 
            ((fcs_pre_cache_key_val_pair_t *)(node->dict_key))->parent_and_move;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void GCC_INLINE pre_cache_insert(
    fcs_pre_cache_t * pre_cache,
    fcs_encoded_state_buffer_t * key,
    fcs_encoded_state_buffer_t * parent_and_move
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
    to_insert->parent_and_move = *parent_and_move;

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
            &(((fcs_pre_cache_key_val_pair_t *)(node->dict_key))->key)
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
    long count_num_processed, count_of_items_in_queue, max_count_num_processed;
    long max_count_of_items_in_queue;
    fcs_bool_t queue_solution_was_found;
    enum TERMINATE_REASON should_terminate;
    fcs_encoded_state_buffer_t queue_solution;
    fcs_meta_compact_allocator_t meta_alloc;
    fcs_compact_allocator_t queue_allocator;
    int queue_num_extracted_and_processed;
    /* TODO : offload the queue to the hard disk. */
    fcs_dbm_queue_item_t * queue_head, * queue_tail, * queue_recycle_bin;
    long num_states_in_collection;
    FILE * out_fh;
} fcs_dbm_solver_instance_t;

static GCC_INLINE void instance_init(
    fcs_dbm_solver_instance_t * instance,
    long pre_cache_max_count,
    long caches_delta,
    const char * dbm_store_path,
    long max_count_of_items_in_queue,
    long iters_delta_limit,
    FILE * out_fh
)
{
    FCS_INIT_LOCK(instance->queue_lock);
    FCS_INIT_LOCK(instance->storage_lock);

    instance->out_fh = out_fh;

    fc_solve_meta_compact_allocator_init(
        &(instance->meta_alloc)
    );
    fc_solve_compact_allocator_init(
        &(instance->queue_allocator), &(instance->meta_alloc)
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

static GCC_INLINE void instance_destroy(
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

    fc_solve_meta_compact_allocator_finish(
        &(instance->meta_alloc)
    );

    FCS_DESTROY_LOCK(instance->queue_lock);
    FCS_DESTROY_LOCK(instance->storage_lock);
}

static GCC_INLINE void instance_check_key(
    fcs_dbm_solver_instance_t * instance,
    fcs_encoded_state_buffer_t * key,
    fcs_encoded_state_buffer_t * parent_and_move
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
    else if (fc_solve_dbm_store_does_key_exist(instance->store, key->s))
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

        new_item->key = (*key);
        new_item->next = NULL;

        if (instance->queue_tail)
        {
            instance->queue_tail = instance->queue_tail->next = new_item;
        }
        else
        {
            instance->queue_head = instance->queue_tail = new_item;
        }
        instance->count_of_items_in_queue++;
        FCS_UNLOCK(instance->queue_lock);
    }
}

static GCC_INLINE void instance_check_multiple_keys(
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
        instance_check_key(instance, &(list->key), &(list->parent_and_move));
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

static void * instance_run_solver_thread(void * void_arg)
{
    thread_arg_t * arg;
    enum TERMINATE_REASON should_terminate;
    fcs_dbm_solver_thread_t * thread;
    fcs_dbm_solver_instance_t * instance;
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

    fc_solve_compact_allocator_init(&(derived_list_allocator), &(instance->meta_alloc));
    derived_list_recycle_bin = NULL;
    derived_list = NULL;
    out_fh = instance->out_fh;

#ifdef DEBUG_OUT
    fc_solve_init_locs(&locs);
#endif
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

        if ((should_terminate = instance->should_terminate) == DONT_TERMINATE)
        {
            if (instance->count_of_items_in_queue >= instance->max_count_of_items_in_queue)
            {
                instance->should_terminate = should_terminate = QUEUE_TERMINATE;
                /* TODO :
                 * Implement dumping the queue to the output filehandle.
                 * */
            }
            else if ((item = instance->queue_head))
            {
                if (!(instance->queue_head = item->next))
                {
                    instance->queue_tail = NULL;
                }
                instance->count_of_items_in_queue--;
                instance->queue_num_extracted_and_processed++;
                if (++instance->count_num_processed % 100000 == 0)
                {
                    fcs_portable_time_t mytime;
                    FCS_GET_TIME(mytime);

                    fprintf (out_fh, "Reached %ld ; States-in-collection: %ld ; Time: %li.%.6li\n",
                        instance->count_num_processed,
                        instance->num_states_in_collection,
                        FCS_TIME_GET_SEC(mytime),
                        FCS_TIME_GET_USEC(mytime)
                    );
                    fflush(out_fh);
                }
                if (instance->count_num_processed >=
                    instance->max_count_num_processed)
                {
                    instance->should_terminate = should_terminate = MAX_ITERS_TERMINATE;
                }
            }

            queue_num_extracted_and_processed =
                instance->queue_num_extracted_and_processed;
        }
        FCS_UNLOCK(instance->queue_lock);

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
            &derived_list,
            &derived_list_recycle_bin,
            &derived_list_allocator,
            TRUE
        ))
        {
            FCS_LOCK(instance->queue_lock);
            instance->should_terminate = SOLUTION_FOUND_TERMINATE;
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
            fcs_init_and_encode_state(
                delta_stater,
                &(derived_iter->state),
                &(derived_iter->key)
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
    fcs_encoded_state_buffer_t * ptr_initial_key,
    fcs_encoded_state_buffer_t * * ptr_trace,
    int * ptr_trace_num
    )
{
    int trace_num;
    int trace_max_num;
    fcs_encoded_state_buffer_t * trace;
    fcs_encoded_state_buffer_t key;

#define GROW_BY 100
    trace_num = 0;
    trace = malloc(sizeof(trace[0]) * (trace_max_num = GROW_BY));
    trace[trace_num] = *ptr_initial_key;

    while (trace[trace_num].s[0])
    {
        /* Omit the move. */
        key = trace[trace_num];
        key.s[key.s[0]+1] = '\0';

        if ((++trace_num) == trace_max_num)
        {
            trace = realloc(trace, sizeof(trace[0]) * (trace_max_num += GROW_BY));
        }
#ifndef FCS_DBM_WITHOUT_CACHES
        if (! pre_cache_lookup_parent_and_move(
            &(instance->pre_cache),
            &key,
            &(trace[trace_num])
            ))
        {
#endif
            assert(fc_solve_dbm_store_lookup_parent_and_move(
                instance->store,
                key.s,
                trace[trace_num].s
                ));
#ifndef FCS_DBM_WITHOUT_CACHES
        }
#endif
    }
#undef GROW_BY
    *ptr_trace_num = trace_num;
    *ptr_trace = trace;

    return;
}

static void populate_instance_with_intermediate_input_line(
    fcs_dbm_solver_instance_t * instance,
    fc_solve_delta_stater_t * delta,
    fcs_state_keyval_pair_t * init_state_ptr,
    char * line,
    long line_num
    )
{                
    char * s_ptr;
    fcs_encoded_state_buffer_t final_stack_encoded_state;
    int hex_digits;
    fcs_kv_state_t kv_init, kv_running;
    fcs_encoded_state_buffer_t running_parent_and_move, running_key;
    fcs_state_keyval_pair_t running_state;
    fcs_dbm_queue_item_t * first_item;
    DECLARE_IND_BUF_T(running_indirect_stacks_buffer)

    fcs_init_encoded_state(&(final_stack_encoded_state));

    s_ptr = line;

    while (*(s_ptr) != ';')
    {
        if (sscanf(s_ptr, "%2X", &hex_digits) != 1)
        {
            fprintf (
                stderr,
                "Error in reading state in line %ld of the --intermediate-input",
                line_num
                );
            exit(-1);
        }
        final_stack_encoded_state.s[
            ++final_stack_encoded_state.s[0]
            ] = (unsigned char)hex_digits;
        s_ptr += 2;
    }

    /* Skip the ';'. */
    s_ptr++;

    kv_init.key = &(init_state_ptr->s);
    kv_init.val = &(init_state_ptr->info);
    kv_running.key = &(running_state.s);
    kv_running.val = &(running_state.info);
    fcs_duplicate_kv_state(&kv_running, &kv_init);

    /* The NULL parent and move for indicating this is the initial
     * state. */
    fcs_init_and_encode_state(delta, &(running_state), &running_key);
    fcs_init_encoded_state(&(running_parent_and_move));

#ifndef FCS_DBM_WITHOUT_CACHES
    pre_cache_insert(&(instance->pre_cache), &(running_key), &running_parent_and_move);
#else
    fc_solve_dbm_store_insert_key_value(instance->store, &(running_key), &running_parent_and_move);
#endif
    instance->num_states_in_collection++;

    running_parent_and_move = running_key;

    while (sscanf(s_ptr, "%2X,", &hex_digits) == 1)
    {
        int src, dest;
        fcs_card_t src_card;

        s_ptr += 3;
        /* Apply the move. */
        src = (hex_digits & 0xF);
        dest = ((hex_digits >> 4) & 0xF);

#define the_state (running_state.s)
        /* Extract the card from the source. */
        if (src < 8)
        {
            fcs_cards_column_t src_col;
            src_col = fcs_state_get_col(the_state, src);
            src_card = fcs_col_get_card(src_col, fcs_col_len(src_col)-1);
            fcs_col_pop_top(src_col);
        }
        else
        {
            src -= 8;
            if (src < 4)
            {
                src_card = fcs_freecell_card(the_state, src);
                fcs_empty_freecell(the_state, src);
            }
            else
            {
                fprintf(stderr,
                        "Error in reading state in line %ld of the --intermediate-input - source cannot be a foundation.",
                        line_num
                       );
                exit(-1);
            }
        }
        /* Apply src_card to dest. */
        if (dest < 8)
        {
            fcs_cards_column_t dest_col;
            dest_col = fcs_state_get_col(the_state, dest);

            fcs_col_push_card(dest_col, src_card);
        }
        else
        {
            dest -= 8;
            if (dest < 4)
            {
                fcs_put_card_in_freecell(the_state, dest, src_card);
            }
            else
            {
                dest -= 4;
                fcs_increment_foundation(the_state, dest);
            }
        }
#undef the_state
        horne_prune(&running_state, NULL, NULL);

        fcs_init_and_encode_state(delta, &(running_state),
                                  &(running_key));

        running_parent_and_move.s[
            running_parent_and_move.s[0]+1
            ] = (unsigned char)hex_digits;

#ifndef FCS_DBM_WITHOUT_CACHES
        pre_cache_insert(&(instance->pre_cache), &(running_key), &running_parent_and_move);
#else
        fc_solve_dbm_store_insert_key_value(instance->store, &(running_key), &running_parent_and_move);
#endif
        instance->num_states_in_collection++;
        running_parent_and_move = running_key;

        /* We need to do the round-trip from encoding back
         * to decoding, because the order can change after
         * the encoding.
         * */
        fc_solve_delta_stater_decode_into_state(
            delta,
            running_key.s,
            &running_state,
            running_indirect_stacks_buffer
            );
    }

    if (memcmp(&running_key, &final_stack_encoded_state,
               sizeof(running_key)) != 0)
    {
        fprintf(stderr,
                "Error in reading state in line %ld of the --intermediate-input - final state does not match that with all states applied.\n",
                line_num
               );
        exit(-1);
    }
    first_item =
        (fcs_dbm_queue_item_t *)
        fcs_compact_alloc_ptr(
            &(instance->queue_allocator),
            sizeof(*first_item)
            );

    first_item->next = NULL;
    first_item->key = running_key;

    instance->queue_head = instance->queue_tail = first_item;
    instance->count_of_items_in_queue++;

    return;
}

static void instance_run_all_threads(
    fcs_dbm_solver_instance_t * instance,
    fcs_state_keyval_pair_t * init_state,
    int num_threads)
{
    int i, check;
    main_thread_item_t * threads;

    threads = malloc(sizeof(threads[0]) * num_threads);

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

    return;
}


int main(int argc, char * argv[])
{
    fcs_dbm_solver_instance_t instance;
    long pre_cache_max_count;
    long caches_delta;
    long max_count_of_items_in_queue = LONG_MAX;
    long iters_delta_limit = -1;
    const char * dbm_store_path;
    int num_threads;
    int arg;
    const char * filename = NULL, * out_filename = NULL, 
          * intermediate_input_filename = NULL;
    FILE * fh = NULL, * out_fh = NULL, * intermediate_in_fh = NULL;
    char user_state[USER_STATE_SIZE];
    fc_solve_delta_stater_t * delta;
    fcs_state_keyval_pair_t init_state;
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
        out_fh = fopen(out_filename, "wt");
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

    instance_init(&instance, pre_cache_max_count, caches_delta, 
                  dbm_store_path, max_count_of_items_in_queue,
                  iters_delta_limit, out_fh);
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

    {
        if (intermediate_in_fh)
        {
            char * line = NULL;
            size_t line_size = 0;
            fcs_bool_t found_line = FALSE;
            long line_num = 1;

            while (getline(&line, &line_size, intermediate_in_fh) >= 0)
            {
                if (strchr(line, ';') != NULL)
                {
                    found_line = TRUE;
                    break;
                }
                line_num++;
            }

            if (found_line)
            {
                populate_instance_with_intermediate_input_line(
                    &instance,
                    delta,
                    &init_state,
                    line,
                    line_num
                );
            }

            free(line);
            line = NULL;
        }
        else
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

            fcs_init_and_encode_state(delta, &(init_state), &(first_item->key));

            /* The NULL parent and move for indicating this is the initial
             * state. */
            fcs_init_encoded_state(&(parent_and_move));

#ifndef FCS_DBM_WITHOUT_CACHES
            pre_cache_insert(&(instance.pre_cache), &(first_item->key), &parent_and_move);
#else
            fc_solve_dbm_store_insert_key_value(instance.store, &(first_item->key), &(parent_and_move));
#endif
            instance.num_states_in_collection++;
            instance.queue_head = instance.queue_tail = first_item;
            instance.count_of_items_in_queue++;
        }
    }

    instance_run_all_threads(&instance, &init_state, num_threads);

    if (instance.queue_solution_was_found)
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

        calc_trace(&instance, &(instance.queue_solution), &trace, &trace_num);

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
                move = trace[i].s[1+trace[i].s[0]];
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
    else if (instance.should_terminate != DONT_TERMINATE)
    {
        fprintf (out_fh, "%s\n", "Intractable.");
        fflush (out_fh);
        if (instance.should_terminate == QUEUE_TERMINATE)
        {
            fcs_dbm_queue_item_t * item;

            for (
                item = instance.queue_head ;
                item ;
                item = item->next
                )
            {
                int i;
                int trace_num;
                fcs_encoded_state_buffer_t * trace;

                for (i=0; i < item->key.s[0] ; i++)
                {
                    fprintf(out_fh, "%.2X", (int)item->key.s[1 + i]);
                }

                fprintf (out_fh, "%s", ";");
                fflush(out_fh);

                calc_trace(&instance, &(item->key), &trace, &trace_num);

                /*
                 * We stop at 1 because the deepest state does not contain
                 * a move (as it is the ultimate state).
                 * */
#define PENULTIMATE_DEPTH 1
                for (i = trace_num-1 ; i >= PENULTIMATE_DEPTH ; i--)
                {
                    fprintf(out_fh, "%.2X,", trace[i].s[1+trace[i].s[0]]);
                }
                free(trace);
                fprintf (out_fh, "\n");
                fflush(out_fh);

#undef PENULTIMATE_DEPTH
            }
        }
        else if (instance.should_terminate == MAX_ITERS_TERMINATE)
        {
            fprintf(out_fh, "Reached Max-or-more iterations of %ld.\n", instance.max_count_num_processed);
        }
    }
    else
    {
        fprintf (out_fh, "%s\n", "Could not solve successfully.");
    }
    
    instance_destroy(&instance);

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
