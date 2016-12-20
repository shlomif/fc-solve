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
static GCC_INLINE void fcs_lock_init(fcs_lock_t *const lock) {}
#define FCS_DESTROY_LOCK(lock)                                                 \
    {                                                                          \
    }

#elif defined(FCS_DBM_USE_RWLOCK)

typedef pthread_rwlock_fcfs_t *fcs_lock_t;
#define FCS_LOCK(lock) pthread_rwlock_fcfs_gain_write(lock)
#define FCS_UNLOCK(lock) pthread_rwlock_fcfs_release(lock)
static GCC_INLINE void fcs_lock_init(fcs_lock_t *const lock)
{
    *lock = pthread_rwlock_fcfs_alloc();
}
#define FCS_DESTROY_LOCK(lock) pthread_rwlock_fcfs_destroy(lock)

#else

static const pthread_mutex_t initial_mutex_constant = PTHREAD_MUTEX_INITIALIZER;
static const pthread_cond_t initial_cond_constant = PTHREAD_COND_INITIALIZER;

typedef pthread_mutex_t fcs_lock_t;
typedef pthread_cond_t fcs_condvar_t;
#define FCS_LOCK(lock) pthread_mutex_lock(&(lock))
static GCC_INLINE void fcs_lock_init(fcs_lock_t *const lock)
{
    *lock = initial_mutex_constant;
}
#define FCS_UNLOCK(lock) pthread_mutex_unlock(&(lock))
#define FCS_DESTROY_LOCK(lock)                                                 \
    {                                                                          \
    }

static GCC_INLINE void fcs_condvar_init(fcs_condvar_t *const cond)
{
    *cond = initial_cond_constant;
    pthread_cond_init(cond, NULL);
}

static GCC_INLINE void fcs_condvar_destroy(fcs_condvar_t *const cond)
{
    pthread_cond_destroy(cond);
}

static GCC_INLINE void fcs_condvar__wait_on(
    fcs_condvar_t *const cond, fcs_lock_t *const mutex)
{
    pthread_cond_wait(cond, mutex);
}

static GCC_INLINE void fcs_condvar_signal(fcs_condvar_t *const cond)
{
    pthread_cond_signal(cond);
}

static GCC_INLINE void fcs_condvar_broadcast(fcs_condvar_t *const cond)
{
    pthread_cond_broadcast(cond);
}
#endif

#ifdef __cplusplus
};
#endif
