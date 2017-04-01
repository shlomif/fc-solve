/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * instance.c - instance/hard_thread/soft_thread functions for
 * Freecell Solver.
 */

#include "dll_thunk.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

#include <sys/types.h>
#include <limits.h>

#define NUM_CHECKED_STATES_STEP 50

#include "config.h"

#include "instance_for_lib.h"
#include "instance.h"
#include "move_funcs_order.h"
#include "preset.h"
#include "scans.h"
#include "state.h"

/*
    General use of this interface:
    1. fc_solve_alloc_instance()
    2. Set the parameters of the game
    3. If you wish to revert, go to step #11.
    4. fc_solve_init_instance()
    5. Call fc_solve_solve_instance() with the initial board.
    6. If it returns FCS_STATE_SUSPEND_PROCESS and you wish to proceed,
       then increase the iteration limit and call
       fc_solve_resume_instance().
    7. Repeat Step #6 zero or more times.
    8. If the solving was successful you can use the move stacks or the
       intermediate stacks. (Just don't destroy them in any way).
    9. Call fc_solve_finish_instance().
    10. Call fc_solve_free_instance().

    The library functions inside lib.c (a.k.a fcs_user()) give an
    easier approach for embedding Freecell Solver into your library. The
    intent of this comment is to document the code, rather than to be
    a guideline for the user.
*/

const fcs_default_weights_t fc_solve_default_befs_weights = {
    .weights = {0.5, 0, 0.3, 0, 0.2, 0}};

static inline void soft_thread_clean_soft_dfs(
    fc_solve_soft_thread_t *const soft_thread)
{
    fcs_soft_dfs_stack_item_t *const soft_dfs_info =
        DFS_VAR(soft_thread, soft_dfs_info);
    /* Check if a Soft-DFS-type scan was called in the first place */
    if (!soft_dfs_info)
    {
        /* If not - do nothing */
        return;
    }

    /* De-allocate the Soft-DFS specific stacks */
    const fcs_soft_dfs_stack_item_t *info_ptr = soft_dfs_info;
    const fcs_soft_dfs_stack_item_t *const max_info_ptr =
        info_ptr + DFS_VAR(soft_thread, depth);
    const fcs_soft_dfs_stack_item_t *const dfs_max_info_ptr =
        info_ptr + DFS_VAR(soft_thread, dfs_max_depth);

    for (; info_ptr < max_info_ptr; info_ptr++)
    {
        free(info_ptr->derived_states_list.states);
        free(info_ptr->derived_states_random_indexes);
    }
    for (; info_ptr < dfs_max_info_ptr; info_ptr++)
    {
        if (likely(info_ptr->derived_states_list.states))
        {
            free(info_ptr->derived_states_list.states);
            free(info_ptr->derived_states_random_indexes);
        }
    }

    free(soft_dfs_info);

    DFS_VAR(soft_thread, soft_dfs_info) = NULL;

    DFS_VAR(soft_thread, dfs_max_depth) = 0;
}

extern void fc_solve_free_soft_thread_by_depth_test_array(
    fc_solve_soft_thread_t *const soft_thread)
{
    const_AUTO(
        by_depth_tests, soft_thread->by_depth_tests_order.by_depth_tests);
    const_AUTO(num, soft_thread->by_depth_tests_order.num);
    for (size_t depth_idx = 0; depth_idx < num; depth_idx++)
    {
        fc_solve_free_tests_order(&(by_depth_tests[depth_idx].tests_order));
    }

    soft_thread->by_depth_tests_order.num = 0;

    free(by_depth_tests);
    soft_thread->by_depth_tests_order.by_depth_tests = NULL;
}

static inline void accumulate_tests_by_ptr(
    size_t *const tests_order, fcs_tests_order_t *const st_tests_order)
{
    const fcs_tests_order_group_t *group_ptr = st_tests_order->groups;
    const fcs_tests_order_group_t *const groups_end =
        group_ptr + st_tests_order->num_groups;
    for (; group_ptr < groups_end; group_ptr++)
    {
        const size_t *test_ptr = group_ptr->order_group_tests;
        const size_t *const tests_end = test_ptr + group_ptr->num;
        for (; test_ptr < tests_end; test_ptr++)
        {
            *tests_order |= (1 << (*test_ptr));
        }
    }
}

