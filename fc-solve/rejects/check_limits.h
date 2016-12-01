/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * check_limits.h - header file for the check_if_limits_exceeded() macro.
 */

#ifndef FC_SOLVE__CHECK_LIMITS_H
#define FC_SOLVE__CHECK_LIMITS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "inline.h"
#include "bool.h"

static GCC_INLINE const fcs_bool_t check_num_states_in_collection(
    const fc_solve_instance_t * const instance
    )
{
    return (instance->active_num_states_in_collection >=
            instance->effective_trim_states_in_collection_from
            );
}

#ifdef FCS_SINGLE_HARD_THREAD
#define check_if_limits_exceeded__num() \
        ((*instance_num_checked_states_ptr) == hard_thread_max_num_checked_states)
#else
#define check_if_limits_exceeded__num() \
        ((*hard_thread_num_checked_states_ptr) == hard_thread_max_num_checked_states)
#endif

/*
 * This macro checks if we need to terminate from running this soft
 * thread and return to the soft thread manager with an
 * FCS_STATE_SUSPEND_PROCESS
 * */
#define check_if_limits_exceeded()                                    \
    (   \
        check_if_limits_exceeded__num() \
            || \
        (instance->num_states_in_collection >=   \
            effective_max_num_states_in_collection) \
    )

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FC_SOLVE__CHECK_LIMITS_H */
