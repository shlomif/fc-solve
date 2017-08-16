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
 * move_funcs_order.h - contains the fc_solve_apply_tests_order function.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "fcs_back_compat.h"
#include "instance.h"

static inline int fc_solve_string_to_test_num(const char needle)
{
    ssize_t l = 0, r = FCS_MOVE_FUNCS_ALIASES_NUM - 1;
    while (l <= r)
    {
        const_AUTO(m, ((l + r) >> 1));
        const_AUTO(found, fc_solve_sfs_move_funcs_aliases[m]);
        if (found.alias < needle)
        {
            l = m + 1;
        }
        else if (found.alias > needle)
        {
            r = m - 1;
        }
        else
        {
            return found.move_func_num;
        }
    }
    return 0;
}

extern int fc_solve_apply_tests_order(
    fcs_tests_order_t *, const char *FCS__PASS_ERR_STR(char *));

#ifdef __cplusplus
}
#endif
