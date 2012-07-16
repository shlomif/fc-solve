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

#include "fcs_dllexport.h"
#include "bit_rw.h"
#include "state.h"
#include "indirect_buffer.h"

#include "delta_states_iface.h"
#include "delta_states.h"

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS
#include "dbm_common.h"
#endif

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
    int max_num_cards;

    self = malloc(sizeof(*self));

#ifndef FCS_FREECELL_ONLY
    self->sequences_are_built_by = sequences_are_built_by;
#endif

    self->num_columns = num_columns;
    self->num_freecells = num_freecells;

    self->_init_state = init_state;

    max_num_cards = 0;
    for (col_idx = 0 ; col_idx < num_columns; col_idx++)
    {
        int num_cards;

        num_cards = fc_solve_get_column_orig_num_cards(self, fcs_state_get_col(*init_state, col_idx));

        if (num_cards > max_num_cards)
        {
            max_num_cards = num_cards;
        }
    }

    {
        int bitmask, num_bits;

        bitmask = 1;
        num_bits = 0;

        while (bitmask <= max_num_cards)
        {
            num_bits++;
            bitmask <<= 1;
        }

        self->bits_per_orig_cards_in_column = num_bits;
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
            self->bits_per_orig_cards_in_column, num_orig_cards
            );

    fc_solve_bit_writer_write(&bit_w, 4, num_derived_cards);

    if (!(init_card == fc_solve_empty_card))
    {
        fc_solve_bit_writer_write(&bit_w, 6, init_card);
    }

    for (i=col_len-num_cards_in_seq ; i<col_len; i++)
    {
#define GET_SUIT_BIT(card) (( (fcs_card_suit(card)) & 0x2 ) >> 1 )

        fc_solve_bit_writer_write(&bit_w,
                1, GET_SUIT_BIT(fcs_col_get_card(col, i))
        );

#undef GET_SUIT_BIT
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
        int empty_idx = num_columns-1;

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

            for (; empty_idx >= 0 ; empty_idx--)
            {
                if (cols[empty_idx].type == COL_TYPE_EMPTY)
                {
                    break;
                }
            }

            if ((empty_idx < 0) || (empty_idx < non_orig_idx))
            {
                break;
            }

            swap_int = cols_indexes[non_orig_idx];
            cols_indexes[non_orig_idx] = cols_indexes[empty_idx];
            cols_indexes[empty_idx] = swap_int;

            non_orig_idx++;
            empty_idx--;
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

        /* Sort the new_non_orig_cols_indexes_count using insertion-sort. */
        {
            int c;

#define b i
#define COMP_BY(idx) (fcs_col_get_card(fcs_state_get_col((*derived), (idx)), 0))
#define ITEM_IDX(idx) (new_non_orig_cols_indexes[idx])
#define COMP_BY_IDX(idx) (COMP_BY(ITEM_IDX(idx)))

            for (b=1 ; b < new_non_orig_cols_indexes_count ; b++)
            {
                for (c = b; (c>0) && (COMP_BY_IDX(c-1) > COMP_BY_IDX(c)) ; c--)
                {
                    swap_int = ITEM_IDX(c);
                    ITEM_IDX(c) = ITEM_IDX(c-1);
                    ITEM_IDX(c-1) = swap_int;
                }
            }
#undef COMP_BY_IDX
#undef ITEM_IDX
#undef COMP_BY
#undef b
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
    fcs_state_t * _init_state;
    int bits_per_orig_cards_in_column;

    for (i = 0 ; i < 4; i++)
    {
        foundations[i] = 14;
    }

#define PROCESS_CARD(card) { if (fcs_card_rank(card) < foundations[fcs_card_suit(card)]) { foundations[fcs_card_suit(card)] = fcs_card_rank(card); } }

    num_freecells = self->num_freecells;
    bits_per_orig_cards_in_column = self->bits_per_orig_cards_in_column;

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
    _init_state = self->_init_state;

    for (col_idx = 0; col_idx < num_columns ; col_idx++)
    {
        fcs_cards_column_t col, orig_col;
        int num_orig_cards;
        int num_derived_cards;
        int num_cards_in_seq;

        col = fcs_state_get_col(*ret, col_idx);
        num_orig_cards = fc_solve_bit_reader_read(bit_r, bits_per_orig_cards_in_column);

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

            for ( i = 0 ; i < num_cards_in_seq ; i++)
            {
                fcs_card_t new_card;
                int suit_bit;

                new_card = fc_solve_empty_card;
                suit_bit = fc_solve_bit_reader_read(bit_r, 1);

                fcs_card_set_num(new_card, fcs_card_rank(last_card)-1);
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

#undef PROCESS_CARD
}

static GCC_INLINE void fc_solve_delta_stater_decode_into_state_proto(
        fc_solve_delta_stater_t * delta_stater,
        const fcs_uchar_t * const enc_state,
        fcs_state_keyval_pair_t * ret
        IND_BUF_T_PARAM(indirect_stacks_buffer)
        )
{
    fc_solve_bit_reader_t bit_r;
    fc_solve_bit_reader_init(&bit_r, enc_state+1);

    fc_solve_state_init(
        ret,
        STACKS_NUM,
        indirect_stacks_buffer
    );

    fc_solve_delta_stater_decode(
        delta_stater,
        &bit_r,
        &(ret->s)
    );
}

#ifdef INDIRECT_STACK_STATES
#define fc_solve_delta_stater_decode_into_state(delta_stater, enc_state, state_ptr, indirect_stacks_buffer) fc_solve_delta_stater_decode_into_state_proto(delta_stater, enc_state, state_ptr, indirect_stacks_buffer)
#else
#define fc_solve_delta_stater_decode_into_state(delta_stater, enc_state, state_ptr, indirect_stacks_buffer) fc_solve_delta_stater_decode_into_state_proto(delta_stater, enc_state, state_ptr)
#endif

static GCC_INLINE void fc_solve_delta_stater_encode_into_buffer(
    fc_solve_delta_stater_t * delta_stater,
    fcs_state_keyval_pair_t * state,
    unsigned char * out_enc_state
)
{
    fc_solve_bit_writer_t bit_w;
    fc_solve_bit_writer_init(&bit_w, out_enc_state+1);
    fc_solve_delta_stater_set_derived(delta_stater, &(state->s));
    fc_solve_delta_stater_encode_composite(delta_stater, &bit_w);
    out_enc_state[0] =
        bit_w.current - bit_w.start + (bit_w.bit_in_char_idx > 0)
        ;
}

static GCC_INLINE void fcs_init_and_encode_state(
    fc_solve_delta_stater_t * delta_stater,
    fcs_state_keyval_pair_t * state,
    fcs_encoded_state_buffer_t * enc_state
)
{
    fcs_init_encoded_state(enc_state);

    fc_solve_delta_stater_encode_into_buffer(
        delta_stater,
        state,
        enc_state->s
    );
}

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS

static char * prepare_state_str(const char * proto)
{
    char * ret;

    ret = strdup(proto);

    /* Process the string in-place to make it available as input
     * to fc-solve again.
     * */

    {
        char * s, * d;
        char c;
        s = d = ret;

        while ((c = *(d++) = *(s++)))
        {
            if ((c == '\n') && (s[0] == ':') && (s[1] = ' '))
            {
                s += 2;
            }
        }
    }

    return ret;
}

/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT char * fc_solve_user_INTERNAL_delta_states_enc_and_dec(
        const char * init_state_str_proto,
        const char * derived_state_str_proto
        )
{
    char * init_state_s, * derived_state_s;
    fcs_state_keyval_pair_t init_state, derived_state, new_derived_state;
    fc_solve_delta_stater_t * delta;
    fcs_uchar_t enc_state[24];
    fc_solve_bit_writer_t bit_w;
    fc_solve_bit_reader_t bit_r;
    char * new_derived_as_str;
    fcs_state_locs_struct_t locs;

    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    DECLARE_IND_BUF_T(derived_stacks_buffer)
    DECLARE_IND_BUF_T(new_derived_indirect_stacks_buffer)

    init_state_s = prepare_state_str(init_state_str_proto);
    derived_state_s = prepare_state_str(derived_state_str_proto);

    fc_solve_initial_user_state_to_c(
            init_state_s,
            &init_state,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            indirect_stacks_buffer
            );

    fc_solve_initial_user_state_to_c(
            derived_state_s,
            &derived_state,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            derived_stacks_buffer
            );

    delta = fc_solve_delta_stater_alloc(
            &(init_state.s),
            STACKS_NUM,
            FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
            , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
            );

    fc_solve_delta_stater_set_derived(delta, &(derived_state.s));

    fc_solve_state_init(
        &new_derived_state,
        STACKS_NUM,
        new_derived_indirect_stacks_buffer
    );

    fc_solve_bit_writer_init(&bit_w, enc_state);
    fc_solve_delta_stater_encode_composite(delta, &bit_w);

    fc_solve_bit_reader_init(&bit_r, enc_state);
    fc_solve_delta_stater_decode(delta, &bit_r, &(new_derived_state.s));

    fc_solve_init_locs(&locs);

    new_derived_as_str =
        fc_solve_state_as_string(
            &(new_derived_state.s),
            &(new_derived_state.info),
            &locs,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            1,
            0,
            1
            );

    free(init_state_s);
    free(derived_state_s);

    fc_solve_delta_stater_free (delta);

    return new_derived_as_str;
}

#endif
