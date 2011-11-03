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
#include "../delta_states.c"

static fcs_card_t make_card(int rank, int suit)
{
    fcs_card_t card;

    card = fc_solve_empty_card;

    fcs_card_set_num(card, rank);
    fcs_card_set_suit(card, suit);

    return card;
}

#define STACKS_NUM 8
int main_tests()
{
    {
        fcs_state_keyval_pair_t s;
        fc_solve_delta_stater_t delta;

#ifdef INDIRECT_STACK_STATES
        char indirect_stacks_buffer[STACKS_NUM << 7];
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

#define FREECELLS_NUM 2
#define STACKS_NUM 8
#define DECKS_NUM 1
    {
        fc_solve_delta_stater_t * delta;
        fcs_state_keyval_pair_t init_state, derived_state;
        char indirect_stacks_buffer[STACKS_NUM << 7];
        char derived_indirect_stacks_buffer[STACKS_NUM << 7];

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

        {
            fc_solve_column_encoding_composite_t enc;
            fc_solve_get_column_encoding_composite(delta, 1, &enc);

#define SUIT_HC 0
#define SUIT_DS 1
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

        fc_solve_delta_stater_free (delta);
    }
    return 0;
}

int main(int argc, char * argv[])
{
    plan_tests(12);
    main_tests();
    return exit_status();
}
