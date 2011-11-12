#ifndef FC_SOLVE_DBM_LEVELDB
#define FC_SOLVE_DBM_LEVELDB

#ifdef __cplusplus
extern "C"
{
#endif

typedef void * fcs_dbm_store_t;
void fc_solve_dbm_store_init(fcs_dbm_store_t * store, const char * path);

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_DBM_LEVELDB */
