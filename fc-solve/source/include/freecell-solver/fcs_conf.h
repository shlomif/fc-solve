// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2018 Shlomi Fish
// fcs_conf.h - additional config helpers based on config.h to avoid keeping
// it cluttered.
#pragma once

#include "freecell-solver/config.h"
#ifdef __cplusplus
extern "C" {
#endif

#if MAX_NUM_DECKS != 1
#error MAX_NUM_DECKS must be 1!
#endif

#ifndef FCS_DISABLE_MULTI_FLARES
#define FCS_WITH_FLARES
#endif

#ifndef FCS_DISABLE_MULTI_NEXT_INSTS
#define FCS_WITH_NI
#endif

#ifndef FCS_DISABLE_MOVES_TRACKING
#define FCS_WITH_MOVES
#endif

#ifdef FCS_WITH_MOVES
#define SFS__PASS_MOVE_STACK(arg) , arg
#else
#define SFS__PASS_MOVE_STACK(arg)
#endif

#ifdef FCS_FREECELL_ONLY
#ifndef FCS_DISABLE_SIMPLE_SIMON
#define FCS_DISABLE_SIMPLE_SIMON
#endif
#define FCS_ON_NOT_FC_ONLY(code)
#define PASS_ON_NOT_FC_ONLY(param)
#else
#define FCS_ON_NOT_FC_ONLY(code) code
#define PASS_ON_NOT_FC_ONLY(param) , param
#endif

#ifndef FCS_INT_BIT_SIZE_LOG2
#define FCS_INT_BIT_SIZE_LOG2                                                  \
    ((sizeof(int) == 8) ? 6 : (sizeof(int) == 2) ? 4 : 5)
#endif

#if defined(FCS_ZERO_FREECELLS_MODE) && (MAX_NUM_FREECELLS != 0)
#error FCS_ZERO_FREECELLS_MODE requires zero freecells
#endif

#if defined(FCS_RCS_STATES) && (!defined(COMPACT_STATES))
#error FCS_RCS_STATES requires COMPACT_STATES
#endif

#ifndef WIN32
#define HAVE_GETLINE 1
#endif

#if (defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) &&    \
     defined(HARD_CODED_NUM_DECKS))
#define HARD_CODED_ALL
#define HARD__SET_GAME_PARAMS()
#else
#define HARD__SET_GAME_PARAMS() SET_GAME_PARAMS()
#endif
#ifdef FCS_ENABLE_PRUNE__R_TF__UNCOND
#define freecell_solver_user_set_pruning(...)
#endif

#ifdef __cplusplus
};
#endif
