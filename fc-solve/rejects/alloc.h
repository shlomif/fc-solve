extern char *
    fc_solve_compact_allocator_alloc(
        fcs_compact_allocator_t * allocator,
        int how_much
            );

extern void fc_solve_compact_allocator_release(fcs_compact_allocator_t * allocator);

extern void fc_solve_compact_allocator_foreach(
    fcs_compact_allocator_t * allocator,
    int data_width,
    void (*ptr_function)(void *, void *),
    void * context
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

