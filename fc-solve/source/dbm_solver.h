#ifndef FC_SOLVE_DBM_SOLVER_H
#define FC_SOLVE_DBM_SOLVER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "config.h"

#include "bool.h"
#include "inline.h"
#include "kaz_tree.h"

typedef unsigned char fcs_encoded_state_buffer_t[24];

struct fcs_cache_key_info_struct
{
    fcs_encoded_state_buffer_t key;
    /* lower_pri and higher_pri form a doubly linked list.
     *
     * pri == priority.
     * */
    struct fcs_cache_key_info_struct * lower_pri, * higher_pri;
};

typedef struct fcs_cache_key_info_struct fcs_cache_key_info_t;

typedef struct
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    Pvoid_t states_values_to_keys_map;
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
    dict_t * kaz_tree;
#else
#error unknown FCS_RCS_CACHE_STORAGE
#endif
    fcs_compact_allocator_t states_values_to_keys_allocator;
    long count_elements_in_cache, max_num_elements_in_cache;

    fcs_cache_key_info_t * lowest_pri, * highest_pri;

    fcs_cache_key_info_t * recycle_bin;
} fcs_lru_cache_t;

union fcs_pre_cache_key_val_pair_struct
{
    struct {
        fcs_encoded_state_buffer_t key;
        fcs_encoded_state_buffer_t parent_and_move;
    };
    union fcs_pre_cache_key_val_pair_struct * next;
};

typedef union fcs_pre_cache_key_val_pair_struct fcs_pre_cache_key_val_pair_t;

typedef struct {
    dict_t * kaz_tree;
    fcs_compact_allocator_t kv_allocator;
    fcs_pre_cache_key_val_pair_t * kv_recycle_bin;
    long count_elements;
} fcs_pre_cache_t;

typedef void * fcs_dbm_store_t;
void fc_solve_dbm_store_init(fcs_dbm_store_t * store, const char * path);

fcs_bool_t fc_solve_dbm_store_does_key_exist(
    fcs_dbm_store_t store,
    const unsigned char * key_raw
);

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_DBM_SOLVER_H */
