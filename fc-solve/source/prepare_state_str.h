/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#pragma once

#include <string.h>

static char *prepare_state_str(const char *const input_str)
{
    char *const ret = strdup(input_str);

    /* Process the string in-place to make it available as input
     * to fc-solve again.
     * */

    {
        char *s, *d;
        char c;
        s = d = ret;

        while ((c = *(d++) = *(s++)))
        {
            if ((c == '\n') && (s[0] == ':'))
            {
                s++;
                while (*s == ' ')
                {
                    s++;
                }
            }
        }
    }

    return ret;
}
