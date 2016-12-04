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

#include "inline.h"

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
#define FCS_TIME_GET_SEC(pt) ((pt).tv.tv_sec)
#define FCS_TIME_GET_USEC(pt) ((pt).tv.tv_usec)
#else

typedef struct
{
    struct _timeb tb;
} fcs_portable_time_t;

#define FCS_GET_TIME(pt)                                                       \
    {                                                                          \
        _ftime(&((pt).tb));                                                    \
    }
#define FCS_TIME_GET_SEC(pt) ((long)((pt).tb.time))
#define FCS_TIME_GET_USEC(pt) ((long)(((pt).tb.millitm) * 1000))
#endif

#ifdef __cplusplus
};
#endif

#define FCS_PRINT_INTRACTABLE_BOARD(mytime, board_num)                         \
    FCS_GET_TIME(mytime);                                                      \
    printf("Intractable Board No. %lld at %li.%.6li\n", board_num,             \
        FCS_TIME_GET_SEC(mytime), FCS_TIME_GET_USEC(mytime))

#define FCS_PRINT_UNSOLVED_BOARD(mytime, board_num)                            \
    FCS_GET_TIME(mytime);                                                      \
    printf("Unsolved Board No. %lld at %li.%.6li\n", board_num,                \
        FCS_TIME_GET_SEC(mytime), FCS_TIME_GET_USEC(mytime))

static GCC_INLINE void fc_solve_print_started_at(void)
{
    fcs_portable_time_t mytime;
    FCS_GET_TIME(mytime);
    printf("Started at %li.%.6li\n", FCS_TIME_GET_SEC(mytime),
        FCS_TIME_GET_USEC(mytime));
}

#define FCS_PRINT_REACHED_BOARD(mytime, board_num, total_num_iters)            \
    FCS_GET_TIME(mytime);                                                      \
    printf(("Reached Board No. %lld at %li.%.6li "                             \
            "(total_num_iters=%lld)\n"),                                       \
        board_num, FCS_TIME_GET_SEC(mytime), FCS_TIME_GET_USEC(mytime),        \
        total_num_iters);

#define FCS_PRINT_FINISHED(mytime, total_num_iters)                            \
    FCS_GET_TIME(mytime);                                                      \
                                                                               \
    printf(("Finished at %li.%.6li (total_num_iters=%lld)\n"),                 \
        FCS_TIME_GET_SEC(mytime), FCS_TIME_GET_USEC(mytime), total_num_iters)
