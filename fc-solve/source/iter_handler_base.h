/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// iter_handler_base.h - define the basic my_iter_handler function.
static inline void my_iter_handler_base(const fcs_int_limit_t iter_num,
    const int depth, void *const user_instance GCC_UNUSED,
    const fc_solve_display_information_context *const dc,
    const fcs_int_limit_t parent_iter_num)
{
    printf("Iteration: %li\nDepth: %i\nStored-States: %li\nScan: %s\n",
        (long)iter_num, depth,
#ifdef FCS_DISABLE_NUM_STORED_STATES
        1L,
#else
        (long)freecell_solver_user_get_num_states_in_collection_long(
            user_instance),
#endif
#ifdef FCS_USE_PRECOMPILED_CMD_LINE_THEME
        "scan"
#else
        freecell_solver_user_get_current_soft_thread_name(user_instance)
#endif
    );
    if (dc->display_parent_iter_num)
    {
        printf("Parent Iteration: %li\n", (long)parent_iter_num);
    }
    putchar('\n');
}
