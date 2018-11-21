/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
// dbm_calc_derived_test.c - testing interface for dbm_calc_derived.h .
// This is compiled into a shared library and then loaded by Inline::C,
// ctypes, etc.
#include <assert.h>
#include "fcs_conf.h"
#undef FCS_RCS_STATES
#include "delta_states_any.h"
#include "dbm_calc_derived.h"
#include "render_state.h"

/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT int fc_solve_user_INTERNAL_calc_derived_states_wrapper(
    const fcs_dbm_variant_type local_variant, const char *init_state_str_proto,
    int *const num_out_derived_states,
    fcs_derived_state_debug **out_derived_states,
    const bool perform_horne_prune)
{
    fcs_state_keyval_pair init_state;
    fcs_encoded_state_buffer enc_state;
    fcs_derived_state *derived_list = NULL;
    fcs_derived_state *derived_list_recycle_bin = NULL;
    size_t states_count = 0;
    fcs_derived_state *iter;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    fc_solve_initial_user_state_to_c(init_state_str_proto, &init_state,
        FREECELLS_NUM, STACKS_NUM, DECKS_NUM, indirect_stacks_buffer);

    fcs_delta_stater delta;
    fc_solve_delta_stater_init(&delta, local_variant, &(init_state.s),
        STACKS_NUM,
        FREECELLS_NUM PASS_ON_NOT_FC_ONLY(FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));

    fcs_init_and_encode_state(&delta, local_variant, &(init_state), &enc_state);

    meta_allocator meta_alloc;
    fc_solve_meta_compact_allocator_init(&meta_alloc);
    compact_allocator allocator;
    fc_solve_compact_allocator_init(&allocator, &meta_alloc);

    instance_solver_thread_calc_derived_states(local_variant, &init_state, NULL,
        &derived_list, &derived_list_recycle_bin, &allocator,
        perform_horne_prune);

    iter = derived_list;

    while (iter)
    {
        states_count++;
        iter = iter->next;
    }

    *(num_out_derived_states) = states_count;

    fcs_derived_state_debug *const debug_ret = SMALLOC(debug_ret, states_count);
    *(out_derived_states) = debug_ret;

    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);

    iter = derived_list;
    size_t idx = 0;
    while (iter)
    {
        debug_ret[idx] = (typeof(debug_ret[idx])){
            .state_string = SMALLOC(debug_ret[idx].state_string, 1000),
            .move = iter->move,
            .core_irreversible_moves_count =
                iter->core_irreversible_moves_count,
            .num_non_reversible_moves_including_prune =
                iter->num_non_reversible_moves_including_prune,
            .which_irreversible_moves_bitmask =
                iter->which_irreversible_moves_bitmask};
        FCS__RENDER_STATE(debug_ret[idx].state_string, &(iter->state.s), &locs);
        /* TODO : Put something meaningful there by passing it to the function.
         */
        ++idx;
        iter = iter->next;
    }

    assert(idx == states_count);
    fc_solve_compact_allocator_finish(&allocator);
    fc_solve_meta_compact_allocator_finish(&meta_alloc);
    fc_solve_delta_stater_release(&delta);

    return 0;
}

DLLEXPORT void fc_solve_user_INTERNAL_free_derived_states(
    const int num_derived_states, fcs_derived_state_debug *const derived_states)
{
    fcs_derived_state_debug *iter = derived_states;
    for (int i = 0; i < num_derived_states; i++, iter++)
    {
        free(iter->state_string);
    }
    free(derived_states);
}

/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT int fc_solve_user_INTERNAL_perform_horne_prune(
    const fcs_dbm_variant_type local_variant, const char *init_state_str_proto,
    char **ret_state_s)
{
    fcs_state_keyval_pair init_state;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);

    fc_solve_initial_user_state_to_c(init_state_str_proto, &init_state,
        FREECELLS_NUM, STACKS_NUM, DECKS_NUM, indirect_stacks_buffer);

    const_AUTO(prune_ret, horne_prune__simple(local_variant, &init_state));
    *ret_state_s = SMALLOC(*ret_state_s, 1000);
    FCS__RENDER_STATE(*ret_state_s, &(init_state.s), &locs);

    return prune_ret;
}

DLLEXPORT void fc_solve_user_INTERNAL_perform_horne_prune__free_ret_state_s(
    char *const ret_state_s)
{
    free(ret_state_s);
}
