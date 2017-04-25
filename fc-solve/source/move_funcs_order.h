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

#include <string.h>

#include "fcs_back_compat.h"
#include "instance.h"

static inline int fc_solve_string_to_test_num_compare_func(
    const void *const a, const void *const b)
{
#define MAP(x) (((const fcs_move_func_aliases_mapping_t *const)(x))->alias)
    return strcmp(MAP(a), MAP(b));
}
#undef MAP

static inline int fc_solve_string_to_test_num(const char *const s)
{
    const fcs_move_func_aliases_mapping_t needle = {.alias = {s[0], '\0'}};
    const fcs_move_func_aliases_mapping_t *const result =
        (fcs_move_func_aliases_mapping_t *)bsearch(&needle,
            fc_solve_sfs_move_funcs_aliases, FCS_MOVE_FUNCS_ALIASES_NUM,
            sizeof(fc_solve_sfs_move_funcs_aliases[0]),
            fc_solve_string_to_test_num_compare_func);

    return (result ? result->move_func_num : 0);
}

extern int fc_solve_apply_tests_order(fcs_tests_order_t *tests_order,
    const char *string FCS__PASS_ERR_STR(char *const error_string));

#ifdef __cplusplus
}
#endif
