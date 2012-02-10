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
 * meta_alloc.c - the Freecell Solver compact allocator based on the
 * meta-allocator concept that is used to collect the pages allocated by
 * the standard allocator after it is destroyed and to recycle them. 
 *
 * Also see alloc.c.
 */

#define BUILDING_DLL 1

#include <stdlib.h>
#include <stdio.h>

#include "config.h"

#include "meta_alloc.h"

#define ALLOCED_SIZE (FCS_IA_PACK_SIZE*1024-(256+128))

void fc_solve_compact_allocator_init(
    fcs_compact_allocator_t * allocator,
    fcs_meta_compact_allocator_t * meta_allocator
    )
{
    allocator->old_list = NULL;
    allocator->meta = meta_allocator;

    fc_solve_compact_allocator_extend(allocator);
    return;
}

#define OLD_LIST_NEXT(ptr) (*((char * *)(ptr)))
#define OLD_LIST_DATA(ptr) ((char *)(&(((char * *)(ptr))[1])))
static GCC_INLINE char * meta_request_new_buffer(
    fcs_meta_compact_allocator_t * meta_allocator
    )
{
    if (meta_allocator->recycle_bin)
    {
        char * ret = meta_allocator->recycle_bin;
        meta_allocator->recycle_bin = OLD_LIST_NEXT(ret);
        return ret;
    }
    else
    {
        return malloc(ALLOCED_SIZE);
    }
}
    
void fc_solve_compact_allocator_extend(
    fcs_compact_allocator_t * allocator
        )
{
    char * new_data;

    new_data = meta_request_new_buffer(allocator->meta);

    OLD_LIST_NEXT(new_data) = allocator->old_list;
    allocator->old_list = new_data;

    allocator->ptr
        = allocator->rollback_ptr
        = OLD_LIST_DATA(new_data);
    allocator->max_ptr = new_data + ALLOCED_SIZE;
}

void fc_solve_meta_compact_allocator_finish(fcs_meta_compact_allocator_t * meta_allocator)
{
    char * iter, * iter_next;

    for (
        iter = meta_allocator->recycle_bin, iter_next = OLD_LIST_NEXT(iter)
            ;
        iter_next
            ;
        iter = iter_next , iter_next = OLD_LIST_NEXT(iter)
        )
    {
        free(iter);
    }

    free(iter);
    meta_allocator->recycle_bin = NULL;
}

void fc_solve_compact_allocator_finish(fcs_compact_allocator_t * allocator)
{
    char * iter, * iter_next;
    fcs_meta_compact_allocator_t * meta;

    meta = allocator->meta;

    /* Enqueue all the allocated buffers in the meta allocator for re-use. 
     * */
    for (
        iter = allocator->old_list, iter_next = OLD_LIST_NEXT(iter)
            ;
        iter_next
            ;
        iter = iter_next , iter_next = OLD_LIST_NEXT(iter)
        )
    {
        OLD_LIST_NEXT(iter) = meta->recycle_bin;
        meta->recycle_bin = iter;
    }

    OLD_LIST_NEXT(iter) = meta->recycle_bin;
    meta->recycle_bin = iter;
}
#undef OLD_LIST_NEXT
#undef OLD_LIST_DATA
