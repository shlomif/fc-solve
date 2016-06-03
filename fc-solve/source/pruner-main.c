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

#include "cl_callback.h"

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
        fc_solve__cmd_line_callback,
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

