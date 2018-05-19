/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2010 Shlomi Fish
 */
// google_hash.h - header file for Google's dense_hash_map as adapted
// for Freecell Solver.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "fcs_conf.h"
#include "rinutils.h"

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)

typedef void *fcs_states_google_hash_handle;

extern fcs_states_google_hash_handle fc_solve_states_google_hash_new(void);

/*
 * Returns 0 if the key is new and the key/val pair was inserted.
 *      - in that case *existing_key / *existing_val will be set to key
 *      and val respectively.
 * Returns 1 if the key is not new and *existing_key / *existing_val
 * was set to it.
 */
extern bool fc_solve_states_google_hash_insert(
    fcs_states_google_hash_handle hash, void *key, void **existing_key);

void fc_solve_states_google_hash_free(fcs_states_google_hash_handle hash);

extern void fc_solve_states_google_hash_foreach(
    fcs_states_google_hash_handle hash,
    bool (*should_delete_ptr)(void *key, void *context), void *context);

#endif

#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)

typedef void *fcs_columns_google_hash_handle;

extern fcs_columns_google_hash_handle fc_solve_columns_google_hash_new(void);

/*
 * Returns 0 if the key is new and the key/val pair was inserted.
 *      - in that case *existing_key / *existing_val will be set to key
 *      and val respectively.
 * Returns 1 if the key is not new and *existing_key / *existing_val
 * was set to it.
 */
extern bool fc_solve_columns_google_hash_insert(
    fcs_columns_google_hash_handle hash, void *key, void **existing_key);

void fc_solve_columns_google_hash_free(fcs_columns_google_hash_handle hash);

#endif

#ifdef __cplusplus
}
#endif
