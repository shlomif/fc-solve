/* Copyright 2009
 * Kaz Kylheku <kkylheku@gmail.com>
 * Vancouver, Canada
 * All rights reserved.
 *
 * BSD License:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3. The name of the author may not be used to endorse or promote
 *      products derived from this software without specific prior
 *      written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/*
 * Adapted from kazlib's dict.h by Shlomi Fish for Freecell Solver:
 *
 * http://git.savannah.gnu.org/cgit/kazlib.git/
 *
 * */
#pragma once

#include <limits.h>

#include "meta_alloc.h"
/*
 * Blurb for inclusion into C++ translation units
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long dictcount_t;
#define DICTCOUNT_T_MAX ULONG_MAX

/*
 * The dictionary is implemented as a red-black tree
 */

typedef enum
{
    dnode_red,
    dnode_black
} dnode_color_t;

/*
 * This is set by dbm_kaztree.c to conserve space when used in the delta-states
 * nodes.
 */
#ifdef FCS_KAZ_TREE_USE_RECORD_DICT_KEY
typedef fcs_dbm_record dict_key_t;
typedef dict_key_t *dict_ret_key_t;
#else
typedef void *dict_key_t;
typedef dict_key_t dict_ret_key_t;
#endif

#define DNODE_NEXT(node) ((node)->dict_left)

typedef struct dnode_t
{
#if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
    struct dnode_t *dict_left;
    struct dnode_t *dict_right;
    struct dnode_t *dict_parent;
    dnode_color_t dict_color;
    dict_key_t dict_key;
#ifdef NO_FC_SOLVE
    void *dict_data;
#endif
#else
    int dict_dummy;
#endif
} dnode_t;

typedef int (*dict_comp_t)(const void *, const void *, void *);
/* Removed from fc-solve */
#if 0
typedef dnode_t *(*dnode_alloc_t)(void *);
typedef void (*dnode_free_t)(dnode_t *, void *);
#endif

typedef struct dict_t
{
#if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
    dnode_t dict_nilnode;
#ifdef NO_FC_SOLVE
    dictcount_t dict_nodecount;
    dictcount_t dict_maxcount;
#endif
    dict_comp_t dict_compare;
/* Removed from fc-solve */
#if 0
    dnode_alloc_t dict_allocnode;
    dnode_free_t dict_free_node;
#else
    compact_allocator dict_allocator;
    dnode_t *dict_recycle_bin;
#endif
    void *dict_context;
#ifdef NO_FC_SOLVE
    int dict_dupes;
#endif
#else
    int dict_dummy;
#endif
} dict_t;

typedef void (*dnode_process_t)(dict_t *, dnode_t *, void *);

typedef struct dict_load_t
{
#if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
    dict_t *dict_dictptr;
    dnode_t dict_nilnode;
#else
    int dict_dummy;
#endif
} dict_load_t;

#ifdef NO_FC_SOLVE
extern dict_t *dict_create(dictcount_t, dict_comp_t, void *);
#else
extern dict_t *fc_solve_kaz_tree_create(dict_comp_t, void *, meta_allocator *);
#endif
#if 0
extern void dict_set_allocator(dict_t *, dnode_alloc_t, dnode_free_t, void *);
#endif
extern void fc_solve_kaz_tree_destroy(dict_t *);
extern void fc_solve_kaz_tree_free_nodes(dict_t *);
#ifdef NO_FC_SOLVE
extern void dict_free(dict_t *);
extern dict_t *dict_init(dict_t *, dictcount_t, dict_comp_t);
#else
extern dict_t *dict_init(dict_t *, dict_comp_t, meta_allocator *);
#endif
#ifdef NO_FC_SOLVE
extern void dict_init_like(dict_t *, const dict_t *);
extern dict_t *dict_init_alloc(
    dict_t *, dictcount_t, dict_comp_t, dnode_alloc_t, dnode_free_t, void *);
#endif
#ifdef NO_FC_SOLVE
extern int dict_verify(dict_t *);
extern int dict_similar(const dict_t *, const dict_t *);
extern dnode_t *dict_lower_bound(dict_t *, const void *);
extern dnode_t *dict_upper_bound(dict_t *, const void *);
extern dnode_t *dict_strict_lower_bound(dict_t *, const void *);
extern dnode_t *dict_strict_upper_bound(dict_t *, const void *);
#endif
extern dict_ret_key_t fc_solve_kaz_tree_insert(dict_t *, dnode_t *, dict_key_t);
#ifdef NO_FC_SOLVE
extern dnode_t *fc_solve_kaz_tree_delete(dict_t *, dnode_t *);
#endif
extern dict_ret_key_t fc_solve_kaz_tree_alloc_insert(dict_t *, dict_key_t);
extern void fc_solve_kaz_tree_delete_free(dict_t *, dnode_t *);
extern dnode_t *fc_solve_kaz_tree_lookup(dict_t *, dict_key_t);
dict_key_t fc_solve_kaz_tree_lookup_value(dict_t *dict, dict_key_t key);

extern dnode_t *fc_solve_kaz_tree_first(dict_t *);
extern dnode_t *fc_solve_kaz_tree_next(dict_t *, dnode_t *);
#ifdef NO_FC_SOLVE
extern dnode_t *dict_last(dict_t *);
extern dnode_t *dict_prev(dict_t *, dnode_t *);
extern dictcount_t dict_count(dict_t *);
extern int dict_isempty(dict_t *);
extern int dict_isfull(dict_t *);
extern int dict_contains(dict_t *, dnode_t *);
#endif
#ifdef NO_FC_SOLVE
extern void dict_allow_dupes(dict_t *);
#endif
#ifdef NO_FC_SOLVE
extern int dnode_is_in_a_dict(dnode_t *);
extern dnode_t *dnode_create(void *);
extern dnode_t *dnode_init(dnode_t *, void *);
extern void dnode_destroy(dnode_t *);
extern void *dnode_get(dnode_t *);
extern dict_key_t *dnode_getkey(dnode_t *);
extern void dnode_put(dnode_t *, void *);
#endif
#ifdef NO_FC_SOLVE
extern void dict_process(dict_t *, void *, dnode_process_t);
extern void dict_load_begin(dict_load_t *, dict_t *);
extern void dict_load_next(dict_load_t *, dnode_t *, const void *);
extern void dict_load_end(dict_load_t *);
extern void dict_merge(dict_t *, dict_t *);
#endif

#if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
#define dict_isfull(D) ((D)->dict_nodecount == (D)->dict_maxcount)
#define dict_count(D) ((D)->dict_nodecount)
#define dict_isempty(D) ((D)->dict_nodecount == 0)
#define dnode_get(N) ((N)->dict_data)
#define dnode_getkey(N) ((N)->dict_key)
#define dnode_put(N, X) ((N)->dict_data = (X))
#endif

#ifdef __cplusplus
}
#endif
