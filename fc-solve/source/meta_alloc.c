/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * meta_alloc.c - the Freecell Solver compact allocator based on the
 * meta-allocator concept that is used to collect the pages allocated by
 * the standard allocator after it is destroyed and to recycle them.
 *
 * Also see alloc.c.
 */

#include "dll_thunk.h"

#include <stdlib.h>
#include <stdio.h>

#include "config.h"

#include "meta_alloc.h"

#define ALLOCED_SIZE (FCS_IA_PACK_SIZE * 1024 - (256 + 128))

void fc_solve_compact_allocator_init(fcs_compact_allocator_t *const allocator,
    fcs_meta_compact_allocator_t *const meta_allocator)
{
    allocator->meta = meta_allocator;

    fc_solve_compact_allocator_init_helper(allocator);

    return;
}

#define OLD_LIST_NEXT(ptr) (*((char **)(ptr)))
#define OLD_LIST_DATA(ptr) ((char *)(&(((char **)(ptr))[1])))
static inline char *meta_request_new_buffer(
    fcs_meta_compact_allocator_t *const meta_allocator)
{
    char *const ret = meta_allocator->recycle_bin;
    if (ret)
    {
        meta_allocator->recycle_bin = OLD_LIST_NEXT(ret);
        return ret;
    }
    else
    {
        return malloc(ALLOCED_SIZE);
    }
}

void fc_solve_compact_allocator_extend(fcs_compact_allocator_t *const allocator)
{
    char *const new_data = meta_request_new_buffer(allocator->meta);

    OLD_LIST_NEXT(new_data) = allocator->old_list;
    allocator->old_list = new_data;

    allocator->ptr = allocator->rollback_ptr = OLD_LIST_DATA(new_data);
    allocator->max_ptr = new_data + ALLOCED_SIZE;
}

void fc_solve_meta_compact_allocator_finish(
    fcs_meta_compact_allocator_t *const meta_allocator)
{
    char *iter, *iter_next;
    for (iter = meta_allocator->recycle_bin,
        iter_next = iter ? OLD_LIST_NEXT(iter) : NULL;
         iter_next; iter = iter_next, iter_next = OLD_LIST_NEXT(iter))
    {
        free(iter);
    }
    free(iter);
    meta_allocator->recycle_bin = NULL;
}

void fc_solve_compact_allocator_finish(fcs_compact_allocator_t *const allocator)
{
    char *iter, *iter_next;
    fcs_meta_compact_allocator_t *const meta = allocator->meta;

    /* Enqueue all the allocated buffers in the meta allocator for re-use.
     * */
    for (iter = allocator->old_list, iter_next = OLD_LIST_NEXT(iter); iter_next;
         iter = iter_next, iter_next = OLD_LIST_NEXT(iter))
    {
        OLD_LIST_NEXT(iter) = meta->recycle_bin;
        meta->recycle_bin = iter;
    }

    OLD_LIST_NEXT(iter) = meta->recycle_bin;
    meta->recycle_bin = iter;
}

#undef OLD_LIST_NEXT
#undef OLD_LIST_DATA
