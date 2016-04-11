/* Copyright (c) 2000 Shlomi Fish
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
 * card.c - functions to convert cards and card components to and from
 * their user representation.
 *
 */

#define BUILDING_DLL 1

#include <string.h>
#include <ctype.h>

#include "state.h"

#include "p2u_rank.h"

DEFINE_fc_solve_empty_card();

/*
 * This function converts a card number from its user representation
 * (e.g: "A", "K", "9") to its card number that can be used by
 * the program.
 * */

const int fc_solve_u2p_rank(const char * string)
{
    while (1)
    {
        switch (FC_SOLVE_MAP_CHAR(*string))
        {
            case '\0':
            case ' ':
            case '\t':
                return 0;
            case 'A':
                return 1;
            case 'J':
                return 11;
            case 'Q':
                return 12;
            case 'K':
                return 13;
#ifndef FC_SOLVE__STRICTER_BOARD_PARSING
            case '1':
                return (string[1] == '0')?10:1;
#endif
            case 'T':
#ifndef FC_SOLVE__STRICTER_BOARD_PARSING
            case '0':
#endif
                return 10;
            case '2':
                return 2;
            case '3':
                return 3;
            case '4':
                return 4;
            case '5':
                return 5;
            case '6':
                return 6;
            case '7':
                return 7;
            case '8':
                return 8;
            case '9':
                return 9;
            default:
                string++;
        }
    }
}

/*
 * Converts a suit to its user representation.
 *
 * */
static GCC_INLINE void fc_solve_p2u_suit(
        const int suit,
        char * str
        )
{
    *(str++) = "HCDS"[suit];
    *(str) = '\0';
}

/*
 * Convert an entire card to its user representation.
 *
 * */
void fc_solve_card_perl2user(const fcs_card_t card, char * const str
    PASS_T(const fcs_bool_t t)
)
{
    fc_solve_p2u_rank(
        fcs_card_rank(card),
        str
        PASS_T(t)
        );
    fc_solve_p2u_suit(
        fcs_card_suit(card),
        str+strlen(str)
        );
}
