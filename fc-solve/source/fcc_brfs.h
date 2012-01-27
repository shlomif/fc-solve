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
 * fcc_brfs.h - contains the perform_FCC_brfs() function to perform the
 * Breadth-first-search on the Fully-connected-component.
 *
 */
#ifndef FC_SOLVE_FCC_BRFS_H
#define FC_SOLVE_FCC_BRFS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "config.h"

#include "bool.h"
#include "inline.h"
#include "state.h"
#include "alloc.h"
#include "fcs_enums.h"

#include "fcs_dllexport.h"
#include "dbm_common.h"
#include "dbm_solver_key.h"
#include "dbm_calc_derived_iface.h"
#include "dbm_cache.h"

enum FCC_brfs_scan_type
{
    FIND_FCC_START_POINTS,
    FIND_MIN_BY_SORTING
};


struct fcs_FCC_start_point_struct
{
    fcs_encoded_state_buffer_t enc_state;
    int count_moves;
    /* Dynamically allocated. */
    fcs_fcc_move_t * moves;
    struct fcs_FCC_start_point_struct * next;
};

typedef struct fcs_FCC_start_point_struct fcs_FCC_start_point_t;

typedef struct {
    fcs_FCC_start_point_t * list;
    fcs_compact_allocator_t allocator;
    fcs_FCC_start_point_t * recycle_bin;
} fcs_FCC_start_points_list_t;

static int fc_solve_compare_encoded_states(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((fcs_encoded_state_buffer_t *)p)
    return memcmp(GET_PARAM(void_a), GET_PARAM(void_b), sizeof(*(GET_PARAM(void_a))));
#undef GET_PARAM
}

