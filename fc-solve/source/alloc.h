
#ifndef __ALLOC_H
#define __ALLOC_H

#ifdef __cplusplus
extern "C" 
{
#endif


struct fcs_compact_allocator_struct
{
    char * * packs;
    int max_num_packs;
    int num_packs;
    char * max_ptr;
    char * ptr;
    char * rollback_ptr;
};

typedef struct fcs_compact_allocator_struct fcs_compact_allocator_t;

extern fcs_compact_allocator_t * 
    freecell_solver_compact_allocator_new(void);

extern char * 
    freecell_solver_compact_allocator_alloc(
        fcs_compact_allocator_t * allocator,
        int how_much
            );

extern void freecell_solver_compact_allocator_release(fcs_compact_allocator_t * allocator);

extern void freecell_solver_compact_allocator_finish(fcs_compact_allocator_t * allocator);
    
extern void freecell_solver_compact_allocator_foreach(
    fcs_compact_allocator_t * allocator,
    int data_width,
    void (*ptr_function)(void *, void *),
    void * context
        );

#ifdef __cplusplus
};
#endif
    
#endif
