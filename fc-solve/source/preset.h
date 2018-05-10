/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// preset.h - header file of the preset management functions for Freecell
// Solver. This header is internal.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "fcs_conf.h"

#ifndef FCS_FREECELL_ONLY
#include "instance.h"

typedef struct
{
    unsigned long long allowed_moves;
    int preset_id;

    fcs_game_type_params game_params;

    char moves_order[FCS_MOVE_FUNCS_NUM * 3 + 1];
} fcs_preset;

extern fc_solve_preset_ret_code_t fc_solve_apply_preset_by_ptr(
    fcs_instance *, const fcs_preset *);

extern fc_solve_preset_ret_code_t fc_solve_get_preset_by_name(
    const char *, const fcs_preset **);

#define fcs_duplicate_preset(d, s) ((d) = (s))

#endif /* FCS_FREECELL_ONLY */

#ifdef __cplusplus
}
#endif
