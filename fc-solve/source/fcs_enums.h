/*
 * fcs_enums.h - header file for various Freecell Solver Enumertaions. Common
 * to the main program headers and to the library headers.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#ifndef __FCS_ENUMS_H
#define __FCS_ENUMS_H

#ifdef __cplusplus
extern "C" {
#endif

enum FCS_EMPTY_STACKS_FILL_T
{
    FCS_ES_FILLED_BY_ANY_CARD,
    FCS_ES_FILLED_BY_KINGS_ONLY,
    FCS_ES_FILLED_BY_NONE
};

enum FCS_SEQUENCES_ARE_BUILT_BY_T
{
    FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
    FCS_SEQ_BUILT_BY_SUIT,
    FCS_SEQ_BUILT_BY_RANK
};

enum FCS_TALON_T
{
    FCS_TALON_NONE,
    FCS_TALON_GYPSY,
    FCS_TALON_KLONDIKE
};

enum freecell_solver_state_solving_return_codes
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
};

enum fcs_presets_return_codes
{
    FCS_PRESET_CODE_OK,
    FCS_PRESET_CODE_NOT_FOUND,
    FCS_PRESET_CODE_FREECELLS_EXCEED_MAX,
    FCS_PRESET_CODE_STACKS_EXCEED_MAX,
    FCS_PRESET_CODE_DECKS_EXCEED_MAX
};


#define FCS_METHOD_NONE -1
#define FCS_METHOD_HARD_DFS 0
#define FCS_METHOD_SOFT_DFS 1
#define FCS_METHOD_BFS 2
#define FCS_METHOD_A_STAR 3
#define FCS_METHOD_OPTIMIZE 4
#define FCS_METHOD_RANDOM_DFS 5


#ifdef __cplusplus
}
#endif

#endif /* __FCS_ENUMS_H */
