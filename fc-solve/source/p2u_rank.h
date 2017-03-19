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
 * p2u_rank.h - header file for the p2u_rank function.
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "config.h"
#include "rinutils.h"
/*
 * Those strings contain the string representations of the different cards.
 * If CARD_DEBUG_PRES is defined then an asterisk is printed as an empty card.
 *
 * Notice that there are two of them: one prints 10 and one prints T for the
 * 10 card.
 *
 * */
#ifdef CARD_DEBUG_PRES
#define CARD_ZERO() "*"
#else
#define CARD_ZERO() " "
#endif

/*
 * Converts a rank from its internal representation to a string.
 *
 * rank_idx - the rank
 * str - the string to output to.
 * t - whether 10 should be printed as T or not.
 * */
#define GEN_CARD_MAP(t_card)                                                   \
    {                                                                          \
        CARD_ZERO()                                                            \
        , "A", "2", "3", "4", "5", "6", "7", "8", "9", t_card, "J", "Q", "K"   \
    }

#ifndef FC_SOLVE_IMPLICIT_T_RANK
static const char card_map_3_10[14][4] = GEN_CARD_MAP("10");

static const char card_map_3_T[14][4] = GEN_CARD_MAP("T");
#else
static const char cards_char_map[15] = (CARD_ZERO() "A23456789TJQK");
#endif

static inline void fc_solve_p2u_rank(
    const int rank_idx, char *const str PASS_T(const fcs_bool_t t))
{
    {
#define INDEX() (rank_idx)
#ifdef FC_SOLVE_IMPLICIT_T_RANK
        str[0] = cards_char_map[INDEX()];
        str[1] = '\0';
#else
        strcpy(str, (t ? card_map_3_T : card_map_3_10)[INDEX()]);
#endif
#undef INDEX
    }
}
