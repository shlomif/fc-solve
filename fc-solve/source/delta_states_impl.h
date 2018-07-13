/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
// delta_states_impl.h - "delta states" are an encoding of states, where the
// states are encoded and decoded based on a compact delta from the initial
// state.
#include "delta_states_iface.h"
#include "delta_states.h"

#ifndef FCS_DEBONDT_DELTA_STATES
static int fc_solve_get_column_orig_num_cards(
    fcs_delta_stater *const self GCC_UNUSED, const fcs_const_cards_column col)
{
    FCS_ON_NOT_FC_ONLY(const_SLOT(sequences_are_built_by, self));
    for (int num_cards = fcs_col_len(col); num_cards >= 2; --num_cards)
    {
        if (!fcs_is_parent_card(fcs_col_get_card(col, num_cards - 1),
                fcs_col_get_card(col, num_cards - 2)))
        {
            return num_cards;
        }
    }
    return 0;
}

static void fc_solve_delta_stater_init(fcs_delta_stater *const self,
    const fcs_dbm_variant_type local_variant GCC_UNUSED,
    fcs_state *const init_state, const size_t num_columns,
    const int num_freecells PASS_ON_NOT_FC_ONLY(
        const int sequences_are_built_by))
{
    FCS_ON_NOT_FC_ONLY(self->sequences_are_built_by = sequences_are_built_by);
    self->num_columns = num_columns;
    self->num_freecells = num_freecells;
    self->init_state = init_state;

    int max_num_cards = 0;
    for (size_t col_idx = 0; col_idx < num_columns; ++col_idx)
    {
        const_AUTO(num_cards, fc_solve_get_column_orig_num_cards(self,
                                  fcs_state_get_col(*init_state, col_idx)));

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
            ++num_bits;
            bitmask <<= 1;
        }

        self->bits_per_orig_cards_in_column = num_bits;
    }
}

static inline void fc_solve_delta_stater_release(
    fcs_delta_stater *const self GCC_UNUSED)
{
}

typedef struct
{
    enum
    {
        COL_TYPE_EMPTY,
        COL_TYPE_ENTIRELY_NON_ORIG,
        COL_TYPE_HAS_ORIG
    } type;
    fcs_uchar enc[4];
    fcs_uchar *end;
    int bit_in_char_idx;
} fcs_column_encoding_composite;

