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
 * app_str.c - implements an append-to-dynamically-growing string printf
 * functionality.
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "app_str.h"

#define GROW_BY 4000

fc_solve_append_string_t * fc_solve_append_string_alloc(int size_margin)
{
    fc_solve_append_string_t * app_str;

    if (size_margin > GROW_BY)
    {
        return NULL;
    }

    app_str = malloc(sizeof(fc_solve_append_string_t));
    app_str->max_size = GROW_BY;
    app_str->end_of_buffer = app_str->buffer = malloc(app_str->max_size);
    app_str->size_of_margin = size_margin;

    return app_str;
}

int fc_solve_append_string_sprintf(
    fc_solve_append_string_t * app_str,
    char * format,
    ...
    )
{
    int num_chars_written;
    va_list my_va_list;

    va_start(my_va_list, format);
    num_chars_written = vsprintf(app_str->end_of_buffer, format, my_va_list);
    app_str->end_of_buffer += num_chars_written;
    /*
     * Check to see if we don't have enough space in which case we should
     * resize
     * */
    if (app_str->buffer + app_str->max_size - app_str->end_of_buffer < app_str->size_of_margin)
    {
        char * old_buffer = app_str->buffer;
        app_str->max_size += GROW_BY;
        app_str->buffer = realloc(app_str->buffer, app_str->max_size);
        /*
         * Adjust end_of_buffer to the new buffer start
         * */
        app_str->end_of_buffer += app_str->buffer - old_buffer;
    }

    return num_chars_written;
}

char * fc_solve_append_string_finalize(
    fc_solve_append_string_t * app_str
    )
{
    char * ret;
    ret = strdup(app_str->buffer);
    free(app_str->buffer);
    free(app_str);
    return ret;
}
