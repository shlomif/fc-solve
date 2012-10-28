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

#include "inline.h"
#include "alloc_wrap.h"
#include "fcs_user.h"

static GCC_INLINE void fc_solve_set_weights(
    const char * const s,
    double * const befs_weights
)
{
    int i;
    freecell_solver_str_t start_num;
    freecell_solver_str_t end_num;
    start_num = s;

    char * const substring_copy =
        SMALLOC(substring_copy, strlen(start_num) + 1);

    for (i=0 ; i<FCS_NUM_BEFS_WEIGHTS ; i++)
    {
        while ((*start_num > '9') && (*start_num < '0') && (*start_num != '\0'))
        {
            start_num++;
        }
        if (*start_num == '\0')
        {
            break;
        }
        end_num = start_num+1;
        while ((((*end_num >= '0') && (*end_num <= '9')) || (*end_num == '.')) && (*end_num != '\0'))
        {
            end_num++;
        }

        strncpy(substring_copy, start_num, end_num-start_num);
        substring_copy[end_num-start_num] = '\0';
        befs_weights[i] = atof(substring_copy);
        /* Make sure that if the string terminated here -
         * we stop.
         * */
        if (*end_num == '\0')
        {
            i++;
            break;
        }
        start_num=end_num+1;
    }

    free(substring_copy);

    /* Initialize all the Best Frist Search weights at first
     * to 0 so
     * we won't have partial initialization.
     * */
    for (; i<FCS_NUM_BEFS_WEIGHTS ; i++)
    {
        befs_weights[i] = 0.0;
    }
}

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__SET_WEIGHTS_H */
