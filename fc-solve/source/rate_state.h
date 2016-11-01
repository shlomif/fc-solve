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
 * rate_state.h - for rating states.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define FCS_BEFS_SEQS_OVER_RENEGADE_CARDS_EXPONENT 1.3

typedef double fc_solve_seq_cards_power_type_t;
extern const fc_solve_seq_cards_power_type_t
    fc_solve_seqs_over_cards_lookup[2 * 13 * 4 + 1];

#if 0
#define FCS_SEQS_OVER_RENEGADE_POWER(n)                                        \
    pow(n, FCS_BEFS_SEQS_OVER_RENEGADE_CARDS_EXPONENT)
#else
#define FCS_SEQS_OVER_RENEGADE_POWER(n) fc_solve_seqs_over_cards_lookup[(n)]
#endif

#ifdef __cplusplus
};
#endif
