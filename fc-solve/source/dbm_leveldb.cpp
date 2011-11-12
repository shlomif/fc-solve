#include <cassert>
#include "leveldb/db.h"

#include "dbm_leveldb.h"

extern "C" void fc_solve_dbm_store_init(fcs_dbm_store_t * store, const char * path)
{
    leveldb::DB * db;
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path, &db);
    assert(status.ok());

    *store = (fcs_dbm_store_t)db;
}
