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
#include <algorithm>
#include "dbm_google_hash.h"

#include <apr_hash.h>
typedef unsigned long int ub4; /* unsigned 4-byte quantities */
typedef unsigned char ub1;

static inline ub4 perl_hash_function(register const ub1 *s_ptr, /* the key */
    register const ub4 length /* the length of the key */
)
{
    register ub4 hash_value_int = 0;
    register const ub1 *s_end = s_ptr + length;

    while (s_ptr < s_end)
    {
        hash_value_int += (hash_value_int << 5) + *(s_ptr++);
    }
    hash_value_int += (hash_value_int >> 5);

    return hash_value_int;
}

extern "C" dict_t fc_solve_kaz_tree_create(
    dict_comp_t cmp, void *baton, meta_allocator *, void *)
{
    apr_pool_t *pool;
    apr_pool_create(&pool, NULL);
    apr_hash_t *ret = apr_hash_make(pool);
    return (dict_t)(ret);
}
const auto siz = sizeof(fcs_dbm_record);
extern "C" dict_ret_key_t fc_solve_kaz_tree_alloc_insert(
    dict_t v, dict_key_t key_proto)
{
    apr_hash_t *h = (apr_hash_t *)v;
    void *key = apr_palloc(apr_hash_pool_get(h), siz);
    memcpy(key, key_proto, siz);
    apr_hash_set(h, key, siz, key);
    return key;
}
extern "C" dict_key_t fc_solve_kaz_tree_lookup_value(
    dict_t dict, cdict_key_t key)
{
    apr_hash_t *h = (apr_hash_t *)dict;
    return apr_hash_get(h, key, siz);
}
extern "C" void fc_solve_kaz_tree_destroy(dict_t dict)
{
    apr_hash_t *h = (apr_hash_t *)dict;
    apr_pool_destroy(apr_hash_pool_get(h));
}
