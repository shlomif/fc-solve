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
 * handle_parsing.h - header file for convenient wrappers for handling
 * parsing the arguments.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "rinutils.h"
#include "fcs_cl.h"

enum
{
    EXIT_AND_RETURN_0 = FCS_CMD_LINE_USER
};

static GCC_INLINE void *alloc_instance_and_parse(const int argc,
    char **const argv, int *const arg_ptr,
    freecell_solver_str_t *const known_parameters,
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
            fprintf(stderr, "Unknown option: %s\n", argv[*arg_ptr]);
            exit(-1);
        }
        break;
    case FCS_CMD_LINE_PARAM_WITH_NO_ARG:
        fprintf(stderr, "The command line parameter \"%s\" requires an argument"
                        " and was not supplied with one.\n",
            argv[*arg_ptr]);
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
        exit(-1);
    }

    return instance;
}

static GCC_INLINE void *simple_alloc_and_parse(
    const int argc, char **const argv, int *const arg_ptr)
{
    return alloc_instance_and_parse(
        argc, argv, arg_ptr, NULL, NULL, NULL, TRUE);
}

#ifdef __cplusplus
}
#endif
