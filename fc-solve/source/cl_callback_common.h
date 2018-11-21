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

#define IS_ARG(s) (!strcmp(arg_str, (s)))
#define IS_ARG_LONG(shrt, lng) (IS_ARG("-" shrt) || IS_ARG("--" lng))

static inline void set_iter_handler(void *const instance GCC_UNUSED,
    fc_solve_display_information_context *const dc GCC_UNUSED)
{
#ifndef FCS_WITHOUT_ITER_HANDLER
    freecell_solver_user_set_iter_handler_long(instance, my_iter_handler, dc);
#endif
}

static inline bool cmd_line_cb__handle_common(const char *const arg_str,
    void *const instance, fc_solve_display_information_context *const dc)
{
    if (IS_ARG_LONG("i", "iter-output"))
    {
        set_iter_handler(instance, dc);
        return TRUE;
    }
    else if (IS_ARG_LONG("s", "state-output"))
    {
        set_iter_handler(instance, dc);
        dc->debug_iter_state_output = TRUE;
        return TRUE;
    }
    else if (IS_ARG_LONG("p", "parseable-output"))
    {
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
        dc->parseable_output = TRUE;
#endif
        return TRUE;
    }
    else if (IS_ARG_LONG("c", "canonized-order-output"))
    {
        dc->canonized_order_output = TRUE;
        return TRUE;
    }
    else if (IS_ARG_LONG("t", "display-10-as-t"))
    {
#ifndef FC_SOLVE_IMPLICIT_T_RANK
        dc->display_10_as_t = TRUE;
#endif
        return TRUE;
    }
    else if (IS_ARG_LONG("m", "display-moves"))
    {
        dc->display_moves = TRUE;
        dc->display_states = FALSE;
        return TRUE;
    }
    else if (IS_ARG_LONG("sn", "standard-notation"))
    {
        dc->standard_notation = FC_SOLVE__STANDARD_NOTATION_REGULAR;
        return TRUE;
    }
    else if (IS_ARG_LONG("snx", "standard-notation-extended"))
    {
        dc->standard_notation = FC_SOLVE__STANDARD_NOTATION_EXTENDED;
        return TRUE;
    }
    else if (IS_ARG_LONG("sam", "display-states-and-moves"))
    {
        dc->display_moves = TRUE;
        dc->display_states = TRUE;
        return TRUE;
    }
    else if (IS_ARG_LONG("pi", "display-parent-iter"))
    {
        dc->display_parent_iter_num = TRUE;
        return TRUE;
    }
    else if (IS_ARG_LONG("sel", "show-exceeded-limits"))
    {
        dc->show_exceeded_limits = TRUE;
        return TRUE;
    }
    else if (IS_ARG_LONG("hoi", "hint-on-intractable"))
    {
        dc->hint_on_intract = TRUE;
        return TRUE;
    }
    return FALSE;
}
