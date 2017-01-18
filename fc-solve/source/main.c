/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * main.c - the main() and related functions of the fc-solve command line
 * executable.
 *
 * It is documented in the documents "README", "USAGE", etc. in the
 * Freecell Solver distribution from http://fc-solve.shlomifish.org/ .
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "fcs_cl.h"
#include "rinutils.h"
#include "output_to_file.h"
#include "handle_parsing.h"
#include "read_state.h"

typedef struct
{
    const char *key;
    const char *screen;
} help_screen_t;

#include "cl_callback_common.h"
static int fc_solve__cmd_line_callback(void *const instance, const int argc,
    const char *argv[], const int arg, int *const num_to_skip, int *const ret,
    void *const context)
{
    fc_solve_display_information_context_t *const display_context =
        (fc_solve_display_information_context_t *)context;
    const char *s;

    *num_to_skip = 0;

    const char *const arg_str = argv[arg];

    if (cmd_line_cb__handle_common(arg_str, instance, display_context))
    {
    }
    else if (IS_ARG("--version"))
    {
        printf("fc-solve\nlibfreecell-solver version %s\n",
            freecell_solver_user_get_lib_version(instance));
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if (IS_ARG("-h") || IS_ARG("--help"))
    {
        const char *help_key = getenv("FREECELL_SOLVER_DEFAULT_HELP");
        if (help_key == NULL)
        {
            help_key = "summary";
        }
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if ((s = try_str_prefix(arg_str, "--help-")))
    {
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if (IS_ARG("-o") || IS_ARG("--output"))
    {
        const int next_arg = arg + 1;
        if (next_arg == argc)
        {
            return FCS_CMD_LINE_STOP;
        }
        *num_to_skip = 2;
        display_context->output_filename = (const char *)argv[next_arg];
        return FCS_CMD_LINE_SKIP;
    }
    else if (IS_ARG("--reset"))
    {
        *display_context = INITIAL_DISPLAY_CONTEXT;
        freecell_solver_user_set_iter_handler_long(instance, NULL, NULL);
        *num_to_skip = 0;
        return FCS_CMD_LINE_OK;
    }
    else
    {
        printf("Unimplemented option - \"%s\"!", arg_str);
        exit(-1);
    }
    *num_to_skip = 1;
    return FCS_CMD_LINE_SKIP;
}

#undef IS_ARG

static freecell_solver_str_t known_parameters[] = {"-h", "--help",
    "--help-configs", "--help-options", "--help-problems", "--help-real-help",
    "--help-short-sol", "--help-summary", "-i", "--iter-output", "-s",
    "--state-output", "-p", "--parseable-output", "-c",
    "--canonized-order-output", "-t", "--display-10-as-t", "-m",
    "--display-moves", "-sn", "--standard-notation", "-snx",
    "--standard-notation-extended", "-sam", "--display-states-and-moves", "-pi",
    "--display-parent-iter", "-sel", "--show-exceeded-limits", "-o", "--output",
    "--reset", "--version", NULL};

typedef enum {
    SUCCESS = 0,
    ERROR = -1,
} exit_code_t;

static inline int fc_solve_main__main(int argc, char *argv[])
{
    fc_solve_display_information_context_t display_context =
        INITIAL_DISPLAY_CONTEXT;

    int arg = 1;
    void *const instance = alloc_instance_and_parse(argc, argv, &arg,
        known_parameters, fc_solve__cmd_line_callback, &display_context, FALSE);
}
#include "default_iter_handler.h"

int main(int argc, char *argv[]) { return fc_solve_main__main(argc, argv); }
