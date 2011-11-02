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
#include "../delta_states.c"

#ifdef DEBUG_STATES

fcs_card_t fc_solve_empty_card = {0,0};

#elif defined(COMPACT_STATES) || defined (INDIRECT_STACK_STATES)

fcs_card_t fc_solve_empty_card = (fcs_card_t)0;

#endif

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
    }
    return 0;
}

int main(int argc, char * argv[])
{
    plan_tests(3);
    main_tests();
    return exit_status();
}
