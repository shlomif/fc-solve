/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// alloc_wrap.h - convenient wrappers for malloc(), realloc(), etc.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/* Short for size-realloc. */
#define SREALLOC(arr, count) (realloc(arr, sizeof(arr[0]) * (count)))
#define SMALLOC(arr, count) (malloc(sizeof(arr[0]) * (count)))
#define SMALLOC1(ptr) SMALLOC(ptr, 1)

#ifdef __cplusplus
};
#endif
