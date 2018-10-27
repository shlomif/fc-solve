/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// internal_move_struct.h - header file for the internal move structure.
//
// This file is not exposed to the outside.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "fcs_conf.h"

#ifdef FCS_WITH_MOVES

#include "freecell-solver/fcs_move.h"

#ifdef FCS_USE_COMPACT_MOVE_TOKENS
typedef struct
{
    unsigned int type : 4;
    unsigned int src : 4;
    unsigned int dest : 4;
    unsigned int num_cards_in_seq : 4;
} fcs_internal_move;
#else
typedef fcs_move_t fcs_internal_move;
#endif

#ifdef FCS_USE_COMPACT_MOVE_TOKENS
#define fcs_int_move_set_src(move, value) (move).src = ((unsigned int)(value));
#define fcs_int_move_set_dest(move, value)                                     \
    (move).dest = ((unsigned int)(value));
#define fcs_int_move_set_type(move, value)                                     \
    (move).type = ((unsigned int)(value));
#define fcs_int_move_set_num_cards_in_seq(move, value)                         \
    (move).num_cards_in_seq = ((unsigned int)(value));

#define fcs_int_move_get_src(move) ((move).src)
#define fcs_int_move_get_dest(move) ((move).dest)
#define fcs_int_move_get_type(move) ((move).type)
#define fcs_int_move_get_num_cards_in_seq(move) ((move).num_cards_in_seq)

#else /* Not FCS_USE_COMPACT_MOVE_TOKENS */

#define fcs_int_move_set_src(move, value)                                      \
    fcs_move_set_src_stack((move), (value))
#define fcs_int_move_set_dest(move, value)                                     \
    fcs_move_set_dest_stack((move), (value))
#define fcs_int_move_set_type(move, value) fcs_move_set_type((move), (value))
#define fcs_int_move_set_num_cards_in_seq(move, value)                         \
    fcs_move_set_num_cards_in_seq((move), (value))

#define fcs_int_move_get_src(move) fcs_move_get_src_stack((move))
#define fcs_int_move_get_dest(move) fcs_move_get_dest_stack((move))
#define fcs_int_move_get_type(move) fcs_move_get_type((move))
#define fcs_int_move_get_num_cards_in_seq(move)                                \
    fcs_move_get_num_cards_in_seq((move))

#endif

typedef struct
{
    fcs_internal_move *moves;
    size_t num_moves;
} fcs_move_stack;

#endif

#ifdef __cplusplus
}
#endif
