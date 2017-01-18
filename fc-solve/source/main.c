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
 * main.c - the main() and related functions of the fc-solve command line
 * executable.
 *
 * It is documented in the documents "README", "USAGE", etc. in the
 * Freecell Solver distribution from http://fc-solve.shlomifish.org/ .
 */
#include <string.h>

typedef unsigned char fcs_game_limit_t;

typedef struct
{
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
    fcs_game_limit_t freecells_num;
#define INSTANCE_FREECELLS_NUM (instance->game_params.freecells_num)
#define LOCAL_FREECELLS_NUM (game_params.freecells_num)
#else
#define INSTANCE_FREECELLS_NUM HARD_CODED_NUM_FREECELLS
#define LOCAL_FREECELLS_NUM HARD_CODED_NUM_FREECELLS
#endif

#ifndef HARD_CODED_NUM_STACKS
    fcs_game_limit_t stacks_num;
#define INSTANCE_STACKS_NUM (instance->game_params.stacks_num)
#define LOCAL_STACKS_NUM (game_params.stacks_num)
#else
#define INSTANCE_STACKS_NUM HARD_CODED_NUM_STACKS
#define LOCAL_STACKS_NUM HARD_CODED_NUM_STACKS
#endif

#ifndef HARD_CODED_NUM_DECKS
    fcs_game_limit_t decks_num;
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
    fcs_game_limit_t game_flags;

#define INSTANCE_GAME_FLAGS (instance->game_params.game_flags)
#define GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance)                          \
    ((instance)->game_params.game_flags & 0x3)

#define INSTANCE_UNLIMITED_SEQUENCE_MOVE (INSTANCE_GAME_FLAGS & (1 << 4))
#define INSTANCE_EMPTY_STACKS_FILL ((INSTANCE_GAME_FLAGS >> 2) & 0x3)

#endif

} fcs_game_type_params_t;
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

enum
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
};

typedef enum {
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

#define FCS_METHOD_NONE -1
#define FCS_METHOD_HARD_DFS 0
#define FCS_METHOD_SOFT_DFS 1
#define FCS_METHOD_BFS 2
#define FCS_METHOD_A_STAR 3
#define FCS_METHOD_OPTIMIZE 4
#define FCS_METHOD_RANDOM_DFS 5
#define FCS_METHOD_PATSOLVE 6

#define FCS_NUM_BEFS_WEIGHTS 6
#define FCS_MOVE_FUNCS_NUM 199

typedef struct
{
    int preset_id;

    fcs_game_type_params_t game_params;

    char tests_order[FCS_MOVE_FUNCS_NUM * 3 + 1];
    char allowed_tests[FCS_MOVE_FUNCS_NUM * 3 + 1];
} fcs_preset_t;

enum
{
    FCS_PRESET_BAKERS_DOZEN,
    FCS_PRESET_BAKERS_GAME,
    FCS_PRESET_CRUEL,
    FCS_PRESET_DER_KATZENSCHWANZ,
    FCS_PRESET_DIE_SCHLANGE,
    FCS_PRESET_EIGHT_OFF,
    FCS_PRESET_FAN,
    FCS_PRESET_FORECELL,
    FCS_PRESET_FREECELL,
    FCS_PRESET_GOOD_MEASURE,
    FCS_PRESET_KINGS_ONLY_BAKERS_GAME,
    FCS_PRESET_RELAXED_FREECELL,
    FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,
    FCS_PRESET_SEAHAVEN_TOWERS,
    FCS_PRESET_SIMPLE_SIMON,
    FCS_PRESET_BELEAGUERED_CASTLE
};

#define MAKE_GAME_PARAMS(freecells_num, stacks_num, decks_num, seqs_build_by,  \
                         seq_move, empty_stacks_fill)                          \
    {                                                                          \
        freecells_num, stacks_num, decks_num,                                  \
            (seqs_build_by | (empty_stacks_fill << 2) | (seq_move << 4))       \
    }