static inline void fc_solve_get_column_encoding_composite(
    fcs_delta_stater *const self, const int col_idx,
    fcs_column_encoding_composite *const ret)
{
    const fcs_state *const derived = self->derived_state;
    const_AUTO(col, fcs_state_get_col(*derived, col_idx));

    const_AUTO(num_orig_cards, fc_solve_get_column_orig_num_cards(self, col));
    const int col_len = fcs_col_len(col);
    const int num_derived_cards = col_len - num_orig_cards;

    int_fast16_t num_cards_in_seq = num_derived_cards;
    fcs_card init_card = fc_solve_empty_card;

    if ((num_orig_cards == 0) && num_derived_cards)
    {
        init_card = fcs_col_get_card(col, 0);
        --num_cards_in_seq;
    }

    /* Prepare the encoding. */
    fc_solve_bit_writer bit_w;
    fc_solve_bit_writer_init(&bit_w, ret->enc);

    fc_solve_bit_writer_write(
        &bit_w, self->bits_per_orig_cards_in_column, num_orig_cards);

    fc_solve_bit_writer_write(&bit_w, 4, num_derived_cards);

    if (fcs_card_is_valid(init_card))
    {
        fc_solve_bit_writer_write(&bit_w, 6, fcs_card2char(init_card));
    }

    for (int i = col_len - num_cards_in_seq; i < col_len; ++i)
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

#if MAX_NUM_FREECELLS > 0
static inline void fc_solve_get_freecells_encoding(
    fcs_delta_stater *const self, fc_solve_bit_writer *const bit_w)
{
    const fcs_state *const derived = self->derived_state;
    const_SLOT(num_freecells, self);

    fcs_card freecells[MAX_NUM_FREECELLS];
    for (size_t i = 0; i < num_freecells; ++i)
    {
        freecells[i] = fcs_freecell_card(*derived, i);
    }

    /* Sort the freecells using selection-sort. */
    for (size_t i = 0; i < num_freecells; ++i)
    {
        size_t min_idx = i;
        for (size_t j = i + 1; j < num_freecells; ++j)
        {
            if (fcs_card2char(freecells[j]) < fcs_card2char(freecells[min_idx]))
            {
                min_idx = j;
            }
        }
        const_AUTO(i_card, freecells[i]);
        fc_solve_bit_writer_write(bit_w, 6,
            fcs_card2char((min_idx != i) ? ({
                const_AUTO(min_card, freecells[min_idx]);
                freecells[min_idx] = i_card;
                min_card;
            })
                                         : i_card));
    }
}
#endif

static inline void fc_solve_delta__promote_empty_cols(const size_t num_columns,
    int *const cols_indexes, fcs_column_encoding_composite *const cols)
{
    size_t non_orig_idx = 0;
    ssize_t empty_col_idx = num_columns - 1;

    while (1)
    {
        for (; non_orig_idx < num_columns; ++non_orig_idx)
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

        for (; empty_col_idx >= 0; --empty_col_idx)
        {
            if (cols[empty_col_idx].type == COL_TYPE_EMPTY)
            {
                break;
            }
        }

        if ((empty_col_idx < 0) || (empty_col_idx < (ssize_t)non_orig_idx))
        {
            break;
        }

        const_AUTO(swap_int, cols_indexes[non_orig_idx]);
        cols_indexes[non_orig_idx] = cols_indexes[empty_col_idx];
        cols_indexes[empty_col_idx] = swap_int;

        ++non_orig_idx;
        --empty_col_idx;
    }
}

static void fc_solve_delta_stater_encode_composite(
    fcs_delta_stater *const self, fc_solve_bit_writer *const bit_w)
{
    int cols_indexes[MAX_NUM_STACKS];
    fcs_column_encoding_composite cols[MAX_NUM_STACKS];
    fcs_state *const derived = self->derived_state;

    const_SLOT(num_columns, self);
    for (size_t i = 0; i < num_columns; ++i)
    {
        cols_indexes[i] = i;
        fc_solve_get_column_encoding_composite(self, i, &(cols[i]));
    }

    /*
     * Move the empty columns to the front, but only within the
     * entirely_non_orig
     * That's because the orig columns should be preserved in their own
     * place.
     * */
    fc_solve_delta__promote_empty_cols(num_columns, cols_indexes, cols);

    int new_non_orig_cols_indexes[MAX_NUM_STACKS];
    int new_non_orig_cols_indexes_count = 0;

    /* Filter the new_non_orig_cols_indexes */
    for (size_t i = 0; i < num_columns; ++i)
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
    for (int b = 1; b < new_non_orig_cols_indexes_count; ++b)
    {
        for (int c = b; (c > 0) && (COMP_BY_IDX(c - 1) > COMP_BY_IDX(c)); --c)
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
    for (size_t i = 0, sorted_idx = 0; i < num_columns; ++i)
    {
        if (cols[cols_indexes[i]].type == COL_TYPE_ENTIRELY_NON_ORIG)
        {
            cols_indexes[i] = new_non_orig_cols_indexes[sorted_idx++];
        }
    }

#if MAX_NUM_FREECELLS > 0
    fc_solve_get_freecells_encoding(self, bit_w);
#endif
    for (size_t i = 0; i < num_columns; ++i)
    {
        const fcs_column_encoding_composite *const col_enc =
            (cols + cols_indexes[i]);
        const fcs_uchar *enc;
        const fcs_uchar *const end = col_enc->end;
        for (enc = col_enc->enc; enc < end; ++enc)
        {
            fc_solve_bit_writer_write(bit_w, NUM_BITS_IN_BYTES, (*enc));
        }
        fc_solve_bit_writer_write(bit_w, col_enc->bit_in_char_idx, (*enc));
    }
}

/* ret must be an empty state. */
static void fc_solve_delta_stater_decode(fcs_delta_stater *const self,
    fcs_bit_reader *const bit_r, fcs_state *const ret)
{
#define PROCESS_CARD(card)                                                     \
    if (fcs_card_rank(card) < foundations[fcs_card_suit(card)])                \
    {                                                                          \
        foundations[fcs_card_suit(card)] = fcs_card_rank(card);                \
    }

    const_SLOT(bits_per_orig_cards_in_column, self);

    int foundations[4] = {14, 14, 14, 14};
    /* Read the Freecells. */

#if MAX_NUM_FREECELLS > 0
    const_SLOT(num_freecells, self);
    for (size_t i = 0; i < num_freecells; ++i)
    {
        const fcs_card card = fcs_char2card(fc_solve_bit_reader_read(bit_r, 6));
        if (fcs_card_is_valid(card))
        {
            PROCESS_CARD(card);
        }
        fcs_put_card_in_freecell(*ret, i, card);
    }
#endif

    const_SLOT(num_columns, self);
    const fcs_state *const init_state = self->init_state;

    for (size_t col_idx = 0; col_idx < num_columns; ++col_idx)
    {
        const_AUTO(col, fcs_state_get_col(*ret, col_idx));
        const int num_orig_cards =
            fc_solve_bit_reader_read(bit_r, bits_per_orig_cards_in_column);
        const_AUTO(orig_col, fcs_state_get_col(*init_state, col_idx));

        for (int i = 0; i < num_orig_cards; ++i)
        {
            const fcs_card card = fcs_col_get_card(orig_col, i);
            PROCESS_CARD(card);
            fcs_col_push_card(col, card);
        }

        const int num_derived_cards = fc_solve_bit_reader_read(bit_r, 4);
        int_fast16_t num_cards_in_seq = num_derived_cards;

        if ((num_orig_cards == 0) && num_derived_cards)
        {
            const fcs_card card =
                fcs_char2card(fc_solve_bit_reader_read(bit_r, 6));
            PROCESS_CARD(card);
            fcs_col_push_card(col, card);

            --num_cards_in_seq;
        }

        if (num_cards_in_seq)
        {
            fcs_card last_card = fcs_col_get_card(col, fcs_col_len(col) - 1);

            for (int_fast16_t i = 0; i < num_cards_in_seq; ++i)
            {
                const int suit_bit = fc_solve_bit_reader_read(bit_r, 1);
                const fcs_card new_card =
                    fcs_make_card(fcs_card_rank(last_card) - 1,
                        ((suit_bit << 1) |
                            ((fcs_card_suit(last_card) & 0x1) ^ 0x1)));

                PROCESS_CARD(new_card);
                fcs_col_push_card(col, new_card);

                last_card = new_card;
            }
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        fcs_set_foundation(*ret, i, foundations[i] - 1);
    }

#undef PROCESS_CARD
}

static inline void fc_solve_delta_stater_decode_into_state_proto(
    const fcs_dbm_variant_type local_variant GCC_UNUSED,
    fcs_delta_stater *const delta_stater, const fcs_uchar *const enc_state,
    fcs_state_keyval_pair *const ret IND_BUF_T_PARAM(indirect_stacks_buffer))
{
    fcs_bit_reader bit_r;
    fc_solve_bit_reader_init(&bit_r, enc_state + 1);
    fc_solve_state_init(ret, STACKS_NUM, indirect_stacks_buffer);
    fc_solve_delta_stater_decode(delta_stater, &bit_r, &(ret->s));
}

static inline void fc_solve_delta_stater_encode_into_buffer(
    fcs_delta_stater *const delta_stater,
    const fcs_dbm_variant_type local_variant GCC_UNUSED,
    fcs_state_keyval_pair *const state, unsigned char *const out_enc_state)
{
    fc_solve_bit_writer bit_w;
    fc_solve_bit_writer_init(&bit_w, out_enc_state + 1);
    fc_solve_delta_stater_set_derived(delta_stater, &(state->s));
    fc_solve_delta_stater_encode_composite(delta_stater, &bit_w);
    out_enc_state[0] =
        bit_w.current - bit_w.start + (bit_w.bit_in_char_idx > 0);
}
#endif
