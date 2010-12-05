/* Copyright (c) 2010 Shlomi Fish
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
 * google_hash.h - header file for Google's dense_hash_map as adapted
 * for Freecell Solver.
 */

#ifndef FC_SOLVE__GOOGLE_HASH_H
#define FC_SOLVE__GOOGLE_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#include "bool.h"

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)

typedef void * fcs_states_google_hash_handle_t;

extern fcs_states_google_hash_handle_t fc_solve_states_google_hash_new();

/*
 * Returns 0 if the key is new and the key/val pair was inserted.
 *      - in that case *existing_key / *existing_val will be set to key
 *      and val respectively.
 * Returns 1 if the key is not new and *existing_key / *existing_val
 * was set to it.
 */
extern fcs_bool_t fc_solve_states_google_hash_insert(
    fcs_states_google_hash_handle_t hash,
    void * key,
    void * * existing_key
    );

void fc_solve_states_google_hash_free(fcs_states_google_hash_handle_t hash);

extern void fc_solve_states_google_hash_foreach(
    fcs_states_google_hash_handle_t hash,
    fcs_bool_t (*should_delete_ptr)(void * key, void * context),
    void * context
    );

#endif

#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)

typedef void * fcs_columns_google_hash_handle_t;

extern fcs_columns_google_hash_handle_t fc_solve_columns_google_hash_new();

/*
 * Returns 0 if the key is new and the key/val pair was inserted.
 *      - in that case *existing_key / *existing_val will be set to key
 *      and val respectively.
 * Returns 1 if the key is not new and *existing_key / *existing_val
 * was set to it.
 */
extern fcs_bool_t fc_solve_columns_google_hash_insert(
    fcs_columns_google_hash_handle_t hash,
    void * key,
    void * * existing_key
    );

void fc_solve_columns_google_hash_free(fcs_columns_google_hash_handle_t hash);

#endif

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE__GOOGLE_HASH_H */
