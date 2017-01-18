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
 * output_to_file.h - header file for outputting a solution to a file.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "rinutils.h"
#include "fcs_enums.h"
#include "fcs_user.h"

typedef struct
{
    const char *output_filename;
    int standard_notation;
    fcs_bool_t debug_iter_state_output;
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    fcs_bool_t parseable_output;
#endif
    fcs_bool_t canonized_order_output;
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    fcs_bool_t display_10_as_t;
#endif
    fcs_bool_t display_parent_iter_num;
    fcs_bool_t debug_iter_output_on;
    fcs_bool_t display_moves;
    fcs_bool_t display_states;
    fcs_bool_t show_exceeded_limits;
} fc_solve_display_information_context_t;

static const fc_solve_display_information_context_t INITIAL_DISPLAY_CONTEXT = {
    .debug_iter_state_output = FALSE,
#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    .parseable_output = FALSE,
#endif
    .canonized_order_output = FALSE,
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    .display_10_as_t = FALSE,
#endif
    .display_parent_iter_num = FALSE,
    .display_moves = FALSE,
    .display_states = TRUE,
    .standard_notation = FC_SOLVE__STANDARD_NOTATION_NO,
    .output_filename = NULL,
    .show_exceeded_limits = FALSE};

static inline void fc_solve_output_result_to_file(FILE *const output_fh,
    void *const instance, const int ret,
    const fc_solve_display_information_context_t *const dc_ptr)
{
}

#ifdef __cplusplus
}
#endif
