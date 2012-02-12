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

#include "fcs_dllexport.h"
#include "delta_states.c"
#include "dbm_calc_derived.h"

/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT int fc_solve_user_INTERNAL_calc_derived_states_wrapper(
        const char * init_state_str_proto,
        int * num_out_derived_states,
        fcs_derived_state_debug_t * * out_derived_states,
        const fcs_bool_t perform_horne_prune
        )
{
    char * init_state_s;
    fcs_state_keyval_pair_t init_state;
    fc_solve_delta_stater_t * delta;
    fcs_encoded_state_buffer_t enc_state;
#ifdef FCS_WITHOUT_LOCS_FIELDS
    fcs_state_locs_struct_t locs;
    int i;
#endif

    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    init_state_s = prepare_state_str(init_state_str_proto);

    fc_solve_initial_user_state_to_c(
            init_state_s,
            &init_state,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM
#ifdef INDIRECT_STACK_STATES
            , indirect_stacks_buffer
#endif
            );

    delta = fc_solve_delta_stater_alloc(
            &(init_state.s),
            STACKS_NUM,
            FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
            , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
            );

    fcs_init_encoded_state(&enc_state);
    fc_solve_delta_stater_encode_into_buffer(
        delta,
        &(init_state),
        enc_state.s
        );

    fcs_derived_state_t * derived_list = NULL;
    fcs_derived_state_t * derived_list_recycle_bin = NULL;
    fcs_compact_allocator_t allocator;
    fcs_meta_compact_allocator_t meta_alloc;
    fc_solve_meta_compact_allocator_init (&meta_alloc);
    fc_solve_compact_allocator_init( &allocator, &meta_alloc);

    instance_solver_thread_calc_derived_states(
        &init_state,
        &enc_state,
        &derived_list,
        &derived_list_recycle_bin,
        &allocator,
        perform_horne_prune
    );

    int states_count = 0;
    fcs_derived_state_t * iter;

    iter = derived_list;

    while (iter)
    {
        states_count++;
        iter = iter->next;
    }

    *(num_out_derived_states) = states_count;

    fcs_derived_state_debug_t * debug_ret;

    debug_ret = malloc(sizeof(debug_ret[0]) * states_count);

    *(out_derived_states) = debug_ret;

#ifdef FCS_WITHOUT_LOCS_FIELDS
    for (i=0 ; i < MAX_NUM_STACKS ; i++)
    {
        locs.stack_locs[i] = i;
    }
    for (i=0 ; i < MAX_NUM_FREECELLS ; i++)
    {
        locs.fc_locs[i] = i;
    }
#endif
    int idx = 0;
    iter = derived_list;
    while (iter)
    {
        debug_ret[idx].state_string = 
        fc_solve_state_as_string(
#ifdef FCS_RCS_STATES
            &(iter->state.s),
            &(iter->state.info),
#else
            &(iter->state),
#endif
#ifdef FCS_WITHOUT_LOCS_FIELDS
            &locs,
#endif
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            1,
            0,
            1
            );
        debug_ret[idx].move = iter->parent_and_move.s[iter->parent_and_move.s[0]+1];
        debug_ret[idx].is_reversible_move = iter->is_reversible_move;
        idx++;
        iter = iter->next;
    }

    assert(idx == states_count);

    free(init_state_s);
    fc_solve_compact_allocator_finish(&allocator);
    fc_solve_meta_compact_allocator_finish( &meta_alloc );

    fc_solve_delta_stater_free (delta);

    return 0;
}
