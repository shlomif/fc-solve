/* Copyright (c) 2010 Shlomi Fish
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
 * portable_int32.h - the Freecell Solver (mostly) portable 32-bit/etc. int
 * typedefs.
 */
#ifndef FC_SOLVE__PORTABLE_INT32_H
#define FC_SOLVE__PORTABLE_INT32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

    /* Taken from:
     * http://www.binarytides.com/data-type-u_int8_t-u_int16_t-u_int32_t-on-windows/
     * */
#if defined(_WIN32)

#include <stdint.h>

typedef unsigned char u_char;
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;

#endif

#ifdef __cplusplus
};
#endif

#endif /* #ifndef FC_SOLVE__PORTABLE_INT32_H */
