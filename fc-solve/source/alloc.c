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

#include <stdlib.h>
#include <stdio.h>

#include "config.h"

#include "alloc.h"

#define ALLOCED_SIZE (64*1024-(256+128))

void fc_solve_compact_allocator_init(
    fcs_compact_allocator_t * allocator
    )
{
    allocator->packs = (char * *)malloc(sizeof(allocator->packs[0]) * IA_STATE_PACKS_GROW_BY);
    allocator->num_packs = 1;
    allocator->max_ptr =
        (allocator->ptr =
        allocator->rollback_ptr =
        allocator->packs[0] =
        malloc(ALLOCED_SIZE))
            + ALLOCED_SIZE;
    return;
}

void fc_solve_compact_allocator_extend(
    fcs_compact_allocator_t * allocator
        )
{
    /* Allocate a new pack */
    if (! ((++allocator->num_packs) & (IA_STATE_PACKS_GROW_BY-1)))
    {
        allocator->packs = (char * *)realloc(
            allocator->packs,
            sizeof(allocator->packs[0]) * 
                ((allocator->num_packs) + IA_STATE_PACKS_GROW_BY)
        );
    }

    allocator->max_ptr =
        (allocator->ptr =
        allocator->rollback_ptr =
        allocator->packs[allocator->num_packs-1] =
        malloc(ALLOCED_SIZE))
            + ALLOCED_SIZE;
}


void fc_solve_compact_allocator_finish(fcs_compact_allocator_t * allocator)
{
    int a;
    for(a=0;a<allocator->num_packs;a++)
    {
        free(allocator->packs[a]);
    }
    free(allocator->packs);
}
