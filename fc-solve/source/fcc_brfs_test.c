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

static void fc_solve_state_string_to_enc(
    fc_solve_delta_stater_t * delta,
    const char * state_s_proto,
    fcs_encoded_state_buffer_t * enc_state
)
{
    fcs_state_keyval_pair_t state;
    DECLARE_IND_BUF_T(state_buffer)

    char * state_s = prepare_state_str(state_s_proto);

    fc_solve_initial_user_state_to_c(
        state_s,
        &state,
        FREECELLS_NUM,
        STACKS_NUM,
        DECKS_NUM
#ifdef INDIRECT_STACK_STATES
        , state_buffer
#endif
        );

    fcs_init_and_encode_state(
        delta,
        &(state),
        enc_state
    );

    free(state_s);
}
                                        
/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT int fc_solve_user_INTERNAL_find_fcc_start_points(
        const char * init_state_str_proto,
        const int start_state_moves_count,
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

    fcs_init_and_encode_state(
        delta,
        &(init_state),
        &enc_state
    );

    fcs_meta_compact_allocator_t meta_alloc;
    fcs_FCC_start_points_list_t start_points_list;
    start_points_list.recycle_bin = NULL;
    fc_solve_meta_compact_allocator_init( &meta_alloc );
    fc_solve_compact_allocator_init(&(start_points_list.allocator), &meta_alloc);

    dict_t * do_next_fcc_start_points_exist =
        fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL, &meta_alloc);

    dict_t * does_min_by_sorting_exist =
        fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL, &meta_alloc);

    fcs_lru_cache_t does_state_exist_in_any_FCC_cache;

    const int max_num_elements_in_cache = 1000;
    cache_init(&does_state_exist_in_any_FCC_cache, max_num_elements_in_cache, &meta_alloc);

    fcs_bool_t is_min_by_sorting_new;
    fcs_encoded_state_buffer_t min_by_sorting;

    fcs_compact_allocator_t moves_list_compact_alloc;
    fc_solve_compact_allocator_init(&(moves_list_compact_alloc), &(meta_alloc));

    fcs_fcc_moves_seq_allocator_t moves_list_allocator;
    moves_list_allocator.recycle_bin = NULL;
    moves_list_allocator.allocator = &(moves_list_compact_alloc);

    fcs_fcc_moves_seq_t start_state_moves_seq;
    start_state_moves_seq.count = start_state_moves_count;
    start_state_moves_seq.moves_list = NULL;
    {
        fcs_fcc_moves_list_item_t * * iter = &(start_state_moves_seq.moves_list);
        int i;
        for ( i=0 ; i < start_state_moves_count ; )
        {
            if (i % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                *(iter) = fc_solve_fcc_alloc_moves_list_item(&moves_list_allocator);
            }
            (*iter)->data.s[i % FCS_FCC_NUM_MOVES_IN_ITEM] = start_state_moves[i];
            if ((++i) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                iter = &((*iter)->next);
            }
        }
    }
    perform_FCC_brfs(
        &(init_state),
        enc_state,
        &start_state_moves_seq,
        &(start_points_list),
        do_next_fcc_start_points_exist,
        &is_min_by_sorting_new,
        &min_by_sorting,
        does_min_by_sorting_exist,
        &does_state_exist_in_any_FCC_cache,
        out_num_new_positions,
        &moves_list_allocator,
        &meta_alloc
    );

    int states_count = 0;
    fcs_FCC_start_point_t * iter;
    struct avl_traverser trav;

    avl_t_init (&trav, do_next_fcc_start_points_exist);
    iter = avl_t_first(&trav, do_next_fcc_start_points_exist);

    while (iter)
    {
        states_count++;
        iter = avl_t_next(&trav);
    }

    fcs_FCC_start_point_result_t * ret;

    *out_fcc_start_points = ret = malloc(sizeof(ret[0]) * (states_count+1));

    ret[states_count].count = 0;

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
    avl_t_init (&trav, do_next_fcc_start_points_exist);
    iter = avl_t_first(&trav, do_next_fcc_start_points_exist);
    for (i = 0; i < states_count ; i++)
    {
        ret[i].count = iter->moves_seq.count;
        ret[i].moves = malloc(sizeof(ret[i].moves[0]) * ret[i].count);
        {
            int moves_idx;
            fcs_fcc_moves_list_item_t * moves_iter = iter->moves_seq.moves_list;
            for (moves_idx = 0 ; moves_idx < ret[i].count ; )
            {
                ret[i].moves[moves_idx] = moves_iter->data.s[moves_idx % FCS_FCC_NUM_MOVES_IN_ITEM];
                if ((++moves_idx) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
                {
                    moves_iter = moves_iter->next;
                }
            }
        }
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

        iter = avl_t_next(&trav);
    }

    free(init_state_s);
    fc_solve_compact_allocator_finish(&(start_points_list.allocator));
    fc_solve_compact_allocator_finish(&(moves_list_compact_alloc));

    fc_solve_delta_stater_free (delta);
    fc_solve_kaz_tree_destroy(do_next_fcc_start_points_exist);
    fc_solve_kaz_tree_destroy(does_min_by_sorting_exist);
    cache_destroy(&does_state_exist_in_any_FCC_cache);
    fc_solve_meta_compact_allocator_finish( &meta_alloc );

    return 0;
}

