/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// delta_states_any.h - choose between the debondt and non-debondt delta_states
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef FCS_DEBONDT_DELTA_STATES
#include "delta_states_debondt_impl.h"
#else
#include "delta_states_impl.h"
#endif

#ifdef INDIRECT_STACK_STATES
#define fc_solve_delta_stater_decode_into_state(                               \
    delta_stater, enc_state, state_ptr, indirect_stacks_buffer)                \
    fc_solve_delta_stater_decode_into_state_proto(local_variant, delta_stater, \
        enc_state, state_ptr, indirect_stacks_buffer)
#else
#define fc_solve_delta_stater_decode_into_state(                               \
    delta_stater, enc_state, state_ptr, indirect_stacks_buffer)                \
    fc_solve_delta_stater_decode_into_state_proto(                             \
        local_variant, delta_stater, enc_state, state_ptr)
#endif

static inline void fcs_init_and_encode_state(
    fcs_delta_stater *const delta_stater,
    const fcs_dbm_variant_type local_variant,
    fcs_state_keyval_pair *const state,
    fcs_encoded_state_buffer *const enc_state)
{
    fcs_init_encoded_state(enc_state);

    fc_solve_delta_stater_encode_into_buffer(
        delta_stater, local_variant, state, enc_state->s);
}

/****************************************************/
/* compare_enc_states */

#ifdef FCS_DEBONDT_DELTA_STATES

static inline int compare_enc_states(
    const fcs_encoded_state_buffer *a, const fcs_encoded_state_buffer *b)
{
    return memcmp(a, b, sizeof(*a));
}

#else

static inline int compare_enc_states(
    const fcs_encoded_state_buffer *a, const fcs_encoded_state_buffer *b)
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
