/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
/*
 * lock.h - FCS_LOCK()/etc. macros for locking.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>

/*
 * Define FCS_DBM_USE_RWLOCK to use the pthread FCFS RWLock which appears
 * to improve CPU utilisations of the various worker threads and possibly
 * overall performance.
 * #define FCS_DBM_USE_RWLOCK 1 */

#ifdef FCS_DBM_USE_RWLOCK
#include <pthread/rwlock_fcfs.h>
#endif

#include "rinutils.h"

#ifdef FCS_DBM_SINGLE_THREAD

typedef fcs_bool_t fcs_lock_t;
#define FCS_LOCK(lock)                                                         \
    {                                                                          \
    }
#define FCS_UNLOCK(lock)                                                       \
    {                                                                          \
    }
#define FCS_INIT_LOCK(lock)                                                    \
    {                                                                          \
    }
#define FCS_DESTROY_LOCK(lock)                                                 \
    {                                                                          \
    }

#elif defined(FCS_DBM_USE_RWLOCK)

typedef pthread_rwlock_fcfs_t *fcs_lock_t;
#define FCS_LOCK(lock) pthread_rwlock_fcfs_gain_write(lock)
#define FCS_UNLOCK(lock) pthread_rwlock_fcfs_release(lock)
#define FCS_INIT_LOCK(lock) ((lock) = pthread_rwlock_fcfs_alloc())
#define FCS_DESTROY_LOCK(lock) pthread_rwlock_fcfs_destroy(lock)

#else

static const pthread_mutex_t initial_mutex_constant = PTHREAD_MUTEX_INITIALIZER;

typedef pthread_mutex_t fcs_lock_t;
#define FCS_LOCK(lock) pthread_mutex_lock(&(lock))
#define FCS_UNLOCK(lock) pthread_mutex_unlock(&(lock))
#define FCS_INIT_LOCK(lock) ((lock) = initial_mutex_constant)
#define FCS_DESTROY_LOCK(lock)                                                 \
    {                                                                          \
    }

#endif

#ifdef __cplusplus
};
#endif
