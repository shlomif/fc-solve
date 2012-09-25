/* Copyright (c) 2011 Shlomi Fish
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
 * dbm_calc_derived_test.c - testing interface for dbm_calc_derived.h .
 * This is compiled into a shared library and then loaded by Inline::C,
 * ctypes, etc.
 */

#define BUILDING_DLL 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "config.h"
#undef FCS_RCS_STATES

#include "fcs_dllexport.h"
#include "delta_states_any.h"
#include "dbm_calc_derived.h"

/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT int fc_solve_user_INTERNAL_calc_derived_states_wrapper(
        enum fcs_dbm_variant_type_t local_variant,
        const char * init_state_str_proto,
        int * num_out_derived_states,
        fcs_derived_state_debug_t * * out_derived_states,
        const fcs_bool_t perform_horne_prune
        )
{
    fcs_state_keyval_pair_t init_state;
    fc_solve_delta_stater_t * delta;
    fcs_encoded_state_buffer_t enc_state;
    fcs_state_locs_struct_t locs;
    fcs_derived_state_t * derived_list = NULL;
    fcs_derived_state_t * derived_list_recycle_bin = NULL;
    fcs_compact_allocator_t allocator;
    fcs_meta_compact_allocator_t meta_alloc;
    int states_count = 0;
    fcs_derived_state_t * iter;
    fcs_derived_state_debug_t * debug_ret;
    int idx = 0;

    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    fc_solve_initial_user_state_to_c(
            init_state_str_proto,
            &init_state,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            indirect_stacks_buffer
            );

    delta = fc_solve_delta_stater_alloc(
            &(init_state.s),
            STACKS_NUM,
            FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
            , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
            );

    fcs_init_and_encode_state(
        delta,
        local_variant,
        &(init_state),
        &enc_state
        );

    fc_solve_meta_compact_allocator_init (&meta_alloc);
    fc_solve_compact_allocator_init( &allocator, &meta_alloc);

    instance_solver_thread_calc_derived_states(
        local_variant,
        &init_state,
        &enc_state,
        NULL,
        &derived_list,
        &derived_list_recycle_bin,
        &allocator,
        perform_horne_prune
    );


    iter = derived_list;

    while (iter)
    {
        states_count++;
        iter = iter->next;
    }

    *(num_out_derived_states) = states_count;


    debug_ret = malloc(sizeof(debug_ret[0]) * states_count);

    *(out_derived_states) = debug_ret;

    fc_solve_init_locs(&locs);

    iter = derived_list;
    while (iter)
    {
        debug_ret[idx].state_string =
        fc_solve_state_as_string(
            &(iter->state.s),
            &(iter->state.info),
            &locs,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            1,
            0,
            1
            );
        debug_ret[idx].move = iter->move;
        debug_ret[idx].core_irreversible_moves_count
            = iter->core_irreversible_moves_count;
        debug_ret[idx].num_non_reversible_moves_including_prune
            = iter->num_non_reversible_moves_including_prune;
        idx++;
        iter = iter->next;
    }

    assert(idx == states_count);

    fc_solve_compact_allocator_finish(&allocator);
    fc_solve_meta_compact_allocator_finish( &meta_alloc );

    fc_solve_delta_stater_free (delta);

    return 0;
}

/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT int fc_solve_user_INTERNAL_perform_horne_prune(
        enum fcs_dbm_variant_type_t local_variant,
        const char * init_state_str_proto,
        char * * ret_state_s
        )
{
    fcs_state_keyval_pair_t init_state;
    fcs_state_locs_struct_t locs;
    int prune_ret;

    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    fc_solve_init_locs(&locs);

    fc_solve_initial_user_state_to_c(
            init_state_str_proto,
            &init_state,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            indirect_stacks_buffer
            );

    prune_ret = horne_prune(local_variant, &init_state, NULL, NULL);
    *ret_state_s =
        fc_solve_state_as_string(
            &(init_state.s),
            &(init_state.info),
            &locs,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            1,
            0,
            1
            );

    return prune_ret;
}
