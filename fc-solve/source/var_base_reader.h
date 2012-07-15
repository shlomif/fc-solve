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
 * var_base_reader.h - read from a packed integer using digits of variable
 * bases. Also see var_base_writer.h which complements it.
 *
 */
#ifndef FC_SOLVE__VAR_BASE_READER_H
#define FC_SOLVE__VAR_BASE_READER_H

#include <assert.h>
#include <gmp.h>

#include "inline.h"

typedef struct
{
    mpz_t data;
    /* To avoid memory fragmentation, we keep those here and re use them. */
    mpz_t data_byte_offset;
    mpz_t r;
} fcs_var_base_reader_t;

static GCC_INLINE void fc_solve_var_base_reader_init(
    fcs_var_base_reader_t * s)
{
    mpz_init(s->data);
    mpz_init(s->data_byte_offset);
    mpz_init(s->r);
}

static GCC_INLINE void fc_solve_var_base_reader_start(
    fcs_var_base_reader_t * s,
    const unsigned char * data,
    size_t data_len
    )
{
    size_t count;
    mp_bitcnt_t shift_count = 0;

    mpz_set_ui(s->data, 0);
#define NUM_BITS 8
    for (count = 0; count < data_len ; count++, shift_count += NUM_BITS)
    {
        mpz_set_ui(s->data_byte_offset, (unsigned long)data[count]);
        mpz_mul_2exp(s->data_byte_offset, s->data_byte_offset, shift_count);
        mpz_add(s->data, s->data, s->data_byte_offset);
    }
}

static GCC_INLINE int fc_solve_var_base_reader_read(
    fcs_var_base_reader_t * reader, int base
)
{
    mpz_fdiv_qr_ui(reader->data, reader->r, reader->data, (unsigned long)base);

    return (int)mpz_get_ui(reader->r);
}

static GCC_INLINE void fc_solve_var_base_reader_release(
    fcs_var_base_reader_t * s
)
{
    mpz_clear(s->data);
    mpz_clear(s->r);
    mpz_clear(s->data_byte_offset);
}

#endif /* FC_SOLVE__VAR_BASE_READER_H */
