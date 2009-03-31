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
 * TODO : Add a description of this file.
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef FC_SOLVE__APP_STR_H
#define FC_SOLVE__APP_STR_H

#ifdef __cplusplus
extern "C" {
#endif

struct fc_solve_append_string_struct
{
    char * buffer;
    char * end_of_buffer;
    int max_size;
    int size_of_margin;
};

typedef struct fc_solve_append_string_struct fc_solve_append_string_t;

extern fc_solve_append_string_t * fc_solve_append_string_alloc(int size_margin);

extern int fc_solve_append_string_sprintf(
    fc_solve_append_string_t * app_str,
    char * format,
    ...
    );

extern char * fc_solve_append_string_finalize(
    fc_solve_append_string_t * app_str
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FC_SOLVE__APP_STR_H */
