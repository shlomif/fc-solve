/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
// delta_states_debondt_impl.h - "delta states" are an encoding of states,
// where the states are encoded and decoded based on a compact delta from the
// initial state.
//
// This encoding improves upon the original delta_states.c .
#include "indirect_buffer.h"
#include "delta_states_iface.h"
#include "delta_states.h"

#ifdef FCS_DEBONDT_DELTA_STATES
#define FOUNDATION_BASE (RANK_KING + 1)

enum
{
    OPT_TOPMOST = 0,
    OPT_DONT_CARE = OPT_TOPMOST,
    OPT_FREECELL = 1,
    OPT_ORIG_POS = 2,
    NUM_KING_OPTS = 3,
    OPT_PARENT_SUIT_MOD_IS_0 = 3,
    OPT_PARENT_SUIT_MOD_IS_1 = 4,
    NUM_OPTS = 5,
    OPT_IN_FOUNDATION = 5,
    OPT__BAKERS_DOZEN__ORIG_POS = 0,
    OPT__BAKERS_DOZEN__FIRST_PARENT = 1,
    NUM__BAKERS_DOZEN__OPTS = OPT__BAKERS_DOZEN__FIRST_PARENT + 4,
    OPT__BAKERS_DOZEN__IN_FOUNDATION = NUM__BAKERS_DOZEN__OPTS,
};

#define IS_BAKERS_DOZEN() (local_variant == FCS_DBM_VARIANT_BAKERS_DOZEN)

static inline void fc_solve_delta_stater_init(fcs_delta_stater *const self,
    const fcs_dbm_variant_type local_variant, fcs_state *const init_state,
    const size_t num_columns,
    const int num_freecells PASS_ON_NOT_FC_ONLY(
        const int sequences_are_built_by))
{
    FCS_ON_NOT_FC_ONLY(self->sequences_are_built_by = sequences_are_built_by);
    self->num_columns = num_columns;
    self->num_freecells = num_freecells;
    self->init_state = init_state;

    memset(self->bakers_dozen_topmost_cards_lookup, '\0',
        sizeof(self->bakers_dozen_topmost_cards_lookup));

    fc_solve_var_base_writer_init(&self->w);
    fc_solve_var_base_reader_init(&self->r);

    if (IS_BAKERS_DOZEN())
    {
        for (size_t col_idx = 0; col_idx < self->num_columns; ++col_idx)
        {
            const_AUTO(col, fcs_state_get_col(*init_state, col_idx));
            const int col_len = fcs_col_len(col);

            if (col_len > 0)
            {
                const_AUTO(top_card, fcs_col_get_card(col, 0));
                self->bakers_dozen_topmost_cards_lookup[top_card >> 3] |=
                    (1 << (top_card & (8 - 1)));
            }
        }
    }
}

static inline void fc_solve_delta_stater__init_card_states(
    fcs_delta_stater *const self)
{
    int *const card_states = self->card_states;
    for (size_t i = 0; i < COUNT(self->card_states); ++i)
    {
        card_states[i] = -1;
    }
}

#ifdef FCS_USE_INT128_FOR_VAR_BASE
#define fc_solve_delta_stater_release(s)
#else
static inline void fc_solve_delta_stater_release(fcs_delta_stater *const self)
{
    fc_solve_var_base_reader_release(&(self->r));
    fc_solve_var_base_writer_release(&(self->w));
}
#endif

static inline int wanted_suit_bit_opt(const fcs_card parent_card)
{
    return ((fcs_card_suit(parent_card) & 0x2) ? OPT_PARENT_SUIT_MOD_IS_1
                                               : OPT_PARENT_SUIT_MOD_IS_0);
}

static inline int wanted_suit_idx_opt(const fcs_card parent_card)
{
    return OPT__BAKERS_DOZEN__FIRST_PARENT + fcs_card_suit(parent_card);
}

static inline int calc_child_card_option(
    const fcs_dbm_variant_type local_variant, const fcs_card parent_card,
    const fcs_card child_card PASS_ON_NOT_FC_ONLY(
        const int sequences_are_built_by))
{
    if (IS_BAKERS_DOZEN())
    {
        if ((fcs_card_rank(child_card) != 1) &&
            (fcs_card_rank(child_card) + 1 == fcs_card_rank(parent_card)))
        {
            return wanted_suit_idx_opt(parent_card);
        }
        else
        {
            return OPT__BAKERS_DOZEN__ORIG_POS;
        }
    }
    else
    {
        if ((fcs_card_rank(child_card) != 1) &&
            (fcs_is_parent_card(child_card, parent_card)))
        {
            return wanted_suit_bit_opt(parent_card);
        }
        else
        {
            return OPT_ORIG_POS;
        }
    }
}

