/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// handle_parsing.h - header file for convenient wrappers for handling
// parsing the arguments.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rinutils.h"
#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
#include "freecell-solver/fcs_cl.h"
#else
#include "fcs_user.h"
#include "fc_solve__precompiled__theme.h"
#endif
enum
{
    EXIT_AND_RETURN_0 = FCS_CMD_LINE_USER
};

static inline void *alloc_instance_and_parse(const int argc GCC_UNUSED,
    char **const argv GCC_UNUSED, int *const arg_ptr GCC_UNUSED,
    freecell_solver_str_t *const known_parameters GCC_UNUSED,
    const freecell_solver_user_cmd_line_known_commands_callback_t callback
        GCC_UNUSED,
    void *const callback_context GCC_UNUSED,
    const bool only_recognized GCC_UNUSED)
{
    void *const instance = freecell_solver_user_alloc();
#ifdef FCS_USE_PRECOMPILED_CMD_LINE_THEME
    my_init_instance(instance);
#else
    FCS__DECL_ERR_PTR(error_string);
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
        fprintf(stderr,
            "The command line parameter \"%s\" requires an argument"
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
#endif
    return instance;
}

static inline void *simple_alloc_and_parse(
    const int argc, char **const argv, int arg)
{
    return alloc_instance_and_parse(argc, argv, &arg, NULL, NULL, NULL, TRUE);
}

static inline void print_flares_plan_error(void *const instance GCC_UNUSED)
{
#ifdef FCS_WITH_ERROR_STRS
    fprintf(stderr, "Flares Plan: %s\n",
        freecell_solver_user_get_last_error_string(instance));
#else
    fprintf(stderr, "%s\n", "Flares Plan Error");
#endif
}

#ifdef __cplusplus
}
#endif
