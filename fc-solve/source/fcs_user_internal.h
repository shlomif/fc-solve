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
 * fcs_user_internal.h - header files for the routines of fcs_user.h for
 * internal use.
 */
#pragma once

#include "freecell-solver/fcs_dllexport.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS
double DLLEXPORT fc_solve_user_INTERNAL_get_befs_weight(
    void *api_instance, int my_index);

int DLLEXPORT fc_solve_user_INTERNAL_compile_all_flares_plans(
    void *api_instance, char **error_string);

int DLLEXPORT fc_solve_user_INTERNAL_get_flares_plan_num_items(
    void *api_instance);

const DLLEXPORT char *fc_solve_user_INTERNAL_get_flares_plan_item_type(
    void *api_instance, int item_idx);

int DLLEXPORT fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(
    void *api_instance, int item_idx);

int DLLEXPORT fc_solve_user_INTERNAL_get_flares_plan_item_iters_count(
    void *api_instance, int item_idx);

int DLLEXPORT fc_solve_user_INTERNAL_get_num_by_depth_tests_order(
    void *api_instance);

int DLLEXPORT fc_solve_user_INTERNAL_get_by_depth_tests_max_depth(
    void *const api_instance, const int depth_idx);

#endif

#ifdef __cplusplus
}
#endif
