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
 * check_limits.h - header file for the check_if_limits_exceeded() macro.
 */

#ifndef FC_SOLVE__CHECK_LIMITS_H
#define FC_SOLVE__CHECK_LIMITS_H

#ifdef __cplusplus
extern "C" {
#endif

#define check_num_states_in_collection(instance) \
    ((instance)->active_num_states_in_collection >=       \
            (instance)->effective_trim_states_in_collection_from)


/*
 * This macro checks if we need to terminate from running this soft
 * thread and return to the soft thread manager with an
 * FCS_STATE_SUSPEND_PROCESS
 * */
#define check_if_limits_exceeded()                                    \
    (                                                                 \
        ((*instance_num_times_ptr) >= instance->effective_max_num_times)    \
            ||                                                        \
        (hard_thread->num_times >= hard_thread->ht_max_num_times)     \
            ||                                                        \
        (hard_thread->num_times >= hard_thread->max_num_times)        \
            ||                                                        \
        (instance->num_states_in_collection >=   \
            instance->effective_max_num_states_in_collection) \
    )

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FC_SOLVE__CHECK_LIMITS_H */
