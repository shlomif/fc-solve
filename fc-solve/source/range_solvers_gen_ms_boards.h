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

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "rinutils.h"

typedef u_int32_t microsoft_rand_uint_t;

typedef long long microsoft_rand_t;

static GCC_INLINE microsoft_rand_uint_t microsoft_rand_rand(microsoft_rand_t * my_rand)
{
    *my_rand = ((*my_rand) * 214013 + 2531011);
    return ((*my_rand) >> 16) & 0x7fff;
}

static GCC_INLINE microsoft_rand_uint_t microsoft_rand_randp(microsoft_rand_t * my_rand)
{
    *my_rand = ((*my_rand) * 214013 + 2531011);
    return ((*my_rand) >> 16) & 0xffff;
}

static GCC_INLINE microsoft_rand_uint_t microsoft_rand__game_num_rand(microsoft_rand_t * const seedx_ptr, const long long gnGameNumber)
{
    if (gnGameNumber < 0x100000000LL)
    {
        const microsoft_rand_uint_t ret = microsoft_rand_rand(seedx_ptr);
        return ((gnGameNumber < 0x80000000) ? ret : (ret | 0x8000));
    }
    else
    {
        return microsoft_rand_randp(seedx_ptr) + 1;
    }
}

typedef int CARD;


#define     SUIT(card)      ((card) & (4-1))
#define     VALUE(card)     ((card) >> 2)

#define     MAXPOS          7
#define     MAXCOL          8

static const char * card_to_string_values = "A23456789TJQK";
static const char * card_to_string_suits = "CDHS";

static GCC_INLINE void card_to_string(char * const s, const CARD card)
{
    s[0] = card_to_string_values[VALUE(card)];
    s[1] = card_to_string_suits[SUIT(card)];
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
void DLLEXPORT fc_solve_get_board_l(const long long gamenumber, char * const ret);

extern void DLLEXPORT fc_solve_get_board_l(const long long gamenumber, char * const ret)
#else
static GCC_INLINE void get_board_l(const long long gamenumber, char * const ret)
#endif
{
    long long seedx = (microsoft_rand_uint_t)((gamenumber < 0x100000000LL) ? gamenumber : (gamenumber - 0x100000000LL));
    strcpy(ret,
        "XX XX XX XX XX XX XX\n"
        "XX XX XX XX XX XX XX\n"
        "XX XX XX XX XX XX XX\n"
        "XX XX XX XX XX XX XX\n"
        "XX XX XX XX XX XX\n"
        "XX XX XX XX XX XX\n"
        "XX XX XX XX XX XX\n"
        "XX XX XX XX XX XX\n"
    );

    CARD deck[52];            /* deck of 52 unique cards */

    /* shuffle cards */

    for (int i = 0; i < 52; i++)      /* put unique card in each deck loc. */
    {
        deck[i] = i;
    }

    {
        int  num_cards_left = 52;          /*  cards left to be chosen in shuffle */
        for (int i = 0; i < 52; i++)
        {
            const int j
                = microsoft_rand__game_num_rand(&seedx, gamenumber) % num_cards_left;
            const int col = (i & (8-1));
            const int card_idx = i >> 3;
            card_to_string(
                &ret[3 * (col * 7 - ((col > 4) ? (col-4) : 0) + card_idx)],
                deck[j]
            );
            deck[j] = deck[--num_cards_left];
        }
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
#ifdef FCS_GEN_BOARDS_WITH_EXTERNAL_API
    fc_solve_get_board_l((long long)gamenumber, ret);
#else
    get_board_l((long long)gamenumber, ret);
#endif
}


typedef char fcs_state_string_t[52*3 + 8 + 1];

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__RANGE_SOLVERS_GEN_MS_BOARDS_H */
