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

    Note: this program is platform independant because it uses its own srand()
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
struct microsoft_rand_struct
{
    long seed;
};

typedef struct microsoft_rand_struct microsoft_rand_t;

static microsoft_rand_t * microsoft_rand_alloc(unsigned int seed)
{
    microsoft_rand_t * ret;

    ret = SMALLOC1(ret);
    ret->seed = (long)seed;

    return ret;
}

static void microsoft_rand_free(microsoft_rand_t * my_rand_gen)
{
    free(my_rand_gen);
}

static int microsoft_rand_rand(microsoft_rand_t * my_rand_gen)
{
    my_rand_gen->seed = (my_rand_gen->seed * 214013 + 2531011);
    return (my_rand_gen->seed >> 16) & 0x7fff;
}

typedef int CARD;

#define     BLACK           0               /* COLOUR(card) */
#define     RED             1

#define     ACE             0               /*  VALUE(card) */
#define     DEUCE           1

#define     CLUB            0               /*  SUIT(card)  */
#define     DIAMOND         1
#define     HEART           2
#define     SPADE           3

#define     SUIT(card)      ((card) % 4)
#define     VALUE(card)     ((card) / 4)
#define     COLOUR(card)    (SUIT(card) == DIAMOND || SUIT(card) == HEART)

#define     MAXPOS         21
#define     MAXCOL          9    /* includes top row as column 0 */

static char * card_to_string(char * s, CARD card, int not_append_ws, int print_ts)
{
    int suit = SUIT(card);
    int v = VALUE(card)+1;

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

    int  i, j;                /*  generic counters */
    int  wLeft = 52;          /*  cards left to be chosen in shuffle */
    CARD deck[52];            /* deck of 52 unique cards */
    int gamenumber;
    microsoft_rand_t * my_rand_gen;
    int print_ts = 0;
    int arg;

    if (argc == 1)
    {
        gamenumber = time(NULL);
    }
    else
    {
        arg = 1;
        if (!strcmp(argv[arg], "-t"))
        {
            print_ts = 1;
            arg++;
        }
        gamenumber = atoi(argv[arg]);
    }

    /* shuffle cards */

    for (i = 0; i < 52; i++)      /* put unique card in each deck loc. */
        deck[i] = i;

    my_rand_gen = microsoft_rand_alloc(gamenumber); /* gamenumber is seed for my_rand_gen() */
    for (i = 0; i < 52; i++)
    {
        j = microsoft_rand_rand(my_rand_gen) % wLeft;
        card[(i%8)+1][i/8] = deck[j];
        deck[j] = deck[--wLeft];
    }

    {
        int stack;
        int c;

        char card_string[10];

        for(stack=1 ; stack<9 ; stack++ )
        {
            for(c=0 ; c < (6+(stack<5)) ; c++)
            {
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
    }

    microsoft_rand_free(my_rand_gen);
    return 0;
}
