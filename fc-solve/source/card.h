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
 * card.h - header file for card functions for Freecell Solver
 *
 */


#ifndef FC_SOLVE__CARD_H
#define FC_SOLVE__CARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "state.h"

/*
 * This function converts an entire card from its string representations
 * (e.g: "AH", "KS", "8D"), to a fcs_card_t data type.
 * */
extern fcs_card_t fc_solve_card_user2perl(const char * str);
#define fcs_card_user2perl(str) (fc_solve_card_user2perl(str))



/*
 * Convert an entire card to its user representation.
 *
 * */
extern char * fc_solve_card_perl2user(
    fcs_card_t card,
    char * str,
    int t
    );

/*
 * Converts a card_number from its internal representation to a string.
 *
 * num - the card number
 * str - the string to output to.
 * card_num_is_null - a pointer to a bool that indicates whether
 *      the card number is out of range or equal to zero
 * t - whether 10 should be printed as T or not.
 * */
extern char * fc_solve_p2u_card_number(
    int num,
    char * str,
    int * card_num_is_null,
    int t
#ifndef FCS_WITHOUT_CARD_FLIPPING
    , int flipped
#endif
    );

/*
 * Converts a suit to its user representation.
 *
 * */
char * fc_solve_p2u_suit(
    int suit,
    char * str,
    int card_num_is_null
#ifndef FCS_WITHOUT_CARD_FLIPPING
    ,
    int flipped
#endif
    );

/*
 * This function converts a card number from its user representation
 * (e.g: "A", "K", "9") to its card number that can be used by
 * the program.
 * */
extern int fc_solve_u2p_card_number(const char * string);
#define fcs_u2p_card_number(string) (fc_solve_u2p_card_number(string))

/*
 * This function converts a string containing a suit letter (that is
 * one of H,S,D,C) into its suit ID.
 *
 * The suit letter may come somewhat after the beginning of the string.
 *
 * */
extern int fc_solve_u2p_suit(const char * deck);
#define fcs_u2p_suit(deck) (fc_solve_u2p_suit(deck))

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__CARD_H */
