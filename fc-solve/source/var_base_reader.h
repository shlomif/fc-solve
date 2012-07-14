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
} fcs_var_base_reader_t;

static GCC_INLINE void fc_solve_var_base_reader_init(
    fcs_var_base_reader_t * s,
    const unsigned char * data,
    size_t data_len
    )
{
    mpz_init(s->data);
    mpz_import(s->data, data_len, 1, sizeof(data[0]), 0, 0, data);
}

static GCC_INLINE int fc_solve_var_base_reader_read(
    fcs_var_base_reader_t * reader, int base
)
{
    mpz_t r;
    int ret;

    mpz_init(r);
    mpz_fdiv_qr_ui(reader->data, r, reader->data, (unsigned long)base);

    ret = (int)mpz_get_ui(r);
    mpz_clear(r);

    return ret;
}

static GCC_INLINE void fc_solve_var_base_reader_release(
    fcs_var_base_reader_t * s
)
{
    mpz_clear(s->data);
}

#endif /* FC_SOLVE__VAR_BASE_READER_H */
