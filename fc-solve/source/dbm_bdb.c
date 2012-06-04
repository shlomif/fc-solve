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
    dbm_t * db;
    int t_ret, ret;

    db = malloc(sizeof(*db));

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
    dbm_t * db;
    int ret, t_ret;
    unsigned char dummy[100];

    db = (dbm_t *)store;
    db->key.data = (unsigned char *)key_raw+1;
    db->key.size = key_raw[0];
    db->data.data = dummy;
    db->data.size = sizeof(dummy);

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
        db->dbp->err(db->dbp, ret, "DB->get");
        if ((t_ret = db->dbp->close(db->dbp, 0)) != 0 && ret == 0)
        {
            ret = t_ret;
        }
        exit(ret);
    }
}

fcs_bool_t fc_solve_dbm_store_lookup_parent_and_move(
    fcs_dbm_store_t store,
    const unsigned char * key_raw,
    unsigned char * parent_and_move
)
{
    dbm_t * db;
    int ret, t_ret;

    db = (dbm_t *)store;
    db->key.data = (unsigned char *)key_raw+1;
    db->key.size = key_raw[0];
    db->data.data = parent_and_move+1;
    db->data.size = sizeof(fcs_encoded_state_buffer_t)-1;

    if ((ret = db->dbp->get(db->dbp, NULL, &(db->key), &(db->data), 0)) == 0)
    {
        parent_and_move[0] = db->data.size-1;
        return TRUE;
    }
    else if (ret == DB_NOTFOUND)
    {
        return FALSE;
    }
    else
    {
        db->dbp->err(db->dbp, ret, "DB->get");
        if ((t_ret = db->dbp->close(db->dbp, 0)) != 0 && ret == 0)
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
    dbm_t * db;
    dnode_t * node;
    dict_t * kaz_tree;
    fcs_pre_cache_key_val_pair_t * kv;
    DB * dbp;
    int ret, t_ret;

    db = (dbm_t *)store;
    kaz_tree = pre_cache->kaz_tree;
    dbp = db->dbp;

    for (node = fc_solve_kaz_tree_first(kaz_tree);
            node ;
            node = fc_solve_kaz_tree_next(kaz_tree, node)
            )
    {
        kv = (fcs_pre_cache_key_val_pair_t *)(node->dict_key);

        db->key.data = kv->key.s+1;
        db->key.size = kv->key.s[0];
        /* We add 1 to the parent and move's length because it includes the
         * trailing one-char move.
         * */
        db->data.data = kv->parent_and_move.s+1;
        db->data.size = kv->parent_and_move.s[0]+1;
        if ((ret = dbp->put(dbp, NULL, &(db->key), &(db->data), 0)) != 0)
        {
            dbp->err(dbp, ret, "DB->put");
            if ((t_ret = dbp->close(dbp, 0)) != 0 && ret == 0)
            {
                ret = t_ret;
            }
            exit(ret);
        }
    }
}

extern void fc_solve_dbm_store_destroy(fcs_dbm_store_t store)
{
    dbm_t * db;
    db = (dbm_t *)store;
    int ret;

    if ((ret = db->dbp->close(db->dbp, 0)) != 0)
    {
        fprintf(stderr, "DB close failed with ret=%d\n", ret);
        exit(-1);
    }
    free(db);
}
