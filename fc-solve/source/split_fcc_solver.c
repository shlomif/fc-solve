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
 * split_fcc_solver.c - a specialised solver that solves one FCC (=
 * fully connected component) at a time - meant to try to
 * reduce the memory consumption. See
 * ../docs/split-fully-connected-components-based-solver-planning.txt
 * in the Freecell Solver git repository.
 *
 */

#if 0
#define DEBUG_OUT 1
#endif

#include "dbm_solver_head.h"
#include <sys/tree.h>
#include <assert.h>
#include "count.h"

#include "depth_multi_queue.h"

#ifdef FCS_DEBONDT_DELTA_STATES

static GCC_INLINE int compare_enc_states(
    const fcs_encoded_state_buffer_t * a, const fcs_encoded_state_buffer_t * b
)
{
    return memcmp(a,b, sizeof(*a));
}

#else

static GCC_INLINE int compare_enc_states(
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
    fcs_meta_compact_allocator_t queue_meta_alloc;
    fcs_depth_multi_queue_t depth_queue;
} fcs_dbm_collection_by_depth_t;

typedef struct FccEntryPointList FccEntryPointList;
typedef struct FccEntryPointNode FccEntryPointNode;

typedef union
{
#if 0
    fcs_dbm_record_t reserve_space;
#endif
    struct
    {
        /* We don't really need it. */
        long location_in_file;
#if 0
        int depth;
        int was_consumed: 1;
        int is_reachable: 1;
#endif
    };
} fcc_entry_point_value_t;

typedef struct
{
    fcs_encoded_state_buffer_t key;
    fcc_entry_point_value_t val;
} fcc_entry_point_t;

struct FccEntryPointNode
{
    RB_ENTRY(FccEntryPointNode) entry_;
    fcc_entry_point_t kv;
};

static int FccEntryPointNode_compare(FccEntryPointNode * a, FccEntryPointNode * b)
{
    return compare_enc_states(&(a->kv.key), &(b->kv.key));
}

RB_HEAD(FccEntryPointList, FccEntryPointNode);
const FccEntryPointList FccEntryPointList_init = RB_INITIALIZER(&(instance->fcc_entry_points));

typedef struct
{
    fcs_lock_t global_lock;
    void * tree_recycle_bin;
    fcs_lock_t storage_lock;
    const char * offload_dir_path;
    int curr_depth;
    fcs_dbm_collection_by_depth_t coll;
    long pre_cache_max_count;
    /* The queue */
    long count_num_processed, count_of_items_in_queue, max_count_num_processed;
    fcs_bool_t queue_solution_was_found;
    enum TERMINATE_REASON should_terminate;
#ifdef FCS_DBM_WITHOUT_CACHES
    fcs_dbm_record_t * queue_solution_ptr;
    fcs_dbm_record_t physical_queue_solution;
#else
    fcs_encoded_state_buffer_t queue_solution;

#endif
    int queue_num_extracted_and_processed;
    long num_states_in_collection;
    FILE * out_fh;
    fcs_encoded_state_buffer_t first_key;
    enum fcs_dbm_variant_type_t variant;
    fcs_meta_compact_allocator_t fcc_meta_alloc;
    FccEntryPointList fcc_entry_points;
    fcs_compact_allocator_t fcc_entry_points_allocator;
    fcs_lock_t fcc_entry_points_lock;
    FccEntryPointNode * start_key_ptr;
#if 0
    fcs_bool_t was_start_key_reachable;
#endif
    int start_key_moves_count;
    fcs_lock_t output_lock;
#if 0
    FILE * consumed_states_fh;
#endif
    fcs_which_moves_bitmask_t fingerprint_which_irreversible_moves_bitmask;
    fcs_lock_t fcc_exit_points_output_lock;
    FILE * fcc_exit_points_out_fh;
    FILE * fingerprint_fh;
    char * fingerprint_line;
    size_t fingerprint_line_size;
    unsigned char * moves_to_state;
    size_t max_moves_to_state_len;
    size_t moves_to_state_len;
    char * moves_base64_encoding_buffer;
    size_t moves_base64_encoding_buffer_max_len;
    const char * dbm_store_path;
} fcs_dbm_solver_instance_t;

#define __unused GCC_UNUSED
RB_GENERATE_STATIC(FccEntryPointList, FccEntryPointNode, entry_, FccEntryPointNode_compare);

