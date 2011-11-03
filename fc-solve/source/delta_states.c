/* Copyright (c) 2011 Shlomi Fish
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
 * delta_states.c - "delta states" are an encoding of states, where the
 * states are encoded and decoded based on a compact delta from the original
 * state.
 *
 */

#define BUILDING_DLL 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bit_rw.h"
#include "state.h"

typedef struct
{
#ifndef FCS_FREECELL_ONLY
    int sequences_are_built_by;
#endif
    int num_freecells;
    int num_columns;
    fcs_state_t * _init_state;
    int _columns_initial_lens[MAX_NUM_STACKS];
} fc_solve_delta_stater_t;

static int fc_solve_get_column_orig_num_cards(
        fc_solve_delta_stater_t * self, 
        fcs_cards_column_t col    
        )
{
    int num_cards;
#ifndef FCS_FREECELL_ONLY
    int sequences_are_built_by;
#endif


#ifndef FCS_FREECELL_ONLY
    sequences_are_built_by = self->sequences_are_built_by;
#endif
    for (num_cards = fcs_col_len(col); num_cards >= 2; num_cards--)
    {
        fcs_card_t child_card = fcs_col_get_card(col, num_cards-1);
        fcs_card_t parent_card = fcs_col_get_card(col, num_cards-2);

        if (!fcs_is_parent_card(child_card, parent_card))
        {
            break;
        }
    }

    return ((num_cards >= 2) ? num_cards : 0);
}

static fc_solve_delta_stater_t * fc_solve_delta_stater_alloc(
        fcs_state_t * init_state,
        int num_columns,
        int num_freecells
#ifndef FCS_FREECELL_ONLY
        , int sequences_are_built_by
#endif
        )
{
    fc_solve_delta_stater_t * self;
    int col_idx;
    int * initial_len_ptr;

    self = malloc(sizeof(*self));

#ifndef FCS_FREECELL_ONLY
    self->sequences_are_built_by = sequences_are_built_by;
#endif

    self->num_columns = num_columns;
    self->num_freecells = num_freecells;

    self->_init_state = init_state;

    initial_len_ptr = self->_columns_initial_lens;
    for (col_idx = 0 ; col_idx < num_columns; col_idx++)
    {
        int num_cards, bitmask, num_bits;

        num_cards = fc_solve_get_column_orig_num_cards(self, fcs_state_get_col(*init_state, col_idx));

        bitmask = 1;
        num_bits = 0;

        while (bitmask <= num_cards)
        {
            num_bits++;
            bitmask <<= 1;
        }

        *(initial_len_ptr++) = num_bits;
    }

    return self;
}