static inline int get_top_rank_for_iter(
    const fcs_dbm_variant_type local_variant)
{
    return (IS_BAKERS_DOZEN() ? (RANK_KING - 1) : RANK_KING);
}

static void fc_solve_delta_stater_encode_composite(fcs_delta_stater *const self,
    const fcs_dbm_variant_type local_variant, fcs_var_base_writer *const writer)
{
    fcs_state *const derived = self->derived_state;

    fc_solve_delta_stater__init_card_states(self);

    for (int suit_idx = 0; suit_idx < FCS_NUM_SUITS; ++suit_idx)
    {
        const unsigned long rank = fcs_foundation_value(*derived, suit_idx);

        fc_solve_var_base_writer_write(writer, FOUNDATION_BASE, rank);

        const unsigned long max_rank = ((rank < 1) ? 1 : rank);

        for (unsigned long r = 1; r <= max_rank; ++r)
        {
#define CARD_POS(card) ((size_t)(card))
#define CARD_STATE(card) self->card_states[CARD_POS(card)]
#define SET_CARD_STATE(card, opt) CARD_STATE(card) = (opt)
#define RS_STATE(rank, suit_idx) CARD_STATE(fcs_make_card(rank, suit_idx))
            RS_STATE(r, suit_idx) = OPT_DONT_CARE;
        }
    }

#if MAX_NUM_FREECELLS > 0
    for (int fc_idx = 0; fc_idx < self->num_freecells; ++fc_idx)
    {
        const fcs_card card = fcs_freecell_card(*derived, fc_idx);

        if (fcs_card_is_valid(card))
        {
            SET_CARD_STATE(card, OPT_FREECELL);
        }
    }
#endif

    if (IS_BAKERS_DOZEN())
    {
        for (size_t col_idx = 0; col_idx < self->num_columns; ++col_idx)
        {
            const_AUTO(col, fcs_state_get_col(*derived, col_idx));
            const int col_len = fcs_col_len(col);

            if (!col_len)
            {
                continue;
            }
            const_AUTO(top_card, fcs_col_get_card(col, 0));

            /* Skip Aces which were already set. */
            if (fcs_card_rank(top_card) != 1)
            {
                SET_CARD_STATE(top_card, OPT__BAKERS_DOZEN__ORIG_POS);
            }

            for (int pos = 1; pos < col_len; ++pos)
            {
                const fcs_card parent_card = fcs_col_get_card(col, pos - 1);
                const fcs_card this_card = fcs_col_get_card(col, pos);

                /* Skip Aces which were already set. */
                if (fcs_card_rank(this_card) != 1)
                {
                    SET_CARD_STATE(
                        this_card, ((fcs_card_rank(this_card) + 1 ==
                                        fcs_card_rank(parent_card))
                                           ? wanted_suit_idx_opt(parent_card)
                                           : OPT__BAKERS_DOZEN__ORIG_POS));
                }
            }
        }
    }
    else
    {
        for (size_t col_idx = 0; col_idx < self->num_columns; ++col_idx)
        {
            const_AUTO(col, fcs_state_get_col(*derived, col_idx));
            const int col_len = fcs_col_len(col);

            if (!col_len)
            {
                continue;
            }
            const_AUTO(top_card, fcs_col_get_card(col, 0));
            if (fcs_card_rank(top_card) != 1)
            {
                SET_CARD_STATE(top_card, OPT_TOPMOST);
            }

            fcs_card parent_card = top_card;
            for (int child_idx = 1; child_idx < col_len; ++child_idx)
            {
                const fcs_card child_card = fcs_col_get_card(col, child_idx);

                if (fcs_card_rank(child_card) != 1)
                {
                    const int opt =
                        calc_child_card_option(local_variant, parent_card,
                            child_card PASS_ON_NOT_FC_ONLY(
                                self->sequences_are_built_by));
                    SET_CARD_STATE(child_card, opt);
                }

                parent_card = child_card;
            }
        }
    }

    /*
     * All cards should be determined now - let's encode.
     *
     * The foundations have already been encoded.
     *
     * Skip encoding the aces, and the kings are encoded with less bits.
     */
    const int top_rank_for_iter = get_top_rank_for_iter(local_variant);
    for (int rank = 2; rank <= top_rank_for_iter; ++rank)
    {
        for (int suit_idx = 0; suit_idx < FCS_NUM_SUITS; ++suit_idx)
        {
            const unsigned long opt = RS_STATE(rank, suit_idx);
            unsigned long base;

            if (IS_BAKERS_DOZEN())
            {
                const_AUTO(card, fcs_card2char(fcs_make_card(rank, suit_idx)));

                if (self->bakers_dozen_topmost_cards_lookup[card >> 3] &
                    (1 << (card & (8 - 1))))
                {
                    continue;
                }
                base = NUM__BAKERS_DOZEN__OPTS;
            }
            else
            {
                base = ((rank == RANK_KING) ? NUM_KING_OPTS : NUM_OPTS);
            }

            assert(opt < base);

            fc_solve_var_base_writer_write(writer, base, opt);
        }
    }
}

