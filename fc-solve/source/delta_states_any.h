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
#define fcs_delta_stater fc_solve_debondt_delta_stater_t
#define fc_solve_delta_stater_decode_into_state(a, b, c, d)                    \
    fc_solve_debondt_delta_stater_decode_into_state(local_variant, a, b, c, d)
#define fcs_init_and_encode_state(a, b, c, d)                                  \
    fcs_debondt_init_and_encode_state(a, b, c, d)
#ifdef FCS_FREECELL_ONLY
static inline void fc_solve_delta_stater_init(
    fc_solve_debondt_delta_stater_t *const delta, fcs_state_t *const init_state,
    const size_t num_columns, const int num_freecells)
{
    fc_solve_debondt_delta_stater_init(delta, FCS_DBM_VARIANT_2FC_FREECELL,
        init_state, num_columns, num_freecells);
}
#else
static inline void fc_solve_delta_stater_init(
    fc_solve_debondt_delta_stater_t *const delta, fcs_state_t *const init_state,
    const size_t num_columns, const int num_freecells,
    const int sequences_are_built_by)
{
    fc_solve_debondt_delta_stater_init(delta, FCS_DBM_VARIANT_2FC_FREECELL,
        init_state, num_columns, num_freecells, sequences_are_built_by);
}
#endif
#define fc_solve_delta_stater_release(a)                                       \
    fc_solve_debondt_delta_stater_release(a)
#endif

    /****************************************************/
    /* compare_enc_states */

#ifdef FCS_DEBONDT_DELTA_STATES

static inline int compare_enc_states(
    const fcs_encoded_state_buffer_t *a, const fcs_encoded_state_buffer_t *b)
{
    return memcmp(a, b, sizeof(*a));
}

#else

static inline int compare_enc_states(
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
