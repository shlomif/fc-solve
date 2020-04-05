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
 * fcs_strndup.h: provide strndup.
 */
#ifndef FC_SOLVE__STRNDUP_H
#define FC_SOLVE__STRNDUP_H

#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "inline.h"

#ifndef HAVE_STRNDUP
/*
 * This implementation of strndup is taken from:
 * http://code.google.com/p/madp-win/source/browse/src/argp-standalone-1.3/strndup.c?r=2d96025e8ad4b150317ff6f0ff8d75c59a83cf97
 *
 * The notice there reads:
 *
 * <<<
 * Written by Niels Möller <nisse@lysator.liu.se>
 *
 * This file is hereby placed in the public domain.
 * >>>
 *
 * */
static GCC_INLINE char * strndup (const char *s, size_t size)
{
    char *r;
    char *end = memchr(s, 0, size);

    if (end)
    {
        /* Length + 1 */
        size = end - s + 1;
    }

    r = malloc(size);

    if (size)
    {
        memcpy(r, s, size-1);
        r[size-1] = '\0';
    }
    return r;
}

#endif

#endif /* #ifndef FC_SOLVE__STRNDUP_H */
