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

#ifdef FCS_DEBONDT_DELTA_STATES
#define FCS_ENCODED_STATE_COUNT_CHARS 16
#else
#define FCS_ENCODED_STATE_COUNT_CHARS 24
#endif

typedef struct { unsigned char s[FCS_ENCODED_STATE_COUNT_CHARS]; } fcs_encoded_state_buffer_t;

#define FCS_PARENT_AND_MOVE__GET_MOVE(parent_and_move) \
    ((parent_and_move).s[ \
        (parent_and_move).s[0] + 1 \
        ] \
    )

#ifdef FCS_DBM_RECORD_POINTER_REPR

struct fcs_dbm_record_struct
{
    fcs_encoded_state_buffer_t key;
    struct fcs_dbm_record_struct * parent_ptr;
};

typedef struct fcs_dbm_record_struct fcs_dbm_record_t;

#else

typedef struct
{
    fcs_encoded_state_buffer_t key;
    fcs_encoded_state_buffer_t parent;
} fcs_dbm_record_t;

#endif

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

extern char * fc_solve_user_INTERNAL_delta_states_enc_and_dec(
        const char * init_state_str_proto,
        const char * derived_state_str_proto
        );

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__DELTA_STATES_H */
