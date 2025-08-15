// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2012 Shlomi Fish
// split_fcc_solver.c - a specialised solver that solves one FCC (=
// fully connected component) at a time - meant to try to
// reduce the memory consumption. See
// ../docs/split-fully-connected-components-based-solver-planning.txt
// in the Freecell Solver git repository.
#ifdef FCS_DBM__VAL_IS_ANCESTOR
#error FCS_DBM__VAL_IS_ANCESTOR must not be defined
#endif

#include "dbm_solver_head.h"
#include <sys/tree.h>
#include "depth_multi_queue.h"
#include "rinutils/portable_time.h"

typedef struct
{
    fcs_dbm__cache_store__common cache_store;
    meta_allocator queue_meta_alloc;
    fcs_depth_multi_queue depth_queue;
} fcs_dbm_collection_by_depth;

typedef struct FccEntryPointList FccEntryPointList;
typedef struct FccEntryPointNode FccEntryPointNode;

struct FccEntryPointNode
{
    RB_ENTRY(FccEntryPointNode) entry_;
    struct
    {
        fcs_dbm_record key;
        struct
        {
            long location_in_file;
#if 0
            int depth;
            int was_consumed: 1;
            int is_reachable: 1;
#endif
        } val;
    } kv;
};

static inline int FccEntryPointNode_compare(
    FccEntryPointNode *a, FccEntryPointNode *b)
{
#define MAP(x) &((x)->kv.key.key)
    return compare_enc_states(MAP(a), MAP(b));
}

RB_HEAD(FccEntryPointList, FccEntryPointNode);
static const FccEntryPointList FccEntryPointList_init =
    RB_INITIALIZER(&(instance->fcc_entry_points));

typedef struct
{
    fcs_dbm_collection_by_depth coll;
    fcs_lock global_lock;
    const char *offload_dir_path;
    size_t curr_depth;
    dbm_instance_common_elems common;
    meta_allocator fcc_meta_alloc;
    FccEntryPointList fcc_entry_points;
    compact_allocator fcc_entry_points_allocator;
    fcs_lock fcc_entry_points_lock;
    FccEntryPointNode *start_key_ptr;
#if 0
    bool was_start_key_reachable;
#endif
    int start_key_moves_count;
    fcs_lock output_lock;
#if 0
    FILE * consumed_states_fh;
#endif
    fcs_which_moves_bitmask fingerprint_which_irreversible_moves_bitmask;
    fcs_lock fcc_exit_points_output_lock;
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
} dbm_solver_instance;

#define CHECK_KEY_CALC_DEPTH()                                                 \
    (instance->curr_depth + list->num_non_reversible_moves_including_prune)

#include "dbm_procs.h"
#include "fcs_base64.h"
RB_GENERATE_STATIC(
    FccEntryPointList, FccEntryPointNode, entry_, FccEntryPointNode_compare)

