/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
// Common t/delta-states-test.c code.
#pragma once
#include <string.h>
#include <tap.h>

#ifndef FCS_COMPILE_DEBUG_FUNCTIONS
#define FCS_COMPILE_DEBUG_FUNCTIONS
#endif

#include "card.c"
#include "state.c"
#include "delta_states_any.h"
#include "indirect_buffer.h"
#include "trim_trailing_whitespace.h"
#include "render_state.h"
#ifdef FCS_FREECELL_ONLY
#include "is_parent.c"
#endif

static bool test_encode_and_decode(const fcs_dbm_variant_type local_variant,
    fcs_delta_stater *const delta, fcs_state_keyval_pair *const state,
    const char *const expected_str, const char *const blurb)
{
    fcs_state_keyval_pair new_derived_state;
    fcs_encoded_state_buffer enc_state;
    DECLARE_IND_BUF_T(new_derived_indirect_stacks_buffer)
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);

    fcs_init_and_encode_state(delta, local_variant, state, &enc_state);

    fc_solve_delta_stater_decode_into_state(delta, (unsigned char *)&enc_state,
        &(new_derived_state), new_derived_indirect_stacks_buffer);

    char as_str[1000];
    FCS__RENDER_STATE(as_str, &(new_derived_state.s), &locs);
    trim_trailing_whitespace(as_str);

    const bool verdict = ok(!strcmp(as_str, expected_str), "%s", blurb);
    if (!verdict)
    {
        diag("got == <<<\n%s\n>>> ; expected == <<<\n%s\n>>>\n", as_str,
            expected_str);
    }
    return verdict;
}
