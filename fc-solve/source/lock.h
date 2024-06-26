// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2012 Shlomi Fish
// lock.h - macros/functions for locking.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Define FCS_DBM_USE_RWLOCK to use the pthread FCFS RWLock which appears
// to improve CPU utilisations of the various worker threads and possibly
// overall performance.
// #define FCS_DBM_USE_RWLOCK 1

#include "rinutils/rinutils.h"

#ifdef FCS_DBM_SINGLE_THREAD

typedef bool fcs_lock;
typedef bool fcs_condvar;
static inline void fcs_lock_lock(GCC_UNUSED fcs_lock *const lock) {}
static inline void fcs_lock_unlock(GCC_UNUSED fcs_lock *const lock) {}
static inline void fcs_lock_init(GCC_UNUSED fcs_lock *const lock) {}
static inline void fcs_lock_destroy(GCC_UNUSED fcs_lock *const lock) {}
static inline void fcs_condvar_init(GCC_UNUSED fcs_condvar *const cond) {}
static inline void fcs_condvar_destroy(GCC_UNUSED fcs_condvar *const cond) {}
static inline void fcs_condvar__wait_on(
    GCC_UNUSED fcs_condvar *const cond, GCC_UNUSED fcs_lock *const lock)
{
}
static inline void fcs_condvar_signal(GCC_UNUSED fcs_condvar *const cond) {}
static inline void fcs_condvar_broadcast(GCC_UNUSED fcs_condvar *const cond) {}
#else
#include <pthread.h>
#if defined(FCS_DBM_USE_RWLOCK)

#ifdef FCS_DBM_USE_RWLOCK
#include <pthread/rwlock_fcfs.h>
#endif

typedef pthread_rwlock_fcfs_t *fcs_lock;
static inline void fcs_lock_lock(fcs_lock *const lock)
{
    pthread_rwlock_fcfs_gain_write(*lock);
}
static inline void fcs_lock_unlock(fcs_lock *const lock)
{
    pthread_rwlock_fcfs_release(*lock);
}
static inline void fcs_lock_init(fcs_lock *const lock)
{
    *lock = pthread_rwlock_fcfs_alloc();
}
static inline void fcs_lock_destroy(fcs_lock *const lock)
{
    pthread_rwlock_fcfs_destroy(*lock);
}

#else

static const pthread_mutex_t initial_mutex_constant = PTHREAD_MUTEX_INITIALIZER;
static const pthread_cond_t initial_cond_constant = PTHREAD_COND_INITIALIZER;
static const int FCS_LOCK_FAILURE_EXIT_CODE = 2;

typedef pthread_mutex_t fcs_lock;
typedef pthread_cond_t fcs_condvar;
static inline void fcs_lock_lock(fcs_lock *const lock)
{
    const_AUTO(ret, pthread_mutex_lock(lock));
    // See: https://github.com/shlomif/fc-solve/issues/85
    if (unlikely(ret))
    {
        exit(FCS_LOCK_FAILURE_EXIT_CODE);
    }
}
static inline void fcs_lock_init(fcs_lock *const lock)
{
    *lock = initial_mutex_constant;
}
static inline void fcs_lock_unlock(fcs_lock *const lock)
{
    const_AUTO(ret, pthread_mutex_unlock(lock));
    if (unlikely(ret))
    {
        exit(FCS_LOCK_FAILURE_EXIT_CODE);
    }
}
static inline void fcs_lock_destroy(fcs_lock *const lock)
{
    pthread_mutex_destroy(lock);
}
static inline void fcs_condvar_init(fcs_condvar *const cond)
{
    *cond = initial_cond_constant;
    pthread_cond_init(cond, NULL);
}

static inline void fcs_condvar_destroy(fcs_condvar *const cond)
{
    pthread_cond_destroy(cond);
}

static inline void fcs_condvar__wait_on(
    fcs_condvar *const cond, fcs_lock *const lock)
{
    pthread_cond_wait(cond, lock);
}

static inline void fcs_condvar_signal(fcs_condvar *const cond)
{
    pthread_cond_signal(cond);
}

static inline void fcs_condvar_broadcast(fcs_condvar *const cond)
{
    pthread_cond_broadcast(cond);
}
#endif
#endif

#ifdef __cplusplus
};
#endif
