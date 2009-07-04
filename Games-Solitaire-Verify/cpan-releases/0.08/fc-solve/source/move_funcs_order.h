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
 * move_funcs_order.h - contains the fc_solve_apply_tests_order function.
 *
 */

#ifndef FC_SOLVE__MOVE_FUNCS_ORDER_H
#define FC_SOLVE__MOVE_FUNCS_ORDER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "instance.h"

#include "inline.h"

static GCC_INLINE int fc_solve_char_to_test_num(char c)
{
    if ((c >= '0') && (c <= '9'))
    {
        return c-'0';
    }
    else if ((c >= 'a') && (c <= 'h'))
    {
        return c-'a'+10;
    }
    else if ((c >= 'A') && (c <= 'Z'))
    {
        return c-'A'+18;
    }
    else
    {
        return 0;
    }
}

extern int fc_solve_apply_tests_order(
    fcs_tests_order_t * tests_order,
    const char * string,
    char * * error_string
    );

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__MOVE_FUNCS_ORDER_H */
