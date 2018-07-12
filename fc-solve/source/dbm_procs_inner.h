/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#pragma once
#include "render_state.h"
#include "read_state.h"
typedef struct
{
    dbm_solver_thread thread;
    thread_arg arg;
    pthread_t id;
} main_thread_item;

static inline main_thread_item *dbm__calc_threads(
    dbm_solver_instance *const instance,
    fcs_state_keyval_pair *const init_state, const size_t num_threads,
    void (*init_thread_cb)(dbm_solver_thread *))
{
#ifdef T
    FILE *const out_fh = instance->common.out_fh;
#endif
    const_AUTO(local_variant, instance->common.variant);
    main_thread_item *const threads = SMALLOC(threads, num_threads);
    TRACE("%s\n", "instance_run_all_threads start");
#ifdef DEBUG_FOO
    fc_solve_delta_stater_init(&global_delta_stater, &(init_state->s),
        STACKS_NUM,
        FREECELLS_NUM PASS_ON_NOT_FC_ONLY(FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));
#endif
    for (size_t i = 0; i < num_threads; i++)
    {
        threads[i].thread.instance = instance;
        fc_solve_delta_stater_init(&(threads[i].thread.delta_stater),
            local_variant, &(init_state->s), STACKS_NUM,
            FREECELLS_NUM PASS_ON_NOT_FC_ONLY(
                FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));
        init_thread_cb(threads[i].arg.thread = &(threads[i].thread));
    }
    return threads;
}

static inline void dbm__free_threads(dbm_solver_instance *const instance,
    const size_t num_threads, main_thread_item *const threads,
    void (*free_thread_cb)(dbm_solver_thread *))
{
#ifdef T
    FILE *const out_fh = instance->common.out_fh;
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
    dbm_solver_instance *const instance, fcs_delta_stater *delta,
    fcs_encoded_state_buffer parent, fcs_encoded_state_buffer child)
{
    fcs_state_keyval_pair parent_state;
    fcs_derived_state *derived_list = NULL, *derived_list_recycle_bin = NULL;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    const_AUTO(local_variant, instance->common.variant);

    meta_allocator meta_alloc;
    fc_solve_meta_compact_allocator_init(&meta_alloc);
    compact_allocator derived_list_allocator;
    fc_solve_compact_allocator_init(&(derived_list_allocator), &meta_alloc);
    fc_solve_delta_stater_decode_into_state(
        delta, parent.s, &parent_state, indirect_stacks_buffer);

    instance_solver_thread_calc_derived_states(local_variant, &parent_state,
        NULL, &derived_list, &derived_list_recycle_bin, &derived_list_allocator,
        TRUE);

    for (var_AUTO(derived_iter, derived_list); derived_iter;
         derived_iter = derived_iter->next)
    {
        fcs_encoded_state_buffer got_child;
        fcs_init_and_encode_state(
            delta, local_variant, &(derived_iter->state), &got_child);
        if (compare_enc_states(&got_child, &child) == 0)
        {
            const_AUTO(move_to_return, derived_iter->move);
            fc_solve_compact_allocator_finish(&(derived_list_allocator));
            fc_solve_meta_compact_allocator_finish(&meta_alloc);
            return move_to_return;
        }
    }
    fc_solve_err("%s\n", "Failed to find move. Terminating.");
}

static void trace_solution(dbm_solver_instance *const instance,
    FILE *const out_fh, fcs_delta_stater *const delta)
{
    fprintf(out_fh, "%s\n", "Success!");
    fflush(out_fh);
/* Now trace the solution */
#ifdef FCS_DBM_WITHOUT_CACHES
    fcs_encoded_state_buffer *trace;
    int trace_num;
    fcs_state_keyval_pair state;
    unsigned char move = '\0';
    char move_buffer[500];
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);
    const_AUTO(local_variant, instance->common.variant);
    calc_trace(instance->common.queue_solution_ptr, &trace, &trace_num);

    for (int i = trace_num - 1; i >= 0; i--)
    {
        fc_solve_delta_stater_decode_into_state(
            delta, trace[i].s, &state, indirect_stacks_buffer);
        if (i > 0)
        {
            move = get_move_from_parent_to_child(
                instance, delta, trace[i], trace[i - 1]);
            move_to_string(move, move_buffer);
        }

        fcs_render_state_str state_str;
        FCS__RENDER_STATE(state_str, &(state.s), &locs);
        fprintf(out_fh, "--------\n%s\n==\n%s\n", state_str,
            (i > 0) ? move_buffer : "END");
        fflush(out_fh);
    }
    free(trace);
#endif
}

static inline void read_state_from_file(
    const fcs_dbm_variant_type local_variant GCC_UNUSED,
    const char *const filename,
    fcs_state_keyval_pair *const init_state IND_BUF_T_PARAM(
        init_indirect_stacks_buffer))
{
    FILE *const fh = fopen(filename, "r");
    if (!fh)
    {
        fc_solve_err("Could not open file '%s' for input.\n", filename);
    }
    const fcs_user_state_str user_state = read_state(fh);
    fc_solve_initial_user_state_to_c(user_state.s, init_state, FREECELLS_NUM,
        STACKS_NUM, DECKS_NUM, init_indirect_stacks_buffer);
#ifndef FCS_DISABLE_STATE_VALIDITY_CHECK
    fcs_card problem_card;
    if (FCS_STATE_VALIDITY__OK !=
        fc_solve_check_state_validity(init_state PASS_FREECELLS(FREECELLS_NUM)
                                          PASS_STACKS(STACKS_NUM)
                                              PASS_DECKS(DECKS_NUM),
            &problem_card))
    {
        fc_solve_err("%s\n", "Invalid input board");
    }
#endif
}

static inline FILE *calc_out_fh(const char *const out_filename)
{
    FILE *const out_fh = (out_filename ? fopen(out_filename, "at") : stdout);
    if (!out_fh)
    {
        fc_solve_err("Cannot open '%s' for output.\n", out_filename);
    }
    return out_fh;
}