static inline void delta_stater__fill_column_with_descendent_cards(
    fcs_delta_stater *const self, const fcs_dbm_variant_type local_variant,
    fcs_cards_column *const col)
{
    fcs_card parent_card = fcs_col_get_card(*col, fcs_col_len(*col) - 1);

    while (fcs_card_is_valid(parent_card))
    {
        const int wanted_opt =
            (IS_BAKERS_DOZEN() ? wanted_suit_idx_opt(parent_card)
                               : wanted_suit_bit_opt(parent_card));

        fcs_card child_card = fc_solve_empty_card;
        const int candidate_rank = fcs_card_rank(parent_card) - 1;
        for (int suit = (IS_BAKERS_DOZEN()
                             ? 0
                             : ((fcs_card_suit(parent_card) & (0x1)) ^ 0x1));
             suit < FCS_NUM_SUITS; suit += (IS_BAKERS_DOZEN() ? 1 : 2))
        {
            const fcs_card candidate_card = fcs_make_card(candidate_rank, suit);

            if (CARD_STATE(candidate_card) == wanted_opt)
            {
                child_card = candidate_card;
                break;
            }
        }

        if (fcs_card_is_valid(child_card))
        {
            fcs_col_push_card(*col, child_card);
        }
        parent_card = child_card;
    }
}

