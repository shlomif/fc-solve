/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// move_stack_compact_alloc.h - A header file for a (possibly inline) function
// that compactly allocates a move stack.
#pragma once

#include "instance.h"
#include "move.h"

#ifdef FCS_WITH_MOVES
static inline fcs_move_stack *fc_solve_move_stack_compact_allocate(
    fcs_hard_thread *const hard_thread,
    fcs_move_stack *const old_move_stack_to_parent)
{
    char *const ptr =
        (char *)fcs_compact_alloc_ptr(&(HT_FIELD(hard_thread, allocator)),
            (sizeof(fcs_move_stack) +
                sizeof(fcs_move_t) * old_move_stack_to_parent->num_moves));

    fcs_move_stack *const new_move_stack_to_parent = (fcs_move_stack *)ptr;
    fcs_internal_move *const new_moves_to_parent =
        (fcs_internal_move *)(ptr + sizeof(fcs_move_stack));
    new_move_stack_to_parent->moves = new_moves_to_parent;
    new_move_stack_to_parent->num_moves = old_move_stack_to_parent->num_moves;
    memcpy(new_moves_to_parent, old_move_stack_to_parent->moves,
        sizeof(fcs_move_t) * old_move_stack_to_parent->num_moves);
    return new_move_stack_to_parent;
}
#endif
