// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2010 Shlomi Fish
// google_hash.h - header file for Google's dense_hash_map as adapted
// for Freecell Solver.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "freecell-solver/fcs_conf.h"
#include "rinutils/rinutils.h"
#include "meta_alloc.h"

#define FCS_NO_DBM_AVL
#ifdef FCS_LIBAVL_STORE_WHOLE_KEYS
#include "delta_states.h"
#endif

typedef void *fcs_dbm__abstract__states_lookup_t;
#ifdef AVL_with_rb_param
typedef int (*dict_comp_t)(const void *, const void *, void *);
#else
typedef int (*dict_comp_t)(const void *, const void *);
#endif
#ifdef FCS_KAZ_TREE_USE_RECORD_DICT_KEY
typedef fcs_dbm_record dict_key_t;
typedef dict_key_t *dict_ret_key_t;
#else
typedef const void *cdict_key_t;
typedef void *dict_key_t;
typedef dict_key_t dict_ret_key_t;
#endif

extern void fcs_dbm__abstract__states_lookup__destroy(
    fcs_dbm__abstract__states_lookup_t);
extern fcs_dbm__abstract__states_lookup_t
fcs_dbm__abstract__states_lookup__create(
    dict_comp_t, void *, meta_allocator *, void **);
extern dict_key_t fcs_dbm__abstract__states_lookup__lookup_value(
    fcs_dbm__abstract__states_lookup_t dict, cdict_key_t key);
extern void fcs_dbm__abstract__states_lookup__delete_by_value(
    fcs_dbm__abstract__states_lookup_t *const kaz_tree, dict_key_t value);
extern dict_ret_key_t fcs_dbm__abstract__states_lookup__alloc_insert(
    fcs_dbm__abstract__states_lookup_t, dict_key_t);

#ifdef __cplusplus
}
#endif
