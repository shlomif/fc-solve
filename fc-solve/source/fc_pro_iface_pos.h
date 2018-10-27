/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * fc_pro_iface_pos.h - generate solutions in standard notation, with
 * implicit (and not included) Horne/Raymond prune moves.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "freecell-solver/fcs_cl.h"
#include "state.h"

typedef struct
{
    fcs_move_t move;
    bool to_empty_stack;
} fcs_extended_move;

typedef struct
{
    size_t next_move_idx;
    size_t num_moves;
    fcs_extended_move *moves;
} fcs_moves_processed;

void fc_solve_moves_processed_gen(fcs_moves_processed *,
    fcs_state_keyval_pair *, int, const fcs_moves_sequence_t *);
void fc_solve_moves_processed_render_move(fcs_extended_move, char *);

static inline int fc_solve_moves_processed_get_moves_left(
    const fcs_moves_processed *const moves)
{
    return moves->num_moves - moves->next_move_idx;
}

static inline bool fc_solve_moves_processed_get_next_move(
    fcs_moves_processed *const moves, fcs_extended_move *const move)
{
    if (moves->next_move_idx == moves->num_moves)
    {
        return TRUE;
    }
    *move = moves->moves[moves->next_move_idx++];
    return FALSE;
}

static inline void fc_solve_moves_processed_free(
    fcs_moves_processed *const moves)
{
    free(moves->moves);
    moves->moves = NULL;
}

#ifdef __cplusplus
}
#endif
