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

typedef enum {
    FCS_PRESET_CODE_OK,
    FCS_PRESET_CODE_NOT_FOUND,
    FCS_PRESET_CODE_FREECELLS_EXCEED_MAX,
    FCS_PRESET_CODE_STACKS_EXCEED_MAX,
    FCS_PRESET_CODE_DECKS_EXCEED_MAX
} fc_solve_preset_ret_code_t;

#define FCS_NUM_BEFS_WEIGHTS 6
#define FCS_MOVE_FUNCS_NUM 199

typedef struct
{
    int preset_id;

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

static const fcs_preset_t fcs_presets[16] =
{
    {
        FCS_PRESET_BAKERS_DOZEN,

        "0123456789",
        "0123456789"
    },
    {
        FCS_PRESET_BAKERS_GAME,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_BELEAGUERED_CASTLE,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_CRUEL,

        "0123456789",
        "0123456789"
    },
    {
        FCS_PRESET_DER_KATZENSCHWANZ,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_DIE_SCHLANGE,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_EIGHT_OFF,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_FAN,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_FORECELL,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_FREECELL,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_GOOD_MEASURE,

        "0123456789",
        "0123456789"
    },
    {
        FCS_PRESET_KINGS_ONLY_BAKERS_GAME,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_RELAXED_FREECELL,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_SEAHAVEN_TOWERS,

        "[01][23456789]",
        "0123456789ABCDE"
    },
    {
        FCS_PRESET_SIMPLE_SIMON,

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
