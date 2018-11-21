/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
/*
 * set_weights.h - header file of the set_weights function.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rinutils.h"
#include "freecell-solver/fcs_user.h"

static inline void fc_solve_set_weights(freecell_solver_str_t start_num,
    const freecell_solver_str_t string_end,
    fc_solve_weighting_float *const befs_weights)
{
    for (int i = 0; i < FCS_NUM_BEFS_WEIGHTS; ++i)
    {
        while (*start_num == ',')
        {
            start_num++;
        }
        if (start_num >= string_end)
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
        char *end_num;
        const_AUTO(val, strtod(start_num, &end_num));
        befs_weights[i] = max(val, 0.0);
        start_num = end_num;
    }
}

#ifdef __cplusplus
}
#endif
