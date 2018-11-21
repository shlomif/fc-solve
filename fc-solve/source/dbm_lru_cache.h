/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "generic_tree.h"
#include "fcc_brfs_test.h"

#ifdef FCS_LRU_KEY_IS_STATE
typedef fcs_state_keyval_pair fcs_cache_key;
#else
typedef fcs_encoded_state_buffer fcs_cache_key;
#endif

struct fcs_cache_key_info_struct
{
    fcs_cache_key key;
    fcs_fcc_move *moves_to_key;
    /* lower_pri and higher_pri form a doubly linked list.
     *
     * pri == priority.
     * */
    struct fcs_cache_key_info_struct *lower_pri, *higher_pri;
};

typedef struct fcs_cache_key_info_struct fcs_cache_key_info;

typedef struct
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    Pvoid_t states_values_to_keys_map;
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
    dict_t *kaz_tree;
#else
#error Unknown FCS_RCS_CACHE_STORAGE
#endif
    compact_allocator states_values_to_keys_allocator;
    long count_elements_in_cache, max_num_elements_in_cache;

    fcs_cache_key_info *lowest_pri, *highest_pri, *recycle_bin;
#define RECYCLE_BIN_NEXT(item) ((item)->higher_pri)
    void *tree_recycle_bin;
} fcs_lru_cache;

#ifdef __cplusplus
}
#endif
