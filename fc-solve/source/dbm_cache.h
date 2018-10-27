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
 * dbm_cache.h - contains the implementation of the DBM solver cache routines.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "meta_alloc.h"
#include "freecell-solver/fcs_enums.h"

#include "freecell-solver/fcs_dllexport.h"
#include "dbm_common.h"
#include "delta_states.h"
#include "dbm_calc_derived_iface.h"
#include "dbm_lru_cache.h"

static int fc_solve_compare_lru_cache_keys(const void *const void_a,
    const void *const void_b, void *context GCC_UNUSED)
{
#define GET_PARAM(p) ((((const fcs_cache_key_info *)(p))->key))
    return memcmp(
        &(GET_PARAM(void_a)), &(GET_PARAM(void_b)), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static inline void cache_destroy_key(fcs_cache_key_info *cache_key)
{
    for (; cache_key; cache_key = RECYCLE_BIN_NEXT(cache_key))
    {
        free(cache_key->moves_to_key);
        cache_key->moves_to_key = NULL;
    }
}

static inline void cache_destroy(fcs_lru_cache *cache)
{
    cache_destroy_key(cache->recycle_bin);
    cache_destroy_key(cache->lowest_pri);
    fc_solve_kaz_tree_destroy(cache->kaz_tree);
    fc_solve_compact_allocator_finish(
        &(cache->states_values_to_keys_allocator));
}

static inline void cache_init(fcs_lru_cache *const cache,
    const long max_num_elements_in_cache, meta_allocator *const meta_alloc)
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    cache->states_values_to_keys_map = ((Pvoid_t)NULL);
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
    cache->tree_recycle_bin = NULL;
    cache->kaz_tree = fc_solve_kaz_tree_create(fc_solve_compare_lru_cache_keys,
        NULL, meta_alloc, &(cache->tree_recycle_bin));
#else
#error Unknown FCS_RCS_CACHE_STORAGE
#endif

    fc_solve_compact_allocator_init(
        &(cache->states_values_to_keys_allocator), meta_alloc);
    cache->lowest_pri = NULL;
    cache->highest_pri = NULL;
    cache->recycle_bin = NULL;
    cache->count_elements_in_cache = 0;
    cache->max_num_elements_in_cache = max_num_elements_in_cache;
}

static inline bool cache_does_key_exist(
    fcs_lru_cache *const cache, fcs_cache_key *const key)
{
    const fcs_cache_key_info to_check = {.key = *key};
    const dict_key_t existing_key =
        fc_solve_kaz_tree_lookup_value(cache->kaz_tree, &to_check);
    if (!existing_key)
    {
        return FALSE;
    }
    else
    {
        /* First - promote this key to the top of the cache. */
        fcs_cache_key_info *const existing = (fcs_cache_key_info *)existing_key;

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

static inline fcs_cache_key_info *cache_insert(fcs_lru_cache *cache,
    const fcs_cache_key *key, const fcs_fcc_move *moves_to_parent,
    const fcs_fcc_move final_move)
{
    fcs_cache_key_info *cache_key;
    var_AUTO(kaz_tree, cache->kaz_tree);

    if (cache->count_elements_in_cache >= cache->max_num_elements_in_cache)
    {
        fc_solve_kaz_tree_delete_by_value(
            kaz_tree, (cache_key = cache->lowest_pri));

        cache->lowest_pri = cache->lowest_pri->higher_pri;
        cache->lowest_pri->lower_pri = NULL;
    }
    else
    {
        cache_key = (fcs_cache_key_info *)fcs_compact_alloc_ptr(
            &(cache->states_values_to_keys_allocator), sizeof(*cache_key));
        cache_key->moves_to_key = NULL;
        ++cache->count_elements_in_cache;
    }

    cache_key->key = *key;
    if (moves_to_parent)
    {
        fcs_fcc_move *moves;
        const_AUTO(len, strlen((const char *)moves_to_parent));
        cache_key->moves_to_key = moves =
            SREALLOC(cache_key->moves_to_key, len + 1 + 1);
        memcpy(moves, moves_to_parent, len);
        moves[len] = final_move;
        moves[len + 1] = '\0';
    }
    else if (final_move)
    {
        cache_key->moves_to_key = SREALLOC(cache_key->moves_to_key, 2);
        cache_key->moves_to_key[0] = final_move;
        cache_key->moves_to_key[1] = '\0';
    }
    else
    {
        free(cache_key->moves_to_key);
        cache_key->moves_to_key = NULL;
    }

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

    fc_solve_kaz_tree_alloc_insert(kaz_tree, cache_key);

    return cache_key;
}

#ifdef __cplusplus
}
#endif
