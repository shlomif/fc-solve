// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2012 Shlomi Fish

// dbm_procs.h - common procedures for dbm_solver.c and depth_dbm_solver.c.
#pragma once

#include "dbm_move_to_string.h"
#include "render_state.h"
#include "try_param.h"
#include "rinutils/portable_time.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fcs_dbm_solver_thread_struct dbm_solver_thread;

typedef struct
{
    dbm_solver_thread *thread;
} thread_arg;

static inline void instance_check_key(dbm_solver_thread *const thread,
    dbm_solver_instance *const instance, const size_t key_depth,
    fcs_encoded_state_buffer *const key,
    const fcs_encoded_state_buffer raw_parent, const uint8_t move,
    const fcs_which_moves_bitmask *const which_irreversible_moves_bitmask);

static inline bool instance_check_multiple_keys(dbm_solver_thread *const thread,
    dbm_solver_instance *const instance,
    fcs_dbm__cache_store__common *const cache_store GCC_UNUSED,
    meta_allocator *const meta_alloc GCC_UNUSED, fcs_derived_state **lists,
    size_t batch_size)
{
    // Small optimization in case the list is empty.
    if (batch_size == 1 && !lists[0])
    {
        return false;
    }
    fcs_lock_lock(&instance->common.storage_lock);
    for (typeof(batch_size) batch_i = 0; batch_i < batch_size; ++batch_i)
    {
        for (var_AUTO(list, lists[batch_i]); list; list = list->next)
        {
            instance_check_key(thread, instance, CHECK_KEY_CALC_DEPTH(),
                &(list->key_and_parent.key), list->key_and_parent.parent,
                list->move, &(list->which_irreversible_moves_bitmask));
        }
    }
#ifdef IN_DEPTH_DBM_SOLVER_dot_c
    const bool have_more = !q_stats_is_empty(
        &instance->colls_by_depth[instance->curr_depth].queue.stats);

    if (have_more)
    {
        fcs_condvar_signal(&(instance->monitor));
    }
    else
    {
        fcs_condvar_broadcast(&(instance->monitor));
    }
#else
    const bool have_more = false;
#endif
    fcs_lock_unlock(&instance->common.storage_lock);
    return have_more;
}

static void instance_print_stats(dbm_solver_instance *const instance)
{
    const_AUTO(mytime, rinutils_get_time());
    FILE *const out_fh = instance->common.out_fh;
    fprintf(out_fh,
        ("Reached %lu ; States-in-collection: %lu ; Time: " RIN_TIME_FMT "\n"
         ">>>Queue Stats: inserted=%lu items_in_queue=%lu extracted=%lu\n"),
        instance->common.count_num_processed,
        instance->common.num_states_in_collection, RIN_TIME__GET_BOTH(mytime),
        instance->common.num_states_in_collection,
        instance->common.count_of_items_in_queue,
        instance->common.count_num_processed);
    fflush(out_fh);
}

#ifdef DEBUG_FOO

static inline void instance_debug_out_state(
    dbm_solver_instance *const instance, fcs_encoded_state_buffer *enc_state)
{
    fcs_state_keyval_pair state;
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    const_AUTO(local_variant, instance->common.variant);
    // Handle item.
    fc_solve_delta_stater_decode_into_state(
        &global_delta_stater, enc_state->s, &state, indirect_stacks_buffer);

    fcs_render_state_str state_str;
    FCS__RENDER_STATE(state_str, &(state.s), &locs);
    fprintf(instance->common.out_fh, "Found State:\n<<<\n%s>>>\n", state_str);
    fflush(instance->common.out_fh);
}

#else
#define instance_debug_out_state(instance, key)                                \
    {                                                                          \
    }
#endif

static bool dbm_lookup_parent(const size_t count_stores, fcs_dbm_store *stores,
    const fcs_encoded_state_buffer key, fcs_encoded_state_buffer *const parent)
{
    for (size_t i = 0; i < count_stores; ++i)
    {
        var_AUTO(store, stores[i]);
        if (fc_solve_dbm_store_lookup_parent(store, key.s, parent->s))

        {
            return true;
        }
    }
    return false;
}

