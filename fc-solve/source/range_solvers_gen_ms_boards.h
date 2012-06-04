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
 * range_solvers_gen_ms_boards.h - a header file that defines some
 * static (and preferably inline) routines for generating the Microsoft
 * boards.
 *
 */

#ifndef FC_SOLVE__RANGE_SOLVERS_GEN_MS_BOARDS_H
#define FC_SOLVE__RANGE_SOLVERS_GEN_MS_BOARDS_H

#include "inline.h"
#include "fcs_dllexport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef long microsoft_rand_t;

static GCC_INLINE int microsoft_rand_rand(microsoft_rand_t * my_rand)
{
    *my_rand= ((*my_rand) * 214013 + 2531011);
    return ((*my_rand) >> 16) & 0x7fff;
}

typedef int CARD;


#define     SUIT(card)      ((card) % 4)
#define     VALUE(card)     ((card) / 4)

#define     MAXPOS          7
#define     MAXCOL          8

#ifndef FCS_WITHOUT_GET_BOARD
static const char * card_to_string_values = "A23456789TJQK";
static const char * card_to_string_suits = "CDHS";

static GCC_INLINE char * card_to_string(char * s, CARD card, int not_append_ws)
{
    s[0] = card_to_string_values[VALUE(card)];
    s[1] = card_to_string_suits[SUIT(card)];

    if (not_append_ws)
    {
        return &(s[2]);
    }
    else
    {
        s[2] = ' ';
        return &(s[3]);
    }
}

#ifdef FCS_GEN_BOARDS_WITH_EXTERNAL_API
/* This is to settle gcc's -Wmissing-prototypes which complains about missing
 * prototypes for "extern" subroutines.
 *
 * It is not critical that it would be in the same place because the only thing
 * that uses this function is Python's ctypes (in the test files under t/t/ )
 * which does not process the
 * included C code. In the future, we may have an external API in which case
 * we'll devise a header for this routine.
 *
 * */
void DLLEXPORT fc_solve_get_board(long gamenumber, char * ret);

extern void DLLEXPORT fc_solve_get_board(long gamenumber, char * ret)
#else
static GCC_INLINE void get_board(long gamenumber, char * ret)
#endif
{
    CARD    card[MAXCOL][MAXPOS];    /* current layout of cards, CARDs are ints */

    int  i, j;                /*  generic counters */
    int  wLeft = 52;          /*  cards left to be chosen in shuffle */
    CARD deck[52];            /* deck of 52 unique cards */
    char * append_to;

    /* shuffle cards */

    for (i = 0; i < 52; i++)      /* put unique card in each deck loc. */
    {
        deck[i] = i;
    }

    for (i = 0; i < 52; i++)
    {
        j = microsoft_rand_rand(&gamenumber) % wLeft;
        card[(i%8)][i/8] = deck[j];
        deck[j] = deck[--wLeft];
    }

    append_to = ret;

    {
        int stack;
        int c;

        for(stack=0 ; stack<8 ; stack++ )
        {
            for(c=0 ; c < (6+(stack<4)) ; c++)
            {
                append_to =
                    card_to_string(
                        append_to,
                        card[stack][c],
                        (c == (6-1+(stack<4)))
                    );
            }
            *(append_to++) = '\n';
        }
    }
    *(append_to) = '\0';
}
#endif

typedef char fcs_state_string_t[52*3 + 8 + 1];

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__RANGE_SOLVERS_GEN_MS_BOARDS_H */
