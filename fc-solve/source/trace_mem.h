/* Copyright (c) 2000 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * trace_mem.h - trace memory.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rinutils.h"

#ifdef FCS_TRACE_MEM
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

static int rss_found = 0;
static long max_rss_delta_deal = -1;
static long long int prev_rss = -1, max_rss_delta = -1;

static GCC_INLINE void trace_mem(const int board_num)
{
    long long int rss;
    unsigned long long unused_unsigned;
    char stat_fn[1024], unused_str[1024];
    FILE *stat;

    snprintf(stat_fn, sizeof(stat_fn), "/proc/%ld/stat", (long)(getpid()));

    {
        /* This was taken from:
         *
         * http://www.brokestream.com/procstat.html
         * */
        stat = fopen(stat_fn, "r");
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
            printf("Max RSS delta %lld encountered at deal %ld\n",
                max_rss_delta, max_rss_delta_deal);
        }
        prev_rss = rss;
        if (rss_found < 2)
        {
            rss_found++;
        }
    }
}
#else

static GCC_INLINE void trace_mem(const int board_num GCC_UNUSED) {}

#endif
#ifdef __cplusplus
}
#endif
