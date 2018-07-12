/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// delta_states_debondt.h - header file for the DeBondt delta states encoding.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "delta_states.h"
#include "var_base_writer.h"
#include "var_base_reader.h"
#include "dbm_common.h"

#ifdef FCS_DEBONDT_DELTA_STATES
#define CARD_ARRAY_LEN ((RANK_KING + 1) * FCS_NUM_SUITS)

typedef struct
{
    FCS_ON_NOT_FC_ONLY(int sequences_are_built_by);
    int num_freecells;
    size_t num_columns;
    fcs_state *init_state, *derived_state;
    int bits_per_orig_cards_in_column;
    int card_states[CARD_ARRAY_LEN];
    int8_t bakers_dozen_topmost_cards_lookup[((1 << 6) / 8) + 1];
    fcs_var_base_reader r;
    fcs_var_base_writer w;
} fcs_delta_stater;

extern char *fc_solve_user_INTERNAL_debondt_delta_states_enc_and_dec(
    fcs_dbm_variant_type, const char *, const char *);

#endif
#ifdef __cplusplus
}
#endif
