// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// meta_alloc.h - the Freecell Solver compact allocator based on the
// meta-allocator concept that is used to collect the pages allocated by
// the standard allocator after it is destroyed and to recycle them.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FCS_DBM_USE_APR
#include <apr_hash.h>
#endif

#include "state.h"

typedef struct
{
    char *recycle_bin;
#ifdef FCS_DBM_USE_APR
    apr_pool_t *apr_pool;
#endif
} meta_allocator;

typedef struct
{
    char *old_list;
    char *max_ptr;
    char *ptr;
    char *rollback_ptr;
    meta_allocator *meta;
} compact_allocator;

#define OLD_LIST_NEXT(ptr) (*((char **)(ptr)))
#define OLD_LIST_DATA(ptr) ((char *)(&(((char **)(ptr))[1])))
#define FCS_METAALLOC_ALLOCED_SIZE (FCS_IA_PACK_SIZE * 1024 - (128))
static inline char *meta_request_new_buffer(meta_allocator *const meta_alloc)
{
    char *const ret = meta_alloc->recycle_bin;
    if (ret)
    {
        meta_alloc->recycle_bin = OLD_LIST_NEXT(ret);
        return ret;
    }
    else
    {
        return malloc(FCS_METAALLOC_ALLOCED_SIZE);
    }
}
static inline void fc_solve_compact_allocator_extend(
    compact_allocator *const allocator)
{
    char *const new_data = meta_request_new_buffer(allocator->meta);

    OLD_LIST_NEXT(new_data) = allocator->old_list;
    allocator->old_list = new_data;

    allocator->ptr = allocator->rollback_ptr = OLD_LIST_DATA(new_data);
    allocator->max_ptr = new_data + FCS_METAALLOC_ALLOCED_SIZE;
}

// Call this after allocator->meta was initialized
static inline void fc_solve_compact_allocator_init_helper(
    compact_allocator *const allocator)
{
    allocator->old_list = NULL;
    fc_solve_compact_allocator_extend(allocator);
}

static inline void fc_solve_meta_compact_allocator_init(
    meta_allocator *const meta)
{
    meta->recycle_bin = NULL;
#ifdef FCS_DBM_USE_APR
    meta->apr_pool = NULL;
#endif
}

extern void fc_solve_meta_compact_allocator_finish(meta_allocator *);

extern void fc_solve_compact_allocator_init(
    compact_allocator *, meta_allocator *);

static inline void *fcs_compact_alloc_ptr(
    compact_allocator *const allocator, const size_t how_much_proto)
{
    // Round ptr to the next pointer boundar
    const size_t how_much =
        how_much_proto +
        ((sizeof(char *) - ((how_much_proto) & (sizeof(char *) - 1))) &
            (sizeof(char *) - 1));

    if ((size_t)(allocator->max_ptr - allocator->ptr) < how_much)
    {
        fc_solve_compact_allocator_extend(allocator);
    }
    else
    {
        allocator->rollback_ptr = allocator->ptr;
    }
    allocator->ptr += how_much;

    return allocator->rollback_ptr;
}

static inline void fcs_compact_alloc_release(compact_allocator *const allocator)
{
    allocator->ptr = allocator->rollback_ptr;
}

extern void fc_solve_compact_allocator_finish(compact_allocator *);

static inline fcs_collectible_state *fcs_state_ia_alloc_into_var(
    compact_allocator *const allocator)
{
    return (fcs_collectible_state *)fcs_compact_alloc_ptr(
        allocator, sizeof(fcs_collectible_state));
}

static inline void fc_solve_compact_allocator_recycle(
    compact_allocator *const allocator)
{
    fc_solve_compact_allocator_finish(allocator);
    fc_solve_compact_allocator_init_helper(allocator);
}

#ifdef __cplusplus
};
#endif
