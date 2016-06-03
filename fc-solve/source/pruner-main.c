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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "fcs_cl.h"
#include "rinutils.h"

#include "output_to_file.h"
#include "handle_parsing.h"

static void my_iter_handler(
    void * user_instance,
    fcs_int_limit_t iter_num,
    int depth,
    void * ptr_state,
    fcs_int_limit_t parent_iter_num,
    void * lp_context
    )
{
    fc_solve_display_information_context_t * context;
    context = (fc_solve_display_information_context_t*)lp_context;

    fprintf(stdout, "Iteration: %li\n", (long)iter_num);
    fprintf(stdout, "Depth: %i\n", depth);
    fprintf(stdout, "Stored-States: %li\n",
        (long)freecell_solver_user_get_num_states_in_collection_long(user_instance)
        );
    fprintf(stdout, "Scan: %s\n",
        freecell_solver_user_get_current_soft_thread_name(user_instance)
        );
    if (context->display_parent_iter_num)
    {
        fprintf(stdout, "Parent Iteration: %li\n", (long)parent_iter_num);
    }
    fprintf(stdout, "\n");


    if (context->debug_iter_state_output)
    {
        char state_string[1000];
            freecell_solver_user_iter_state_stringify(
                user_instance,
                state_string,
                ptr_state
                FC_SOLVE__PASS_PARSABLE(TRUE)
                , FALSE
                FC_SOLVE__PASS_T(TRUE)
                );
        printf("%s\n---------------\n\n\n", state_string);

        /* Process the string in-place to make it available as input
         * to fc-solve again.
         * */

        {
            char * s, * d;
            char c;
            s = d = state_string;

            while ((c = *(d++) = *(s++)))
            {
                if ((c == '\n') && (s[0] == ':') && (s[1] = ' '))
                {
                    s += 2;
                }
            }
        }

        /* Now pass it to a secondary user_instance prune it. */
        {
            void * const pruner = freecell_solver_user_alloc();

            freecell_solver_user_set_num_freecells(pruner, 2);
            char * error_string;
            freecell_solver_user_set_tests_order(pruner, "01ABCDE", &error_string);
            free(error_string);
            freecell_solver_user_limit_iterations_long(pruner, 128*1024);

            const int ret = freecell_solver_user_solve_board(pruner, state_string);
            if (ret == FCS_STATE_SUSPEND_PROCESS)
            {
                printf ("\n\nVerdict: INDETERMINATE\n\n");
            }
            else if (ret == FCS_STATE_WAS_SOLVED)
            {
                printf("\n\nVerdict: SOLVED\n\nYay! We found a solution from this one.");
                exit(0);
            }
            else if (ret == FCS_STATE_IS_NOT_SOLVEABLE)
            {
                printf("\n\nVerdict: PRUNED\n\n");
            }
            else
            {
                printf("\n\nVerdict: unknown ret code: %d\n\n", ret);
                exit(-1);
            }

            freecell_solver_user_free(pruner);
        }
    }
}

