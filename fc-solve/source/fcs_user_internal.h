/* Copyright (c) 2000 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * fcs_user.h - main header file for the Freecell Solver library.
 *
 */
#ifndef FC_SOLVE__FCS_USER_INTERNAL_H
#define FC_SOLVE__FCS_USER_INTERNAL_H

#include "fcs_dllexport.h"
#include "fcs_enums.h"
#include "fcs_move.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS
double DLLEXPORT fc_solve_user_INTERNAL_get_befs_weight(
    void * api_instance,
    int index
    );

int DLLEXPORT fc_solve_user_INTERNAL_compile_all_flares_plans(
    void * api_instance,
    int * instance_list_index,
    char * * error_string
    );

int DLLEXPORT fc_solve_user_INTERNAL_get_flares_plan_num_items(
    void * api_instance
    );

const DLLEXPORT char * fc_solve_user_INTERNAL_get_flares_plan_item_type(
    void * api_instance,
    int item_idx
    );

int DLLEXPORT fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(
    void * api_instance,
    int item_idx
    );

int DLLEXPORT fc_solve_user_INTERNAL_get_flares_plan_item_iters_count(
    void * api_instance,
    int item_idx
    );

int DLLEXPORT fc_solve_user_INTERNAL_get_num_by_depth_tests_order(
    void * api_instance
    );

int DLLEXPORT fc_solve_user_INTERNAL_get_by_depth_tests_max_depth(
    void * api_instance,
    int depth_idx
    );

#endif

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__FCS_USER_INTERNAL_H */