static inline void accumulate_tests_order(
    fc_solve_soft_thread_t *const soft_thread, void *const context)
{
    accumulate_tests_by_ptr((size_t *)context,
        &(soft_thread->by_depth_tests_order.by_depth_tests[0].tests_order));
}

static inline void determine_scan_completeness(
    fc_solve_soft_thread_t *const soft_thread, void *const global_tests_order)
{
    size_t tests_order = 0;

    accumulate_tests_by_ptr(&tests_order,
        &(soft_thread->by_depth_tests_order.by_depth_tests[0].tests_order));

    STRUCT_SET_FLAG_TO(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN,
        (tests_order == *(size_t *)global_tests_order));
}

void fc_solve_foreach_soft_thread(fc_solve_instance_t *const instance,
    const fcs_foreach_st_callback_choice_t callback_choice, void *const context)
{
#ifdef FCS_SINGLE_HARD_THREAD
    const int num_soft_threads = instance->hard_thread.num_soft_threads;
    for (int st_idx = 0; st_idx <= num_soft_threads; st_idx++)
    {
        fc_solve_soft_thread_t *soft_thread;
        if (st_idx < num_soft_threads)
        {
            soft_thread = &(instance->hard_thread.soft_threads[st_idx]);
        }
#ifdef FCS_WITH_MOVES
        else if (instance->is_optimization_st)
        {
            soft_thread = &(instance->optimization_soft_thread);
        }
#endif
        else
        {
            break;
        }
#else
    for (uint_fast32_t ht_idx = 0; ht_idx <= instance->num_hard_threads;
         ht_idx++)
    {
        fc_solve_hard_thread_t *hard_thread;
        if (ht_idx < instance->num_hard_threads)
        {
            hard_thread = &(instance->hard_threads[ht_idx]);
        }
#ifdef FCS_WITH_MOVES
        else if (instance->optimization_thread)
        {
            hard_thread = instance->optimization_thread;
        }
#endif
        else
        {
            break;
        }
        ST_LOOP_START()
#endif

        {
            switch (callback_choice)
            {
            case FOREACH_SOFT_THREAD_CLEAN_SOFT_DFS:
                soft_thread_clean_soft_dfs(soft_thread);
                break;

            case FOREACH_SOFT_THREAD_FREE_INSTANCE:
                fc_solve_free_instance_soft_thread_callback(soft_thread);
                break;

            case FOREACH_SOFT_THREAD_ACCUM_TESTS_ORDER:
                accumulate_tests_order(soft_thread, context);
                break;

            case FOREACH_SOFT_THREAD_DETERMINE_SCAN_COMPLETENESS:
                determine_scan_completeness(soft_thread, context);
                break;
            }
        }
#ifdef FCS_SINGLE_HARD_THREAD
    }
#else
    }
#endif
}

static inline void clean_soft_dfs(fc_solve_instance_t *const instance)
{
    fc_solve_foreach_soft_thread(
        instance, FOREACH_SOFT_THREAD_CLEAN_SOFT_DFS, NULL);
}

