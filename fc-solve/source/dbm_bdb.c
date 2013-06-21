#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <db.h>

#include "dbm_solver.h"

typedef struct
{
    DB * dbp;
    DBT key, data;
} dbm_t;

void fc_solve_dbm_store_init(fcs_dbm_store_t * store, const char * path)
{
    dbm_t * db = SMALLOC1(db);

    int ret;

    if ((ret = db_create(&(db->dbp), NULL, 0)) != 0)
    {
        fprintf(stderr, "db_create: %s\n", db_strerror(ret));
        exit (1);
    }

    if ((ret = db->dbp->open(db->dbp,
        NULL, path, NULL, DB_BTREE, DB_CREATE, 0664))
            != 0)
    {
        db->dbp->err(db->dbp, ret, "%s", path);
        goto err;
    }
    memset(&(db->key), 0, sizeof(db->key));
    memset(&(db->data), 0, sizeof(db->data));
    *store = (fcs_dbm_store_t)db;
    return;

    err:
    int t_ret;
    if ((t_ret = db->dbp->close(db->dbp, 0)) != 0 && ret == 0)
    {
        ret = t_ret;
    }
    exit(ret);
}

fcs_bool_t fc_solve_dbm_store_does_key_exist(
    fcs_dbm_store_t store,
    const unsigned char * key_raw
)
{
    unsigned char dummy[100];

    dbm_t * db = (dbm_t *)store;
    db->key.data = (unsigned char *)key_raw+1;
    db->key.size = key_raw[0];
    db->data.data = dummy;
    db->data.size = sizeof(dummy);

    int ret;
    if ((ret = db->dbp->get(db->dbp, NULL, &(db->key), &(db->data), 0)) == 0)
    {
        return TRUE;
    }
    else if (ret == DB_NOTFOUND)
    {
        return FALSE;
    }
    else
    {
        int t_ret;
        db->dbp->err(db->dbp, ret, "DB->get");
        if ((t_ret = db->dbp->close(db->dbp, 0)) != 0 && ret == 0)
        {
            ret = t_ret;
        }
        exit(ret);
    }
}

fcs_bool_t fc_solve_dbm_store_lookup_parent(
    fcs_dbm_store_t store,
    const unsigned char * key_raw,
    unsigned char * parent
)
{
    dbm_t * db = (dbm_t *)store;
    db->key.data = (unsigned char *)key_raw+1;
    db->key.size = key_raw[0];
    db->data.data = parent+1;
    db->data.size = sizeof(fcs_encoded_state_buffer_t)-1;

    int ret;
    if ((ret = db->dbp->get(db->dbp, NULL, &(db->key), &(db->data), 0)) == 0)
    {
        parent[0] = db->data.size-1;
        return TRUE;
    }
    else if (ret == DB_NOTFOUND)
    {
        return FALSE;
    }
    else
    {
        db->dbp->err(db->dbp, ret, "DB->get");
        if ((int t_ret = db->dbp->close(db->dbp, 0)) != 0 && ret == 0)
        {
            ret = t_ret;
        }
        exit(ret);
    }
}

#define MAX_ITEMS_IN_TRANSACTION 10000
extern void fc_solve_dbm_store_offload_pre_cache(
    fcs_dbm_store_t store,
    fcs_pre_cache_t * pre_cache
)
{
    dbm_t * const db = (dbm_t *)store;
    dict_t * const kaz_tree = pre_cache->kaz_tree;
    DB * const dbp = db->dbp;

    for (dnode_t * node = fc_solve_kaz_tree_first(kaz_tree);
            node ;
            node = fc_solve_kaz_tree_next(kaz_tree, node)
            )
    {
        fcs_pre_cache_key_val_pair_t * const kv =
            (fcs_pre_cache_key_val_pair_t *)(node->dict_key);

        db->key.data = kv->key.s+1;
        db->key.size = kv->key.s[0];
        db->data.data = kv->parent.s+1;
        db->data.size = kv->parent.s[0];
        if ((int ret = dbp->put(dbp, NULL, &(db->key), &(db->data), 0)) != 0)
        {
            dbp->err(dbp, ret, "DB->put");
            if ((int t_ret = dbp->close(dbp, 0)) != 0 && ret == 0)
            {
                ret = t_ret;
            }
            exit (ret);
        }
    }
}

extern void fc_solve_dbm_store_destroy(fcs_dbm_store_t store)
{
    dbm_t * const db = (dbm_t *)store;
    if ((int ret = db->dbp->close(db->dbp, 0)) != 0)
    {
        fprintf(stderr, "DB close failed with ret=%d\n", ret);
        exit(-1);
    }
    free(db);
}
