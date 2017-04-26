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

#include "fcs_cl.h"
#include "state.h"

typedef struct
{
    fcs_move_t move;
    fcs_bool_t to_empty_stack;
} fcs_extended_move_t;

typedef struct
{
    size_t next_move_idx;
    size_t num_moves;
    fcs_extended_move_t *moves;
} fcs_moves_processed_t;

void fc_solve_moves_processed_gen(fcs_moves_processed_t *const ret,
    fcs_state_keyval_pair_t *const orig, const int num_freecells,
    const fcs_moves_sequence_t *const moves_seq);

void fc_solve_moves_processed_render_move(
    fcs_extended_move_t move, char *string);

static inline int fc_solve_moves_processed_get_moves_left(
    const fcs_moves_processed_t *const moves)
{
    return moves->num_moves - moves->next_move_idx;
}

static inline fcs_bool_t fc_solve_moves_processed_get_next_move(
    fcs_moves_processed_t *const moves, fcs_extended_move_t *const move)
{
    if (moves->next_move_idx == moves->num_moves)
    {
        return TRUE;
    }
    *move = moves->moves[moves->next_move_idx++];
    return FALSE;
}

static inline void fc_solve_moves_processed_free(
    fcs_moves_processed_t *const moves)
{
    free(moves->moves);
    moves->moves = NULL;
}

#ifdef __cplusplus
}
#endif
