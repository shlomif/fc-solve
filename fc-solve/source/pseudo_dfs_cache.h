/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
/*
 * pseudo_dfs_cache.h - contains the implementation of the cache for
 * the pseudo_dfs solver.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <Judy.h>
#include "meta_alloc.h"
#include "freecell-solver/fcs_enums.h"
#include "dbm_common.h"
#include "delta_states.h"
#include "dbm_calc_derived_iface.h"

typedef fcs_encoded_state_buffer fcs_pdfs_key;

typedef struct fcs_pdfs_key_info_struct
{
    fcs_pdfs_key key;
    /* lower_pri and higher_pri form a doubly linked list.
     *
     * pri == priority.
     * */
    struct fcs_pdfs_key_info_struct *lower_pri, *higher_pri;
} fcs_pdfs_cache_key_info;

typedef struct
{
    Pvoid_t states_values_to_keys_map;
    compact_allocator states_values_to_keys_allocator;
    long count_elements_in_cache, max_num_elements_in_cache;
    fcs_pdfs_cache_key_info *lowest_pri, *highest_pri;
#define RECYCLE_BIN_NEXT(item) ((item)->higher_pri)
    fcs_pdfs_cache_key_info *recycle_bin;
} fcs_pseudo_dfs_lru_cache;

static inline void fcs_pdfs_cache_destroy(fcs_pseudo_dfs_lru_cache *const cache)
{
    Word_t rc_word;
    JHSFA(rc_word, cache->states_values_to_keys_map);
    fc_solve_compact_allocator_finish(
        &(cache->states_values_to_keys_allocator));
}

static inline void fcs_pdfs_cache_init(fcs_pseudo_dfs_lru_cache *const cache,
    const long max_num_elements_in_cache, meta_allocator *const meta_alloc)
{
    cache->states_values_to_keys_map = ((Pvoid_t)NULL);

    fc_solve_compact_allocator_init(
        &(cache->states_values_to_keys_allocator), meta_alloc);
    cache->lowest_pri = NULL;
    cache->highest_pri = NULL;
    cache->recycle_bin = NULL;
    cache->count_elements_in_cache = 0;
    cache->max_num_elements_in_cache = max_num_elements_in_cache;
}

static inline bool fcs_pdfs_cache_does_key_exist(
    fcs_pseudo_dfs_lru_cache *const cache, fcs_pdfs_key *const key)
{
    fcs_pdfs_cache_key_info *existing;

    Word_t *PValue;
    JHSG(PValue, cache->states_values_to_keys_map, key, sizeof(*key));
    if (!PValue)
    {
        existing = NULL;
    }
    else
    {
        existing = (typeof(existing))(*PValue);
    }

    if (!existing)
    {
        return FALSE;
    }
    else
    {
        /* First - promote this key to the top of the cache. */
        if (existing->higher_pri)
        {
            existing->higher_pri->lower_pri = existing->lower_pri;
            if (existing->lower_pri)
            {
                existing->lower_pri->higher_pri = existing->higher_pri;
            }
            else
            {
                cache->lowest_pri = existing->higher_pri;
                /* Bug fix: keep the chain intact. */
                existing->higher_pri->lower_pri = NULL;
            }
            cache->highest_pri->higher_pri = existing;
            existing->lower_pri = cache->highest_pri;
            cache->highest_pri = existing;
            existing->higher_pri = NULL;
        }

        return TRUE;
    }
}

static inline fcs_pdfs_cache_key_info *fcs_pdfs_cache_insert(
    fcs_pseudo_dfs_lru_cache *const cache, fcs_pdfs_key *const key)
{
    fcs_pdfs_cache_key_info *cache_key;
    if (cache->count_elements_in_cache >= cache->max_num_elements_in_cache)
    {
        cache_key = cache->lowest_pri;
        int rc_int;
        JHSD(rc_int, cache->states_values_to_keys_map, &(cache_key->key),
            sizeof(cache_key->key));

        cache->lowest_pri = cache->lowest_pri->higher_pri;
        cache->lowest_pri->lower_pri = NULL;
    }
    else
    {
        cache_key = (fcs_pdfs_cache_key_info *)fcs_compact_alloc_ptr(
            &(cache->states_values_to_keys_allocator), sizeof(*cache_key));
        ++cache->count_elements_in_cache;
    }

    cache_key->key = *key;

    if (cache->highest_pri)
    {
        cache_key->lower_pri = cache->highest_pri;
        cache_key->higher_pri = NULL;
        cache->highest_pri->higher_pri = cache_key;
        cache->highest_pri = cache_key;
    }
    else
    {
        cache->highest_pri = cache->lowest_pri = cache_key;
        cache_key->higher_pri = cache_key->lower_pri = NULL;
    }

    Word_t *PValue;
    JHSI(PValue, cache->states_values_to_keys_map, key, sizeof(*key));
    *PValue = ((Word_t)cache_key);

    return cache_key;
}

#ifdef __cplusplus
}
#endif
