#pragma once

typedef struct
{
    fcs_dbm_solver_thread_t thread;
    thread_arg_t arg;
    pthread_t id;
} main_thread_item_t;

static GCC_INLINE main_thread_item_t *dbm__calc_threads(
    fcs_dbm_solver_instance_t *const instance,
    fcs_state_keyval_pair_t *const init_state, const size_t num_threads)
{
#ifdef T
    FILE *const out_fh = instance->out_fh;
#endif
#ifndef FCS_FREECELL_ONLY
    const_AUTO(local_variant, instance->variant);
#endif
    main_thread_item_t *const threads = SMALLOC(threads, num_threads);
    TRACE0("instance_run_all_threads start");
#ifdef DEBUG_FOO
    fc_solve_delta_stater_init(
        &global_delta_stater, &(init_state->s), STACKS_NUM, FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
        ,
        FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
        );
#endif
    for (size_t i = 0; i < num_threads; i++)
    {
        threads[i].thread.instance = instance;
        fc_solve_delta_stater_init(&(threads[i].thread.delta_stater),
            &(init_state->s), STACKS_NUM, FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
            ,
            FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
            );
        fc_solve_meta_compact_allocator_init(
            &(threads[i].thread.thread_meta_alloc));
        threads[i].arg.thread = &(threads[i].thread);
    }
    return threads;
}

static GCC_INLINE void dbm__spawn_threads(
    fcs_dbm_solver_instance_t *const instance, const size_t num_threads,
    main_thread_item_t *const threads)
{
#ifdef T
    FILE *const out_fh = instance->out_fh;
#endif
    TRACE1("Running threads for curr_depth=%d\n", instance->curr_depth);
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
    TRACE1(
        "Finished running threads for curr_depth=%d\n", instance->curr_depth);
}
