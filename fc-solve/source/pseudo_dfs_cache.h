/* Copyright (c) 2012 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * pseudo_dfs_cache.h - contains the implementation of the cache for
 * the pseudo_dfs solver.
 *
 */
#ifndef FC_SOLVE_PSEUDO_DFS_CACHE_H
#define FC_SOLVE_PSEUDO_DFS_CACHE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "config.h"

#include <Judy.h>

#include "bool.h"
#include "inline.h"
#include "alloc_wrap.h"

#include "state.h"
#include "meta_alloc.h"
#include "fcs_enums.h"

#include "fcs_dllexport.h"
#include "dbm_common.h"
#include "dbm_solver_key.h"
#include "dbm_calc_derived_iface.h"

typedef fcs_state_t fcs_pdfs_key_t;

typedef struct fcs_pdfs_key_info_struct
{
    fcs_pdfs_key_t key;
    /* lower_pri and higher_pri form a doubly linked list.
     *
     * pri == priority.
     * */
    struct fcs_pdfs_key_info_struct * lower_pri, * higher_pri;
} fcs_pdfs_cache_key_info_t;

typedef struct
{
    Pvoid_t states_values_to_keys_map;
    fcs_compact_allocator_t states_values_to_keys_allocator;
    long count_elements_in_cache, max_num_elements_in_cache;

    fcs_pdfs_cache_key_info_t * lowest_pri, * highest_pri;

#define RECYCLE_BIN_NEXT(el) ((el)->higher_pri)
    fcs_pdfs_cache_key_info_t * recycle_bin;
} fcs_pdfs_lru_cache_t;

static GCC_INLINE void fcs_pdfs_cache_destroy(fcs_pdfs_lru_cache_t * const cache)
{
    Word_t Rc_word;
    JHSFA(Rc_word, cache->states_values_to_keys_map);
    fc_solve_compact_allocator_finish(&(cache->states_values_to_keys_allocator));
}

static GCC_INLINE void fcs_pdfs_cache_init(fcs_pdfs_lru_cache_t * const cache, const long max_num_elements_in_cache, fcs_meta_compact_allocator_t * const meta_alloc)
{
    cache->states_values_to_keys_map = ((Pvoid_t) NULL);

    fc_solve_compact_allocator_init(
        &(cache->states_values_to_keys_allocator), meta_alloc
    );
    cache->lowest_pri = NULL;
    cache->highest_pri = NULL;
    cache->recycle_bin = NULL;
    cache->count_elements_in_cache = 0;
    cache->max_num_elements_in_cache = max_num_elements_in_cache;
}

static GCC_INLINE const fcs_bool_t fcs_pdfs_cache_does_key_exist(fcs_pdfs_lru_cache_t * const cache, fcs_pdfs_key_t * const key)
{
    fcs_pdfs_cache_key_info_t * existing;

    Word_t * PValue;
    JHSG(PValue, cache->states_values_to_keys_map, key, sizeof(*key));
    if (! PValue)
    {
        existing = NULL;
    }
    else
    {
        existing = (typeof(existing))(*PValue);
    }

    if (! existing)
    {
        return FALSE;
    }
    else
    {
        /* First - promote this key to the top of the cache. */
        if (existing->higher_pri)
        {
            existing->higher_pri->lower_pri =
                existing->lower_pri;
            if (existing->lower_pri)
            {
                existing->lower_pri->higher_pri =
                    existing->higher_pri;
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

static GCC_INLINE fcs_pdfs_cache_key_info_t * fcs_pdfs_cache_insert(fcs_pdfs_lru_cache_t * const cache, fcs_pdfs_key_t * const key)
{
    fcs_pdfs_cache_key_info_t * cache_key;
    if (cache->count_elements_in_cache >= cache->max_num_elements_in_cache)
    {
        int Rc_int;
        cache_key = cache->lowest_pri;
        JHSD( Rc_int, cache->states_values_to_keys_map, &(cache_key->key), sizeof(cache_key->key));

        cache->lowest_pri = cache->lowest_pri->higher_pri;
        cache->lowest_pri->lower_pri = NULL;
    }
    else
    {
        cache_key =
            (fcs_pdfs_cache_key_info_t *)
            fcs_compact_alloc_ptr(
                &(cache->states_values_to_keys_allocator),
                sizeof(*cache_key)
            );
        cache->count_elements_in_cache++;
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

    Word_t * PValue;
    JHSI(PValue, cache->states_values_to_keys_map, key, sizeof(*key));
    *PValue = ((Word_t)cache_key);

    return cache_key;
}

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_PSEUDO_DFS_CACHE_H */