static void fc_solve_delta_stater_decode(fcs_delta_stater *const self,
    const fcs_dbm_variant_type local_variant, fcs_var_base_reader *const reader,
    fcs_state *const ret)
{
    fcs_card new_top_most_cards[MAX_NUM_STACKS];

    fc_solve_delta_stater__init_card_states(self);

    for (int suit_idx = 0; suit_idx < FCS_NUM_SUITS; ++suit_idx)
    {
        const unsigned long foundation_rank =
            fc_solve_var_base_reader_read(reader, FOUNDATION_BASE);

        for (unsigned long rank = 1; rank <= foundation_rank; ++rank)
        {
            RS_STATE(rank, suit_idx) = OPT_IN_FOUNDATION;
        }

        fcs_set_foundation(*ret, suit_idx, foundation_rank);
    }

#define IS_IN_FOUNDATIONS(card)                                                \
    (fcs_card_rank(card) <= fcs_foundation_value(*ret, fcs_card_suit(card)))

    fcs_state *const init_state = self->init_state;

    const int orig_pos_opt =
        (IS_BAKERS_DOZEN() ? OPT__BAKERS_DOZEN__ORIG_POS : OPT_ORIG_POS);

#if MAX_NUM_FREECELLS > 0
    const_SLOT(num_freecells, self);
#endif

    bool orig_top_most_cards[CARD_ARRAY_LEN] = {FALSE};
    for (size_t col_idx = 0; col_idx < self->num_columns; ++col_idx)
    {
        const_AUTO(col, fcs_state_get_col(*init_state, col_idx));

        if (fcs_col_len(col))
        {
            orig_top_most_cards[CARD_POS(fcs_col_get_card(col, 0))] = TRUE;
        }
    }

    /* Process the kings: */
    if (IS_BAKERS_DOZEN())
    {
        for (int suit_idx = 0; suit_idx < FCS_NUM_SUITS; ++suit_idx)
        {
            const fcs_card card = fcs_make_card(RANK_KING, suit_idx);

            if (!IS_IN_FOUNDATIONS(card))
            {
                SET_CARD_STATE(card, OPT__BAKERS_DOZEN__ORIG_POS);
            }
        }
    }

#if MAX_NUM_FREECELLS > 0
    int next_freecell_idx = 0;
#endif
    int next_new_top_most_cards = 0;
    const int top_rank_for_iter = get_top_rank_for_iter(local_variant);
    for (int rank = 1; rank <= top_rank_for_iter; ++rank)
    {
        for (int suit_idx = 0; suit_idx < FCS_NUM_SUITS; ++suit_idx)
        {
            const fcs_card card = fcs_make_card(rank, suit_idx);
            const_AUTO(card_char, fcs_card2char(card));

            if (IS_BAKERS_DOZEN())
            {
                if ((self->bakers_dozen_topmost_cards_lookup[card_char >> 3] &
                        (1 << (card_char & (8 - 1)))))
                {
                    if (!IS_IN_FOUNDATIONS(card))
                    {
                        SET_CARD_STATE(card, OPT__BAKERS_DOZEN__ORIG_POS);
                    }
                    continue;
                }
            }

            const int existing_opt = RS_STATE(rank, suit_idx);

            if (rank == 1)
            {
                if (existing_opt < 0)
                {
                    RS_STATE(rank, suit_idx) = orig_pos_opt;
                }
            }
            else
            {
                const unsigned long base =
                    (IS_BAKERS_DOZEN()
                            ? NUM__BAKERS_DOZEN__OPTS
                            : ((rank == RANK_KING) ? NUM_KING_OPTS : NUM_OPTS));
                const unsigned long item_opt =
                    fc_solve_var_base_reader_read(reader, base);

                if (existing_opt < 0)
                {
                    RS_STATE(rank, suit_idx) = item_opt;

                    if (!IS_BAKERS_DOZEN())
                    {
                        if (item_opt == OPT_FREECELL)
                        {
#if MAX_NUM_FREECELLS > 0
                            fcs_put_card_in_freecell(
                                *ret, next_freecell_idx, card);
                            ++next_freecell_idx;
#endif
                        }
                        else if (item_opt == OPT_TOPMOST)
                        {
                            if (!orig_top_most_cards[CARD_POS(card)])
                            {
                                new_top_most_cards[next_new_top_most_cards++] =
                                    card;
                            }
                        }
                    }
                }
            }
        }
    }

#if MAX_NUM_FREECELLS > 0
    for (; next_freecell_idx < num_freecells; ++next_freecell_idx)
    {
        fcs_empty_freecell(*ret, next_freecell_idx);
    }
#endif

    for (size_t col_idx = 0; col_idx < self->num_columns; ++col_idx)
    {
        fcs_card top_card;
        int top_opt;

        fcs_cards_column col = fcs_state_get_col(*ret, col_idx);
        const_AUTO(orig_col, fcs_state_get_col(*init_state, col_idx));
        const_AUTO(orig_col_len, fcs_col_len(orig_col));

        if (orig_col_len)
        {
            top_card = fcs_col_get_card(orig_col, 0);
            top_opt = CARD_STATE(top_card);
        }
        else
        {
            top_card = fc_solve_empty_card;
            top_opt = -1;
        }

        if (fcs_card_is_empty(top_card) ||
            (!((top_opt == OPT_TOPMOST) || (top_opt == orig_pos_opt))))
        {
            if (next_new_top_most_cards > 0)
            {
                fcs_col_push_card(
                    col, new_top_most_cards[--next_new_top_most_cards]);

                delta_stater__fill_column_with_descendent_cards(
                    self, local_variant, &col);
            }
        }
        else
        {
            fcs_col_push_card(col, top_card);
            var_AUTO(parent_card, top_card);
            for (int pos = 1; pos < orig_col_len; ++pos)
            {
                const fcs_card child_card = fcs_col_get_card(orig_col, pos);

                if ((!IS_IN_FOUNDATIONS(child_card)) &&
                    (CARD_STATE(child_card) ==
                        calc_child_card_option(local_variant, parent_card,
                            child_card PASS_ON_NOT_FC_ONLY(
                                self->sequences_are_built_by))))
                {
                    fcs_col_push_card(col, child_card);
                    parent_card = child_card;
                }
                else
                {
                    break;
                }
            }

            delta_stater__fill_column_with_descendent_cards(
                self, local_variant, &col);
        }
    }
#undef IS_IN_FOUNDATIONS
}
#undef IS_BAKERS_DOZEN

static inline void fc_solve_delta_stater_decode_into_state_proto(
    const fcs_dbm_variant_type local_variant,
    fcs_delta_stater *const delta_stater, const fcs_uchar *const enc_state,
    fcs_state_keyval_pair *const ret IND_BUF_T_PARAM(indirect_stacks_buffer))
{
    fc_solve_var_base_reader_start(
        &(delta_stater->r), enc_state, sizeof(fcs_encoded_state_buffer));

    fc_solve_state_init(ret, STACKS_NUM, indirect_stacks_buffer);

    fc_solve_delta_stater_decode(
        delta_stater, local_variant, &(delta_stater->r), &(ret->s));
}

static inline void fc_solve_delta_stater_encode_into_buffer(
    fcs_delta_stater *const delta_stater,
    const fcs_dbm_variant_type local_variant,
    fcs_state_keyval_pair *const state, unsigned char *const out_enc_state)
{
    fc_solve_var_base_writer_start(&(delta_stater->w));
    fc_solve_delta_stater_set_derived(delta_stater, &(state->s));
    fc_solve_delta_stater_encode_composite(
        delta_stater, local_variant, &(delta_stater->w));
    fc_solve_var_base_writer_get_data(&(delta_stater->w), out_enc_state);
}
#endif
