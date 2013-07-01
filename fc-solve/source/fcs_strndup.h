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

