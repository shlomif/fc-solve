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
#include "fcs_user.h"
#include "output_to_file.h"
#include "iter_handler_base.h"

static void my_iter_handler(void *const user_instance,
    const fcs_int_limit_t iter_num, const int depth, void *const ptr_state,
    const fcs_int_limit_t parent_iter_num, void *const context)
{
    const fc_solve_display_information_context_t *const display_context =
        (const fc_solve_display_information_context_t *const)context;
    my_iter_handler_base(
        iter_num, depth, user_instance, display_context, parent_iter_num);
    if (display_context->debug_iter_state_output)
    {
        char state_string[1000];
        freecell_solver_user_iter_state_stringify(
            user_instance, state_string, ptr_state FC_SOLVE__PASS_PARSABLE(
                                             display_context->parseable_output),
            display_context->canonized_order_output FC_SOLVE__PASS_T(
                display_context->display_10_as_t));
        printf("%s\n---------------\n\n\n", state_string);
    }
}
