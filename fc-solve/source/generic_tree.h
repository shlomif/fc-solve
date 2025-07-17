// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2012 Shlomi Fish
// generic_tree.h - provide a unified interface to either kaz_tree.h
// or fcs-libavl/rb.h .
#pragma once
#include "freecell-solver/fcs_conf.h"

#ifdef FCS_DBM_USE_HASH
#include "dbm_hashtable.h"
#else

#ifdef FCS_DBM_USE_LIBAVL

#include "rb.h"
typedef struct rb_table dict_t;
typedef void *dict_key_t;
#define fc_solve_kaz_tree_destroy(tree) rb_destroy(tree, NULL)
#define fc_solve_kaz_tree_create(comparator, context, meta, recycle_bin_ptr)   \
    rb_create(comparator, context, meta, recycle_bin_ptr)
#define fc_solve_kaz_tree_lookup_value(tree, value) rb_find(tree, value)
#define fc_solve_kaz_tree_delete_by_value(tree, value) rb_delete(tree, value)
#define fc_solve_kaz_tree_alloc_insert(tree, value) rb_insert(tree, value)
typedef struct rb_table fcs_dbm__abstract__states_lookup_t;
typedef void *dict_key_t;
#define fcs_dbm__abstract__states_lookup__destroy(tree)                        \
    fc_solve_kaz_tree_destroy(tree)
#define fcs_dbm__abstract__states_lookup__create(                              \
    comparator, context, meta, recycle_bin_ptr)                                \
    rb_create(comparator, context, meta, recycle_bin_ptr)
#define fcs_dbm__abstract__states_lookup__lookup_value(tree, value)            \
    rb_find(tree, value)
#define fcs_dbm__abstract__states_lookup__delete_by_value(tree, value)         \
    rb_delete(tree, value)
#define fcs_dbm__abstract__states_lookup__alloc_insert(tree, value)            \
    rb_insert(tree, value)

#else

#include "kaz_tree.h"
typedef dict_t fcs_dbm__abstract__states_lookup_t;
static inline void fc_solve_kaz_tree_delete_by_value(
    dict_t *const kaz_tree, dict_key_t value)
{
    fc_solve_kaz_tree_delete_free(
        kaz_tree, fc_solve_kaz_tree_lookup(kaz_tree, value));
}
#define fcs_dbm__abstract__states_lookup__destroy(tree)                        \
    fc_solve_kaz_tree_destroy(tree)
#define fcs_dbm__abstract__states_lookup__create(                              \
    comparator, context, meta, recycle_bin_ptr)                                \
    fc_solve_kaz_tree_create(comparator, context, meta, recycle_bin_ptr)
#define fcs_dbm__abstract__states_lookup__lookup_value(tree, value)            \
    fc_solve_kaz_tree_lookup_value(tree, value)
#define fcs_dbm__abstract__states_lookup__delete(tree, value)         \
    fc_solve_kaz_tree_delete(tree, value)
#define fcs_dbm__abstract__states_lookup__alloc_insert(tree, value)            \
    fc_solve_kaz_tree_alloc_insert(tree, value)
static inline void fcs_dbm__abstract__states_lookup__delete_by_value(
    fcs_dbm__abstract__states_lookup_t *const kaz_tree, dict_key_t value)
{
    fc_solve_kaz_tree_delete_free(
        kaz_tree, fc_solve_kaz_tree_lookup(kaz_tree, value));
}

#endif
#endif
