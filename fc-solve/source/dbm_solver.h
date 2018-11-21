/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "generic_tree.h"
#include "delta_states.h"

/* We need it for the typedef of fcs_fcc_move. */
#include "fcc_brfs_test.h"
#include "lock.h"

#ifndef FCS_DBM_WITHOUT_CACHES

#include "dbm_lru_cache.h"

typedef union fcs_pre_cache_key_val_pair_struct {
    struct
    {
        fcs_encoded_state_buffer key;
        fcs_encoded_state_buffer parent;
    };
    union fcs_pre_cache_key_val_pair_struct *next;
} pre_cache_key_val_pair;

typedef struct
{
    dict_t *kaz_tree;
    compact_allocator kv_allocator;
    pre_cache_key_val_pair *kv_recycle_bin;
    long count_elements;
    void *tree_recycle_bin;
} fcs_pre_cache;
#endif

typedef void *fcs_dbm_store;
void fc_solve_dbm_store_init(fcs_dbm_store *, const char *, void **);
dict_t *fc_solve_dbm_store_get_dict(fcs_dbm_store);
bool fc_solve_dbm_store_does_key_exist(fcs_dbm_store, const unsigned char *);
bool fc_solve_dbm_store_lookup_parent(
    fcs_dbm_store, const unsigned char *const, unsigned char *const);

fcs_dbm_record *fc_solve_dbm_store_insert_key_value(fcs_dbm_store store,
    const fcs_encoded_state_buffer *key, fcs_dbm_record *parent,
    const bool should_modify_parent);

#ifndef FCS_DBM_WITHOUT_CACHES
void fc_solve_dbm_store_offload_pre_cache(
    fcs_dbm_store store, fcs_pre_cache *const pre_cache);
#endif

void fc_solve_dbm_store_destroy(fcs_dbm_store store);

typedef struct fcs_dbm_queue_item_struct
{
    fcs_encoded_state_buffer key;
    /* TODO : maybe get rid of moves_seq with FCS_DBM_WITHOUT_CACHES
     * to save space. */
    fcs_fcc_moves_seq moves_seq;
#ifndef FCS_DBM_WITHOUT_CACHES
    fcs_fcc_move *moves_to_key;
#endif

    struct fcs_dbm_queue_item_struct *next;
} fcs_dbm_queue_item;

enum TERMINATE_REASON
{
    DONT_TERMINATE = 0,
    QUEUE_TERMINATE,
    MAX_ITERS_TERMINATE,
    SOLUTION_FOUND_TERMINATE
};

typedef struct
{
    fcs_lock storage_lock;
    long queue_num_extracted_and_processed;
    fcs_encoded_state_buffer first_key;
    long num_states_in_collection;
    FILE *out_fh;
    fcs_dbm_variant_type variant;
    long count_num_processed, count_of_items_in_queue, max_count_num_processed;
    bool queue_solution_was_found;
    enum TERMINATE_REASON should_terminate;
#ifdef FCS_DBM_WITHOUT_CACHES
    fcs_dbm_record *queue_solution_ptr;
#else
    fcs_encoded_state_buffer queue_solution;
#endif
    void *tree_recycle_bin;
} dbm_instance_common_elems;

static inline void fcs_dbm__found_solution(
    dbm_instance_common_elems *const common, fcs_dbm_record *const token,
    fcs_dbm_queue_item *const item GCC_UNUSED)
{
    common->should_terminate = SOLUTION_FOUND_TERMINATE;
    common->queue_solution_was_found = TRUE;
#ifdef FCS_DBM_WITHOUT_CACHES
    common->queue_solution_ptr = token;
#else
    common->queue_solution = item->key;
#endif
}

static inline void fcs_dbm__common_init(dbm_instance_common_elems *const common,
    const long iters_delta_limit, const fcs_dbm_variant_type local_variant,
    FILE *const out_fh)
{
    common->variant = local_variant;
    common->out_fh = out_fh;
    common->queue_solution_was_found = FALSE;
    common->should_terminate = DONT_TERMINATE;
    common->queue_num_extracted_and_processed = 0;
    common->num_states_in_collection = 0;
    common->count_num_processed = 0;
    if (iters_delta_limit >= 0)
    {
        common->max_count_num_processed =
            common->count_num_processed + iters_delta_limit;
    }
    else
    {
        common->max_count_num_processed = LONG_MAX;
    }
    common->count_of_items_in_queue = 0;
    common->tree_recycle_bin = NULL;
    fcs_lock_init(&common->storage_lock);
}

typedef struct
{
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
    fcs_pre_cache pre_cache;
#endif
    fcs_lru_cache cache;
#endif
#ifndef FCS_DBM_CACHE_ONLY
    fcs_dbm_store store;
#endif

    /* The queue */
    long pre_cache_max_count;
} fcs_dbm__cache_store__common;

#ifdef __cplusplus
}
#endif
