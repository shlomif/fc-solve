// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish

// delta_states.h - header file for common functions to the delta_states
// functionality.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rinutils/bit_rw.h"
#include "dbm_common.h"

#ifdef FCS_DEBONDT_DELTA_STATES
#include "var_base_writer.h"
#include "var_base_reader.h"
#define FCS_ENCODED_STATE_COUNT_CHARS 16
#define CARD_ARRAY_LEN ((RANK_KING + 1) * FCS_NUM_SUITS)

typedef struct
{
    FCS_ON_NOT_FC_ONLY(int sequences_are_built_by;)
    int num_freecells;
    size_t num_columns;
    fcs_state *init_state, *derived_state;
    size_t bits_per_orig_cards_in_column;
    int card_states[CARD_ARRAY_LEN];
    int8_t bakers_dozen_topmost_cards_lookup[((1 << 6) / 8) + 1];
    fcs_var_base_reader r;
    fcs_var_base_writer w;
} fcs_delta_stater;

#else
#define FCS_ENCODED_STATE_COUNT_CHARS 24
typedef struct
{
    FCS_ON_NOT_FC_ONLY(int sequences_are_built_by;)
    size_t num_freecells;
    size_t num_columns;
    fcs_state *init_state, *derived_state;
    size_t bits_per_orig_cards_in_column;
} fcs_delta_stater;
#endif

static inline void fc_solve_delta_stater_set_derived(
    fcs_delta_stater *const self, fcs_state *const state)
{
    self->derived_state = state;
}

typedef struct
{
    uint8_t s[FCS_ENCODED_STATE_COUNT_CHARS];
} fcs_encoded_state_buffer;

#ifdef FCS_DBM_RECORD_POINTER_REPR
#error FCS_DBM_RECORD_POINTER_REPR is not supported
#else
typedef struct
{
    fcs_encoded_state_buffer key;
    fcs_encoded_state_buffer parent;
} fcs_dbm_record;

typedef fcs_dbm_record *fcs_dbm_store_val;
#endif

static inline void fcs_init_encoded_state(fcs_encoded_state_buffer *enc_state)
{
    memset(enc_state, '\0', sizeof(*enc_state));
}

#ifdef __cplusplus
}
#endif
