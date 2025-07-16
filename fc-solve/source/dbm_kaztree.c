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

typedef struct
{
    fcs_dbm__abstract__states_lookup_t *kaz_tree;
    meta_allocator meta_alloc;
#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
    compact_allocator allocator;
#endif
} fcs_dbm;

void fc_solve_dbm_store_init(fcs_dbm_store *const store,
    const char *path GCC_UNUSED, void **const recycle_bin_ptr)
{
    fcs_dbm *const db = SMALLOC1(db);

    fc_solve_meta_compact_allocator_init(&(db->meta_alloc));

    db->kaz_tree = fcs_dbm__abstract__states_lookup__create(
        compare_records, NULL, &(db->meta_alloc), recycle_bin_ptr);

#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
    fc_solve_compact_allocator_init(&(db->allocator), &(db->meta_alloc));
#endif

    *store = (fcs_dbm_store)db;
}

fcs_dbm__abstract__states_lookup_t *__attribute__((pure))
fc_solve_dbm_store_get_dict(fcs_dbm_store store)
{
    return (((fcs_dbm *)store)->kaz_tree);
}

#ifndef FCS_DBM_USE_DISK_CACHES
// Returns true if the key was added (it didn't already exist.)
bool fc_solve_dbm_store_insert_key_value(
    fcs_dbm_token_type *const ret_token_body, fcs_dbm_store store,
    const fcs_encoded_state_buffer *key, fcs_dbm_store_val parent,
    const bool should_modify_parent GCC_UNUSED)
{
#ifdef FCS_LIBAVL_STORE_WHOLE_KEYS
    fcs_dbm_record record_on_stack;

    // This memset() call is done to please valgrind and for general
    // good measure. It is not absolutely necessary (but should not
    // hurt much). It is needed due to struct padding and alignment
    // issues.
    memset(&record_on_stack, '\0', sizeof(record_on_stack));
#endif

    fcs_dbm *const db = (fcs_dbm *)store;

    fcs_dbm_record *to_check;
#ifdef FCS_LIBAVL_STORE_WHOLE_KEYS
    to_check = &record_on_stack;
#else
    to_check = (fcs_dbm_record *)fcs_compact_alloc_ptr(
        &(db->allocator), sizeof(*to_check));
#endif

#ifdef FCS_DBM_RECORD_POINTER_REPR
    to_check->key = *key;
#ifdef FCS_DBM__VAL_IS_ANCESTOR
    to_check->ancestor = parent;
#else
    fcs_dbm_record_set_parent_ptr(to_check, parent);
#endif

#else
    to_check->key = *key;
    to_check->parent = parent->parent;
#endif
    const bool was_item_inserted_now =
        (fcs_dbm__abstract__states_lookup__alloc_insert(
             db->kaz_tree, to_check) == NULL);

    if (!was_item_inserted_now)
    {
#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
        fcs_compact_alloc_release(&(db->allocator));
#endif
        return false;
    }
#ifndef FCS_DBM__VAL_IS_ANCESTOR
    if (should_modify_parent && parent)
    {
        fcs_dbm_record_increment_refcount(parent);
    }
#endif

    var_AUTO(ret_ptr,
        ((fcs_dbm_record *)(fcs_dbm__abstract__states_lookup__lookup_value(
            db->kaz_tree, to_check))));
#if 0
    printf("fc_solve_dbm_store_insert_key_value ret_ptr=%p\n", ret_ptr);
#endif
    *ret_token_body = (ret_ptr);
    return true;
}

bool fc_solve_dbm_store_lookup_parent(
    fcs_dbm_store store, const unsigned char *key, unsigned char *parent)
{
    fcs_dbm_record to_check = {.key = *(const fcs_encoded_state_buffer *)key};

    dict_key_t existing = fcs_dbm__abstract__states_lookup__lookup_value(
        ((fcs_dbm *)store)->kaz_tree, &to_check);
    if (!existing)
    {
        return false;
    }
#ifdef FCS_DBM__VAL_IS_ANCESTOR
    *((fcs_dbm_store_val *)parent) = ((fcs_dbm_store_val)existing);
#else

#ifdef FCS_DBM_RECORD_POINTER_REPR
    fcs_dbm_record *const p =
        fcs_dbm_record_get_parent_ptr((fcs_dbm_record *)existing);

    if (p)
    {
        *(fcs_encoded_state_buffer *)parent = p->key;
    }
    else
    {
        fcs_init_encoded_state((fcs_encoded_state_buffer *)parent);
    }
#else
    *(fcs_encoded_state_buffer *)parent = ((fcs_dbm_record *)existing)->parent;
#endif
#endif

    return true;
}
#endif

extern void fc_solve_dbm_store_destroy(fcs_dbm_store store)
{
    fcs_dbm *const db = (fcs_dbm *)store;

    fcs_dbm__abstract__states_lookup__destroy(db->kaz_tree);
#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
    fc_solve_compact_allocator_finish(&(db->allocator));
#endif
    fc_solve_meta_compact_allocator_finish(&(db->meta_alloc));
    free(db);
}
