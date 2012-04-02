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
    fcs_compact_allocator_t allocator;
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

    fc_solve_compact_allocator_init(
        &(db->allocator), &(db->meta_alloc)
    );

    *store = (fcs_dbm_store_t)db;
    return;
}

/* 
 * Returns TRUE if the key was added (it didn't already exist.)
 * */
fcs_bool_t fc_solve_dbm_store_insert_key_value(
    fcs_dbm_store_t store,
    const fcs_encoded_state_buffer_t * key,
    fcs_encoded_state_buffer_t * parent_and_move
)
{
    dbm_t * db;
    record_t * to_check;
    fcs_bool_t ret;

    db = (dbm_t *)store;

    to_check = (record_t *)fcs_compact_alloc_ptr(&(db->allocator), sizeof(*to_check));

    to_check->key = *key;
    to_check->parent_and_move = *parent_and_move;
    if (! (ret = (fc_solve_kaz_tree_alloc_insert(db->kaz_tree, to_check) == NULL)))
    {
        fcs_compact_alloc_release(&(db->allocator));
    }
    return ret;
}

fcs_bool_t fc_solve_dbm_store_lookup_parent_and_move(
    fcs_dbm_store_t store,
    const unsigned char * key,
    unsigned char * parent_and_move
    )
{
    dict_key_t existing;

    record_t to_check;
    to_check.key = *(const fcs_encoded_state_buffer_t *)key;

    existing = fc_solve_kaz_tree_lookup_value(((dbm_t *)store)->kaz_tree, &to_check);

    if (! existing)
    {
        return FALSE;
    }
    else
    {
        *(fcs_encoded_state_buffer_t *)parent_and_move
            = ((record_t *)existing)->parent_and_move;
        return TRUE;
    }
}

extern void fc_solve_dbm_store_destroy(fcs_dbm_store_t store)
{
    dbm_t * db;
    db = (dbm_t *)store;

    fc_solve_kaz_tree_destroy( db->kaz_tree );
    fc_solve_compact_allocator_finish( &(db->allocator) );
    fc_solve_meta_compact_allocator_finish( &(db->meta_alloc) );
    free(db);
}
