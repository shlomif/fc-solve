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
 * dbm_cache.h - contains the implementation of the DBM solver cache routines. 
 *
 */
#ifndef FC_SOLVE_DBM_CACHE_H
#define FC_SOLVE_DBM_CACHE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "config.h"

#include "bool.h"
#include "inline.h"
#include "state.h"
#include "meta_alloc.h"
#include "fcs_enums.h"

#include "fcs_dllexport.h"
#include "dbm_common.h"
#include "dbm_solver_key.h"
#include "dbm_calc_derived_iface.h"
#include "dbm_lru_cache.h"

/* TODO: make sure the key is '\0'-padded. */
static int fc_solve_compare_lru_cache_keys(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((((const fcs_cache_key_info_t *)(p))->key))
    return memcmp(&(GET_PARAM(void_a)), &(GET_PARAM(void_b)), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static GCC_INLINE void cache_destroy(fcs_lru_cache_t * cache)
{
    {
        int i;
#define NUM_CHAINS_TO_RELEASE 2
        fcs_cache_key_info_t * to_release[NUM_CHAINS_TO_RELEASE];

        to_release[0] = cache->recycle_bin;
        to_release[1] = cache->lowest_pri;

        for (i=0 ; i < NUM_CHAINS_TO_RELEASE ; i++)
        {
            fcs_cache_key_info_t * cache_key;

            for (cache_key = to_release[i] ; 
                 cache_key ; 
                 cache_key = RECYCLE_BIN_NEXT(cache_key))
            {
                free(cache_key->moves_to_key);
                cache_key->moves_to_key = NULL;
            }
        }
    }
    fc_solve_kaz_tree_destroy(cache->kaz_tree);
    fc_solve_compact_allocator_finish(&(cache->states_values_to_keys_allocator));
}

static GCC_INLINE void cache_init(fcs_lru_cache_t * cache, long max_num_elements_in_cache, fcs_meta_compact_allocator_t * meta_alloc)
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    cache->states_values_to_keys_map = ((Pvoid_t) NULL);
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
    cache->kaz_tree = fc_solve_kaz_tree_create(fc_solve_compare_lru_cache_keys, NULL, meta_alloc);
#else
#error Unknown FCS_RCS_CACHE_STORAGE
#endif

    fc_solve_compact_allocator_init(
        &(cache->states_values_to_keys_allocator), meta_alloc
    );
    cache->lowest_pri = NULL;
    cache->highest_pri = NULL;
    cache->recycle_bin = NULL;
    cache->count_elements_in_cache = 0;
    cache->max_num_elements_in_cache = max_num_elements_in_cache;
}

static GCC_INLINE fcs_bool_t cache_does_key_exist(fcs_lru_cache_t * cache, fcs_encoded_state_buffer_t * key)
{
    fcs_cache_key_info_t to_check;
    dict_key_t existing_key;

    to_check.key = *key;

    existing_key = fc_solve_kaz_tree_lookup_value(cache->kaz_tree, &to_check);

    if (! existing_key)
    {
        return FALSE;
    }
    else
    {
        /* First - promote this key to the top of the cache. */
        fcs_cache_key_info_t * existing;

        existing = (fcs_cache_key_info_t *)existing_key;

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

static GCC_INLINE fcs_cache_key_info_t * cache_insert(fcs_lru_cache_t * cache, const fcs_encoded_state_buffer_t * key, const fcs_fcc_move_t * moves_to_parent, const fcs_fcc_move_t final_move)
{
    fcs_cache_key_info_t * cache_key;
    dict_t * kaz_tree;

    kaz_tree = cache->kaz_tree;

    if (cache->count_elements_in_cache >= cache->max_num_elements_in_cache)
    {
        fc_solve_kaz_tree_delete_by_value(kaz_tree, (cache_key = cache->lowest_pri));

        cache->lowest_pri = cache->lowest_pri->higher_pri;
        cache->lowest_pri->lower_pri = NULL;
    }
    else
    {
        cache_key =
            (fcs_cache_key_info_t *)
            fcs_compact_alloc_ptr(
                &(cache->states_values_to_keys_allocator),
                sizeof(*cache_key)
            );
        cache_key->moves_to_key = NULL;
        cache->count_elements_in_cache++;
    }

    cache_key->key = *key;
    if (moves_to_parent)
    {
        size_t len;
        fcs_fcc_move_t * moves;
        len = strlen((const char *)moves_to_parent) + 1;
        cache_key->moves_to_key = moves = realloc(cache_key->moves_to_key, len+1);
        memcpy(moves, moves_to_parent, len-1);
        moves[len] = final_move;
        moves[len+1] = '\0';
    }
    else if (final_move)
    {
        cache_key->moves_to_key = realloc(cache_key->moves_to_key, 2);
        cache_key->moves_to_key[0] = final_move;
        cache_key->moves_to_key[1] = '\0';
    }
    else
    {
        free (cache_key->moves_to_key);
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

#endif /*  FC_SOLVE_DBM_CACHE_H */
