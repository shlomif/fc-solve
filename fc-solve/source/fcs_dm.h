/*
    fcs_dm.h - Header file for Freecell Solver's Data Management
    routines.

    For more information consult fcs_dm.c.

    Written by Shlomi Fish, 2000
    This file is distributed under the public domain.
    (It is not copyrighted)
*/

#ifndef __FCS_DATA_H
#define __FCS_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>


void * freecell_solver_bsearch
(
    void * key,
    void * void_array,
    size_t len,
    size_t width,
    int (* compare)(const void *, const void *, void *),
    void * context,
    int * found
);

int freecell_solver_merge_large_and_small_sorted_arrays
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

#endif /* __FCS_DATA_H */

