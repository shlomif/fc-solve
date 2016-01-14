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
#include "inline.h"
#include "bool.h"

DEFINE_fc_solve_empty_card();

/*
 * This function converts a card number from its user representation
 * (e.g: "A", "K", "9") to its card number that can be used by
 * the program.
 * */
#ifdef FC_SOLVE__STRICTER_BOARD_PARSING
#define FCS_MAP_CHAR(c) (c)
#else
#define FCS_MAP_CHAR(c) (toupper(c))
#endif

const int fc_solve_u2p_rank(const char * string)
{
    while (1)
    {
        switch (FCS_MAP_CHAR(*string))
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
 * This function converts a string containing a suit letter (that is
 * one of H,S,D,C) into its suit ID.
 *
 * The suit letter may come somewhat after the beginning of the string.
 *
 * */
const int fc_solve_u2p_suit(const char * suit)
{
    while (TRUE)
    {
        switch(FCS_MAP_CHAR(*suit))
        {
            case 'H':
            case ' ':
            case '\0':
                return 0;
            case 'C':
                return 1;
            case 'D':
                return 2;
            case 'S':
                return 3;
            default:
                suit++;
        }
    }
}

#ifndef FCS_WITHOUT_CARD_FLIPPING
static GCC_INLINE const int fcs_u2p_flipped_status(const char * str)
{
    while (*str)
    {
        if ((*str != ' ') && (*str != '\t'))
        {
            return (*str == '<');
        }
        str++;
    }
    return FALSE;
}
#endif

/*
 * Converts a suit to its user representation.
 *
 * */
static GCC_INLINE void fc_solve_p2u_suit(
        const int suit,
        char * str
#ifndef FCS_WITHOUT_CARD_FLIPPING
        , const fcs_bool_t flipped
#endif
        )
{
#if !defined(CARD_DEBUG_PRES) && !defined(FCS_WITHOUT_CARD_FLIPPING)
    if (flipped)
    {
        strncpy(str, "*", 2);
    }
    else
#endif
    {
        *(str++) = "HCDS"[suit];
        *(str) = '\0';
    }
}

/*
 * Convert an entire card to its user representation.
 *
 * */
void fc_solve_card_perl2user(const fcs_card_t card, char * const str
    PASS_T(const fcs_bool_t t)
)
{
#ifdef CARD_DEBUG_PRES
    if (fcs_card_get_flipped(card))
    {
        *str = '<';
        str++;
    }
#endif
    fc_solve_p2u_rank(
        fcs_card_rank(card),
        str
        PASS_T(t)
#ifndef FCS_WITHOUT_CARD_FLIPPING
        ,
        fcs_card_get_flipped(card)
#endif
        );
    fc_solve_p2u_suit(
        fcs_card_suit(card),
        str+strlen(str)
#ifndef FCS_WITHOUT_CARD_FLIPPING
        ,
        fcs_card_get_flipped(card)
#endif
        );

#ifdef CARD_DEBUG_PRES
    if (fcs_card_get_flipped(card))
    {
        strcat(str, ">");
    }
#endif
}