static void perform_FCC_brfs(
    /* The first state in the game, from which all states are encoded. */
    fcs_state_keyval_pair_t * init_state,
    /* The start state. */
    fcs_encoded_state_buffer_t start_state,
    /* Type of scan. */
    enum FCC_brfs_scan_type scan_type,
    /* The moves leading up to the state. 
     * Used only when scan_type is FIND_FCC_START_POINTS .
     * */
    const int count_start_state_moves,
    const fcs_fcc_move_t * const start_state_moves,
    /* [Output]: FCC start points. 
     * Will be NULL if scan_type is not FIND_FCC_START_POINTS
     * */
    fcs_FCC_start_points_list_t * fcc_start_points,
    /* [Output]: Is the min_by_sorting new.
     * Will be NULL if scan_type is not FIND_MIN_BY_SORTING
     * */
    fcs_bool_t * is_min_by_sorting_new,
    /* [Output]: The min_by_sorting.
     * Will be NULL if scan_type is not FIND_MIN_BY_SORTING
     * */
    fcs_encoded_state_buffer_t * min_by_sorting,
    /* [Input/Output]: The ${next}_depth_FCCs.DoesExist
     * (for the right depth based on the current depth and pruning.)
     * Of type Map{min_by_sorting => Bool Exists} DoesExist.
     * Will be NULL if scan_type is not FIND_MIN_BY_SORTING
    */
    dict_t * does_min_by_sorting_exist,
    /* [Input/Output]: The LRU.
     * Of type <<LRU_Map{any_state_in_the_FCCs => Bool Exists} Cache>>
     * Will be NULL if scan_type is not FIND_MIN_BY_SORTING
     * */
    fcs_lru_cache_t * does_state_exist_in_any_FCC_cache
)
{
    fcs_dbm_queue_item_t * queue_head, * queue_tail, * queue_recycle_bin, * new_item, * extracted_item;
    fcs_compact_allocator_t queue_allocator, derived_list_allocator;
    fcs_derived_state_t * derived_list, * derived_list_recycle_bin,
                        * derived_iter, * next_derived_iter;
    dict_t * traversed_states, * found_new_start_points;
    fc_solve_bit_reader_t bit_r;
    fc_solve_bit_writer_t bit_w;
    fc_solve_delta_stater_t * delta_stater;
    fcs_state_keyval_pair_t state;
    fcs_bool_t running_min_was_assigned = FALSE;
    fcs_encoded_state_buffer_t running_min;

#ifdef INDIRECT_STACK_STATES
    dll_ind_buf_t indirect_stacks_buffer;
#endif

    /* Some sanity checks. */
#ifndef NDEBUG
    if (scan_type == FIND_FCC_START_POINTS)
    {
        assert(fcc_start_points);
        assert(! is_min_by_sorting_new);
        assert(! min_by_sorting);
        assert(! does_min_by_sorting_exist);
        assert(! does_state_exist_in_any_FCC_cache);
    }
    else
    {
        assert(! fcc_start_points);
        assert(is_min_by_sorting_new);
        assert(min_by_sorting);
        assert(does_min_by_sorting_exist);
        assert(does_state_exist_in_any_FCC_cache);
    }
#endif

    /* Initialize the queue_allocator. */
    fc_solve_compact_allocator_init( &(queue_allocator) );
    fc_solve_compact_allocator_init( &(derived_list_allocator) );
    queue_recycle_bin = NULL;

    /* TODO : maybe pass delta_stater as an argument  */
    delta_stater = fc_solve_delta_stater_alloc(
        &(init_state->s),
        STACKS_NUM,
        FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
        , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
    );

    traversed_states = fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL);
    found_new_start_points = fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL);

    new_item =
        (fcs_dbm_queue_item_t *)
        fcs_compact_alloc_ptr(
            &(queue_allocator),
            sizeof(*new_item)
        );

    new_item->key = start_state;
    new_item->next = NULL;

    queue_head = queue_tail = new_item;

    derived_list_recycle_bin = NULL;

    /* Extract an item from the queue. */
    while ((extracted_item = queue_head))
    {
        if (! (queue_head = extracted_item->next) )
        {
            queue_tail = NULL;
        }

        /* Handle the min_by_sorting scan. */
        if (scan_type == FIND_MIN_BY_SORTING)
        {
            if (cache_does_key_exist(
                does_state_exist_in_any_FCC_cache, 
                &(extracted_item->key)
            ))
            {
                *is_min_by_sorting_new = FALSE;
                goto free_resources;
            }
            else
            {
                cache_insert(does_state_exist_in_any_FCC_cache, &(extracted_item->key));
            }

            if (! running_min_was_assigned)
            {
                running_min_was_assigned = TRUE;
                running_min = extracted_item->key;
            }
            else
            {
                if (memcmp(&(extracted_item->key), &running_min, sizeof(running_min)) < 0)
                {
                    running_min = extracted_item->key;
                }
            }
        }
        /* Calculate the derived list. */
        derived_list = NULL;

        /* Handle item. */
        fc_solve_bit_reader_init(&bit_r, extracted_item->key.s + 1);

        fc_solve_state_init(&state, STACKS_NUM
#ifdef INDIRECT_STACK_STATES
            , indirect_stacks_buffer
#endif
        );

        fc_solve_delta_stater_decode(
            delta_stater,
            &bit_r,
            &(state.s)
        );

        instance_solver_thread_calc_derived_states(
            &state,
            &(extracted_item->key),
            &derived_list,
            &derived_list_recycle_bin,
            &derived_list_allocator,
            TRUE
        );


        /* Allocate a spare 'new_item'. */
        if (queue_recycle_bin)
        {
            new_item = queue_recycle_bin;
            queue_recycle_bin = queue_recycle_bin->next;
        }
        else
        {
            new_item =
                (fcs_dbm_queue_item_t *)
                fcs_compact_alloc_ptr(
                    &(queue_allocator),
                    sizeof(*new_item)
                );
        }

        /* Enqueue the new elements to the queue. */
        for (derived_iter = derived_list;
                derived_iter ;
                derived_iter = next_derived_iter
        )
        {
            fcs_bool_t is_reversible = derived_iter->is_reversible_move;
            if (is_reversible || (scan_type == FIND_FCC_START_POINTS))
            {
                dict_t * right_tree;

                right_tree = (is_reversible ? traversed_states : found_new_start_points);

                memset(&(new_item->key), '\0', sizeof(new_item->key));
                fc_solve_bit_writer_init(&bit_w, new_item->key.s+1);
                fc_solve_delta_stater_set_derived(
                        delta_stater, &(derived_iter->state.s)
                        );
                fc_solve_delta_stater_encode_composite(delta_stater, &bit_w);
                new_item->key.s[0] =
                    bit_w.current - bit_w.start + (bit_w.bit_in_char_idx > 0)
                    ;
                if (! fc_solve_kaz_tree_lookup(
                    right_tree,
                    &(new_item->key)
                    )
                )
                {
                    fc_solve_kaz_tree_alloc_insert(
                        right_tree,
                        &(new_item->key)
                    );
                    if (scan_type == FIND_FCC_START_POINTS)
                    {
                        int count_moves;
                        fcs_fcc_move_t * moves, * end_moves;

                        count_moves = (extracted_item->count_moves + 1);

                        if (! is_reversible)
                        {
                            count_moves += count_start_state_moves;
                        }

                        /* Fill in the moves. */
                        moves = malloc(
                            sizeof(new_item->moves[0]) * count_moves
                        );

                        if (is_reversible)
                        {
                            end_moves = moves;
                        }
                        else
                        {
                            memcpy(moves, start_state_moves, count_start_state_moves);
                            end_moves = moves + count_start_state_moves;
                        }

                        memcpy(end_moves, extracted_item->moves, extracted_item->count_moves * sizeof(new_item->moves[0]));
                        end_moves[extracted_item->count_moves]
                            = derived_iter->parent_and_move.s[
                            derived_iter->parent_and_move.s[0]
                            ];

                        if (is_reversible)
                        {
                            new_item->moves = moves;
                            new_item->count_moves = count_moves;
                        }
                        else
                        {
                            fcs_FCC_start_point_t * new_start_point;
                            /* Enqueue the new FCC start point. */
                            if (fcc_start_points->recycle_bin)
                            {
                                new_start_point = fcc_start_points->recycle_bin;
                                fcc_start_points->recycle_bin = fcc_start_points->recycle_bin->next;
                            }
                            else
                            {
                                new_start_point = (fcs_FCC_start_point_t *)
                                    fcs_compact_alloc_ptr(
                                        &(fcc_start_points->allocator),
                                        sizeof (*new_start_point)
                                    );
                            }
                            new_start_point->enc_state = new_item->key;
                            new_start_point->count_moves = count_moves;
                            new_start_point->moves = moves;
                        }
                    }

                    if (is_reversible)
                    {
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
                    /* Allocate a new new_item */
                    if (queue_recycle_bin)
                    {
                        new_item = queue_recycle_bin;
                        queue_recycle_bin = queue_recycle_bin->next;
                    }
                    else
                    {
                        new_item =
                            (fcs_dbm_queue_item_t *)
                            fcs_compact_alloc_ptr(
                                &(queue_allocator),
                                sizeof(*new_item)
                            );
                    }
                }
            }
            /* Recycle derived_iter.  */
            next_derived_iter = derived_iter->next;
            derived_iter->next = derived_list_recycle_bin;
            derived_list_recycle_bin = derived_iter;
        }

        /* We always hold one new_item spare, so let's recycle i now. */
        new_item->next = queue_recycle_bin;
        queue_recycle_bin = new_item;

        /* Clean up the extracted_item's resources. We no longer need them
         * because we are interested only in those of the derived items.
         * */
        free(extracted_item->moves);
        extracted_item->moves = NULL;
    }

    if (scan_type == FIND_MIN_BY_SORTING)
    {
        if ((*is_min_by_sorting_new =
            (!fc_solve_kaz_tree_lookup(
                does_min_by_sorting_exist,
                &(running_min))
             )
            )
        )
        {
            *min_by_sorting = running_min;
        }
    }

    /* Free the allocated resources. */
free_resources:
    fc_solve_compact_allocator_finish(&(queue_allocator));
    fc_solve_compact_allocator_finish(&(derived_list_allocator));
    fc_solve_delta_stater_free(delta_stater);
    fc_solve_kaz_tree_destroy(traversed_states);
    fc_solve_kaz_tree_destroy(found_new_start_points);
}

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_FCC_BRFS_H */
