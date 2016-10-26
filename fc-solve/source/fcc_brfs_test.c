/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
/*
 * fcc_brfs_test.c - provide utility routines to test fcc_brfs.h
 */

#include "dll_thunk.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#undef FCS_RCS_STATES

#include "dbm_common.h"
#include "delta_states_any.h"
#include "fcc_brfs.h"
#include "fcc_brfs_test.h"
#include "render_state.h"

static void fc_solve_state_string_to_enc(
    const enum fcs_dbm_variant_type_t local_variant,
    fc_solve_delta_stater_t *delta, const char *const state_s_proto,
    fcs_encoded_state_buffer_t *enc_state)
{
    fcs_state_keyval_pair_t state;
    DECLARE_IND_BUF_T(state_indirect_stacks_buffer)

    fc_solve_initial_user_state_to_c(state_s_proto, &state, FREECELLS_NUM,
        STACKS_NUM, DECKS_NUM, state_indirect_stacks_buffer);

    fcs_init_and_encode_state(delta, local_variant, &(state), enc_state);
}

/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT int fc_solve_user_INTERNAL_find_fcc_start_points(
    const enum fcs_dbm_variant_type_t local_variant,
    const char *init_state_str_proto, const int start_state_moves_count,
    const fcs_fcc_move_t *const start_state_moves,
    fcs_FCC_start_point_result_t **const out_fcc_start_points,
    long *const out_num_new_positions)
{
    fcs_state_keyval_pair_t init_state;
    fcs_encoded_state_buffer_t enc_state;
    fcs_state_locs_struct_t locs;
    fcs_FCC_start_points_list_t start_points_list;
    dict_t *do_next_fcc_start_points_exist;
    dict_t *does_min_by_sorting_exist;
    fcs_lru_cache_t does_state_exist_in_any_FCC_cache;
    const int max_num_elements_in_cache = 1000;
    fcs_encoded_state_buffer_t min_by_sorting;
    fcs_fcc_moves_seq_allocator_t moves_list_allocator;
    fcs_fcc_moves_seq_t start_state_moves_seq;
    add_start_point_context_t add_start_point_context;
    void *tree_recycle_bin = NULL;

    fcs_compact_allocator_t moves_list_compact_alloc;

    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    fc_solve_initial_user_state_to_c(init_state_str_proto, &init_state,
        FREECELLS_NUM, STACKS_NUM, DECKS_NUM, indirect_stacks_buffer);

    fc_solve_delta_stater_t delta;
    fc_solve_delta_stater_init(
        &delta, &(init_state.s), STACKS_NUM, FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
        ,
        FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
        );

    fcs_init_and_encode_state(&delta, local_variant, &(init_state), &enc_state);

    start_points_list.list = NULL;
    start_points_list.recycle_bin = NULL;
    fcs_meta_compact_allocator_t meta_alloc;
    fc_solve_meta_compact_allocator_init(&meta_alloc);
    fc_solve_compact_allocator_init(
        &(start_points_list.allocator), &meta_alloc);

    do_next_fcc_start_points_exist = fc_solve_kaz_tree_create(
        fc_solve_compare_encoded_states, NULL, &meta_alloc, &tree_recycle_bin);

    does_min_by_sorting_exist = fc_solve_kaz_tree_create(
        fc_solve_compare_encoded_states, NULL, &meta_alloc, &tree_recycle_bin);

    cache_init(&does_state_exist_in_any_FCC_cache, max_num_elements_in_cache,
        &meta_alloc);

    fc_solve_compact_allocator_init(&(moves_list_compact_alloc), &(meta_alloc));

    moves_list_allocator.recycle_bin = NULL;
    moves_list_allocator.allocator = &(moves_list_compact_alloc);

    start_state_moves_seq.count = start_state_moves_count;
    start_state_moves_seq.moves_list = NULL;
    {
        fcs_fcc_moves_list_item_t **moves_iter =
            &(start_state_moves_seq.moves_list);
        for (int i = 0; i < start_state_moves_count;)
        {
            if (i % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                *(moves_iter) =
                    fc_solve_fcc_alloc_moves_list_item(&moves_list_allocator);
            }
            (*moves_iter)->data.s[i % FCS_FCC_NUM_MOVES_IN_ITEM] =
                start_state_moves[i];
            if ((++i) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                moves_iter = &((*moves_iter)->next);
            }
        }
    }

    add_start_point_context.do_next_fcc_start_points_exist =
        do_next_fcc_start_points_exist;
    add_start_point_context.next_start_points_list = &start_points_list;
    add_start_point_context.moves_list_allocator = &moves_list_allocator;

    fcs_bool_t is_min_by_sorting_new;
    perform_FCC_brfs(local_variant, &(init_state), enc_state,
        &start_state_moves_seq, fc_solve_add_start_point_in_mem,
        &add_start_point_context, &is_min_by_sorting_new, &min_by_sorting,
        does_min_by_sorting_exist, &does_state_exist_in_any_FCC_cache,
        out_num_new_positions, &moves_list_allocator, &meta_alloc);

    const fcs_FCC_start_point_t *iter = start_points_list.list;

    size_t states_count = 0;
    while (iter)
    {
        states_count++;
        iter = iter->next;
    }

    fcs_FCC_start_point_result_t *const ret = *out_fcc_start_points =
        SMALLOC(ret, states_count + 1);

    ret[states_count].count = 0;

    fc_solve_init_locs(&locs);

    iter = start_points_list.list;
    for (size_t i = 0; i < states_count; i++)
    {
        fcs_state_keyval_pair_t state;
        DECLARE_IND_BUF_T(state_indirect_stacks_buffer)

        ret[i].count = iter->moves_seq.count;
        ret[i].moves = SMALLOC(ret[i].moves, (size_t)ret[i].count);
        {
            fcs_fcc_moves_list_item_t *moves_iter = iter->moves_seq.moves_list;
            for (int moves_idx = 0; moves_idx < ret[i].count;)
            {
                ret[i].moves[moves_idx] =
                    moves_iter->data.s[moves_idx % FCS_FCC_NUM_MOVES_IN_ITEM];
                if ((++moves_idx) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
                {
                    moves_iter = moves_iter->next;
                }
            }
        }
        fc_solve_delta_stater_decode_into_state(
            &delta, iter->enc_state.s, &(state), state_indirect_stacks_buffer);
        ret[i].state_as_string = SMALLOC(ret[i].state_as_string, 1000);
        FCS__RENDER_STATE(ret[i].state_as_string, &(state.s), &locs);
        iter = iter->next;
    }

    fc_solve_compact_allocator_finish(&(start_points_list.allocator));
    fc_solve_compact_allocator_finish(&(moves_list_compact_alloc));

    fc_solve_delta_stater_release(&delta);
    fc_solve_kaz_tree_destroy(do_next_fcc_start_points_exist);
    fc_solve_kaz_tree_destroy(does_min_by_sorting_exist);
    cache_destroy(&does_state_exist_in_any_FCC_cache);
    fc_solve_meta_compact_allocator_finish(&meta_alloc);

    return 0;
}

DLLEXPORT int fc_solve_user_INTERNAL_is_fcc_new(
    const enum fcs_dbm_variant_type_t local_variant,
    const char *init_state_str_proto, const char *start_state_str_proto,
    /* NULL-terminated */
    const char **min_states,
    /* NULL-terminated */
    const char **states_in_cache, fcs_bool_t *const out_is_fcc_new)
{
    fcs_state_keyval_pair_t init_state;
    fcs_encoded_state_buffer_t enc_state;
    fcs_encoded_state_buffer_t start_enc_state;
    fcs_FCC_start_points_list_t start_points_list;
    dict_t *do_next_fcc_start_points_exist;
    dict_t *does_min_by_sorting_exist;
    fcs_compact_allocator_t temp_allocator;
    fcs_fcc_moves_seq_allocator_t moves_list_allocator;
    const int max_num_elements_in_cache = 1000000;
    fcs_lru_cache_t does_state_exist_in_any_FCC_cache;
    fcs_encoded_state_buffer_t min_by_sorting;
    fcs_fcc_moves_seq_t init_moves_seq;
    add_start_point_context_t add_start_point_context;
    void *tree_recycle_bin = NULL;

    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    fc_solve_initial_user_state_to_c(init_state_str_proto, &init_state,
        FREECELLS_NUM, STACKS_NUM, DECKS_NUM, indirect_stacks_buffer);

    fc_solve_delta_stater_t delta;
    fc_solve_delta_stater_init(
        &delta, &(init_state.s), STACKS_NUM, FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
        ,
        FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
        );

    fcs_init_and_encode_state(&delta, local_variant, &(init_state), &enc_state);

    fc_solve_state_string_to_enc(
        local_variant, &delta, start_state_str_proto, &(start_enc_state));

    fcs_meta_compact_allocator_t meta_alloc;
    fc_solve_meta_compact_allocator_init(&meta_alloc);

    start_points_list.list = NULL;
    start_points_list.recycle_bin = NULL;
    fc_solve_compact_allocator_init(
        &(start_points_list.allocator), &meta_alloc);

    do_next_fcc_start_points_exist = fc_solve_kaz_tree_create(
        fc_solve_compare_encoded_states, NULL, &meta_alloc, &tree_recycle_bin);

    does_min_by_sorting_exist = fc_solve_kaz_tree_create(
        fc_solve_compare_encoded_states, NULL, &meta_alloc, &tree_recycle_bin);

    fc_solve_compact_allocator_init(&(temp_allocator), &meta_alloc);

    moves_list_allocator.recycle_bin = NULL;
    moves_list_allocator.allocator = &(temp_allocator);

    /* Populate does_min_by_sorting_exist from min_states */
    {
        const char **min_states_iter = min_states;

        for (; *(min_states_iter); min_states_iter++)
        {
            fcs_encoded_state_buffer_t *min_enc_state;
            min_enc_state = (fcs_encoded_state_buffer_t *)fcs_compact_alloc_ptr(
                &(temp_allocator), sizeof(*min_enc_state));

            fc_solve_state_string_to_enc(
                local_variant, &delta, *(min_states_iter), min_enc_state);

            fc_solve_kaz_tree_alloc_insert(
                does_min_by_sorting_exist, min_enc_state);
        }
    }

    cache_init(&does_state_exist_in_any_FCC_cache, max_num_elements_in_cache,
        &meta_alloc);

    /* Populate does_state_exist_in_any_FCC_cache from states_in_cache */
    {
        const char **min_states_iter = states_in_cache;

        for (; *(min_states_iter); min_states_iter++)
        {
            fcs_encoded_state_buffer_t *min_enc_state;
            min_enc_state = (fcs_encoded_state_buffer_t *)fcs_compact_alloc_ptr(
                &(temp_allocator), sizeof(*min_enc_state));

            fc_solve_state_string_to_enc(
                local_variant, &delta, *(min_states_iter), min_enc_state);

            cache_insert(
                &does_state_exist_in_any_FCC_cache, min_enc_state, NULL, '\0');
        }
    }

    init_moves_seq.moves_list = NULL;
    init_moves_seq.count = 0;

    add_start_point_context.do_next_fcc_start_points_exist =
        do_next_fcc_start_points_exist;
    add_start_point_context.next_start_points_list = &start_points_list;
    add_start_point_context.moves_list_allocator = &moves_list_allocator;
    {
        long num_new_positions_temp;
        perform_FCC_brfs(local_variant, &(init_state), start_enc_state,
            &init_moves_seq, fc_solve_add_start_point_in_mem,
            &add_start_point_context, out_is_fcc_new, &min_by_sorting,
            does_min_by_sorting_exist, &does_state_exist_in_any_FCC_cache,
            &num_new_positions_temp, &moves_list_allocator, &meta_alloc);
    }

    fc_solve_compact_allocator_finish(&(start_points_list.allocator));
    fc_solve_compact_allocator_finish(&(temp_allocator));

    fc_solve_delta_stater_release(&delta);
    fc_solve_kaz_tree_destroy(do_next_fcc_start_points_exist);
    fc_solve_kaz_tree_destroy(does_min_by_sorting_exist);
    cache_destroy(&does_state_exist_in_any_FCC_cache);
    fc_solve_meta_compact_allocator_finish(&meta_alloc);

    return 0;
}
