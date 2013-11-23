/* Copyright (c) 2012 Shlomi Fish
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
 * dbm_solver_head.h - contains the common part (headers/etc.) from the
 * beginning of depth_dbm_solver.c and dbm_solver.c .
 *
 */

#ifndef FC_SOLVE__DBM_SOLVER_HEAD_H
#define FC_SOLVE__DBM_SOLVER_HEAD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <limits.h>

#if 0
#define DEBUG_FOO
#endif

/*
 * Define FCS_DBM_SINGLE_THREAD to have single thread-per-instance traversal.
 */
#if 1
#define FCS_DBM_SINGLE_THREAD 1
#endif


#include "config.h"

#undef FCS_RCS_STATES

#include "bool.h"
#include "inline.h"
#include "portable_time.h"
#include "lock.h"

#include "dbm_calc_derived.h"
#include "delta_states_any.h"

#include "dbm_common.h"
#include "dbm_solver.h"
#include "dbm_cache.h"

#define FCS_DBM_USE_OFFLOADING_QUEUE

#include "offloading_queue.h"



#ifdef FCS_DBM_USE_OFFLOADING_QUEUE

#if (!defined(FCS_DBM_WITHOUT_CACHES))
#error FCS_DBM_USE_OFFLOADING_QUEUE requires FCS_DBM_WITHOUT_CACHES
#endif

#if (defined(FCS_DBM_CACHE_ONLY))
#error FCS_DBM_USE_OFFLOADING_QUEUE is not compatible with FCS_DBM_CACHE_ONLY
#endif

#endif

#include "dbm_trace.h"

#ifdef DEBUG_FOO
fc_solve_delta_stater_t * global_delta_stater;
#endif

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE__DBM_SOLVER_HEAD_H */