#ifndef FCS_SINGLE_HARD_THREAD
static inline
#endif
    void
    fc_solve_init_soft_thread(fc_solve_hard_thread_t *const hard_thread,
        fc_solve_soft_thread_t *const soft_thread)
{
    *soft_thread = (fc_solve_soft_thread_t){
        .hard_thread = hard_thread,
        .id = (HT_INSTANCE(hard_thread)->next_soft_thread_id)++,
        .method_specific =
            {
                .soft_dfs =
                    {
                        .dfs_max_depth = 0,
                        .soft_dfs_info = NULL,
                        .depth = 0,
                        .tests_by_depth_array =
                            {
                                .num_units = 0, .by_depth_units = NULL,
                            },
                        .rand_seed = 24,
                    },
                .befs =
                    {
                        .tests_list = NULL,
                        .meth =
                            {
                                .befs =
                                    {
                                        .weighting =
                                            {
                                                .befs_weights =
                                                    fc_solve_default_befs_weights,
                                            },
                                    },
                                .brfs =
                                    {
                                        .bfs_queue_last_item = NULL,
                                        .bfs_queue = NULL,
                                    },
                            },
                    },
            },
        .by_depth_tests_order =
            {
                .num = 1,
                .by_depth_tests =
                    SMALLOC1(soft_thread->by_depth_tests_order.by_depth_tests),
            },
        .is_befs = FALSE,
#ifdef FCS_WITH_MOVES
        .is_optimize_scan = FALSE,
#endif
        .super_method_type = FCS_SUPER_METHOD_DFS,
        .master_to_randomize = FALSE,
        .checked_states_step = NUM_CHECKED_STATES_STEP,
        .name = "",
        .enable_pruning = FALSE,
#ifndef FCS_DISABLE_PATSOLVE
        .pats_scan = NULL,
#endif
    };
    soft_thread->by_depth_tests_order.by_depth_tests[0] =
        (typeof(soft_thread->by_depth_tests_order.by_depth_tests[0])){
            .max_depth = SSIZE_MAX,
            .tests_order = tests_order_dup(
                &(fcs_st_instance(soft_thread)->instance_tests_order)),
        };

    fc_solve_reset_soft_thread(soft_thread);
}

void fc_solve_instance__init_hard_thread(
#ifndef FCS_SINGLE_HARD_THREAD
    fc_solve_instance_t *const instance,
#endif
    fc_solve_hard_thread_t *const hard_thread)
{
#ifndef FCS_SINGLE_HARD_THREAD
    hard_thread->instance = instance;
#endif

    HT_FIELD(hard_thread, num_soft_threads) = 0;

    HT_FIELD(hard_thread, soft_threads) = NULL;

    fc_solve_new_soft_thread(hard_thread);

    HT_FIELD(hard_thread, prelude_as_string) = NULL;
    HT_FIELD(hard_thread, prelude) = NULL;
    HT_FIELD(hard_thread, prelude_num_items) = 0;
    HT_FIELD(hard_thread, prelude_idx) = 0;

    fc_solve_reset_hard_thread(hard_thread);
    fc_solve_compact_allocator_init(&(HT_FIELD(hard_thread, allocator)),
#ifdef FCS_SINGLE_HARD_THREAD
        hard_thread->meta_alloc
#else
        instance->meta_alloc
#endif
        );

#ifdef FCS_WITH_MOVES
    HT_FIELD(hard_thread, reusable_move_stack) = fcs_move_stack__new();
#endif
}

/* These are all stack comparison functions to be used for the stacks
   cache when using INDIRECT_STACK_STATES
*/
#if defined(INDIRECT_STACK_STATES)

#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
/* A hash calculation function for use in glib's hash */
static guint fc_solve_glib_hash_stack_hash_function(gconstpointer key)
{
    guint hash_value_int;
    /* Calculate the hash value for the stack */
    /* This hash function was ripped from the Perl source code.
     * (It is not derived work however). */
    const char *s_ptr = (char *)key;
    const char *const s_end = s_ptr + fcs_col_len((fcs_card_t *)key) + 1;
    hash_value_int = 0;
    while (s_ptr < s_end)
    {
        hash_value_int += (hash_value_int << 5) + *(s_ptr++);
    }
    hash_value_int += (hash_value_int >> 5);

    return hash_value_int;
}

static gint fc_solve_glib_hash_stack_compare(gconstpointer a, gconstpointer b)
{
    return !(fc_solve_stack_compare_for_comparison(a, b));
}
#endif /* (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH) */

#endif /* defined(INDIRECT_STACK_STATES) */

