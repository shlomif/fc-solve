/* Copyright (c) 2000 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * ms_ca.h - A header file for a (possibly inline) function that compactly
 * allocates a move stack.
 *
 */

#include "inline.h"

static GCC_INLINE fcs_move_stack_t * fc_solve_move_stack_compact_allocate(fc_solve_hard_thread_t * hard_thread, fcs_move_stack_t * old_move_stack_to_parent)
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

