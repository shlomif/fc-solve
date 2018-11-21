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
 * alloc.c - the Freecell Solver compact allocator. Used to allocate
 * columns and other small allocations of a short size. Is able to revert the
 * last allocation.
 */

#define BUILDING_DLL 1

#include <stdlib.h>
#include <stdio.h>

#include "config.h"

#include "rinutils.h"

#include "alloc.h"

#define ALLOCED_SIZE (FCS_IA_PACK_SIZE*1024-(256+128))

void fc_solve_compact_allocator_init(
    fcs_compact_allocator_t * allocator
    )
{
    allocator->packs = NULL;
    allocator->num_packs = 0;
    allocator->max_num_packs = 1;

    fc_solve_compact_allocator_extend(allocator);
    return;
}

void fc_solve_compact_allocator_extend(
    fcs_compact_allocator_t * allocator
        )
{
    /* Allocate a new pack */
    if ((++allocator->num_packs) == allocator->max_num_packs)
    {
        allocator->packs = (char * *)SREALLOC(
            allocator->packs,
            allocator->max_num_packs += IA_STATE_PACKS_GROW_BY
        );
        allocator->packs[allocator->num_packs-1] =
           SMALLOC(allocator->packs[allocator->num_packs-1], ALLOCED_SIZE);

        for (int i = allocator->num_packs ; i < allocator->max_num_packs ; i++)
        {
            allocator->packs[i] = NULL;
        }
    }
    else
    {
        if (! allocator->packs[allocator->num_packs - 1])
        {
            allocator->packs[allocator->num_packs - 1] = SMALLOC(
                allocator->packs[allocator->num_packs - 1], ALLOCED_SIZE
            );
        }
    }

    allocator->max_ptr =
        (allocator->ptr
         = allocator->rollback_ptr
         = allocator->packs[allocator->num_packs - 1]
        ) + ALLOCED_SIZE;
}


void fc_solve_compact_allocator_finish(fcs_compact_allocator_t * allocator)
{
    char * * const packs_end = (char * * curr_pack = allocator->packs) + allocator->max_num_packs;

    for ( ; curr_pack < packs_end ; curr_pack++)
    {
        free(*(curr_pack));
    }

    free(allocator->packs);
}
