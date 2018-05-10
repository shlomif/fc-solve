/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
// bit_rw.h - bit readers and writers. Used by delta_states.c .
#pragma once
#include <stddef.h>

#define NUM_BITS_IN_BYTES 8

typedef size_t fcs_bit_data;
typedef unsigned char fcs_uchar;

typedef struct
{
    fcs_uchar *current;
    int bit_in_char_idx;
    fcs_uchar *start;
} fc_solve_bit_writer;

static inline void fc_solve_bit_writer_init(
    fc_solve_bit_writer *const writer, fcs_uchar *const start)
{
    *(writer->start = writer->current = start) = 0;
    writer->bit_in_char_idx = 0;
}

static inline void fc_solve_bit_writer_write(
    fc_solve_bit_writer *const writer, int len, fcs_bit_data data)
{
    for (; len; len--, (data >>= 1))
    {
        *(writer->current) |= ((data & 0x1) << (writer->bit_in_char_idx++));
        if (writer->bit_in_char_idx == NUM_BITS_IN_BYTES)
        {
            *(++writer->current) = 0;
            writer->bit_in_char_idx = 0;
        }
    }
}

typedef struct
{
    const fcs_uchar *current;
    int bit_in_char_idx;
    const fcs_uchar *start;
} fcs_bit_reader;

static inline void fc_solve_bit_reader_init(
    fcs_bit_reader *const reader, const fcs_uchar *const start)
{
    reader->start = reader->current = start;
    reader->bit_in_char_idx = 0;
}

static inline fcs_bit_data fc_solve_bit_reader_read(
    fcs_bit_reader *reader, int len)
{
    fcs_bit_data ret = 0;
    for (int idx = 0; idx < len; ++idx)
    {
        ret |= (((*(reader->current) >> (reader->bit_in_char_idx++)) & 0x1)
                << idx);

        if (reader->bit_in_char_idx == NUM_BITS_IN_BYTES)
        {
            ++reader->current;
            reader->bit_in_char_idx = 0;
        }
    }
    return ret;
}