static void calc_trace(const size_t count_stores, fcs_dbm_store *stores,
    fcs_dbm_record *const ptr_initial_record,
    fcs_encoded_state_buffer **const ptr_trace, size_t *const ptr_trace_num)
{
#define GROW_BY 100
    size_t trace_num = 0;
    size_t trace_max_num = GROW_BY;
    fcs_encoded_state_buffer *trace = SMALLOC(trace, trace_max_num);
    fcs_encoded_state_buffer *key_ptr = trace;
    fcs_encoded_state_buffer parent;
    fcs_dbm_record init_record = *ptr_initial_record;
    fcs_dbm_record *record = &init_record;

    while (record)
    {
#if 1
        *(key_ptr) = record->key;
        if ((++trace_num) == trace_max_num)
        {
            trace = SREALLOC(trace, trace_max_num += GROW_BY);
            key_ptr = &(trace[trace_num - 1]);
        }
        const_AUTO(
            found, dbm_lookup_parent(count_stores, stores, *key_ptr, &parent));
        if (found)
        {
            record->parent = record->key;
            record->key = parent;
        }
        else
        {
            record = NULL;
        }
        key_ptr++;
#endif
    }
#undef GROW_BY
    *ptr_trace_num = trace_num - 1;
    *ptr_trace = trace;
}

static inline void mark_and_sweep_old_states(
    dbm_solver_instance *const instance GCC_UNUSED,
    dict_t *const kaz_tree GCC_UNUSED, const size_t curr_depth GCC_UNUSED)
{
#ifdef FCS_NO_DBM_AVL
#error FCS_NO_DBM_AVL
#else
#if 0

    // Now that we are about to descend to a new depth, let's
    // mark-and-sweep the old states, some of which are no longer of interest.
    FILE *const out_fh = instance->common.out_fh;
    TRACE("Start mark-and-sweep cleanup for curr_depth=%lu\n",
        (unsigned long)curr_depth);
    const_AUTO(tree_recycle_bin,
        ((struct rb_node **)(&(instance->common.tree_recycle_bin))));

    struct rb_traverser trav;
    rb_t_init(&trav, kaz_tree);

    const size_t items_count = kaz_tree->rb_count;
    size_t idx = 0;
    return;
    for (dict_key_t item = rb_t_first(&trav, kaz_tree); item;
        item = rb_t_next(&trav))
    {
#if 0
        if (!rb_get_decommissioned_flag(item))
        {
            var_AUTO(ancestor, (struct rb_node *)item);
#if 0
            while (fcs_dbm_record_get_refcount(&(ancestor->rb_data)) == 0)
#else
            while (true)
#endif
            {
                rb_set_decommissioned_flag(ancestor, 1);

                AVL_SET_NEXT(ancestor, *tree_recycle_bin);
                *tree_recycle_bin = ancestor;

                if (!(ancestor =
                            (struct rb_node *)fcs_dbm_record_get_parent_ptr(
                                &(ancestor->rb_data))))
                {
                    break;
                }
                fcs_dbm_record_decrement_refcount(&(ancestor->rb_data));
            }
        }
#endif
        var_AUTO(ancestor, (struct rb_node *)item);
        AVL_SET_NEXT(ancestor, *tree_recycle_bin);
        *tree_recycle_bin = ancestor;
        if (((++idx) % 100000) == 0)
        {
#ifdef WIN32
            fprintf(out_fh,
                "Mark+Sweep Progress - " RIN_ULL_FMT "/" RIN_ULL_FMT "\n",
                (unsigned long long)idx, (unsigned long long)items_count);
#else
            fprintf(
                out_fh, "Mark+Sweep Progress - %zu/%zu\n", idx, items_count);
#endif
        }
    }
    TRACE("Finish mark-and-sweep cleanup for curr_depth=%lu\n",
        (unsigned long)curr_depth);
#endif
#endif
}

#ifdef FCS_DBM_SINGLE_THREAD
#define NUM_THREADS() 1
#else
#define NUM_THREADS() num_threads
#endif

#define DESTROY_STORE(instance)                                                \
    fc_solve_dbm_store_destroy((instance)->cache_store.store)

#define DESTROY_CACHE(instance) DESTROY_STORE(instance)

