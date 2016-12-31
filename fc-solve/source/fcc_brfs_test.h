/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
/*
 * fcc_brfs_test.h - the test interface declarations of fcc_brfs.h and
 * fcc_brfs_test.c .
 *
 * This file is also included by the production code.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "dbm_common.h"

typedef unsigned char fcs_fcc_move_t;

#define FCS_FCC_NUM_MOVES_IN_ITEM 8

typedef struct fcs_fcc_moves_list_item_struct
{
    struct fcs_fcc_moves_list_item_struct *next;
    struct
    {
        fcs_fcc_move_t s[FCS_FCC_NUM_MOVES_IN_ITEM];
    } data;
} fcs_fcc_moves_list_item_t;

typedef struct
{
    int count;
    fcs_fcc_moves_list_item_t *moves_list;
} fcs_fcc_moves_seq_t;

typedef struct
{
    int count;
    fcs_fcc_move_t *moves;
    char *state_as_string;
} fcs_FCC_start_point_result_t;

DLLEXPORT int fc_solve_user_INTERNAL_find_fcc_start_points(
    fcs_dbm_variant_type_t local_variant, const char *init_state_str_proto,
    const int start_state_moves_count,
    const fcs_fcc_move_t *const start_state_moves,
    fcs_FCC_start_point_result_t **out_fcc_start_points,
    long *const out_num_new_positions);

DLLEXPORT void fc_solve_user_INTERNAL_free_fcc_start_points(
    fcs_FCC_start_point_result_t *const out_fcc_start_points);

DLLEXPORT int fc_solve_user_INTERNAL_is_fcc_new(
    fcs_dbm_variant_type_t local_variant, const char *init_state_str_proto,
    const char *start_state_str_proto,
    /* NULL-terminated */
    const char **min_states,
    /* NULL-terminated */
    const char **states_in_cache, fcs_bool_t *const out_is_fcc_new);

#ifdef __cplusplus
}
#endif
