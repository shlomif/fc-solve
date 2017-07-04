/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// trace_mem.h - trace memory.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rinutils.h"

#ifdef FCS_TRACE_MEM
static int rss_found = 0;
static long long max_rss_delta_deal = -1, prev_rss = -1, max_rss_delta = -1;

static inline void trace_mem(const long long board_num)
{
    long long int rss;
    unsigned long long unused_unsigned;
    char stat_fn[1024], unused_str[1024];

    snprintf(stat_fn, sizeof(stat_fn), "/proc/%ld/stat", (long)(getpid()));

    // This was taken from: http://www.brokestream.com/procstat.html
    FILE *const stat = fopen(stat_fn, "r");
#define readone(unused) (fscanf(stat, "%lld ", &rss))
#define readstr(unused) (fscanf(stat, "%1000s ", unused_str))
#define readchar(unused) (fscanf(stat, "%c ", unused_str))
#define readunsigned(unused) (fscanf(stat, "%llu ", &unused_unsigned))
    readone(&pid);
    readstr(tcomm);
    readchar(&state);
    readone(&ppid);
    readone(&pgid);
    readone(&sid);
    readone(&tty_nr);
    readone(&tty_pgrp);
    readone(&flags);
    readone(&min_flt);
    readone(&cmin_flt);
    readone(&maj_flt);
    readone(&cmaj_flt);
    readone(&utime);
    readone(&stimev);
    readone(&cutime);
    readone(&cstime);
    readone(&priority);
    readone(&nicev);
    readone(&num_threads);
    readone(&it_real_value);
    readunsigned(&start_time);
    readone(&vsize);
    readone(&rss);
#undef readone
#undef readunsigned
#undef readchar
#undef readstr

    fclose(stat);

    const long long rss_delta = rss - prev_rss;
    if (rss_found == 1)
    {
        max_rss_delta = rss_delta;
        max_rss_delta_deal = board_num;
    }
    else if (rss_found == 2)
    {
        if (rss_delta > max_rss_delta)
        {
            max_rss_delta = rss_delta;
            max_rss_delta_deal = board_num;
        }
        printf("Max RSS delta %lld encountered at deal %lld\n", max_rss_delta,
            max_rss_delta_deal);
    }
    prev_rss = rss;
    if (rss_found < 2)
    {
        rss_found++;
    }
}
#else

static inline void trace_mem(const long long board_num GCC_UNUSED) {}

#endif
#ifdef __cplusplus
}
#endif
