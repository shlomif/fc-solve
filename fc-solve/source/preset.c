/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// preset.c - game presets management
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
    seq_move, empty_stacks_fill)                                               \
    {                                                                          \
        freecells_num, stacks_num, decks_num,                                  \
            (seqs_build_by | (empty_stacks_fill << 2) | (seq_move << 4))       \
    }

#include "fcs_presets_generated.h"

typedef struct
{
    const char name[32];
    int preset_id;
} preset_name;

static const preset_name fcs_preset_names[23] = {
    {
        "bakers_dozen",
        FCS_PRESET_BAKERS_DOZEN,
    },
    {
        "bakers_game",
        FCS_PRESET_BAKERS_GAME,
    },
    {
        "beleaguered_castle",
        FCS_PRESET_BELEAGUERED_CASTLE,
    },
    {
        "citadel",
        FCS_PRESET_BELEAGUERED_CASTLE,
    },
    {
        "cruel",
        FCS_PRESET_CRUEL,
    },
    {
        "der_katzenschwanz",
        FCS_PRESET_DER_KATZENSCHWANZ,
    },
    {
        "der_katz",
        FCS_PRESET_DER_KATZENSCHWANZ,
    },
    {
        "die_schlange",
        FCS_PRESET_DIE_SCHLANGE,
    },
    {
        "eight_off",
        FCS_PRESET_EIGHT_OFF,
    },
    {
        "fan",
        FCS_PRESET_FAN,
    },
    {
        "forecell",
        FCS_PRESET_FORECELL,
    },
    {
        "freecell",
        FCS_PRESET_FREECELL,
    },
    {
        "good_measure",
        FCS_PRESET_GOOD_MEASURE,
    },
    {
        "ko_bakers_game",
        FCS_PRESET_KINGS_ONLY_BAKERS_GAME,
    },
    {
        "kings_only_bakers_game",
        FCS_PRESET_KINGS_ONLY_BAKERS_GAME,
    },
    {
        "relaxed_freecell",
        FCS_PRESET_RELAXED_FREECELL,
    },
    {
        "relaxed_seahaven_towers",
        FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,
    },
    {
        "relaxed_seahaven",
        FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,
    },
    {
        "seahaven_towers",
        FCS_PRESET_SEAHAVEN_TOWERS,
    },
    {
        "seahaven",
        FCS_PRESET_SEAHAVEN_TOWERS,
    },
    {
        "simple_simon",
        FCS_PRESET_SIMPLE_SIMON,
    },
    {
        "streets_and_alleys",
        FCS_PRESET_BELEAGUERED_CASTLE,
    },
};

static inline int fcs_get_preset_id_by_name(const char *const name)
{
    for (int i = 0; i < (int)COUNT(fcs_preset_names); i++)
    {
        if (!strcmp(name, fcs_preset_names[i].name))
        {
            return fcs_preset_names[i].preset_id;
        }
    }

    return -1;
}

static inline void apply_moves(fcs_moves_order *moves_order,
    const unsigned long long allowed_moves, const char *const s)
{
    FCS__DECL_ERR_BUF(no_use);
    for (size_t group_idx = 0; group_idx < moves_order->num; ++group_idx)
    {
        const fcs_move_func *const move_funcs_idxs =
            moves_order->groups[group_idx].move_funcs;
        const_AUTO(moves_order_num, moves_order->groups[group_idx].num);

        for (size_t move_i = 0; move_i < moves_order_num; move_i++)
        {
            if (!(allowed_moves & (1 << move_funcs_idxs[move_i].idx)))
            {
                fc_solve_apply_moves_order(
                    moves_order, s FCS__PASS_ERR_STR(no_use));
                return;
            }
        }
    }
}

fc_solve_preset_ret_code_t fc_solve_apply_preset_by_ptr(
    fcs_instance *const instance, const fcs_preset *const preset_ptr)
{
    FCS__DECL_ERR_BUF(no_use);
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

// This code makes sure that all the moves in all the existing
// soft threads are acceptable by the new preset.
#ifdef FCS_SINGLE_HARD_THREAD
    if (instance->next_soft_thread_id)
#endif
    {
        HT_LOOP_START()
        {
            ST_LOOP_START()
            {
                fcs_by_depth_moves_order *const by_depth_moves_order =
                    soft_thread->by_depth_moves_order.by_depth_moves;

                for (size_t depth_idx = 0;
                     depth_idx < soft_thread->by_depth_moves_order.num;
                     ++depth_idx)
                {
                    apply_moves(&by_depth_moves_order[depth_idx].moves_order,
                        preset.allowed_moves, preset.moves_order);
                }
            }
        }
    }

    // Assign the master moves order
    fc_solve_apply_moves_order(&(instance->instance_moves_order),
        preset.moves_order FCS__PASS_ERR_STR(no_use));
#undef preset
    return FCS_PRESET_CODE_OK;
}

static inline fc_solve_preset_ret_code_t fcs_get_preset_by_id(
    const int preset_id, const fcs_preset **const preset_ptr)
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
    const char *const name, const fcs_preset **const preset_ptr)
{
    return fcs_get_preset_by_id(fcs_get_preset_id_by_name(name), preset_ptr);
}
