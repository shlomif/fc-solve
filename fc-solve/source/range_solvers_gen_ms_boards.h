// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// range_solvers_gen_ms_boards.h - a header file that defines some
// static (and preferably inline) routines for generating the Microsoft
// boards.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "freecell-solver/fcs_back_compat.h"
#include "gen_ms_boards__rand.h"
#include "board_gen_lookup1.h"

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

static inline void get_board__setup_string(char *const ret)
{
    strcpy(ret, "XX XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX\n"
                "XX XX XX XX XX XX\n");
}

#ifdef FCS_BREAK_BACKWARD_COMPAT_2
#define FCS_BOARD_GEN__SWAP_SUITS 1
#endif
#ifdef FCS_BOARD_GEN__SWAP_SUITS
typedef enum
{
    FCS_SWAP_NEVER = 0,
    FCS_SWAP_ALWAYS,
    FCS_SWAP_LOWER_FIRST,
    FCS_SWAP_HIGHER_FIRST,
} fcs_get_board_when_to_swap;

typedef enum
{
    WHICH_COLOR = 0,
    WHICH_BLACK,
    WHICH_RED,
} fcs_get_board_which_swapper;

typedef struct
{
    fcs_get_board_when_to_swap whens[3];
} fcs_get_board_strategy_type;

static fcs_get_board_strategy_type global_strategy;

static int fcs_int_getenv(const char *name)
{
    const char *const val = getenv(name);
    return (val ? atoi(val) : 0);
}

static void fcs_init_get_board_strategy(fcs_get_board_strategy_type *const dest)
{
    dest->whens[WHICH_COLOR] = fcs_int_getenv("FCS_SWAP_COLOR");
    dest->whens[WHICH_BLACK] = fcs_int_getenv("FCS_SWAP_BLACK");
    dest->whens[WHICH_RED] = fcs_int_getenv("FCS_SWAP_RED");
}
#endif

static inline void get_board_l__without_setup(
#ifdef FCS_DEAL_ONLY_UP_TO_2G
    const fc_solve_ms_deal_idx_type seedx
#else
    const fc_solve_ms_deal_idx_type deal_idx
#endif
    ,
    char *const ret)
{
// #define FCS_DEAL_ONLY_UP_TO_2G
#ifdef FCS_DEAL_ONLY_UP_TO_2G
#else
    microsoft_rand seedx =
        microsoft_rand__calc_init_seedx((microsoft_rand)deal_idx);
#endif
    CARD deck[52]; /* deck of 52 unique cards */

    /* shuffle cards */
#ifdef FCS_BOARD_GEN__SWAP_SUITS
    CARD color_mask = (global_strategy.whens[WHICH_COLOR] <= FCS_SWAP_ALWAYS
                           ? global_strategy.whens[WHICH_COLOR]
                           : 4);
    CARD black_mask = (global_strategy.whens[WHICH_BLACK] <= FCS_SWAP_ALWAYS
                           ? (global_strategy.whens[WHICH_BLACK] * 3)
                           : 4);
    CARD red_mask = (global_strategy.whens[WHICH_RED] <= FCS_SWAP_ALWAYS
                         ? (global_strategy.whens[WHICH_RED] * 3)
                         : 4);
    // CARD black_mask = 4, red_mask = 0;
#endif
    for (size_t i = 0; i < 52; ++i) /* put unique card in each deck loc. */
    {
        deck[i] = i;
    }

    microsoft_rand_uint num_cards_left = 52;
    for (size_t i = 0; i < 52; ++i)
    {
#ifdef FCS_DEAL_ONLY_UP_TO_2G
        const microsoft_rand_uint j =
            microsoft_rand_rand(&seedx) % num_cards_left;
#if 0
        exit(-1);
#endif
#else
        const microsoft_rand_uint j =
            microsoft_rand__game_num_rand(&seedx, deal_idx) % num_cards_left;
#endif
        CARD card = deck[j];
#ifdef FCS_BOARD_GEN__SWAP_SUITS
        CARD suit = SUIT(card);
        if (color_mask == 4)
        {
            color_mask =
                ((global_strategy.whens[WHICH_COLOR] ==
                     FCS_SWAP_HIGHER_FIRST) == (suit == 0 || suit == 3));
        }
        card ^= color_mask;
        suit = SUIT(card);
        if (suit == 0 || suit == 3)
        {
            if (black_mask == 4)
            {
                black_mask = ((global_strategy.whens[WHICH_BLACK] ==
                                  FCS_SWAP_HIGHER_FIRST) == (suit == 0)) *
                             3;
            }
            card ^= black_mask;
        }
        else
        {
            if (red_mask == 4)
            {
                red_mask = ((global_strategy.whens[WHICH_RED] ==
                                FCS_SWAP_HIGHER_FIRST) == (suit == 1)) *
                           3;
            }
            card ^= red_mask;
        }
#endif
        card_to_string(&ret[offset_by_i[i]], card);
        deck[j] = deck[--num_cards_left];
    }
}

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
void DLLEXPORT fc_solve_get_board_l(
    const fc_solve_ms_deal_idx_type deal_idx, char *const ret);
#endif

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
void DLLEXPORT fc_solve_get_board_l(
    const fc_solve_ms_deal_idx_type deal_idx, char *const ret);

extern void DLLEXPORT fc_solve_get_board_l(
#else
static inline void get_board_l(
#endif
    const fc_solve_ms_deal_idx_type seedx, char *const ret)
{
    get_board__setup_string(ret);
    get_board_l__without_setup(seedx, ret);
}

#ifndef FCS_GEN_BOARDS_WITH_EXTERNAL_API
static inline void get_board(unsigned long deal_idx, char *ret)
{
    get_board_l(deal_idx, ret);
}
#endif

typedef char fcs_state_string[52 * 3 + 8 + 1];

#ifdef __cplusplus
}
#endif
