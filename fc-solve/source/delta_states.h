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
 * move.h - header file for the move and move stacks functions of
 * Freecell Solver
 *
 */

#ifndef FC_SOLVE__DELTA_STATES_H
#define FC_SOLVE__DELTA_STATES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "state.h"

typedef struct { unsigned char s[24]; } fcs_encoded_state_buffer_t;

typedef struct
{
    fcs_encoded_state_buffer_t key;
    fcs_encoded_state_buffer_t parent_and_move;
} fcs_dbm_record_t;

typedef struct
{
#ifndef FCS_FREECELL_ONLY
    int sequences_are_built_by;
#endif
    int num_freecells;
    int num_columns;
    fcs_state_t * _init_state, * _derived_state;
    int bits_per_orig_cards_in_column;
} fc_solve_delta_stater_t;

static GCC_INLINE void fcs_init_encoded_state(fcs_encoded_state_buffer_t * enc_state)
{
    memset(enc_state, '\0', sizeof(*enc_state));
}

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__DELTA_STATES_H */
