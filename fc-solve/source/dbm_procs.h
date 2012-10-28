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
 * dbm_procs.h - common procedures for dbm_solver.c and depth_dbm_solver.c.
 */
#ifndef FC_SOLVE__DBM_PROCS_H
#define FC_SOLVE__DBM_PROCS_H

#include "inline.h"
#include "portable_time.h"

#ifdef __cplusplus
extern "C"
{
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

#ifndef FCS_DBM_CACHE_ONLY

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

static GCC_INLINE void instance_check_key(
    fcs_dbm_solver_instance_t * instance,
    int key_depth,
    fcs_encoded_state_buffer_t * key,
    fcs_dbm_record_t * parent,
    unsigned char move
#ifdef FCS_DBM_CACHE_ONLY
    , const fcs_fcc_move_t * moves_to_parent
#endif
);

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
            CHECK_KEY_CALC_DEPTH(),
            &(list->key), list->parent, list->move
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
    fprintf (out_fh, ">>>Queue Stats: inserted=%ld items_in_queue=%ld extracted=%ld\n",
             instance->num_states_in_collection,
             instance->num_states_in_collection - instance->count_num_processed,
             instance->count_num_processed
            );
    fflush(out_fh);
}

#ifdef DEBUG_FOO

static GCC_INLINE void instance_debug_out_state(
    fcs_dbm_solver_instance_t * instance,
    fcs_encoded_state_buffer_t * enc_state
)
{
    char * state_str;
    fcs_state_keyval_pair_t state;
    fcs_state_locs_struct_t locs;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    fc_solve_init_locs(&locs);
    /* Handle item. */
    fc_solve_delta_stater_decode_into_state(
        global_delta_stater,
        enc_state->s,
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

#else
#define instance_debug_out_state(instance, key) {}
#endif

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
    trace = SMALLOC(trace, (trace_max_num = GROW_BY));
    key_ptr = trace;
    record = ptr_initial_record;

    while (record)
    {
#if 1
        *(key_ptr) = record->key;
        if ((++trace_num) == trace_max_num)
        {
            trace = SREALLOC(trace, trace_max_num += GROW_BY);
            key_ptr = &(trace[trace_num-1]);
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

#ifdef FCS_DBM_SINGLE_THREAD
#define NUM_THREADS() 1
#else
#define NUM_THREADS() num_threads
#endif

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE__DBM_PROCS_H */
