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
 * generic_tree.h - provide a unified interface to either kaz_tree.h
 * or libavl/avl.h .
 *
 */

#ifndef FC_SOLVE__GENERIC_TREE_H
#define FC_SOLVE__GENERIC_TREE_H

#include "config.h"

#ifdef FCS_DBM_USE_LIBAVL

#include "avl.h"

typedef struct avl_table dict_t;
typedef void * dict_key_t;
#define fc_solve_kaz_tree_destroy(tree) avl_destroy(tree, NULL)
#define fc_solve_kaz_tree_create(comparator, context, meta, recycle_bin_ptr) avl_create(comparator, context, meta, recycle_bin_ptr)
#define fc_solve_kaz_tree_lookup_value(tree, value) avl_find(tree, value)
#define fc_solve_kaz_tree_delete_by_value(tree, value) avl_delete(tree, value)
#define fc_solve_kaz_tree_alloc_insert(tree, value) avl_insert(tree, value)
#define dict_allocator avl_allocator
#else

#include "inline.h"
#include "kaz_tree.h"

static GCC_INLINE void fc_solve_kaz_tree_delete_by_value(
    dict_t * kaz_tree,
    dict_key_t value
)
{
    fc_solve_kaz_tree_delete_free(
        kaz_tree,
        fc_solve_kaz_tree_lookup(
            kaz_tree, value
            )
        );
}

#endif

#endif
