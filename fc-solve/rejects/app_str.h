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
 * app_str.h - implements an append-to-dynamically-growing string printf
 * functionality.
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "inline.h"
#include "alloc_wrap.h"

#ifndef FC_SOLVE__APP_STR_H
#define FC_SOLVE__APP_STR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    char * buffer;
    char * end_of_buffer;
    int max_size;
} fc_solve_append_string_t;

#define FC_SOLVE_APPEND_STRING_GROW_BY 4000
#define FC_SOLVE_APPEND_STRING_MARGIN_SIZE 500

static GCC_INLINE void fc_solve_append_string_init(fc_solve_append_string_t * const app_str)
{
    app_str->end_of_buffer = app_str->buffer =
        SMALLOC(app_str->buffer,
            app_str->max_size = FC_SOLVE_APPEND_STRING_GROW_BY
            );
}

static GCC_INLINE char * fc_solve_append_string_finalize(
    fc_solve_append_string_t * const app_str
    )
{
    char * const ret = strdup(app_str->buffer);
    free(app_str->buffer);
    return ret;
}

extern void fc_solve_append_string_sprintf(
    fc_solve_append_string_t * app_str,
    const char * format,
    ...
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FC_SOLVE__APP_STR_H */
