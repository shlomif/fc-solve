// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
#include "dbm_solver.h"
#include "generic_tree.h"
#include "dbm_kaztree_compare.h"

#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
#error This code assumes FCS_LIBAVL_STORE_WHOLE_KEYS = 1
#endif

typedef struct
{
    dict_t *kaz_tree;
    meta_allocator meta_alloc;
} fcs_dbm;

void fc_solve_dbm_store_init(fcs_dbm_store *const store,
    const char *path GCC_UNUSED, void **const recycle_bin_ptr)
{
    fcs_dbm *const db = SMALLOC1(db);

    fc_solve_meta_compact_allocator_init(&(db->meta_alloc));

    db->kaz_tree = fc_solve_kaz_tree_create(
        fcs_dbm__compare_records, NULL, &(db->meta_alloc), recycle_bin_ptr);

    *store = (fcs_dbm_store)db;
}

dict_t *__attribute__((pure)) fc_solve_dbm_store_get_dict(fcs_dbm_store store)
{
    return (((fcs_dbm *)store)->kaz_tree);
}

// Returns true if the key was added (it didn't already exist.)
fcs_dbm_record *fc_solve_dbm_store_insert_key_value(fcs_dbm_store store,
    const fcs_encoded_state_buffer *key,
    const fcs_encoded_state_buffer raw_parent)
{
    fcs_dbm_record record_on_stack;

    // This memset() call is done to please valgrind and for general
    // good measure. It is not absolutely necessary (but should not
    // hurt much). It is needed due to struct padding and alignment
    // issues.
    memset(&record_on_stack, '\0', sizeof(record_on_stack));

    fcs_dbm *const db = (fcs_dbm *)store;

    fcs_dbm_record *to_check = &record_on_stack;

    to_check->key = *key;
    to_check->parent = raw_parent;

    const bool was_item_inserted_now =
        (fc_solve_kaz_tree_alloc_insert(db->kaz_tree, to_check) == NULL);

    if (!was_item_inserted_now)
    {
        return NULL;
    }

    var_AUTO(ret_ptr, ((fcs_dbm_record *)(fc_solve_kaz_tree_lookup_value(
                          db->kaz_tree, to_check))));
    return ret_ptr;
}

bool fc_solve_dbm_store_lookup_parent(
    fcs_dbm_store store, const unsigned char *key, unsigned char *parent)
{
    fcs_dbm_record to_check = {.key = *(const fcs_encoded_state_buffer *)key};

    dict_key_t existing =
        fc_solve_kaz_tree_lookup_value(((fcs_dbm *)store)->kaz_tree, &to_check);
    if (!existing)
    {
        return false;
    }
    *(fcs_encoded_state_buffer *)parent = ((fcs_dbm_record *)existing)->parent;

    return true;
}

extern void fc_solve_dbm_store_destroy(fcs_dbm_store store)
{
    fcs_dbm *const db = (fcs_dbm *)store;

    fc_solve_kaz_tree_destroy(db->kaz_tree);
    fc_solve_meta_compact_allocator_finish(&(db->meta_alloc));
    free(db);
}
