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

#define NUM_BITS_IN_BYTES 8

typedef int fc_solve_bit_data_t;
typedef unsigned char fcs_uchar_t;

typedef struct
{
    fcs_uchar_t *current;
    int bit_in_char_idx;
    fcs_uchar_t *start;
} fc_solve_bit_writer_t;

static inline void fc_solve_bit_writer_init(
    fc_solve_bit_writer_t *const writer, fcs_uchar_t *const start)
{
    *(writer->start = writer->current = start) = 0;
    writer->bit_in_char_idx = 0;
}

static inline void fc_solve_bit_writer_write(
    fc_solve_bit_writer_t *const writer, int len, fc_solve_bit_data_t data)
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
    const fcs_uchar_t *current;
    int bit_in_char_idx;
    const fcs_uchar_t *start;
} fc_solve_bit_reader_t;

static inline void fc_solve_bit_reader_init(
    fc_solve_bit_reader_t *const reader, const fcs_uchar_t *const start)
{
    reader->start = reader->current = start;
    reader->bit_in_char_idx = 0;
}

static inline fc_solve_bit_data_t fc_solve_bit_reader_read(
    fc_solve_bit_reader_t *reader, int len)
{
    int idx;
    fc_solve_bit_data_t ret = 0;

    for (idx = 0; idx < len; idx++)
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
