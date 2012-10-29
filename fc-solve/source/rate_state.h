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
 * alloc.h - the Freecell Solver compact allocator. Used to allocate
 * columns and other small allocations of a short size. Is able to revert the
 * last allocation.
 */
#ifndef FC_SOLVE__RATE_STATE_H
#define FC_SOLVE__RATE_STATE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <math.h>

#define FCS_BEFS_SEQS_OVER_RENEGADE_CARDS_EXPONENT 1.3

typedef double fc_solve_seq_cards_power_type_t;
extern fc_solve_seq_cards_power_type_t fc_solve_seqs_over_cards_lookup[2 * 13 * 4 + 1];

#if 0
#define FCS_SEQS_OVER_RENEGADE_POWER(n) pow(n, FCS_BEFS_SEQS_OVER_RENEGADE_CARDS_EXPONENT)
#else
#define FCS_SEQS_OVER_RENEGADE_POWER(n) fc_solve_seqs_over_cards_lookup[(n)]
#endif

#ifdef __cplusplus
};
#endif

#endif
