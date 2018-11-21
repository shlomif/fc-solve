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

#include <signal.h>

#include "freecell-solver/fcs_cl.h"
#include "rinutils.h"
#include "output_to_file.h"
#include "handle_parsing.h"
#include "read_state.h"

typedef struct
{
    const char *key;
    const char *screen;
} help_screen;

static help_screen help_screens[] = {
    {"configs",
        "These configurations are usually faster than the unmodified run:\n"
        "\n"
        "    fc-solve -l cookie-monster\n"
        "    fc-solve -l one-big-family\n"
        "\n"
        "Or if you want an accurate verdict:\n"
        "\n"
        "    fc-solve -l toons-for-twenty-somethings\n"
        "\n"
        "If you want to try constructing your own configurations refer to the\n"
        "USAGE file in the Freecell Solver distribution\n"},
    {"options", "fc-solve [options] board_file\n"
                "\n"
                "If board_file is - or unspecified reads standard input\n"
                "\n"
                "See http://fc-solve.shlomifish.org/docs/distro/USAGE.html .\n"
                "\n"
                "(The listing here suffered from lack of maintenance and so "
                "was removed.\n"
                "\n"
                "Freecell Solver was written by Shlomi Fish.\n"
                "Homepage: http://fc-solve.shlomifish.org/\n"
                "Send comments and suggestions to "
                "http://www.shlomifish.org/me/contact-me/\n"},
    {"real-help", "The environment variable FREECELL_SOLVER_DEFAULT_HELP sets "
                  "the default help\n"
                  "screen. The name of the help screen is the same name as its "
                  "\"--help-\" flag\n"
                  "but without the preceding \"--help-\". Type:\n"
                  "\n"
                  "    fc-solve --help-summary\n"
                  "\n"
                  "for the available help screens.\n"
                  "\n"
                  "Refer to your system's documentation for information on how "
                  "to set environment\n"
                  "variables.\n"},
    {"problems", "To be discussed.\n"},
    {"short-sol",
        "The following configurations may produce shorter solutions:\n"
        "\n"
        "    fc-solve -l video-editing\n"
        "    fc-solve -l children-playing-ball\n"
        "    fc-solve -l slick-rock\n"
        "\n"
        "You may also try adding the \"-opt\" and/or \"--reparent-states\" "
        "options\n"
        "which may make things a little better.\n"
        "\n"
        "Refer to the file 'USAGE' for more information.\n"},
    {"summary",
        "fc-solve [flags] [board_file|-]\n"
        "\n"
        "Reads board from standard input by default or if a \"-\" is "
        "specified.\n"
        "\n"
        "- If it takes too long to finish, type \"fc-solve --help-configs\"\n"
        "- If it erroneously reports a board as unsolvable, try adding the\n"
        "  \"-to 01ABCDE\" flag\n"
        "- If the solution is too long type \"fc-solve --help-short-sol\"\n"
        "- To present the moves only try adding \"-m\" or \"-m -snx\"\n"
        "- For a description of all options type \"fc-solve --help-options\"\n"
        "- To deal with other problems type \"fc-solve --help-problems\"\n"
        "- To turn --help into something more useful, type\n"
        "  \"fc-solve --help-real-help\"\n"
        "\n"
        "Contact Shlomi Fish, http://www.shlomifish.org/ for more "
        "information.\n"},
    {NULL, NULL}};

static void print_help_string(const char *key)
{
    int i;
    for (i = 0; help_screens[i].key != NULL; i++)
    {
        if (!strcmp(key, help_screens[i].key))
        {
            printf("%s", help_screens[i].screen);
        }
    }
}

#ifndef FCS_WITHOUT_ITER_HANDLER
static void my_iter_handler(
    void *, fcs_int_limit_t, int, void *, fcs_int_limit_t, void *);