static GCC_INLINE void instance_init(
    fcs_dbm_solver_instance_t * instance,
    enum fcs_dbm_variant_type_t local_variant,
    long pre_cache_max_count,
    long caches_delta,
    const char * dbm_store_path,
    const char * dbm_fcc_entry_points_path,
    long iters_delta_limit,
    const char * offload_dir_path,
    fcs_which_moves_bitmask_t * fingerprint_which_irreversible_moves_bitmask,
    FILE * out_fh
)
{
    fcs_dbm_collection_by_depth_t * coll;


    instance->dbm_store_path = dbm_store_path;
    instance->variant = local_variant;

    instance->fingerprint_which_irreversible_moves_bitmask =
        (*fingerprint_which_irreversible_moves_bitmask);

    instance->moves_base64_encoding_buffer = NULL;
    instance->moves_base64_encoding_buffer_max_len = 0;
    {
        int curr_depth = 0;
        {
            unsigned char c;
            for (int i = 0 ;
                i < COUNT(fingerprint_which_irreversible_moves_bitmask->s) ;
                i++)
            {
                c = fingerprint_which_irreversible_moves_bitmask->s[i];
                while (c != 0)
                {
                    curr_depth += (c & 0x3);
                    c >>= 2;
                }
            }
        }
        instance->curr_depth = curr_depth;
    }

    FCS_INIT_LOCK(instance->global_lock);
    instance->offload_dir_path = offload_dir_path;

    instance->out_fh = out_fh;

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
    instance->tree_recycle_bin = NULL;

    FCS_INIT_LOCK(instance->storage_lock);
    FCS_INIT_LOCK(instance->output_lock);
    FCS_INIT_LOCK(instance->fcc_entry_points_lock);
    FCS_INIT_LOCK(instance->fcc_exit_points_output_lock);
    {
        coll = &(instance->coll);
        FCS_INIT_LOCK(coll->queue_lock);

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

    fc_solve_meta_compact_allocator_init(
        &(instance->fcc_meta_alloc)
    );
    instance->fcc_entry_points = FccEntryPointList_init;

    fc_solve_compact_allocator_init(&(instance->fcc_entry_points_allocator), &(instance->fcc_meta_alloc));

    instance->start_key_ptr = NULL;
}

static GCC_INLINE void instance_recycle(
    fcs_dbm_solver_instance_t * instance
    )
{
     {
         fcs_dbm_collection_by_depth_t * coll = &(instance->coll);

         fcs_depth_multi_queue__destroy(&(coll->depth_queue));
/*  DONE at the loop level. */
#if 0

#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
         fcs_offloading_queue__init(&(coll->queue), NUM_ITEMS_PER_PAGE, instance->offload_dir_path, depth);
#else
         fcs_offloading_queue__init(&(coll->queue), &(coll->queue_meta_alloc));
#endif

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
    fcs_dbm_collection_by_depth_t * coll;

    fc_solve_compact_allocator_finish(&(instance->fcc_entry_points_allocator));
    fc_solve_meta_compact_allocator_finish(&(instance->fcc_meta_alloc));
    {
        coll = &(instance->coll);
        fcs_depth_multi_queue__destroy(&(coll->depth_queue));
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
        fc_solve_meta_compact_allocator_finish(&(coll->queue_meta_alloc));
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
    FCS_DESTROY_LOCK(instance->global_lock);
    FCS_DESTROY_LOCK(instance->fcc_entry_points_lock);
    FCS_DESTROY_LOCK(instance->fcc_exit_points_output_lock);
    FCS_DESTROY_LOCK(instance->output_lock);
}

#define CHECK_KEY_CALC_DEPTH() \
    ( instance->curr_depth + list->num_non_reversible_moves_including_prune)

#include "dbm_procs.h"
#include "fcs_base64.h"

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
    fcs_meta_compact_allocator_t meta_alloc;
    enum fcs_dbm_variant_type_t local_variant;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    local_variant = instance->variant;

    fc_solve_meta_compact_allocator_init(&meta_alloc);
    fc_solve_compact_allocator_init(&(derived_list_allocator), &meta_alloc);
    fc_solve_delta_stater_decode_into_state(
        delta,
        parent.s,
        &parent_state,
        indirect_stacks_buffer
    );

    derived_list = NULL;
    derived_list_recycle_bin = NULL;

    instance_solver_thread_calc_derived_states(
        local_variant,
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
            local_variant,
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
    fc_solve_meta_compact_allocator_finish(&meta_alloc);

    return move_to_return;
}

struct fcs_dbm_solver_thread_struct
{
    fcs_dbm_solver_instance_t * instance;
    fc_solve_delta_stater_t * delta_stater;
    fcs_meta_compact_allocator_t thread_meta_alloc;
    int state_depth;
};

static GCC_INLINE void instance_check_key(
    fcs_dbm_solver_thread_t * thread,
    fcs_dbm_solver_instance_t * instance,
    int key_depth,
    fcs_encoded_state_buffer_t * key,
    fcs_dbm_record_t * parent,
    unsigned char move,
    fcs_which_moves_bitmask_t * which_irreversible_moves_bitmask
#ifdef FCS_DBM_CACHE_ONLY
    , const fcs_fcc_move_t * moves_to_parent
#endif
)
{
#ifdef DEBUG_OUT
    fcs_state_locs_struct_t locs;
    fc_solve_init_locs(&locs);
    enum fcs_dbm_variant_type_t local_variant = instance->variant;;
#endif

    fcs_dbm_collection_by_depth_t * coll;
    coll = &(instance->coll);
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
            if ((token = fc_solve_dbm_store_insert_key_value(coll->store, key, parent, TRUE)))
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

                if (key_depth == instance->curr_depth)
                {
                    /* Now insert it into the queue. */

                    FCS_LOCK(coll->queue_lock);
                    fcs_depth_multi_queue__insert(
                        &(coll->depth_queue),
                        thread->state_depth+1,
                        (const fcs_offloading_queue_item_t *)(&token)
                    );
                    FCS_UNLOCK(coll->queue_lock);

                    FCS_LOCK(instance->global_lock);

                    instance->count_of_items_in_queue++;
                    instance->num_states_in_collection++;

                    instance_debug_out_state(instance, &(token->key));

                    FCS_UNLOCK(instance->global_lock);
                }
                else
                {
                    /* Handle an irreversible move */

                    /* Calculate the new fingerprint to which the exit
                     * point belongs. */
                    fcs_which_moves_bitmask_t new_fingerprint = {{'\0'}};
                    for (int i=0 ; i < COUNT(new_fingerprint.s) ; i++)
                    {
                        new_fingerprint.s[i] = which_irreversible_moves_bitmask->s[i] + instance->fingerprint_which_irreversible_moves_bitmask.s[i];
                    }
                    int trace_num;
                    fcs_encoded_state_buffer_t * trace;
                    FCS_LOCK(instance->fcc_exit_points_output_lock);
                    /* instance->storage_lock is already locked
                     * in instance_check_multiple_keys and we should not
                     * lock it here. */
                    calc_trace(instance, token, &trace, &trace_num);

                    {
                        FccEntryPointNode fcc_entry_key;
                        fcc_entry_key.kv.key = trace[trace_num-1];
                        FccEntryPointNode * val_proto = RB_FIND(
                            FccEntryPointList,
                            &(instance->fcc_entry_points),
                            &fcc_entry_key
                        );
                        const long location_in_file = val_proto->kv.val.location_in_file;
                        fseek(instance->fingerprint_fh, location_in_file, SEEK_SET);

                        getline(&(instance->fingerprint_line), &(instance->fingerprint_line_size), instance->fingerprint_fh);
                        char * moves_to_state_enc = strchr(instance->fingerprint_line, ' ');
                        moves_to_state_enc = strchr(moves_to_state_enc+1, ' ');
                        moves_to_state_enc++;
                        char * trailing_newline = strchr(moves_to_state_enc, '\n');
                        if (trailing_newline)
                        {
                            *trailing_newline = '\0';
                        }
                        const size_t string_len = strlen(moves_to_state_enc);
                        const size_t buffer_size = (((string_len * 3) >> 2) + 20);
                        if (buffer_size > instance->max_moves_to_state_len)
                        {
                            instance->moves_to_state = SREALLOC(
                                instance->moves_to_state,
                                buffer_size
                            );
                            instance->max_moves_to_state_len = buffer_size;
                        }
                        base64_decode (moves_to_state_enc, string_len,
                            ((unsigned char *)instance->moves_to_state),
                            &(instance->moves_to_state_len));
                    }


                    const size_t moves_to_state_len = instance->moves_to_state_len;
                    const size_t added_moves_to_output = moves_to_state_len + trace_num-1;
                    if (added_moves_to_output > instance->max_moves_to_state_len)
                    {
                        instance->moves_to_state = SREALLOC(
                            instance->moves_to_state,
                            added_moves_to_output
                        );
                        instance->max_moves_to_state_len = added_moves_to_output;
                    }
                    unsigned char * const moves_to_state
                        = instance->moves_to_state;
                    for (int i = trace_num-1 ; i > 0 ; i--)
                    {
                        moves_to_state[moves_to_state_len+trace_num-1-i] =
                            get_move_from_parent_to_child(
                                instance,
                                thread->delta_stater,
                                trace[i],
                                trace[i-1]
                            );
                    }

                    const size_t new_max_enc_len =
                        ((added_moves_to_output * 4) / 3) + 20;

                    if (new_max_enc_len > instance->moves_base64_encoding_buffer_max_len)
                    {
                        instance->moves_base64_encoding_buffer =
                            SREALLOC(
                                instance->moves_base64_encoding_buffer,
                                new_max_enc_len
                            );
                        instance->moves_base64_encoding_buffer_max_len
                            = new_max_enc_len;
                    }

                    {
                        size_t unused_output_len;
                        base64_encode(
                            moves_to_state,
                            added_moves_to_output,
                            instance->moves_base64_encoding_buffer,
                            &unused_output_len
                        );
                    }
                    char fingerprint_base64[100];
                    char state_base64[100];
                    size_t unused_output_len;
                    base64_encode(
                        new_fingerprint.s,
                        sizeof(new_fingerprint),
                        fingerprint_base64,
                        &unused_output_len
                    );
                    base64_encode(
                        (unsigned char *)&(*key),
                        sizeof(*key),
                        state_base64,
                        &unused_output_len
                    );
                    /* Output the exit point. */
                    fprintf(
                        instance->fcc_exit_points_out_fh,
                        "%s %s %ld %s\n",
                        fingerprint_base64,
                        state_base64,
                        added_moves_to_output,
                        instance->moves_base64_encoding_buffer
                    );
#ifdef DEBUG_OUT
                    {
                        fcs_state_keyval_pair_t state;
                        DECLARE_IND_BUF_T(indirect_stacks_buffer)
                        fc_solve_delta_stater_decode_into_state(
                            thread->delta_stater,
                            key->s,
                            &state,
                            indirect_stacks_buffer
                            );
                        char * state_as_str =
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
                    fprintf(
                        stderr,
                        "Check Key: <<<\n%s\n>>>\n\n[%s %s %ld %s]\n\n",
                        state_as_str,
                        fingerprint_base64,
                        state_base64,
                        added_moves_to_output,
                        instance->moves_base64_encoding_buffer
                    );
                    free(state_as_str);
                    }
#endif
                    fflush(instance->fcc_exit_points_out_fh);

                    FCS_UNLOCK(instance->fcc_exit_points_output_lock);

                    free(trace);
                }
            }
    }
}



typedef struct {
    fcs_dbm_solver_thread_t * thread;
} thread_arg_t;


static void * instance_run_solver_thread(void * void_arg)
{
    thread_arg_t * arg;
    fcs_dbm_collection_by_depth_t * coll;

    enum TERMINATE_REASON should_terminate;
    enum fcs_dbm_variant_type_t local_variant;
    fcs_dbm_solver_thread_t * thread;
    fcs_dbm_solver_instance_t * instance;
    fcs_dbm_queue_item_t physical_item;
    fcs_dbm_record_t * token;
    fcs_dbm_queue_item_t * item, * prev_item;
    int queue_num_extracted_and_processed;
    fcs_derived_state_t * derived_list, * derived_list_recycle_bin,
                        * derived_iter;
    fcs_compact_allocator_t derived_list_allocator;
    fc_solve_delta_stater_t * delta_stater;
    fcs_state_keyval_pair_t state;
    FILE * out_fh;
    char * base64_encoding_buffer = NULL;
#if 0
    size_t base64_encoding_buffer_max_len = 0;
#endif

#ifdef DEBUG_OUT
    fcs_state_locs_struct_t locs;
    fc_solve_init_locs(&locs);
#endif
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    arg = (thread_arg_t *)void_arg;
    thread = arg->thread;
    instance = thread->instance;
    delta_stater = thread->delta_stater;
    local_variant = instance->variant;

    prev_item = item = NULL;
    queue_num_extracted_and_processed = 0;

    fc_solve_compact_allocator_init(&(derived_list_allocator), &(thread->thread_meta_alloc));
    derived_list_recycle_bin = NULL;
    derived_list = NULL;
    out_fh = instance->out_fh;

    TRACE0("instance_run_solver_thread start");

    coll = &(instance->coll);

#if 0
    fcs_bool_t was_start_key_reachable = instance->was_start_key_reachable;
#endif

    while (1)
    {
        /* First of all extract an item. */
        FCS_LOCK(coll->queue_lock);

        if (prev_item)
        {
            FCS_LOCK(instance->global_lock);
            instance->queue_num_extracted_and_processed--;
            FCS_UNLOCK(instance->global_lock);
        }

        if ((should_terminate = instance->should_terminate) == DONT_TERMINATE)
        {
            if (fcs_depth_multi_queue__extract(&(coll->depth_queue), &(thread->state_depth), (fcs_offloading_queue_item_t *)(&token)))
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
                FREECELLS_NUM,
                STACKS_NUM,
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

#if 0
        {
            FccEntryPointNode key;
            key.kv.key = item->key;
            FCS_LOCK(instance->fcc_entry_points_lock);
            FccEntryPointNode * val_proto = RB_FIND(
                FccEntryPointList,
                &(instance->fcc_entry_points),
                &(key)
            );

            fcs_bool_t to_prune = FALSE;
            fcs_bool_t to_output = FALSE;
            if (val_proto)
            {
                val_proto->kv.val.is_reachable = TRUE;
                const int moves_count = instance->start_key_moves_count
                    + item->moves_seq.count;

                if (was_start_key_reachable)
                {
                    if (val_proto->kv.val.depth <= moves_count)
                    {
                        /* We can prune based on here. */
                        to_prune = TRUE;
                    }
                    else
                    {
                        /* We can set it to the more pessimstic move count
                         * for future trimming. */
                        to_output = !(val_proto->kv.val.was_consumed);
                        val_proto->kv.val.depth = moves_count;
                        val_proto->kv.val.was_consumed = TRUE;
                    }
                }
                else
                {
                    if (! val_proto->kv.val.was_consumed)
                    {
                        to_output = TRUE;
                        if (val_proto->kv.val.depth >= moves_count)
                        {
                            val_proto->kv.val.depth = moves_count;
                            val_proto->kv.val.was_consumed = TRUE;
                        }
                    }
                }
            }
            FCS_UNLOCK(instance->fcc_entry_points_lock);

            if (to_output)
            {
                const size_t needed_len = ( (sizeof(key.kv.key)+2) << 2 ) / 3 + 20;
                if (base64_encoding_buffer_max_len < needed_len)
                {
                    base64_encoding_buffer = SREALLOC(base64_encoding_buffer, needed_len);
                    base64_encoding_buffer_max_len = needed_len;
                }

                size_t unused_output_len;

                base64_encode(
                    (unsigned char *)&(key.kv.key),
                    sizeof(key.kv.key),
                    base64_encoding_buffer,
                    &unused_output_len
                );

                FCS_LOCK(instance->output_lock);
                fprintf(
                    instance->consumed_states_fh,
                    "%s\n", base64_encoding_buffer
                );
                fflush(instance->consumed_states_fh);
                FCS_UNLOCK(instance->output_lock);
            }

            if (to_prune)
            {
                continue;
            }
        }
#endif

        if (instance_solver_thread_calc_derived_states(
            local_variant,
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
                local_variant,
                &(derived_iter->state),
                &(derived_iter->key)
            );
        }

        instance_check_multiple_keys(thread, instance, derived_list
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

    free (base64_encoding_buffer);

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

static void instance_run_all_threads(
    fcs_dbm_solver_instance_t * instance,
    fcs_state_keyval_pair_t * init_state,
    FccEntryPointNode * key_ptr,
    int num_threads)
{
    int i, check;
    main_thread_item_t * threads;

#ifndef FCS_FREECELL_ONLY
    int local_variant;
#endif
#ifdef T
    FILE * out_fh = instance->out_fh;
#endif

#ifndef FCS_FREECELL_ONLY
    local_variant = instance->variant;
#endif

    threads = SMALLOC(threads, num_threads);

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

        fc_solve_meta_compact_allocator_init(
            &(threads[i].thread.thread_meta_alloc)
        );
        threads[i].arg.thread = &(threads[i].thread);
    }

    /* TODO : do something meaningful with start_key_ptr . */
    instance->start_key_ptr = key_ptr;

#if 0
    instance->was_start_key_reachable = (key_ptr->kv.val.is_reachable);
    instance->start_key_moves_count = (key_ptr->kv.val.depth);
#endif

    {
        TRACE1("Running threads for curr_depth=%d\n", 0);
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
        TRACE1("Finished running threads for curr_depth=%d\n", instance->curr_depth);
        if (! instance->queue_solution_was_found)
        {
            /* Now that we are about to ascend to a new depth, let's mark-and-sweep
             * the old states, some of which are no longer of interest.
             * */
            dict_t * kaz_tree;
            struct avl_traverser trav;
            dict_key_t item;
            struct avl_node * ancestor;
            struct avl_node * * tree_recycle_bin;
            size_t items_count, idx;

            TRACE1("Start mark-and-sweep cleanup for curr_depth=%d\n", instance->curr_depth);
            tree_recycle_bin =
                (
                    (struct avl_node * *)(&(instance->tree_recycle_bin))
                );

            kaz_tree = fc_solve_dbm_store_get_dict(
                instance->coll.store
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
            TRACE1("Finish mark-and-sweep cleanup for curr_depth=%d\n", instance->curr_depth);
        }
    }

    for (i=0; i < num_threads ; i++)
    {
        fc_solve_delta_stater_free(threads[i].thread.delta_stater);
        fc_solve_meta_compact_allocator_finish(
            &(threads[i].thread.thread_meta_alloc)
        );
    }

    free(threads);

#ifdef DEBUG_FOO
    fc_solve_delta_stater_free(global_delta_stater);
#endif

    TRACE0("instance_run_all_threads end");
    return;
}




static void trace_solution(
    fcs_dbm_solver_instance_t * instance,
    FILE * out_fh,
    fc_solve_delta_stater_t * delta
)
{
    enum fcs_dbm_variant_type_t local_variant;
    fcs_encoded_state_buffer_t * trace;
    int trace_num;
    int i;
    fcs_state_keyval_pair_t state;
    unsigned char move = '\0';
    char * state_as_str;
    char move_buffer[500];
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    fcs_state_locs_struct_t locs;

    local_variant = instance->variant;

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
        if (instance->should_terminate == MAX_ITERS_TERMINATE)
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


static GCC_INLINE void release_starting_state_specific_instance_resources(
    fcs_dbm_solver_instance_t * instance
)
{
    fcs_dbm_collection_by_depth_t * coll = &(instance->coll);

    /* TODO : Implement. */
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
        /* That is important to avoid stale nodes. */
        instance->tree_recycle_bin = NULL;

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
        pre_cache_init (&(coll->pre_cache), &(coll->meta_alloc));
#endif
        coll->pre_cache_max_count = pre_cache_max_count;
        cache_init (&(coll->cache), pre_cache_max_count+caches_delta, &(coll->meta_alloc));
#endif
#ifndef FCS_DBM_CACHE_ONLY
        fc_solve_dbm_store_init(&(coll->store), instance->dbm_store_path, &(instance->tree_recycle_bin));
#endif
}

int main(int argc, char * argv[])
{
    build_decoding_table();

#ifdef DEBUG_OUT
    fcs_state_locs_struct_t locs;
    fc_solve_init_locs(&locs);
#endif

    long pre_cache_max_count;
    long caches_delta;
    long iters_delta_limit = -1;
#if 0
    long start_line = 1;
#endif
    const char * dbm_store_path;
    const char * mod_base64_fcc_fingerprint = NULL;
    const char * fingerprint_input_location_path = NULL;
    int num_threads;
    int arg;
    const char * path_to_output_dir = NULL;
    const char * filename = NULL, * offload_dir_path = NULL;
    FILE * fh = NULL, * out_fh = NULL;
    char user_state[USER_STATE_SIZE];
    fc_solve_delta_stater_t * delta;
    fcs_dbm_record_t * token;
    enum fcs_dbm_variant_type_t local_variant = FCS_DBM_VARIANT_2FC_FREECELL;

    fcs_state_keyval_pair_t init_state;
#if 0
    fcs_bool_t skip_queue_output = FALSE;
#endif
    DECLARE_IND_BUF_T(init_indirect_stacks_buffer)

    pre_cache_max_count = 1000000;
    caches_delta = 1000000;
    dbm_store_path = "./fc_solve_dbm_store";
    const char * dbm_fcc_entry_points_path = "./fc_solve_fcc_entry_points_dbm_store";

    num_threads = 1;

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
        else if (!strcmp(argv[arg], "--board"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--board came without an argument.");
                exit(-1);
            }
            filename = argv[arg];
        }
        else if (!strcmp(argv[arg], "--game"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--game came without an argument!\n");
                exit(-1);
            }
            if (!strcmp(argv[arg], "bakers_dozen"))
            {
                local_variant = FCS_DBM_VARIANT_BAKERS_DOZEN;
            }
            else if (!strcmp(argv[arg], "freecell"))
            {
                local_variant = FCS_DBM_VARIANT_2FC_FREECELL;
            }
            else
            {
                fprintf(stderr, "Unknown game '%s'. Aborting\n", argv[arg]);
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
        else if (!strcmp(argv[arg], "--output"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--output came without an argument.\n");
                exit(-1);
            }
            path_to_output_dir = argv[arg];
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
        else if (!strcmp(argv[arg], "--fingerprint"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--fingerprint came without an argument.\n");
                exit(-1);
            }
            mod_base64_fcc_fingerprint = argv[arg];
        }
        else if (!strcmp(argv[arg], "--input"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--input came without an argument.\n");
                exit(-1);
            }
            fingerprint_input_location_path = argv[arg];
        }
        else
        {
            break;
        }
    }

    if (arg != argc)
    {
        fprintf (stderr, "%s\n", "Junk at the end of the parameters");
        exit(-1);
    }

    if (!
        (
               filename
            && fingerprint_input_location_path
            && mod_base64_fcc_fingerprint
            && path_to_output_dir
            && offload_dir_path
        )
    )
    {
        fprintf(stderr, "One or more of these parameters was not specified: %s\n",
            "--board, --fingerprint, --input, --output, --offload-dir-path"
        );
        exit(-1);
    }

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

    fcs_which_moves_bitmask_t initial_which_irreversible_moves_bitmask
        = {{'\0'}};
#if 0
    int initial_irrev_moves_depth =
#endif
    horne_prune(local_variant, &init_state,
        &initial_which_irreversible_moves_bitmask, NULL, NULL
    );

    fcs_which_moves_bitmask_t fingerprint_which_irreversible_moves_bitmask
        = {{'\0'}};

    size_t fingerprint_data_len = 0;
    base64_decode(
        mod_base64_fcc_fingerprint, strlen(mod_base64_fcc_fingerprint),
        fingerprint_which_irreversible_moves_bitmask.s, &fingerprint_data_len);

    if (fingerprint_data_len != sizeof(fingerprint_which_irreversible_moves_bitmask))
    {
        fprintf(stderr, "%s\n", "--fingerprint is invalid length.");
        exit(-1);
    }

    /* Calculate the fingerprint_which_irreversible_moves_bitmask's curr_depth. */

    delta = fc_solve_delta_stater_alloc(
            &init_state.s,
            STACKS_NUM,
            FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
            , ((local_variant == FCS_DBM_VARIANT_BAKERS_DOZEN)
               ? FCS_SEQ_BUILT_BY_RANK
               : FCS_SEQ_BUILT_BY_ALTERNATE_COLOR)
#endif
    );

    {
        out_fh = stdout;

        fcs_dbm_solver_instance_t instance;
        FccEntryPointNode * key_ptr;

#define KEY_PTR() (key_ptr)

        fcs_encoded_state_buffer_t parent_state_enc;

        instance_init(
            &instance, local_variant, pre_cache_max_count,
            caches_delta, dbm_store_path, dbm_fcc_entry_points_path,
            iters_delta_limit, offload_dir_path,
            &fingerprint_which_irreversible_moves_bitmask,
            out_fh
        );

        FILE * fingerprint_fh = fopen(fingerprint_input_location_path, "rt");

        if (! fingerprint_fh)
        {
            fprintf(stderr, "Cannot open '%s' for reading. Exiting.",
                fingerprint_input_location_path
            );
            exit(-1);
        }

        instance.fingerprint_line = NULL;
        instance.fingerprint_line_size = 0;
        ssize_t read;

        long location_in_file = 0;
        fcs_bool_t was_init = FALSE;
        while ((read = getline(&instance.fingerprint_line, &instance.fingerprint_line_size, fingerprint_fh)) != -1)
        {
            char state_base64[100];
            int state_depth;
            FccEntryPointNode * entry_point =
                fcs_compact_alloc_ptr(
                    &(instance.fcc_entry_points_allocator),
                    sizeof(*entry_point)
                );
            sscanf(instance.fingerprint_line, "%99s %d",
                state_base64,
                &state_depth
            );
            size_t unused_size;
            base64_decode(state_base64, strlen(state_base64),
                ((unsigned char *)&(entry_point->kv.key)), &(unused_size));

            assert (unused_size == sizeof(entry_point->kv.key));

            /* TODO : Should traverse starting from key. */
            key_ptr = entry_point;
            /* The NULL parent_state_enc and move for indicating this is the
             * initial state. */
            fcs_init_encoded_state(&(parent_state_enc));

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
            pre_cache_insert(&(instance.pre_cache), &(key_ptr->kv.key), &parent_state_enc);
#else
            cache_insert(&(instance.cache), &(key_ptr->kv.key), NULL, '\0');
#endif
#else
            token = fc_solve_dbm_store_insert_key_value(instance.coll.store, &(key_ptr->kv.key), NULL, TRUE);
#endif

            entry_point->kv.val.location_in_file = location_in_file;
#if 0
            entry_point->kv.val.was_consumed =
                entry_point->kv.val.is_reachable =
                FALSE;
#endif

            RB_INSERT(
                FccEntryPointList,
                &(instance.fcc_entry_points),
                entry_point
            );

            if (was_init)
            {
                fcs_depth_multi_queue__insert(
                    &(instance.coll.depth_queue),
                    state_depth,
                    (const fcs_offloading_queue_item_t *)(&token)
                );
            }
            else
            {
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
#define NUM_ITEMS_PER_PAGE (128 * 1024)
                fcs_depth_multi_queue__init(
                    &(instance.coll.depth_queue),
                    NUM_ITEMS_PER_PAGE,
                    instance.offload_dir_path,
                    state_depth,
                    (const fcs_offloading_queue_item_t *)(&token)
                );
#else
                fc_solve_meta_compact_allocator_init(
                    &(coll->queue_meta_alloc)
                );
                fcs_offloading_queue__init(&(coll->queue), &(coll->queue_meta_alloc));
#endif
            }
            instance.num_states_in_collection++;
            instance.count_of_items_in_queue++;
            /* Valid for the next iteration: */
            location_in_file = ftell(fingerprint_fh);
        }

        fseek (fingerprint_fh, 0, SEEK_SET);
        instance.fingerprint_fh = fingerprint_fh;

        long count_of_instance_runs = 0;
        instance.moves_to_state  = NULL;
        instance.max_moves_to_state_len = instance.moves_to_state_len = 0;

        count_of_instance_runs++;

        char fcc_exit_points_out_fn[PATH_MAX], fcc_exit_points_out_fn_temp[PATH_MAX];
        sprintf(fcc_exit_points_out_fn, "%s/exits.%ld", path_to_output_dir, count_of_instance_runs);
        sprintf(fcc_exit_points_out_fn_temp, "%s.temp", fcc_exit_points_out_fn);
        instance.fcc_exit_points_out_fh
            = fopen(fcc_exit_points_out_fn_temp, "wt");

        instance_run_all_threads(&instance, &init_state, key_ptr, NUM_THREADS());
        fclose (instance.fcc_exit_points_out_fh);
        rename(fcc_exit_points_out_fn_temp, fcc_exit_points_out_fn);
#if 0
        while ((read = getline(&fingerprint_line, &fingerprint_line_size, fingerprint_fh)) != -1)
        {
            char state_base64[100];
            FccEntryPointNode key;
            int state_depth;
            sscanf( fingerprint_line, "%99s %d", state_base64, &state_depth );
            size_t unused_size;
            base64_decode(state_base64, strlen(state_base64),
                ((unsigned char *)&(key.kv.key)), &(unused_size));


            FccEntryPointNode * val_proto = RB_FIND(
                FccEntryPointList,
                &(instance.fcc_entry_points),
                &(key)
            );

            if (! val_proto->kv.val.was_consumed)
            {
                char * moves_to_state_enc = strchr(fingerprint_line, ' ');
                moves_to_state_enc = strchr(moves_to_state_enc+1, ' ');
                moves_to_state_enc++;
                char * trailing_newline = strchr(moves_to_state_enc, '\n');
                if (trailing_newline)
                {
                    *trailing_newline = '\0';
                }
                const size_t string_len = strlen(moves_to_state_enc);
                const size_t buffer_size = (((string_len * 3) >> 2) + 20);
                if (buffer_size > instance.max_moves_to_state_len)
                {
                    instance.moves_to_state = SREALLOC(
                        instance.moves_to_state,
                        buffer_size
                    );
                    instance.max_moves_to_state_len = buffer_size;
                }
                base64_decode (moves_to_state_enc, string_len,
                    ((unsigned char *)instance.moves_to_state),
                    &(instance.moves_to_state_len));

                /* TODO : Should traverse starting from key. */
                key_ptr = val_proto;
                /* The NULL parent_state_enc and move for indicating this is the
                 * initial state. */
                fcs_init_encoded_state(&(parent_state_enc));

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
                pre_cache_insert(&(instance.pre_cache), &(key_ptr->kv.key), &parent_state_enc);
#else
                cache_insert(&(instance.cache), &(key_ptr->kv.key), NULL, '\0');
#endif
#else
                token = fc_solve_dbm_store_insert_key_value(instance.coll.store, &(key_ptr->kv.key), NULL, TRUE);
#endif

                count_of_instance_runs++;

                char consumed_states_fn[PATH_MAX], consumed_states_fn_temp[PATH_MAX];
                sprintf(consumed_states_fn, "%s/consumed.%ld", path_to_output_dir, count_of_instance_runs);
                sprintf(consumed_states_fn_temp, "%s.temp", consumed_states_fn);
                instance.consumed_states_fh
                    = fopen(consumed_states_fn_temp, "wt");

                char fcc_exit_points_out_fn[PATH_MAX], fcc_exit_points_out_fn_temp[PATH_MAX];
                sprintf(fcc_exit_points_out_fn, "%s/exits.%ld", path_to_output_dir, count_of_instance_runs);
                sprintf(fcc_exit_points_out_fn_temp, "%s.temp", fcc_exit_points_out_fn);
                instance.fcc_exit_points_out_fh
                    = fopen(fcc_exit_points_out_fn_temp, "wt");

                instance_run_all_threads(&instance, &init_state, key_ptr, NUM_THREADS());

                fclose (instance.consumed_states_fh);
                rename(consumed_states_fn_temp, consumed_states_fn);

                fclose (instance.fcc_exit_points_out_fh);
                rename(fcc_exit_points_out_fn_temp, fcc_exit_points_out_fn);

                release_starting_state_specific_instance_resources(&instance);

            }

        }
#endif
        fclose(fingerprint_fh);
        handle_and_destroy_instance_solution(&instance, out_fh, delta);
        free (instance.moves_to_state);
        free (instance.moves_base64_encoding_buffer);
    }


    fc_solve_delta_stater_free(delta);
    delta = NULL;

    return 0;
}
#ifdef FCS_KAZ_TREE_USE_RECORD_DICT_KEY
#error DEFINED
#endif
