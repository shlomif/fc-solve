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
 * delta_states_debondt.h - header file for the DeBondt delta states encoding.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "state.h"

#include "delta_states.h"
#include "var_base_writer.h"
#include "var_base_reader.h"

#include "dbm_common.h"

#define RANK_KING 13

typedef struct
{
#ifndef FCS_FREECELL_ONLY
    int sequences_are_built_by;
#endif
    int num_freecells;
    int num_columns;
    fcs_state_t *init_state, *derived_state;
    int bits_per_orig_cards_in_column;
    int card_states[4 * RANK_KING];
    char bakers_dozen_topmost_cards_lookup[((1 << 6) / 8) + 1];
    fcs_var_base_reader_t r;
    fcs_var_base_writer_t w;
} fc_solve_debondt_delta_stater_t;

extern char *fc_solve_user_INTERNAL_debondt_delta_states_enc_and_dec(
    enum fcs_dbm_variant_type_t local_variant, const char *init_state_str_proto,
    const char *derived_state_str_proto);

#ifdef __cplusplus
}
#endif
