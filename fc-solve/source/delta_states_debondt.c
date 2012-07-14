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
 * delta_states_debondt.c - "delta states" are an encoding of states, where the
 * states are encoded and decoded based on a compact delta from the original
 * state.
 *
 * This encoding improves upon the original delta_states.c .
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

#include "var_base_reader.h"
#include "var_base_writer.h"

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS
#include "dbm_common.h"
#endif

#define FOUNDATION_BASE (RANK_KING + 1);

enum
{
    OPT_TOPMOST = 0,
    OPT_DONT_CARE = OPT_TOPMOST,
    OPT_FREECELL = 1,
    OPT_ORIG_POS = 2,
    NUM_KING_OPTS = 3,
    OPT_PARENT_SUIT_MOD_IS_0 = 3,
    OPT_PARENT_SUIT_MOD_IS_1 = 4,
    OPT_IN_FOUNDATION = 5,
    NUM_OPTS_FOR_READ = 6
};

static fc_solve_debondt_delta_stater_t * fc_solve_debondt_delta_stater_alloc(
        fcs_state_t * init_state,
        int num_columns,
        int num_freecells
#ifndef FCS_FREECELL_ONLY
        , int sequences_are_built_by
#endif
        )
{
    fc_solve_debondt_delta_stater_alloc * self;
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

static GCC_INLINE void fc_solve_debondt_delta_stater__init_card_states(
    fc_solve_debondt_delta_stater_t * self
    )
{
    int i;
    int * card_states = self->card_states;
    for (i = 0 ; i < sizeof(self->card_states) / sizeof(self->card_states[0])
         ; i++)
    {
        card_states[i] = -1;
    }
}

static void fc_solve_debondt_delta_stater_free(fc_solve_debondt_delta_stater_free * self)
{
    free(self);
}

static GCC_INLINE void fc_solve_debondt_delta_stater_set_derived(fc_solve_debondt_delta_stater_t * self, fcs_state_t * state)
{
    self->_derived_state = state;
}

#define NUM_SUITS 4
#define GET_SUIT_BIT(card) (( (fcs_card_suit(card)) & 0x2 ) >> 1 )

static GCC_INLINE int wanted_suit_bit_opt(fcs_card_t parent_card)
{
    return GET_SUIT_BIT(parent_card)
        ? OPT_PARENT_SUIT_MOD_IS_1
        : OPT_PARENT_SUIT_MOD_IS_0
    ;
}

static GCC_INLINE int calc_child_card_option(fcs_card_t parent_card, fcs_card_t child_card)
{
    if (
        (fcs_card_card_num(child_card) != 1)
            &&
        (fcs_is_parent_card(child_card, parent_card))
    )
    {
        return wanted_suit_bit_opt(parent_card);
    }
    else
    {
        return OPT_ORIG_POS;
    }
}

#define NUM_SUITS 4
static void fc_solve_debondt_delta_stater_encode_composite(
    fc_solve_debondt_delta_stater_t * self,
    fcs_var_base_writer_t * writer
)
{
    fcs_state_t * derived;

    derived = self->_derived_state;

    fc_solve_debondt_delta_stater__init_card_states(self);

    {
        int suit_idx, rank, max_rank, r;
        for (suit_idx = 0 ; suit_idx < NUM_SUITS ; suit_idx++)
        {
            rank = fcs_foundation_value(*derived, suit_idx);

            fc_solve_var_base_writer_write(writer, FOUNDATION_BASE, rank);

            max_rank = ((rank < 1) ? 1 : rank);

            for (r = 1 ; r <= max_rank ; r++)
            {
#define STATE_POS(suit_idx, rank) (rank - 1 + suit_idx * RANK_KING)
#define CARD_POS(card) (STATE_POS((fcs_card_suit(card)), (fcs_card_card_num(card))))
                self->card_states[STATE_POS(suit_idx, r)] = OPT_DONT_CARE;
            }
        }
    }
    {
        int fc_idx;

        for (fc_idx = 0 ; fc_idx < self->num_freecells ; fc_idx++)
        {
            fcs_card_t card;

            card = fcs_freecell_card(*derived, fc_idx);

            if (fcs_card_card_num(card) != 0)
            {
                self->card_states[CARD_POS(card)] = OPT_DONT_CARE;
            }
        }
    }
    {
        int col_idx;

        for (col_idx = 0; col_idx < self->num_columns; col_idx++)
        {
            fcs_cards_column_t col;
            int col_len;

            col = fcs_state_get_col(*derived, col_idx);
            col_len = fcs_col_len(col);

            if (col_len > 0)
            {
                fcs_card_t top_card, parent_card;
                int child_idx;

                top_card = fcs_col_get_card(col, 0);

                if (fcs_card_card_num(top_card) != 1)
                {
                    self->card_states[CARD_POS(top_card)] = OPT_DONT_CARE;
                }

                parent_card= top_card;

                for (child_idx = 1 ; child_idx < col_len ; child_idx++)
                {
                    fcs_card_t child_card;

                    child_card = fcs_col_get_card(col, child_idx);

                    if (fcs_card_card_num(child_card) != 1)
                    {
                        self->card_states[CARD_POS(child_card)] =
                            calc_child_card_option(parent_card, child_card)
                            ;
                    }

                    parent_card = child_card;
                }
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
    {
        int rank, suit_idx;

        for (rank = 2 ; rank <= RANK_KING ; rank++)
        {
            for (suit_idx = 0 ; suit_idx < NUM_SUITS ; suit_idx++)
            {
                int opt, base;
                opt = self->card_states[STATE_POS(suit_idx, rank)];

                base = ((rank == RANK_KING) ? NUM_KING_OPTS : NUM_OPTS);

                assert (opt >= 0);
                assert (opt < base);

                fc_solve_var_base_writer_write(writer, base, opt);
            }
        }
    }
}

static GCC_INLINE void fc_solve_debondt_delta_stater_decode_into_state_proto(
        fc_solve_debondt_delta_stater_t * delta_stater,
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

    fc_solve_debondt_delta_stater_decode(
        delta_stater,
        &bit_r,
        &(ret->s)
    );
}

#ifdef INDIRECT_STACK_STATES
#define fc_solve_debondt_delta_stater_decode_into_state(delta_stater, enc_state, state_ptr, indirect_stacks_buffer) fc_solve_debondt_delta_stater_decode_into_state_proto(delta_stater, enc_state, state_ptr, indirect_stacks_buffer)
#else
#define fc_solve_debondt_delta_stater_decode_into_state(delta_stater, enc_state, state_ptr, indirect_stacks_buffer) fc_solve_debondt_delta_stater_decode_into_state_proto(delta_stater, enc_state, state_ptr)
#endif

static GCC_INLINE void fc_solve_debondt_delta_stater_encode_into_buffer(
    fc_solve_debondt_delta_stater_t * delta_stater,
    fcs_state_keyval_pair_t * state,
    unsigned char * out_enc_state
)
{
    fc_solve_bit_writer_t bit_w;
    fc_solve_bit_writer_init(&bit_w, out_enc_state+1);
    fc_solve_debondt_delta_stater_set_derived(delta_stater, &(state->s));
    fc_solve_debondt_delta_stater_encode_composite(delta_stater, &bit_w);
    out_enc_state[0] =
        bit_w.current - bit_w.start + (bit_w.bit_in_char_idx > 0)
        ;
}

static GCC_INLINE void fcs_init_and_encode_state(
    fc_solve_debondt_delta_stater_t * delta_stater,
    fcs_state_keyval_pair_t * state,
    fcs_encoded_state_buffer_t * enc_state
)
{
    fcs_init_encoded_state(enc_state);

    fc_solve_debondt_delta_stater_encode_into_buffer(
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
    fc_solve_debondt_delta_stater_t * delta;
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

    delta = fc_solve_debondt_delta_stater_alloc(
            &(init_state.s),
            STACKS_NUM,
            FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
            , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
            );

    fc_solve_debondt_delta_stater_set_derived(delta, &(derived_state.s));

    fc_solve_state_init(
        &new_derived_state,
        STACKS_NUM,
        new_derived_indirect_stacks_buffer
    );

    fc_solve_bit_writer_init(&bit_w, enc_state);
    fc_solve_debondt_delta_stater_encode_composite(delta, &bit_w);

    fc_solve_bit_reader_init(&bit_r, enc_state);
    fc_solve_debondt_delta_stater_decode(delta, &bit_r, &(new_derived_state.s));

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

    fc_solve_debondt_delta_stater_free (delta);

    return new_derived_as_str;
}

#endif
