/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// range_solvers_gen_ms_boards.h - a header file that defines some
// static (and preferably inline) routines for generating the Microsoft
// boards.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gen_ms_boards__rand.h"

typedef size_t CARD;

#define SUIT(card) ((card) & (4 - 1))
#define VALUE(card) ((card) >> 2)

static const char *card_to_string_values = "A23456789TJQK";
static const char *card_to_string_suits = "CDHS";

static inline void card_to_string(char *const s, const CARD card)
{
    s[0] = card_to_string_values[VALUE(card)];
    s[1] = card_to_string_suits[SUIT(card)];
}

#include "board_gen_lookup1.h"
#ifdef FCS_GEN_BOARDS_WITH_EXTERNAL_API
/* This is to settle gcc's -Wmissing-prototypes which complains about
 * missing prototypes for "extern" subroutines.
 *
 * It is not critical that it would be in the same place because the only
 * thing that uses this function is Python's ctypes (in the test files under
 * t/t/ ) which does not process the included C code. In the future, we may
 * have an external API in which case we'll devise a header for this
 * routine.
 *
 * */
void DLLEXPORT fc_solve_get_board_l(const long long deal_idx, char *const ret);

extern void DLLEXPORT fc_solve_get_board_l(
    const long long deal_idx, char *const ret)
#else
static inline void get_board_l(const long long deal_idx, char *const ret)
#endif
{
    long long seedx = microsoft_rand__calc_init_seedx(deal_idx);
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

    for (size_t i = 0; i < 52; ++i) /* put unique card in each deck loc. */
    {
        deck[i] = i;
    }

    microsoft_rand_uint num_cards_left = 52;
    for (size_t i = 0; i < 52; ++i)
    {
        const microsoft_rand_uint j =
            microsoft_rand__game_num_rand(&seedx, deal_idx) % num_cards_left;
        card_to_string(&ret[offset_by_i[i]], deck[j]);
        deck[j] = deck[--num_cards_left];
    }
}

#ifndef FCS_GEN_BOARDS_WITH_EXTERNAL_API
static inline void get_board(long deal_idx, char *ret)
{
    get_board_l((long long)deal_idx, ret);
}
#endif

typedef char fcs_state_string[52 * 3 + 8 + 1];

#ifdef __cplusplus
}
#endif
