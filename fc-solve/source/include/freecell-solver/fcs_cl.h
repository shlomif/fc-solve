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
 * fcs_cl.h - the Freecell Solver command line arguments-like parsing routines.
 * Useful for more easily configuring a Freecell Solver instance.
 */
#pragma once

#include "freecell-solver/fcs_dllexport.h"
#include "freecell-solver/fcs_user.h"
#include "freecell-solver/fcs_back_compat.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*freecell_solver_user_cmd_line_known_commands_callback_t)(
    void *instance, int argc, freecell_solver_str_t argv[], int arg_index,
    int *num_to_skip, int *ret, void *context);

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

/* One should use
 * freecell_solver_user_cmd_line_parse_args_with_file_nesting_count()
 * instead.
 * */
#ifndef FC_SOLVE__REMOVE_OLD_API_1
DLLEXPORT extern int freecell_solver_user_cmd_line_parse_args(void *instance,
    int argc, freecell_solver_str_t argv[], int start_arg,
    freecell_solver_str_t *known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void *callback_context FCS__PASS_ERR_STR(char **error_string),
    int *last_arg);
#endif

DLLEXPORT extern int
freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(void *instance,
    int argc, freecell_solver_str_t argv[], int start_arg,
    freecell_solver_str_t *known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void *callback_context FCS__PASS_ERR_STR(char **error_string),
    int *last_arg, int file_nesting_count,
    freecell_solver_str_t opened_files_dir);

DLLEXPORT extern int freecell_solver_user_cmd_line_read_cmd_line_preset(
    void *instance, const char *const preset_name,
    freecell_solver_str_t *known_parameters FCS__PASS_ERR_STR(
        char **error_string),
    int file_nesting_count, freecell_solver_str_t opened_files_dir);

#ifdef __cplusplus
};
#endif
