// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
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
#ifndef FCS_ZERO_FREECELLS_MODE
#include "move_funcs_order.h"
#endif

static inline void fcs_free_moves_list(fcs_soft_thread *const soft_thread)
{
#ifndef FCS_ZERO_FREECELLS_MODE
    /* Free the BeFS data. */
    free(BEFS_M_VAR(soft_thread, moves_list));
    BEFS_M_VAR(soft_thread, moves_list) = NULL;

    /* Free the DFS data. */
    fcs_moves_by_depth_array *const arr =
        &(DFS_VAR(soft_thread, moves_by_depth));
    const_SLOT(num_units, arr);
    for (size_t unit_idx = 0; unit_idx < num_units; unit_idx++)
    {
        if (arr->by_depth_units[unit_idx].move_funcs.groups)
        {
            fcs_moves_group *const groups =
                arr->by_depth_units[unit_idx].move_funcs.groups;
            const_AUTO(num, arr->by_depth_units[unit_idx].move_funcs.num);

            for (typeof(arr->by_depth_units[unit_idx].move_funcs.num) i = 0;
                 i < num; i++)
            {
                free(groups[i].move_funcs);
            }
            free(groups);
        }
    }
    free(arr->by_depth_units);
    arr->by_depth_units = NULL;
#endif
}

#ifdef FCS_WITH_MOVES
static inline void instance_free_solution_moves(fcs_instance *const instance)
{
    if (instance->solution_moves.moves)
    {
        fcs_move_stack_static_destroy(instance->solution_moves);
        instance->solution_moves.moves = NULL;
    }
}
#endif

static inline void st_free_pq(fcs_soft_thread *const soft_thread)
{
    fc_solve_PQueueFree(&(BEFS_VAR(soft_thread, pqueue)));
}

static inline void fc_solve_free_instance_soft_thread_callback(
    fcs_soft_thread *const soft_thread)
{
    st_free_pq(soft_thread);
    fcs_free_moves_list(soft_thread);
    fc_solve_free_soft_thread_by_depth_move_array(soft_thread);
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
