/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2010 Shlomi Fish
 */
// google_hash.cpp - module file for Google's dense_hash_map as adapted
// for Freecell Solver.
#include "dbm_hashtable.h"

#include <apr_hash.h>
#include "wrap_xxhash.h"

static unsigned int hashfunc(const char *key, apr_ssize_t *klen)
{
    return DO_XXH(key, *klen);
}

dict_t fc_solve_kaz_tree_create(dict_comp_t cmp, void *baton,
    meta_allocator *const meta_alloc, void *vunused)
{
    if (!meta_alloc->apr_pool)
    {
        apr_pool_create(&meta_alloc->apr_pool, NULL);
    }
    apr_hash_t *ret = apr_hash_make_custom(meta_alloc->apr_pool, hashfunc);
    return (dict_t)(ret);
}
const size_t siz = sizeof(fcs_dbm_record);
const size_t ksiz = sizeof(fcs_encoded_state_buffer);
dict_ret_key_t fc_solve_kaz_tree_alloc_insert(dict_t v, dict_key_t key_proto)
{
    apr_hash_t *h = (apr_hash_t *)v;
    void *exist_key = apr_hash_get(h, key_proto, ksiz);
    if (exist_key)
    {
        return exist_key;
    }
    void *key = apr_palloc(apr_hash_pool_get(h), siz);
    memcpy(key, key_proto, siz);
    apr_hash_set(h, key, ksiz, key);
    return NULL;
}
dict_key_t fc_solve_kaz_tree_lookup_value(dict_t dict, cdict_key_t key)
{
    apr_hash_t *h = (apr_hash_t *)dict;
    return apr_hash_get(h, key, ksiz);
}
void fc_solve_kaz_tree_destroy(dict_t dict) {}
