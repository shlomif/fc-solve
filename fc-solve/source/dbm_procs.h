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
 * dbm_procs.h - common procedures for dbm_solver.c and depth_dbm_solver.c.
 */
#pragma once

#include "dbm_move_to_string.h"
#include "render_state.h"
#include "try_param.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FCS_DBM_WITHOUT_CACHES

static int fc_solve_compare_pre_cache_keys(
    const void *const void_a, const void *const void_b, void *const context)
{
#define GET_PARAM(p) ((((const pre_cache_key_val_pair *)(p))->key))
    return memcmp(
        &(GET_PARAM(void_a)), &(GET_PARAM(void_b)), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static inline void pre_cache_init(
    fcs_pre_cache *const pre_cache_ptr, meta_allocator *const meta_alloc)
{
    pre_cache_ptr->tree_recycle_bin = NULL;
    pre_cache_ptr->kaz_tree =
        fc_solve_kaz_tree_create(fc_solve_compare_pre_cache_keys, NULL,
            meta_alloc, &(pre_cache_ptr->tree_recycle_bin));

    fc_solve_compact_allocator_init(&(pre_cache_ptr->kv_allocator), meta_alloc);
    pre_cache_ptr->kv_recycle_bin = NULL;
    pre_cache_ptr->count_elements = 0;
}

static inline void pre_cache_insert(fcs_pre_cache *pre_cache,
    fcs_encoded_state_buffer *key, fcs_encoded_state_buffer *parent)
{
    pre_cache_key_val_pair *to_insert;

    if (pre_cache->kv_recycle_bin)
    {
        pre_cache->kv_recycle_bin =
            (to_insert = pre_cache->kv_recycle_bin)->next;
    }
    else
    {
        to_insert = fcs_compact_alloc_ptr(
            &(pre_cache->kv_allocator), sizeof(*to_insert));
    }
    to_insert->key = *key;
    to_insert->parent = *parent;

    fc_solve_kaz_tree_alloc_insert(pre_cache->kaz_tree, to_insert);
    pre_cache->count_elements++;
}

static inline bool pre_cache_does_key_exist(
    fcs_pre_cache *pre_cache, fcs_encoded_state_buffer *key)
{
    pre_cache_key_val_pair to_check = {.key = *key};
    return (
        fc_solve_kaz_tree_lookup_value(pre_cache->kaz_tree, &to_check) != NULL);
}

static inline void cache_populate_from_pre_cache(
    fcs_lru_cache *const cache, fcs_pre_cache *const pre_cache)
{
#ifdef FCS_DBM_USE_LIBAVL
    struct rb_traverser trav;
    var_AUTO(kaz_tree, pre_cache->kaz_tree);
    rb_t_init(&trav, kaz_tree);

    for (dict_key_t item = rb_t_first(&trav, kaz_tree); item;
         item = rb_t_next(&trav))
    {
        cache_insert(
            cache, &(((pre_cache_key_val_pair *)(item))->key), NULL, '\0');
    }
#else
    var_AUTO(kaz_tree, pre_cache->kaz_tree);
    for (dnode_t *node = fc_solve_kaz_tree_first(kaz_tree); node;
         node = fc_solve_kaz_tree_next(kaz_tree, node))
    {
        cache_insert(
            cache, &(((pre_cache_key_val_pair *)(node->dict_key))->key));
    }
#endif
}

static inline void pre_cache_offload_and_destroy(fcs_pre_cache *const pre_cache,
    fcs_dbm_store store, fcs_lru_cache *const cache)
{
    fc_solve_dbm_store_offload_pre_cache(store, pre_cache);
    cache_populate_from_pre_cache(cache, pre_cache);

    /* Now reset the pre_cache. */
    fc_solve_kaz_tree_destroy(pre_cache->kaz_tree);
    fc_solve_compact_allocator_finish(&(pre_cache->kv_allocator));
}

#ifndef FCS_DBM_CACHE_ONLY
#define PRE_CACHE_OFFLOAD(instance)                                            \
    pre_cache_offload_and_destroy(&((instance)->cache_store.pre_cache),        \
        (instance)->cache_store.store, &((instance)->cache_store.cache))
#else
#define PRE_CACHE_OFFLOAD(instance)                                            \
    {                                                                          \
    }
#endif

#ifndef FCS_DBM_CACHE_ONLY

static inline void pre_cache_offload_and_reset(fcs_pre_cache *const pre_cache,
    const fcs_dbm_store store, fcs_lru_cache *const cache,
    meta_allocator *const meta_alloc)
{
    pre_cache_offload_and_destroy(pre_cache, store, cache);
    pre_cache_init(pre_cache, meta_alloc);
}

#endif

#endif // FCS_DBM_WITHOUT_CACHES

typedef struct fcs_dbm_solver_thread_struct dbm_solver_thread;

typedef struct
{
    dbm_solver_thread *thread;
} thread_arg;

static inline void instance_check_key(
    dbm_solver_thread *const thread, dbm_solver_instance *const instance,
    const int key_depth, fcs_encoded_state_buffer *const key,
    fcs_dbm_record *const parent, const unsigned char move,
    const fcs_which_moves_bitmask *const which_irreversible_moves_bitmask
#ifndef FCS_DBM_WITHOUT_CACHES
    ,
    const fcs_fcc_move *moves_to_parent
#endif
);

static inline bool instance_check_multiple_keys(
    dbm_solver_thread *const thread, dbm_solver_instance *const instance,
    fcs_dbm__cache_store__common *const cache_store GCC_UNUSED,
    meta_allocator *const meta_alloc GCC_UNUSED, fcs_derived_state **lists,
    size_t batch_size
#ifndef FCS_DBM_WITHOUT_CACHES
    ,
    const fcs_fcc_move *moves_to_parent
#endif
)
{
    /* Small optimization in case the list is empty. */
    if (batch_size == 1 && !lists[0])
    {
        return FALSE;
    }
    fcs_lock_lock(&instance->common.storage_lock);
    for (typeof(batch_size) batch_i = 0; batch_i < batch_size; ++batch_i)
    {
        for (var_AUTO(list, lists[batch_i]); list; list = list->next)
        {
            instance_check_key(thread, instance, CHECK_KEY_CALC_DEPTH(),
                &(list->key), list->parent, list->move,
                &(list->which_irreversible_moves_bitmask)
#ifndef FCS_DBM_WITHOUT_CACHES
                    ,
                moves_to_parent
#endif
            );
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
            if (cache_store->pre_cache.count_elements >=
                cache_store->pre_cache_max_count)
            {
                pre_cache_offload_and_reset(&(cache_store->pre_cache),
                    cache_store->store, &(cache_store->cache), meta_alloc);
            }
#endif
#endif
        }
    }
#ifdef MAX_FCC_DEPTH
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
    const bool have_more = FALSE;
#endif
    fcs_lock_unlock(&instance->common.storage_lock);
    return have_more;
}

static void instance_print_stats(dbm_solver_instance *const instance)
{
    const_AUTO(mytime, fcs_get_time());
    FILE *const out_fh = instance->common.out_fh;
    fprintf(out_fh,
        ("Reached %ld ; States-in-collection: %ld ; Time: " FCS_T_FMT "\n"
         ">>>Queue Stats: inserted=%ld items_in_queue=%ld extracted=%ld\n"),
        instance->common.count_num_processed,
        instance->common.num_states_in_collection, FCS_TIME_GET_SEC(mytime),
        FCS_TIME_GET_USEC(mytime), instance->common.num_states_in_collection,
        instance->common.num_states_in_collection -
            instance->common.count_num_processed,
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
    /* Handle item. */
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

static void calc_trace(fcs_dbm_record *const ptr_initial_record,
    fcs_encoded_state_buffer **const ptr_trace, int *const ptr_trace_num)
{
#define GROW_BY 100
    size_t trace_num = 0;
    size_t trace_max_num = GROW_BY;
    fcs_encoded_state_buffer *trace = SMALLOC(trace, trace_max_num);
    fcs_encoded_state_buffer *key_ptr = trace;
    fcs_dbm_record *record = ptr_initial_record;

    while (record)
    {
#if 1
        *(key_ptr) = record->key;
        if ((++trace_num) == trace_max_num)
        {
            trace = SREALLOC(trace, trace_max_num += GROW_BY);
            key_ptr = &(trace[trace_num - 1]);
        }
        record = fcs_dbm_record_get_parent_ptr(record);
        key_ptr++;
#endif
    }
#undef GROW_BY
    *ptr_trace_num = trace_num;
    *ptr_trace = trace;
}

static inline void mark_and_sweep_old_states(
    dbm_solver_instance *const instance, dict_t *const kaz_tree,
    const int curr_depth)
{
    /* Now that we are about to ascend to a new depth, let's
     * mark-and-sweep
     * the old states, some of which are no longer of interest.
     * */
    FILE *const out_fh = instance->common.out_fh;
    TRACE("Start mark-and-sweep cleanup for curr_depth=%d\n", curr_depth);
    const_AUTO(tree_recycle_bin,
        ((struct rb_node **)(&(instance->common.tree_recycle_bin))));

    struct rb_traverser trav;
    rb_t_init(&trav, kaz_tree);

    const size_t items_count = kaz_tree->rb_count;
    size_t idx = 0;
    for (dict_key_t item = rb_t_first(&trav, kaz_tree); item;
         item = rb_t_next(&trav))
    {
        if (!rb_get_decommissioned_flag(item))
        {
            var_AUTO(ancestor, (struct rb_node *)item);
            while (fcs_dbm_record_get_refcount(&(ancestor->rb_data)) == 0)
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
        if (((++idx) % 100000) == 0)
        {
#ifdef WIN32
            fprintf(out_fh,
                "Mark+Sweep Progress - " FCS_LL_FMT "/" FCS_LL_FMT "\n",
                (long long)idx, (long long)items_count);
#else
            fprintf(
                out_fh, "Mark+Sweep Progress - %zu/%zu\n", idx, items_count);
#endif
        }
    }
    TRACE("Finish mark-and-sweep cleanup for curr_depth=%d\n", curr_depth);
}

#ifdef FCS_DBM_SINGLE_THREAD
#define NUM_THREADS() 1
#else
#define NUM_THREADS() num_threads
#endif

#ifndef FCS_DBM_CACHE_ONLY
#define DESTROY_STORE(instance)                                                \
    fc_solve_dbm_store_destroy((instance)->cache_store.store)
#else
#define DESTROY_STORE(instance)
#endif

#ifndef FCS_DBM_WITHOUT_CACHES
#define DESTROY_CACHE(instance)                                                \
    {                                                                          \
        PRE_CACHE_OFFLOAD(instance);                                           \
        cache_destroy(&(instance->cache_store.cache));                         \
        DESTROY_STORE(instance);                                               \
    }
#else
#define DESTROY_CACHE(instance) DESTROY_STORE(instance)
#endif

static inline void instance_increment(dbm_solver_instance *const instance)
{
    --instance->common.count_of_items_in_queue;
    ++instance->common.queue_num_extracted_and_processed;
    if (++instance->common.count_num_processed % 100000 == 0)
    {
        instance_print_stats(instance);
    }
    if (instance->common.count_num_processed >=
        instance->common.max_count_num_processed)
    {
        instance->common.should_terminate = MAX_ITERS_TERMINATE;
    }
}

static inline void fcs_dbm__cache_store__init(
    fcs_dbm__cache_store__common *const cache_store,
    dbm_instance_common_elems *const common,
    meta_allocator *const meta_alloc GCC_UNUSED,
    const char *const dbm_store_path, const long pre_cache_max_count GCC_UNUSED,
    const long caches_delta GCC_UNUSED)
{
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
    pre_cache_init(&(cache_store->pre_cache), meta_alloc);
#endif
    cache_init(
        &(cache_store->cache), pre_cache_max_count + caches_delta, meta_alloc);
#endif
#ifndef FCS_DBM_CACHE_ONLY
    fc_solve_dbm_store_init(
        &(cache_store->store), dbm_store_path, &(common->tree_recycle_bin));
#endif
}

typedef struct
{
    fcs_dbm_variant_type local_variant;
    const char *offload_dir_path, *dbm_store_path;
    long pre_cache_max_count, iters_delta_limit, caches_delta;
    size_t num_threads;
} fcs_dbm_common_input;

static const fcs_dbm_common_input fcs_dbm_common_input_init = {
    .local_variant = FCS_DBM_VARIANT_2FC_FREECELL,
    .offload_dir_path = NULL,
    .dbm_store_path = "./fc_solve_dbm_store",
    .pre_cache_max_count = 1000000,
    .iters_delta_limit = -1,
    .caches_delta = 1000000,
    .num_threads = 2};

static inline bool fcs_dbm__extract_common_from_argv(const int argc,
    char **const argv, int *const arg, fcs_dbm_common_input *const inp)
{
    const char *param;
    if ((param = TRY_PARAM("--pre-cache-max-count")))
    {
        if ((inp->pre_cache_max_count = atol(param)) < 1000)
        {
            fc_solve_err("--pre-cache-max-count must be at least 1,000.\n");
        }
        return TRUE;
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
            fc_solve_err("Unknown game '%s'. Aborting\n", param);
        }
        return TRUE;
    }
    else if ((param = TRY_PARAM("--offload-dir-path")))
    {
        inp->offload_dir_path = param;
        return TRUE;
    }
    else if ((param = TRY_PARAM("--num-threads")))
    {
        if ((inp->num_threads = (size_t)atoi(param)) < 1)
        {
            fc_solve_err("--num-threads must be at least 1.\n");
        }
        return TRUE;
    }
    else if ((param = TRY_PARAM("--iters-delta-limit")))
    {
        inp->iters_delta_limit = atol(param);
        return TRUE;
    }
    else if ((param = TRY_PARAM("--caches-delta")))
    {
        if ((inp->caches_delta = atol(param)) < 1000)
        {
            fc_solve_err("--caches-delta must be at least 1,000.\n");
        }
        return TRUE;
    }
    else if ((param = TRY_PARAM("--dbm-store-path")))
    {
        inp->dbm_store_path = param;
        return TRUE;
    }
    return FALSE;
}

static inline fcs_dbm_record *cache_store__has_key(
    fcs_dbm__cache_store__common *const cache_store,
    fcs_encoded_state_buffer *const key, fcs_dbm_record *const parent)
{
#ifndef FCS_DBM_WITHOUT_CACHES
    if (cache_does_key_exist(&(cache_store->cache), key))
    {
        return NULL;
    }
#ifndef FCS_DBM_CACHE_ONLY
    else if (pre_cache_does_key_exist(&(cache_store->pre_cache), key))
    {
        return NULL;
    }
    else if (fc_solve_dbm_store_does_key_exist(cache_store->store, key->s))
    {
        cache_insert(&(cache_store->cache), key, NULL, '\0');
        return NULL;
    }
#endif
    return ((fcs_dbm_record *)key);
#else
    return fc_solve_dbm_store_insert_key_value(
        cache_store->store, key, parent, TRUE);
#endif
}

#ifndef FCS_DBM_WITHOUT_CACHES
static inline fcs_cache_key_info *cache_store__insert_key(
    fcs_dbm__cache_store__common *const cache_store,
    fcs_encoded_state_buffer *const key, fcs_dbm_record *const parent,
    const fcs_fcc_move *const moves_to_parent,
    const unsigned char move GCC_UNUSED)
{
#ifndef FCS_DBM_CACHE_ONLY
    pre_cache_insert(&(cache_store->pre_cache), key, &(parent->key));
    return NULL;
#else
    return cache_insert(&(cache_store->cache), key, moves_to_parent, move);
#endif
}
#endif

#ifdef __cplusplus
}
#endif
