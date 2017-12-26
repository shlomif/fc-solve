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
 * fcs_dm.h - Header file for Freecell Solver's Data Management routines.
 *
 * For more information consult fcs_dm.c.
 */

#ifndef FC_SOLVE__FCS_DATA_H
#define FC_SOLVE__FCS_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include "bool.h"

void * fc_solve_bsearch
(
    void * key,
    void * void_array,
    size_t len,
    size_t width,
    int (* compare)(const void *, const void *, void *),
    void * context,
    fcs_bool_t * found
);

int fc_solve_merge_large_and_small_sorted_arrays
(
    void * void_big_array,
    size_t size_big_array,
    void * void_small_array,
    size_t size_small_array,
    size_t width,
    int (*compare) (const void *, const void *, void *),
    void * context
);

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__FCS_DATA_H */
