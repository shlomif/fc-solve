/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// instance_for_lib.h - header file that contains declarations
// and definitions that depend on instance.h functions and are only used only
// by lib.c.
// This is done to speed up compilation.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include "scans_impl.h"
#include "preset.h"
#include "move_funcs_order.h"

static inline void fc_solve_release_tests_list(
    fc_solve_soft_thread_t *const soft_thread)
{
    /* Free the BeFS data. */
    free(BEFS_M_VAR(soft_thread, tests_list));
    BEFS_M_VAR(soft_thread, tests_list) = NULL;

    /* Free the DFS data. */
    fcs_tests_by_depth_array_t *const arr =
        &(DFS_VAR(soft_thread, tests_by_depth_array));
    const_SLOT(num_units, arr);
    for (size_t unit_idx = 0; unit_idx < num_units; unit_idx++)
    {
        if (arr->by_depth_units[unit_idx].move_funcs.lists)
        {
            fcs_move_funcs_list *const lists =
                arr->by_depth_units[unit_idx].move_funcs.lists;
            const_AUTO(
                num_lists, arr->by_depth_units[unit_idx].move_funcs.num_lists);

            for (typeof(arr->by_depth_units[unit_idx].move_funcs.num_lists)
                     i = 0;
                 i < num_lists; i++)
            {
                free(lists[i].move_funcs);
            }
            free(lists);
        }
    }
    free(arr->by_depth_units);
    arr->by_depth_units = NULL;
}

#ifdef FCS_WITH_MOVES
static inline void instance_free_solution_moves(
    fc_solve_instance_t *const instance)
{
    if (instance->solution_moves.moves)
    {
        fcs_move_stack_static_destroy(instance->solution_moves);
        instance->solution_moves.moves = NULL;
    }
}
#endif

static inline void fc_solve_st_free_pq(
    fc_solve_soft_thread_t *const soft_thread)
{
    fc_solve_PQueueFree(&(BEFS_VAR(soft_thread, pqueue)));
}

static inline void fc_solve_free_instance_soft_thread_callback(
    fc_solve_soft_thread_t *const soft_thread)
{
    fc_solve_st_free_pq(soft_thread);
    fc_solve_release_tests_list(soft_thread);
    fc_solve_free_soft_thread_by_depth_test_array(soft_thread);
#ifndef FCS_DISABLE_PATSOLVE
    const_SLOT(pats_scan, soft_thread);

    if (pats_scan)
    {
        fc_solve_pats__recycle_soft_thread(pats_scan);
        fc_solve_pats__destroy_soft_thread(pats_scan);
        free(pats_scan);
        soft_thread->pats_scan = NULL;
    }
#endif
}

#ifdef __cplusplus
}
#endif
