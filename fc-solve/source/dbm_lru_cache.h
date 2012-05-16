#ifndef FC_SOLVE_DBM_LRU_CACHE_H
#define FC_SOLVE_DBM_LRU_CACHE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "generic_tree.h"
#include "fcc_brfs_test.h"

struct fcs_cache_key_info_struct
{
    fcs_encoded_state_buffer_t key;
    fcs_fcc_move_t * moves_to_key;
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

#define RECYCLE_BIN_NEXT(el) ((el)->higher_pri)
    fcs_cache_key_info_t * recycle_bin;
} fcs_lru_cache_t;

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE_DBM_LRU_CACHE_H */
