
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

extern void freecell_solver_compact_allocator_extend(
    fcs_compact_allocator_t * allocator
        );
#if 0
extern char * 
    freecell_solver_compact_allocator_alloc(
        fcs_compact_allocator_t * allocator,
        int how_much
            );
#else
#define fcs_compact_alloc_into_var(result,allocator_orig,what_t) \
{ \
   register fcs_compact_allocator_t * allocator = (allocator_orig); \
   if (allocator->max_ptr - allocator->ptr < sizeof(what_t))  \
    {      \
        freecell_solver_compact_allocator_extend(allocator);      \
    }         \
    allocator->rollback_ptr = allocator->ptr;       \
    allocator->ptr += ((sizeof(what_t))+(sizeof(char *)-((sizeof(what_t))&(sizeof(char *)-1))));      \
    result = (what_t *)allocator->rollback_ptr;       \
}

#define fcs_compact_alloc_typed_ptr_into_var(result, type_t, allocator_orig, how_much_orig) \
{ \
    register fcs_compact_allocator_t * allocator = (allocator_orig); \
    register int how_much = (how_much_orig);     \
    if (allocator->max_ptr - allocator->ptr < how_much)  \
    {      \
        freecell_solver_compact_allocator_extend(allocator);      \
    }         \
    allocator->rollback_ptr = allocator->ptr;       \
    /* Round ptr to the next pointer boundary */      \
    allocator->ptr += ((how_much)+((sizeof(char *)-((how_much)&(sizeof(char *)-1)))&(sizeof(char*)-1)));      \
    result = (type_t *)allocator->rollback_ptr;       \
}
        
#endif

#if 0
extern void freecell_solver_compact_allocator_release(fcs_compact_allocator_t * allocator);
#else
#define fcs_compact_alloc_release(allocator) \
{    \
    (allocator)->ptr = (allocator)->rollback_ptr; \
} 
#endif

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