static const fcs_preset_t fcs_presets[16] =
{
    {
        FCS_PRESET_BAKERS_DOZEN,
        MAKE_GAME_PARAMS(
            0,
            13,
            1,

            FCS_SEQ_BUILT_BY_RANK,
            0,
            FCS_ES_FILLED_BY_NONE
        ),

        "0123456789",
        "0123456789"
    },
    {
        FCS_PRESET_BAKERS_GAME,
        MAKE_GAME_PARAMS(
            4,
            8,
            1,

            FCS_SEQ_BUILT_BY_SUIT,
            0,
            FCS_ES_FILLED_BY_ANY_CARD
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_BELEAGUERED_CASTLE,
        MAKE_GAME_PARAMS(
            0,
            8,
            1,

            FCS_SEQ_BUILT_BY_RANK,
            0,
            FCS_ES_FILLED_BY_ANY_CARD
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_CRUEL,
        MAKE_GAME_PARAMS(
            0,
            12,
            1,

            FCS_SEQ_BUILT_BY_SUIT,
            0,
            FCS_ES_FILLED_BY_NONE
        ),

        "0123456789",
        "0123456789"
    },
    {
        FCS_PRESET_DER_KATZENSCHWANZ,
        MAKE_GAME_PARAMS(
            8,
            9,
            2,

            FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
            1,
            FCS_ES_FILLED_BY_NONE
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_DIE_SCHLANGE,
        MAKE_GAME_PARAMS(
            8,
            9,
            2,

            FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
            0,
            FCS_ES_FILLED_BY_NONE
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_EIGHT_OFF,
        MAKE_GAME_PARAMS(
            8,
            8,
            1,

            FCS_SEQ_BUILT_BY_SUIT,
            0,
            FCS_ES_FILLED_BY_KINGS_ONLY
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_FAN,
        MAKE_GAME_PARAMS(
            0,
            18,
            1,

            FCS_SEQ_BUILT_BY_SUIT,
            0,
            FCS_ES_FILLED_BY_KINGS_ONLY
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_FORECELL,
        MAKE_GAME_PARAMS(
            4,
            8,
            1,

            FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
            0,
            FCS_ES_FILLED_BY_KINGS_ONLY
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_FREECELL,
        MAKE_GAME_PARAMS(
            4,
            8,
            1,

            FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
            0,
            FCS_ES_FILLED_BY_ANY_CARD
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_GOOD_MEASURE,
        MAKE_GAME_PARAMS(
            0,
            10,
            1,

            FCS_SEQ_BUILT_BY_RANK,
            0,
            FCS_ES_FILLED_BY_NONE
        ),

        "0123456789",
        "0123456789"
    },
    {
        FCS_PRESET_KINGS_ONLY_BAKERS_GAME,
        MAKE_GAME_PARAMS(
            4,
            8,
            1,

            FCS_SEQ_BUILT_BY_SUIT,
            0,
            FCS_ES_FILLED_BY_KINGS_ONLY
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_RELAXED_FREECELL,
        MAKE_GAME_PARAMS(
            4,
            8,
            1,

            FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
            1,
            FCS_ES_FILLED_BY_ANY_CARD
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,
        MAKE_GAME_PARAMS(
            4,
            10,
            1,

            FCS_SEQ_BUILT_BY_SUIT,
            1,
            FCS_ES_FILLED_BY_KINGS_ONLY
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_SEAHAVEN_TOWERS,
        MAKE_GAME_PARAMS(
            4,
            10,
            1,

            FCS_SEQ_BUILT_BY_SUIT,
            0,
            FCS_ES_FILLED_BY_KINGS_ONLY
        ),

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_SIMPLE_SIMON,
        MAKE_GAME_PARAMS(
            0,
            10,
            1,

            FCS_SEQ_BUILT_BY_SUIT,
            0,
            FCS_ES_FILLED_BY_ANY_CARD
        ),

        "abcdefgh",
        "abcdefghi"
    }
};

typedef struct
{
    const char name[32];
    int preset_id;
} fcs_preset_name_t;

#define NUM_NAMES 23
static const fcs_preset_name_t fcs_preset_names[NUM_NAMES] = {
    {
        "bakers_dozen", FCS_PRESET_BAKERS_DOZEN,
    },
    {
        "bakers_game", FCS_PRESET_BAKERS_GAME,
    },
    {
        "beleaguered_castle", FCS_PRESET_BELEAGUERED_CASTLE,
    },
    {
        "citadel", FCS_PRESET_BELEAGUERED_CASTLE,
    },
    {
        "cruel", FCS_PRESET_CRUEL,
    },
    {
        "der_katzenschwanz", FCS_PRESET_DER_KATZENSCHWANZ,
    },
    {
        "der_katz", FCS_PRESET_DER_KATZENSCHWANZ,
    },
    {
        "die_schlange", FCS_PRESET_DIE_SCHLANGE,
    },
    {
        "eight_off", FCS_PRESET_EIGHT_OFF,
    },
    {
        "fan", FCS_PRESET_FAN,
    },
    {
        "forecell", FCS_PRESET_FORECELL,
    },
    {
        "freecell", FCS_PRESET_FREECELL,
    },
    {
        "good_measure", FCS_PRESET_GOOD_MEASURE,
    },
    {
        "ko_bakers_game", FCS_PRESET_KINGS_ONLY_BAKERS_GAME,
    },
    {
        "kings_only_bakers_game", FCS_PRESET_KINGS_ONLY_BAKERS_GAME,
    },
    {
        "relaxed_freecell", FCS_PRESET_RELAXED_FREECELL,
    },
    {
        "relaxed_seahaven_towers", FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,
    },
    {
        "relaxed_seahaven", FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,
    },
    {
        "seahaven_towers", FCS_PRESET_SEAHAVEN_TOWERS,
    },
    {
        "seahaven", FCS_PRESET_SEAHAVEN_TOWERS,
    },
    {
        "simple_simon", FCS_PRESET_SIMPLE_SIMON,
    },
    {
        "streets_and_alleys", FCS_PRESET_BELEAGUERED_CASTLE,
    },
};

#define NUM_PRESETS NUM_NAMES

static inline int fcs_get_preset_id_by_name(const char *const name)
{
    for (int i = 0; i < NUM_PRESETS; i++)
    {
        if (!strcmp(name, fcs_preset_names[i].name))
        {
            return fcs_preset_names[i].preset_id;
        }
    }

    return -1;
}

static inline fc_solve_preset_ret_code_t fcs_get_preset_by_id(
    const int preset_id, const fcs_preset_t **const preset_ptr)
{
    for (size_t preset_index = 0; preset_index < NUM_PRESETS; preset_index++)
    {
        if (fcs_presets[preset_index].preset_id == preset_id)
        {
            *preset_ptr = &(fcs_presets[preset_index]);
            return FCS_PRESET_CODE_OK;
        }
    }

    return FCS_PRESET_CODE_NOT_FOUND;
}

fc_solve_preset_ret_code_t fc_solve_get_preset_by_name(
    const char *const name, const fcs_preset_t **const preset_ptr)
{
    return fcs_get_preset_by_id(fcs_get_preset_id_by_name(name), preset_ptr);
}

int main(int argc, char *argv[])
{
    const fcs_preset_t *new_preset_ptr;
    const fc_solve_preset_ret_code_t status1 = fc_solve_get_preset_by_name("notexist", &new_preset_ptr);
    if (status1 != FCS_PRESET_CODE_OK)
    {
        return status1;
    }
    return 0;
}
