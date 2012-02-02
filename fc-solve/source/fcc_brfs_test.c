/* Copyright (c) 2012 Shlomi Fish
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
 * fcc_brfs_test.c - provide utility routines to test fcc_brfs.h
 */

#define BUILDING_DLL 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "fcs_dllexport.h"
#include "delta_states.c"
#include "fcc_brfs.h"
#include "fcc_brfs_test.h"

/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT int fc_solve_user_INTERNAL_find_fcc_start_points(
        const char * init_state_str_proto,
        const int count_start_state_moves,
        const fcs_fcc_move_t * const start_state_moves,
        fcs_FCC_start_point_result_t * * out_fcc_start_points,
        long * out_num_new_positions
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

#ifdef INDIRECT_STACK_STATES
    dll_ind_buf_t indirect_stacks_buffer;
#endif

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

    memset(&(enc_state), '\0', sizeof(enc_state));

    fc_solve_delta_stater_encode_into_buffer(
        delta,
        &(init_state),
        enc_state.s
    );

    fcs_FCC_start_points_list_t start_points_list;
    start_points_list.list = NULL;
    start_points_list.recycle_bin = NULL;
    fc_solve_compact_allocator_init(&(start_points_list.allocator));

    dict_t * do_next_fcc_start_points_exist =
        fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL);

    dict_t * does_min_by_sorting_exist =
        fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL);

    fcs_lru_cache_t does_state_exist_in_any_FCC_cache;

    const int max_num_elements_in_cache = 1000;
    cache_init(&does_state_exist_in_any_FCC_cache, max_num_elements_in_cache);

    fcs_bool_t is_min_by_sorting_new;
    fcs_encoded_state_buffer_t min_by_sorting;

    perform_FCC_brfs(
        &(init_state),
        enc_state,
        count_start_state_moves,
        start_state_moves,
        &(start_points_list),
        do_next_fcc_start_points_exist,
        &is_min_by_sorting_new,
        &min_by_sorting,
        does_min_by_sorting_exist,
        &does_state_exist_in_any_FCC_cache,
        out_num_new_positions
    );

    int states_count = 0;
    fcs_FCC_start_point_t * iter;

    iter = start_points_list.list;

    while (iter)
    {
        states_count++;
        iter = iter->next;
    }

    fcs_FCC_start_point_result_t * ret;

    *out_fcc_start_points = ret = malloc(sizeof(ret[0]) * (states_count+1));

    ret[states_count].count_moves = 0;

    int i;

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
    iter = start_points_list.list;
    for (i = 0; i < states_count ; i++)
    {
        ret[i].count_moves = iter->count_moves;
        ret[i].moves = iter->moves;
        fcs_state_keyval_pair_t state;
        fc_solve_delta_stater_decode_into_state(delta, iter->enc_state.s, &(state), indirect_stacks_buffer);
        ret[i].state_as_string =
        fc_solve_state_as_string(
#ifdef FCS_RCS_STATES
            &(state.s),
            &(state.info),
#else
            &(state),
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

        iter = iter->next;
    }

    free(init_state_s);
    fc_solve_compact_allocator_finish(&(start_points_list.allocator));

    fc_solve_delta_stater_free (delta);
    fc_solve_kaz_tree_destroy(do_next_fcc_start_points_exist);
    fc_solve_kaz_tree_destroy(does_min_by_sorting_exist);
    cache_destroy(&does_state_exist_in_any_FCC_cache);

    return 0;
}
