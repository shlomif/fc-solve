/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
// delta_states.c - "delta states" are an encoding of states, where the
// states are encoded and decoded based on a compact delta from the original
// state.
#include "delta_states_any.h"
#include "render_state.h"

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS
#ifndef FCS_DEBONDT_DELTA_STATES
// The char * returned is malloc()ed and should be free()ed.
DLLEXPORT char *fc_solve_user_INTERNAL_delta_states_enc_and_dec(
    const fcs_dbm_variant_type local_variant GCC_UNUSED,
    const char *const init_state_s, const char *const derived_state_s)
{
    fcs_state_keyval_pair init_state, derived_state, new_derived_state;
    fcs_uchar enc_state[24];
    fcs_bit_reader bit_r;
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);

    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    DECLARE_IND_BUF_T(derived_stacks_buffer)
    DECLARE_IND_BUF_T(new_derived_indirect_stacks_buffer)

    fc_solve_initial_user_state_to_c(init_state_s, &init_state, FREECELLS_NUM,
        STACKS_NUM, DECKS_NUM, indirect_stacks_buffer);

    fc_solve_initial_user_state_to_c(derived_state_s, &derived_state,
        FREECELLS_NUM, STACKS_NUM, DECKS_NUM, derived_stacks_buffer);

    fcs_delta_stater delta;
    fc_solve_delta_stater_init(&delta, local_variant, &(init_state.s),
        STACKS_NUM,
        FREECELLS_NUM PASS_ON_NOT_FC_ONLY(FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));

    fc_solve_delta_stater_set_derived(&delta, &(derived_state.s));

    fc_solve_state_init(
        &new_derived_state, STACKS_NUM, new_derived_indirect_stacks_buffer);

    fc_solve_bit_writer bit_w;
    fc_solve_bit_writer_init(&bit_w, enc_state);
    fc_solve_delta_stater_encode_composite(&delta, &bit_w);

    fc_solve_bit_reader_init(&bit_r, enc_state);
    fc_solve_delta_stater_decode(&delta, &bit_r, &(new_derived_state.s));

    char *new_derived_as_str = SMALLOC(new_derived_as_str, 1000);
    FCS__RENDER_STATE(new_derived_as_str, &(new_derived_state.s), &locs);

    fc_solve_delta_stater_release(&delta);
    return new_derived_as_str;
}

#endif
#endif
