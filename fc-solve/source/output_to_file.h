/* Copyright (c) 2000 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * output_to_file.h - header file for outputting a solution to a file.
 *
 */

#ifndef FC_SOLVE__OUTPUT_TO_FILE_H
#define FC_SOLVE__OUTPUT_TO_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "inline.h"
#include "bool.h"

struct fc_solve_display_information_context_struct
{
    fcs_bool_t debug_iter_state_output;
    int freecells_num;
    int stacks_num;
    int decks_num;
    fcs_bool_t parseable_output;
    fcs_bool_t canonized_order_output;
    fcs_bool_t display_10_as_t;
    fcs_bool_t display_parent_iter_num;
    fcs_bool_t debug_iter_output_on;
    fcs_bool_t display_moves;
    fcs_bool_t display_states;
    int standard_notation;
    const char * output_filename;
};

typedef struct fc_solve_display_information_context_struct fc_solve_display_information_context_t;

static GCC_INLINE void fc_solve_output_result_to_file(
    FILE * output_fh,
    void * instance,
    int ret,
    fc_solve_display_information_context_t * dc_ptr
)
{
    fc_solve_display_information_context_t debug_context;

    debug_context = (*dc_ptr);
    if (ret == FCS_STATE_WAS_SOLVED)
    {
        fprintf(output_fh, "-=-=-=-=-=-=-=-=-=-=-=-\n\n");
        {
            fcs_move_t move;
            FILE * move_dump;
            char * as_string;
            int move_num = 0;

            move_dump = output_fh;

            if (debug_context.display_states)
            {
                as_string =
                    freecell_solver_user_current_state_as_string(
                            instance,
                            debug_context.parseable_output,
                            debug_context.canonized_order_output,
                            debug_context.display_10_as_t
                            );

                fprintf(move_dump, "%s\n", as_string);

                free(as_string);

                fprintf(move_dump, "%s", "\n====================\n\n");
            }

            while (
                    freecell_solver_user_get_next_move(
                        instance,
                        &move
                        ) == 0
                  )
            {
                if (debug_context.display_moves)
                {
                    as_string =
                        freecell_solver_user_move_to_string_w_state(
                                instance,
                                move,
                                debug_context.standard_notation
                                );

                    if (debug_context.display_states && debug_context.standard_notation)
                    {
                        fprintf(move_dump, "Move: ");
                    }

                    fprintf(
                            move_dump,
                            (debug_context.standard_notation ?
                             "%s " :
                             "%s\n"
                            ),
                            as_string
                           );
                    move_num++;
                    if (debug_context.standard_notation)
                    {
                        if ((move_num % 10 == 0) || debug_context.display_states)
                        {
                            fprintf(move_dump, "\n");
                        }
                    }
                    if (debug_context.display_states)
                    {
                        fprintf(move_dump, "\n");
                    }
                    fflush(move_dump);
                    free(as_string);
                }

                if (debug_context.display_states)
                {
                    as_string =
                        freecell_solver_user_current_state_as_string(
                                instance,
                                debug_context.parseable_output,
                                debug_context.canonized_order_output,
                                debug_context.display_10_as_t
                                );

                    fprintf(move_dump, "%s\n", as_string);

                    free(as_string);
                }

                if (debug_context.display_states || (!debug_context.standard_notation))
                {
                    fprintf(move_dump, "%s", "\n====================\n\n");
                }
            }

            if (debug_context.standard_notation && (!debug_context.display_states))
            {
                fprintf(move_dump, "\n\n");
            }
        }

        fprintf(output_fh, "This game is solveable.\n");
    }
    else
    {
        fprintf (output_fh, "I could not solve this game.\n");
    }

    fprintf(
            output_fh,
            "Total number of states checked is %i.\n",
            freecell_solver_user_get_num_times(instance)
           );
#if 1
    fprintf(
            output_fh,
            "This scan generated %i states.\n",
            freecell_solver_user_get_num_states_in_collection(instance)
           );
#endif

    return;
}

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__OUTPUT_TO_FILE_H */

