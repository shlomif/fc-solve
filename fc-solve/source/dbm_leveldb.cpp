#include <cassert>
#include "leveldb/db.h"
#include "leveldb/write_batch.h"

#include "dbm_common.h"
#include "dbm_solver.h"
#include "dbm_cache.h"

extern "C" void fc_solve_dbm_store_init(fcs_dbm_store_t * store, const char * path)
{
    leveldb::DB * db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path, &db);
    assert(status.ok());

    *store = (fcs_dbm_store_t)db;
}

extern "C" fcs_bool_t fc_solve_dbm_store_does_key_exist(
    fcs_dbm_store_t store,
    const unsigned char * key_raw
)
{
    leveldb::Slice key((const char *)(key_raw+1),key_raw[0]);
    std::string value;

    return
        ((leveldb::DB *)store)->Get(leveldb::ReadOptions(), key, &value).ok();
}

fcs_bool_t fc_solve_dbm_store_lookup_parent_and_move(
    fcs_dbm_store_t store,
    const unsigned char * key_raw,
    unsigned char * parent_and_move
    )
{
    leveldb::Slice key((const char *)(key_raw+1),key_raw[0]);
    std::string value;

    leveldb::Status status =
        ((leveldb::DB *)store)->Get(leveldb::ReadOptions(), key, &value);

    if (status.ok())
    {
        memcpy(
            parent_and_move+1,
            value.data()+1,
            (parent_and_move[0] = value.length()-1)+1
        );

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#define MAX_ITEMS_IN_TRANSACTION 10000
extern "C" void fc_solve_dbm_store_offload_pre_cache(
    fcs_dbm_store_t store,
    fcs_pre_cache_t * pre_cache
)
{
    leveldb::DB * db;
#ifdef FCS_DBM_USE_LIBAVL
    struct avl_traverser trav;
    dict_key_t item;
#else
    dnode_t * node;
#endif
    dict_t * kaz_tree;
    int num_left_in_transaction = MAX_ITEMS_IN_TRANSACTION;
    leveldb::WriteBatch batch;
    fcs_pre_cache_key_val_pair_t * kv;

    db = (leveldb::DB *)store;
    kaz_tree = pre_cache->kaz_tree;

#ifdef FCS_DBM_USE_LIBAVL
    avl_t_init(&trav, kaz_tree);
#endif

#ifdef FCS_DBM_USE_LIBAVL
    for (
        item = avl_t_first(&trav, kaz_tree)
            ;
        item
            ;
        item = avl_t_next(&trav)
        )
#else
    for (node = fc_solve_kaz_tree_first(kaz_tree);
            node ;
            node = fc_solve_kaz_tree_next(kaz_tree, node)
            )
#define item (node->dict_key)
#endif
    {
        kv = (fcs_pre_cache_key_val_pair_t *)(item);

    leveldb::Slice key((const char *)(kv->key.s+1),kv->key.s[0]);
    /* We add 1 to the parent and move's length because it includes the
     * trailing one-char move.
     * */
    leveldb::Slice parent_and_move((const char *)(kv->parent_and_move.s+1),kv->parent_and_move.s[0]+1);
        batch.Put(key, parent_and_move);

        if ((--num_left_in_transaction) <= 0)
        {
#define WRITE() assert(db->Write(leveldb::WriteOptions(), &batch).ok())
            WRITE();
            batch.Clear();
            num_left_in_transaction = MAX_ITEMS_IN_TRANSACTION;
        }
    }
    WRITE();
#undef WRITE
}
#ifndef FCS_DBM_USE_LIBAVL
#undef item
#endif

extern "C" void fc_solve_dbm_store_destroy(fcs_dbm_store_t store)
{
    delete ((leveldb::DB *)store);
}
