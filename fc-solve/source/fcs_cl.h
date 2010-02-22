
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
 * fcs_cl.h - the Freecell Solver command line arguments-like parsing routines.
 * Useful for more easily configuring a Freecell Solver instance.
 */
#ifndef FC_SOLVE__FCS_CL_H
#define FC_SOLVE__FCS_CL_H

#include "fcs_dllexport.h"
#include "fcs_user.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*freecell_solver_user_cmd_line_known_commands_callback_t)
    (
     void * instance,
     int argc,
     const char * argv[],
     int arg_index,
     int * num_to_skip,
     int * ret,
     void * context
     );

enum
{
    FCS_CMD_LINE_OK,
    FCS_CMD_LINE_SKIP,
    FCS_CMD_LINE_STOP,
    FCS_CMD_LINE_UNRECOGNIZED_OPTION,
    FCS_CMD_LINE_PARAM_WITH_NO_ARG,
    FCS_CMD_LINE_ERROR_IN_ARG,

    FCS_CMD_LINE_USER = 0x10000
};

DLLEXPORT extern int freecell_solver_user_cmd_line_parse_args(
    void * instance,
    int argc,
    const char * argv[],
    int start_arg,
    const char * * known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void * callback_context,
    char * * error_string,
    int * last_arg
    );

DLLEXPORT extern int freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
    void * instance,
    int argc,
    const char * argv[],
    int start_arg,
    const char * * known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void * callback_context,
    char * * error_string,
    int * last_arg,
    int file_nesting_count,
    const char * opened_files_dir
    );


#ifdef __cplusplus
};
#endif

#endif /* #ifndef FC_SOLVE__FCS_CL_H */
