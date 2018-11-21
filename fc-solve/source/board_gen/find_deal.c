/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#include "gen_ms_boards__find_deal.h"

long long DLLEXPORT __attribute__((pure)) fc_solve_find_deal_in_range(
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

#define NUM_CARDS ((4 * 13) - 1)
typedef struct
{
    uint_fast32_t ints[NUM_CARDS];
    char ret[128];
} find_deal;

DLLEXPORT void *fc_solve_user__find_deal__alloc(void)
{
    find_deal *const ret = SMALLOC1(ret);
    return ret;
}

void DLLEXPORT fc_solve_user__find_deal__free(void *obj) { free(obj); }

void DLLEXPORT fc_solve_user__find_deal__fill(
    void *const obj_ptr, const char *const str)
{
    const size_t WIDTH = 10;
    find_deal *const ret = (find_deal *)obj_ptr;

    if (strlen(str) != WIDTH * NUM_CARDS)
    {
        return;
    }
    for (size_t i = 0; i < NUM_CARDS; ++i)
    {
        ret->ints[i] = atol(str + WIDTH * i);
    }
}

extern DLLEXPORT const char *fc_solve_user__find_deal__run(
    void *obj_ptr, const char *const start, const char *const end)
{
    find_deal *obj = obj_ptr;
    sprintf(obj->ret, "%lld",
        fc_solve_find_deal_in_range(atoll(start), atoll(end), obj->ints));
    return obj->ret;
}
