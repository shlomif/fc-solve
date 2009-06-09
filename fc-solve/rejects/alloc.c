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

