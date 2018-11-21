/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
// A test for the Debondt delta states routines.
#include "delta_states_test_common.h"

static void main_tests(void)
{
    const fcs_dbm_variant_type local_variant = FCS_DBM_VARIANT_2FC_FREECELL;

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
             ": 2C\n"
             ": 5H QH 3C AC 3H 4H QD JC TH 9C 8D 7S\n"
             ": QC 9S 6H 9H 8C 7D 6C 5D 4C 3D\n"
             ": 2H\n"
             ": 2D KD QS JH TC 9D 8S\n"
             ": 7H JS KH TS KC 7C 6D 5C 4D\n"
             ": AH 5S 6S AD 8H JD\n"
             ":\n"),
            "DeBonodt: encode_composite + decode test No. 2 (deal #24)");
        fc_solve_delta_stater_release(&delta);
    }
}

int main(void)
{
    plan_tests(2);
    main_tests();
    return exit_status();
}
