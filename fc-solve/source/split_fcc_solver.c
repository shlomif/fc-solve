/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
/*
 * split_fcc_solver.c - a specialised solver that solves one FCC (=
 * fully connected component) at a time - meant to try to
 * reduce the memory consumption. See
 * ../docs/split-fully-connected-components-based-solver-planning.txt
 * in the Freecell Solver git repository.
 */

#include "dbm_solver_head.h"
#include <sys/tree.h>
#include <assert.h>

#include "depth_multi_queue.h"

typedef struct
{
    fcs_dbm__cache_store__common_t cache_store;
    fcs_meta_compact_allocator_t queue_meta_alloc;
    fcs_depth_multi_queue_t depth_queue;
} fcs_dbm_collection_by_depth_t;

typedef struct FccEntryPointList FccEntryPointList;
typedef struct FccEntryPointNode FccEntryPointNode;

struct FccEntryPointNode
{
    RB_ENTRY(FccEntryPointNode) entry_;
    struct
    {
        fcs_dbm_record_t key;
        /* fcc_entry_point_value_t */
        struct
        {
            /* We don't really need it. */
            long location_in_file;
#if 0
            int depth;
            int was_consumed: 1;
            int is_reachable: 1;
#endif
        } val;
    } kv;
};

static GCC_INLINE int FccEntryPointNode_compare(
    FccEntryPointNode *a, FccEntryPointNode *b)
{
#define MAP(x) &((x)->kv.key.key)
    return compare_enc_states(MAP(a), MAP(b));
}

RB_HEAD(FccEntryPointList, FccEntryPointNode);
const FccEntryPointList FccEntryPointList_init =
    RB_INITIALIZER(&(instance->fcc_entry_points));

typedef struct
{
    fcs_dbm_collection_by_depth_t coll;
    fcs_lock_t global_lock;
    fcs_lock_t storage_lock;
    const char *offload_dir_path;
    int curr_depth;
    fcs_dbm_instance_common_elems_t common;
    fcs_meta_compact_allocator_t fcc_meta_alloc;
    FccEntryPointList fcc_entry_points;
    fcs_compact_allocator_t fcc_entry_points_allocator;
    fcs_lock_t fcc_entry_points_lock;
    FccEntryPointNode *start_key_ptr;
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
    FILE *fcc_exit_points_out_fh;
    FILE *fingerprint_fh;
    char *fingerprint_line;
    size_t fingerprint_line_size;
    unsigned char *moves_to_state;
    size_t max_moves_to_state_len;
    size_t moves_to_state_len;
    char *moves_base64_encoding_buffer;
    size_t moves_base64_encoding_buffer_max_len;
    const char *dbm_store_path;
} fcs_dbm_solver_instance_t;

#define CHECK_KEY_CALC_DEPTH()                                                 \
    (instance->curr_depth + list->num_non_reversible_moves_including_prune)

#include "dbm_procs.h"
#include "fcs_base64.h"
#define __unused GCC_UNUSED
RB_GENERATE_STATIC(
    FccEntryPointList, FccEntryPointNode, entry_, FccEntryPointNode_compare);

