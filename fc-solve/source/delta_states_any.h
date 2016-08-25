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
 * delta_states_any.h - choose between the debondt and non-debondt delta_states
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FCS_DEBONDT_DELTA_STATES
#include "delta_states_debondt_impl.h"
#else
#include "delta_states_impl.h"
#endif

#ifdef FCS_DEBONDT_DELTA_STATES
#define fc_solve_delta_stater_t fc_solve_debondt_delta_stater_t
#define fc_solve_delta_stater_decode_into_state(a, b, c, d)                    \
    fc_solve_debondt_delta_stater_decode_into_state(local_variant, a, b, c, d)
#define fcs_init_and_encode_state(a, b, c, d)                                  \
    fcs_debondt_init_and_encode_state(a, b, c, d)
#ifdef FCS_FREECELL_ONLY
static GCC_INLINE void fc_solve_delta_stater_init(
    fc_solve_debondt_delta_stater_t *const delta, fcs_state_t *const init_state,
    const int num_columns, const int num_freecells)
{
    return fc_solve_debondt_delta_stater_init(delta,
        FCS_DBM_VARIANT_2FC_FREECELL, init_state, num_columns, num_freecells);
}
#else
static GCC_INLINE void fc_solve_delta_stater_init(
    fc_solve_debondt_delta_stater_t *const delta, fcs_state_t *const init_state,
    const int num_columns, const int num_freecells,
    const int sequences_are_built_by)
{
    return fc_solve_debondt_delta_stater_init(delta,
        FCS_DBM_VARIANT_2FC_FREECELL, init_state, num_columns, num_freecells,
        sequences_are_built_by);
}
#endif
#define fc_solve_delta_stater_release(a)                                       \
    fc_solve_debondt_delta_stater_release(a)
#endif

/****************************************************/
/* compare_enc_states */

#ifdef FCS_DEBONDT_DELTA_STATES

static GCC_INLINE int compare_enc_states(
    const fcs_encoded_state_buffer_t *a, const fcs_encoded_state_buffer_t *b)
{
    return memcmp(a, b, sizeof(*a));
}

#else

static GCC_INLINE int compare_enc_states(
    const fcs_encoded_state_buffer_t *a, const fcs_encoded_state_buffer_t *b)
{
    if (a->s[0] < b->s[0])
    {
        return -1;
    }
    else if (a->s[0] > b->s[0])
    {
        return 1;
    }
    else
    {
        return memcmp(a->s, b->s, a->s[0] + 1);
    }
}

#endif

#ifdef __cplusplus
}
#endif
