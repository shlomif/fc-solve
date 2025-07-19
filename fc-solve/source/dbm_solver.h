// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "generic_tree.h"
#include "delta_states.h"

// We need it for the typedef of fcs_fcc_move.
#include "fcc_brfs_test.h"
#include "lock.h"

typedef void *fcs_dbm_store;
void fc_solve_dbm_store_init(fcs_dbm_store *, const char *, void **);
dict_t *fc_solve_dbm_store_get_dict(fcs_dbm_store);
bool fc_solve_dbm_store_does_key_exist(fcs_dbm_store, const unsigned char *);
bool fc_solve_dbm_store_lookup_parent(
    fcs_dbm_store, const unsigned char *const, unsigned char *const);

fcs_dbm_record *fc_solve_dbm_store_insert_key_value(fcs_dbm_store store,
    const fcs_encoded_state_buffer *key, const fcs_encoded_state_buffer parent);

void fc_solve_dbm_store_destroy(fcs_dbm_store store);

typedef struct fcs_dbm_queue_item_struct
{
    fcs_encoded_state_buffer key;
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
    unsigned long queue_num_extracted_and_processed;
    fcs_encoded_state_buffer first_key;
    unsigned long num_states_in_collection, max_num_states_in_collection;
    FILE *out_fh;
    fcs_dbm_variant_type variant;
    unsigned long count_num_processed;
    unsigned long count_of_items_in_queue;
    unsigned long max_count_num_processed;
    bool queue_solution_was_found;
    enum TERMINATE_REASON should_terminate;
    fcs_dbm_record *queue_solution_ptr;
    void *tree_recycle_bin;
} dbm_instance_common_elems;

static inline void fcs_dbm__found_solution(
    dbm_instance_common_elems *const common, fcs_dbm_record *const token,
    fcs_dbm_queue_item *const item GCC_UNUSED)
{
    common->should_terminate = SOLUTION_FOUND_TERMINATE;
    common->queue_solution_was_found = true;
    common->queue_solution_ptr = token;
}

static inline void fcs_dbm__common_init(dbm_instance_common_elems *const common,
    const unsigned long iters_delta_limit,
    const unsigned long max_num_states_in_collection,
    const fcs_dbm_variant_type local_variant, FILE *const out_fh)
{
    common->variant = local_variant;
    common->out_fh = out_fh;
    common->queue_solution_was_found = false;
    common->should_terminate = DONT_TERMINATE;
    common->queue_num_extracted_and_processed = 0;
    common->num_states_in_collection = 0;
    common->count_num_processed = 0;
    common->max_num_states_in_collection = max_num_states_in_collection;
    common->max_count_num_processed = iters_delta_limit;
    common->count_of_items_in_queue = 0;
    common->tree_recycle_bin = NULL;
    fcs_lock_init(&common->storage_lock);
}

typedef struct
{
    fcs_dbm_store store;
    unsigned long pre_cache_max_count;
} fcs_dbm__cache_store__common;

#ifdef __cplusplus
}
#endif
