/*
 * Copyright (c) 2016 Shlomi Fish
 *
 * Licensed under the MIT/X11 license.
 * */
#pragma once

#define IS_ARG(s) (!strcmp(arg_str, (s)))

static GCC_INLINE void set_iter_handler(
    void *const instance, fc_solve_display_information_context_t *const dc)
{
    freecell_solver_user_set_iter_handler_long(instance, my_iter_handler, dc);
    dc->debug_iter_output_on = TRUE;
}

static GCC_INLINE fcs_bool_t cmd_line_cb__handle_common(
    const char *const arg_str, void *const instance,
    fc_solve_display_information_context_t *const dc)
{
    if (IS_ARG("-i") || IS_ARG("--iter-output"))
    {
        set_iter_handler(instance, dc);
        return TRUE;
    }
    else if (IS_ARG("-s") || IS_ARG("--state-output"))
    {
        set_iter_handler(instance, dc);
        dc->debug_iter_state_output = TRUE;
        return TRUE;
    }
    else if (IS_ARG("-p") || IS_ARG("--parseable-output"))
    {
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
        dc->parseable_output = TRUE;
#endif
        return TRUE;
    }
    else if (IS_ARG("-c") || IS_ARG("--canonized-order-output"))
    {
        dc->canonized_order_output = TRUE;
        return TRUE;
    }
    else if (IS_ARG("-t") || IS_ARG("--display-10-as-t"))
    {
#ifndef FC_SOLVE_IMPLICIT_T_RANK
        dc->display_10_as_t = TRUE;
#endif
        return TRUE;
    }
    else if (IS_ARG("-m") || IS_ARG("--display-moves"))
    {
        dc->display_moves = TRUE;
        dc->display_states = FALSE;
        return TRUE;
    }
    else if (IS_ARG("-sn") || IS_ARG("--standard-notation"))
    {
        dc->standard_notation = FC_SOLVE__STANDARD_NOTATION_REGULAR;
        return TRUE;
    }
    else if (IS_ARG("-snx") || IS_ARG("--standard-notation-extended"))
    {
        dc->standard_notation = FC_SOLVE__STANDARD_NOTATION_EXTENDED;
        return TRUE;
    }
    else if (IS_ARG("-sam") || IS_ARG("--display-states-and-moves"))
    {
        dc->display_moves = TRUE;
        dc->display_states = TRUE;
        return TRUE;
    }
    else if (IS_ARG("-pi") || IS_ARG("--display-parent-iter"))
    {
        dc->display_parent_iter_num = TRUE;
        return TRUE;
    }
    else if (IS_ARG("-sel") || IS_ARG("--show-exceeded-limits"))
    {
        dc->show_exceeded_limits = TRUE;
        return TRUE;
    }
    return FALSE;
}
