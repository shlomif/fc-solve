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
 * dbm_calc_derived_iface.h - the public interface to dbm_calc_derived.h
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "dbm_common.h"

typedef struct
{
    unsigned char s[RANK_KING];
} fcs_which_moves_bitmask_t;

typedef struct
{
    char *state_string;
    unsigned char move;
    int core_irreversible_moves_count;
    int num_non_reversible_moves_including_prune;
    fcs_which_moves_bitmask_t which_irreversible_moves_bitmask;
} fcs_derived_state_debug_t;

DLLEXPORT int fc_solve_user_INTERNAL_calc_derived_states_wrapper(
    enum fcs_dbm_variant_type_t local_variant, const char *init_state_str_proto,
    int *num_out_derived_states, fcs_derived_state_debug_t **out_derived_states,
    const fcs_bool_t perform_horne_prune);

/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT int fc_solve_user_INTERNAL_perform_horne_prune(
    enum fcs_dbm_variant_type_t local_variant, const char *init_state_str_proto,
    char **ret_state_s);

#ifdef __cplusplus
}
#endif
