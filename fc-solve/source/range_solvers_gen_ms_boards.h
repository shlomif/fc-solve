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
 * range_solvers_gen_ms_boards.h - a header file that defines some
 * static (and preferably inline) routines for generating the Microsoft
 * boards.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "gen_ms_boards__rand.h"

typedef int CARD;

#define SUIT(card) ((card) & (4 - 1))
#define VALUE(card) ((card) >> 2)

#define MAXPOS 7
#define MAXCOL 8

static const char *card_to_string_values = "A23456789TJQK";
static const char *card_to_string_suits = "CDHS";

static GCC_INLINE void card_to_string(char *const s, const CARD card)
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
void DLLEXPORT fc_solve_get_board_l(
    const long long gamenumber, char *const ret);

extern void DLLEXPORT fc_solve_get_board_l(
    const long long gamenumber, char *const ret)
#else
static GCC_INLINE void get_board_l(const long long gamenumber, char *const ret)
#endif
{
    long long seedx = microsoft_rand__calc_init_seedx(gamenumber);
    strcpy(ret, "XX XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX\n");

    CARD deck[52]; /* deck of 52 unique cards */

    /* shuffle cards */

    for (int i = 0; i < 52; i++) /* put unique card in each deck loc. */
    {
        deck[i] = i;
    }

    {
        microsoft_rand_uint_t num_cards_left =
            52; /*  cards left to be chosen in shuffle */
        for (int i = 0; i < 52; i++)
        {
            const microsoft_rand_uint_t j =
                microsoft_rand__game_num_rand(&seedx, gamenumber) %
                num_cards_left;
            const int col = (i & (8 - 1));
            const int card_idx = i >> 3;
            card_to_string(
                &ret[3 * (col * 7 - ((col > 4) ? (col - 4) : 0) + card_idx)],
                deck[j]);
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
void DLLEXPORT fc_solve_get_board(long gamenumber, char *ret);

extern void DLLEXPORT fc_solve_get_board(long gamenumber, char *ret)
#else
static GCC_INLINE void get_board(long gamenumber, char *ret)
#endif
{
#ifdef FCS_GEN_BOARDS_WITH_EXTERNAL_API
    fc_solve_get_board_l((long long)gamenumber, ret);
#else
    get_board_l((long long)gamenumber, ret);
#endif
}

typedef char fcs_state_string_t[52 * 3 + 8 + 1];

#ifdef __cplusplus
}
#endif
