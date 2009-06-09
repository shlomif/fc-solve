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


struct fcs_compact_allocator_struct
{
    char * * packs;
    int num_packs;
    char * max_ptr;
    char * ptr;
    char * rollback_ptr;
};

typedef struct fcs_compact_allocator_struct fcs_compact_allocator_t;

extern fcs_compact_allocator_t *
    fc_solve_compact_allocator_new(void);

extern void fc_solve_compact_allocator_extend(
    fcs_compact_allocator_t * allocator
        );

#define fcs_compact_alloc_into_var_MYSIZE(what_t) ((sizeof(what_t))+(sizeof(char *)-((sizeof(what_t))&(sizeof(char *)-1))))

#define fcs_compact_alloc_into_var(result,allocator_orig,what_t) \
{ \
   register fcs_compact_allocator_t * allocator = (allocator_orig); \
   if (allocator->max_ptr - allocator->ptr < fcs_compact_alloc_into_var_MYSIZE(what_t))  \
    {      \
        fc_solve_compact_allocator_extend(allocator);      \
    }         \
    allocator->rollback_ptr = allocator->ptr;       \
    allocator->ptr += fcs_compact_alloc_into_var_MYSIZE(what_t);      \
    result = (what_t *)allocator->rollback_ptr;       \
}

#define fcs_compact_alloc_typed_ptr_into_var(result, type_t, allocator_orig, how_much_orig) \
{ \
    register fcs_compact_allocator_t * allocator = (allocator_orig); \
    register int how_much = (how_much_orig);     \
    if (allocator->max_ptr - allocator->ptr < how_much)  \
    {      \
        fc_solve_compact_allocator_extend(allocator);      \
    }         \
    allocator->rollback_ptr = allocator->ptr;       \
    /* Round ptr to the next pointer boundary */      \
    allocator->ptr += ((how_much)+((sizeof(char *)-((how_much)&(sizeof(char *)-1)))&(sizeof(char*)-1)));      \
    result = (type_t *)allocator->rollback_ptr;       \
}

#define fcs_compact_alloc_release(allocator) \
{    \
    (allocator)->ptr = (allocator)->rollback_ptr; \
}

extern void fc_solve_compact_allocator_finish(fcs_compact_allocator_t * allocator);

#ifdef __cplusplus
};
#endif

#endif
