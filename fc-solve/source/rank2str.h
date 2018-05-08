/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "fcs_conf.h"
#include "rinutils.h"

#ifdef CARD_DEBUG_PRES
#define CARD_ZERO() "*"
#else
#define CARD_ZERO() " "
#endif

#define GEN_CARD_MAP(t_card)                                                   \
    {                                                                          \
        CARD_ZERO()                                                            \
        , "A", "2", "3", "4", "5", "6", "7", "8", "9", t_card, "J", "Q", "K"   \
    }

static inline void rank2str(
    const int rank_idx, char *const str PASS_T(const bool display_T))
{
#ifdef FC_SOLVE_IMPLICIT_T_RANK
    static const char cards_char_map[15] = (CARD_ZERO() "A23456789TJQK");
    str[0] = cards_char_map[rank_idx];
    str[1] = '\0';
#else
    static const char card_map_3_10[14][4] = GEN_CARD_MAP("10");
    static const char card_map_3_T[14][4] = GEN_CARD_MAP("T");
    strcpy(str, (display_T ? card_map_3_T : card_map_3_10)[rank_idx]);
#endif
}
