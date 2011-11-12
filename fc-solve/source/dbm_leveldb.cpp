#include <cassert>
#include "leveldb/db.h"

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