typedef struct
{
    int idx;
    enum
    {
        FREECELL,
        COLUMN
    } type;
} find_card_ret_t;

static inline int find_empty_col(
    const fcs_state_t *const dynamic_state STACKS_NUM__ARG)
{
    for (int i = 0; i < STACKS_NUM__VAL; i++)
    {
        if (!fcs_col_len(fcs_state_get_col(*dynamic_state, i)))
        {
            return i;
        }
    }

    return -1;
}

static inline int find_col_card(const fcs_state_t *const dynamic_state,
    const fcs_card_t needle STACKS_NUM__ARG)
{
    for (int i = 0; i < STACKS_NUM__VAL; i++)
    {
        fcs_const_cards_column_t col = fcs_state_get_col(*dynamic_state, i);
        const int col_len = fcs_col_len(col);
        if (col_len > 0 && (fcs_col_get_card(col, col_len - 1) == needle))
        {
            return i;
        }
    }

    return -1;
}

static inline int find_fc_card(const fcs_state_t *const dynamic_state,
    const fcs_card_t needle FREECELLS_NUM__ARG)
{
    for (int dest = 0; dest < FREECELLS_NUM__VAL; dest++)
    {
        if (fcs_freecell_card(*dynamic_state, dest) == needle)
        {
            return dest;
        }
    }

    return -1;
}

static inline find_card_ret_t find_card_src_string(
    const fcs_state_t *const dynamic_state,
    const fcs_card_t needle FREECELLS_AND_STACKS_ARGS())
{
    const int src_col_idx =
        find_col_card(dynamic_state, needle PASS_STACKS(STACKS_NUM__VAL));
    if (src_col_idx < 0)
    {
        return (find_card_ret_t){
            .idx = (find_fc_card(
                dynamic_state, needle PASS_FREECELLS(FREECELLS_NUM__VAL))),
            .type = FREECELL};
    }
    else
    {
        return (find_card_ret_t){.idx = src_col_idx, .type = COLUMN};
    }
}

/*
 * This function traces the solution from the final state down
 * to the initial state
 * */
