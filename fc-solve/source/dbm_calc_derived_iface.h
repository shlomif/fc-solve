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
#ifndef FC_SOLVE_DBM_CALC_DERIVED_IFACE_H
#define FC_SOLVE_DBM_CALC_DERIVED_IFACE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "fcs_dllexport.h"
#include "bool.h"

typedef struct
{
    char * state_string;
    unsigned char move;
    fcs_bool_t is_reversible_move;
} fcs_derived_state_debug_t;

DLLEXPORT int fc_solve_user_INTERNAL_calc_derived_states_wrapper(
        const char * init_state_str_proto,
        int * num_out_derived_states,
        fcs_derived_state_debug_t * * out_derived_states,
        const fcs_bool_t perform_horne_prune
        );

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_DBM_CALC_DERIVED_IFACE_H */
