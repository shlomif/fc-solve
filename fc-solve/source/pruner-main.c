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
 * pruner-main.c : pruning solver.
 */
#include "cl_callback.h"
#include "iter_handler_base.h"

#ifndef FCS_WITHOUT_ITER_HANDLER
static void my_iter_handler(void *const user_instance GCC_UNUSED,
    const fcs_int_limit_t iter_num GCC_UNUSED, const int depth GCC_UNUSED,
    void *const ptr_state GCC_UNUSED,
    const fcs_int_limit_t parent_iter_num GCC_UNUSED,
    void *const context GCC_UNUSED)
{
#ifdef FCS_WITH_MOVES
    const fc_solve_display_information_context *const dc =
        (const fc_solve_display_information_context *const)context;
    my_iter_handler_base(iter_num, depth, user_instance, dc, parent_iter_num);
    if (dc->debug_iter_state_output)
    {
        char state_string[1000];
        freecell_solver_user_iter_state_stringify(user_instance, state_string,
            ptr_state FC_SOLVE__PASS_PARSABLE(TRUE),
            FALSE FC_SOLVE__PASS_T(TRUE));
        printf("%s\n---------------\n\n\n", state_string);
        /* Now pass it to a secondary user_instance prune it. */
        void *const pruner = freecell_solver_user_alloc();
#ifndef HARD_CODED_NUM_FREECELLS
        freecell_solver_user_set_num_freecells(pruner, 2);
#endif
        FCS__DECL_ERR_PTR(error_string);
        freecell_solver_user_set_depth_tests_order(
            pruner, 0, "01ABCDE" FCS__PASS_ERR_STR(&error_string));
#ifdef FCS_WITH_ERROR_STRS
        free(error_string);
#endif
#ifndef FCS_WITHOUT_MAX_NUM_STATES
        freecell_solver_user_limit_iterations_long(pruner, 128 * 1024);
#endif
        const int ret = freecell_solver_user_solve_board(pruner, state_string);
        switch (ret)
        {
        case FCS_STATE_SUSPEND_PROCESS:
            printf("\n\nVerdict: INDETERMINATE\n\n");
            break;

        case FCS_STATE_WAS_SOLVED:
            printf("\n\nVerdict: SOLVED\n\nYay! We found a solution from "
                   "this one.");
            exit(0);

        case FCS_STATE_IS_NOT_SOLVEABLE:
            printf("\n\nVerdict: PRUNED\n\n");
            break;

        default:
            printf("\n\nVerdict: unknown ret code: %d\n\n", ret);
            exit(-1);
        }
        freecell_solver_user_free(pruner);
    }
#endif
}
#endif
