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
#include <stdio.h>

#include "rinutils.h"
#include "fcs_cl.h"

enum
{
    EXIT_AND_RETURN_0 = FCS_CMD_LINE_USER
};

static inline void *alloc_instance_and_parse(const int argc, char **const argv,
    int *const arg_ptr, freecell_solver_str_t *const known_parameters,
    const freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void *const callback_context, const fcs_bool_t only_recognized)
{
    void *const instance = freecell_solver_user_alloc();

#ifdef FCS_WITH_ERROR_STRS
    char *error_string;
#endif
    switch (freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
        instance, argc, (freecell_solver_str_t *)(void *)argv, (*arg_ptr),
        known_parameters, callback,
        callback_context FCS__PASS_ERR_STR(&error_string), arg_ptr, -1, NULL))
    {
    case EXIT_AND_RETURN_0:
        freecell_solver_user_free(instance);
        exit(0);

    case FCS_CMD_LINE_UNRECOGNIZED_OPTION:
        if (only_recognized)
        {
            fc_solve_err("Unknown option: %s\n", argv[*arg_ptr]);
        }
        break;
    case FCS_CMD_LINE_PARAM_WITH_NO_ARG:
        fprintf(stderr, "The command line parameter \"%s\" requires an argument"
                        " and was not supplied with one.\n",
            argv[*arg_ptr]);
        freecell_solver_user_free(instance);
        exit(-1);
    case FCS_CMD_LINE_ERROR_IN_ARG:
#ifdef FCS_WITH_ERROR_STRS
        if (error_string)
        {
            fprintf(stderr, "%s", error_string);
            free(error_string);
        }
#else
        fprintf(stderr, "%s\n", "Error in command line arg.");
#endif
        freecell_solver_user_free(instance);
        exit(-1);
    }

    return instance;
}

static inline void *simple_alloc_and_parse(
    const int argc, char **const argv, int *const arg_ptr)
{
    return alloc_instance_and_parse(
        argc, argv, arg_ptr, NULL, NULL, NULL, TRUE);
}

#define USER_STATE_SIZE 1024

typedef struct
{
    char s[USER_STATE_SIZE];
} fcs_user_state_str_t;

static inline fcs_user_state_str_t read_state(FILE *f)
{
    fcs_user_state_str_t user_state;
    memset(user_state.s, '\0', sizeof(user_state));
    fread(user_state.s, sizeof(user_state.s[0]), USER_STATE_SIZE - 1, f);
    fclose(f);

    return user_state;
}

typedef struct
{
    const char *key;
    const char *screen;
} help_screen_t;

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

static int fc_solve__cmd_line_callback(void *const instance, const int argc,
    const char *argv[], const int arg, int *const num_to_skip, int *const ret,
    void *const context)
{
    fc_solve_display_information_context_t *const display_context =
        (fc_solve_display_information_context_t *)context;
    const char *s;

    *num_to_skip = 0;

    const char *const arg_str = argv[arg];

    if (IS_ARG("--version"))
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
