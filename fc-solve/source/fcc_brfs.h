/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
// fcc_brfs.h - contains the perform_FCC_brfs() function to perform the
// Breadth-first-search on the Fully-connected-component.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include "state.h"
#include "meta_alloc.h"
#include "freecell-solver/fcs_enums.h"
#include "dbm_common.h"
#include "delta_states.h"
#include "dbm_calc_derived_iface.h"
#include "dbm_cache.h"
#include "fcc_brfs_test.h"
#include "dbm_solver.h"
#include "dbm_calc_derived.h"

typedef struct fcs_FCC_start_point_struct
{
    fcs_encoded_state_buffer enc_state;
    fcs_fcc_moves_seq moves_seq;
    struct fcs_FCC_start_point_struct *next;
} fcs_FCC_start_point;

typedef struct
{
    fcs_FCC_start_point *list;
    compact_allocator allocator;
    fcs_FCC_start_point *recycle_bin;
} fcs_FCC_start_points_list;

static int fc_solve_compare_encoded_states(const void *const void_a,
    const void *const void_b, void *const context GCC_UNUSED)
{
#define GET_PARAM(p) ((const fcs_encoded_state_buffer *)p)
    return memcmp(
        GET_PARAM(void_a), GET_PARAM(void_b), sizeof(*(GET_PARAM(void_a))));
#undef GET_PARAM
}

static inline void fc_solve_fcc_release_moves_seq(
    fcs_fcc_moves_seq *const moves_seq,
    fcs_fcc_moves_seq_allocator *const moves_list_allocator)
{
    fcs_fcc_moves_list_item *iter = moves_seq->moves_list;
    fcs_fcc_moves_list_item *iter_next;

    if (iter)
    {
        for (iter_next = iter->next; iter_next;
             iter = iter_next, iter_next = iter_next->next)
        {
            iter->next = moves_list_allocator->recycle_bin;
            moves_list_allocator->recycle_bin = iter;
        }
        /* Fix for a leak - iter is still defined, and needs
         * to be recycled.
         * */
        iter->next = moves_list_allocator->recycle_bin;
        moves_list_allocator->recycle_bin = iter;
    }
    moves_seq->moves_list = NULL;
    moves_seq->count = -1;
}

