/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
/*
 * delta_states_impl.h - "delta states" are an encoding of states, where the
 * states are encoded and decoded based on a compact delta from the initial
 * state.
 */
#include "dll_thunk.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "state.h"
#include "bit_rw.h"
#include "delta_states_iface.h"
#include "delta_states.h"

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS
#include "dbm_common.h"
#endif

static int fc_solve_get_column_orig_num_cards(
    fc_solve_delta_stater_t *const self, const fcs_const_cards_column_t col)
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
        fcs_card_t child_card = fcs_col_get_card(col, num_cards - 1);
        fcs_card_t parent_card = fcs_col_get_card(col, num_cards - 2);

        if (!fcs_is_parent_card(child_card, parent_card))
        {
            break;
        }
    }

    return ((num_cards >= 2) ? num_cards : 0);
}

static void fc_solve_delta_stater_init(
    fc_solve_delta_stater_t *const self, fcs_state_t *const init_state,
    const int num_columns, const int num_freecells
#ifndef FCS_FREECELL_ONLY
    ,
    const int sequences_are_built_by
#endif
    )
{
    int col_idx;
    int max_num_cards;

#ifndef FCS_FREECELL_ONLY
    self->sequences_are_built_by = sequences_are_built_by;
#endif

    self->num_columns = num_columns;
    self->num_freecells = num_freecells;

    self->_init_state = init_state;

    max_num_cards = 0;
    for (col_idx = 0; col_idx < num_columns; col_idx++)
    {
        int num_cards;

        num_cards = fc_solve_get_column_orig_num_cards(
            self, fcs_state_get_col(*init_state, col_idx));

        if (num_cards > max_num_cards)
        {
            max_num_cards = num_cards;
        }
    }

    {
        int bitmask = 1;
        int num_bits = 0;

        while (bitmask <= max_num_cards)
        {
            num_bits++;
            bitmask <<= 1;
        }

        self->bits_per_orig_cards_in_column = num_bits;
    }
}

static GCC_INLINE void fc_solve_delta_stater_release(
    fc_solve_delta_stater_t *const self GCC_UNUSED)
{
}

static GCC_INLINE void fc_solve_delta_stater_set_derived(
    fc_solve_delta_stater_t *const self, fcs_state_t *const state)
{
    self->_derived_state = state;
}

typedef struct
{
    enum
    {
        COL_TYPE_EMPTY,
        COL_TYPE_ENTIRELY_NON_ORIG,
        COL_TYPE_HAS_ORIG
    } type;
    fcs_uchar_t enc[4];
    fcs_uchar_t *end;
    int bit_in_char_idx;
} fc_solve_column_encoding_composite_t;

static GCC_INLINE void fc_solve_get_column_encoding_composite(
    fc_solve_delta_stater_t *const self, const int col_idx,
    fc_solve_column_encoding_composite_t *const ret)
{
    const fcs_state_t *const derived = self->_derived_state;
    fcs_const_cards_column_t col = fcs_state_get_col(*derived, col_idx);

    const int num_orig_cards = fc_solve_get_column_orig_num_cards(self, col);
    const int col_len = fcs_col_len(col);
    const int num_derived_cards = col_len - num_orig_cards;

    int num_cards_in_seq = num_derived_cards;
    fcs_card_t init_card = fc_solve_empty_card;

    if ((num_orig_cards == 0) && num_derived_cards)
    {
        init_card = fcs_col_get_card(col, 0);
        num_cards_in_seq--;
    }

    /* Prepare the encoding. */
    fc_solve_bit_writer_t bit_w;
    fc_solve_bit_writer_init(&bit_w, ret->enc);

    fc_solve_bit_writer_write(
        &bit_w, self->bits_per_orig_cards_in_column, num_orig_cards);

    fc_solve_bit_writer_write(&bit_w, 4, num_derived_cards);

    if (fcs_card_is_valid(init_card))
    {
        fc_solve_bit_writer_write(&bit_w, 6, fcs_card2char(init_card));
    }

    for (int i = col_len - num_cards_in_seq; i < col_len; i++)
    {
#define GET_SUIT_BIT(card) (((fcs_card_suit(card)) & 0x2) >> 1)

        fc_solve_bit_writer_write(
            &bit_w, 1, GET_SUIT_BIT(fcs_col_get_card(col, i)));

#undef GET_SUIT_BIT
    }

    ret->end = bit_w.current;
    ret->bit_in_char_idx = bit_w.bit_in_char_idx;

    /* Calculate the type. */
    ret->type = ((col_len == 0) ? COL_TYPE_EMPTY
                                : num_orig_cards ? COL_TYPE_HAS_ORIG
                                                 : COL_TYPE_ENTIRELY_NON_ORIG);
}

