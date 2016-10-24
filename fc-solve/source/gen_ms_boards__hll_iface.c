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
 * gen_ms_boards__hll_iface.c - high-level-language interface to the rand.
 */
#include "gen_ms_boards__hll_iface.h"
#include <string.h>

static fc_solve__hll_ms_rand_t singleton = {.gamenumber = 1, .seedx = 1};

fc_solve__hll_ms_rand_t *fc_solve__hll_ms_rand__get_singleton(void)
{
    return &singleton;
}

void fc_solve__hll_ms_rand__init(
    fc_solve__hll_ms_rand_t *const instance, const char *const gamenumber_s)
{
    const microsoft_rand_t gamenumber = atoll(gamenumber_s);
    const long long seedx = (microsoft_rand_uint_t)(
        (gamenumber < 0x100000000LL) ? gamenumber
                                     : (gamenumber - 0x100000000LL));
    instance->gamenumber = gamenumber;
    instance->seedx = seedx;
}

extern int fc_solve__hll_ms_rand__mod_rand(
    fc_solve__hll_ms_rand_t *const instance, const int limit)
{
    return (microsoft_rand__game_num_rand(
                &(instance->seedx), instance->gamenumber) %
            limit);
}
