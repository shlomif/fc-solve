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
 * preset.c - game presets management for Freecell Solver
 */

#include "dll_thunk.h"

#include "instance.h"
#include "preset.h"
#include "move_funcs_order.h"

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

#include "fcs_presets_generated.h"

typedef struct
{
    const char name[32];
    int preset_id;
} fcs_preset_name_t;

static const fcs_preset_name_t fcs_preset_names[23] = {
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

#define NUM_PRESETS ((int)COUNT(fcs_preset_names))

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

fc_solve_preset_ret_code_t fc_solve_apply_preset_by_ptr(
    fc_solve_instance_t *const instance, const fcs_preset_t *const preset_ptr)
{
#ifdef FCS_WITH_ERROR_STRS
    char no_use[120];
#endif

#define preset (*preset_ptr)
    if (preset.game_params.freecells_num > MAX_NUM_FREECELLS)
    {
        return FCS_PRESET_CODE_FREECELLS_EXCEED_MAX;
    }
    if (preset.game_params.stacks_num > MAX_NUM_STACKS)
    {
        return FCS_PRESET_CODE_STACKS_EXCEED_MAX;
    }
    if (preset.game_params.decks_num > MAX_NUM_DECKS)
    {
        return FCS_PRESET_CODE_DECKS_EXCEED_MAX;
    }

    instance->game_params = preset.game_params;
#ifndef FCS_DISABLE_SIMPLE_SIMON
    instance->is_simple_simon = (preset.preset_id == FCS_PRESET_SIMPLE_SIMON);
#endif

/*
 * This code makes sure that all the tests in all the existing
 * soft threads are acceptable by the new preset.
 * */

#ifdef FCS_SINGLE_HARD_THREAD
    if (instance->next_soft_thread_id)
#endif
    {
        HT_LOOP_START()
        {
            ST_LOOP_START()
            {
                /* Check every test */

                fcs_by_depth_tests_order_t *const by_depth_tests_order =
                    soft_thread->by_depth_tests_order.by_depth_tests;

                for (int depth_idx = 0;
                     depth_idx < soft_thread->by_depth_tests_order.num;
                     depth_idx++)
                {
                    for (int group_idx = 0;
                         group_idx <
                         by_depth_tests_order[depth_idx].tests_order.num_groups;
                         group_idx++)
                    {
                        const size_t *const move_funcs_idxs =
                            by_depth_tests_order[depth_idx]
                                .tests_order.groups[group_idx]
                                .order_group_tests;
                        const_AUTO(
                            tests_order_num, by_depth_tests_order[depth_idx]
                                                 .tests_order.groups[group_idx]
                                                 .num);

                        for (size_t num_valid_move_funcs = 0;
                             num_valid_move_funcs < tests_order_num;
                             num_valid_move_funcs++)
                        {
                            const char *s;
                            for (s = preset.allowed_tests; *s != '\0'; s++)
                            {
                                const char test_name[2] = {*s, '\0'};
                                /* Check if this test corresponds to this
                                 * character */
                                if (move_funcs_idxs[num_valid_move_funcs] ==
                                    fc_solve_string_to_test_num(test_name))
                                {
                                    break;
                                }
                            }
                            /* If the end of the string was reached, it means
                             * this test is unacceptable by this preset. */
                            if (*s == '\0')
                            {
                                fc_solve_apply_tests_order(
                                    &(by_depth_tests_order[depth_idx]
                                            .tests_order),
                                    preset.tests_order FCS__PASS_ERR_STR(
                                        no_use));
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    /* Assign the master tests order */
    fc_solve_apply_tests_order(&(instance->instance_tests_order),
        preset.tests_order FCS__PASS_ERR_STR(no_use));
#undef preset
    return FCS_PRESET_CODE_OK;
}

static inline fc_solve_preset_ret_code_t fcs_get_preset_by_id(
    const int preset_id, const fcs_preset_t **const preset_ptr)
{
    for (size_t preset_index = 0; preset_index < COUNT(fcs_presets);
         preset_index++)
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
