#include <malloc.h>

#include "config.h"

#include "alloc.h"

#define ALLOCED_SIZE (0x10000 - 0x200)

fcs_compact_allocator_t * 
    freecell_solver_compact_allocator_new(void)
{
    fcs_compact_allocator_t * allocator;


    allocator = (fcs_compact_allocator_t *)malloc(sizeof(*allocator));
    allocator->max_num_packs = IA_STATE_PACKS_GROW_BY;
    allocator->packs = (char * *)malloc(sizeof(allocator->packs[0]) * allocator->max_num_packs);
    allocator->num_packs = 1;
    allocator->max_ptr = 
        (allocator->ptr = 
        allocator->rollback_ptr = 
        allocator->packs[0] = 
        malloc(ALLOCED_SIZE))
            + ALLOCED_SIZE;
    
    return allocator;
}