static GCC_INLINE void fc_solve_get_freecells_encoding(
    fc_solve_delta_stater_t *const self, fc_solve_bit_writer_t *const bit_w)
{

    const fcs_state_t *const derived = self->_derived_state;
    const_SLOT(num_freecells, self);

    fcs_card_t freecells[MAX_NUM_FREECELLS];
    for (int i = 0; i < num_freecells; i++)
    {
        freecells[i] = fcs_freecell_card(*derived, i);
    }

    /* Sort the freecells using selection-sort. */
    for (int i = 0; i < num_freecells; i++)
    {
        int min_idx = i;
        for (int j = i + 1; j < num_freecells; j++)
        {
            if (fcs_card2char(freecells[j]) < fcs_card2char(freecells[min_idx]))
            {
                min_idx = j;
            }
        }
        const_AUTO(i_card, freecells[i]);
        fc_solve_bit_writer_write(
            bit_w, 6, fcs_card2char((min_idx != i) ? ({
                const_AUTO(min_card, freecells[min_idx]);
                freecells[min_idx] = i_card;
                min_card;
            })
                                                   : i_card));
    }
}

static void fc_solve_delta_stater_encode_composite(
    fc_solve_delta_stater_t *const self, fc_solve_bit_writer_t *const bit_w)
{
    int cols_indexes[MAX_NUM_STACKS];
    fc_solve_column_encoding_composite_t cols[MAX_NUM_STACKS];
    fcs_state_t *const derived = self->_derived_state;

    const_SLOT(num_columns, self);
    for (int i = 0; i < num_columns; i++)
    {
        cols_indexes[i] = i;
        fc_solve_get_column_encoding_composite(self, i, &(cols[i]));
    }

    {
        int non_orig_idx = 0;
        int empty_idx = num_columns - 1;

        /*
         * Move the empty columns to the front, but only within the
         * entirely_non_orig
         * That's because the orig columns should be preserved in their own
         * place.
         * */
        while (1)
        {
            for (; non_orig_idx < num_columns; non_orig_idx++)
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

            for (; empty_idx >= 0; empty_idx--)
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

            const_AUTO(swap_int, cols_indexes[non_orig_idx]);
            cols_indexes[non_orig_idx] = cols_indexes[empty_idx];
            cols_indexes[empty_idx] = swap_int;

            non_orig_idx++;
            empty_idx--;
        }
    }

    {
        int new_non_orig_cols_indexes[MAX_NUM_STACKS];
        int new_non_orig_cols_indexes_count = 0;

        /* Filter the new_non_orig_cols_indexes */
        for (int i = 0; i < num_columns; i++)
        {
            if (cols[cols_indexes[i]].type == COL_TYPE_ENTIRELY_NON_ORIG)
            {
                new_non_orig_cols_indexes[new_non_orig_cols_indexes_count++] =
                    cols_indexes[i];
            }
        }

/* Sort the new_non_orig_cols_indexes_count using insertion-sort. */
#define COMP_BY(idx)                                                           \
    (fcs_card2char(fcs_col_get_card(fcs_state_get_col((*derived), (idx)), 0)))
#define ITEM_IDX(idx) (new_non_orig_cols_indexes[idx])
#define COMP_BY_IDX(idx) (COMP_BY(ITEM_IDX(idx)))
        for (int b = 1; b < new_non_orig_cols_indexes_count; b++)
        {
            for (int c = b; (c > 0) && (COMP_BY_IDX(c - 1) > COMP_BY_IDX(c));
                 c--)
            {
                const_AUTO(swap_int, ITEM_IDX(c));
                ITEM_IDX(c) = ITEM_IDX(c - 1);
                ITEM_IDX(c - 1) = swap_int;
            }
        }
#undef COMP_BY_IDX
#undef ITEM_IDX
#undef COMP_BY
#undef b
        for (int i = 0, sorted_idx = 0; i < num_columns; i++)
        {
            if (cols[cols_indexes[i]].type == COL_TYPE_ENTIRELY_NON_ORIG)
            {
                cols_indexes[i] = new_non_orig_cols_indexes[sorted_idx++];
            }
        }
    }

    fc_solve_get_freecells_encoding(self, bit_w);
    for (int i = 0; i < num_columns; i++)
    {
        const fc_solve_column_encoding_composite_t *const col_enc =
            (cols + cols_indexes[i]);
        const fcs_uchar_t *enc;
        const fcs_uchar_t *const end = col_enc->end;
        for (enc = col_enc->enc; enc < end; enc++)
        {
            fc_solve_bit_writer_write(bit_w, NUM_BITS_IN_BYTES, (*enc));
        }
        fc_solve_bit_writer_write(bit_w, col_enc->bit_in_char_idx, (*enc));
    }
}

