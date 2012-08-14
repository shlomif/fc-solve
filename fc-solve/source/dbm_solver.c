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

#if 0
#define DEBUG_FOO
#endif

/*
 * Define FCS_DBM_SINGLE_THREAD to have single thread-per-instance traversal.
 */
#if 0
#define FCS_DBM_SINGLE_THREAD 1
#endif

#include "config.h"

#undef FCS_RCS_STATES

#include "bool.h"
#include "inline.h"
#include "portable_time.h"
#include "lock.h"

#include "dbm_calc_derived.h"
#include "delta_states_any.h"

#include "dbm_common.h"
#include "dbm_solver.h"
#include "dbm_cache.h"

#define FCS_DBM_USE_OFFLOADING_QUEUE

#include "offloading_queue.h"



#ifdef FCS_DBM_USE_OFFLOADING_QUEUE

#if (!defined(FCS_DBM_WITHOUT_CACHES))
#error FCS_DBM_USE_OFFLOADING_QUEUE requires FCS_DBM_WITHOUT_CACHES
#endif

#if (defined(FCS_DBM_CACHE_ONLY))
#error FCS_DBM_USE_OFFLOADING_QUEUE is not compatible with FCS_DBM_CACHE_ONLY
#endif

#endif

#include "dbm_trace.h"

#ifdef DEBUG_FOO
fc_solve_delta_stater_t * global_delta_stater;
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
#ifndef FCS_DBM_CACHE_ONLY
    fcs_pre_cache_t pre_cache;
#endif
    fcs_lru_cache_t cache;
#endif
#ifndef FCS_DBM_CACHE_ONLY
    fcs_dbm_store_t store;
#endif

    long pre_cache_max_count;
    /* The queue */

    fcs_lock_t queue_lock;
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
    fcs_meta_compact_allocator_t meta_alloc;
    int queue_num_extracted_and_processed;
    fcs_offloading_queue_t queue;
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
    const char * offload_dir_path;
#endif
    fcs_encoded_state_buffer_t first_key;
    long num_states_in_collection;
    FILE * out_fh;
    fcs_encoded_state_buffer_t parent_state_enc;
    void * tree_recycle_bin;
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
    FCS_INIT_LOCK(instance->queue_lock);
    FCS_INIT_LOCK(instance->storage_lock);

    instance->out_fh = out_fh;

    fc_solve_meta_compact_allocator_init(
        &(instance->meta_alloc)
    );
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
#define NUM_ITEMS_PER_PAGE (128 * 1024)
    fcs_offloading_queue__init(&(instance->queue), NUM_ITEMS_PER_PAGE, instance->offload_dir_path = offload_dir_path, 0);
#else
    fcs_offloading_queue__init(&(instance->queue), &(instance->meta_alloc));
#endif
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

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
    pre_cache_init (&(instance->pre_cache), &(instance->meta_alloc));
#endif
    instance->pre_cache_max_count = pre_cache_max_count;
    cache_init (&(instance->cache), pre_cache_max_count+caches_delta, &(instance->meta_alloc));
#endif
#ifndef FCS_DBM_CACHE_ONLY
    instance->tree_recycle_bin = NULL;
    fc_solve_dbm_store_init(&(instance->store), dbm_store_path, &(instance->tree_recycle_bin));
#endif
}

static GCC_INLINE void instance_recycle(
    fcs_dbm_solver_instance_t * instance
    )
{

    fcs_offloading_queue__destroy(&(instance->queue));
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
    fcs_offloading_queue__init(&(instance->queue), NUM_ITEMS_PER_PAGE, instance->offload_dir_path, 0);
#else
    fcs_offloading_queue__init(&(instance->queue), &(instance->meta_alloc));
#endif

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
    fcs_offloading_queue__destroy(&(instance->queue));

#ifndef FCS_DBM_WITHOUT_CACHES

#ifndef FCS_DBM_CACHE_ONLY
    pre_cache_offload_and_destroy(
        &(instance->pre_cache),
        instance->store,
        &(instance->cache)
    );
#endif

    cache_destroy(&(instance->cache));
#endif

#ifndef FCS_DBM_CACHE_ONLY
    fc_solve_dbm_store_destroy(instance->store);
#endif

    fc_solve_meta_compact_allocator_finish(
        &(instance->meta_alloc)
    );

    FCS_DESTROY_LOCK(instance->queue_lock);
    FCS_DESTROY_LOCK(instance->storage_lock);
}

static GCC_INLINE void instance_check_key(
    fcs_dbm_solver_instance_t * instance,
    fcs_encoded_state_buffer_t * key,
    fcs_dbm_record_t * parent,
    unsigned char move
#ifdef FCS_DBM_CACHE_ONLY
    , const fcs_fcc_move_t * moves_to_parent
#endif
)
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
    if ((token = fc_solve_dbm_store_insert_key_value(instance->store, key, parent)))
