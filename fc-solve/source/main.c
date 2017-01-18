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
    FCS_PRESET_CODE_NOT_FOUND
} fc_solve_preset_ret_code_t;

typedef struct
{
    int preset_id;
} fcs_preset_t;

#define NUM_PRESETS 16
static const fcs_preset_t fcs_presets[NUM_PRESETS] =
{
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
    },
    {
        0,
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
        "bakers_dozen", 0,
    },
    {
        "bakers_game", 0,
    },
    {
        "beleaguered_castle", 0,
    },
    {
        "citadel", 0,
    },
    {
        "cruel", 0,
    },
    {
        "der_katzenschwanz", 0,
    },
    {
        "der_katz", 0,
    },
    {
        "die_schlange", 0,
    },
    {
        "eight_off", 0,
    },
    {
        "fan", 0,
    },
    {
        "forecell", 0,
    },
    {
        "freecell", 0,
    },
    {
        "good_measure", 0,
    },
    {
        "ko_bakers_game", 0,
    },
    {
        "kings_only_bakers_game", 0,
    },
    {
        "relaxed_freecell", 0,
    },
    {
        "relaxed_seahaven_towers", 0,
    },
    {
        "relaxed_seahaven", 0,
    },
    {
        "seahaven_towers", 0,
    },
    {
        "seahaven", 0,
    },
    {
        "simple_simon", 0,
    },
    {
        "streets_and_alleys", 0,
    },
};

static inline int fcs_get_preset_id_by_name(const char *const name)
{
    for (int i = 0; i < NUM_NAMES; i++)
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
