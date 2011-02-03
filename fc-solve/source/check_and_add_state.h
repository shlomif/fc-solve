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
 * header file for fc_solve_check_and_add_state() .
 */
#ifndef FC_SOLVE__CHECK_AND_ADD_STATE_H
#define FC_SOLVE__CHECK_AND_ADD_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "bool.h"
#include "state.h"
#include "instance.h"

/* #define FCS_USE_INLINE */

/*
 * check_and_add_state is defined in check_and_add_state.c.
 *
 * DFS stands for Depth First Search which is the type of scan Freecell
 * Solver uses to solve a given board.
 * */

GCC_INLINE fcs_bool_t fc_solve_check_and_add_state(
    fc_solve_hard_thread_t * hard_thread,
    fcs_pass_state_t * new_state_val,
    fcs_lvalue_pass_state_t * existing_state_val
    );

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
extern guint fc_solve_hash_function(gconstpointer key);
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FC_SOLVE__CHECK_AND_ADD_STATE_H */