static int cmd_line_callback(
    void * const instance,
    const int argc,
    const char * argv[],
    const int arg,
    int * const num_to_skip,
    int * const ret GCC_UNUSED,
    void * const context
    )
{
    fc_solve_display_information_context_t * dc;
    *num_to_skip = 0;

    dc = (fc_solve_display_information_context_t * )context;

    if ((!strcmp(argv[arg], "-i")) || (!strcmp(argv[arg], "--iter-output")))
    {
#define set_iter_handler() \
        freecell_solver_user_set_iter_handler_long(   \
            instance,   \
            my_iter_handler,   \
            dc    \
            );        \
        dc->debug_iter_output_on = TRUE;

        set_iter_handler();
    }
    else if ((!strcmp(argv[arg], "-s")) || (!strcmp(argv[arg], "--state-output")))
    {
        set_iter_handler();
        dc->debug_iter_state_output = TRUE;
#undef set_iter_handler
    }
    else if ((!strcmp(argv[arg], "-p")) || (!strcmp(argv[arg], "--parseable-output")))
    {
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
        dc->parseable_output = TRUE;
#endif
    }
    else if ((!strcmp(argv[arg], "-c")) || (!strcmp(argv[arg], "--canonized-order-output")))
    {
        dc->canonized_order_output = TRUE;
    }
    else if ((!strcmp(argv[arg], "-o")) || (!strcmp(argv[arg], "--output")))
    {
        const int next_arg = arg+1;
        if (next_arg == argc)
        {
            return FCS_CMD_LINE_STOP;
        }
        *num_to_skip = 2;
        dc->output_filename = (const char *)argv[next_arg];
        return FCS_CMD_LINE_SKIP;
    }
    else if ((!strcmp(argv[arg], "-t")) || (!strcmp(argv[arg], "--display-10-as-t")))
    {
#ifndef FC_SOLVE_IMPLICIT_T_RANK
        dc->display_10_as_t = TRUE;
#endif
    }
    else if ((!strcmp(argv[arg], "-m")) || (!strcmp(argv[arg], "--display-moves")))
    {
        dc->display_moves = TRUE;
        dc->display_states = FALSE;
    }
    else if ((!strcmp(argv[arg], "-sn")) || (!strcmp(argv[arg], "--standard-notation")))
    {
        dc->standard_notation = FC_SOLVE__STANDARD_NOTATION_REGULAR;

    }
    else if ((!strcmp(argv[arg], "-snx")) || (!strcmp(argv[arg], "--standard-notation-extended")))
    {
        dc->standard_notation = FC_SOLVE__STANDARD_NOTATION_EXTENDED;
    }
    else if ((!strcmp(argv[arg], "-sam")) || (!strcmp(argv[arg], "--display-states-and-moves")))
    {
        dc->display_moves = TRUE;
        dc->display_states = TRUE;
    }
    else if ((!strcmp(argv[arg], "-pi")) || (!strcmp(argv[arg], "--display-parent-iter")))
    {
        dc->display_parent_iter_num = TRUE;
    }
    else if ((!strcmp(argv[arg], "--reset")))
    {
        *dc = INITIAL_DISPLAY_CONTEXT;
        freecell_solver_user_set_iter_handler_long(
            instance,
            NULL,
            NULL
            );
        *num_to_skip = 0;
        return FCS_CMD_LINE_OK;
    }
    else
    {
        printf("Unimplemented option - \"%s\"!", argv[arg]);
        exit(-1);
    }
    *num_to_skip = 1;
    return FCS_CMD_LINE_SKIP;
}


static int command_num = 0;
static int debug_iter_output_on = FALSE;

static void select_signal_handler(int signal_num GCC_UNUSED)
{
    command_num = (command_num+1)%3;
}

static void * current_instance;
static fc_solve_display_information_context_t * dc;


static void command_signal_handler(int signal_num GCC_UNUSED)
{
    if (command_num == 0)
    {
        fprintf(
            stderr,
            "The number of iterations is %li\n",
            (long)freecell_solver_user_get_num_times_long(current_instance)
            );
    }
    else if (command_num == 1)
    {
        if (debug_iter_output_on)
        {
            freecell_solver_user_set_iter_handler_long(
                current_instance,
                NULL,
                NULL
                );
            debug_iter_output_on = FALSE;
        }
        else
        {
            freecell_solver_user_set_iter_handler_long(
                current_instance,
                my_iter_handler,
                dc
                );
            debug_iter_output_on = TRUE;
        }
    }
    else if (command_num == 2)
    {
        dc->debug_iter_state_output = ! dc->debug_iter_state_output;
    }

    command_num = 0;
}


