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
 * app_str.c - implements an append-to-dynamically-growing string printf
 * functionality.
 */
#define BUILDING_DLL 1

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "alloc_wrap.h"
#include "app_str.h"

void fc_solve_append_string_sprintf(
    fc_solve_append_string_t * const app_str,
    const char * const format,
    ...
    )
{
    va_list my_va_list;

    va_start(my_va_list, format);
    app_str->end_of_buffer += vsprintf(app_str->end_of_buffer, format, my_va_list);
    va_end(my_va_list);

    /*
     * Check to see if we don't have enough space in which case we should
     * resize
     * */
    if (app_str->buffer + app_str->max_size - app_str->end_of_buffer < FC_SOLVE_APPEND_STRING_MARGIN_SIZE)
    {
        char * const old_buffer = app_str->buffer;
        app_str->max_size += FC_SOLVE_APPEND_STRING_GROW_BY;
        app_str->buffer = SREALLOC(app_str->buffer, app_str->max_size);
        /*
         * Adjust end_of_buffer to the new buffer start
         * */
        app_str->end_of_buffer += app_str->buffer - old_buffer;
    }

    return;
}
