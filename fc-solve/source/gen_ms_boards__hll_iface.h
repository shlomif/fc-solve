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
 * gen_ms_boards__hll_iface.h - high-level-language interface to the rand.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gen_ms_boards__rand.h"

typedef struct
{
    microsoft_rand_t gamenumber;
    microsoft_rand_t seedx;
} fc_solve__hll_ms_rand_t;

extern fc_solve__hll_ms_rand_t *fc_solve__hll_ms_rand__get_singleton(void);
extern void fc_solve__hll_ms_rand__init(
    fc_solve__hll_ms_rand_t *const instance, const char *const gamenumber);
extern int fc_solve__hll_ms_rand__mod_rand(
    fc_solve__hll_ms_rand_t *const instance, const int limit);

#ifdef __cplusplus
}
#endif
