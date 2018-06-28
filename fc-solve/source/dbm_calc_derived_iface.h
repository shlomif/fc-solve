/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
// dbm_calc_derived_iface.h - the public interface to dbm_calc_derived.h
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "dbm_common.h"

typedef struct
{
    unsigned char s[RANK_KING];
} fcs_which_moves_bitmask;

typedef struct
{
    char *state_string;
    unsigned char move;
    int core_irreversible_moves_count;
    int num_non_reversible_moves_including_prune;
    fcs_which_moves_bitmask which_irreversible_moves_bitmask;
} fcs_derived_state_debug;

DLLEXPORT int fc_solve_user_INTERNAL_calc_derived_states_wrapper(
    fcs_dbm_variant_type, const char *, int *, fcs_derived_state_debug **,
    bool);

DLLEXPORT void fc_solve_user_INTERNAL_free_derived_states(
    const int num_derived_states,
    fcs_derived_state_debug *const derived_states);

// The char * returned is malloc()ed and should be free()ed.
DLLEXPORT int fc_solve_user_INTERNAL_perform_horne_prune(
    fcs_dbm_variant_type, const char *, char **);

DLLEXPORT void fc_solve_user_INTERNAL_perform_horne_prune__free_ret_state_s(
    char *const ret_state_s);

#ifdef __cplusplus
}
#endif
