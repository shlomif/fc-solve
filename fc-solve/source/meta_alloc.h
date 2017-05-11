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
 * meta_alloc.h - the Freecell Solver compact allocator based on the
 * meta-allocator concept that is used to collect the pages allocated by
 * the standard allocator after it is destroyed and to recycle them.
 *
 * Also see alloc.h.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "state.h"

typedef struct
{
    char *recycle_bin;
} fcs_meta_compact_allocator_t;

typedef struct
{
    char *old_list;
    char *max_ptr;
    char *ptr;
    char *rollback_ptr;
    fcs_meta_compact_allocator_t *meta;
} fcs_compact_allocator_t;

extern void fc_solve_compact_allocator_extend(fcs_compact_allocator_t *);

/* To be called after the meta_alloc was set. */
static inline void fc_solve_compact_allocator_init_helper(
    fcs_compact_allocator_t *const allocator)
{
    allocator->old_list = NULL;
    fc_solve_compact_allocator_extend(allocator);
}

static inline void fc_solve_meta_compact_allocator_init(
    fcs_meta_compact_allocator_t *const meta)
{
    meta->recycle_bin = NULL;
}

extern void fc_solve_meta_compact_allocator_finish(
    fcs_meta_compact_allocator_t *);

extern void fc_solve_compact_allocator_init(
    fcs_compact_allocator_t *, fcs_meta_compact_allocator_t *);

static inline void *fcs_compact_alloc_ptr(
    fcs_compact_allocator_t *const allocator, const size_t how_much_proto)
{
    /* Round ptr to the next pointer boundary */
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

static inline void fcs_compact_alloc_release(
    fcs_compact_allocator_t *const allocator)
{
    allocator->ptr = allocator->rollback_ptr;
}

extern void fc_solve_compact_allocator_finish(fcs_compact_allocator_t *);

static inline fcs_collectible_state_t *fcs_state_ia_alloc_into_var(
    fcs_compact_allocator_t *const allocator)
{
    return (fcs_collectible_state_t *)fcs_compact_alloc_ptr(
        allocator, sizeof(fcs_collectible_state_t));
}

static inline void fc_solve_compact_allocator_recycle(
    fcs_compact_allocator_t *const allocator)
{
    fc_solve_compact_allocator_finish(allocator);
    fc_solve_compact_allocator_init_helper(allocator);
}

#ifdef __cplusplus
};
#endif
