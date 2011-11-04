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
 * A test for the delta states routines.
 */

#include <string.h>
#include <stdio.h>

#include <tap.h>
#include "../card.c"
#include "../app_str.c"
#include "../state.c"
#include "../delta_states.c"

static fcs_card_t make_card(int rank, int suit)
{
    fcs_card_t card;

    card = fc_solve_empty_card;

    fcs_card_set_num(card, rank);
    fcs_card_set_suit(card, suit);

    return card;
}

#define FREECELLS_NUM 2
#define STACKS_NUM 8
#define DECKS_NUM 1

#ifdef INDIRECT_STACK_STATES
typedef char ind_buf_t[MAX_NUM_STACKS << 7];
#endif

int main_tests()
{
    {
        fcs_state_keyval_pair_t s;
        fc_solve_delta_stater_t delta;

#ifdef INDIRECT_STACK_STATES
        ind_buf_t indirect_stacks_buffer;
#endif
        fcs_cards_column_t col;

#ifndef FCS_FREECELL_ONLY
        delta.sequences_are_built_by = FCS_SEQ_BUILT_BY_ALTERNATE_COLOR;
#endif
        fc_solve_state_init(&s, STACKS_NUM
#ifdef INDIRECT_STACK_STATES
                , indirect_stacks_buffer
#endif
                );

        col = fcs_state_get_col(s.s, 0);

        /* TEST
         * */
        ok (fc_solve_get_column_orig_num_cards(&delta, col) == 0,
                "Empty column has zero orig cards."
           );

        fcs_col_push_card(col, make_card(13, 0));
        /* TEST
         * */
        ok (fc_solve_get_column_orig_num_cards(&delta, col) == 0,
                "Column with a single card has zero orig cards."
           );

        fcs_col_push_card(col, make_card(12, 1));
        /* TEST
         * */
        ok (fc_solve_get_column_orig_num_cards(&delta, col) == 0,
                "Column with a seq of 2 cards has zero orig cards."
           );

        /* A non-matching card. */
        fcs_col_push_card(col, make_card(4, 1));
        /* TEST
         * */
        ok (fc_solve_get_column_orig_num_cards(&delta, col) == 3,
                "3 original cards."
           );

        /* A matching card. */
        fcs_col_push_card(col, make_card(3, 2));
        /* TEST
         * */
        ok (fc_solve_get_column_orig_num_cards(&delta, col) == 3,
                "3 original cards, with one card on top."
           );

    }

    {
        fc_solve_delta_stater_t * delta;
        fcs_state_keyval_pair_t init_state, derived_state;
#ifdef INDIRECT_STACK_STATES
        ind_buf_t indirect_stacks_buffer, derived_indirect_stacks_buffer;
#endif

        /* MS Freecell No. 982 Initial state.
         * */
        fc_solve_initial_user_state_to_c(
                ("Foundations: H-0 C-0 D-A S-0\n"
                "6D 3C 3H KD 8C 5C\n"
                "TC 9C 9H 4S JC 6H 5H\n"
                "2H 2D 3S 5D 9D QS KS\n"
                "6S TD QC KH AS AH 7C\n"
                "KC 4H TH 7S 2C 9S\n"
                "AC QD 8D QH 3D 8S\n"
                "7H 7D JD JH TS 6C\n"
                "4C 4D 5S 2S JS 8H\n"),
                &init_state,
                FREECELLS_NUM,
                STACKS_NUM,
                DECKS_NUM
#ifdef INDIRECT_STACK_STATES
                , indirect_stacks_buffer
#endif
        );

        delta = fc_solve_delta_stater_alloc(
                &init_state.s,
                STACKS_NUM,
                FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
                , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
                );

        /* TEST
         *  */
        ok (delta, "Delta was created.");

        fc_solve_initial_user_state_to_c(
                (
                 "Foundations: H-0 C-2 D-A S-0 \n"
                 "Freecells:  8D  QD\n"
                 "6D 3C 3H KD 8C 5C\n"
                 "TC 9C 9H 8S\n"
                 "2H 2D 3S 5D 9D QS KS QH JC\n"
                 "6S TD QC KH AS AH 7C 6H\n"
                 "KC 4H TH 7S\n"
                 "9S\n"
                 "7H 7D JD JH TS 6C 5H 4S 3D\n"
                 "4C 4D 5S 2S JS 8H\n"
                ) ,
                &derived_state,
                FREECELLS_NUM,
                STACKS_NUM,
                DECKS_NUM
#ifdef INDIRECT_STACK_STATES
                , derived_indirect_stacks_buffer
#endif
        );

        fc_solve_delta_stater_set_derived(delta, &(derived_state.s));

        {
            fc_solve_column_encoding_composite_t enc;
            fc_solve_get_column_encoding_composite(delta, 0, &enc);

            /* TEST
             * */
            ok (enc.enc[0] == 
                    (6  /* 3 bits of orig len. */
                     | (0 << 3) /*  4 bits of derived len. */
                    )
                    , "fc_solve_get_column_encoding_composite() test 1 - byte 0"
            );

            /* TEST
             */
            ok (enc.end == enc.enc, "Only 7 bits.");

            /* TEST
             * */
            ok (enc.bit_in_char_idx == 7, "Only 7 bits (2).");
        }

#define SUIT_HC 0
#define SUIT_DS 1

        {
            fc_solve_column_encoding_composite_t enc;
            fc_solve_get_column_encoding_composite(delta, 1, &enc);

            /* TEST
             * */
            ok (enc.enc[0] ==
                    (3  /* 3 bits of orig len. */
                     | (1 << 3) /*  4 bits of derived len. */
                     | (SUIT_DS << (3+4)) /* 1 bit of suit. */
                    )
                    , "fc_solve_get_column_encoding_composite() test 2 - byte 0"
            );

            /* TEST
             */
            ok (enc.end == enc.enc+1, "8 bits.");

            /* TEST
             * */
            ok (enc.bit_in_char_idx == 0, "8 bits (2).");
        }

        {
            fc_solve_column_encoding_composite_t enc;
            int card_9S;

            fc_solve_get_column_encoding_composite(delta, 5, &enc);

            card_9S = (9 | (3 << 4));

            /* TEST
             * */
            ok (enc.enc[0] ==
                    (0  /* 3 bits of orig len. */
                     | (1 << 3) /*  4 bits of derived len. */
                     | ((card_9S&0x1) << (3+4)) /* 1 bit of init_card. */
                    )
                    , "fc_solve_get_column_encoding_composite() col 5 - byte 0"
            );

            /* TEST
             * */
            ok ((enc.enc[1] ==
                    (card_9S >> 1) /* Remaining 5 bits of card. */
                )
                , "fc_solve_get_column_encoding_composite() col 5 - byte 1"
            );

            /* TEST
             */
            ok (enc.end == enc.enc+1, "3+4+7 bits.");

            /* TEST
             * */
            ok (enc.bit_in_char_idx == (3+4+6-8), "3+4+7 bits (2).");
        }

        {
            fc_solve_bit_writer_t bit_w;
            fc_solve_bit_reader_t bit_r;
            fcs_uchar_t enc[10];

            fc_solve_bit_writer_init(&bit_w, enc);
            fc_solve_bit_reader_init(&bit_r, enc);

            fc_solve_get_freecells_encoding(delta, &bit_w);

            /* TEST
             * */
            ok (fc_solve_bit_reader_read(&bit_r, 6) == (8 | (2 << 4)), /* 8D */
                    "First freecell is 8D.");

            /* TEST
             * */
            ok (fc_solve_bit_reader_read(&bit_r, 6) == (12 | (2 << 4)), /* QD */
                    "Second freecell is QD.");

            /* TEST
             * */
            ok (bit_r.current == bit_w.current && 
                    (bit_r.bit_in_char_idx == bit_w.bit_in_char_idx),
                    "Reached the end of the encoding.");

        }

        {
            fcs_state_keyval_pair_t new_derived_state;
#ifdef INDIRECT_STACK_STATES
            ind_buf_t new_derived_indirect_stacks_buffer;
#endif
            fcs_uchar_t enc_state[24];
            fc_solve_bit_writer_t bit_w;
            fc_solve_bit_reader_t bit_r;
            char * as_str;
            const char * expected_str = 
            (
"Foundations: H-0 C-2 D-A S-0 \n"
"Freecells:  8D  QD\n"
": 6D 3C 3H KD 8C 5C\n"
": TC 9C 9H 8S\n"
": 2H 2D 3S 5D 9D QS KS QH JC\n"
": 6S TD QC KH AS AH 7C 6H\n"
": KC 4H TH 7S\n"
": 9S\n"
": 7H 7D JD JH TS 6C 5H 4S 3D\n"
": 4C 4D 5S 2S JS 8H\n"
            );

            fc_solve_state_init(&new_derived_state, STACKS_NUM
#ifdef INDIRECT_STACK_STATES
                , new_derived_indirect_stacks_buffer
#endif
                );

            fc_solve_bit_writer_init(&bit_w, enc_state);
            fc_solve_delta_stater_encode_composite(delta, &bit_w);
            
            fc_solve_bit_reader_init(&bit_r, enc_state);
            fc_solve_delta_stater_decode(delta, &bit_r, &(new_derived_state.s));

            as_str =
                fc_solve_state_as_string(
                    &new_derived_state,
                    FREECELLS_NUM,
                    STACKS_NUM,
                    DECKS_NUM,
                    1,
                    0,
                    1
                    );

            /* TEST
             * */
            if (!ok(!strcmp(as_str, expected_str), "encode_composite + decode test"))
            {
                diag("got == <<<\n%s\n>>> ; expected == <<<\n%s\n>>>\n",
                        as_str,
                        expected_str
                    );
            }

            free(as_str);
        }

        fc_solve_delta_stater_free (delta);
    }
    return 0;
}

int main(int argc, char * argv[])
{
    plan_tests(20);
    main_tests();
    return exit_status();
}