#ifdef FCS_WITH_MOVES
extern void fc_solve_trace_solution(fc_solve_instance_t *const instance)
{
    fcs_internal_move_t canonize_move = fc_solve_empty_move;
    fcs_int_move_set_type(canonize_move, FCS_MOVE_TYPE_CANONIZE);

    instance_free_solution_moves(instance);
    instance->solution_moves = fcs_move_stack__new();

    const_AUTO(solution_moves_ptr, &(instance->solution_moves));
/*
 * Handle the case if it's patsolve.
 * */
#ifndef FCS_DISABLE_PATSOLVE
    const_SLOT(solving_soft_thread, instance);
    if (solving_soft_thread->super_method_type == FCS_SUPER_METHOD_PATSOLVE)
    {
        fcs_state_locs_struct_t locs;
        fc_solve_init_locs(&(locs));
        const_SLOT(pats_scan, solving_soft_thread);
        var_AUTO(num_moves, pats_scan->num_moves_to_win);

        fcs_state_keyval_pair_t s_and_info;

        DECLARE_IND_BUF_T(indirect_stacks_buffer)
        FCS_STATE__DUP_keyval_pair(s_and_info, *(instance->state_copy_ptr));

#ifndef HARD_CODED_NUM_STACKS
        const int stacks_num = INSTANCE_STACKS_NUM;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
        const int freecells_num = INSTANCE_FREECELLS_NUM;
#endif

        fcs_state_t *const s = &(s_and_info.s);
#ifdef INDIRECT_STACK_STATES
        for (int i = 0; i < STACKS_NUM__VAL; i++)
        {
            fcs_copy_stack(
                s_and_info.s, s_and_info.info, i, indirect_stacks_buffer);
        }
#endif

        solution_moves_ptr->num_moves = num_moves;
        solution_moves_ptr->moves =
            SREALLOC(solution_moves_ptr->moves, num_moves);
        var_AUTO(mp, pats_scan->moves_to_win);
        for (int i = 0; i < num_moves; i++, mp++)
        {
            const fcs_card_t card = mp->card;
            fcs_internal_move_t out_move = fc_solve_empty_move;
            switch (mp->totype)
            {
            case FCS_PATS__TYPE_FREECELL:
            {
                int src_col_idx;
                for (src_col_idx = 0; src_col_idx < STACKS_NUM__VAL;
                     src_col_idx++)
                {
                    fcs_cards_column_t src_col =
                        fcs_state_get_col(s_and_info.s, src_col_idx);
                    const int src_cards_num = fcs_col_len(src_col);
                    if (src_cards_num)
                    {
                        if (card ==
                            fcs_col_get_card(src_col, src_cards_num - 1))
                        {
                            break;
                        }
                    }
                }

                int dest;
                for (dest = 0; dest < FREECELLS_NUM__VAL; dest++)
                {
                    if (fcs_freecell_is_empty(s_and_info.s, dest))
                    {
                        break;
                    }
                }
                fcs_int_move_set_type(
                    out_move, FCS_MOVE_TYPE_STACK_TO_FREECELL);
                fcs_int_move_set_src_stack(out_move, src_col_idx);
                fcs_int_move_set_dest_freecell(out_move, dest);
            }
            break;
            case FCS_PATS__TYPE_FOUNDATION:
            {
                const find_card_ret_t src = find_card_src_string(
                    &(s_and_info.s), card PASS_FREECELLS(FREECELLS_NUM__VAL)
                                         PASS_STACKS(STACKS_NUM__VAL));
                if (src.type == FREECELL)
                {
                    fcs_int_move_set_type(
                        out_move, FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION);
                    fcs_int_move_set_src_freecell(out_move, src.idx);
                }
                else
                {
                    fcs_int_move_set_type(
                        out_move, FCS_MOVE_TYPE_STACK_TO_FOUNDATION);
                    fcs_int_move_set_src_stack(out_move, src.idx);
                }
                fcs_int_move_set_foundation(out_move, fcs_card_suit(card));
            }
            break;
            default:
            {
                const fcs_card_t dest_card = mp->destcard;
                const find_card_ret_t src = find_card_src_string(s,
                    card PASS_FREECELLS(FREECELLS_NUM__VAL)
                        PASS_STACKS(STACKS_NUM__VAL));
                if (src.type == FREECELL)
                {
                    fcs_int_move_set_type(
                        out_move, FCS_MOVE_TYPE_FREECELL_TO_STACK);
                    fcs_int_move_set_src_freecell(out_move, src.idx);
                }
                else
                {
                    fcs_int_move_set_type(
                        out_move, FCS_MOVE_TYPE_STACK_TO_STACK);
                    fcs_int_move_set_src_stack(out_move, src.idx);
                    fcs_int_move_set_num_cards_in_seq(out_move, 1);
                }
                fcs_int_move_set_dest_stack(out_move,
                    ((dest_card == fc_solve_empty_card)
                            ? find_empty_col(s PASS_STACKS(STACKS_NUM__VAL))
                            : find_col_card(
                                  s, dest_card PASS_STACKS(STACKS_NUM__VAL))));
            }
            break;
            }

            fc_solve_apply_move(&(s_and_info.s), &locs,
                out_move PASS_FREECELLS(FREECELLS_NUM__VAL)
                    PASS_STACKS(STACKS_NUM__VAL));
            solution_moves_ptr->moves[num_moves - 1 - i] = out_move;
        }
    }
    else
#endif
    {
        fcs_collectible_state_t *s1 = instance->final_state;

        /* Retrace the step from the current state to its parents */
        while (FCS_S_PARENT(s1) != NULL)
        {
            /* Mark the state as part of the non-optimized solution */
            FCS_S_VISITED(s1) |= FCS_VISITED_IN_SOLUTION_PATH;

            /* Each state->parent_state stack has an implicit CANONIZE
             * move. */
            fcs_move_stack_push(solution_moves_ptr, canonize_move);

            /* Merge the move stack */
            {
                const fcs_move_stack_t *const stack = FCS_S_MOVES_TO_PARENT(s1);
                const fcs_internal_move_t *const moves = stack->moves;
                for (int move_idx = stack->num_moves - 1; move_idx >= 0;
                     move_idx--)
                {
                    fcs_move_stack_push(solution_moves_ptr, moves[move_idx]);
                }
            }
            /* Duplicate the state to a freshly malloced memory */

            /* Move to the parent state */
            s1 = FCS_S_PARENT(s1);
        }
        /* There's one more state than there are move stacks */
        FCS_S_VISITED(s1) |= FCS_VISITED_IN_SOLUTION_PATH;
    }
}
#endif

