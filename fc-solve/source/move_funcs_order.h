/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// move_funcs_order.h - contains the fc_solve_apply_moves_order function.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "freecell-solver/fcs_back_compat.h"
#include "instance.h"

static inline int fc_solve_string_to_move_num(const char needle)
{
    return fc_solve_sfs_move_funcs_aliases[(size_t)(uint8_t)needle];
}

extern int fc_solve_apply_moves_order(
    fcs_moves_order *, const char *FCS__PASS_ERR_STR(char *));

#ifdef __cplusplus
}
#endif
