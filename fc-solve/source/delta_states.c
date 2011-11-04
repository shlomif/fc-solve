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
    fcs_state_t * _init_state, * _derived_state;
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

static void fc_solve_delta_stater_free(fc_solve_delta_stater_t * self)
{
    free(self);
}

static GCC_INLINE void fc_solve_delta_stater_set_derived(fc_solve_delta_stater_t * self, fcs_state_t * state)
{
    self->_derived_state = state;
}

#define GET_SUIT_BIT(card) (( (fcs_card_suit(card)) & 0x2 ) >> 1 )

enum COL_TYPE
{
    COL_TYPE_EMPTY,
    COL_TYPE_ENTIRELY_NON_ORIG,
    COL_TYPE_HAS_ORIG
};

typedef struct
{
    int type;
    fcs_uchar_t enc[4];
    fcs_uchar_t * end;
    int bit_in_char_idx;
} fc_solve_column_encoding_composite_t;

static void fc_solve_get_column_encoding_composite(
        fc_solve_delta_stater_t * self,
        int col_idx,
        fc_solve_column_encoding_composite_t * ret
        )
{
    fcs_state_t * derived;
    fcs_cards_column_t col;
    int num_orig_cards;
    int col_len;
    int num_derived_cards;
    int num_cards_in_seq;
    fcs_card_t init_card;
    fc_solve_bit_writer_t bit_w;
    int i;
    
    derived = self->_derived_state;
    col = fcs_state_get_col(*derived, col_idx);

    num_orig_cards = fc_solve_get_column_orig_num_cards(self, col);
    col_len = fcs_col_len(col);
    num_derived_cards = col_len - num_orig_cards;
    
    num_cards_in_seq = num_derived_cards;
    init_card = fc_solve_empty_card;

    if ((num_orig_cards == 0) && num_derived_cards)
    {
        init_card = fcs_col_get_card(col, 0);
        num_cards_in_seq--;
    }

    /* Prepare the encoding. */
    fc_solve_bit_writer_init(&bit_w, ret->enc);

    fc_solve_bit_writer_write(&bit_w, 
            self->_columns_initial_lens[col_idx], num_orig_cards
            );

    fc_solve_bit_writer_write(&bit_w, 4, num_derived_cards);

    if (!(init_card == fc_solve_empty_card))
    {
        fc_solve_bit_writer_write(&bit_w, 6, init_card);
    }

    for (i=col_len-num_cards_in_seq ; i<col_len; i++)
    {
        fc_solve_bit_writer_write(&bit_w,
                1, GET_SUIT_BIT(fcs_col_get_card(col, i))
        );
    }
    
    ret->end = bit_w.current;
    ret->bit_in_char_idx = bit_w.bit_in_char_idx;

    /* Calculate the type. */
    ret->type =
    (
          (col_len == 0) ? COL_TYPE_EMPTY
        : num_orig_cards ? COL_TYPE_HAS_ORIG
        :                  COL_TYPE_ENTIRELY_NON_ORIG
    );
}

static void fc_solve_get_freecells_encoding(
        fc_solve_delta_stater_t * self,
        fc_solve_bit_writer_t * bit_w
        )
{
    fcs_card_t freecells[MAX_NUM_FREECELLS];
    int i, j, min_idx;
    fcs_state_t * derived;
    int num_freecells;
    fcs_card_t min_card;

    derived = self->_derived_state;
    num_freecells = self->num_freecells;

    for (i=0 ; i < num_freecells ; i++)
    {
        freecells[i] = fcs_freecell_card(*derived, i);
    }

    /* Sort the freecells using selection-sort. */
    for (i=0 ; i < num_freecells ; i++)
    {
        min_idx = i;
        for (j=i+1 ; j < num_freecells ; j++)
        {
            if (freecells[j] < freecells[min_idx])
            {
                min_idx = j;
            }
        }
        if (min_idx != i)
        {
            min_card = freecells[min_idx];
            freecells[min_idx] = freecells[i];
        }
        else
        {
            min_card = freecells[i];
        }
        fc_solve_bit_writer_write(bit_w, 6, min_card);
    }
}

