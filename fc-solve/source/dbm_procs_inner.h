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
    TRACE("%s\n", "instance_run_all_threads start");
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

static GCC_INLINE void dbm__free_threads(
    fcs_dbm_solver_instance_t *const instance, const size_t num_threads,
    main_thread_item_t *const threads,
    void (*free_thread_cb)(fcs_dbm_solver_thread_t *))
{
#ifdef T
    FILE *const out_fh = instance->out_fh;
#endif
    for (size_t i = 0; i < num_threads; i++)
    {
        fc_solve_delta_stater_release(&(threads[i].thread.delta_stater));
        free_thread_cb(&(threads[i].thread));
    }
    free(threads);
#ifdef DEBUG_FOO
    fc_solve_delta_stater_release(&global_delta_stater);
#endif
    TRACE("%s\n", "instance_run_all_threads end");
}

static unsigned char get_move_from_parent_to_child(
    fcs_dbm_solver_instance_t *instance, fc_solve_delta_stater_t *delta,
    fcs_encoded_state_buffer_t parent, fcs_encoded_state_buffer_t child)
{
    unsigned char move_to_return;
    fcs_encoded_state_buffer_t got_child;
    fcs_state_keyval_pair_t parent_state;
    fcs_derived_state_t *derived_list, *derived_list_recycle_bin, *derived_iter;
    fcs_compact_allocator_t derived_list_allocator;
    fcs_meta_compact_allocator_t meta_alloc;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    const_AUTO(local_variant, instance->variant);

    fc_solve_meta_compact_allocator_init(&meta_alloc);
    fc_solve_compact_allocator_init(&(derived_list_allocator), &meta_alloc);
    fc_solve_delta_stater_decode_into_state(
        delta, parent.s, &parent_state, indirect_stacks_buffer);

    derived_list = NULL;
    derived_list_recycle_bin = NULL;

    instance_solver_thread_calc_derived_states(local_variant, &parent_state,
        NULL, &derived_list, &derived_list_recycle_bin, &derived_list_allocator,
        TRUE);

    for (derived_iter = derived_list; derived_iter;
         derived_iter = derived_iter->next)
    {
        fcs_init_and_encode_state(
            delta, local_variant, &(derived_iter->state), &got_child);

        if (compare_enc_states(&got_child, &child) == 0)
        {
            break;
        }
    }

    if (!derived_iter)
    {
        fprintf(stderr, "%s\n", "Failed to find move. Terminating.");
        exit(-1);
    }
    move_to_return = derived_iter->move;

    fc_solve_compact_allocator_finish(&(derived_list_allocator));
    fc_solve_meta_compact_allocator_finish(&meta_alloc);

    return move_to_return;
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

static GCC_INLINE void read_state_from_file(
    const enum fcs_dbm_variant_type_t local_variant, const char *const filename,
    fcs_state_keyval_pair_t *const init_state IND_BUF_T_PARAM(
        init_indirect_stacks_buffer))
{
    FILE *const fh = fopen(filename, "r");
    if (!fh)
    {
        fprintf(stderr, "Could not open file '%s' for input.\n", filename);
        exit(-1);
    }
    char user_state[USER_STATE_SIZE];
    memset(user_state, '\0', sizeof(user_state));
    fread(user_state, sizeof(user_state[0]), USER_STATE_SIZE - 1, fh);
    fclose(fh);

    fc_solve_initial_user_state_to_c(user_state, init_state, FREECELLS_NUM,
        STACKS_NUM, DECKS_NUM, init_indirect_stacks_buffer);
}
