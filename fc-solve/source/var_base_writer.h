/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
// var_base_writer.h - write to a packed integer using digits of variable
// bases. Also see var_base_reader.h which complements it.
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

static inline void fc_solve_var_base_writer_init(fcs_var_base_writer_t *const s)
{
    FCS_var_base_int__init(s->data);
    FCS_var_base_int__init(s->multiplier);
    FCS_var_base_int__init(s->remainder);
}

static inline void fc_solve_var_base_writer_start(
    fcs_var_base_writer_t *const s)
{
    FCS_var_base_int__set_ui(s->data, 0);
    FCS_var_base_int__set_ui(s->multiplier, 1);
}

static inline void fc_solve_var_base_writer_write(
    fcs_var_base_writer_t *const w, const unsigned long base,
    const unsigned long item)
{
    assert(item < base);
    FCS_var_base_int__addmul_ui(w->data, w->multiplier, item);
    FCS_var_base_int__mul_ui(w->multiplier, base);
}

static inline size_t fc_solve_var_base_writer_get_data(
    fcs_var_base_writer_t *const w, unsigned char *const exported)
{
    size_t count = 0;

#ifdef FCS_USE_INT128_FOR_VAR_BASE
    var_AUTO(data_, w->data);
#else
#define data_ w->data
#endif
#define NUM_BITS 8
    while (FCS_var_base_int__not_zero(data_))
    {
#ifdef FCS_USE_INT128_FOR_VAR_BASE
        exported[count++] = (unsigned char)((data_) & (((1 << NUM_BITS) - 1)));
        data_ >>= NUM_BITS;
#else
        mpz_fdiv_r_2exp(w->remainder, w->data, NUM_BITS);
        mpz_fdiv_q_2exp(w->data, w->data, NUM_BITS);
        exported[count++] =
            (unsigned char)FCS_var_base_int__get_ui(w->remainder);
#endif
    }
#ifdef FCS_USE_INT128_FOR_VAR_BASE
    w->data = data_;
#else
#undef data_
#endif

    return count;
}

static inline void fc_solve_var_base_writer_release(
    fcs_var_base_writer_t *const w)
{
    FCS_var_base_int__clear(w->data);
    FCS_var_base_int__clear(w->multiplier);
    FCS_var_base_int__clear(w->remainder);
}