#endif
#include "cl_callback_common.h"
static int fc_solve__cmd_line_callback(void *const instance, const int argc,
    const char **const argv, const int arg, int *const num_to_skip,
    int *const ret, void *const context)
{
    fc_solve_display_information_context *const display_context =
        (fc_solve_display_information_context *)context;
    const char *s;
    *num_to_skip = 0;

    const char *const arg_str = argv[arg];

    if (cmd_line_cb__handle_common(arg_str, instance, display_context))
    {
    }
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    else if (IS_ARG("--version"))
    {
        printf("fc-solve\nlibfreecell-solver version %s\n",
            freecell_solver_user_get_lib_version(instance));
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
#endif
    else if (IS_ARG_LONG("h", "help"))
    {
        const char *help_key = getenv("FREECELL_SOLVER_DEFAULT_HELP");
        print_help_string(help_key ? help_key : "summary");
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if ((s = try_str_prefix(arg_str, "--help-")))
    {
        print_help_string(s);
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if (IS_ARG_LONG("o", "output"))
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
    else if (IS_ARG("--iter-output-step"))
    {
        const int next_arg = arg + 1;
        if (next_arg == argc)
        {
            return FCS_CMD_LINE_STOP;
        }
        *num_to_skip = 2;
        display_context->iters_display_step = (size_t)atol(argv[next_arg]);
        return FCS_CMD_LINE_SKIP;
    }
    else if (IS_ARG("--reset"))
    {
        *display_context = INITIAL_DISPLAY_CONTEXT;
#ifndef FCS_WITHOUT_ITER_HANDLER
        freecell_solver_user_set_iter_handler_long(instance, NULL, NULL);
#endif
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

static void *instance;
static fc_solve_display_information_context display_context;
#ifndef WIN32
static int command_num = 0;
#ifndef FCS_WITHOUT_ITER_HANDLER
static int debug_iter_output_on = FALSE;
#endif

static void select_signal_handler(int signal_num GCC_UNUSED)
{
    command_num = (command_num + 1) % 3;
}

static void command_signal_handler(int signal_num GCC_UNUSED)
{
    switch (command_num)
    {
    case 0:
        fprintf(stderr, "The number of iterations is %li\n",
            (long)freecell_solver_user_get_num_times_long(instance));
        break;

    case 1:
#ifndef FCS_WITHOUT_ITER_HANDLER
        if (debug_iter_output_on)
        {
            freecell_solver_user_set_iter_handler_long(instance, NULL, NULL);
            debug_iter_output_on = FALSE;
        }
        else
        {
            freecell_solver_user_set_iter_handler_long(
                instance, my_iter_handler, &display_context);
            debug_iter_output_on = TRUE;
        }
#endif
        break;

    case 2:
        display_context.debug_iter_state_output =
            !display_context.debug_iter_state_output;
        break;
    }
    command_num = 0;
}

static void abort_signal_handler(int signal_num GCC_UNUSED)
{
#ifndef FCS_WITHOUT_MAX_NUM_STATES
    freecell_solver_user_limit_iterations_long(instance, 0);
#endif
}
#endif

static freecell_solver_str_t known_parameters[] = {"-h", "--help",
    "--help-configs", "--help-options", "--help-problems", "--help-real-help",
    "--help-short-sol", "--help-summary", "-i", "--iter-output", "-s",
    "--state-output", "-p", "--parseable-output", "-c",
    "--canonized-order-output", "-t", "--display-10-as-t", "-m",
    "--display-moves", "-sn", "--standard-notation", "-snx",
    "--standard-notation-extended", "-sam", "--display-states-and-moves", "-pi",
    "--display-parent-iter", "-sel", "--show-exceeded-limits", "-o", "--output",
    "-hoi", "--hint-on-intractable", "--iter-output-step", "--reset",
    "--version", NULL};

typedef enum
{
    SUCCESS = 0,
    ERROR = -1,
} exit_code_type;

#if 0
static inline int solve_board(void * const instance, const char * const user_state)
{
    fcs_int_limit_t limit = 500;
    freecell_solver_user_limit_iterations_long(instance, limit);
    int ret = freecell_solver_user_solve_board(instance, user_state);
    while (ret == FCS_STATE_SUSPEND_PROCESS)
    {
        limit += 500;
        freecell_solver_user_limit_iterations_long(instance, limit);
        ret = freecell_solver_user_resume_solution(instance);
    }
    return ret;
}
#else
#define solve_board(instance, user_state)                                      \
    freecell_solver_user_solve_board((instance), (user_state))
#endif
