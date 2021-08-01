// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// instance.c - instance/hard_thread/soft_thread functions for Freecell Solver.
#include "instance_for_lib.h"

// General use of this interface:
// 1. fc_solve_alloc_instance()
// 2. Set the parameters of the game
// 3. If you wish to revert, go to step #11.
// 4. fc_solve_init_instance()
// 5. Call fc_solve_solve_instance() with the initial board.
// 6. If it returns FCS_STATE_SUSPEND_PROCESS and you wish to proceed,
//    then increase the iteration limit and call
//    fc_solve_resume_instance().
// 7. Repeat Step #6 zero or more times.
// 8. If the solving was successful you can use the move stacks or the
//    intermediate stacks. (Just don't destroy them in any way).
// 9. Call fc_solve_finish_instance().
// 10. Call fc_solve_free_instance().
// The library functions inside lib.c (a.k.a fcs_user()) give an
// easier approach for embedding Freecell Solver into your library. The
// intent of this comment is to document the code, rather than to be
// a guideline for the end-user.

static const fcs_default_weights fc_solve_default_befs_weights = {
    .weights = {0.5, 0, 0.3, 0, 0.2, 0}};

static inline void soft_thread_clean_soft_dfs(
    fcs_soft_thread *const soft_thread)
{
    fcs_soft_dfs_stack_item *const soft_dfs_info =
        DFS_VAR(soft_thread, soft_dfs_info);
    // Check if a Soft-DFS-type scan was called in the first place
    if (!soft_dfs_info)
    {
        return;
    }

    // De-allocate the Soft-DFS specific stacks
    const fcs_soft_dfs_stack_item *info_ptr = soft_dfs_info;
    const fcs_soft_dfs_stack_item *const max_info_ptr =
        info_ptr + DFS_VAR(soft_thread, depth);
    const fcs_soft_dfs_stack_item *const dfs_max_info_ptr =
        info_ptr + DFS_VAR(soft_thread, dfs_max_depth);

    for (; info_ptr < max_info_ptr; ++info_ptr)
    {
        free(info_ptr->derived_states_list.states);
#ifndef FCS_ZERO_FREECELLS_MODE
        free(info_ptr->derived_states_random_indexes);
#endif
    }
    for (; info_ptr < dfs_max_info_ptr; ++info_ptr)
    {
        if (likely(info_ptr->derived_states_list.states))
        {
            free(info_ptr->derived_states_list.states);
#ifndef FCS_ZERO_FREECELLS_MODE
            free(info_ptr->derived_states_random_indexes);
#endif
        }
    }

    free(soft_dfs_info);
    DFS_VAR(soft_thread, soft_dfs_info) = NULL;
    DFS_VAR(soft_thread, dfs_max_depth) = 0;
}

#ifndef FCS_ZERO_FREECELLS_MODE
extern void fc_solve_free_soft_thread_by_depth_move_array(
    fcs_soft_thread *const soft_thread)
{
    const_AUTO(
        by_depth_moves, soft_thread->by_depth_moves_order.by_depth_moves);
    const_AUTO(num, soft_thread->by_depth_moves_order.num);
    for (size_t i = 0; i < num; ++i)
    {
        moves_order__free(&(by_depth_moves[i].moves_order));
    }

    soft_thread->by_depth_moves_order.num = 0;

    free(by_depth_moves);
    soft_thread->by_depth_moves_order.by_depth_moves = NULL;
}

static inline void accumulate_tests_by_ptr(
    size_t *const moves_order, fcs_moves_order *const st_tests_order)
{
    const fcs_moves_group *group_ptr = st_tests_order->groups;
    const fcs_moves_group *const groups_end = group_ptr + st_tests_order->num;
    for (; group_ptr < groups_end; ++group_ptr)
    {
        const fcs_move_func *test_ptr = group_ptr->move_funcs;
        const fcs_move_func *const tests_end = test_ptr + group_ptr->num;
        for (; test_ptr < tests_end; ++test_ptr)
        {
            *moves_order |= (1 << (test_ptr->idx));
        }
    }
}
#endif

