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
 * its user representation.
 *
 */

#define BUILDING_DLL 1

#include <string.h>
#include <ctype.h>

#include "card.h"
#include "inline.h"
#include "bool.h"

#ifdef DEBUG_STATES

fcs_card_t fc_solve_empty_card = {0,0};

#elif defined(COMPACT_STATES) || defined (INDIRECT_STACK_STATES)

fcs_card_t fc_solve_empty_card = (fcs_card_t)0;

#endif

/*
 * This function converts a card number from its user representation
 * (e.g: "A", "K", "9") to its card number that can be used by
 * the program.
 * */
int fc_solve_u2p_rank(const char * string)
{
    while (1)
    {
        switch (toupper(*string))
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
            case '1':
                return (string[1] == '0')?10:1;
            case 'T':
            case '0':
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
int fc_solve_u2p_suit(const char * suit)
{
    while (TRUE)
    {
        switch(toupper(*suit))
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
static GCC_INLINE int fcs_u2p_flipped_status(const char * str)
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
 * This function converts an entire card from its string representations
 * (e.g: "AH", "KS", "8D"), to a fcs_card_t data type.
 * */
fcs_card_t fc_solve_card_user2perl(const char * str)
{
    fcs_card_t card;

    card = fc_solve_empty_card;

#ifndef FCS_WITHOUT_CARD_FLIPPING
    fcs_card_set_flipped(card, fcs_u2p_flipped_status(str));
#endif
    fcs_card_set_num(card, fc_solve_u2p_rank(str));
    fcs_card_set_suit(card, fc_solve_u2p_suit(str));

    return card;
}


/*
 * Those strings contain the string representations of the different cards.
 * If CARD_DEBUG_PRES is defined then an asterisk is printed as an empty card.
 *
 * Notice that there are two of them: one prints 10 and one prints T for the
 * 10 card.
 *
 * */
#ifdef CARD_DEBUG_PRES
static char card_map_3_10[14][4] = { "*", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };

static char card_map_3_T[14][4] = { "*", "A", "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K" };

#else
static char card_map_3_10[14][4] = { " ", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };

static char card_map_3_T[14][4] = { " ", "A", "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K" };

#endif

/*
 * Converts a rank from its internal representation to a string.
 *
 * rank_idx - the card number
 * str - the string to output to.
 * rank_is_null - a pointer to a bool that indicates whether
 *      the card number is out of range or equal to zero
 * t - whether 10 should be printed as T or not.
 * flipped - whether the card is face down
 * */
char * fc_solve_p2u_rank(
    int rank_idx,
    char * str,
    fcs_bool_t * rank_is_null,
    fcs_bool_t t
#ifndef FCS_WITHOUT_CARD_FLIPPING
    , fcs_bool_t flipped
#endif
    )
{
    char (*card_map_3) [4] = card_map_3_10;
    if (t)
    {
        card_map_3 = card_map_3_T;
    }
#if defined(CARD_DEBUG_PRES) || defined(FCS_WITHOUT_CARD_FLIPPING)
#else
    if (flipped)
    {
        strncpy(str, "*", 2);
        *rank_is_null = FALSE;
    }
    else
#endif
    {
        if ((rank_idx >= 0) && (rank_idx <= 13))
        {
            strcpy(str, card_map_3[rank_idx]);
            *rank_is_null = (rank_idx == 0);
        }
        else
        {
            strncpy(str, card_map_3[0], strlen(card_map_3[0])+1);
            *rank_is_null = TRUE;
        }
    }
    return str;
}

/*
 * Converts a suit to its user representation.
 *
 * */
static GCC_INLINE char * fc_solve_p2u_suit(
        int suit,
        char * str,
        fcs_bool_t card_num_is_null
#ifndef FCS_WITHOUT_CARD_FLIPPING
        , fcs_bool_t flipped
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
    if (suit == 0)
    {
        if (card_num_is_null)
#ifdef CARD_DEBUG_PRES
            strncpy(str, "*", 2);
#else
            strncpy(str, " ", 2);
#endif
        else
            strncpy(str, "H", 2);
    }
    else if (suit == 1)
        strncpy(str, "C", 2);
    else if (suit == 2)
        strncpy(str, "D", 2);
    else if (suit == 3)
        strncpy(str, "S", 2);
    else
        strncpy(str, " ", 2);
    return str;
}

/*
 * Convert an entire card to its user representation.
 *
 * */
char * fc_solve_card_perl2user(fcs_card_t card, char * str, fcs_bool_t t)
{
    fcs_bool_t card_num_is_null;
#ifdef CARD_DEBUG_PRES
    if (fcs_card_get_flipped(card))
    {
        *str = '<';
        str++;
    }
#endif

    fc_solve_p2u_rank(
        fcs_card_rank(card),
        str,
        &card_num_is_null,
        t
#ifndef FCS_WITHOUT_CARD_FLIPPING
        ,
        fcs_card_get_flipped(card)
#endif
        );
    /*
     * Notice that if card_num_is_null is found to be true
     * it will affect the output of the suit too.
     *
     * */
    fc_solve_p2u_suit(
        fcs_card_suit(card),
        str+strlen(str),
        card_num_is_null
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

    return str;
}