/*
    This function should be called after the user has retrieved the
    results generated by the scan as it will destroy them.
  */
void fc_solve_finish_instance(fc_solve_instance_t *const instance)
{
/* De-allocate the state collection */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    rbdestroy(instance->tree);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE)
    fcs_libavl2_states_tree_destroy(instance->tree, NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE)
        fc_solve_kaz_tree_free_nodes(instance->tree);
        fc_solve_kaz_tree_destroy(instance->tree);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
        g_tree_destroy(instance->tree);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY)
        {
            Word_t rc_word;
            JHSFA(rc_word, instance->judy_array);
        }
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
        g_hash_table_destroy(instance->hash);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
        fc_solve_hash_free(&(instance->hash));
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)
        fc_solve_states_google_hash_free(instance->hash);
#else
#error FCS_STATE_STORAGE is not defined
#endif

/* De-allocate the stack collection while free()'ing the stacks
in the process */
#ifdef INDIRECT_STACK_STATES
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH)
    fc_solve_hash_free(&(instance->stacks_hash));
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)
    fcs_libavl2_stacks_tree_destroy(instance->stacks_tree, NULL);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    rbdestroy(instance->stacks_tree);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
    g_tree_destroy(instance->stacks_tree);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
    g_hash_table_destroy(instance->stacks_hash);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)
    fc_solve_columns_google_hash_free(instance->stacks_hash);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)
    {
        Word_t rc_word;
        JHSFA(rc_word, instance->stacks_judy_array);
    }
#else
#error FCS_STACK_STORAGE is not set to a good value.
#endif
#endif

#ifndef FCS_DISABLE_NUM_STORED_STATES
    instance->num_states_in_collection = 0;
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    instance->db->close(instance->db, 0);
#endif

#ifdef FCS_RCS_STATES

#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    {
        Word_t rc_word;
        JLFA(rc_word, instance->rcs_states_cache.states_values_to_keys_map);
    }
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
    fc_solve_kaz_tree_free_nodes(instance->rcs_states_cache.kaz_tree);
    fc_solve_kaz_tree_destroy(instance->rcs_states_cache.kaz_tree);
#else
#error Unknown FCS_RCS_CACHE_STORAGE
#endif

    fc_solve_compact_allocator_finish(
        &(instance->rcs_states_cache.states_values_to_keys_allocator));

#endif

    clean_soft_dfs(instance);
}

fc_solve_soft_thread_t *fc_solve_new_soft_thread(
    fc_solve_hard_thread_t *const hard_thread)
{
    /* Make sure we are not exceeding the maximal number of soft threads
     * for an instance. */
    if (HT_INSTANCE(hard_thread)->next_soft_thread_id == MAX_NUM_SCANS)
    {
        return NULL;
    }

    HT_FIELD(hard_thread, soft_threads) =
        SREALLOC(HT_FIELD(hard_thread, soft_threads),
            HT_FIELD(hard_thread, num_soft_threads) + 1);

    fc_solve_soft_thread_t *ret;

    fc_solve_init_soft_thread(hard_thread,
        (ret = &(HT_FIELD(hard_thread,
             soft_threads)[HT_FIELD(hard_thread, num_soft_threads)++])));

    return ret;
}
