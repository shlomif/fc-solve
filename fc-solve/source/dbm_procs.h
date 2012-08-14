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
 * dbm_procs.h - common procedures for dbm_solver.c and depth_dbm_solver.c.
 */
#ifndef FC_SOLVE__DBM_PROCS_H
#define FC_SOLVE__DBM_PROCS_H

#ifdef __cplusplus
extern "C"
{
#endif

static void instance_print_stats(
    fcs_dbm_solver_instance_t * instance,
    FILE * out_fh
    )
{
    fcs_portable_time_t mytime;
    FCS_GET_TIME(mytime);

    fprintf (out_fh, "Reached %ld ; States-in-collection: %ld ; Time: %li.%.6li\n",
             instance->count_num_processed,
             instance->num_states_in_collection,
             FCS_TIME_GET_SEC(mytime),
             FCS_TIME_GET_USEC(mytime)
            );
    fprintf (out_fh, ">>>Queue Stats: inserted=%ld items_in_queue=%ld extracted=%ld\n",
             instance->num_states_in_collection,
             instance->num_states_in_collection - instance->count_num_processed,
             instance->count_num_processed
            );
    fflush(out_fh);
}

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE__DBM_PROCS_H */
