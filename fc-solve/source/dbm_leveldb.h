#ifndef FC_SOLVE_DBM_LEVELDB
#define FC_SOLVE_DBM_LEVELDB

#ifdef __cplusplus
extern "C"
{
#endif

#include "bool.h"
    
typedef void * fcs_dbm_store_t;
void fc_solve_dbm_store_init(fcs_dbm_store_t * store, const char * path);

fcs_bool_t fc_solve_dbm_store_does_key_exist(
    fcs_dbm_store_t store,
    const unsigned char * key_raw
);

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_DBM_LEVELDB */
