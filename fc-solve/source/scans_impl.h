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
 * scans_impl.h - code implementing the various scans as inline functions.
 *
 */

#ifndef FC_SOLVE__SCANS_IMPL_H
#define FC_SOLVE__SCANS_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "config.h"

#include "check_limits.h"
#include "inline.h"
#include "likely.h"
#include "bool.h"
#include "min_and_max.h"

/*
 * fc_solve_patsolve_do_solve() is the event loop of the
 * Patsolve scan.
 */
static GCC_INLINE int fc_solve_patsolve_do_solve(
    fc_solve_soft_thread_t * const soft_thread
    )
{
    typeof(soft_thread->hard_thread) hard_thread = soft_thread->hard_thread;
    typeof(soft_thread->pats_scan) pats_scan = soft_thread->pats_scan;

    typeof(hard_thread->num_checked_states) delta =
        hard_thread->max_num_checked_states - hard_thread->num_checked_states;

    typeof (pats_scan->num_checked_states) start_from = pats_scan->num_checked_states;

    pats_scan->max_num_checked_states = start_from + delta;

    pats_scan->status = FCS_PATS__NOSOL;

    fc_solve_pats__do_it(pats_scan);

    typeof(start_from) after_scan_delta = pats_scan->num_checked_states - start_from;
    hard_thread->num_checked_states += after_scan_delta;
    hard_thread->instance->num_checked_states += after_scan_delta;

    const typeof(pats_scan->status) status = pats_scan->status;
    return
    (
        (status == FCS_PATS__WIN) ? FCS_STATE_WAS_SOLVED
        : (status == FCS_PATS__NOSOL) ? FCS_STATE_IS_NOT_SOLVEABLE
        : FCS_STATE_SUSPEND_PROCESS
    );
}

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__SCANS_IMPL_H */