static inline void soft_thread_run_cb(fcs_soft_thread *const soft_thread,
    const foreach_st_callback_choice callback_choice,
    void *const context GCC_UNUSED)
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
#ifndef FCS_ZERO_FREECELLS_MODE
        accumulate_tests_by_ptr((size_t *)context,
            &(soft_thread->by_depth_moves_order.by_depth_moves[0].moves_order));
#endif
        break;

    case FOREACH_SOFT_THREAD_DETERMINE_SCAN_COMPLETENESS:
#ifndef FCS_ZERO_FREECELLS_MODE
    {
        size_t moves_order = 0;

        accumulate_tests_by_ptr(&moves_order,
            &(soft_thread->by_depth_moves_order.by_depth_moves[0].moves_order));

        STRUCT_SET_FLAG_TO(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN,
            (moves_order == *(size_t *)context));
    }
#endif
    break;
    }
}

void fc_solve_foreach_soft_thread(fcs_instance *const instance,
    const foreach_st_callback_choice callback_choice, void *const context)
{
    const_AUTO(num_soft_threads, instance->hard_thread.num_soft_threads);
    for (fastest_type_for_num_soft_threads__unsigned st_idx = 0;
         st_idx <= num_soft_threads; ++st_idx)
    {
        fcs_soft_thread *soft_thread;
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
        soft_thread_run_cb(soft_thread, callback_choice, context);
    }
}

