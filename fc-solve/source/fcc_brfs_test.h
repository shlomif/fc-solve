/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
// fcc_brfs_test.h - the test interface declarations of fcc_brfs.h and
// fcc_brfs_test.c .
//
// This file is also included by the production code.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "dbm_common.h"

typedef unsigned char fcs_fcc_move;

#define FCS_FCC_NUM_MOVES_IN_ITEM 8

typedef struct fcs_fcc_moves_list_item_struct
{
    struct fcs_fcc_moves_list_item_struct *next;
    struct
    {
        fcs_fcc_move s[FCS_FCC_NUM_MOVES_IN_ITEM];
    } data;
} fcs_fcc_moves_list_item;

typedef struct
{
    int count;
    fcs_fcc_moves_list_item *moves_list;
} fcs_fcc_moves_seq;

typedef struct
{
    int count;
    fcs_fcc_move *moves;
    char *state_as_string;
} fcs_FCC_start_point_result;

DLLEXPORT int fc_solve_user_INTERNAL_find_fcc_start_points(fcs_dbm_variant_type,
    const char *, const int, const fcs_fcc_move *const,
    fcs_FCC_start_point_result **, long *const);

DLLEXPORT void fc_solve_user_INTERNAL_free_fcc_start_points(
    fcs_FCC_start_point_result *const);

DLLEXPORT int fc_solve_user_INTERNAL_is_fcc_new(fcs_dbm_variant_type,
    const char *, const char *,
    /* NULL-terminated */
    const char **,
    /* NULL-terminated */
    const char **, bool *const);

#ifdef __cplusplus
}
#endif
