/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
#pragma once

static void trim_trailing_whitespace(char *const as_str)
{
    char *dest = as_str;
    char *src = as_str;

    while (1)
    {
        while (!(*(src) == ' ' || *(src) == '\0'))
        {
            *(dest++) = *(src++);
        }
        if (*(src) == '\0')
        {
            break;
        }
        char *prev_src = src;
        while (*src == ' ')
        {
            ++src;
        }
        if (*(src) == '\0')
        {
            break;
        }
        if (*src != '\n')
        {
            while (*prev_src == ' ')
            {
                *(dest++) = *(prev_src++);
            }
        }
    }
    *(dest++) = '\0';
}
