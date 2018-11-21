/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
// A test for the delta states routines.
#include "delta_states_test_common.h"

static fcs_card make_card(int rank, int suit)
{
    return fcs_make_card(rank, suit);
}

static void main_tests(void)
{
    const fcs_dbm_variant_type local_variant = FCS_DBM_VARIANT_2FC_FREECELL;
    {
        fcs_state_keyval_pair s;
        fcs_delta_stater delta;

        fcs_cards_column col;
        DECLARE_IND_BUF_T(indirect_stacks_buffer)

        FCS_ON_NOT_FC_ONLY(
            delta.sequences_are_built_by = FCS_SEQ_BUILT_BY_ALTERNATE_COLOR);
        fc_solve_state_init(&s, STACKS_NUM, indirect_stacks_buffer);

        col = fcs_state_get_col(s.s, 0);

        /* TEST
         * */
        ok(fc_solve_get_column_orig_num_cards(&delta, col) == 0,
            "Empty column has zero orig cards.");

        fcs_col_push_card(col, make_card(13, 0));
        /* TEST
         * */
        ok(fc_solve_get_column_orig_num_cards(&delta, col) == 0,
            "Column with a single card has zero orig cards.");

        fcs_col_push_card(col, make_card(12, 1));
        /* TEST
         * */
        ok(fc_solve_get_column_orig_num_cards(&delta, col) == 0,
            "Column with a seq of 2 cards has zero orig cards.");

        /* A non-matching card. */
        fcs_col_push_card(col, make_card(4, 1));
        /* TEST
         * */
        ok(fc_solve_get_column_orig_num_cards(&delta, col) == 3,
            "3 original cards.");

        /* A matching card. */
        fcs_col_push_card(col, make_card(3, 2));
        /* TEST
         * */
        ok(fc_solve_get_column_orig_num_cards(&delta, col) == 3,
            "3 original cards, with one card on top.");
    }

    {
        fcs_state_keyval_pair init_state, derived_state;
        DECLARE_IND_BUF_T(indirect_stacks_buffer)
        DECLARE_IND_BUF_T(derived_indirect_stacks_buffer)

        // MS Freecell No. 982 Initial state.
        fc_solve_initial_user_state_to_c(("Foundations: H-0 C-0 D-A S-0\n"
                                          "6D 3C 3H KD 8C 5C\n"
                                          "TC 9C 9H 4S JC 6H 5H\n"
                                          "2H 2D 3S 5D 9D QS KS\n"
                                          "6S TD QC KH AS AH 7C\n"
                                          "KC 4H TH 7S 2C 9S\n"
                                          "AC QD 8D QH 3D 8S\n"
                                          "7H 7D JD JH TS 6C\n"
                                          "4C 4D 5S 2S JS 8H\n"),
            &init_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            indirect_stacks_buffer);

        fcs_delta_stater delta;
        fc_solve_delta_stater_init(&delta, local_variant, &init_state.s,
            STACKS_NUM,
            FREECELLS_NUM PASS_ON_NOT_FC_ONLY(
                FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));

        fc_solve_initial_user_state_to_c(("Foundations: H-0 C-2 D-A S-0\n"
                                          "Freecells:  8D  QD\n"
                                          "6D 3C 3H KD 8C 5C\n"
                                          "TC 9C 9H 8S\n"
                                          "2H 2D 3S 5D 9D QS KS QH JC\n"
                                          "6S TD QC KH AS AH 7C 6H\n"
                                          "KC 4H TH 7S\n"
                                          "9S\n"
                                          "7H 7D JD JH TS 6C 5H 4S 3D\n"
                                          "4C 4D 5S 2S JS 8H\n"),
            &derived_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            derived_indirect_stacks_buffer);

        fc_solve_delta_stater_set_derived(&delta, &(derived_state.s));

        {
            fcs_column_encoding_composite enc;
            fc_solve_get_column_encoding_composite(&delta, 0, &enc);

            /* TEST
             * */
            ok(enc.enc[0] == (6             /* 3 bits of orig len. */
                                 | (0 << 3) /*  4 bits of derived len. */
                                 ),
                "fc_solve_get_column_encoding_composite() test 1 - byte 0");

            /* TEST
             */
            ok(enc.end == enc.enc, "Only 7 bits.");

            /* TEST
             * */
            ok(enc.bit_in_char_idx == 7, "Only 7 bits (2).");
        }
#define SUIT_DS 1
        {
            fcs_column_encoding_composite enc;
            fc_solve_get_column_encoding_composite(&delta, 1, &enc);

            /* TEST
             * */
            ok(enc.enc[0] == (3             /* 3 bits of orig len. */
                                 | (1 << 3) /*  4 bits of derived len. */
                                 | (SUIT_DS << (3 + 4)) /* 1 bit of suit. */
                                 ),
                "fc_solve_get_column_encoding_composite() test 2 - byte 0");

            /* TEST
             */
            ok(enc.end == enc.enc + 1, "8 bits.");

            /* TEST
             * */
            ok(enc.bit_in_char_idx == 0, "8 bits (2).");
        }

        {
            fcs_column_encoding_composite enc;
            fcs_card card_9S;

            fc_solve_get_column_encoding_composite(&delta, 5, &enc);

            card_9S = make_card(9, 3);
            /* TEST
             * */
            ok(enc.enc[0] ==
                    (0             /* 3 bits of orig len. */
                        | (1 << 3) /*  4 bits of derived len. */
                        | ((card_9S & 0x1) << (3 + 4)) /* 1 bit of init_card. */
                        ),
                "fc_solve_get_column_encoding_composite() col 5 - byte 0");

            /* TEST
             * */
            ok((enc.enc[1] == (card_9S >> 1) /* Remaining 5 bits of card. */
                   ),
                "fc_solve_get_column_encoding_composite() col 5 - byte 1");

            /* TEST
             */
            ok(enc.end == enc.enc + 1, "3+4+7 bits.");

            /* TEST
             * */
            ok(enc.bit_in_char_idx == (3 + 4 + 6 - 8), "3+4+7 bits (2).");
        }

#if MAX_NUM_FREECELLS > 0
        {
            fc_solve_bit_writer bit_w;
            fcs_bit_reader bit_r;
            fcs_uchar enc[10];

            fc_solve_bit_writer_init(&bit_w, enc);
            fc_solve_bit_reader_init(&bit_r, enc);

            fc_solve_get_freecells_encoding(&delta, &bit_w);

            /* TEST
             * */
            ok((fcs_card)fc_solve_bit_reader_read(&bit_r, 6) ==
                    make_card(8, 2), /* 8D */
                "First freecell is 8D.");

            /* TEST
             * */
            ok((fcs_card)fc_solve_bit_reader_read(&bit_r, 6) ==
                    make_card(12, 2), /* QD */
                "Second freecell is QD.");

            /* TEST
             * */
            ok(bit_r.current == bit_w.current &&
                    (bit_r.bit_in_char_idx == bit_w.bit_in_char_idx),
                "Reached the end of the encoding.");
        }
#endif

        /* TEST
         * */
        test_encode_and_decode(local_variant, &delta, &derived_state,
            ("Foundations: H-0 C-2 D-A S-0\n"
             "Freecells:  8D  QD\n"
             ": 6D 3C 3H KD 8C 5C\n"
             ": TC 9C 9H 8S\n"
             ": 2H 2D 3S 5D 9D QS KS QH JC\n"
             ": 6S TD QC KH AS AH 7C 6H\n"
             ": KC 4H TH 7S\n"
             ": 9S\n"
             ": 7H 7D JD JH TS 6C 5H 4S 3D\n"
             ": 4C 4D 5S 2S JS 8H\n"),
            "encode_composite + decode test");
        fc_solve_delta_stater_release(&delta);
    }

    /* More encode_composite tests - this time from the output of:
     * pi-make-microsoft-freecell-board -t 24 | \
     *      ./fc-solve -to 01ABCDE --freecells-num 2 -s -i -p -t
     */
    {
        fcs_state_keyval_pair init_state, derived_state;

        DECLARE_IND_BUF_T(indirect_stacks_buffer)
        DECLARE_IND_BUF_T(derived_indirect_stacks_buffer)

        // MS Freecell No. 24 Initial state.
        fc_solve_initial_user_state_to_c(("Foundations: H-0 C-0 D-0 S-0\n"
                                          "Freecells:\n"
                                          "4C 2C 9C 8C QS 4S 2H\n"
                                          "5H QH 3C AC 3H 4H QD\n"
                                          "QC 9S 6H 9H 3S KS 3D\n"
                                          "5D 2S JC 5C JH 6D AS\n"
                                          "2D KD TH TC TD 8D\n"
                                          "7H JS KH TS KC 7C\n"
                                          "AH 5S 6S AD 8H JD\n"
                                          "7S 6C 7D 4D 8S 9D\n"),
            &init_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            indirect_stacks_buffer);

        fcs_delta_stater delta;
        fc_solve_delta_stater_init(&delta, local_variant, &init_state.s,
            STACKS_NUM,
            FREECELLS_NUM PASS_ON_NOT_FC_ONLY(
                FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));

        fc_solve_initial_user_state_to_c(
            ("Foundations: H-0 C-0 D-0 S-4\n"
             "Freecells:  KS  TD\n"
             "2C\n"
             "5H QH 3C AC 3H 4H QD JC TH 9C 8D 7S\n"
             "QC 9S 6H 9H 8C 7D 6C 5D 4C 3D\n"
             "\n"
             "2D KD QS JH TC 9D 8S\n"
             "7H JS KH TS KC 7C 6D 5C 4D\n"
             "AH 5S 6S AD 8H JD\n"
             "2H\n"),
            &derived_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            derived_indirect_stacks_buffer);

        /* TEST
         * */
        test_encode_and_decode(local_variant, &delta, &derived_state,
            ("Foundations: H-0 C-0 D-0 S-4\n"
             "Freecells:  TD  KS\n"
             ":\n"
             ": 5H QH 3C AC 3H 4H QD JC TH 9C 8D 7S\n"
             ": QC 9S 6H 9H 8C 7D 6C 5D 4C 3D\n"
             ": 2H\n"
             ": 2D KD QS JH TC 9D 8S\n"
             ": 7H JS KH TS KC 7C 6D 5C 4D\n"
             ": AH 5S 6S AD 8H JD\n"
             ": 2C\n"),
            "encode_composite + decode test No. 2 (deal #24)");

        fc_solve_initial_user_state_to_c(("Foundations: H-0 C-0 D-0 S-2\n"
                                          "Freecells:  TD  4C\n"
                                          "8S\n"
                                          "5H QH 3C AC 3H 4H 3S 2H\n"
                                          "QC 9S 6H 9H 8C 7D 6C 5D 4S 3D 2C\n"
                                          "5C 4D\n"
                                          "2D KD QS JH TC 9D\n"
                                          "7H JS KH TS KC 7C\n"
                                          "AH 5S 6S AD 8H JD\n"
                                          "KS QD JC TH 9C 8D 7S 6D\n"),
            &derived_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            derived_indirect_stacks_buffer);

        /* TEST
         * */
        test_encode_and_decode(local_variant, &delta, &derived_state,
            ("Foundations: H-0 C-0 D-0 S-2\n"
             "Freecells:  4C  TD\n"
             ": 5C 4D\n"
             ": 5H QH 3C AC 3H 4H 3S 2H\n"
             ": QC 9S 6H 9H 8C 7D 6C 5D 4S 3D 2C\n"
             ": 8S\n"
             ": 2D KD QS JH TC 9D\n"
             ": 7H JS KH TS KC 7C\n"
             ": AH 5S 6S AD 8H JD\n"
             ": KS QD JC TH 9C 8D 7S 6D\n"),
            "encode_composite + decode test No. 3 (deal #24)");

        fc_solve_delta_stater_release(&delta);
    }

    {
        char *s = fc_solve_user_INTERNAL_delta_states_enc_and_dec(local_variant,
            ("Foundations: H-0 C-0 D-0 S-0\n"
             "Freecells:\n"
             ": 4C 2C 9C 8C QS 4S 2H\n"
             ": 5H QH 3C AC 3H 4H QD\n"
             ": QC 9S 6H 9H 3S KS 3D\n"
             ": 5D 2S JC 5C JH 6D AS\n"
             ": 2D KD TH TC TD 8D\n"
             ": 7H JS KH TS KC 7C\n"
             ": AH 5S 6S AD 8H JD\n"
             ": 7S 6C 7D 4D 8S 9D\n"),
            ("Foundations: H-0 C-0 D-0 S-4\n"
             "Freecells:  KS  TD\n"
             ": 2C\n"
             ": 5H QH 3C AC 3H 4H QD JC TH 9C 8D 7S\n"
             ": QC 9S 6H 9H 8C 7D 6C 5D 4C 3D\n"
             ":\n"
             ": 2D KD QS JH TC 9D 8S\n"
             ": 7H JS KH TS KC 7C 6D 5C 4D\n"
             ": AH 5S 6S AD 8H JD\n"
             ": 2H\n"));

        trim_trailing_whitespace(s);
        /* TEST
         * */
        ok(!strcmp(s, ("Foundations: H-0 C-0 D-0 S-4\n"
                       "Freecells:  TD  KS\n"
                       ":\n"
                       ": 5H QH 3C AC 3H 4H QD JC TH 9C 8D 7S\n"
                       ": QC 9S 6H 9H 8C 7D 6C 5D 4C 3D\n"
                       ": 2H\n"
                       ": 2D KD QS JH TC 9D 8S\n"
                       ": 7H JS KH TS KC 7C 6D 5C 4D\n"
                       ": AH 5S 6S AD 8H JD\n"
                       ": 2C\n")),
            "Good string after encode_composite+decode.");

        free(s);
    }

    /* Make sure encode_composite avoids permutations of empty columns
     * and completely-non-original states.
     */
    {
        fcs_delta_stater delta;
        fcs_state_keyval_pair init_state, derived_state;

        DECLARE_IND_BUF_T(indirect_stacks_buffer)
        DECLARE_IND_BUF_T(derived_indirect_stacks_buffer)

        fc_solve_initial_user_state_to_c(("Foundations: H-K C-K D-J S-Q\n"
                                          "Freecells:  KD\n"
                                          "\n"
                                          "\n"
                                          "KS QD\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"),
            &init_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            indirect_stacks_buffer);

        fc_solve_delta_stater_init(&delta, local_variant, &init_state.s,
            STACKS_NUM,
            FREECELLS_NUM PASS_ON_NOT_FC_ONLY(
                FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));

        fc_solve_initial_user_state_to_c(("Foundations: H-K C-K D-J S-Q\n"
                                          "Freecells:  KD\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "KS\n"
                                          "\n"
                                          "\n"
                                          "\n"),
            &derived_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            derived_indirect_stacks_buffer);

        fcs_encoded_state_buffer first_enc_state;

        fcs_init_and_encode_state(
            &delta, local_variant, &derived_state, &first_enc_state);

        fc_solve_initial_user_state_to_c(("Foundations: H-K C-K D-J S-Q\n"
                                          "Freecells:  KD\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "KS\n"),
            &derived_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            derived_indirect_stacks_buffer);

        fcs_encoded_state_buffer second_enc_state;

        fcs_init_and_encode_state(
            &delta, local_variant, &derived_state, &second_enc_state);

        /* TEST
         * */
        ok((!memcmp(
               first_enc_state.s, second_enc_state.s, sizeof(first_enc_state))),
            "Make sure encode_composite avoids permutations of empty columns "
            "and completely-non-original states.");

        fc_solve_delta_stater_release(&delta);
    }
    /* Make sure encode_composite avoids permutations of empty columns
     * and completely-non-original states.
     *
     * Another edge-case.
     */
    {
        fcs_delta_stater delta;
        fcs_state_keyval_pair init_state, derived_state;

        DECLARE_IND_BUF_T(indirect_stacks_buffer)
        DECLARE_IND_BUF_T(derived_indirect_stacks_buffer)
        fc_solve_initial_user_state_to_c(("Foundations: H-K C-K D-J S-Q\n"
                                          "Freecells:  KD\n"
                                          "\n"
                                          "\n"
                                          "KS QD\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"),
            &init_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            indirect_stacks_buffer);

        fc_solve_delta_stater_init(&delta, local_variant, &init_state.s,
            STACKS_NUM,
            FREECELLS_NUM PASS_ON_NOT_FC_ONLY(
                FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));

        fc_solve_initial_user_state_to_c(("Foundations: H-K C-K D-J S-Q\n"
                                          "Freecells:  KD\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "KS\n"),
            &derived_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            derived_indirect_stacks_buffer);

        fcs_encoded_state_buffer first_enc_state;
        fcs_init_and_encode_state(
            &delta, local_variant, &derived_state, &first_enc_state);

        fc_solve_initial_user_state_to_c(("Foundations: H-K C-K D-J S-Q\n"
                                          "Freecells:  KD\n"
                                          "KS\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"
                                          "\n"),
            &derived_state, FREECELLS_NUM, STACKS_NUM, DECKS_NUM,
            derived_indirect_stacks_buffer);

        fcs_encoded_state_buffer second_enc_state;
        fcs_init_and_encode_state(
            &delta, local_variant, &derived_state, &second_enc_state);

        /* TEST
         * */
        ok((!memcmp(
               &first_enc_state, &second_enc_state, sizeof(first_enc_state))),
            "encode_composite unique encoding No. 2");

        fc_solve_delta_stater_release(&delta);
    }
}

int main(void)
{
    plan_tests(24);
    main_tests();
    return exit_status();
}
