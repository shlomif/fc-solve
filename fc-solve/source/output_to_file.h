/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// output_to_file.h - header file for outputting a solution to a file.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rinutils.h"
#include "freecell-solver/fcs_enums.h"
#include "freecell-solver/fcs_user.h"

typedef struct
{
    const char *output_filename;
    int standard_notation;
    bool debug_iter_state_output;
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    bool parseable_output;
#endif
    bool canonized_order_output;
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    bool display_10_as_t;
#endif
    bool display_parent_iter_num;
    bool display_moves;
    bool display_states;
    bool show_exceeded_limits;
    bool hint_on_intract;
    size_t iters_display_step;
} fc_solve_display_information_context;

static const fc_solve_display_information_context INITIAL_DISPLAY_CONTEXT = {
    .debug_iter_state_output = FALSE,
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    .parseable_output = FALSE,
#endif
    .canonized_order_output = FALSE,
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    .display_10_as_t = FALSE,
#endif
    .display_parent_iter_num = FALSE,
    .display_moves = FALSE,
    .display_states = TRUE,
    .standard_notation = FC_SOLVE__STANDARD_NOTATION_NO,
    .output_filename = NULL,
    .show_exceeded_limits = FALSE,
    .hint_on_intract = FALSE,
    .iters_display_step = 1,
};

static inline void fc_solve_output_result_to_file(FILE *const output_fh,
    void *const instance, const int ret,
    const fc_solve_display_information_context *const dc_ptr)
{
    const_AUTO(display_context, (*dc_ptr));
    const bool was_solved = (ret == FCS_STATE_WAS_SOLVED);
    const bool was_suspend = (ret == FCS_STATE_SUSPEND_PROCESS);
#ifdef FCS_WITH_MOVES
    if (was_solved || (display_context.hint_on_intract && was_suspend))
    {
        fputs("-=-=-=-=-=-=-=-=-=-=-=-\n\n", output_fh);
        fcs_move_t move;
        char state_as_string[1000];

        if (display_context.display_states)
        {
            freecell_solver_user_current_state_stringify(instance,
                state_as_string FC_SOLVE__PASS_PARSABLE(
                    display_context.parseable_output),
                display_context.canonized_order_output FC_SOLVE__PASS_T(
                    display_context.display_10_as_t));

            fputs(state_as_string, output_fh);
            fputs("\n\n====================\n\n", output_fh);
        }

        int move_num = 0;
        while (freecell_solver_user_get_next_move(instance, &move) == 0)
        {
            if (display_context.display_moves)
            {
                if (display_context.display_states &&
                    display_context.standard_notation)
                {
                    fputs("Move: ", output_fh);
                }

                freecell_solver_user_stringify_move_w_state(instance,
                    state_as_string, move, display_context.standard_notation);
                fprintf(output_fh,
                    (display_context.standard_notation ? "%s " : "%s\n"),
                    state_as_string);
                move_num++;
                if (display_context.standard_notation)
                {
                    if ((move_num % 10 == 0) || display_context.display_states)
                    {
                        putc('\n', output_fh);
                    }
                }
                if (display_context.display_states)
                {
                    putc('\n', output_fh);
                }
                fflush(output_fh);
            }

            if (display_context.display_states)
            {
                freecell_solver_user_current_state_stringify(instance,
                    state_as_string FC_SOLVE__PASS_PARSABLE(
                        display_context.parseable_output),
                    display_context.canonized_order_output FC_SOLVE__PASS_T(
                        display_context.display_10_as_t));

                fprintf(output_fh, "%s\n", state_as_string);
            }

            if (display_context.display_states ||
                (!display_context.standard_notation))
            {
                fputs("\n====================\n\n", output_fh);
            }
        }

        if (display_context.standard_notation &&
            (!display_context.display_states))
        {
            fputs("\n\n", output_fh);
        }
    }
#endif
    fputs((was_solved ? "This game is solveable.\n"
                      : (display_context.show_exceeded_limits && was_suspend)
                            ? "Iterations count exceeded.\n"
                            : "I could not solve this game.\n"),
        output_fh);

    fprintf(output_fh, "Total number of states checked is %ld.\n",
        (long)freecell_solver_user_get_num_times_long(instance));
#ifndef FCS_DISABLE_NUM_STORED_STATES
    fprintf(output_fh, "This scan generated %ld states.\n",
        (long)freecell_solver_user_get_num_states_in_collection_long(instance));
#endif
}

#ifdef __cplusplus
}
#endif
