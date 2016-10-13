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
 */
#pragma once

#include <assert.h>
#include "var_base_int.h"

typedef struct
{
    fcs_var_base_int_t data;
    /* To avoid memory fragmentation, we keep those here and re use them. */
    fcs_var_base_int_t data_byte_offset;
    fcs_var_base_int_t r;
} fcs_var_base_reader_t;

static GCC_INLINE void fc_solve_var_base_reader_init(
    fcs_var_base_reader_t *const s)
{
    FCS_var_base_int__init(s->data);
    FCS_var_base_int__init(s->data_byte_offset);
    FCS_var_base_int__init(s->r);
}

static GCC_INLINE void fc_solve_var_base_reader_start(
    fcs_var_base_reader_t *const s, const unsigned char *const data,
    const size_t data_len)
{
    FCS_var_base_int__set_ui(s->data, 0);
#define NUM_BITS 8
    unsigned long shift_count = 0;
    for (size_t count = 0; count < data_len; count++, shift_count += NUM_BITS)
    {
        FCS_var_base_int__set_ui(
            s->data_byte_offset, (unsigned long)data[count]);
        FCS_var_base_int__left_shift(s->data_byte_offset, shift_count);
        FCS_var_base_int__add(s->data, s->data_byte_offset);
    }
}

static GCC_INLINE int fc_solve_var_base_reader_read(
    fcs_var_base_reader_t *const reader, const int base)
{
    FCS_var_base_int__mod_div(reader->data, reader->r, (unsigned long)base);

    return (int)FCS_var_base_int__get_ui(reader->r);
}

static GCC_INLINE void fc_solve_var_base_reader_release(
    fcs_var_base_reader_t *const s)
{
    FCS_var_base_int__clear(s->data);
    FCS_var_base_int__clear(s->r);
    FCS_var_base_int__clear(s->data_byte_offset);
}
