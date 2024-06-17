#pragma once
#include "gen_ms_boards__rand.h"
extern long long DLLEXPORT fc_solve_find_deal_in_range(
    const unsigned long long start, const unsigned long long end,
    const uint_fast32_t *const ints);
extern DLLEXPORT void *fc_solve_user__find_deal__alloc(void);
extern void DLLEXPORT fc_solve_user__find_deal__free(void *);
extern void DLLEXPORT fc_solve_user__find_deal__fill(void *, const char *);
extern DLLEXPORT void *fc_solve_user__find_deal__get_singleton(void);
extern DLLEXPORT const char *fc_solve_user__find_deal__run(
    void *, const char *, const char *);
