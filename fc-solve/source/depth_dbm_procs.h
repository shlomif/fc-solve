// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
#pragma once

#include "dbm_procs_inner.h"

static inline void dbm__spawn_threads(dbm_solver_instance *const instance,
    const size_t num_threads, main_thread_item *const threads)
{
    FILE *const out_fh = instance->common.out_fh;
    TRACE("Running threads for curr_depth=%lu\n",
        (unsigned long)instance->curr_depth);
    if (num_threads == 1)
    {
        instance_run_solver_thread(&(threads[0].arg));
    }
#ifndef FCS_DBM_SINGLE_THREAD
    else
    {
        for (size_t i = 0; i < num_threads; i++)
        {
            if (pthread_create(&(threads[i].id), NULL,
                    instance_run_solver_thread, &(threads[i].arg)))
            {
                exit_error("Worker Thread No. %u Initialization failed!\n",
                    (unsigned)i);
            }
        }

        for (size_t i = 0; i < num_threads; i++)
        {
            pthread_join(threads[i].id, NULL);
        }
    }
#endif
    TRACE("Finished running threads for curr_depth=%lu\n",
        (unsigned long)instance->curr_depth);
}

static inline void init_thread(dbm_solver_thread *const thread)
{
    fc_solve_meta_compact_allocator_init(&(thread->thread_meta_alloc));
}

static inline void free_thread(dbm_solver_thread *const thread)
{
    fc_solve_meta_compact_allocator_finish(&(thread->thread_meta_alloc));
}

// Returns whether the process should terminate.
static bool handle_and_destroy_instance_solution(
    dbm_solver_instance *const instance,
    fcs_delta_stater *const delta GCC_UNUSED)
{
    FILE *const out_fh = instance->common.out_fh;
    bool ret = false;
    TRACE("%s\n", "handle_and_destroy_instance_solution start");
    instance_print_stats(instance);
    if (instance->common.queue_solution_was_found)
    {
#ifdef IN_DEPTH_DBM_SOLVER_dot_c
        fcs_dbm_store stores_by_depth[MAX_FCC_DEPTH];
        for (size_t i = 0; i < MAX_FCC_DEPTH; ++i)
        {
            /* code */
            stores_by_depth[i] = instance->colls_by_depth[i].cache_store.store;
        }
        fcs__parent_lookup__type parent_lookup;
        parent_lookup.count_stores = MAX_FCC_DEPTH;
        parent_lookup.stores = stores_by_depth;
        trace_solution(&parent_lookup, instance, out_fh, delta);
#endif
        ret = true;
    }
    else if (instance->common.should_terminate != DONT_TERMINATE)
    {
        fprintf(out_fh, "%s\n", "Intractable.");
        fflush(out_fh);
        if (instance->common.should_terminate == MAX_ITERS_TERMINATE)
        {
            fprintf(out_fh, "Reached Max-or-more iterations of %lu.\n",
                instance->common.max_count_num_processed);
        }
    }
    else
    {
        fprintf(out_fh, "%s\n", "Could not solve successfully.");
    }

    TRACE("%s\n", "handle_and_destroy_instance_solution end");

    instance_destroy(instance);

    return ret;
}
