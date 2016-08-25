#pragma once

typedef struct
{
    fcs_dbm_solver_thread_t thread;
    thread_arg_t arg;
    pthread_t id;
} main_thread_item_t;

static GCC_INLINE void dbm__spawn_threads(
    fcs_dbm_solver_instance_t *const instance,
    const size_t num_threads,
    main_thread_item_t *const threads)
{
#ifdef T
    FILE *const out_fh = instance->out_fh;
#endif
    TRACE1("Running threads for curr_depth=%d\n", instance->curr_depth);
    for (size_t i = 0; i < num_threads; i++)
    {
        if (pthread_create(&(threads[i].id), NULL,
                instance_run_solver_thread, &(threads[i].arg)))
        {
            fprintf(stderr,
                "Worker Thread No. %zd Initialization failed!\n", i);
            exit(-1);
        }
    }

    for (size_t i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i].id, NULL);
    }
    TRACE1("Finished running threads for curr_depth=%d\n",
        instance->curr_depth);
}
