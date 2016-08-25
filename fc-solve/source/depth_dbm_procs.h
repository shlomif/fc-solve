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

static void trace_solution(fcs_dbm_solver_instance_t *const instance,
    FILE *const out_fh, fc_solve_delta_stater_t *const delta)
{
    fcs_encoded_state_buffer_t *trace;
    int trace_num;
    fcs_state_keyval_pair_t state;
    unsigned char move = '\0';
    char move_buffer[500];
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    fcs_state_locs_struct_t locs;
    const_AUTO(local_variant, instance->variant);

    fprintf(out_fh, "%s\n", "Success!");
    fflush(out_fh);
    /* Now trace the solution */

    calc_trace(instance->queue_solution_ptr, &trace, &trace_num);

    fc_solve_init_locs(&locs);

    for (int i = trace_num - 1; i >= 0; i--)
    {
        fc_solve_delta_stater_decode_into_state(
            delta, trace[i].s, &state, indirect_stacks_buffer);
        if (i > 0)
        {
            move = get_move_from_parent_to_child(
                instance, delta, trace[i], trace[i - 1]);
        }

        char state_as_str[1000];
        fc_solve_state_as_string(state_as_str, &(state.s),
            &locs PASS_FREECELLS(FREECELLS_NUM) PASS_STACKS(STACKS_NUM)
                PASS_DECKS(DECKS_NUM) FC_SOLVE__PASS_PARSABLE(TRUE),
            FALSE PASS_T(TRUE));

        fprintf(out_fh, "--------\n%s\n==\n%s\n", state_as_str,
            (i > 0) ? move_to_string(move, move_buffer) : "END");
        fflush(out_fh);
    }
    free(trace);
}

/* Returns if the process should terminate. */
static fcs_bool_t handle_and_destroy_instance_solution(
    fcs_dbm_solver_instance_t *instance, FILE *out_fh,
    fc_solve_delta_stater_t *delta)
{
    fcs_bool_t ret = FALSE;

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
