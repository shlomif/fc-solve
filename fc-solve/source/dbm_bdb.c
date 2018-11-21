/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#include <db.h>

#include "dbm_solver.h"

typedef struct
{
    DB *dbp;
    DBT key, data;
} fcs_dbm;

static __attribute__((noreturn)) inline void my_close(DB *const dbp, int ret)
{
    int close_ret;
    if ((close_ret = dbp->close(dbp, 0)) != 0 && ret == 0)
    {
        ret = close_ret;
    }
    exit(ret);
}

void fc_solve_dbm_store_init(fcs_dbm_store *const store, const char *const path,
    void **const recycle_bin_ptr)
{
    fcs_dbm *db = SMALLOC1(db);

    int ret;

    if ((ret = db_create(&(db->dbp), NULL, 0)) != 0)
    {
        fc_solve_err("db_create: %s\n", db_strerror(ret));
    }

    if ((ret = db->dbp->open(
             db->dbp, NULL, path, NULL, DB_BTREE, DB_CREATE, 0664)) != 0)
    {
        db->dbp->err(db->dbp, ret, "%s", path);
        goto err;
    }
    memset(&(db->key), 0, sizeof(db->key));
    memset(&(db->data), 0, sizeof(db->data));
    *store = (fcs_dbm_store)db;
    return;

err:
    my_close(db->dbp, ret);
}

bool fc_solve_dbm_store_does_key_exist(
    fcs_dbm_store store, const unsigned char *const key_raw)
{
    unsigned char dummy[100];

    fcs_dbm *db = (fcs_dbm *)store;
    db->key.data = (unsigned char *)key_raw + 1;
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
        db->dbp->err(db->dbp, ret, "DB->get");
        my_close(db->dbp, ret);
    }
}

bool fc_solve_dbm_store_lookup_parent(fcs_dbm_store store,
    const unsigned char *const key_raw, unsigned char *const parent)
{
    fcs_dbm *db = (fcs_dbm *)store;
    db->key.data = (unsigned char *)key_raw + 1;
    db->key.size = key_raw[0];
    db->data.data = parent + 1;
    db->data.size = sizeof(fcs_encoded_state_buffer) - 1;

    int ret;
    if ((ret = db->dbp->get(db->dbp, NULL, &(db->key), &(db->data), 0)) == 0)
    {
        parent[0] = db->data.size - 1;
        return TRUE;
    }
    else if (ret == DB_NOTFOUND)
    {
        return FALSE;
    }
    else
    {
        db->dbp->err(db->dbp, ret, "DB->get");
        my_close(db->dbp, ret);
    }
}

extern void fc_solve_dbm_store_offload_pre_cache(
    fcs_dbm_store store, fcs_pre_cache *const pre_cache)
{
    fcs_dbm *const db = (fcs_dbm *)store;
    dict_t *const kaz_tree = pre_cache->kaz_tree;
    DB *const dbp = db->dbp;

    for (dnode_t *node = fc_solve_kaz_tree_first(kaz_tree); node;
         node = fc_solve_kaz_tree_next(kaz_tree, node))
    {
        const_AUTO(kv, (pre_cache_key_val_pair *)(node->dict_key));

        db->key.data = kv->key.s + 1;
        db->key.size = kv->key.s[0];
        db->data.data = kv->parent.s + 1;
        db->data.size = kv->parent.s[0];
        int ret;
        if ((ret = dbp->put(dbp, NULL, &(db->key), &(db->data), 0)) != 0)
        {
            dbp->err(dbp, ret, "DB->put");
            my_close(dbp, ret);
        }
    }
}

extern void fc_solve_dbm_store_destroy(fcs_dbm_store store)
{
    fcs_dbm *const db = (fcs_dbm *)store;
    int ret;
    if ((ret = db->dbp->close(db->dbp, 0)) != 0)
    {
        fc_solve_err("DB close failed with ret=%d\n", ret);
    }
    free(db);
}
