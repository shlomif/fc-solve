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
    pi_make_microsoft_freecell_board.c - Program to generate the initial
    board of Microsoft Freecell or Freecell Pro for input to
    Freecell Solver.

    Usage: pi-make-microsoft-freecell-board [board-number] | fc-solve

    Note: this program is platform independent because it uses its own srand()
    and random() functions which are simliar to the ones used by the Microsoft
    32-bit compilers.

    Based on the code by Jim Horne (who wrote the original Microsoft Freecell)
    Based on code from the Microsoft C Run-Time Library.

    Modified By Shlomi Fish, 2000
*/

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "alloc_wrap.h"
#include "bool.h"
#include "gen_ms_boards__rand.h"

typedef int CARD;

#define NUM_CARDS (4 * 13)
#define SUIT(card) ((card) % 4)
#define VALUE(card) ((card) / 4)

#define MAXCOL 8

static const char *card_to_string_values = "A23456789TJQK";
static const char *card_to_string_suits = "CDHS";
static void card_to_string(char *const s, const CARD card,
    const fcs_bool_t not_append_ws, const fcs_bool_t print_ts)
{
    const int v = VALUE(card);

    if ((v == 9) && (!print_ts))
    {
        strcpy(s, "10");
    }
    else
    {
        sprintf(s, "%c", card_to_string_values[v]);
    }

    sprintf(strchr(s, '\0'), "%c", card_to_string_suits[SUIT(card)]);

    if (!not_append_ws)
    {
        strcat(s, " ");
    }
}

int main(int argc, char *argv[])
{
    CARD card[MAXCOL][7]; /* current layout of cards, CARDs are ints */
    int num_cards_left = NUM_CARDS; /*  cards left to be chosen in shuffle */
    CARD deck[NUM_CARDS];           /* deck of 52 unique cards */
    fcs_bool_t print_ts = FALSE;

    int arg = 1;
    if (arg < argc)
    {
        if (!strcmp(argv[arg], "-t"))
        {
            print_ts = TRUE;
            ++arg;
        }
    }
    const long long gamenumber =
        ((arg < argc) ? atoll(argv[arg++]) : (long long)time(NULL));

    /* shuffle cards */

    for (int i = 0; i < NUM_CARDS; ++i) /* put unique card in each deck loc. */
        deck[i] = i;

    long long seedx = microsoft_rand__calc_init_seedx(gamenumber);
    for (int i = 0; i < NUM_CARDS; ++i)
    {
        const microsoft_rand_uint_t j =
            microsoft_rand__game_num_rand(&seedx, gamenumber) % num_cards_left;
        card[i % 8][i / 8] = deck[j];
        deck[j] = deck[--num_cards_left];
    }

    for (int stack = 0; stack < 8; ++stack)
    {
        const int lim = 6 + (stack < 4);
        for (int c = 0; c < lim; ++c)
        {
            char card_string[5];
            card_to_string(
                card_string, card[stack][c], (c == lim - 1), print_ts);
            fputs(card_string, stdout);
        }
        putchar('\n');
    }

    return 0;
}
