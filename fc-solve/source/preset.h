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
 * preset.h - header file of the preset management functions for Freecell
 * Solver.
 * This header is internal to Freecell Solver.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#ifndef FCS_FREECELL_ONLY
#include "instance.h"
#include "move_funcs_maps.h"

typedef struct
{
    int preset_id;

    fcs_game_type_params_t game_params;

    char tests_order[FCS_MOVE_FUNCS_NUM * 3 + 1];
    char allowed_tests[FCS_MOVE_FUNCS_NUM * 3 + 1];
} fcs_preset_t;

extern fc_solve_preset_ret_code_t fc_solve_apply_preset_by_ptr(
    fc_solve_instance_t *, const fcs_preset_t *);

extern fc_solve_preset_ret_code_t fc_solve_get_preset_by_name(
    const char *, const fcs_preset_t **);

static inline fc_solve_preset_ret_code_t fc_solve_apply_preset_by_name(
    fc_solve_instance_t *const instance, const char *const name)
{
    const fcs_preset_t *preset_ptr;
    const_AUTO(ret, fc_solve_get_preset_by_name(name, &preset_ptr));
    if (ret != FCS_PRESET_CODE_OK)
    {
        return ret;
    }
    return fc_solve_apply_preset_by_ptr(instance, preset_ptr);
}

#define fcs_duplicate_preset(d, s) ((d) = (s))

#endif /* FCS_FREECELL_ONLY */

#ifdef __cplusplus
}
#endif
