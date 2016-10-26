/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * card.c - functions to convert cards and card components to and from
 * their user representation.
 */

#include "dll_thunk.h"
#include <string.h>
#include <ctype.h>
#include "state.h"
#include "p2u_rank.h"

#ifdef DEFINE_fc_solve_empty_card
DEFINE_fc_solve_empty_card();
#endif

/*
 * This function converts a card number from its user representation
 * (e.g: "A", "K", "9") to its card number that can be used by
 * the program.
 * */

int fc_solve_u2p_rank(const char *string)
{
    while (1)
    {
        switch (FC_SOLVE_MAP_CHAR(*string))
        {
        case '\0':
        case ' ':
        case '\t':
        case '\n':
        case '\r':
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
            return (string[1] == '0') ? 10 : 1;
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
static GCC_INLINE void fc_solve_p2u_suit(const int suit, char *str)
{
    *(str++) = "HCDS"[suit];
    *(str) = '\0';
}

/*
 * Convert an entire card to its user representation.
 *
 * */
void fc_solve_card_stringify(
    const fcs_card_t card, char *const str PASS_T(const fcs_bool_t t))
{
    fc_solve_p2u_rank(fcs_card_rank(card), str PASS_T(t));
    fc_solve_p2u_suit(fcs_card_suit(card), strchr(str, '\0'));
}
