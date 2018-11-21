/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
// var_base_reader.h - read from a packed integer using digits of variable
// bases. Also see var_base_writer.h which complements it.
#pragma once

#include "var_base_int.h"

typedef struct
{
    var_base_int data;
#ifndef FCS_USE_INT128_FOR_VAR_BASE
    /* To avoid memory fragmentation, we keep those here and re use them. */
    var_base_int data_byte_offset;
    var_base_int r;
#endif
} fcs_var_base_reader;

#ifdef FCS_USE_INT128_FOR_VAR_BASE
#define fc_solve_var_base_reader_init(s)
#else
static inline void fc_solve_var_base_reader_init(fcs_var_base_reader *const s)
{
    FCS_var_base_int__init(s->data);
    FCS_var_base_int__init(s->data_byte_offset);
    FCS_var_base_int__init(s->r);
}
#endif
static inline void fc_solve_var_base_reader_start(fcs_var_base_reader *const s,
    const unsigned char *const data, const size_t data_len)
{
    FCS_var_base_int__set_ui(s->data, 0);
#define NUM_BITS 8
    unsigned long shift_count = 0;
    for (size_t count = 0; count < data_len; count++, shift_count += NUM_BITS)
    {
#ifdef FCS_USE_INT128_FOR_VAR_BASE
        s->data |= (((var_base_int)data[count]) << shift_count);
#else
        FCS_var_base_int__set_ui(
            s->data_byte_offset, (unsigned long)data[count]);
        FCS_var_base_int__left_shift(s->data_byte_offset, shift_count);
        FCS_var_base_int__add(s->data, s->data_byte_offset);
#endif
    }
}

static inline unsigned long fc_solve_var_base_reader_read(
    fcs_var_base_reader *const reader, const unsigned long base)
{
#ifdef FCS_USE_INT128_FOR_VAR_BASE
    var_base_int rem;
#else
#define rem (reader->r)
#endif
    FCS_var_base_int__mod_div(reader->data, rem, base);

    return FCS_var_base_int__get_ui(rem);
#ifndef FCS_USE_INT128_FOR_VAR_BASE
#undef rem
#endif
}

#ifdef FCS_USE_INT128_FOR_VAR_BASE
#define fc_solve_var_base_reader_release(s)
#else
static inline void fc_solve_var_base_reader_release(
    fcs_var_base_reader *const s)
{
    FCS_var_base_int__clear(s->data);
    FCS_var_base_int__clear(s->r);
    FCS_var_base_int__clear(s->data_byte_offset);
}
#endif
