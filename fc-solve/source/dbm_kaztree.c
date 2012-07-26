#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "dbm_solver.h"
#include "generic_tree.h"

#include "dbm_kaztree_compare.h"

typedef struct
{
    dict_t * kaz_tree;
    fcs_meta_compact_allocator_t meta_alloc;
#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
    fcs_compact_allocator_t allocator;
#endif
} dbm_t;

void fc_solve_dbm_store_init(fcs_dbm_store_t * store, const char * path)
{
    dbm_t * db;
    db = malloc(sizeof(*db));

    fc_solve_meta_compact_allocator_init(
        &(db->meta_alloc)
    );

    db->kaz_tree =
        fc_solve_kaz_tree_create(compare_records, NULL, &(db->meta_alloc));

#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
    fc_solve_compact_allocator_init(
        &(db->allocator), &(db->meta_alloc)
    );
#endif

    *store = (fcs_dbm_store_t)db;
    return;
}

/*
 * Returns TRUE if the key was added (it didn't already exist.)
 * */
const unsigned char * fc_solve_dbm_store_insert_key_value(
    fcs_dbm_store_t store,
    const fcs_encoded_state_buffer_t * key,
    fcs_encoded_state_buffer_t * parent
)
{
    dbm_t * db;
    fcs_dbm_record_t * to_check;
    fcs_bool_t ret;
#ifdef FCS_LIBAVL_STORE_WHOLE_KEYS
    fcs_dbm_record_t record_on_stack;
#endif

    db = (dbm_t *)store;

#ifdef FCS_LIBAVL_STORE_WHOLE_KEYS
    to_check = &record_on_stack;
#else
    to_check = (fcs_dbm_record_t *)fcs_compact_alloc_ptr(&(db->allocator), sizeof(*to_check));
#endif

#ifdef FCS_DBM_RECORD_POINTER_REPR
    to_check->key = *key;
    {
        fcs_dbm_record_t parent_to_check;

        parent_to_check.key = *parent;
        fcs_dbm_record_set_parent_ptr(to_check, (fcs_dbm_record_t *)fc_solve_kaz_tree_lookup_value(((dbm_t *)store)->kaz_tree, &parent_to_check));
    }
#else
    to_check->key = *key;
    to_check->parent = *parent;
#endif
    ret = (fc_solve_kaz_tree_alloc_insert(db->kaz_tree, to_check) == NULL);

#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
    if (! ret)
    {
        fcs_compact_alloc_release(&(db->allocator));
    }
#endif
    if (ret)
    {
        return (unsigned char *)&(((fcs_dbm_record_t *)(fc_solve_kaz_tree_lookup_value(db->kaz_tree, to_check)))->key);
    }
    else
    {
        return NULL;
    }
}

fcs_bool_t fc_solve_dbm_store_lookup_parent(
    fcs_dbm_store_t store,
    const unsigned char * key,
    unsigned char * parent
    )
{
    dict_key_t existing;

    fcs_dbm_record_t to_check;
    to_check.key = *(const fcs_encoded_state_buffer_t *)key;

    existing = fc_solve_kaz_tree_lookup_value(((dbm_t *)store)->kaz_tree, &to_check);

    if (! existing)
    {
        return FALSE;
    }
    else
    {
#ifdef FCS_DBM_RECORD_POINTER_REPR
        fcs_dbm_record_t * p = fcs_dbm_record_get_parent_ptr((fcs_dbm_record_t *)existing);

        if (p)
        {
            *(fcs_encoded_state_buffer_t *)parent
                = p->key;
        }
        else
        {
            fcs_init_encoded_state((fcs_encoded_state_buffer_t *)parent);
        }
#else
        *(fcs_encoded_state_buffer_t *)parent
            = ((fcs_dbm_record_t *)existing)->parent;
#endif
        return TRUE;
    }
}

extern void fc_solve_dbm_store_destroy(fcs_dbm_store_t store)
{
    dbm_t * db;
    db = (dbm_t *)store;

    fc_solve_kaz_tree_destroy( db->kaz_tree );
#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
    fc_solve_compact_allocator_finish( &(db->allocator) );
#endif
    fc_solve_meta_compact_allocator_finish( &(db->meta_alloc) );
    free(db);
}
