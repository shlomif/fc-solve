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

#include <assert.h>
#include "config.h"

#include "bool.h"
#include "inline.h"
#include "state.h"
#include "meta_alloc.h"
#include "fcs_enums.h"

#include "fcs_dllexport.h"
#include "dbm_common.h"
#include "dbm_solver_key.h"
#include "dbm_calc_derived_iface.h"
#include "dbm_cache.h"
#include "fcc_brfs_test.h"
#include "dbm_solver.h"
#include "dbm_calc_derived.h"

enum FCC_brfs_scan_type
{
    FIND_FCC_START_POINTS,
    FIND_MIN_BY_SORTING
};


struct fcs_FCC_start_point_struct
{
    fcs_encoded_state_buffer_t enc_state;
    fcs_fcc_moves_seq_t moves_seq;
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
#define GET_PARAM(p) ((const fcs_encoded_state_buffer_t *)p)
    return memcmp(GET_PARAM(void_a), GET_PARAM(void_b), sizeof(*(GET_PARAM(void_a))));
#undef GET_PARAM
}

static GCC_INLINE void fc_solve_fcc_release_moves_seq(
    fcs_fcc_moves_seq_t * moves_seq,
    fcs_fcc_moves_seq_allocator_t * moves_list_allocator
    )
{
    fcs_fcc_moves_list_item_t * iter = moves_seq->moves_list;
    fcs_fcc_moves_list_item_t * iter_next;

    if (iter)
    {
        for(
            iter_next = iter->next
            ;
            iter_next
            ;
            iter = iter_next, iter_next = iter_next->next
           )
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

static void perform_FCC_brfs(
    /* The first state in the game, from which all states are encoded. */
    fcs_state_keyval_pair_t * init_state,
    /* The start state. */
    fcs_encoded_state_buffer_t start_state,
    /* The moves leading up to the state. 
     * */
    const fcs_fcc_moves_seq_t * const start_state_moves_seq,
#if 0
    /* [Output]: FCC start points. 
     * */
    fcs_FCC_start_points_list_t * fcc_start_points,
    /* [Input/Output]: make sure the fcc_start_points don't repeat themselves,
     * in the same FCC-based-depth.
     * */
    dict_t * do_next_fcc_start_points_exist,
#else
    /*
     * [Output] a callback to add a point to the next_start_points,
     * and its context.
     */
    fcs_bool_t (*add_start_point)(
        fcs_encoded_state_buffer_t * enc_state,
        const fcs_fcc_moves_seq_t * start_moves_seq,
        fcs_fcc_moves_seq_t * after_start_moves_seq,
        unsigned char extra_move,
        void * context
    ),
    void * add_start_point_context,
#endif
    /* [Output]: Is the min_by_sorting new.
     * */
    fcs_bool_t * is_min_by_sorting_new,
    /* [Output]: The min_by_sorting.
     * */
    fcs_encoded_state_buffer_t * min_by_sorting,
    /* [Input/Output]: The ${next}_depth_FCCs.DoesExist
     * (for the right depth based on the current depth and pruning.)
     * Of type Map{min_by_sorting => Bool Exists} DoesExist.
    */
    dict_t * does_min_by_sorting_exist,
    /* [Input/Output]: The LRU cache.
     * Of type <<LRU_Map{any_state_in_the_FCCs => Bool Exists} Cache>>
     * */
    fcs_lru_cache_t * does_state_exist_in_any_FCC_cache,
    /* [Output]: the number of new positions/states scanned in the FCC. If it's
     * not new, this will be set to zero (0). It includes the initial position,
     * but does not include the start points of the new FCC.
     * */
    long * out_num_new_positions,
    /* [Input/Output]: the list allocator. */
    fcs_fcc_moves_seq_allocator_t * moves_list_allocator,
    /* [Input/Output]: The meta allocator - needed to allocate and free
     * the compact allocators. */
    fcs_meta_compact_allocator_t * meta_alloc
)
{
    fcs_dbm_queue_item_t * queue_head, * queue_tail, * queue_recycle_bin, * new_item, * extracted_item;
    fcs_compact_allocator_t queue_allocator, derived_list_allocator;
    fcs_derived_state_t * derived_list, * derived_list_recycle_bin,
                        * derived_iter, * next_derived_iter;
    dict_t * traversed_states;
    fc_solve_delta_stater_t * delta_stater;
    fcs_state_keyval_pair_t state;
    fcs_bool_t running_min_was_assigned = FALSE;
    fcs_encoded_state_buffer_t running_min;
    long num_new_positions;

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

    /* Initialize the queue_allocator. */
    fc_solve_compact_allocator_init( &(queue_allocator), meta_alloc );
    fc_solve_compact_allocator_init( &(derived_list_allocator), meta_alloc );
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

    traversed_states = fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL, meta_alloc);
#if 0
    found_new_start_points = fc_solve_kaz_tree_create(fc_solve_compare_encoded_states, NULL);
#endif

    new_item =
        (fcs_dbm_queue_item_t *)
        fcs_compact_alloc_ptr(
            &(queue_allocator),
            sizeof(*new_item)
        );

    new_item->key = start_state;
    new_item->next = NULL;
    new_item->moves_seq.count = 0;
    new_item->moves_seq.moves_list = NULL;

    queue_head = queue_tail = new_item;

    derived_list_recycle_bin = NULL;

    *out_num_new_positions = num_new_positions = 0;

    fc_solve_kaz_tree_alloc_insert(
        traversed_states,
        &(new_item->key)
    );

    /* Extract an item from the queue. */
    while ((extracted_item = queue_head))
    {
        num_new_positions++;
        if (! (queue_head = extracted_item->next) )
        {
            queue_tail = NULL;
        }

        /* Handle the min_by_sorting scan. */
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
            cache_insert(does_state_exist_in_any_FCC_cache, &(extracted_item->key), NULL, '\0');
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

        /* Calculate the derived list. */
        derived_list = NULL;

        /* Handle item. */
        fc_solve_delta_stater_decode_into_state(
            delta_stater,
            extracted_item->key.s,
            &(state),
            indirect_stacks_buffer
        );

#if 0
        char * state_as_str =
                fc_solve_state_as_string(
#ifdef FCS_RCS_STATES
                        &(state.s),
                        &(state.info),
#else
                        &state,
#endif
                        &locs,
                        FREECELLS_NUM,
                        STACKS_NUM,
                        DECKS_NUM,
                        1,
                        0,
                        1
                );

        printf("Checking = <<<\n%s\n>>>\n", state_as_str);
        
        free(state_as_str);
#endif

        instance_solver_thread_calc_derived_states(
            &state,
            &(extracted_item->key),
            &derived_list,
            &derived_list_recycle_bin,
            &derived_list_allocator,
            /* Horne's Prune should be disabled because that interferes
             * with the FCC-depth. */
            FALSE
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
            unsigned char extra_move;
            

            fcs_init_and_encode_state(
                delta_stater,
                &(derived_iter->state),
                &(new_item->key)
            );

            extra_move =
                FCS_PARENT_AND_MOVE__GET_MOVE(derived_iter->parent_and_move);

            if (! 
                (
                    is_reversible
                    ?  (fc_solve_kaz_tree_lookup_value(
                       traversed_states,
                       &(new_item->key)
                       ) != NULL)
                   : add_start_point(
                       &(new_item->key),
                       start_state_moves_seq,
                       &(extracted_item->moves_seq),
                       extra_move,
                       add_start_point_context
                   )
                )
            )
            {
                fcs_fcc_moves_list_item_t * moves_list, * * end_moves_iter;
                int pos_in_moves;
                fcs_encoded_state_buffer_t * key_to_add;

                key_to_add = 
                    fcs_compact_alloc_ptr(
                        &(traversed_states->dict_allocator),
                        sizeof(*key_to_add)
                    );
                *key_to_add = new_item->key;

                if (is_reversible)
                {
                    fc_solve_kaz_tree_alloc_insert(
                        traversed_states,
                        key_to_add
                        );
                }

                /* Fill in the moves. */
                end_moves_iter = &(moves_list);
                pos_in_moves = 0;

                if (is_reversible)
                {
                    int copy_from_idx;
                    fcs_fcc_moves_list_item_t const * copy_from_iter =
                        extracted_item->moves_seq.moves_list;

                    for(
                        copy_from_idx = 0
                        ;
                        copy_from_idx < extracted_item->moves_seq.count
                        ;
                       )
                    {
                        if (pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
                        {
                            (*end_moves_iter) = fc_solve_fcc_alloc_moves_list_item(moves_list_allocator);
                        }
                        (*end_moves_iter)->data.s[
                            pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM
                            ] = copy_from_iter->data.s[
                            copy_from_idx % FCS_FCC_NUM_MOVES_IN_ITEM
                            ];
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
                        (*end_moves_iter) = fc_solve_fcc_alloc_moves_list_item(moves_list_allocator);
                    }
                    (*end_moves_iter)->data.s[
                        (pos_in_moves++) % FCS_FCC_NUM_MOVES_IN_ITEM
                    ] = extra_move;

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
                 * and non-reversible moves because pointers to their keys are
                 * kept inside the dict_t-s.
                 * */
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
            &(extracted_item->moves_seq), 
            moves_list_allocator
        );
    }

    if ((*is_min_by_sorting_new =
        (!fc_solve_kaz_tree_lookup_value(
            does_min_by_sorting_exist,
            &(running_min))
         )
        )
    )
    {
        *min_by_sorting = running_min;
        *out_num_new_positions = num_new_positions;
    }

    /* Free the allocated resources. */
free_resources:
    fc_solve_compact_allocator_finish(&(queue_allocator));
    fc_solve_compact_allocator_finish(&(derived_list_allocator));
    fc_solve_delta_stater_free(delta_stater);
    fc_solve_kaz_tree_destroy(traversed_states);
#if 0
    fc_solve_kaz_tree_destroy(found_new_start_points);
#endif
}

typedef struct
{
    fcs_FCC_start_points_list_t * next_start_points_list;
    dict_t * do_next_fcc_start_points_exist;
    fcs_fcc_moves_seq_allocator_t * moves_list_allocator;
} add_start_point_context_t;

/*
 * Returns if already exist (the NOT of if the state is new).
 */
static fcs_bool_t fc_solve_add_start_point_in_mem(
    fcs_encoded_state_buffer_t * enc_state,
    const fcs_fcc_moves_seq_t * start_state_moves_seq,
    fcs_fcc_moves_seq_t * after_start_moves_seq,
    unsigned char extra_move,
    void * void_context
)
{
    add_start_point_context_t * context = (add_start_point_context_t *)void_context;
    dict_t * tree = context->do_next_fcc_start_points_exist;
    const fcs_fcc_moves_list_item_t * const start_state_moves_item = start_state_moves_seq->moves_list;
    int count_start_state_moves = start_state_moves_seq->count;
    fcs_encoded_state_buffer_t * key_to_add;
    fcs_fcc_moves_list_item_t const * start_iter;
    int pos_in_moves;
    fcs_fcc_moves_list_item_t * moves_list, * * end_moves_iter;
    fcs_fcc_moves_seq_allocator_t * moves_list_allocator = context->moves_list_allocator;
    fcs_FCC_start_point_t * new_start_point;
    fcs_FCC_start_points_list_t * fcc_start_points = context->next_start_points_list;

    if (fc_solve_kaz_tree_lookup_value(
            tree,
            enc_state
            )
       )
    {
        return TRUE;
    }

    key_to_add = 
        fcs_compact_alloc_ptr(
            &(tree->dict_allocator),
            sizeof(*key_to_add)
            );
    *key_to_add = *enc_state;
    fc_solve_kaz_tree_alloc_insert(
        tree,
        key_to_add
        );
    /* Fill in the moves. */
    end_moves_iter = &(moves_list);
    pos_in_moves = 0;
    /*
     * TODO : optimise the loop so the data will be copied
     * in one go by jumps of FCS_FCC_NUM_MOVES_IN_ITEM
     * */
    start_iter = start_state_moves_item;
    for(
        ;
        pos_in_moves < count_start_state_moves
        ;
       )
    {
        if (pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
        {
            (*end_moves_iter) = fc_solve_fcc_alloc_moves_list_item(moves_list_allocator);
        }
        (*end_moves_iter)->data.s[
            pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM
            ] = start_iter->data.s[
            pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM
            ];
        if ((++pos_in_moves) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
        {
            end_moves_iter = &((*end_moves_iter)->next);
            start_iter = start_iter->next;
        }
    }
    {
        int copy_from_idx;
        fcs_fcc_moves_list_item_t const * copy_from_iter = after_start_moves_seq->moves_list;

        for(
            copy_from_idx = 0
            ;
            copy_from_idx < after_start_moves_seq->count
            ;
           )
        {
            if (pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                (*end_moves_iter) = fc_solve_fcc_alloc_moves_list_item(moves_list_allocator);
            }
            (*end_moves_iter)->data.s[
                pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM
                ] = copy_from_iter->data.s[
                copy_from_idx % FCS_FCC_NUM_MOVES_IN_ITEM
                ];
            if ((++pos_in_moves) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                end_moves_iter = &((*end_moves_iter)->next);
            }
            if ((++copy_from_idx) % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
            {
                copy_from_iter = copy_from_iter->next;
            }
        }
    }
    /* Append the remaining moves. */
    if (pos_in_moves % FCS_FCC_NUM_MOVES_IN_ITEM == 0)
    {
        (*end_moves_iter) = fc_solve_fcc_alloc_moves_list_item(moves_list_allocator);
    }
    (*end_moves_iter)->data.s[
        (pos_in_moves++) % FCS_FCC_NUM_MOVES_IN_ITEM
        ] = extra_move;
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
    new_start_point->enc_state = *enc_state;
    new_start_point->moves_seq.count = pos_in_moves;
    new_start_point->moves_seq.moves_list = moves_list;

    /* 
     * Enqueue the new start point - it won't work without it,
     * retardo! */
    new_start_point->next = fcc_start_points->list;
    fcc_start_points->list = new_start_point;

    return FALSE;
}

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_FCC_BRFS_H */
