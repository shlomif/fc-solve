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
 * delta_states_iface.h - some public interface functions for delta_state.c
 * (without all the baggage).
 *
 */

#ifndef FC_SOLVE__DELTA_STATES_ANY_H
#define FC_SOLVE__DELTA_STATES_ANY_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FCS_DEBONDT_DELTA_STATES
#include "delta_states_debondt.c"
#else
#include "delta_states.c"
#endif

#ifdef FCS_DEBONDT_DELTA_STATES
#define fc_solve_delta_stater_t fc_solve_debondt_delta_stater_t
#define fc_solve_delta_stater_decode_into_state(a,b,c,d) fc_solve_debondt_delta_stater_decode_into_state(a,b,c,d)
#define fcs_init_and_encode_state(a,b,c) fcs_debondt_init_and_encode_state(a,b,c)
#ifdef FCS_FREECELL_ONLY
#define fc_solve_delta_stater_alloc(a,b,c) fc_solve_debondt_delta_stater_alloc(a,b,c)
#else
#define fc_solve_delta_stater_alloc(a,b,c,d) fc_solve_debondt_delta_stater_alloc(a,b,c,d)
#endif
#define fc_solve_delta_stater_free(a) fc_solve_debondt_delta_stater_free(a)
#endif

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__DELTA_STATES_ANY_H */