static inline void instance_init(dbm_solver_instance *const instance,
    const fcs_dbm_common_input *const inp,
    fcs_which_moves_bitmask *fingerprint_which_irreversible_moves_bitmask,
    FILE *const out_fh)
{
    fc_solve_meta_compact_allocator_init(&(instance->fcc_meta_alloc));
    instance->fcc_entry_points = FccEntryPointList_init;

    fc_solve_compact_allocator_init(
        &(instance->fcc_entry_points_allocator), &(instance->fcc_meta_alloc));

    instance->start_key_ptr = NULL;
    instance->dbm_store_path = inp->dbm_store_path;
    instance->fingerprint_which_irreversible_moves_bitmask =
        (*fingerprint_which_irreversible_moves_bitmask);

    instance->moves_base64_encoding_buffer = NULL;
    instance->moves_base64_encoding_buffer_max_len = 0;
    {
        size_t curr_depth = 0;
        {
            for (size_t i = 0;
                i < COUNT(fingerprint_which_irreversible_moves_bitmask->s); ++i)
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

    fcs_lock_init(&instance->global_lock);
    instance->offload_dir_path = inp->offload_dir_path;
    fcs_dbm__common_init(&(instance->common), inp->iters_delta_limit,
        inp->max_num_states_in_collection, inp->local_variant, out_fh);

    fcs_lock_init(&instance->output_lock);
    fcs_lock_init(&instance->fcc_entry_points_lock);
    fcs_lock_init(&instance->fcc_exit_points_output_lock);
    {
        fcs_dbm_collection_by_depth *const coll = &(instance->coll);

        fcs_dbm__cache_store__init(&(coll->cache_store), &(instance->common),
            &(coll->queue_meta_alloc), inp->dbm_store_path,
            inp->pre_cache_max_count, inp->caches_delta);
    }
}

static inline void instance_destroy(dbm_solver_instance *const instance)
{
    fc_solve_compact_allocator_finish(&(instance->fcc_entry_points_allocator));
    fc_solve_meta_compact_allocator_finish(&(instance->fcc_meta_alloc));
    fcs_dbm_collection_by_depth *coll = &(instance->coll);
    fcs_depth_multi_queue__destroy(&(coll->depth_queue));
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
    fc_solve_meta_compact_allocator_finish(&(coll->queue_meta_alloc));
#endif
    DESTROY_CACHE(coll);
    fcs_lock_destroy(&instance->common.storage_lock);
    fcs_lock_destroy(&instance->global_lock);
    fcs_lock_destroy(&instance->fcc_entry_points_lock);
    fcs_lock_destroy(&instance->fcc_exit_points_output_lock);
    fcs_lock_destroy(&instance->output_lock);
}

struct fcs_dbm_solver_thread_struct
{
    dbm_solver_instance *instance;
    fcs_delta_stater delta_stater;
    meta_allocator thread_meta_alloc;
    int state_depth;
};

static void *instance_run_solver_thread(void *const void_arg)
{
    fcs_dbm_queue_item physical_item;
    fcs_dbm_record *token = NULL;
    fcs_derived_state *derived_list = NULL, *derived_list_recycle_bin = NULL;
    compact_allocator derived_list_allocator;
    fcs_state_keyval_pair state;
#ifdef WITH__base64_encoding_buffer
    char *base64_encoding_buffer = NULL;
    size_t base64_encoding_buffer_max_len = 0;
#endif

#ifdef DEBUG_OUT
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);
#endif
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    const_AUTO(thread, ((thread_arg *)void_arg)->thread);
    const_SLOT(instance, thread);
    const_AUTO(delta_stater, &(thread->delta_stater));
    const_AUTO(local_variant, instance->common.variant);

    fcs_dbm_queue_item *item = NULL, *prev_item = NULL;
    unsigned long queue_num_extracted_and_processed = 0;
    fc_solve_compact_allocator_init(
        &(derived_list_allocator), &(thread->thread_meta_alloc));
    FILE *const out_fh = instance->common.out_fh;
    TRACE("%s\n", "instance_run_solver_thread start");
    const_AUTO(coll, &(instance->coll));
#if 0
    bool was_start_key_reachable = instance->was_start_key_reachable;
#endif
    while (1)
    {
        // First of all extract an item.
        fcs_lock_lock(&instance->global_lock);

        if (prev_item)
        {
            --instance->common.queue_num_extracted_and_processed;
        }

        if (instance->common.should_terminate == DONT_TERMINATE)
        {
            if (fcs_depth_multi_queue__extract(&(coll->depth_queue),
                    &(thread->state_depth), &(token->key)))
            {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
                physical_item.key = token->key;
#pragma GCC diagnostic pop
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
        fcs_lock_unlock(&instance->global_lock);

        if ((instance->common.should_terminate != DONT_TERMINATE) ||
            (!queue_num_extracted_and_processed))
        {
            break;
        }

        if (!item)
        {
            // Sleep until more items become available in the queue.
            const struct timespec ts = {.tv_sec = 0, .tv_nsec = 5000000};
            struct timespec no_use;
            nanosleep(&ts, &no_use);
        }
        else
        {
            // Handle item.
            fc_solve_delta_stater_decode_into_state(
                delta_stater, item->key.s, &state, indirect_stacks_buffer);

            // A section for debugging.
            FCS__OUTPUT_STATE(out_fh, "", &(state.s), &locs);
#if 0
        {
            FccEntryPointNode key;
            key.kv.key.key = item->key;
            fcs_lock_lock(&instance->fcc_entry_points_lock);
            FccEntryPointNode * val_proto = RB_FIND(
                FccEntryPointList,
                &(instance->fcc_entry_points),
                &(key)
            );

            bool to_prune = false;
            bool to_output = false;
            if (val_proto)
            {
                val_proto->kv.val.is_reachable = true;
                const int moves_count = instance->start_key_moves_count
                    + item->moves_seq.count;

                if (was_start_key_reachable)
                {
                    if (val_proto->kv.val.depth <= moves_count)
                    {
                        to_prune = true;
                    }
                    else
                    {
                        // We can set it to the more pessimstic move count for future trimming.
                        to_output = !(val_proto->kv.val.was_consumed);
                        val_proto->kv.val.depth = moves_count;
                        val_proto->kv.val.was_consumed = true;
                    }
                }
                else
                {
                    if (! val_proto->kv.val.was_consumed)
                    {
                        to_output = true;
                        if (val_proto->kv.val.depth >= moves_count)
                        {
                            val_proto->kv.val.depth = moves_count;
                            val_proto->kv.val.was_consumed = true;
                        }
                    }
                }
            }
            fcs_lock_unlock(&instance->fcc_entry_points_lock);

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

                fcs_lock_lock(&instance->output_lock);
                fprintf(
                    instance->consumed_states_fh,
                    "%s\n", base64_encoding_buffer
                );
                fflush(instance->consumed_states_fh);
                fcs_lock_unlock(&instance->output_lock);
            }

            if (to_prune)
            {
                continue;
            }
        }
#endif

            if (instance_solver_thread_calc_derived_states(local_variant,
                    &state, token->key, &derived_list,
                    &derived_list_recycle_bin, &derived_list_allocator, true))
            {
                fcs_lock_lock(&instance->global_lock);
                fcs_dbm__found_solution(&(instance->common), token);
                fcs_lock_unlock(&instance->global_lock);
                break;
            }

            // Encode all the states.
            for (var_AUTO(derived_iter, derived_list); derived_iter;
                derived_iter = derived_iter->next)
            {
                fcs_init_and_encode_state(delta_stater, local_variant,
                    &(derived_iter->state),
                    &(derived_iter->key_and_parent.key));
            }

            instance_check_multiple_keys(thread, instance, &(coll->cache_store),
                &(coll->queue_meta_alloc), &derived_list, 1);

            fcs_derived_state_list__recycle(
                &derived_list_recycle_bin, &derived_list);
            derived_list = NULL;
            // End handle item.
        }
        // End of main thread loop
        prev_item = item;
    }

#ifdef WITH__base64_encoding_buffer
    free(base64_encoding_buffer);
#endif
    fc_solve_compact_allocator_finish(&(derived_list_allocator));
    TRACE("%s\n", "instance_run_solver_thread end");
    return NULL;
}

#include "depth_dbm_procs.h"

static inline void instance_alloc_num_moves(
    dbm_solver_instance *const instance, const size_t buffer_size)
{
    if (buffer_size > instance->max_moves_to_state_len)
    {
        instance->moves_to_state =
            SREALLOC(instance->moves_to_state, buffer_size);
        instance->max_moves_to_state_len = buffer_size;
    }
}

static inline void instance_check_key(dbm_solver_thread *const thread,
    dbm_solver_instance *const instance, const size_t key_depth,
    fcs_encoded_state_buffer *const key,
    const fcs_encoded_state_buffer raw_parent,

    const unsigned char move GCC_UNUSED,
    const fcs_which_moves_bitmask *const which_irreversible_moves_bitmask)
{
#ifdef DEBUG_OUT
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);
    const_AUTO(local_variant, instance->common.variant);
#endif
    const_AUTO(coll, &(instance->coll));
    fcs_dbm_record *token;
    if ((token = cache_store__has_key(&coll->cache_store, key, raw_parent)))
    {
#if 0
        printf("key_depth = %zu ; curr_depth = %zu\n", key_depth, instance->curr_depth);
#endif
        if (key_depth == instance->curr_depth)
        {
            // Now insert it into the queue.
            fcs_lock_lock(&instance->global_lock);

            fcs_depth_multi_queue__insert(
                &(coll->depth_queue), thread->state_depth + 1, (&token->key));

            ++instance->common.count_of_items_in_queue;
            ++instance->common.num_states_in_collection;

            instance_debug_out_state(instance, &(token->key));

            fcs_lock_unlock(&instance->global_lock);
        }
        else
        {
            // Handle an irreversible move

            // Calculate the new fingerprint to which the exit point belongs.
            fcs_which_moves_bitmask new_fingerprint = {{'\0'}};
            for (size_t i = 0; i < COUNT(new_fingerprint.s); ++i)
            {
                new_fingerprint.s[i] =
                    which_irreversible_moves_bitmask->s[i] +
                    instance->fingerprint_which_irreversible_moves_bitmask.s[i];
            }
            fcs_lock_lock(&instance->fcc_exit_points_output_lock);
            // instance->storage_lock is already locked in
            // instance_check_multiple_keys and we should not lock it here.
#if 0
            size_t trace_num;
            fcs_encoded_state_buffer *trace;
            calc_trace(token, &trace, &trace_num);
#endif
            {
#if 0
            size_t trace_num;
                FccEntryPointNode fcc_entry_key;
                fcc_entry_key.kv.key.key = trace[trace_num - 1];
                FccEntryPointNode *val_proto = RB_FIND(FccEntryPointList,
                if (!val_proto)
                {
                    goto cleanup;
                }
                const long location_in_file =
                    val_proto->kv.val.location_in_file;
                fseek(instance->fingerprint_fh, location_in_file, SEEK_SET);

#ifdef HAVE_GETLINE
                if (getline(&(instance->fingerprint_line),
                        &(instance->fingerprint_line_size),
                        instance->fingerprint_fh) <= 0)
                {
                    // https://github.com/shlomif/fc-solve/issues/83
                    fcs_lock_unlock(&instance->fcc_exit_points_output_lock);
                    abort();
                }
#else
                fgets(instance->fingerprint_line,
                    (int)instance->fingerprint_line_size,
                    instance->fingerprint_fh);
#endif
                char *const moves_to_state_enc =
                    strchr(strchr(instance->fingerprint_line, ' ') + 1, ' ') +
                    1;
                char *const trailing_newline = strchr(moves_to_state_enc, '\n');
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
#endif
            }

            const fcs_encoded_state_buffer outkey = token->key;
            const size_t keysize = sizeof(outkey);
            const_SLOT(moves_to_state_len, instance);
            const size_t added_moves_to_output = moves_to_state_len + keysize;
            instance_alloc_num_moves(instance, added_moves_to_output);
            unsigned char *const moves_to_state = instance->moves_to_state;
            memcpy(&moves_to_state[moves_to_state_len], &outkey, keysize);

            const size_t new_max_enc_len =
                ((added_moves_to_output * 4) / 3) + 20;

            if (new_max_enc_len >
                instance->moves_base64_encoding_buffer_max_len)
            {
                instance->moves_base64_encoding_buffer = SREALLOC(
                    instance->moves_base64_encoding_buffer, new_max_enc_len);
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
            base64_encode((unsigned char *)&(*key), sizeof(*key), state_base64,
                &unused_output_len);
            // Output the exit point.
            fprintf(instance->fcc_exit_points_out_fh,
                "%s %s " RIN_LL_FMT " %s\n", fingerprint_base64, state_base64,
                (long long)added_moves_to_output,
                instance->moves_base64_encoding_buffer);
#ifdef DEBUG_OUT
            {
                fcs_state_keyval_pair state;
                DECLARE_IND_BUF_T(indirect_stacks_buffer)
                fc_solve_delta_stater_decode_into_state(&(thread->delta_stater),
                    key->s, &state, indirect_stacks_buffer);
                fcs_render_state_str state_str;
                FCS__RENDER_STATE(state_str, &(state.s), &locs);
                fprintf(stderr, "Check Key: <<<\n%s\n>>>\n\n[%s %s %ld %s]\n\n",
                    state_str, fingerprint_base64, state_base64,
                    added_moves_to_output,
                    instance->moves_base64_encoding_buffer);
            }
#endif
            fflush(instance->fcc_exit_points_out_fh);
#if 0
        cleanup:
#endif
            fcs_lock_unlock(&instance->fcc_exit_points_output_lock);
        }
    }
}

static void instance_run_all_threads(dbm_solver_instance *const instance,
    fcs_state_keyval_pair *const init_state, FccEntryPointNode *key_ptr,
    const size_t num_threads)
{
    const_AUTO(threads,
        dbm__calc_threads(instance, init_state, num_threads, init_thread));
    // TODO : do something meaningful with start_key_ptr .
    instance->start_key_ptr = key_ptr;

#if 0
    instance->was_start_key_reachable = (key_ptr->kv.val.is_reachable);
    instance->start_key_moves_count = (key_ptr->kv.val.depth);
#endif

    dbm__spawn_threads(instance, num_threads, threads);
#if 0
    if (!instance->common.queue_solution_was_found)
    {
        mark_and_sweep_old_states(instance,
            fc_solve_dbm_store_get_dict(instance->coll.cache_store.store),
            instance->curr_depth);
    }
#endif
    dbm__free_threads(instance, num_threads, threads, free_thread);
}

int main(int argc, char *argv[])
{
    apr_initialize();
    build_decoding_table();
#ifdef DEBUG_OUT
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);
#endif
    const char *mod_base64_fcc_fingerprint = NULL;
    const char *fingerprint_input_location_path = NULL;
    const char *path_to_output_dir = NULL;
    const char *filename = NULL;
    DECLARE_IND_BUF_T(init_indirect_stacks_buffer)
    const char *param;
    fcs_dbm_common_input inp = fcs_dbm_common_input_init;

    int arg;
    for (arg = 1; arg < argc; ++arg)
    {
        if (fcs_dbm__extract_common_from_argv(argc, argv, &arg, &inp))
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
        exit_error("%s\n", "Junk at the end of the parameters");
    }

    if (!(filename && fingerprint_input_location_path &&
            mod_base64_fcc_fingerprint && path_to_output_dir &&
            inp.offload_dir_path))
    {
        exit_error("One or more of these parameters was not specified: %s\n",
            "--board, --fingerprint, --input, --output, --offload-dir-path");
    }
    fcs_state_keyval_pair init_state;
    read_state_from_file(inp.local_variant, filename,
        &init_state PASS_IND_BUF_T(init_indirect_stacks_buffer));
    horne_prune__simple(inp.local_variant, &init_state);
    fcs_which_moves_bitmask fingerprint_which_irreversible_moves_bitmask = {
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
            exit_error("%s\n", "--fingerprint is invalid length.");
        }
    }

    const_AUTO(local_variant, inp.local_variant);
#ifndef FCS_DBM_SINGLE_THREAD
    const_AUTO(num_threads, inp.num_threads);
#endif
    // Calculate the fingerprint_which_irreversible_moves_bitmask's curr_depth.
    fcs_delta_stater delta;
    fc_solve_delta_stater_init(&delta, local_variant, &init_state.s, STACKS_NUM,
        FREECELLS_NUM PASS_ON_NOT_FC_ONLY(CALC_SEQUENCES_ARE_BUILT_BY()));

    dbm_solver_instance instance;
    FccEntryPointNode *key_ptr = NULL;
    fcs_encoded_state_buffer parent_state_enc;

    instance_init(
        &instance, &inp, &fingerprint_which_irreversible_moves_bitmask, stdout);

    FILE *fingerprint_fh = fopen(fingerprint_input_location_path, "rt");
    if (!fingerprint_fh)
    {
        exit_error("Cannot open '%s' for reading. Exiting.",
            fingerprint_input_location_path);
    }

#ifdef HAVE_GETLINE
    instance.fingerprint_line = NULL;
    instance.fingerprint_line_size = 0;
#else
    instance.fingerprint_line_size = 16000;
    instance.fingerprint_line =
        SMALLOC(instance.fingerprint_line, instance.fingerprint_line_size);
#endif

    long location_in_file = 0;
    bool was_init = false;
#ifdef HAVE_GETLINE
    while (getline(&instance.fingerprint_line, &instance.fingerprint_line_size,
               fingerprint_fh) != -1)
#else
    while (fgets(instance.fingerprint_line,
        (int)instance.fingerprint_line_size - 1, fingerprint_fh))
#endif
    {
        FccEntryPointNode *const entry_point = fcs_compact_alloc_ptr(
            &(instance.fcc_entry_points_allocator), sizeof(*entry_point));
        entry_point->kv.key.parent = entry_point->kv.key.parent;

        char state_base64[100] = {0};
        int state_depth = -1;
        assert(2 == sscanf(instance.fingerprint_line, "%99s %d", state_base64,
                        &state_depth));
        if (state_depth < 0)
        {
            exit_error("Wrongly input state_depth!\n");
        }
        size_t unused_size;
        base64_decode(state_base64, strlen(state_base64),
            ((unsigned char *)&(entry_point->kv.key.key)), &(unused_size));

        assert(unused_size == sizeof(entry_point->kv.key.key));

        // TODO : Should traverse starting from key.
        key_ptr = entry_point;
        // The NULL parent_state_enc and move for indicating this is the initial
        // state.
        fcs_init_encoded_state(&(parent_state_enc));

// These states should be traversed - not blocked, so don't put them inside the
// store.
#if 0
        fcs_encoded_state_buffer null_parent;
        fcs_init_encoded_state(&null_parent);
        token = fc_solve_dbm_store_insert_key_value(instance.coll.store, &(key_ptr->kv.key.key), &null_parent);

#if 0
        entry_point->kv.val.was_consumed =
            entry_point->kv.val.is_reachable =
            false;
#endif
#endif
        entry_point->kv.val.location_in_file = location_in_file;

        RB_INSERT(FccEntryPointList, &(instance.fcc_entry_points), entry_point);

        {
            const offloading_queue_item token = (entry_point->kv.key.key);
            if (was_init)
            {
                fcs_depth_multi_queue__insert(
                    &(instance.coll.depth_queue), state_depth, &(token));
            }
            else
            {
                fcs_depth_multi_queue__init(&(instance.coll.depth_queue),
                    instance.offload_dir_path, state_depth, &(token));
                was_init = true;
            }
        }
        ++instance.common.num_states_in_collection;
        ++instance.common.count_of_items_in_queue;
        // Valid for the next iteration:
        location_in_file = ftell(fingerprint_fh);
    }

    fseek(fingerprint_fh, 0, SEEK_SET);
    instance.fingerprint_fh = fingerprint_fh;

    long count_of_instance_runs = 0;
    instance.moves_to_state = NULL;
    instance.max_moves_to_state_len = instance.moves_to_state_len = 0;

    ++count_of_instance_runs;

    char fcc_exit_points_out_fn[PATH_MAX - 40],
        fcc_exit_points_out_fn_temp[PATH_MAX + 1];
    snprintf(fcc_exit_points_out_fn, COUNT(fcc_exit_points_out_fn),
        "%s/exits.%ld", path_to_output_dir, count_of_instance_runs);
    snprintf(fcc_exit_points_out_fn_temp, PATH_MAX, "%s.temp",
        fcc_exit_points_out_fn);
    instance.fcc_exit_points_out_fh = fopen(fcc_exit_points_out_fn_temp, "wt");

    instance_run_all_threads(&instance, &init_state, key_ptr, NUM_THREADS());
    fclose(instance.fcc_exit_points_out_fh);
    rename(fcc_exit_points_out_fn_temp, fcc_exit_points_out_fn);
    fclose(fingerprint_fh);
    trace_solution(0, NULL, NULL, NULL, NULL);
    handle_and_destroy_instance_solution(&instance, &delta);
    free(instance.moves_to_state);
    free(instance.moves_base64_encoding_buffer);
    free(instance.fingerprint_line);
    fc_solve_delta_stater_release(&delta);
    apr_terminate();
    return 0;
}
#ifdef FCS_KAZ_TREE_USE_RECORD_DICT_KEY
#error "FCS_KAZ_TREE_USE_RECORD_DICT_KEY was defined and this is incompatible"
#endif
