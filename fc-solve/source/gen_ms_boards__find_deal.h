#pragma once
#include "gen_ms_boards__rand.h"
extern long long DLLEXPORT fc_solve_find_deal_in_range(const long long start,
    const long long end, const uint_fast32_t *const ints);
extern DLLEXPORT void *fc_solve_user__find_deal__alloc(void);
extern void DLLEXPORT fc_solve_user__find_deal__free(void *);
extern void DLLEXPORT fc_solve_user__find_deal__fill(void *, const char *);
extern DLLEXPORT const char *fc_solve_user__find_deal__run(
    void *, const char *, const char *);