#endif
    {
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

        FCS_LOCK(instance->queue_lock);
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

            fprintf(instance->out_fh, "Found State:\n<<<\n%s>>>\n", state_str);
            fflush(instance->out_fh);
            free(state_str);
        }
#endif

        instance->num_states_in_collection++;

        fcs_offloading_queue__insert(
            &(instance->queue),
            ((fcs_offloading_queue_item_t *)(&token))
            );
        instance->count_of_items_in_queue++;
        FCS_UNLOCK(instance->queue_lock);
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
        instance_check_key(instance, &(list->key), list->parent, list->move
#ifdef FCS_DBM_CACHE_ONLY
            , moves_to_parent
#endif
        );
    }
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
    FCS_UNLOCK(instance->storage_lock);
}

typedef struct {
    fcs_dbm_solver_instance_t * instance;
    fc_solve_delta_stater_t * delta_stater;
} fcs_dbm_solver_thread_t;

typedef struct {
    fcs_dbm_solver_thread_t * thread;
} thread_arg_t;

#include "dbm_procs.h"

static void * instance_run_solver_thread(void * void_arg)
{
    thread_arg_t * arg;
    enum TERMINATE_REASON should_terminate;
    fcs_dbm_solver_thread_t * thread;
    fcs_dbm_solver_instance_t * instance;
    fcs_dbm_queue_item_t physical_item;
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
    fcs_dbm_record_t * token;
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

    TRACE0("instance_run_solver_thread start");
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
            else if (fcs_offloading_queue__extract(&(instance->queue), (fcs_offloading_queue_item_t *)(&token)))
            {

                physical_item.key = token->key;
                item = &physical_item;

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
            else
            {
                item = NULL;
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
            token,
            &derived_list,
            &derived_list_recycle_bin,
            &derived_list_allocator,
            TRUE
        ))
        {
            FCS_LOCK(instance->queue_lock);
            instance->should_terminate = SOLUTION_FOUND_TERMINATE;
            instance->queue_solution_was_found = TRUE;
#ifdef FCS_DBM_WITHOUT_CACHES
            instance->queue_solution_ptr = token;
#else
            instance->queue_solution = item->key;
#endif
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

    TRACE0("instance_run_solver_thread end");

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

static void populate_instance_with_intermediate_input_line(
    fcs_dbm_solver_instance_t * instance,
    fc_solve_delta_stater_t * delta,
    fcs_state_keyval_pair_t * init_state_ptr,
    char * line,
    long line_num,
    fcs_encoded_state_buffer_t * parent_state_enc
    )
{
    char * s_ptr;
    fcs_encoded_state_buffer_t final_stack_encoded_state;
    int hex_digits;
    fcs_kv_state_t kv_init, kv_running;
    fcs_encoded_state_buffer_t running_key;
    fcs_dbm_record_t * running_parent;
    fcs_state_keyval_pair_t running_state;
    fcs_dbm_record_t * token = NULL;
#ifdef FCS_DBM_CACHE_ONLY
    fcs_fcc_move_t * running_moves;
#endif
    fcs_fcc_move_t move;
    DECLARE_IND_BUF_T(running_indirect_stacks_buffer)

    fc_solve_state_init(
        &running_state, STACKS_NUM, running_indirect_stacks_buffer
    );
    fcs_init_encoded_state(&(final_stack_encoded_state));
    *parent_state_enc = final_stack_encoded_state;

    s_ptr = line;

    while (*(s_ptr) != '|')
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

    /* Skip the '|'. */
    s_ptr++;

    kv_init.key = &(init_state_ptr->s);
    kv_init.val = &(init_state_ptr->info);
    kv_running.key = &(running_state.s);
    kv_running.val = &(running_state.info);
    fcs_duplicate_kv_state(&kv_running, &kv_init);

    /* The NULL parent and move for indicating this is the initial
     * state. */
    fcs_init_and_encode_state(delta, &(running_state), &running_key);
    running_parent = NULL;

#ifdef FCS_DBM_CACHE_ONLY
    running_moves = NULL;
#endif
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
    pre_cache_insert(&(instance->pre_cache), &(running_key), &running_parent);
#else
    cache_insert(&(instance->cache), &(running_key), running_moves, '\0');
#endif
#else
    running_parent = fc_solve_dbm_store_insert_key_value(instance->store, &(running_key), running_parent);
#endif
    instance->num_states_in_collection++;

    while (sscanf(s_ptr, "%2X,", &hex_digits) == 1)
    {
        int src, dest;
        fcs_card_t src_card;

        s_ptr += 3;

        move = (fcs_fcc_move_t)hex_digits;
        /* Apply the move. */
        src = (move & 0xF);
        dest = ((move >> 4) & 0xF);

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

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
        pre_cache_insert(&(instance->pre_cache), &(running_key), &running_parent);
#else
        running_moves = (cache_insert(&(instance->cache), &(running_key), running_moves, move))->moves_to_key;
#endif
#else
        token = fc_solve_dbm_store_insert_key_value(instance->store, &(running_key), running_parent);
#endif
        instance->num_states_in_collection++;

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
    fcs_offloading_queue__insert(&(instance->queue), (const fcs_offloading_queue_item_t *)(&token));
    instance->count_of_items_in_queue++;

    return;
}

static void instance_run_all_threads(
    fcs_dbm_solver_instance_t * instance,
    fcs_state_keyval_pair_t * init_state,
    fcs_encoded_state_buffer_t * parent_state_enc,
    int num_threads)
{
    int i, check;
    main_thread_item_t * threads;

#ifdef T
    FILE * out_fh = instance->out_fh;
#endif

    instance->parent_state_enc = *parent_state_enc;
    threads = malloc(sizeof(threads[0]) * num_threads);

    TRACE0("instance_run_all_threads start");

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

    TRACE0("instance_run_all_threads end");
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

    fc_solve_compact_allocator_init(&(derived_list_allocator), &(instance->meta_alloc));
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
    fcs_dbm_record_t * token;

    TRACE0("handle_and_destroy_instance_solution start");
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
            fcs_dbm_queue_item_t physical_item;
            fcs_dbm_queue_item_t * item;

            item = &physical_item;

            while (fcs_offloading_queue__extract(&(instance->queue), (fcs_offloading_queue_item_t *)(&token)))
            {
                int i;
                physical_item.key = token->key;

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

                    calc_trace(instance, token, &trace, &trace_num);

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

    TRACE0("handle_and_destroy_instance_solution end");

    instance_destroy(instance);

    return ret;
}

int main(int argc, char * argv[])
{
    long pre_cache_max_count;
    long caches_delta;
    long max_count_of_items_in_queue = LONG_MAX;
    long iters_delta_limit = -1;
    long start_line = 1;
    const char * dbm_store_path;
    int num_threads;
    int arg;
    const char * filename = NULL, * out_filename = NULL,
          * intermediate_input_filename = NULL, * offload_dir_path = NULL;
    FILE * fh = NULL, * out_fh = NULL, * intermediate_in_fh = NULL;
    char user_state[USER_STATE_SIZE];
    fc_solve_delta_stater_t * delta;
    fcs_dbm_record_t * token;

    fcs_state_keyval_pair_t init_state;
    fcs_bool_t skip_queue_output = FALSE;
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
                    fcs_encoded_state_buffer_t parent_state_enc;
                    populate_instance_with_intermediate_input_line(
                        &limit_instance,
                        delta,
                        &init_state,
                        line,
                        line_num,
                        &parent_state_enc
                        );

#ifdef FCS_DBM_SINGLE_THREAD
#define NUM_THREADS() 1
#else
#define NUM_THREADS() num_threads
#endif
                    instance_run_all_threads(
                        &limit_instance, &init_state, &parent_state_enc, NUM_THREADS()
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
                    fcs_encoded_state_buffer_t parent_state_enc;
                    populate_instance_with_intermediate_input_line(
                        &queue_instance,
                        delta,
                        &init_state,
                        line,
                        line_num,
                        &parent_state_enc
                        );

                    instance_run_all_threads(
                        &queue_instance, &init_state, &parent_state_enc, NUM_THREADS()
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
    {
        fcs_dbm_solver_instance_t instance;
        fcs_encoded_state_buffer_t * key_ptr;
#define KEY_PTR() (key_ptr)

        fcs_encoded_state_buffer_t parent;

        instance_init(&instance, pre_cache_max_count, caches_delta,
                      dbm_store_path, max_count_of_items_in_queue,
                      iters_delta_limit, offload_dir_path, out_fh);

        key_ptr = &(instance.first_key);
        fcs_init_and_encode_state(delta, &(init_state), KEY_PTR());

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
        token = fc_solve_dbm_store_insert_key_value(instance.store, KEY_PTR(), NULL);
#endif

        fcs_offloading_queue__insert(&(instance.queue), (fcs_offloading_queue_item_t *)&token);
        instance.num_states_in_collection++;
        instance.count_of_items_in_queue++;

        instance_run_all_threads(&instance, &init_state, &parent, NUM_THREADS());
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