#if !(defined(FCS_WITH_MOVES))
static inline
#endif
    void
    fc_solve_init_soft_thread(
        fcs_hard_thread *const hard_thread, fcs_soft_thread *const soft_thread)
{
    *soft_thread = (fcs_soft_thread){
        .hard_thread = hard_thread,
        .id = (HT_INSTANCE(hard_thread)->next_soft_thread_id)++,
        .method_specific =
            {
                .soft_dfs =
                    {
                        .dfs_max_depth = 0,
                        .soft_dfs_info = NULL,
                        .depth = 0,
#ifndef FCS_ZERO_FREECELLS_MODE

                        .moves_by_depth =
                            {
                                .num_units = 0,
                                .by_depth_units = NULL,
                            },
#endif
                        .rand_seed = 24,
                    },
                .befs =
                    {
                        .moves_list = NULL,
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

#ifndef FCS_ZERO_FREECELLS_MODE

        .by_depth_moves_order =
            {
                .num = 1,
                .by_depth_moves =
                    SMALLOC1(soft_thread->by_depth_moves_order.by_depth_moves),
            },
#endif

        .is_befs = false,
#ifdef FCS_WITH_MOVES
        .is_optimize_scan = false,
#endif
        .super_method_type = FCS_SUPER_METHOD_DFS,
        .master_to_randomize = false,
        .checked_states_step = 50,
#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
        .name = "",
#endif
#ifndef FCS_ENABLE_PRUNE__R_TF__UNCOND
        .enable_pruning = false,
#endif
#ifndef FCS_DISABLE_PATSOLVE
        .pats_scan = NULL,
#endif
    };
#ifndef FCS_ZERO_FREECELLS_MODE
    soft_thread->by_depth_moves_order.by_depth_moves[0] =
        (typeof(soft_thread->by_depth_moves_order.by_depth_moves[0])){
            .max_depth = SSIZE_MAX,
            .moves_order = moves_order_dup(
                &(fcs_st_instance(soft_thread)->instance_moves_order)),
        };
#endif

    fc_solve_reset_soft_thread(soft_thread);
}

typedef struct
{
    int idx;
    enum
    {
        FREECELL,
        COLUMN
    } type;
} find_card_ret;

// This function traces the solution from the final state down to the initial
// state
#ifdef FCS_WITH_MOVES

#ifndef FCS_DISABLE_PATSOLVE
static inline int find_empty_col(
    const fcs_state *const dynamic_state STACKS_NUM__ARG)
{
    for (size_t i = 0; i < STACKS_NUM__VAL; ++i)
    {
        if (fcs_state_col_is_empty(*dynamic_state, i))
        {
            return (int)i;
        }
    }

    return -1;
}

static inline int find_col_card(
    const fcs_state *const dynamic_state, const fcs_card needle STACKS_NUM__ARG)
{
    for (size_t i = 0; i < STACKS_NUM__VAL; ++i)
    {
        const_AUTO(col, fcs_state_get_col(*dynamic_state, i));
        const int col_len = fcs_col_len(col);
        if (col_len > 0 && (fcs_col_get_card(col, col_len - 1) == needle))
        {
            return (int)i;
        }
    }

    return -1;
}

#if MAX_NUM_FREECELLS > 0
static inline int find_fc_card(const fcs_state *const dynamic_state,
    const fcs_card needle FREECELLS_NUM__ARG)
{
    for (size_t dest = 0; dest < FREECELLS_NUM__VAL; ++dest)
    {
        if (fcs_freecell_card(*dynamic_state, dest) == needle)
        {
            return (int)dest;
        }
    }

    return -1;
}
#endif

static inline find_card_ret find_card_src_string(
    const fcs_state *const dynamic_state,
    const fcs_card needle FREECELLS_AND_STACKS_ARGS())
{
    const int src_col_idx =
        find_col_card(dynamic_state, needle PASS_STACKS(STACKS_NUM__VAL));
#if MAX_NUM_FREECELLS > 0
    if (src_col_idx < 0)
    {
        return (find_card_ret){.idx = (find_fc_card(dynamic_state,
                                   needle PASS_FREECELLS(FREECELLS_NUM__VAL))),
            .type = FREECELL};
    }
    else
#endif
    {
        return (find_card_ret){.idx = src_col_idx, .type = COLUMN};
    }
}
#endif

extern void fc_solve_trace_solution(fcs_instance *const instance)
{
    fcs_internal_move canonize_move = fc_solve_empty_move;
    fcs_int_move_set_type(canonize_move, FCS_MOVE_TYPE_CANONIZE);

    instance_free_solution_moves(instance);
    instance->solution_moves = fcs_move_stack__new();

    const_AUTO(solution_moves_ptr, &(instance->solution_moves));
#ifndef FCS_DISABLE_PATSOLVE
    const_SLOT(solving_soft_thread, instance);
    if (solving_soft_thread->super_method_type == FCS_SUPER_METHOD_PATSOLVE)
    {
        fcs_state_locs_struct locs;
        fc_solve_init_locs(&(locs));
        const_SLOT(pats_scan, solving_soft_thread);
        var_AUTO(num_moves, pats_scan->num_moves_to_win);

        fcs_state_keyval_pair s_and_info;
        DECLARE_IND_BUF_T(indirect_stacks_buffer)
        FCS_STATE__DUP_keyval_pair(s_and_info, (instance->state_copy));

#ifndef HARD_CODED_NUM_STACKS
        const size_t stacks_num = INSTANCE_STACKS_NUM;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
        const size_t freecells_num = INSTANCE_FREECELLS_NUM;
#endif

        fcs_state *const s = &(s_and_info.s);
#ifdef INDIRECT_STACK_STATES
        for (stack_i i = 0; i < STACKS_NUM__VAL; ++i)
        {
            fcs_copy_stack(
                s_and_info.s, s_and_info.info, i, indirect_stacks_buffer);
        }
#endif

        solution_moves_ptr->num_moves = num_moves;
        solution_moves_ptr->moves =
            SREALLOC(solution_moves_ptr->moves, num_moves);
        var_AUTO(move_ptr, pats_scan->moves_to_win);
        for (size_t i = 0; i < num_moves; ++i, ++move_ptr)
        {
            const fcs_card card = move_ptr->card;
            fcs_internal_move out_move = fc_solve_empty_move;
            switch (move_ptr->totype)
            {
#if MAX_NUM_FREECELLS > 0
            case FCS_PATS__TYPE_FREECELL: {
                stack_i src_col_idx;
                for (src_col_idx = 0; src_col_idx < STACKS_NUM__VAL;
                     ++src_col_idx)
                {
                    var_AUTO(
                        src_col, fcs_state_get_col(s_and_info.s, src_col_idx));
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

                for (stack_i dest = 0; dest < FREECELLS_NUM__VAL; ++dest)
                {
                    if (fcs_freecell_is_empty(s_and_info.s, dest))
                    {
                        fcs_int_move_set_type(
                            out_move, FCS_MOVE_TYPE_STACK_TO_FREECELL);
                        fcs_int_move_set_src(out_move, src_col_idx);
                        fcs_int_move_set_dest(out_move, dest);
                        break;
                    }
                }
            }
            break;
#endif
            case FCS_PATS__TYPE_FOUNDATION: {
                const find_card_ret src = find_card_src_string(&(s_and_info.s),
                    card PASS_FREECELLS(FREECELLS_NUM__VAL)
                        PASS_STACKS(STACKS_NUM__VAL));
                if (src.type == FREECELL)
                {
                    fcs_int_move_set_type(
                        out_move, FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION);
                }
                else
                {
                    fcs_int_move_set_type(
                        out_move, FCS_MOVE_TYPE_STACK_TO_FOUNDATION);
                }
                fcs_int_move_set_src(out_move, src.idx);
                fcs_int_move_set_dest(out_move, fcs_card_suit(card));
            }
            break;
            default: {
                const fcs_card dest_card = move_ptr->destcard;
                const find_card_ret src = find_card_src_string(s,
                    card PASS_FREECELLS(FREECELLS_NUM__VAL)
                        PASS_STACKS(STACKS_NUM__VAL));
                if (src.type == FREECELL)
                {
                    fcs_int_move_set_type(
                        out_move, FCS_MOVE_TYPE_FREECELL_TO_STACK);
                }
                else
                {
                    fcs_int_move_set_type(
                        out_move, FCS_MOVE_TYPE_STACK_TO_STACK);
                    fcs_int_move_set_num_cards_in_seq(out_move, 1);
                }
                fcs_int_move_set_src(out_move, src.idx);
                fcs_int_move_set_dest(out_move,
                    (fcs_card_is_empty(dest_card)
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
        fcs_collectible_state *s1 = instance->final_state;

        // Retrace the path from the current state to its parents
        while (FCS_S_PARENT(s1) != NULL)
        {
            // Mark the state as part of the non-optimized solution
            FCS_S_VISITED(s1) |= FCS_VISITED_IN_SOLUTION_PATH;

            // Each state->parent_state stack has an implicit CANONIZE move.
            fcs_move_stack_push(solution_moves_ptr, canonize_move);

            // Merge the move stack
            const fcs_move_stack *const stack = FCS_S_MOVES_TO_PARENT(s1);
            const fcs_internal_move *const moves = stack->moves;
            for (long move_idx = (long)stack->num_moves - 1; move_idx >= 0;
                 --move_idx)
            {
                fcs_move_stack_push(solution_moves_ptr, moves[move_idx]);
            }

            s1 = FCS_S_PARENT(s1);
        }
        // There's one more state than there are move stacks
        FCS_S_VISITED(s1) |= FCS_VISITED_IN_SOLUTION_PATH;
    }
}
#endif

fcs_soft_thread *fc_solve_new_soft_thread(fcs_hard_thread *const hard_thread)
{
    if (HT_INSTANCE(hard_thread)->next_soft_thread_id == MAX_NUM_SCANS)
    {
        return NULL;
    }

    HT_FIELD(hard_thread, soft_threads) =
        SREALLOC(HT_FIELD(hard_thread, soft_threads),
            HT_FIELD(hard_thread, num_soft_threads) + 1);

    fcs_soft_thread *ret;

    fc_solve_init_soft_thread(hard_thread,
        (ret = &(HT_FIELD(hard_thread,
             soft_threads)[HT_FIELD(hard_thread, num_soft_threads)++])));

    return ret;
}