/* ret must be an empty state. */
static void fc_solve_delta_stater_decode(fc_solve_delta_stater_t *const self,
    fc_solve_bit_reader_t *const bit_r, fcs_state_t *const ret)
{
#define PROCESS_CARD(card)                                                     \
    {                                                                          \
        if (fcs_card_rank(card) < foundations[fcs_card_suit(card)])            \
        {                                                                      \
            foundations[fcs_card_suit(card)] = fcs_card_rank(card);            \
        }                                                                      \
    }

    const_SLOT(num_freecells, self);
    const_SLOT(bits_per_orig_cards_in_column, self);

    int foundations[4] = {14, 14, 14, 14};
    /* Read the Freecells. */

    for (int i = 0; i < num_freecells; i++)
    {
        const fcs_card_t card =
            fcs_char2card(fc_solve_bit_reader_read(bit_r, 6));
#ifdef DEBUG_STATES
        if (fc_solve_card_compare(card, fc_solve_empty_card))
#else
        if (!(card == fc_solve_empty_card))
#endif
        {
            PROCESS_CARD(card);
        }
        fcs_put_card_in_freecell(*ret, i, card);
    }

    const_SLOT(num_columns, self);
    const fcs_state_t *const _init_state = self->_init_state;

    for (int col_idx = 0; col_idx < num_columns; col_idx++)
    {
        const fcs_cards_column_t col = fcs_state_get_col(*ret, col_idx);
        const int num_orig_cards =
            fc_solve_bit_reader_read(bit_r, bits_per_orig_cards_in_column);

        fcs_const_cards_column_t orig_col =
            fcs_state_get_col(*_init_state, col_idx);

        for (int i = 0; i < num_orig_cards; i++)
        {
            const fcs_card_t card = fcs_col_get_card(orig_col, i);
            PROCESS_CARD(card);
            fcs_col_push_card(col, card);
        }

        const int num_derived_cards = fc_solve_bit_reader_read(bit_r, 4);
        int num_cards_in_seq = num_derived_cards;

        if ((num_orig_cards == 0) && num_derived_cards)
        {
            const fcs_card_t card =
                fcs_char2card(fc_solve_bit_reader_read(bit_r, 6));
            PROCESS_CARD(card);
            fcs_col_push_card(col, card);

            num_cards_in_seq--;
        }

        if (num_cards_in_seq)
        {
            fcs_card_t last_card = fcs_col_get_card(col, fcs_col_len(col) - 1);

            for (int i = 0; i < num_cards_in_seq; i++)
            {
                const int suit_bit = fc_solve_bit_reader_read(bit_r, 1);
                const fcs_card_t new_card =
                    fcs_make_card(fcs_card_rank(last_card) - 1,
                        ((suit_bit
                             << 1) | ((fcs_card_suit(last_card) & 0x1) ^ 0x1)));

                PROCESS_CARD(new_card);
                fcs_col_push_card(col, new_card);

                last_card = new_card;
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        fcs_set_foundation(*ret, i, foundations[i] - 1);
    }

#undef PROCESS_CARD
}

static GCC_INLINE void fc_solve_delta_stater_decode_into_state_proto(
    const enum fcs_dbm_variant_type_t local_variant,
    fc_solve_delta_stater_t *const delta_stater,
    const fcs_uchar_t *const enc_state,
    fcs_state_keyval_pair_t *const ret IND_BUF_T_PARAM(indirect_stacks_buffer))
{
    fc_solve_bit_reader_t bit_r;
    fc_solve_bit_reader_init(&bit_r, enc_state + 1);

    fc_solve_state_init(ret, STACKS_NUM, indirect_stacks_buffer);

    fc_solve_delta_stater_decode(delta_stater, &bit_r, &(ret->s));
}

#ifdef INDIRECT_STACK_STATES
#define fc_solve_delta_stater_decode_into_state(                               \
    delta_stater, enc_state, state_ptr, indirect_stacks_buffer)                \
    fc_solve_delta_stater_decode_into_state_proto(local_variant, delta_stater, \
        enc_state, state_ptr, indirect_stacks_buffer)
#else
#define fc_solve_delta_stater_decode_into_state(                               \
    delta_stater, enc_state, state_ptr, indirect_stacks_buffer)                \
    fc_solve_delta_stater_decode_into_state_proto(                             \
        local_variant, delta_stater, enc_state, state_ptr)
#endif

static GCC_INLINE void fc_solve_delta_stater_encode_into_buffer(
    fc_solve_delta_stater_t *const delta_stater,
    const enum fcs_dbm_variant_type_t local_variant GCC_UNUSED,
    fcs_state_keyval_pair_t *const state, unsigned char *const out_enc_state)
{
    fc_solve_bit_writer_t bit_w;
    fc_solve_bit_writer_init(&bit_w, out_enc_state + 1);
    fc_solve_delta_stater_set_derived(delta_stater, &(state->s));
    fc_solve_delta_stater_encode_composite(delta_stater, &bit_w);
    out_enc_state[0] =
        bit_w.current - bit_w.start + (bit_w.bit_in_char_idx > 0);
}

static GCC_INLINE void fcs_init_and_encode_state(
    fc_solve_delta_stater_t *delta_stater,
    const enum fcs_dbm_variant_type_t local_variant,
    fcs_state_keyval_pair_t *state, fcs_encoded_state_buffer_t *enc_state)
{
    fcs_init_encoded_state(enc_state);

    fc_solve_delta_stater_encode_into_buffer(
        delta_stater, local_variant, state, enc_state->s);
}
