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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FCS_DBM_WITHOUT_CACHES

static int fc_solve_compare_pre_cache_keys(
    const void *const void_a, const void *const void_b, void *const context)
{
#define GET_PARAM(p) ((((const fcs_pre_cache_key_val_pair_t *)(p))->key))
    return memcmp(
        &(GET_PARAM(void_a)), &(GET_PARAM(void_b)), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static GCC_INLINE void pre_cache_init(fcs_pre_cache_t *const pre_cache_ptr,
    fcs_meta_compact_allocator_t *const meta_alloc)
{
    pre_cache_ptr->kaz_tree = fc_solve_kaz_tree_create(
        fc_solve_compare_pre_cache_keys, NULL, meta_alloc);

    fc_solve_compact_allocator_init(&(pre_cache_ptr->kv_allocator), meta_alloc);
    pre_cache_ptr->kv_recycle_bin = NULL;
    pre_cache_ptr->count_elements = 0;
}

static GCC_INLINE void pre_cache_insert(fcs_pre_cache_t *pre_cache,
    fcs_encoded_state_buffer_t *key, fcs_encoded_state_buffer_t *parent)
{
    fcs_pre_cache_key_val_pair_t *to_insert;

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

static GCC_INLINE fcs_bool_t pre_cache_does_key_exist(
    fcs_pre_cache_t *pre_cache, fcs_encoded_state_buffer_t *key)
{
    fcs_pre_cache_key_val_pair_t to_check;

    to_check.key = *key;

    return (
        fc_solve_kaz_tree_lookup_value(pre_cache->kaz_tree, &to_check) != NULL);
}

static GCC_INLINE void cache_populate_from_pre_cache(
    fcs_lru_cache_t *const cache, fcs_pre_cache_t *const pre_cache)
{
#ifdef FCS_DBM_USE_LIBAVL
    dict_t *kaz_tree;
    struct avl_traverser trav;
    dict_key_t item;

    kaz_tree = pre_cache->kaz_tree;
    avl_t_init(&trav, kaz_tree);

    for (item = avl_t_first(&trav, kaz_tree); item; item = avl_t_next(&trav))
    {
        cache_insert(cache, &(((fcs_pre_cache_key_val_pair_t *)(item))->key),
            NULL, '\0');
    }
#else
    dnode_t *node;
    dict_t *kaz_tree;

    kaz_tree = pre_cache->kaz_tree;

    for (node = fc_solve_kaz_tree_first(kaz_tree); node;
         node = fc_solve_kaz_tree_next(kaz_tree, node))
    {
        cache_insert(
            cache, &(((fcs_pre_cache_key_val_pair_t *)(node->dict_key))->key));
    }
#endif
}

static GCC_INLINE void pre_cache_offload_and_destroy(
    fcs_pre_cache_t *const pre_cache, fcs_dbm_store_t store,
    fcs_lru_cache_t *const cache)
{
    fc_solve_dbm_store_offload_pre_cache(store, pre_cache);
    cache_populate_from_pre_cache(cache, pre_cache);

    /* Now reset the pre_cache. */
    fc_solve_kaz_tree_destroy(pre_cache->kaz_tree);
    fc_solve_compact_allocator_finish(&(pre_cache->kv_allocator));
}

#ifndef FCS_DBM_CACHE_ONLY
#define PRE_CACHE_OFFLOAD(instance)                                            \
    pre_cache_offload_and_destroy( \
        &((instance)->pre_cache), (instance)->store, &((instance)->cache)
#else
#define PRE_CACHE_OFFLOAD(instance)                                            \
    {                                                                          \
    }
#endif

#ifndef FCS_DBM_CACHE_ONLY

static GCC_INLINE void pre_cache_offload_and_reset(
    fcs_pre_cache_t *const pre_cache, const fcs_dbm_store_t store,
    fcs_lru_cache_t *const cache,
    fcs_meta_compact_allocator_t *const meta_alloc)
{
    pre_cache_offload_and_destroy(pre_cache, store, cache);
    pre_cache_init(pre_cache, meta_alloc);
}

#endif

#endif /* FCS_DBM_WITHOUT_CACHES */

typedef struct fcs_dbm_solver_thread_struct fcs_dbm_solver_thread_t;

static GCC_INLINE void instance_check_key(fcs_dbm_solver_thread_t *const thread,
    fcs_dbm_solver_instance_t *const instance, const int key_depth,
    fcs_encoded_state_buffer_t *const key, fcs_dbm_record_t *const parent,
    const unsigned char move,
    const fcs_which_moves_bitmask_t *const which_irreversible_moves_bitmask
#ifdef FCS_DBM_CACHE_ONLY
    ,
    const fcs_fcc_move_t *moves_to_parent
#endif
    );

static GCC_INLINE void instance_check_multiple_keys(
    fcs_dbm_solver_thread_t *thread, fcs_dbm_solver_instance_t *instance,
    fcs_derived_state_t *list
#ifdef FCS_DBM_CACHE_ONLY
    ,
    const fcs_fcc_move_t *moves_to_parent
#endif
    )
{
    /* Small optimization in case the list is empty. */
    if (!list)
    {
        return;
    }
    FCS_LOCK(instance->storage_lock);
    for (; list; list = list->next)
    {
        instance_check_key(
            thread, instance, CHECK_KEY_CALC_DEPTH(), &(list->key),
            list->parent, list->move, &(list->which_irreversible_moves_bitmask)
#ifdef FCS_DBM_CACHE_ONLY
                                          ,
            moves_to_parent
#endif
            );
    }
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
    if (instance->pre_cache.count_elements >= instance->pre_cache_max_count)
    {
        pre_cache_offload_and_reset(&(instance->pre_cache), instance->store,
            &(instance->cache), &(instance->meta_alloc));
    }
#endif
#endif
    FCS_UNLOCK(instance->storage_lock);
}

static void instance_print_stats(fcs_dbm_solver_instance_t *const instance)
{
    fcs_portable_time_t mytime;
    FCS_GET_TIME(mytime);

    FILE *const out_fh = instance->out_fh;
    fprintf(out_fh,
        ("Reached %ld ; States-in-collection: %ld ; Time: %li.%.6li\n"
         ">>>Queue Stats: inserted=%ld items_in_queue=%ld extracted=%ld\n"),
        instance->count_num_processed, instance->num_states_in_collection,
        FCS_TIME_GET_SEC(mytime), FCS_TIME_GET_USEC(mytime),
        instance->num_states_in_collection,
        instance->num_states_in_collection - instance->count_num_processed,
        instance->count_num_processed);
    fflush(out_fh);
}

#ifdef DEBUG_FOO

static GCC_INLINE void instance_debug_out_state(
    fcs_dbm_solver_instance_t *instance, fcs_encoded_state_buffer_t *enc_state)
{
    fcs_state_keyval_pair_t state;
    fcs_state_locs_struct_t locs;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    fc_solve_init_locs(&locs);
    const_AUTO(local_variant, instance->variant);
    /* Handle item. */
    fc_solve_delta_stater_decode_into_state(
        &global_delta_stater, enc_state->s, &state, indirect_stacks_buffer);

    char state_str[1000];
    FCS__RENDER_STATE(state_str, &(state.s), &locs);
    fprintf(instance->out_fh, "Found State:\n<<<\n%s>>>\n", state_str);
    fflush(instance->out_fh);
}

#else
#define instance_debug_out_state(instance, key)                                \
    {                                                                          \
    }
#endif

static void calc_trace(fcs_dbm_record_t *const ptr_initial_record,
    fcs_encoded_state_buffer_t **const ptr_trace, int *const ptr_trace_num)
{
#define GROW_BY 100
    size_t trace_num = 0;
    size_t trace_max_num = GROW_BY;
    fcs_encoded_state_buffer_t *trace = SMALLOC(trace, trace_max_num);
    fcs_encoded_state_buffer_t *key_ptr = trace;
    fcs_dbm_record_t *record = ptr_initial_record;

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

    return;
}

static GCC_INLINE void mark_and_sweep_old_states(
    fcs_dbm_solver_instance_t *const instance, dict_t *const kaz_tree,
    const int curr_depth)
{
    /* Now that we are about to ascend to a new depth, let's
     * mark-and-sweep
     * the old states, some of which are no longer of interest.
     * */
    FILE *const out_fh = instance->out_fh;
    TRACE("Start mark-and-sweep cleanup for curr_depth=%d\n", curr_depth);
    struct avl_node **tree_recycle_bin =
        ((struct avl_node **)(&(instance->tree_recycle_bin)));

    struct avl_traverser trav;
    avl_t_init(&trav, kaz_tree);

    const size_t items_count = kaz_tree->avl_count;
    size_t idx = 0;
    for (dict_key_t item = avl_t_first(&trav, kaz_tree); item;
         item = avl_t_next(&trav))
    {
        if (!avl_get_decommissioned_flag(item))
        {
            var_AUTO(ancestor, (struct avl_node *)item);
            while (fcs_dbm_record_get_refcount(&(ancestor->avl_data)) == 0)
            {
                avl_set_decommissioned_flag(ancestor, 1);

                AVL_SET_NEXT(ancestor, *(tree_recycle_bin));
                *(tree_recycle_bin) = ancestor;

                if (!(ancestor =
                            (struct avl_node *)fcs_dbm_record_get_parent_ptr(
                                &(ancestor->avl_data))))
                {
                    break;
                }
                fcs_dbm_record_decrement_refcount(&(ancestor->avl_data));
            }
        }
        if (((++idx) % 100000) == 0)
        {
            fprintf(
                out_fh, "Mark+Sweep Progress - %zd/%zd\n", idx, items_count);
        }
    }
    TRACE("Finish mark-and-sweep cleanup for curr_depth=%d\n", curr_depth);
}

#ifdef FCS_DBM_SINGLE_THREAD
#define NUM_THREADS() 1
#else
#define NUM_THREADS() num_threads
#endif

#ifndef FCS_DBM_WITHOUT_CACHES
#define DESTROY_CACHE(instance)                                                \
    {                                                                          \
        PRE_CACHE_OFFLOAD(instance);                                           \
        cache_destroy(&(instance->cache));                                     \
    }
#else
#define DESTROY_CACHE(instance)                                                \
    {                                                                          \
    }
#endif

#ifdef __cplusplus
}
#endif
