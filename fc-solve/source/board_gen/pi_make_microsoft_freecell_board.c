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
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "alloc_wrap.h"
#include "bool.h"
#include "gen_ms_boards__rand.h"

typedef int CARD;

const int NUM_CARDS = (4*13);
#define     CLUB            0               /*  SUIT(card)  */
#define     DIAMOND         1
#define     HEART           2
#define     SPADE           3

#define     SUIT(card)      ((card) % 4)
#define     VALUE(card)     ((card) / 4)

#define     MAXPOS         21
#define     MAXCOL          9    /* includes top row as column 0 */

static char * card_to_string(char * s, const CARD card, const fcs_bool_t not_append_ws, const fcs_bool_t print_ts)
{
    const int suit = SUIT(card);
    const int v = VALUE(card)+1;

    if (v == 1)
    {
        strcpy(s, "A");
    }
    else if (v < 10)
    {
        sprintf(s, "%i", v);
    }
    else if (v == 10)
    {
        if (print_ts)
        {
            strcpy(s, "T");
        }
        else
        {
            strcpy(s, "10");
        }
    }
    else
    {
        strcpy(s, (v == 11)?"J":((v == 12)?"Q":"K"));
    }

    switch (suit)
    {
        case CLUB:
            strcat(s, "C");
            break;
        case DIAMOND:
            strcat(s, "D");
            break;
        case HEART:
            strcat(s, "H");
            break;
        case SPADE:
            strcat(s, "S");
            break;
    }

    if (!not_append_ws)
    {
        strcat(s, " ");
    }

    return s;
}

int main(int argc, char * argv[])
{

    CARD    card[MAXCOL][MAXPOS];    /* current layout of cards, CARDs are ints */

    int  num_cards_left = NUM_CARDS;          /*  cards left to be chosen in shuffle */
    CARD deck[NUM_CARDS];            /* deck of 52 unique cards */
    fcs_bool_t print_ts = FALSE;

    int arg = 1;
    if (arg < argc)
    {
        if (!strcmp(argv[arg], "-t"))
        {
            print_ts = TRUE;
            arg++;
        }
    }
    const long long gamenumber = ((arg < argc) ? atoll(argv[arg++]) : (long long)time(NULL));

    /* shuffle cards */

    for (int i = 0; i < NUM_CARDS; i++)      /* put unique card in each deck loc. */
        deck[i] = i;

    long long seedx = (microsoft_rand_uint_t)(
        (gamenumber < 0x100000000LL) ? gamenumber
                                     : (gamenumber - 0x100000000LL));
    for (int i = 0; i < NUM_CARDS; i++)
    {
        const microsoft_rand_uint_t j =
                microsoft_rand__game_num_rand(&seedx, gamenumber) %
                num_cards_left;
        card[(i%8)+1][i/8] = deck[j];
        deck[j] = deck[--num_cards_left];
    }

    for (int stack=1 ; stack<9 ; stack++ )
    {
        for (int c=0 ; c < (6+(stack<5)) ; c++)
        {
            char card_string[10];
            printf("%s",
                card_to_string(
                    card_string,
                    card[stack][c],
                    (c == (6+(stack<5))),
                    print_ts
                )
            );
        }
        printf("%s", "\n");
    }

    return 0;
}
