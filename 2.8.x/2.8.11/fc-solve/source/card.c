/*
 * card.c - functions to convert cards and card components to and from
 * its user representation.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#include <string.h>

#include "card.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif


#define uc(c) ( (((c)>='a') && ((c)<='z')) ?  ((c)+'A'-'a') : (c))

/*
 * This function converts a card number from its user representation
 * (e.g: "A", "K", "9") to its card number that can be used by
 * the program.
 * */
int freecell_solver_u2p_card_number(const char * string)
{
    char rest;

    while (1)
    {
        rest = uc(*string);

        if ((rest == '\0') || (rest == ' ') || (rest == '\t'))
        {
            return 0;
        }
        if (rest == 'A')
        {
            return 1;
        }
        else if (rest =='J')
        {
            return 11;
        }
        else if (rest == 'Q')
        {
            return 12;
        }
        else if (rest == 'K')
        {
            return 13;
        }
        else if (rest == '1')
        {
            return (*(string+1) == '0')?10:1;
        }
        else if ((rest == '0') || (rest == 'T'))
        {
            return 10;
        }
        else if ((rest >= '2') && (rest <= '9'))
        {
            return (rest-'0');
        }
        else
        {
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
int freecell_solver_u2p_suit(const char * suit)
{
    char c;

    c = uc(*suit);
    while (
            (c != 'H') &&
            (c != 'S') &&
            (c != 'D') &&
            (c != 'C') &&
            (c != ' ') &&
            (c != '\0'))
    {
        suit++;
        c = uc(*suit);
    }

    if (c == 'H')
        return 0;
    else if (c == 'C')
        return 1;
    else if (c == 'D')
        return 2;
    else if (c == 'S')
        return 3;
    else
        return 0;
}

static int fcs_u2p_flipped_status(const char * str)
{
    while (*str != '\0')
    {
        if ((*str != ' ') && (*str != '\t'))
        {
            return (*str == '<');
        }
        str++;
    }
    return 0;
}
/*
 * This function converts an entire card from its string representations
 * (e.g: "AH", "KS", "8D"), to a fcs_card_t data type.
 * */
fcs_card_t freecell_solver_card_user2perl(const char * str)
{
    fcs_card_t card;
#if defined(COMPACT_STATES)||defined(INDIRECT_STACK_STATES)
    card = 0;
#endif
    fcs_card_set_flipped(card, fcs_u2p_flipped_status(str));
    fcs_card_set_num(card, fcs_u2p_card_number(str));
    fcs_card_set_suit(card, fcs_u2p_suit(str));

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
 * Converts a card_number from its internal representation to a string.
 *
 * num - the card number
 * str - the string to output to.
 * card_num_is_null - a pointer to a bool that indicates whether
 *      the card number is out of range or equal to zero
 * t - whether 10 should be printed as T or not.
 * flipped - whether the card is face down
 * */
char * freecell_solver_p2u_card_number(
    int num,
    char * str,
    int * card_num_is_null,
    int t,
    int flipped)
{
    char (*card_map_3) [4] = card_map_3_10;
    if (t)
    {
        card_map_3 = card_map_3_T;
    }
#ifdef CARD_DEBUG_PRES
    if (0)
    {
    }
#else
    if (flipped)
    {
        strncpy(str, "*", 2);
        *card_num_is_null = 0;
    }
#endif
    else
    {
        if ((num >= 0) && (num <= 13))
        {
            strncpy(str, card_map_3[num], strlen(card_map_3[num])+1);
            *card_num_is_null = (num == 0);
        }
        else
        {
            strncpy(str, card_map_3[0], strlen(card_map_3[0])+1);
            *card_num_is_null = 1;
        }
    }
    return str;
}

/*
 * Converts a suit to its user representation.
 *
 * */
char * freecell_solver_p2u_suit(int suit, char * str, int card_num_is_null, int flipped)
{
#ifndef CARD_DEBUG_PRES
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
char * freecell_solver_card_perl2user(fcs_card_t card, char * str, int t)
{
    int card_num_is_null;
#ifdef CARD_DEBUG_PRES
    if (fcs_card_get_flipped(card))
    {
        *str = '<';
        str++;
    }
#endif

    fcs_p2u_card_number(
        fcs_card_card_num(card),
        str,
        &card_num_is_null,
        t,
        fcs_card_get_flipped(card)
        );
    /*
     * Notice that if card_num_is_null is found to be true
     * it will affect the output of the suit too.
     *
     * */
    fcs_p2u_suit(
        fcs_card_suit(card),
        str+strlen(str),
        card_num_is_null,
        fcs_card_get_flipped(card)
        );

#ifdef CARD_DEBUG_PRES
    if (fcs_card_get_flipped(card))
    {
        strcat(str, ">");
    }
#endif

    return str;
}