static inline void instance_increment(dbm_solver_instance *const instance)
{
    --instance->common.count_of_items_in_queue;
    ++instance->common.queue_num_extracted_and_processed;
    if (++instance->common.count_num_processed % 100000 == 0)
    {
        instance_print_stats(instance);
    }
    if (unlikely((instance->common.count_num_processed >=
                     instance->common.max_count_num_processed) ||
                 (instance->common.num_states_in_collection >=
                     instance->common.max_num_states_in_collection)))
    {
        instance->common.should_terminate = MAX_ITERS_TERMINATE;
    }
}

static inline void fcs_dbm__cache_store__init(
    fcs_dbm__cache_store__common *const cache_store,
    dbm_instance_common_elems *const common,
    meta_allocator *const meta_alloc GCC_UNUSED,
    const char *const dbm_store_path,
    const unsigned long pre_cache_max_count GCC_UNUSED,
    const unsigned long caches_delta GCC_UNUSED)
{
    fc_solve_dbm_store_init(
        &(cache_store->store), dbm_store_path, &(common->tree_recycle_bin));
}

typedef struct
{
    fcs_dbm_variant_type local_variant;
    const char *offload_dir_path, *dbm_store_path;
    unsigned long iters_delta_limit, max_num_states_in_collection;
    unsigned long pre_cache_max_count, caches_delta;
    size_t num_threads;
} fcs_dbm_common_input;

static const fcs_dbm_common_input fcs_dbm_common_input_init = {
    .local_variant = FCS_DBM_VARIANT_2FC_FREECELL,
    .offload_dir_path = NULL,
    .dbm_store_path = "./fc_solve_dbm_store",
    .pre_cache_max_count = 1000000,
    .iters_delta_limit = ULONG_MAX,
    .max_num_states_in_collection = ULONG_MAX,
    .caches_delta = 1000000,
    .num_threads = 2};

static inline bool fcs_dbm__extract_common_from_argv(const int argc,
    char **const argv, int *const arg, fcs_dbm_common_input *const inp)
{
    const char *param;
    if ((param = TRY_PARAM("--pre-cache-max-count")))
    {
        if ((inp->pre_cache_max_count = (unsigned long)atol(param)) < 1000)
        {
            exit_error("--pre-cache-max-count must be at least 1,000.\n");
        }
        return true;
    }
    else if ((param = TRY_PARAM("--game")))
    {
        if (!strcmp(param, "bakers_dozen"))
        {
            inp->local_variant = FCS_DBM_VARIANT_BAKERS_DOZEN;
        }
        else if (!strcmp(param, "freecell"))
        {
            inp->local_variant = FCS_DBM_VARIANT_2FC_FREECELL;
        }
        else
        {
            exit_error("Unknown game '%s'. Aborting\n", param);
        }
        return true;
    }
    else if ((param = TRY_PARAM("--offload-dir-path")))
    {
        inp->offload_dir_path = param;
        return true;
    }
    else if ((param = TRY_PARAM("--num-threads")))
    {
        if ((inp->num_threads = (size_t)atoi(param)) < 1)
        {
            exit_error("--num-threads must be at least 1.\n");
        }
        return true;
    }
    else if ((param = TRY_PARAM("--max-num-states")))
    {
        inp->max_num_states_in_collection = (unsigned long)atol(param);
        return true;
    }
    else if ((param = TRY_PARAM("--iters-delta-limit")))
    {
        inp->iters_delta_limit = (unsigned long)atol(param);
        return true;
    }
    else if ((param = TRY_PARAM("--caches-delta")))
    {
        if ((inp->caches_delta = (unsigned long)atol(param)) < 1000)
        {
            exit_error("--caches-delta must be at least 1,000.\n");
        }
        return true;
    }
    else if ((param = TRY_PARAM("--dbm-store-path")))
    {
        inp->dbm_store_path = param;
        return true;
    }
    return false;
}

static inline fcs_dbm_record *cache_store__has_key(
    fcs_dbm__cache_store__common *const cache_store,
    fcs_encoded_state_buffer *const key, const fcs_encoded_state_buffer parent)
{
    return fc_solve_dbm_store_insert_key_value(cache_store->store, key, parent);
}

#ifdef __cplusplus
}
#endif
