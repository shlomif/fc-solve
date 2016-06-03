/*
 * Copyright (c) 2016 Shlomi Fish
 *
 * Licensed under the MIT/X11 license.
 * */
#pragma once

static void my_iter_handler(
    void * const user_instance,
    const fcs_int_limit_t iter_num,
    const int depth,
    void * const ptr_state,
    const fcs_int_limit_t parent_iter_num,
    void * lp_context
    )
{
    fc_solve_display_information_context_t * const dc = (fc_solve_display_information_context_t*)lp_context;

    fprintf(stdout, "Iteration: %li\n", (long)iter_num);
    fprintf(stdout, "Depth: %i\n", depth);
    fprintf(stdout, "Stored-States: %li\n",
        (long)freecell_solver_user_get_num_states_in_collection_long(user_instance)
        );
    fprintf(stdout, "Scan: %s\n",
        freecell_solver_user_get_current_soft_thread_name(user_instance)
        );
    if (dc->display_parent_iter_num)
    {
        fprintf(stdout, "Parent Iteration: %li\n", (long)parent_iter_num);
    }
    fprintf(stdout, "\n");


    if (dc->debug_iter_state_output)
    {
        char state_string[1000];
        freecell_solver_user_iter_state_stringify(
                user_instance,
                state_string,
                ptr_state
                FC_SOLVE__PASS_PARSABLE(dc->parseable_output)
                , dc->canonized_order_output
                FC_SOLVE__PASS_T(dc->display_10_as_t)
                );
        printf("%s\n---------------\n\n\n", state_string);
    }
}
