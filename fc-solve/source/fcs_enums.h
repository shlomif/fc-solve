/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// fcs_enums.h - header file for various Freecell Solver Enumerations. Common
// to the main program headers and to the library headers.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    FCS_ES_FILLED_BY_ANY_CARD,
    FCS_ES_FILLED_BY_KINGS_ONLY,
    FCS_ES_FILLED_BY_NONE
};

enum
{
    FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
    FCS_SEQ_BUILT_BY_SUIT,
    FCS_SEQ_BUILT_BY_RANK
};

enum
{
    FCS_TALON_NONE,
    FCS_TALON_GYPSY,
    FCS_TALON_KLONDIKE
};

typedef enum
{
    FCS_STATE_WAS_SOLVED,
    FCS_STATE_IS_NOT_SOLVEABLE,
    FCS_STATE_ALREADY_EXISTS,
    FCS_STATE_EXCEEDS_MAX_NUM_TIMES,
    FCS_STATE_BEGIN_SUSPEND_PROCESS,
    FCS_STATE_SUSPEND_PROCESS,
    FCS_STATE_EXCEEDS_MAX_DEPTH,
    FCS_STATE_ORIGINAL_STATE_IS_NOT_SOLVEABLE,
    FCS_STATE_INVALID_STATE,
    FCS_STATE_NOT_BEGAN_YET,
    FCS_STATE_DOES_NOT_EXIST,
    FCS_STATE_OPTIMIZED,
    FCS_STATE_FLARES_PLAN_ERROR
} fc_solve_solve_process_ret_t;

typedef enum
{
    FCS_PRESET_CODE_OK,
    FCS_PRESET_CODE_NOT_FOUND,
    FCS_PRESET_CODE_FREECELLS_EXCEED_MAX,
    FCS_PRESET_CODE_STACKS_EXCEED_MAX,
    FCS_PRESET_CODE_DECKS_EXCEED_MAX
} fc_solve_preset_ret_code_t;

enum
{
    FC_SOLVE__STANDARD_NOTATION_NO = 0,
    FC_SOLVE__STANDARD_NOTATION_REGULAR = 1,
    FC_SOLVE__STANDARD_NOTATION_EXTENDED = 2
};

#define FCS_METHOD_HARD_DFS 0
#define FCS_METHOD_SOFT_DFS 1
#define FCS_METHOD_BFS 2
#define FCS_METHOD_A_STAR 3
#define FCS_METHOD_RANDOM_DFS 5
#define FCS_METHOD_PATSOLVE 6

#define FCS_NUM_BEFS_WEIGHTS 6

#ifdef __cplusplus
}
#endif
