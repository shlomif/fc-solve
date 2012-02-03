#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "dbm_solver.h"
#include "kaz_tree.h"

/*
 * TODO : We waste too much space and fragment it storing the
 * key/parent_move separatley from the dnode_t. We should use a struct
 * for that instead of a pointer.
 * */
typedef struct
{
    fcs_encoded_state_buffer_t key;
    fcs_encoded_state_buffer_t parent_and_move;
} record_t;


typedef struct
{
    dict_t * kaz_tree;
    fcs_compact_allocator_t allocator;
} dbm_t;

/* TODO: make sure the key is '\0'-padded. */
static int compare_records(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) (&(((const record_t *)(p))->key))
    return memcmp(GET_PARAM(void_a), GET_PARAM(void_b), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

void fc_solve_dbm_store_init(fcs_dbm_store_t * store, const char * path)
{
    dbm_t * db;
    db = malloc(sizeof(*db));

    db->kaz_tree =
        fc_solve_kaz_tree_create(compare_records, NULL);

    fc_solve_compact_allocator_init(
        &(db->allocator)
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
    dnode_t * node;

    record_t to_check;
    to_check.key = *(fcs_encoded_state_buffer_t *)key;

    node = fc_solve_kaz_tree_lookup(((dbm_t *)store)->kaz_tree, &to_check);

    if (! node)
    {
        return FALSE;
    }
    else
    {
        *(fcs_encoded_state_buffer_t *)parent_and_move
            = ((record_t *)(node->dict_key))->parent_and_move;
        return TRUE;
    }
}

extern void fc_solve_dbm_store_destroy(fcs_dbm_store_t store)
{
    dbm_t * db;
    db = (dbm_t *)store;

    fc_solve_kaz_tree_destroy( db->kaz_tree );
    fc_solve_compact_allocator_finish( &(db->allocator) );
    free(db);
}