static freecell_solver_str_t known_parameters[] = {
    "-h", "--help",
        "--help-configs", "--help-options", "--help-problems",
        "--help-real-help", "--help-short-sol", "--help-summary",
    "-i", "--iter-output",
    "-s", "--state-output",
    "-p", "--parseable-output",
    "-c", "--canonized-order-output",
    "-t", "--display-10-as-t",
    "-m", "--display-moves",
    "-sn", "--standard-notation",
    "-snx", "--standard-notation-extended",
    "-sam", "--display-states-and-moves",
    "-pi", "--display-parent-iter",
    "-o", "--output",
    "--reset",
    NULL
    };

#define USER_STATE_SIZE 1024

int main(int argc, char * argv[])
{
    FILE * file;
    char user_state[USER_STATE_SIZE];
    int ret;

    fc_solve_display_information_context_t debug_context = INITIAL_DISPLAY_CONTEXT;

    dc = &debug_context;

    int arg = 1;
    void * const instance = alloc_instance_and_parse(
        argc,
        argv,
        &arg,
        known_parameters,
        cmd_line_callback,
        &debug_context,
        FALSE
    );

    current_instance = instance;

    if ((arg == argc) || (!strcmp(argv[arg], "-")))
    {
        file = stdin;
        if (!getenv("FREECELL_SOLVER_QUIET"))
        {
            fprintf(stderr, "%s",
                    "Reading the board from the standard input.\n"
                    "Type \"fc-solve --help\" for more usage information.\n"
                    "To cancel this message set the FREECELL_SOLVER_QUIET environment variable.\n"
                   );
        }
    }
    else if (argv[arg][0] == '-')
    {
        fprintf(stderr,
                "Unknown option \"%s\". "
                "Type \"%s --help\" for usage information.\n",
                argv[arg],
                argv[0]
                );
        freecell_solver_user_free(instance);

        return -1;
    }
    else
    {
        file = fopen(argv[arg], "r");
        if (file == NULL)
        {
            fprintf(stderr,
                "Could not open file \"%s\" for input. Exiting.\n",
                argv[arg]
                );
            freecell_solver_user_free(instance);

            return -1;
        }
    }
    memset(user_state, '\0', sizeof(user_state));
    fread(user_state, sizeof(user_state[0]), USER_STATE_SIZE-1, file);
    fclose(file);

    /* Win32 Does not have those signals */
#ifndef WIN32
    signal(SIGUSR1, command_signal_handler);
    signal(SIGUSR2, select_signal_handler);
#endif

#if 0
    {
        fcs_int_limit_t limit = 500;
        freecell_solver_user_limit_iterations_long(instance, limit);
        ret = freecell_solver_user_solve_board(instance, user_state);
        while (ret == FCS_STATE_SUSPEND_PROCESS)
        {
            limit += 500;
            freecell_solver_user_limit_iterations_long(instance, limit);
            ret = freecell_solver_user_resume_solution(instance);
        }
    }
#else
    ret = freecell_solver_user_solve_board(instance, user_state);
#endif

    if (ret == FCS_STATE_INVALID_STATE)
    {
        char error_string[80];

        freecell_solver_user_get_invalid_state_error_into_string(
            instance,
            error_string
            FC_SOLVE__PASS_T(debug_context.display_10_as_t)
        );
        printf("%s\n", error_string);
    }
    else if (ret == FCS_STATE_FLARES_PLAN_ERROR)
    {
        printf("Flares Plan: %s\n", freecell_solver_user_get_last_error_string(instance));
    }
    else
    {
        FILE * output_fh;

        if (debug_context.output_filename)
        {
            output_fh = fopen(debug_context.output_filename, "wt");
            if (! output_fh)
            {
                fprintf(stderr,
                        "Could not open output file '%s' for writing!",
                        debug_context.output_filename
                       );
                return -1;
            }
        }
        else
        {
            output_fh = stdout;
        }

        fc_solve_output_result_to_file(
            output_fh, instance, ret, &debug_context
        );

        if (debug_context.output_filename)
        {
            fclose(output_fh);
            output_fh = NULL;
        }
    }

    freecell_solver_user_free(instance);

    return 0;
}

