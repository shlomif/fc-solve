/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#define FCS_GEN_BOARDS_WITH_EXTERNAL_API

#include "gen_ms_boards__rand.h"

extern long long DLLEXPORT fc_solve_find_deal_in_range(
    const long long start, const long long end, const uint_fast32_t *const ints)
{
    for (long long deal_idx = start; deal_idx <= end; ++deal_idx)
    {
        long long seedx = microsoft_rand__calc_init_seedx(deal_idx);
        const uint_fast32_t *ptr = ints;
        for (uint_fast32_t n = 4 * 13; n > 1; --n, ++ptr)
        {
            if (microsoft_rand__game_num_rand(&seedx, deal_idx) % n != *ptr)
            {
                goto next_deal;
            }
        }
        return deal_idx;
    next_deal:;
    }
    return -1;
}
