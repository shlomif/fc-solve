// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// meta_alloc.c - the Freecell Solver compact allocator based on the
// meta-allocator concept that is used to collect the pages allocated by
// the standard allocator after it is destroyed and to recycle them.
#include "meta_alloc.h"

void fc_solve_compact_allocator_init(
    compact_allocator *const allocator, meta_allocator *const meta_alloc)
{
    allocator->meta = meta_alloc;

    fc_solve_compact_allocator_init_helper(allocator);
}

void fc_solve_meta_compact_allocator_finish(meta_allocator *const meta_alloc)
{
    char *iter = meta_alloc->recycle_bin;
    char *iter_next = iter ? OLD_LIST_NEXT(iter) : NULL;
    for (; iter_next; iter = iter_next, iter_next = OLD_LIST_NEXT(iter))
    {
        free(iter);
    }
    free(iter);
    meta_alloc->recycle_bin = NULL;
#ifdef FCS_DBM_USE_APR
    if (meta_alloc->apr_pool)
    {
        apr_pool_destroy(meta_alloc->apr_pool);
        meta_alloc->apr_pool = NULL;
    }
#endif
}

void fc_solve_compact_allocator_finish(compact_allocator *const allocator)
{
    char *iter, *iter_next;
    meta_allocator *const meta = allocator->meta;
    var_AUTO(bin, meta->recycle_bin);
    // Enqueue all the allocated buffers in the meta allocator for re-use.
    for (iter = allocator->old_list, iter_next = OLD_LIST_NEXT(iter); iter_next;
        iter = iter_next, iter_next = OLD_LIST_NEXT(iter))
    {
        OLD_LIST_NEXT(iter) = bin;
        bin = iter;
    }

    OLD_LIST_NEXT(iter) = bin;
    meta->recycle_bin = iter;
}

#undef OLD_LIST_NEXT
#undef OLD_LIST_DATA
