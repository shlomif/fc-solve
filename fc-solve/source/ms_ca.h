/*
 * ms_ca.h - A header file for a (possibly inline) function that compactly
 * allocates a move stack.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2002
 *
 * This file is in the public domain (it's uncopyrighted).
 * */

#include "inline.h"

static GCC_INLINE fcs_move_stack_t * freecell_solver_move_stack_compact_allocate(freecell_solver_hard_thread_t * hard_thread, fcs_move_stack_t * old_move_stack_to_parent)
{
    char * ptr;
    fcs_move_stack_t * new_move_stack_to_parent;
    fcs_move_t * new_moves_to_parent;

    fcs_compact_alloc_typed_ptr_into_var(
        ptr, 
        char, 
        hard_thread->move_stacks_allocator, 
        (sizeof(fcs_move_stack_t) + sizeof(fcs_move_t)*old_move_stack_to_parent->num_moves)
        );
    new_move_stack_to_parent = (fcs_move_stack_t *)ptr;
    new_moves_to_parent = (fcs_move_t *)(ptr+sizeof(fcs_move_stack_t));
    new_move_stack_to_parent->moves = new_moves_to_parent;
    new_move_stack_to_parent->num_moves = 
        new_move_stack_to_parent->max_num_moves = 
        old_move_stack_to_parent->num_moves;
    memcpy(new_moves_to_parent, old_move_stack_to_parent->moves, sizeof(fcs_move_t)*old_move_stack_to_parent->num_moves);
    return new_move_stack_to_parent;
}