static inline void fc_solve__internal__copy_moves(
    fcs_fcc_moves_seq *const moves_seq, int *const ptr_to_pos_in_moves,
    fcs_fcc_moves_list_item ***const ptr_to_end_moves_iter,
    const unsigned char extra_move,
    fcs_fcc_moves_seq_allocator *const moves_list_allocator)
{
    int pos_in_moves = *ptr_to_pos_in_moves;
    var_AUTO(end_moves_iter, *ptr_to_end_moves_iter);
    const fcs_fcc_moves_list_item *copy_from_iter = moves_seq->moves_list;
    const_SLOT(count, moves_seq);

    for (int copy_from_idx = 0; copy_from_idx < count;)
    {
        if (pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
        {
            (*end_moves_iter) =
                fc_solve_fcc_alloc_moves_list_item(moves_list_allocator);
        }
        (*end_moves_iter)->data.s[pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM] =
            copy_from_iter->data.s[copy_from_idx % FCS_FCC_NUM_MOVES_IN_ITEM];
        if ((++pos_in_moves) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
        {
            end_moves_iter = &((*end_moves_iter)->next);
        }
        if ((++copy_from_idx) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
        {
            copy_from_iter = copy_from_iter->next;
        }
    }

    /* Append the remaining moves. */
    if (pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
    {
        (*end_moves_iter) =
            fc_solve_fcc_alloc_moves_list_item(moves_list_allocator);
    }
    (*end_moves_iter)->data.s[(pos_in_moves++) % FCS_FCC_NUM_MOVES_IN_ITEM] =
        extra_move;

    *ptr_to_pos_in_moves = pos_in_moves;
    *ptr_to_end_moves_iter = end_moves_iter;
}

static inline dict_t *fcc_brfs_kaz_tree_create(
    meta_allocator *const meta_alloc, void **const tree_recycle_bin)
{
    return fc_solve_kaz_tree_create(
        fc_solve_compare_encoded_states, NULL, meta_alloc, tree_recycle_bin);
}

static inline void fcc_brfs_add_key_to_tree(
    dict_t *const tree, const fcs_encoded_state_buffer *const val)
{
    fcs_encoded_state_buffer *key_to_add =
        fcs_compact_alloc_ptr(&(tree->dict_allocator), sizeof(*key_to_add));
    *key_to_add = *val;
    fc_solve_kaz_tree_alloc_insert(tree, key_to_add);
}

static void perform_FCC_brfs(const fcs_dbm_variant_type local_variant,
    /* The first state in the game, from which all states are encoded. */
    fcs_state_keyval_pair *init_state,
    /* The start state. */
    fcs_encoded_state_buffer start_state,
    /* The moves leading up to the state.
     * */
    const fcs_fcc_moves_seq *const start_state_moves_seq,
#if 0
    /* [Output]: FCC start points.
     * */
    fcs_FCC_start_points_list * fcc_start_points,
    /* [Input/Output]: make sure the fcc_start_points don't repeat themselves,
     * in the same FCC-based-depth.
     * */
    dict_t * do_next_fcc_start_points_exist,
#else
    /*
     * [Output] a callback to add a point to the next_start_points,
     * and its context.
     */
    bool (*add_start_point)(const fcs_encoded_state_buffer *const enc_state,
        const fcs_fcc_moves_seq *const start_state_moves_seq,
        fcs_fcc_moves_seq *const after_start_moves_seq,
        const unsigned char extra_move, void *const context),
    void *add_start_point_context,
#endif
    /* [Output]: Is the min_by_sorting new.
     * */
    bool *is_min_by_sorting_new,
    /* [Output]: The min_by_sorting.
     * */
    fcs_encoded_state_buffer *const min_by_sorting,
    /* [Input/Output]: The ${next}_depth_FCCs.DoesExist
     * (for the right depth based on the current depth and pruning.)
     * Of type Map{min_by_sorting => Bool Exists} DoesExist.
     */
    dict_t *does_min_by_sorting_exist,
    /* [Input/Output]: The LRU cache.
     * Of type <<LRU_Map{any_state_in_the_FCCs => Bool Exists} Cache>>
     * */
    fcs_lru_cache *does_state_exist_in_any_FCC_cache,
    /* [Output]: the number of new positions/states scanned in the FCC. If
     * it's not new, this will be set to zero (0). It includes the initial
     * position, but does not include the start points of the new FCC.
     * */
    long *const out_num_new_positions,
    /* [Input/Output]: the list allocator. */
    fcs_fcc_moves_seq_allocator *const moves_list_allocator,
    /* [Input/Output]: The meta allocator - needed to allocate and free
     * the compact allocators. */
    meta_allocator *const meta_alloc)
{
    void *tree_recycle_bin = NULL;
    fcs_dbm_queue_item *queue_head, *queue_tail, *queue_recycle_bin = NULL,
                                                 *extracted_item;
    fcs_derived_state *derived_list_recycle_bin = NULL, *next_derived_iter;
    fcs_state_keyval_pair state;
    bool running_min_was_assigned = FALSE;
    fcs_encoded_state_buffer running_min = {{0}};
    long num_new_positions = 0;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

/* Some sanity checks. */
#ifndef NDEBUG
    assert(add_start_point);
    assert(add_start_point_context);
    assert(is_min_by_sorting_new);
    assert(min_by_sorting);
    assert(does_min_by_sorting_exist);
    assert(does_state_exist_in_any_FCC_cache);
#endif

    /* Initialize the allocators. */
    compact_allocator queue_allocator, derived_list_allocator;
    fc_solve_compact_allocator_init(&(queue_allocator), meta_alloc);
    fc_solve_compact_allocator_init(&(derived_list_allocator), meta_alloc);

    /* TODO : maybe pass delta_stater as an argument  */
    fcs_delta_stater delta_stater;
    fc_solve_delta_stater_init(&delta_stater, FCS_DBM_VARIANT_2FC_FREECELL,
        &(init_state->s), STACKS_NUM,
        FREECELLS_NUM PASS_ON_NOT_FC_ONLY(FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));
    dict_t *traversed_states =
        fcc_brfs_kaz_tree_create(meta_alloc, &tree_recycle_bin);

    fcs_dbm_queue_item *new_item = (fcs_dbm_queue_item *)fcs_compact_alloc_ptr(
        &(queue_allocator), sizeof(*new_item));
    *new_item = (typeof(*new_item)){.key = start_state,
        .next = NULL,
        .moves_seq = {.count = 0, .moves_list = NULL}};
    queue_head = queue_tail = new_item;

    *out_num_new_positions = num_new_positions;

    fc_solve_kaz_tree_alloc_insert(traversed_states, &(new_item->key));

    /* Extract an item from the queue. */
    while ((extracted_item = queue_head))
    {
        num_new_positions++;
        if (!(queue_head = extracted_item->next))
        {
            queue_tail = NULL;
        }

        /* Handle the min_by_sorting scan. */
        if ((!running_min_was_assigned) ||
            (memcmp(&(extracted_item->key), &running_min, sizeof(running_min)) <
                0))
        {
            running_min_was_assigned = TRUE;
            running_min = extracted_item->key;
            if (cache_does_key_exist(
                    does_state_exist_in_any_FCC_cache, &(running_min)))
            {
                *is_min_by_sorting_new = FALSE;
                goto free_resources;
            }
            else
            {
                cache_insert(does_state_exist_in_any_FCC_cache, &(running_min),
                    NULL, '\0');
            }
        }

        /* Calculate the derived list. */
        fcs_derived_state *derived_list = NULL;

        /* Handle item. */
        fc_solve_delta_stater_decode_into_state(&delta_stater,
            extracted_item->key.s, &(state), indirect_stacks_buffer);

        instance_solver_thread_calc_derived_states(local_variant, &state, NULL,
            &derived_list, &derived_list_recycle_bin, &derived_list_allocator,
            /* Horne's Prune should be disabled because that interferes
             * with the FCC-depth. */
            FALSE);

        /* Allocate a spare 'new_item'. */
        if (queue_recycle_bin)
        {
            new_item = queue_recycle_bin;
            queue_recycle_bin = queue_recycle_bin->next;
        }
        else
        {
            new_item = (fcs_dbm_queue_item *)fcs_compact_alloc_ptr(
                &(queue_allocator), sizeof(*new_item));
        }

        /* Enqueue the new elements to the queue. */
        for (var_AUTO(derived_iter, derived_list); derived_iter;
             derived_iter = next_derived_iter)
        {
            const bool is_reversible =
                (derived_iter->core_irreversible_moves_count == 0);
            fcs_init_and_encode_state(&delta_stater, local_variant,
                &(derived_iter->state), &(new_item->key));
            const_AUTO(extra_move, derived_iter->move);

            if (!(is_reversible
                        ? (fc_solve_kaz_tree_lookup_value(
                               traversed_states, &(new_item->key)) != NULL)
                        : add_start_point(&(new_item->key),
                              start_state_moves_seq,
                              &(extracted_item->moves_seq), extra_move,
                              add_start_point_context)))
            {
                if (is_reversible)
                {
                    fcc_brfs_add_key_to_tree(traversed_states, &new_item->key);
                    fcs_fcc_moves_list_item *moves_list,
                        **end_moves_iter = &moves_list;
                    /* Fill in the moves. */
                    int pos_in_moves = 0;
                    fc_solve__internal__copy_moves(&(extracted_item->moves_seq),
                        &pos_in_moves, &end_moves_iter, extra_move,
                        moves_list_allocator);

                    new_item->moves_seq.count = pos_in_moves;
                    new_item->moves_seq.moves_list = moves_list;

                    /* Enqueue the item in the queue. */
                    new_item->next = NULL;
                    if (queue_tail)
                    {
                        queue_tail = queue_tail->next = new_item;
                    }
                    else
                    {
                        queue_head = queue_tail = new_item;
                    }
                }

                /*
                 * Allocate a new new_item.
                 *
                 * Note: we need to allocate it for both reversibles
                 * and non-reversible moves because pointers to their keys
                 * are kept inside the dict_t-s.
                 * */
                if (queue_recycle_bin)
                {
                    new_item = queue_recycle_bin;
                    queue_recycle_bin = queue_recycle_bin->next;
                }
                else
                {
                    new_item = (fcs_dbm_queue_item *)fcs_compact_alloc_ptr(
                        &(queue_allocator), sizeof(*new_item));
                }
            }

            /* Recycle derived_iter.  */
            next_derived_iter = derived_iter->next;
            derived_iter->next = derived_list_recycle_bin;
            derived_list_recycle_bin = derived_iter;
        }

        /* We always hold one new_item spare, so let's recycle it now. */
        new_item->next = queue_recycle_bin;
        queue_recycle_bin = new_item;

        /* Clean up the extracted_item's resources. We no longer need them
         * because we are interested only in those of the derived items.
         * */
        fc_solve_fcc_release_moves_seq(
            &(extracted_item->moves_seq), moves_list_allocator);
    }

    if ((*is_min_by_sorting_new = (!fc_solve_kaz_tree_lookup_value(
             does_min_by_sorting_exist, &(running_min)))))
    {
        *min_by_sorting = running_min;
        *out_num_new_positions = num_new_positions;
    }

/* Free the allocated resources. */
free_resources:
    fc_solve_compact_allocator_finish(&(queue_allocator));
    fc_solve_compact_allocator_finish(&(derived_list_allocator));
    fc_solve_delta_stater_release(&delta_stater);
    fc_solve_kaz_tree_destroy(traversed_states);
}

typedef struct
{
    fcs_FCC_start_points_list *next_start_points_list;
    dict_t *do_next_fcc_start_points_exist;
    fcs_fcc_moves_seq_allocator *moves_list_allocator;
} add_start_point_context;

/*
 * Returns if already exist (the NOT of if the state is new).
 */
static bool fc_solve_add_start_point_in_mem(
    const fcs_encoded_state_buffer *const enc_state,
    const fcs_fcc_moves_seq *const start_state_moves_seq,
    fcs_fcc_moves_seq *const after_start_moves_seq,
    const unsigned char extra_move, void *const void_context)
{
    add_start_point_context *const context =
        (add_start_point_context *)void_context;
    dict_t *const tree = context->do_next_fcc_start_points_exist;

    if (fc_solve_kaz_tree_lookup_value(tree, enc_state))
    {
        return TRUE;
    }

    fcc_brfs_add_key_to_tree(tree, enc_state);
    /* Fill in the moves. */
    fcs_fcc_moves_list_item *moves_list;
    fcs_fcc_moves_list_item **end_moves_iter = &moves_list;
    int pos_in_moves = 0;
    /*
     * TODO : optimise the loop so the data will be copied
     * in one go by jumps of FCS_FCC_NUM_MOVES_IN_ITEM
     * */
    const fcs_fcc_moves_list_item *start_iter =
        start_state_moves_seq->moves_list;
    const int count_start_state_moves = start_state_moves_seq->count;
    fcs_fcc_moves_seq_allocator *const moves_list_allocator =
        context->moves_list_allocator;
    for (; pos_in_moves < count_start_state_moves;)
    {
        if (pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
        {
            (*end_moves_iter) =
                fc_solve_fcc_alloc_moves_list_item(moves_list_allocator);
        }
        (*end_moves_iter)->data.s[pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM] =
            start_iter->data.s[pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM];
        if ((++pos_in_moves) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
        {
            end_moves_iter = &((*end_moves_iter)->next);
            start_iter = start_iter->next;
        }
    }
    fc_solve__internal__copy_moves(after_start_moves_seq, &pos_in_moves,
        &end_moves_iter, extra_move, moves_list_allocator);

    /* Enqueue the new FCC start point. */
    fcs_FCC_start_points_list *const fcc_start_points =
        context->next_start_points_list;
    fcs_FCC_start_point *new_start_point;
    if (fcc_start_points->recycle_bin)
    {
        new_start_point = fcc_start_points->recycle_bin;
        fcc_start_points->recycle_bin = fcc_start_points->recycle_bin->next;
    }
    else
    {
        new_start_point = (fcs_FCC_start_point *)fcs_compact_alloc_ptr(
            &(fcc_start_points->allocator), sizeof(*new_start_point));
    }
    *new_start_point = (typeof(*new_start_point)){.enc_state = *enc_state,
        .moves_seq = {.count = pos_in_moves, .moves_list = moves_list},
        .next = fcc_start_points->list};

    /*
     * Enqueue the new start point - it won't work without it,
     * retardo! */
    fcc_start_points->list = new_start_point;

    return FALSE;
}

#ifdef __cplusplus
}
#endif
