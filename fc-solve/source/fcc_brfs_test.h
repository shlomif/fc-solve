/* Copyright (c) 2012 Shlomi Fish
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
 * dbm_calc_derived_iface.h - the public interface to dbm_calc_derived.h
 *
 */
#ifndef FC_SOLVE_FCC_BRFS_TEST_H
#define FC_SOLVE_FCC_BRFS_TEST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "fcs_dllexport.h"
#include "bool.h"

typedef unsigned char fcs_fcc_move_t;

typedef struct 
{
    int count_moves;
    fcs_fcc_move_t * moves;
    char * state_as_string;
} fcs_FCC_start_point_result_t;

DLLEXPORT int fc_solve_user_INTERNAL_find_fcc_start_points(
        const char * init_state_str_proto,
        const int count_start_state_moves,
        const fcs_fcc_move_t * const start_state_moves,
        fcs_FCC_start_point_result_t * * out_fcc_start_points
        );

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_FCC_BRFS_TEST_H */
