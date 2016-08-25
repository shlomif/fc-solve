#pragma once
#define USER_STATE_SIZE 2000
typedef struct
{
    fcs_dbm_solver_thread_t thread;
    thread_arg_t arg;
    pthread_t id;
} main_thread_item_t;

static GCC_INLINE main_thread_item_t *dbm__calc_threads(
    fcs_dbm_solver_instance_t *const instance,
    fcs_state_keyval_pair_t *const init_state, const size_t num_threads,
    void (*init_thread_cb)(fcs_dbm_solver_thread_t *))
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
        init_thread_cb(threads[i].arg.thread = &(threads[i].thread));
    }
    return threads;
}
