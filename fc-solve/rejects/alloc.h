extern char *
    fc_solve_compact_allocator_alloc(
        fcs_compact_allocator_t * allocator,
        int how_much
            );

extern void fc_solve_compact_allocator_release(fcs_compact_allocator_t * allocator);
