#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "dbm_solver_key.h"
#define FCS_KAZ_TREE_USE_RECORD_DICT_KEY
#include "dbm_solver.h"
#include "kaz_tree.h"
#include "kaz_tree.c"

/*
 * TODO : We waste too much space and fragment it storing the
 * key/parent_move separatley from the dnode_t. We should use a struct
 * for that instead of a pointer.
 * */

typedef dict_t dbm_t;

/* TODO: make sure the key is '\0'-padded. */
static int compare_records(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((((const fcs_dbm_record_t *)(p))->key))
    return memcmp(GET_PARAM(void_a), GET_PARAM(void_b), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

void fc_solve_dbm_store_init(fcs_dbm_store_t * store, const char * path)
{
    *store = (fcs_dbm_store_t)fc_solve_kaz_tree_create(compare_records, NULL);
    return;
}

/* 
 * Returns TRUE if the key was added (it didn't already exist.)
 * */
fcs_bool_t fc_solve_dbm_store_insert_key_value(
    fcs_dbm_store_t store,
    const unsigned char * key,
    unsigned char * parent_and_move
)
{
    fcs_dbm_record_t to_check;

    memcpy(to_check.key, key, sizeof(to_check.key));
    memcpy(to_check.parent_and_move, parent_and_move, sizeof(to_check.parent_and_move));
    return (fc_solve_kaz_tree_alloc_insert((dbm_t *)store, to_check) == NULL);
}

fcs_bool_t fc_solve_dbm_store_lookup_parent_and_move(
    fcs_dbm_store_t store,
    const unsigned char * key,
    unsigned char * parent_and_move
    )
{
    dnode_t * node;

    fcs_dbm_record_t to_check;
    memcpy(to_check.key, key, sizeof(to_check.key));

    node = fc_solve_kaz_tree_lookup(((dbm_t *)store), to_check);

    if (! node)
    {
        return FALSE;
    }
    else
    {
        memcpy(
            parent_and_move,
            ((node->dict_key)).parent_and_move,
            sizeof (((node->dict_key)).parent_and_move)
        );
        return TRUE;
    }
}

extern void fc_solve_dbm_store_destroy(fcs_dbm_store_t store)
{
    fc_solve_kaz_tree_destroy( (dbm_t *)store);
}