static GCC_INLINE void instance_init(fcs_dbm_solver_instance_t *const instance,
    const fcs_dbm_variant_type_t local_variant,
    const long pre_cache_max_count GCC_UNUSED,
    const long caches_delta GCC_UNUSED, const char *dbm_store_path,
    const long iters_delta_limit, const char *offload_dir_path,
    fcs_which_moves_bitmask_t *fingerprint_which_irreversible_moves_bitmask,
    FILE *out_fh)
{
    fc_solve_meta_compact_allocator_init(&(instance->fcc_meta_alloc));
    instance->fcc_entry_points = FccEntryPointList_init;

    fc_solve_compact_allocator_init(
        &(instance->fcc_entry_points_allocator), &(instance->fcc_meta_alloc));

    instance->start_key_ptr = NULL;
    instance->dbm_store_path = dbm_store_path;
    instance->fingerprint_which_irreversible_moves_bitmask =
        (*fingerprint_which_irreversible_moves_bitmask);

    instance->moves_base64_encoding_buffer = NULL;
    instance->moves_base64_encoding_buffer_max_len = 0;
    {
        int curr_depth = 0;
        {
            for (size_t i = 0;
                 i < COUNT(fingerprint_which_irreversible_moves_bitmask->s);
                 i++)
            {
                unsigned char c =
                    fingerprint_which_irreversible_moves_bitmask->s[i];
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
    fcs_dbm__common_init(
        &(instance->common), iters_delta_limit, local_variant, out_fh);

    FCS_INIT_LOCK(instance->storage_lock);
    FCS_INIT_LOCK(instance->output_lock);
    FCS_INIT_LOCK(instance->fcc_entry_points_lock);
    FCS_INIT_LOCK(instance->fcc_exit_points_output_lock);
    {
        fcs_dbm_collection_by_depth_t *const coll = &(instance->coll);
        FCS_INIT_LOCK(coll->queue_lock);

        fcs_dbm__cache_store__init(&(coll->cache_store), &(instance->common),
            &(coll->queue_meta_alloc), dbm_store_path, pre_cache_max_count,
            caches_delta);
    }
}

static GCC_INLINE void instance_destroy(fcs_dbm_solver_instance_t *instance)
{
    fcs_dbm_collection_by_depth_t *coll;

    fc_solve_compact_allocator_finish(&(instance->fcc_entry_points_allocator));
    fc_solve_meta_compact_allocator_finish(&(instance->fcc_meta_alloc));
    {
        coll = &(instance->coll);
        fcs_depth_multi_queue__destroy(&(coll->depth_queue));
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
        fc_solve_meta_compact_allocator_finish(&(coll->queue_meta_alloc));
#endif
        DESTROY_CACHE(coll);
        FCS_DESTROY_LOCK(coll->queue_lock);
    }
    FCS_DESTROY_LOCK(instance->storage_lock);
    FCS_DESTROY_LOCK(instance->global_lock);
    FCS_DESTROY_LOCK(instance->fcc_entry_points_lock);
    FCS_DESTROY_LOCK(instance->fcc_exit_points_output_lock);
    FCS_DESTROY_LOCK(instance->output_lock);
}

struct fcs_dbm_solver_thread_struct
{
    fcs_dbm_solver_instance_t *instance;
    fc_solve_delta_stater_t delta_stater;
    fcs_meta_compact_allocator_t thread_meta_alloc;
    int state_depth;
};

typedef struct
{
    fcs_dbm_solver_thread_t *thread;
} thread_arg_t;

static void *instance_run_solver_thread(void *const void_arg)
{
    fcs_dbm_queue_item_t physical_item;
    fcs_dbm_record_t *token;
    fcs_dbm_queue_item_t *item, *prev_item;
    fcs_derived_state_t *derived_list, *derived_list_recycle_bin, *derived_iter;
    fcs_compact_allocator_t derived_list_allocator;
    fcs_state_keyval_pair_t state;
    char *base64_encoding_buffer = NULL;
#if 0
    size_t base64_encoding_buffer_max_len = 0;
#endif

#ifdef DEBUG_OUT
    fcs_state_locs_struct_t locs;
    fc_solve_init_locs(&locs);
#endif
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    const_AUTO(thread, ((thread_arg_t *)void_arg)->thread);
    const_SLOT(instance, thread);
    const_AUTO(delta_stater, &(thread->delta_stater));
    const_AUTO(local_variant, instance->common.variant);

    prev_item = item = NULL;
    int queue_num_extracted_and_processed = 0;
    fc_solve_compact_allocator_init(
        &(derived_list_allocator), &(thread->thread_meta_alloc));
    derived_list_recycle_bin = NULL;
    derived_list = NULL;
    FILE *const out_fh = instance->common.out_fh;
    TRACE("%s\n", "instance_run_solver_thread start");
    const_AUTO(coll, &(instance->coll));
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
            instance->common.queue_num_extracted_and_processed--;
            FCS_UNLOCK(instance->global_lock);
        }

        if (instance->common.should_terminate == DONT_TERMINATE)
        {
            if (fcs_depth_multi_queue__extract(&(coll->depth_queue),
                    &(thread->state_depth),
                    (fcs_offloading_queue_item_t *)(&token)))
            {
                physical_item.key = token->key;
                item = &physical_item;
                instance_increment(instance);
            }
            else
            {
                item = NULL;
            }

            queue_num_extracted_and_processed =
                instance->common.queue_num_extracted_and_processed;
        }
        FCS_UNLOCK(coll->queue_lock);

        if ((instance->common.should_terminate != DONT_TERMINATE) ||
            (!queue_num_extracted_and_processed))
        {
            break;
        }

        if (!item)
        {
            /* Sleep until more items become available in the
             * queue. */
            usleep(5000);
        }
        else
        {
            /* Handle item. */
            fc_solve_delta_stater_decode_into_state(
                delta_stater, item->key.s, &state, indirect_stacks_buffer);

            /* A section for debugging. */
            FCS__OUTPUT_STATE(out_fh, "", &(state.s), &locs);
#if 0
        {
            FccEntryPointNode key;
            key.kv.key.key = item->key;
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
                const size_t needed_len = ( (sizeof(key.kv.key.key)+2) << 2 ) / 3 + 20;
                if (base64_encoding_buffer_max_len < needed_len)
                {
                    base64_encoding_buffer = SREALLOC(base64_encoding_buffer, needed_len);
                    base64_encoding_buffer_max_len = needed_len;
                }

                size_t unused_output_len;

                base64_encode(
                    (unsigned char *)&(key.kv.key.key),
                    sizeof(key.kv.key.key),
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

            if (instance_solver_thread_calc_derived_states(local_variant,
                    &state, token, &derived_list, &derived_list_recycle_bin,
                    &derived_list_allocator, TRUE))
            {
                FCS_LOCK(instance->global_lock);
                fcs_dbm__found_solution(&(instance->common), token, item);
                FCS_UNLOCK(instance->global_lock);
                break;
            }

            /* Encode all the states. */
            for (derived_iter = derived_list; derived_iter;
                 derived_iter = derived_iter->next)
            {
                fcs_init_and_encode_state(delta_stater, local_variant,
                    &(derived_iter->state), &(derived_iter->key));
            }

            instance_check_multiple_keys(thread, instance, &(coll->cache_store),
                &(coll->queue_meta_alloc), derived_list
#ifdef FCS_DBM_CACHE_ONLY
                ,
                item->moves_to_key
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

    free(base64_encoding_buffer);

    fc_solve_compact_allocator_finish(&(derived_list_allocator));

    TRACE("%s\n", "instance_run_solver_thread end");

    return NULL;
}

#include "depth_dbm_procs.h"

static GCC_INLINE void instance_alloc_num_moves(
    fcs_dbm_solver_instance_t *const instance, const size_t buffer_size)
{
    if (buffer_size > instance->max_moves_to_state_len)
    {
        instance->moves_to_state =
            SREALLOC(instance->moves_to_state, buffer_size);
        instance->max_moves_to_state_len = buffer_size;
    }
}

static GCC_INLINE void instance_check_key(fcs_dbm_solver_thread_t *const thread,
    fcs_dbm_solver_instance_t *const instance, const int key_depth,
    fcs_encoded_state_buffer_t *const key, fcs_dbm_record_t *const parent,
    const unsigned char move GCC_UNUSED,
    const fcs_which_moves_bitmask_t *const which_irreversible_moves_bitmask
#ifdef FCS_DBM_CACHE_ONLY
    ,
    const fcs_fcc_move_t *moves_to_parent
#endif
    )
{
#ifdef DEBUG_OUT
    fcs_state_locs_struct_t locs;
    fc_solve_init_locs(&locs);
    fcs_dbm_variant_type_t local_variant = instance->common.variant;
#endif
    const_AUTO(coll, &(instance->coll));
    {
#ifdef FCS_DBM_WITHOUT_CACHES
        fcs_dbm_record_t *token;
#else
        fcs_dbm_record_t *token = key;
#endif
#ifndef FCS_DBM_WITHOUT_CACHES
        if (cache_does_key_exist(&(coll->cache_store.cache), key))
        {
            return;
        }
#ifndef FCS_DBM_CACHE_ONLY
        else if (pre_cache_does_key_exist(&(coll->cache_store.pre_cache), key))
        {
            return;
        }
#endif
#ifndef FCS_DBM_CACHE_ONLY
        else if (fc_solve_dbm_store_does_key_exist(
                     coll->cache_store.store, key->s))
        {
            cache_insert(&(coll->cache_store.cache), key, NULL, '\0');
            return;
        }
#endif
        else
#else
        if ((token = fc_solve_dbm_store_insert_key_value(
                 coll->cache_store.store, key, parent, TRUE)))
#endif
        {
#ifdef FCS_DBM_CACHE_ONLY
            fcs_cache_key_info_t *cache_key;
#endif

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
            pre_cache_insert(&(coll->cache_store.pre_cache), key, parent);
#else
            cache_key = cache_insert(
                &(coll->cache_store.cache), key, moves_to_parent, move);
#endif
#endif

            if (key_depth == instance->curr_depth)
            {
                /* Now insert it into the queue. */

                FCS_LOCK(coll->queue_lock);
                fcs_depth_multi_queue__insert(
                    &(coll->depth_queue), thread->state_depth + 1,
#ifdef FCS_DBM_WITHOUT_CACHES
                    (const fcs_offloading_queue_item_t *)(&token)
#else
                    key
#endif
                        );
                FCS_UNLOCK(coll->queue_lock);

                FCS_LOCK(instance->global_lock);

                instance->common.count_of_items_in_queue++;
                instance->common.num_states_in_collection++;

                instance_debug_out_state(instance, &(token->key));

                FCS_UNLOCK(instance->global_lock);
            }
            else
            {
                /* Handle an irreversible move */

                /* Calculate the new fingerprint to which the exit
                 * point belongs. */
                fcs_which_moves_bitmask_t new_fingerprint = {{'\0'}};
                for (size_t i = 0; i < COUNT(new_fingerprint.s); i++)
                {
                    new_fingerprint.s[i] =
                        which_irreversible_moves_bitmask->s[i] +
                        instance->fingerprint_which_irreversible_moves_bitmask
                            .s[i];
                }
                int trace_num;
                fcs_encoded_state_buffer_t *trace;
                FCS_LOCK(instance->fcc_exit_points_output_lock);
                /* instance->storage_lock is already locked
                 * in instance_check_multiple_keys and we should not
                 * lock it here. */
                calc_trace(token, &trace, &trace_num);
                {
                    FccEntryPointNode fcc_entry_key;
                    fcc_entry_key.kv.key.key = trace[trace_num - 1];
                    FccEntryPointNode *val_proto = RB_FIND(FccEntryPointList,
                        &(instance->fcc_entry_points), &fcc_entry_key);
                    const long location_in_file =
                        val_proto->kv.val.location_in_file;
                    fseek(instance->fingerprint_fh, location_in_file, SEEK_SET);

                    getline(&(instance->fingerprint_line),
                        &(instance->fingerprint_line_size),
                        instance->fingerprint_fh);
                    char *const moves_to_state_enc =
                        strchr(
                            strchr(instance->fingerprint_line, ' ') + 1, ' ') +
                        1;
                    char *const trailing_newline =
                        strchr(moves_to_state_enc, '\n');
                    if (trailing_newline)
                    {
                        *trailing_newline = '\0';
                    }
                    const size_t string_len = strlen(moves_to_state_enc);
                    instance_alloc_num_moves(
                        instance, ((string_len * 3) >> 2) + 20);
                    base64_decode(moves_to_state_enc, string_len,
                        ((unsigned char *)instance->moves_to_state),
                        &(instance->moves_to_state_len));
                }

                const_SLOT(moves_to_state_len, instance);
                const size_t added_moves_to_output =
                    moves_to_state_len + trace_num - 1;
                instance_alloc_num_moves(instance, added_moves_to_output);
                unsigned char *const moves_to_state = instance->moves_to_state;
                for (int i = trace_num - 1; i > 0; i--)
                {
                    moves_to_state[moves_to_state_len + trace_num - 1 - i] =
                        get_move_from_parent_to_child(instance,
                            &(thread->delta_stater), trace[i], trace[i - 1]);
                }

                const size_t new_max_enc_len =
                    ((added_moves_to_output * 4) / 3) + 20;

                if (new_max_enc_len >
                    instance->moves_base64_encoding_buffer_max_len)
                {
                    instance->moves_base64_encoding_buffer =
                        SREALLOC(instance->moves_base64_encoding_buffer,
                            new_max_enc_len);
                    instance->moves_base64_encoding_buffer_max_len =
                        new_max_enc_len;
                }

                size_t unused_output_len;
                base64_encode(moves_to_state, added_moves_to_output,
                    instance->moves_base64_encoding_buffer, &unused_output_len);
                char fingerprint_base64[100];
                char state_base64[100];
                base64_encode(new_fingerprint.s, sizeof(new_fingerprint),
                    fingerprint_base64, &unused_output_len);
                base64_encode((unsigned char *)&(*key), sizeof(*key),
                    state_base64, &unused_output_len);
                /* Output the exit point. */
                fprintf(instance->fcc_exit_points_out_fh, "%s %s %zd %s\n",
                    fingerprint_base64, state_base64, added_moves_to_output,
                    instance->moves_base64_encoding_buffer);
#ifdef DEBUG_OUT
                {
                    fcs_state_keyval_pair_t state;
                    DECLARE_IND_BUF_T(indirect_stacks_buffer)
                    fc_solve_delta_stater_decode_into_state(
                        &(thread->delta_stater), key->s, &state,
                        indirect_stacks_buffer);
                    char state_str[2000];
                    FCS__RENDER_STATE(state_str, &(state.s), &locs);
                    fprintf(stderr,
                        "Check Key: <<<\n%s\n>>>\n\n[%s %s %ld %s]\n\n",
                        state_str, fingerprint_base64, state_base64,
                        added_moves_to_output,
                        instance->moves_base64_encoding_buffer);
                }
#endif
                fflush(instance->fcc_exit_points_out_fh);

                FCS_UNLOCK(instance->fcc_exit_points_output_lock);

                free(trace);
            }
        }
    }
}

static void instance_run_all_threads(fcs_dbm_solver_instance_t *instance,
    fcs_state_keyval_pair_t *init_state, FccEntryPointNode *key_ptr,
    size_t num_threads)
{
    const_AUTO(threads,
        dbm__calc_threads(instance, init_state, num_threads, init_thread));
    /* TODO : do something meaningful with start_key_ptr . */
    instance->start_key_ptr = key_ptr;

#if 0
    instance->was_start_key_reachable = (key_ptr->kv.val.is_reachable);
    instance->start_key_moves_count = (key_ptr->kv.val.depth);
#endif

    {
        dbm__spawn_threads(instance, num_threads, threads);
        if (!instance->common.queue_solution_was_found)
        {
            mark_and_sweep_old_states(instance,
                fc_solve_dbm_store_get_dict(instance->coll.cache_store.store),
                instance->curr_depth);
        }
    }
    dbm__free_threads(instance, num_threads, threads, free_thread);
}

#define TRY_P(s) try_argv_param(argc, argv, &arg, s)
int main(int argc, char *argv[])
{
    build_decoding_table();
#ifdef DEBUG_OUT
    fcs_state_locs_struct_t locs;
    fc_solve_init_locs(&locs);
#endif
    const char *mod_base64_fcc_fingerprint = NULL;
    const char *fingerprint_input_location_path = NULL;
    const char *path_to_output_dir = NULL;
    const char *filename = NULL, *offload_dir_path = NULL;
    fcs_dbm_variant_type_t local_variant = FCS_DBM_VARIANT_2FC_FREECELL;
    DECLARE_IND_BUF_T(init_indirect_stacks_buffer)
    long pre_cache_max_count = 1000000, caches_delta = 1000000,
         iters_delta_limit = -1;
    const char *dbm_store_path = "./fc_solve_dbm_store";
    const char *param;

    size_t num_threads = 1;
    int arg;
    for (arg = 1; arg < argc; arg++)
    {
        if (fcs_dbm__extract_common_from_argv(argc, argv, &arg, &local_variant,
                &offload_dir_path, &num_threads, &pre_cache_max_count,
                &iters_delta_limit, &caches_delta, &dbm_store_path))
        {
        }
        else if ((param = TRY_P("--board")))
        {
            filename = param;
        }
        else if ((param = TRY_P("--output")))
        {
            path_to_output_dir = param;
        }
        else if ((param = TRY_P("--fingerprint")))
        {
            mod_base64_fcc_fingerprint = param;
        }
        else if ((param = TRY_P("--input")))
        {
            fingerprint_input_location_path = param;
        }
        else
        {
            break;
        }
    }

    if (arg != argc)
    {
        fc_solve_err("%s\n", "Junk at the end of the parameters");
    }

    if (!(filename && fingerprint_input_location_path &&
            mod_base64_fcc_fingerprint && path_to_output_dir &&
            offload_dir_path))
    {
        fc_solve_err("One or more of these parameters was not specified: %s\n",
            "--board, --fingerprint, --input, --output, --offload-dir-path");
    }
    fcs_state_keyval_pair_t init_state;
    read_state_from_file(local_variant, filename,
        &init_state PASS_IND_BUF_T(init_indirect_stacks_buffer));
    horne_prune__simple(local_variant, &init_state);
    fcs_which_moves_bitmask_t fingerprint_which_irreversible_moves_bitmask = {
        {'\0'}};
    {
        size_t fingerprint_data_len = 0;
        base64_decode(mod_base64_fcc_fingerprint,
            strlen(mod_base64_fcc_fingerprint),
            fingerprint_which_irreversible_moves_bitmask.s,
            &fingerprint_data_len);

        if (fingerprint_data_len !=
            sizeof(fingerprint_which_irreversible_moves_bitmask))
        {
            fc_solve_err("%s\n", "--fingerprint is invalid length.");
        }
    }

    /* Calculate the fingerprint_which_irreversible_moves_bitmask's curr_depth.
     */

    fc_solve_delta_stater_t delta;
    fc_solve_delta_stater_init(&delta, &init_state.s, STACKS_NUM, FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
        ,
        CALC_SEQUENCES_ARE_BUILT_BY()
#endif
            );

    {
        fcs_dbm_solver_instance_t instance;
        FccEntryPointNode *key_ptr = NULL;
        fcs_encoded_state_buffer_t parent_state_enc;

        instance_init(&instance, local_variant, pre_cache_max_count,
            caches_delta, dbm_store_path, iters_delta_limit, offload_dir_path,
            &fingerprint_which_irreversible_moves_bitmask, stdout);

        FILE *fingerprint_fh = fopen(fingerprint_input_location_path, "rt");

        if (!fingerprint_fh)
        {
            fc_solve_err("Cannot open '%s' for reading. Exiting.",
                fingerprint_input_location_path);
        }

        instance.fingerprint_line = NULL;
        instance.fingerprint_line_size = 0;

        long location_in_file = 0;
        fcs_bool_t was_init = FALSE;
        while (getline(&instance.fingerprint_line,
                   &instance.fingerprint_line_size, fingerprint_fh) != -1)
        {
            FccEntryPointNode *const entry_point = fcs_compact_alloc_ptr(
                &(instance.fcc_entry_points_allocator), sizeof(*entry_point));
            fcs_dbm_record_set_parent_ptr(&(entry_point->kv.key), NULL);
            char state_base64[100];
            int state_depth;
            sscanf(instance.fingerprint_line, "%99s %d", state_base64,
                &state_depth);
            size_t unused_size;
            base64_decode(state_base64, strlen(state_base64),
                ((unsigned char *)&(entry_point->kv.key.key)), &(unused_size));

            assert(unused_size == sizeof(entry_point->kv.key.key));

            /* TODO : Should traverse starting from key. */
            key_ptr = entry_point;
            /* The NULL parent_state_enc and move for indicating this is the
             * initial state. */
            fcs_init_encoded_state(&(parent_state_enc));

/*
 * These states should be traversed - not blocked, so don't
 * put them inside the store.
 * */
#if 0
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
            pre_cache_insert(&(instance.pre_cache), &(key_ptr->kv.key.key), &parent_state_enc);
#else
            cache_insert(&(instance.cache), &(key_ptr->kv.key.key), NULL, '\0');
#endif
#else
            token = fc_solve_dbm_store_insert_key_value(instance.coll.store, &(key_ptr->kv.key.key), NULL, TRUE);
#endif

#if 0
            entry_point->kv.val.was_consumed =
                entry_point->kv.val.is_reachable =
                FALSE;
#endif
#endif
            entry_point->kv.val.location_in_file = location_in_file;

            RB_INSERT(
                FccEntryPointList, &(instance.fcc_entry_points), entry_point);

            {
                const fcs_offloading_queue_item_t token = ((
                    const fcs_offloading_queue_item_t)(&(entry_point->kv.key)));
                if (was_init)
                {
                    fcs_depth_multi_queue__insert(
                        &(instance.coll.depth_queue), state_depth, &(token));
                }
                else
                {
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
                    fcs_depth_multi_queue__init(&(instance.coll.depth_queue),
                        instance.offload_dir_path, state_depth, &(token));
#else
                    fc_solve_meta_compact_allocator_init(
                        &(instance.coll.queue_meta_alloc));
                    fcs_offloading_queue__init(
                        &(instance.coll.cache_store.queue),
                        &(instance.coll.queue_meta_alloc));
                    fcs_depth_multi_queue__insert(
                        &(instance.coll.queue), state_depth, &(token));
#endif

                    was_init = TRUE;
                }
            }
            instance.common.num_states_in_collection++;
            instance.common.count_of_items_in_queue++;
            /* Valid for the next iteration: */
            location_in_file = ftell(fingerprint_fh);
        }

        fseek(fingerprint_fh, 0, SEEK_SET);
        instance.fingerprint_fh = fingerprint_fh;

        long count_of_instance_runs = 0;
        instance.moves_to_state = NULL;
        instance.max_moves_to_state_len = instance.moves_to_state_len = 0;

        count_of_instance_runs++;

        char fcc_exit_points_out_fn[PATH_MAX + 1],
            fcc_exit_points_out_fn_temp[PATH_MAX + 1];
        sprintf(fcc_exit_points_out_fn, "%s/exits.%ld", path_to_output_dir,
            count_of_instance_runs);
        sprintf(fcc_exit_points_out_fn_temp, "%s.temp", fcc_exit_points_out_fn);
        instance.fcc_exit_points_out_fh =
            fopen(fcc_exit_points_out_fn_temp, "wt");

        instance_run_all_threads(
            &instance, &init_state, key_ptr, NUM_THREADS());
        fclose(instance.fcc_exit_points_out_fh);
        rename(fcc_exit_points_out_fn_temp, fcc_exit_points_out_fn);
        fclose(fingerprint_fh);
        handle_and_destroy_instance_solution(&instance, &delta);
        free(instance.moves_to_state);
        free(instance.moves_base64_encoding_buffer);
        free(instance.fingerprint_line);
    }

    fc_solve_delta_stater_release(&delta);

    return 0;
}
#ifdef FCS_KAZ_TREE_USE_RECORD_DICT_KEY
#error "FCS_KAZ_TREE_USE_RECORD_DICT_KEY was defined and this is incompatible"
#endif
