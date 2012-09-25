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
 * A test for the Debondt delta states routines.
 */

#include <string.h>
#include <stdio.h>

#include <tap.h>

#ifndef FCS_COMPILE_DEBUG_FUNCTIONS
#define FCS_COMPILE_DEBUG_FUNCTIONS
#endif

#include "../card.c"
#include "../card_compare_lookups.c"
#include "../app_str.c"
#include "../state.c"
#include "../delta_states_debondt.c"
#include "../dbm_solver_key.h"
#include "../indirect_buffer.h"

static int debondt_test_encode_and_decode(fc_solve_debondt_delta_stater_t * delta, fcs_state_keyval_pair_t * state, const char * expected_str, const char * blurb)
{
    int verdict;
    fcs_state_keyval_pair_t new_derived_state;
    fcs_encoded_state_buffer_t enc_state;
    char * as_str;
    DECLARE_IND_BUF_T(new_derived_indirect_stacks_buffer)
    fcs_state_locs_struct_t locs;
    enum fcs_dbm_variant_type_t local_variant;

    local_variant = FCS_DBM_VARIANT_2FC_FREECELL;

    fc_solve_init_locs(&locs);

    fcs_debondt_init_and_encode_state(
        delta,
        local_variant,
        state,
        &enc_state
    );

    fc_solve_debondt_delta_stater_decode_into_state(
        local_variant,
        delta,
        (unsigned char *)&enc_state,
        &(new_derived_state),
        new_derived_indirect_stacks_buffer
    );

    as_str =
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

    if (!(verdict = ok(!strcmp(as_str, expected_str), "%s", blurb)))
    {
        diag("got == <<<\n%s\n>>> ; expected == <<<\n%s\n>>>\n",
                as_str,
                expected_str
            );
    }

    free(as_str);

    return verdict;
}

static int main_tests(void)
{
    enum fcs_dbm_variant_type_t local_variant;

    local_variant = FCS_DBM_VARIANT_2FC_FREECELL;

    {
        fc_solve_debondt_delta_stater_t * db_delta;
        fcs_state_keyval_pair_t init_state, derived_state;
        DECLARE_IND_BUF_T(indirect_stacks_buffer)
        DECLARE_IND_BUF_T(derived_indirect_stacks_buffer)

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
            DECKS_NUM,
            indirect_stacks_buffer
        );

        db_delta = fc_solve_debondt_delta_stater_alloc(
                local_variant,
                &init_state.s,
                STACKS_NUM,
                FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
                , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
                );

        /* TEST
         *  */
        ok (db_delta, "Debondt-Delta was created.");

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
                DECKS_NUM,
                derived_indirect_stacks_buffer
        );

        fc_solve_debondt_delta_stater_set_derived(db_delta, &(derived_state.s));

#define SUIT_HC 0
#define SUIT_DS 1

        /* TEST
         * */
        debondt_test_encode_and_decode(
            db_delta,
            &derived_state,
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
            ),
            "DeBondt: encode_composite + decode test"
        );
        fc_solve_debondt_delta_stater_free (db_delta);
    }

/* More encode_composite tests - this time from the output of:
 * pi-make-microsoft-freecell-board -t 24 | \
 *      ./fc-solve -to 01ABCDE --freecells-num 2 -s -i -p -t
 */
    {
        fc_solve_debondt_delta_stater_t * db_delta;
        fcs_state_keyval_pair_t init_state, derived_state;

        DECLARE_IND_BUF_T(indirect_stacks_buffer)
        DECLARE_IND_BUF_T(derived_indirect_stacks_buffer)

        /* MS Freecell No. 24 Initial state.
         * */
        fc_solve_initial_user_state_to_c(
("Foundations: H-0 C-0 D-0 S-0 \n"
"Freecells:        \n"
"4C 2C 9C 8C QS 4S 2H\n"
"5H QH 3C AC 3H 4H QD\n"
"QC 9S 6H 9H 3S KS 3D\n"
"5D 2S JC 5C JH 6D AS\n"
"2D KD TH TC TD 8D\n"
"7H JS KH TS KC 7C\n"
"AH 5S 6S AD 8H JD\n"
"7S 6C 7D 4D 8S 9D\n"),
            &init_state,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            indirect_stacks_buffer
        );

        db_delta = fc_solve_debondt_delta_stater_alloc(
                local_variant,
                &init_state.s,
                STACKS_NUM,
                FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
                , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
                );

        fc_solve_initial_user_state_to_c(
            (
"Foundations: H-0 C-0 D-0 S-4 \n"
"Freecells:  KS  TD\n"
"2C\n"
"5H QH 3C AC 3H 4H QD JC TH 9C 8D 7S\n"
"QC 9S 6H 9H 8C 7D 6C 5D 4C 3D\n"
"\n"
"2D KD QS JH TC 9D 8S\n"
"7H JS KH TS KC 7C 6D 5C 4D\n"
"AH 5S 6S AD 8H JD\n"
"2H\n"
            ) ,
            &derived_state,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            derived_indirect_stacks_buffer
        );

        /* TEST
         * */
        debondt_test_encode_and_decode(
            db_delta,
            &derived_state,
            (
"Foundations: H-0 C-0 D-0 S-4 \n"
"Freecells:  TD  KS\n"
": 2C\n"
": 5H QH 3C AC 3H 4H QD JC TH 9C 8D 7S\n"
": QC 9S 6H 9H 8C 7D 6C 5D 4C 3D\n"
": 2H\n"
": 2D KD QS JH TC 9D 8S\n"
": 7H JS KH TS KC 7C 6D 5C 4D\n"
": AH 5S 6S AD 8H JD\n"
": \n"
            ),
            "DeBonodt: encode_composite + decode test No. 2 (deal #24)"
        );


        fc_solve_debondt_delta_stater_free (db_delta);
    }

    return 0;
}

int main(int argc, char * argv[])
{
    plan_tests(3);
    main_tests();
    return exit_status();
}