DLLEXPORT int fc_solve_user_INTERNAL_is_fcc_new(
        const char * init_state_str_proto,
        const char * start_state_str_proto,
        /* NULL-terminated */
        const char * * min_states,
        /* NULL-terminated */
        const char * * states_in_cache,
        fcs_bool_t * out_is_fcc_new
        )
{
    char * init_state_s;
    fcs_state_keyval_pair_t init_state;
    fc_solve_delta_stater_t * delta;
    fcs_encoded_state_buffer_t enc_state;
    fcs_encoded_state_buffer_t start_enc_state;
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

    fcs_init_and_encode_state(
        delta,
        &(init_state),
        &enc_state
    );

    fc_solve_state_string_to_enc(
        delta,
        start_state_str_proto,
        &(start_enc_state)
    );

    fcs_meta_compact_allocator_t meta_alloc;
    fcs_FCC_start_points_list_t start_points_list;

    fc_solve_meta_compact_allocator_init( &meta_alloc );
    start_points_list.recycle_bin = NULL;
    fc_solve_compact_allocator_init(&(start_points_list.allocator), &meta_alloc);

    dict_t * do_next_fcc_start_points_exist =
        fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL, &meta_alloc);

    dict_t * does_min_by_sorting_exist =
        fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL, &meta_alloc);

    fcs_compact_allocator_t temp_allocator;
    fc_solve_compact_allocator_init(&(temp_allocator), &meta_alloc);

    fcs_fcc_moves_seq_allocator_t moves_list_allocator;
    moves_list_allocator.recycle_bin = NULL;
    moves_list_allocator.allocator = &(temp_allocator);

    /* Populate does_min_by_sorting_exist from min_states */
    {
        const char * * min_states_iter = min_states;

        for (; *(min_states_iter) ; min_states_iter++)
        {
            fcs_encoded_state_buffer_t * min_enc_state;
            min_enc_state = (fcs_encoded_state_buffer_t *)
                fcs_compact_alloc_ptr(
                    &(temp_allocator),
                    sizeof (*min_enc_state)
                    );

            fc_solve_state_string_to_enc(
                delta,
                *(min_states_iter),
                min_enc_state
            );

            fc_solve_kaz_tree_alloc_insert(
                does_min_by_sorting_exist,
                min_enc_state
            );
        }
    }

    const int max_num_elements_in_cache = 1000000;

    fcs_lru_cache_t does_state_exist_in_any_FCC_cache;
    cache_init(&does_state_exist_in_any_FCC_cache, max_num_elements_in_cache, &meta_alloc);

    /* Populate does_state_exist_in_any_FCC_cache from states_in_cache */
    {
        const char * * min_states_iter = states_in_cache;

        for (; *(min_states_iter) ; min_states_iter++)
        {
            fcs_encoded_state_buffer_t * min_enc_state;
            min_enc_state = (fcs_encoded_state_buffer_t *)
                fcs_compact_alloc_ptr(
                    &(temp_allocator),
                    sizeof (*min_enc_state)
                    );

            fc_solve_state_string_to_enc(
                delta,
                *(min_states_iter),
                min_enc_state
            );

            cache_insert (&does_state_exist_in_any_FCC_cache, min_enc_state);
        }
    }

    fcs_encoded_state_buffer_t min_by_sorting;
    long num_new_positions_temp;
    fcs_fcc_moves_seq_t init_moves_seq;

    init_moves_seq.moves_list = NULL;
    init_moves_seq.count = 0;

    perform_FCC_brfs(
        &(init_state),
        start_enc_state,
        &init_moves_seq,
        &(start_points_list),
        do_next_fcc_start_points_exist,
        out_is_fcc_new,
        &min_by_sorting,
        does_min_by_sorting_exist,
        &does_state_exist_in_any_FCC_cache,
        &num_new_positions_temp,
        &moves_list_allocator,
        &meta_alloc
    );

    free(init_state_s);
    fc_solve_compact_allocator_finish(&(start_points_list.allocator));
    fc_solve_compact_allocator_finish(&(temp_allocator));

    fc_solve_delta_stater_free (delta);
    fc_solve_kaz_tree_destroy(do_next_fcc_start_points_exist);
    fc_solve_kaz_tree_destroy(does_min_by_sorting_exist);
    cache_destroy(&does_state_exist_in_any_FCC_cache);
    fc_solve_meta_compact_allocator_finish( &meta_alloc );

    return 0;
}