static void fc_solve_delta_stater_encode_composite(
        fc_solve_delta_stater_t * self,
        fc_solve_bit_writer_t * bit_w
        )
{
    int cols_indexes[MAX_NUM_STACKS];
    fc_solve_column_encoding_composite_t cols[MAX_NUM_STACKS];
    fcs_state_t * derived;
    int i, swap_int;
    int num_columns;

    derived = self->_derived_state;

    num_columns = self->num_columns;
    for ( i=0 ; i < num_columns ; i++)
    {
        cols_indexes[i] = i;
        fc_solve_get_column_encoding_composite(self, i, &(cols[i]));
    }

    {
        int non_orig_idx = 0;

        /* 
         * Move the empty columns to the front, but only within the
         * entirely_non_orig
         * That's because the orig columns should be preserved in their own
         * place.
         * */
        while (1)
        {
            for ( ; non_orig_idx < num_columns ; non_orig_idx++)
            {
                if (cols[non_orig_idx].type == COL_TYPE_ENTIRELY_NON_ORIG)
                {
                    break;
                }
            }

            if (non_orig_idx == num_columns)
            {
                break;
            }

            {
                int empty_idx;
                
                for (empty_idx = non_orig_idx+1; empty_idx < num_columns ; empty_idx++)
                {
                    if (cols[empty_idx].type == COL_TYPE_EMPTY)
                    {
                        break;
                    }
                }

                if (empty_idx == num_columns)
                {
                    break;
                }

                swap_int = cols_indexes[non_orig_idx];
                cols_indexes[non_orig_idx] = cols_indexes[empty_idx];
                cols_indexes[empty_idx] = swap_int;

                non_orig_idx++;
            }
        }
    }

    {
        int new_non_orig_cols_indexes[MAX_NUM_STACKS];
        int new_non_orig_cols_indexes_count;

        /* Filter the new_non_orig_cols_indexes */
        for (new_non_orig_cols_indexes_count = 0, i=0; i < num_columns ; i++)
        {
            if (cols[cols_indexes[i]].type == COL_TYPE_ENTIRELY_NON_ORIG)
            {
                new_non_orig_cols_indexes[new_non_orig_cols_indexes_count++] =
                    cols_indexes[i];
            }
        }

        /* Sort the new_non_orig_cols_indexes_count using selection-sort. */
        {
            int j, min_idx;

            for (i=0 ; i < new_non_orig_cols_indexes_count-1 ; i++)
            {
                min_idx = i;
                for (j=i+1; j < new_non_orig_cols_indexes_count; j++)
                {
#define COMP_BY(idx) (fcs_col_get_card(fcs_state_get_col((*derived), (idx)), 0))
#define COMP_BY_IDX(idx) (COMP_BY(new_non_orig_cols_indexes[idx]))
                    if (COMP_BY_IDX(j) < COMP_BY_IDX(min_idx))
                    {
                        min_idx = j;
                    }
                }

                swap_int = new_non_orig_cols_indexes[min_idx];
                new_non_orig_cols_indexes[min_idx] = new_non_orig_cols_indexes[i];
                new_non_orig_cols_indexes[i] = swap_int;
            }
        }

        {
            int sorted_idx;
            for ( i=0 , sorted_idx = 0; i < num_columns ; i++)
            {
                if (cols[cols_indexes[i]].type == COL_TYPE_ENTIRELY_NON_ORIG)
                {
                    cols_indexes[i] = new_non_orig_cols_indexes[sorted_idx++];
                }
            }
        }
    }

    fc_solve_get_freecells_encoding(self, bit_w);
    for ( i=0 ; i < num_columns ; i++)
    {
        fc_solve_column_encoding_composite_t * col_enc;
        fcs_uchar_t * enc;

        col_enc = (cols + cols_indexes[i]);
        for (enc = col_enc->enc ; enc < col_enc->end ; enc++)
        {
            fc_solve_bit_writer_write(bit_w, NUM_BITS_IN_BYTES, (*enc));
        }
        fc_solve_bit_writer_write(bit_w, col_enc->bit_in_char_idx, (*enc));
    }
}

/* ret must be an empty state. */
static void fc_solve_delta_stater_decode(
        fc_solve_delta_stater_t * self,
        fc_solve_bit_reader_t * bit_r,
        fcs_state_t * ret
        )
{
    int foundations[4];
    int i, col_idx;
    int num_freecells;
    int num_columns;
    int * _columns_initial_lens;
    fcs_state_t * _init_state;

    for (i = 0 ; i < 4; i++)
    {
        foundations[i] = 14;
    }

#define PROCESS_CARD(card) { if (fcs_card_card_num(card) < foundations[fcs_card_suit(card)]) { foundations[fcs_card_suit(card)] = fcs_card_card_num(card); } }

    num_freecells = self->num_freecells;
    /* Read the Freecells. */
    
    for ( i=0 ; i < num_freecells ; i++)
    {
        fcs_card_t card;

        card = (fcs_card_t)fc_solve_bit_reader_read(bit_r, 6);
        if (!(card == fc_solve_empty_card))
        {
            PROCESS_CARD(card);
        }
        fcs_put_card_in_freecell(*ret, i, card);
    }

    num_columns = self->num_columns;
    _columns_initial_lens = self->_columns_initial_lens;
    _init_state = self->_init_state;

    for (col_idx = 0; col_idx < num_columns ; num_columns++)
    {
        fcs_cards_column_t col, orig_col;
        int num_orig_cards;
        int num_derived_cards;
        int num_cards_in_seq;

        col = fcs_state_get_col(*ret, col_idx);
        num_orig_cards = fc_solve_bit_reader_read(bit_r, _columns_initial_lens[col_idx]);

        orig_col = fcs_state_get_col(*_init_state, col_idx);

        for (i = 0 ; i < num_orig_cards ; i++)
        {
            fcs_card_t card;

            card = fcs_col_get_card(orig_col, i);
            PROCESS_CARD(card);
            fcs_col_push_card(col, card);
        }

        num_cards_in_seq = num_derived_cards =
            fc_solve_bit_reader_read(bit_r, 4);

        if ((num_orig_cards == 0) && num_derived_cards)
        {
            fcs_card_t card;

            card = (fcs_card_t)fc_solve_bit_reader_read(bit_r, 6);
            PROCESS_CARD(card);
            fcs_col_push_card(col, card);
            
            num_cards_in_seq--;
        }

        if (num_cards_in_seq)
        {
            fcs_card_t last_card;

            last_card = fcs_col_get_card(col, fcs_col_len(col)-1);

            for ( i=0 ; i < num_cards_in_seq -1 ; i++)
            {
                fcs_card_t new_card;
                int suit_bit;

                new_card = fc_solve_empty_card;
                suit_bit = fc_solve_bit_reader_read(bit_r, 1);

                fcs_card_set_num(new_card, fcs_card_card_num(last_card)-1);
                fcs_card_set_suit(new_card, 
                    ((suit_bit << 1) | 
                        ((fcs_card_suit(last_card) & 0x1) ^ 0x1)
                    )
                );

                PROCESS_CARD(new_card);
                fcs_col_push_card(col, new_card);

                last_card = new_card;
            }
        }
    }

    for (i = 0 ; i < 4 ; i++)
    {
        fcs_set_foundation(*ret, i, foundations[i]-1);
    }
}
