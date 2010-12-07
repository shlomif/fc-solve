/* Copyright (c) 2010 Shlomi Fish
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
 * fcs_cl.h - the Freecell Solver command line arguments-like parsing routines.
 * Useful for more easily configuring a Freecell Solver instance.
 */
#ifndef FC_SOLVE__PORTABLE_TIME_H
#define FC_SOLVE__PORTABLE_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

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

#define FCS_GET_TIME(pt) { gettimeofday(&((pt).tv), &((pt).tz)); }
#define FCS_TIME_GET_SEC(pt) ((pt).tv.tv_sec)
#define FCS_TIME_GET_USEC(pt) ((pt).tv.tv_usec)
#else

typedef struct
{
    struct _timeb tb;
} fcs_portable_time_t;

#define FCS_GET_TIME(pt) { _ftime(&((pt).tb)); }
#define FCS_TIME_GET_SEC(pt) ((pt).tb.time)
#define FCS_TIME_GET_USEC(pt) ((long)(((pt).tb.millitm) * 1000))
#endif

#ifdef __cplusplus
};
#endif

#define FCS_PRINT_INTRACTABLE_BOARD(mytime, board_num) \
    FCS_GET_TIME(mytime); \
    printf("Intractable Board No. %i at %li.%.6li\n", \
        board_num, \
        FCS_TIME_GET_SEC(mytime), \
        FCS_TIME_GET_USEC(mytime) \
    )

#define FCS_PRINT_UNSOLVED_BOARD(mytime, board_num) \
    FCS_GET_TIME(mytime); \
    printf("Unsolved Board No. %i at %li.%.6li\n", \
            board_num, \
            FCS_TIME_GET_SEC(mytime), \
            FCS_TIME_GET_USEC(mytime) \
    )

#define FCS_PRINT_STARTED_AT(mytime) \
    FCS_GET_TIME(mytime); \
    printf("Started at %li.%.6li\n", \
            FCS_TIME_GET_SEC(mytime), \
            FCS_TIME_GET_USEC(mytime) \
    )

#define FCS_PRINT_REACHED_BOARD(mytime, board_num, total_num_iters) \
    FCS_GET_TIME(mytime); \
    printf( \
    ( \
     "Reached Board No. %i at %li.%.6li (total_num_iters=" \
     FCS_INT64_FORMAT ")\n" \
    ), \
    board_num, \
    FCS_TIME_GET_SEC(mytime), \
    FCS_TIME_GET_USEC(mytime), \
    total_num_iters \
    );



#endif /* #ifndef FC_SOLVE__PORTABLE_TIME_H */
