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
 * delta_states_debondt.h - header file for the DeBondt delta states encoding.
 *
 */

#ifndef FC_SOLVE__DELTA_STATES_DEBONDT_H
#define FC_SOLVE__DELTA_STATES_DEBONDT_H

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
    fcs_state_t * _init_state, * _derived_state;
    int bits_per_orig_cards_in_column;
    int card_states[4 * RANK_KING];
    char bakers_dozen_topmost_cards_lookup[((1 << 6) / 8) + 1];
    fcs_var_base_reader_t r;
    fcs_var_base_writer_t w;
} fc_solve_debondt_delta_stater_t;

extern char * fc_solve_user_INTERNAL_debondt_delta_states_enc_and_dec(
        enum fcs_dbm_variant_type_t local_variant,
        const char * init_state_str_proto,
        const char * derived_state_str_proto
        );

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__DELTA_STATES_DEBONDT_H */
