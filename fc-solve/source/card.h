/*
 * card.h - header file for card functions for Freecell Solver
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */


#ifndef __CARD_H
#define __CARD_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __STATE_H
#include "state.h"
#endif

/*
 * This function converts an entire card from its string representations
 * (e.g: "AH", "KS", "8D"), to a fcs_card_t data type.
 * */
extern fcs_card_t freecell_solver_card_user2perl(const char * str);
#define fcs_card_user2perl(str) (freecell_solver_card_user2perl(str))



/*
 * Convert an entire card to its user representation.
 *
 * */
extern char * freecell_solver_card_perl2user(
    fcs_card_t card,
    char * str,
    int t
    );

#define fcs_card_perl2user(card,str,t) (freecell_solver_card_perl2user((card),(str),(t)))



/*
 * Converts a card_number from its internal representation to a string.
 *
 * num - the card number
 * str - the string to output to.
 * card_num_is_null - a pointer to a bool that indicates whether
 *      the card number is out of range or equal to zero
 * t - whether 10 should be printed as T or not.
 * */
extern char * freecell_solver_p2u_card_number(
    int num,
    char * str,
    int * card_num_is_null,
    int t,
    int flipped
    );

#define fcs_p2u_card_number(num,str,card_num_is_null,t,flipped) \
    (freecell_solver_p2u_card_number((num),(str),(card_num_is_null),(t),(flipped)))

/*
 * Converts a suit to its user representation.
 *
 * */
char * freecell_solver_p2u_suit(
    int suit,
    char * str,
    int card_num_is_null,
    int flipped
    );

#define fcs_p2u_suit(suit,str,card_num_is_null,flipped) \
    (freecell_solver_p2u_suit((suit),(str),(card_num_is_null),(flipped)))

/*
 * This function converts a card number from its user representation
 * (e.g: "A", "K", "9") to its card number that can be used by
 * the program.
 * */
extern int freecell_solver_u2p_card_number(const char * string);
#define fcs_u2p_card_number(string) (freecell_solver_u2p_card_number(string))

/*
 * This function converts a string containing a suit letter (that is
 * one of H,S,D,C) into its suit ID.
 *
 * The suit letter may come somewhat after the beginning of the string.
 *
 * */
extern int freecell_solver_u2p_suit(const char * deck);
#define fcs_u2p_suit(deck) (freecell_solver_u2p_suit(deck))

#ifdef __cplusplus
}
#endif

#endif /* __CARD_H */
