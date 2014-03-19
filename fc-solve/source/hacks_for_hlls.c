/* Copyright (c) 2013 Shlomi Fish
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
 * hacks_for_hlls.c - the Freecell Solver utility methods for high-level
 * languages.
 */

#define BUILDING_DLL 1

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "set_weights.h"
#include "fcs_user.h"
#include "fcs_cl.h"
#include "split_cmd_line.h"

#include "prefix.h"
#include "inline.h"
#include "bool.h"

#include "cmd_line_enum.h"

DLLEXPORT long freecell_solver_user_move_ptr_to_string_w_state(
    void * user_instance,
    fcs_move_t * move_ptr,
    int standard_notation
)
{
    return (long)freecell_solver_user_move_to_string_w_state(
        user_instance,
        *move_ptr,
        standard_notation
    );
}

