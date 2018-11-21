/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// card.c - functions to convert cards and card components to and from
// their user representation.
#include "state.h"
#include "rank2str.h"

// Converts a suit to its user representation.
static inline void suit2str(const int suit, char *const str)
{
    str[0] = "HCDS"[suit];
    str[1] = '\0';
}

// Convert an entire card to its user representation.
void fc_solve_card_stringify(
    const fcs_card card, char *const str PASS_T(const bool t))
{
    rank2str(fcs_card_rank(card), str PASS_T(t));
    suit2str(fcs_card_suit(card), strchr(str, '\0'));
}
