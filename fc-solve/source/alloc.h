
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

fcs_compact_allocator_t * 
    freecell_solver_compact_allocator_new(void);

#ifdef __cplysplys
};
#endif
    
#endif
