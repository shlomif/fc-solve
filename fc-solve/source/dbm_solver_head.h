/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
// dbm_solver_head.h - contains the common part (headers/etc.) from the
// beginning of depth_dbm_solver.c and dbm_solver.c .
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

// Define FCS_DBM_SINGLE_THREAD to have single thread-per-instance traversal.
#if 0
#define FCS_DBM_SINGLE_THREAD 1
#endif

#include "fcs_conf.h"
#undef FCS_RCS_STATES

#include "lock.h"
#include "dbm_calc_derived.h"
#include "delta_states_any.h"
#include "dbm_common.h"
#include "dbm_solver.h"
#include "dbm_cache.h"

#ifdef FCS_DBM_WITHOUT_CACHES
#define FCS_DBM_USE_OFFLOADING_QUEUE
#endif

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
fcs_delta_stater global_delta_stater;
#endif

#ifdef __cplusplus
}
#endif
