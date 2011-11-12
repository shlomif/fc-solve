#include <cassert>
#include "leveldb/db.h"
#include "leveldb/write_batch.h"

#include "dbm_solver.h"

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

#define MAX_ITEMS_IN_TRANSACTION 10000
void fc_solve_dbm_store_offload_pre_cache(
    fcs_dbm_store_t store,
    fcs_pre_cache_t * pre_cache
)
{
    leveldb::DB * db;
    dnode_t * node;
    dict_t * kaz_tree;
    int num_left_in_transaction = MAX_ITEMS_IN_TRANSACTION;
    leveldb::WriteBatch batch;
    fcs_pre_cache_key_val_pair_t * kv;

    db = (leveldb::DB *)store;
    kaz_tree = pre_cache->kaz_tree;

    for (node = fc_solve_kaz_tree_first(kaz_tree); 
            node ; 
            node = fc_solve_kaz_tree_next(kaz_tree, node)
            )
    {
        kv = (fcs_pre_cache_key_val_pair_t *)(node->dict_key);

        leveldb::Slice key((const char *)(kv->key+1),kv->key[0]);
        /* We add 1 to the parent and move's length because it includes the 
         * trailing one-char move.
         * */
        leveldb::Slice parent_and_move((const char *)(kv->parent_and_move+1),kv->parent_and_move[0]+1);
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

