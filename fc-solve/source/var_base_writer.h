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
 *
 */
#ifndef FC_SOLVE__VAR_BASE_WRITER_H
#define FC_SOLVE__VAR_BASE_WRITER_H

#include <assert.h>
#include <gmp.h>

#include "inline.h"

typedef struct
{
    mpz_t data;
    mpz_t multiplier;
    /* To avoid memory fragmentation, we keep those here and re use them. */
    mpz_t remainder;
} fcs_var_base_writer_t;

static GCC_INLINE void fc_solve_var_base_writer_init(fcs_var_base_writer_t * s)
{
    mpz_init(s->data);
    mpz_init(s->multiplier);
    mpz_init(s->remainder);
}

static GCC_INLINE void fc_solve_var_base_writer_start(fcs_var_base_writer_t * s)
{
    mpz_set_ui(s->data, 0);
    mpz_set_ui(s->multiplier, 1);
}

static GCC_INLINE void fc_solve_var_base_writer_write(
    fcs_var_base_writer_t * w,
    int base, int item
)
{
    assert(item >= 0);
    assert(item < base);

    mpz_addmul_ui(w->data, w->multiplier, ((unsigned long)item));

    mpz_mul_ui(w->multiplier, w->multiplier, ((unsigned long)base));
}

static GCC_INLINE size_t fc_solve_var_base_writer_get_data(
    fcs_var_base_writer_t * w,
    unsigned char * exported
)
{
    size_t count = 0;

#define NUM_BITS 8
    while (mpz_cmp_ui(w->data, 0) != 0)
    {
        mpz_fdiv_r_2exp(w->remainder, w->data, NUM_BITS);
        mpz_fdiv_q_2exp(w->data, w->data, NUM_BITS);
        exported[count++] = (unsigned char)mpz_get_ui(w->remainder);
    }

    return count;
}

static GCC_INLINE void fc_solve_var_base_writer_release(
    fcs_var_base_writer_t * w
)
{
    mpz_clear(w->data);
    mpz_clear(w->multiplier);
    mpz_clear(w->remainder);
}

#endif /* FC_SOLVE__VAR_BASE_WRITER_H */
