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
 * alloc.h - the Freecell Solver compact allocator. Used to allocate
 * columns and other small allocations of a short size. Is able to revert the
 * last allocation.
 */
#ifndef FC_SOLVE__ALLOC_H
#define FC_SOLVE__ALLOC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "state.h"

typedef struct
{
    char * * packs;
    int num_packs, max_num_packs;
    char * max_ptr;
    char * ptr;
    char * rollback_ptr;
} fcs_compact_allocator_t;

extern void
    fc_solve_compact_allocator_init(fcs_compact_allocator_t * allocator);

extern void fc_solve_compact_allocator_extend(
    fcs_compact_allocator_t * allocator
        );

static GCC_INLINE void * fcs_compact_alloc_ptr(fcs_compact_allocator_t * allocator, int how_much)
{
    /* Round ptr to the next pointer boundary */
    how_much +=
        (
         (sizeof(char *)-((how_much)&(sizeof(char *)-1)))&(sizeof(char*)-1)
        );

    if (allocator->max_ptr - allocator->ptr < how_much)
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

#define fcs_compact_alloc_release(allocator) \
{    \
    (allocator)->ptr = (allocator)->rollback_ptr; \
}

extern void fc_solve_compact_allocator_finish(fcs_compact_allocator_t * allocator);

static GCC_INLINE fcs_collectible_state_t * fcs_state_ia_alloc_into_var(fcs_compact_allocator_t * const allocator)
{
    return (fcs_collectible_state_t *)
        fcs_compact_alloc_ptr(
            allocator, sizeof(fcs_collectible_state_t)
        );
}

static GCC_INLINE void fc_solve_compact_allocator_recycle(fcs_compact_allocator_t * allocator)
{
    allocator->num_packs = 0;

    /* Now num_packs will be 1 and a max_ptr and ptr will be set. */
    fc_solve_compact_allocator_extend(allocator);

    return;
}

#ifdef __cplusplus
};
#endif

#endif
