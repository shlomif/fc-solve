/* Copyright (c) 2000 Shlomi Fish
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
 * alloc.c - the Freecell Solver compact allocator. Used to allocate
 * columns and other small allocations of a short size. Is able to revert the
 * last allocation.
 */

#define BUILDING_DLL 1

#include <stdlib.h>
#include <stdio.h>

#include "config.h"

#include "alloc_wrap.h"

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
        int i;

        allocator->packs = (char * *)SREALLOC(
            allocator->packs,
            allocator->max_num_packs += IA_STATE_PACKS_GROW_BY
        );
        allocator->packs[allocator->num_packs-1] =
            malloc(ALLOCED_SIZE);

        for ( i = allocator->num_packs ; i < allocator->max_num_packs ; i++)
        {
            allocator->packs[i] = NULL;
        }
    }
    else
    {
        if (! allocator->packs[allocator->num_packs - 1])
        {
            allocator->packs[allocator->num_packs - 1] = malloc(ALLOCED_SIZE);
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
    char * * curr_pack, * * packs_end;

    packs_end = (curr_pack = allocator->packs) + allocator->max_num_packs;

    for ( ; curr_pack < packs_end ; curr_pack++)
    {
        free(*(curr_pack));
    }

    free(allocator->packs);
}

