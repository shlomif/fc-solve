#if 0
char *
    fc_solve_compact_allocator_alloc(
        fcs_compact_allocator_t * allocator,
        int how_much
            )
{
    if (allocator->max_ptr - allocator->ptr < how_much)
    {
        fc_solve_compact_allocator_extend(allocator);
    }
    allocator->rollback_ptr = allocator->ptr;
    allocator->ptr += (how_much+(4-(how_much&0x3)));
    return allocator->rollback_ptr;
}

void fc_solve_compact_allocator_release(fcs_compact_allocator_t * allocator)
{
    allocator->ptr = allocator->rollback_ptr;
}
#endif

void fc_solve_compact_allocator_foreach(
    fcs_compact_allocator_t * allocator,
    int data_width,
    void (*ptr_function)(void *, void *),
    void * context
        )
{
    int pack;
    char * ptr, * max_ptr;
    for(pack=0;pack<allocator->num_packs-1;pack++)
    {
        ptr = allocator->packs[pack];
        max_ptr = ptr + ALLOCED_SIZE - data_width;
        while (ptr <= max_ptr)
        {
            ptr_function(ptr, context);
            ptr += data_width;
        }
    }
    /* Run the callback on the last pack */
    ptr = allocator->packs[pack];
    max_ptr = allocator->rollback_ptr;
    while (ptr <= max_ptr)
    {
        ptr_function(ptr, context);
        ptr += data_width;
    }
}
