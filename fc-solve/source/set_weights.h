/* Copyright (c) 2012 Shlomi Fish
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
 * set_weights.h - header file of the set_weights function.
 */

#ifndef FC_SOLVE__SET_WEIGHTS_H
#define FC_SOLVE__SET_WEIGHTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <ctype.h>

#include "inline.h"
#include "alloc_wrap.h"
#include "fcs_user.h"

static GCC_INLINE void fc_solve_set_weights(
    freecell_solver_str_t start_num,
    double * const befs_weights
)
{
    for (int i=0 ; i<FCS_NUM_BEFS_WEIGHTS ; i++)
    {
        while (*start_num == ',')
        {
            start_num++;
        }
        if (! *start_num)
        {
            /* Initialize all the Best First Search weights at first
             * to 0 so
             * we won't have partial initialization.
             * */
            do
            {
                befs_weights[i++] = 0.0;
            } while (i < FCS_NUM_BEFS_WEIGHTS);

            return;
        }
        char * end_num;
        befs_weights[i] = strtod(start_num, &end_num);
        start_num = end_num;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__SET_WEIGHTS_H */
