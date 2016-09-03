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
 * pruner-main.c : pruning solver.
 */
#include "cl_callback.h"
#include "iter_handler_base.h"

static void my_iter_handler(void *const user_instance,
    const fcs_int_limit_t iter_num, const int depth, void *const ptr_state,
    const fcs_int_limit_t parent_iter_num, void *const lp_context)
{
    const fc_solve_display_information_context_t *const display_context =
        (const fc_solve_display_information_context_t *const)lp_context;
    my_iter_handler_base(
        iter_num, depth, user_instance, display_context, parent_iter_num);
    if (display_context->debug_iter_state_output)
    {
        char state_string[1000];
        freecell_solver_user_iter_state_stringify(user_instance, state_string,
            ptr_state FC_SOLVE__PASS_PARSABLE(TRUE),
            FALSE FC_SOLVE__PASS_T(TRUE));
        printf("%s\n---------------\n\n\n", state_string);
        /* Now pass it to a secondary user_instance prune it. */
        {
            void *const pruner = freecell_solver_user_alloc();

            freecell_solver_user_set_num_freecells(pruner, 2);
#ifdef FCS_WITH_ERROR_STRS
            char *error_string;
#endif
            freecell_solver_user_set_tests_order(
                pruner, "01ABCDE" FCS__PASS_ERR_STR(&error_string));
#ifdef FCS_WITH_ERROR_STRS
            free(error_string);
#endif
            freecell_solver_user_limit_iterations_long(pruner, 128 * 1024);

            const int ret =
                freecell_solver_user_solve_board(pruner, state_string);
            switch (ret)
            {
            case FCS_STATE_SUSPEND_PROCESS:
                printf("\n\nVerdict: INDETERMINATE\n\n");
                break;

            case FCS_STATE_WAS_SOLVED:
                printf("\n\nVerdict: SOLVED\n\nYay! We found a solution from "
                       "this one.");
                exit(0);
                break;

            case FCS_STATE_IS_NOT_SOLVEABLE:
                printf("\n\nVerdict: PRUNED\n\n");
                break;

            default:
                printf("\n\nVerdict: unknown ret code: %d\n\n", ret);
                exit(-1);
                break;
            }
            freecell_solver_user_free(pruner);
        }
    }
}

int main(int argc, char *argv[]) { return fc_solve_main__main(argc, argv); }
