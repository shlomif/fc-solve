/* Copyright (c) 2000 Shlomi Fish
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
 * app_str.h - implements an append-to-dynamically-growing string printf
 * functionality.
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "inline.h"

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

#define GROW_BY 4000
#define FC_SOLVE_APPEND_STRING_MARGIN_SIZE 500

static GCC_INLINE void fc_solve_append_string_init(fc_solve_append_string_t * app_str)
{
    app_str->max_size = GROW_BY;
    app_str->end_of_buffer = app_str->buffer = malloc(app_str->max_size);

    return;
}


static GCC_INLINE char * fc_solve_append_string_finalize(
    fc_solve_append_string_t * app_str
    )
{
    char * ret;
    ret = strdup(app_str->buffer);
    free(app_str->buffer);
    return ret;
}

extern void fc_solve_append_string_sprintf(
    fc_solve_append_string_t * app_str,
    char * format,
    ...
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FC_SOLVE__APP_STR_H */
