#pragma once

#include "dbm_procs_inner.h"

static GCC_INLINE void dbm__spawn_threads(
    fcs_dbm_solver_instance_t *const instance, const size_t num_threads,
    main_thread_item_t *const threads)
{
#ifdef T
    FILE *const out_fh = instance->out_fh;
#endif
    TRACE("Running threads for curr_depth=%d\n", instance->curr_depth);
    for (size_t i = 0; i < num_threads; i++)
    {
        if (pthread_create(&(threads[i].id), NULL, instance_run_solver_thread,
                &(threads[i].arg)))
        {
            fprintf(
                stderr, "Worker Thread No. %zd Initialization failed!\n", i);
            exit(-1);
        }
    }

    for (size_t i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i].id, NULL);
    }
    TRACE("Finished running threads for curr_depth=%d\n", instance->curr_depth);
}

static void init_thread(fcs_dbm_solver_thread_t *const thread)
{
    fc_solve_meta_compact_allocator_init(&(thread->thread_meta_alloc));
}

static void free_thread(fcs_dbm_solver_thread_t *const thread)
{
    fc_solve_meta_compact_allocator_finish(&(thread->thread_meta_alloc));
}

/* Returns if the process should terminate. */
static fcs_bool_t handle_and_destroy_instance_solution(
    fcs_dbm_solver_instance_t *const instance,
    fc_solve_delta_stater_t *const delta)
{
    fcs_bool_t ret = FALSE;
    FILE *const out_fh = instance->out_fh;

    TRACE("%s\n", "handle_and_destroy_instance_solution start");
    instance_print_stats(instance, out_fh);

    if (instance->queue_solution_was_found)
    {
        trace_solution(instance, out_fh, delta);
        ret = TRUE;
    }
    else if (instance->should_terminate != DONT_TERMINATE)
    {
        fprintf(out_fh, "%s\n", "Intractable.");
        fflush(out_fh);
        if (instance->should_terminate == MAX_ITERS_TERMINATE)
        {
            fprintf(out_fh, "Reached Max-or-more iterations of %ld.\n",
                instance->max_count_num_processed);
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
