/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// game_type_params.h: provide fcs_game_type_params_t
#pragma once

#include "game_type_limit.h"

#ifndef FCS_FREECELL_ONLY
typedef struct
{
#endif
    /*
     * The number of Freecells, Stacks and Foundations present in the game.
     *
     * freecells_num and stacks_num are variable and may be specified at
     * the beginning of the execution of the algorithm. However, there
     * is a maximal limit to them which is set in config.h.
     *
     * decks_num can be 1 or 2 .
     * */

#define SET_INSTANCE_GAME_PARAMS(instance) const_SLOT(game_params, instance)

#define SET_GAME_PARAMS() SET_INSTANCE_GAME_PARAMS(instance)

#ifndef HARD_CODED_NUM_FREECELLS
    fcs_game_limit freecells_num;
#define INSTANCE_FREECELLS_NUM (instance->game_params.freecells_num)
#define LOCAL_FREECELLS_NUM (game_params.freecells_num)
#else
#define INSTANCE_FREECELLS_NUM HARD_CODED_NUM_FREECELLS
#define LOCAL_FREECELLS_NUM HARD_CODED_NUM_FREECELLS
#endif

#ifndef HARD_CODED_NUM_STACKS
    fcs_game_limit stacks_num;
#define INSTANCE_STACKS_NUM (instance->game_params.stacks_num)
#define LOCAL_STACKS_NUM (game_params.stacks_num)
#else
#define INSTANCE_STACKS_NUM HARD_CODED_NUM_STACKS
#define LOCAL_STACKS_NUM HARD_CODED_NUM_STACKS
#endif

#ifndef HARD_CODED_NUM_DECKS
    fcs_game_limit decks_num;
#define INSTANCE_DECKS_NUM (instance->game_params.decks_num)
#define LOCAL_DECKS_NUM (game_params.decks_num)
#else
#define INSTANCE_DECKS_NUM HARD_CODED_NUM_DECKS
#define LOCAL_DECKS_NUM HARD_CODED_NUM_DECKS
#endif

#ifdef FCS_FREECELL_ONLY
#define GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance)                          \
    FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#else
/* sequences_are_built_by - (bits 0:1) - what two adjacent cards in the
 * same sequence can be.
 *
 * empty_stacks_fill (bits 2:3) - with what cards can empty stacks be
 * filled with.
 *
 * unlimited_sequence_move - (bit 4) - whether an entire sequence can be
 * moved from one place to the other regardless of the number of
 * unoccupied Freecells there are.
 * */
fcs_game_limit game_flags;

#define INSTANCE_GAME_FLAGS (instance->game_params.game_flags)
#define GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance)                          \
    ((instance)->game_params.game_flags & 0x3)

#define INSTANCE_UNLIMITED_SEQUENCE_MOVE (INSTANCE_GAME_FLAGS & (1 << 4))
#define INSTANCE_EMPTY_STACKS_FILL ((INSTANCE_GAME_FLAGS >> 2) & 0x3)

#endif
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
#define FC__STACKS__SET_PARAMS() SET_GAME_PARAMS()
#else
#define FC__STACKS__SET_PARAMS()
#endif
#ifndef HARD_CODED_NUM_STACKS
#define STACKS__SET_PARAMS() SET_GAME_PARAMS()
#else
#define STACKS__SET_PARAMS()
#endif
#ifndef FCS_FREECELL_ONLY
} fcs_game_type_params;
#endif
