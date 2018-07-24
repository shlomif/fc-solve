/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2016 Shlomi Fish
 */
#pragma once
#include "fcs_conf.h"
#include "output_to_file.h"
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
    const_SLOT(iters_display_step, dc);
    if (iters_display_step > 1 && (iter_num % iters_display_step != 0))
    {
        return;
    }
    my_iter_handler_base(iter_num, depth, user_instance, dc, parent_iter_num);
    if (dc->debug_iter_state_output)
    {
        char state_string[1000];
        freecell_solver_user_iter_state_stringify(user_instance, state_string,
            ptr_state FC_SOLVE__PASS_PARSABLE(dc->parseable_output),
            dc->canonized_order_output FC_SOLVE__PASS_T(dc->display_10_as_t));
        printf("%s\n---------------\n\n\n", state_string);
    }
#endif
}
#endif
