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
