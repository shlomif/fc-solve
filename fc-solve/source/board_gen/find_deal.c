// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
#include <rinutils/longlong.h>
#include "gen_ms_boards__find_deal.h"

long long DLLEXPORT __attribute__((pure))
fc_solve_find_deal_in_range(const unsigned long long start,
    const unsigned long long end, const uint_fast32_t *const ints)
{
    for (unsigned long long deal_idx = (microsoft_rand)start; deal_idx <= end;
         ++deal_idx)
    {
        microsoft_rand seedx = microsoft_rand__calc_init_seedx(deal_idx);
        const uint_fast32_t *ptr = ints;
        for (uint_fast32_t n = 4 * 13; n > 1; --n, ++ptr)
        {
            if (microsoft_rand__game_num_rand(&seedx, deal_idx) % n != *ptr)
            {
                goto next_deal;
            }
        }
        return (long long)deal_idx;
    next_deal:;
    }
    return -1;
}

#define NUM_CARDS ((4 * 13) - 1)
typedef uint_fast32_t fcs_find_deal__int;
typedef struct
{
    fcs_find_deal__int ints[NUM_CARDS];
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
        char *no_use;
        ret->ints[i] =
            (fcs_find_deal__int)strtoul(str + WIDTH * i, &no_use, 10);
    }
}

extern DLLEXPORT const char *fc_solve_user__find_deal__run(
    void *obj_ptr, const char *const start, const char *const end)
{
    find_deal *const obj = obj_ptr;
    sprintf(obj->ret, RIN_LL_FMT,
        fc_solve_find_deal_in_range((unsigned long long)atoll(start),
            (unsigned long long)atoll(end), obj->ints));
    return obj->ret;
}
