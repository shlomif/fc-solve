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
 * var_base_writer.h - write to a packed integer using digits of variable
 * bases. Also see var_base_reader.h which complements it.
 */
#pragma once

#include <assert.h>
#include "var_base_int.h"

typedef struct
{
    fcs_var_base_int_t data;
    fcs_var_base_int_t multiplier;
    /* To avoid memory fragmentation, we keep those here and re use them. */
    fcs_var_base_int_t remainder;
} fcs_var_base_writer_t;

static GCC_INLINE void fc_solve_var_base_writer_init(
    fcs_var_base_writer_t *const s)
{
    FCS_var_base_int__init(s->data);
    FCS_var_base_int__init(s->multiplier);
    FCS_var_base_int__init(s->remainder);
}

static GCC_INLINE void fc_solve_var_base_writer_start(
    fcs_var_base_writer_t *const s)
{
    FCS_var_base_int__set_ui(s->data, 0);
    FCS_var_base_int__set_ui(s->multiplier, 1);
}

static GCC_INLINE void fc_solve_var_base_writer_write(
    fcs_var_base_writer_t *const w, const int base, const int item)
{
    assert(item >= 0);
    assert(item < base);

    FCS_var_base_int__addmul_ui(w->data, w->multiplier, ((unsigned long)item));

    FCS_var_base_int__mul_ui(w->multiplier, ((unsigned long)base));
}

static GCC_INLINE size_t fc_solve_var_base_writer_get_data(
    fcs_var_base_writer_t *const w, unsigned char *const exported)
{
    size_t count = 0;

#define NUM_BITS 8
    while (FCS_var_base_int__not_zero(w->data))
    {
#ifdef FCS_USE_INT128_FOR_VAR_BASE
        exported[count++] =
            (unsigned char)((w->data) & (((1 << NUM_BITS) - 1)));
        w->data >>= NUM_BITS;
#else
        mpz_fdiv_r_2exp(w->remainder, w->data, NUM_BITS);
        mpz_fdiv_q_2exp(w->data, w->data, NUM_BITS);
        exported[count++] =
            (unsigned char)FCS_var_base_int__get_ui(w->remainder);
#endif
    }

    return count;
}

static GCC_INLINE void fc_solve_var_base_writer_release(
    fcs_var_base_writer_t *const w)
{
    FCS_var_base_int__clear(w->data);
    FCS_var_base_int__clear(w->multiplier);
    FCS_var_base_int__clear(w->remainder);
}
