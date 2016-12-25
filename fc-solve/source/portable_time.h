/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2010 Shlomi Fish
 */
/*
 * portable_time.h - the Freecell Solver (mostly) portable time handling
 * routines. Works on Win32 and UNIX-like systems.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <sys/types.h>
#include <sys/timeb.h>
#endif

#ifndef WIN32

typedef struct
{
    struct timeval tv;
    struct timezone tz;
} fcs_portable_time_t;

#define FCS_GET_TIME(pt)                                                       \
    {                                                                          \
        gettimeofday(&((pt).tv), &((pt).tz));                                  \
    }
#define FCS_TIME_GET_SEC(pt) ((long long)((pt).tv.tv_sec))
#define FCS_TIME_GET_USEC(pt) ((long long)((pt).tv.tv_usec))
#define FCS_LL_FMT "%lld"
#define FCS_LL6_FMT "%.6lld"
#else

typedef struct
{
    struct _timeb tb;
} fcs_portable_time_t;

#define FCS_GET_TIME(pt)                                                       \
    {                                                                          \
        _ftime(&((pt).tb));                                                    \
    }
#define FCS_TIME_GET_SEC(pt) ((long long)((pt).tb.time))
#define FCS_TIME_GET_USEC(pt) ((long long)(((pt).tb.millitm) * 1000))
#define FCS_LL_FMT "%I64d"
#define FCS_LL6_FMT "%.6I64d"
#endif

#ifdef __cplusplus
};
#endif

#define FCS_T_FMT FCS_LL_FMT "." FCS_LL6_FMT
#define FCS_B_AT_FMT "Board No. " FCS_LL_FMT " at " FCS_T_FMT
static inline void fc_solve_print_intractable(const long long board_num)
{
    fcs_portable_time_t mytime;
    FCS_GET_TIME(mytime);
    printf("Intractable " FCS_B_AT_FMT "\n", board_num,
        FCS_TIME_GET_SEC(mytime), FCS_TIME_GET_USEC(mytime));
}

static inline void fc_solve_print_unsolved(const long long board_num)
{
    fcs_portable_time_t mytime;
    FCS_GET_TIME(mytime);
    printf("Unsolved " FCS_B_AT_FMT "\n", board_num, FCS_TIME_GET_SEC(mytime),
        FCS_TIME_GET_USEC(mytime));
}

static inline void fc_solve_print_started_at(void)
{
    fcs_portable_time_t mytime;
    FCS_GET_TIME(mytime);
    printf("Started at " FCS_T_FMT "\n", FCS_TIME_GET_SEC(mytime),
        FCS_TIME_GET_USEC(mytime));
}

static inline void fc_solve_print_reached(
    const long long board_num, const long long total_num_iters)
{
    fcs_portable_time_t mytime;
    FCS_GET_TIME(mytime);
    printf("Reached " FCS_B_AT_FMT " (total_num_iters=" FCS_LL_FMT ")\n",
        board_num, FCS_TIME_GET_SEC(mytime), FCS_TIME_GET_USEC(mytime),
        total_num_iters);
}

static inline void fc_solve_print_finished(const long long total_num_iters)
{
    fcs_portable_time_t mytime;
    FCS_GET_TIME(mytime);
    printf(("Finished at " FCS_T_FMT " (total_num_iters=" FCS_LL_FMT ")\n"),
        FCS_TIME_GET_SEC(mytime), FCS_TIME_GET_USEC(mytime), total_num_iters);
}
