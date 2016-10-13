/* Copyright (c) 2016 Shlomi Fish
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
 * var_base_int.h - abstract/wrap either mpz_t or the faster unsigned __int128.
 */
#pragma once
#include "config.h"
#include "rinutils.h"

#ifdef FCS_USE_INT128_FOR_VAR_BASE

typedef unsigned __int128 fcs_var_base_int_t;
#define FCS_var_base_int__init(i) ((i) = 0)
#define FCS_var_base_int__set_ui(i, val) ((i) = (val))
#define FCS_var_base_int__left_shift(i, shift) ((i) <<= (shift))
#define FCS_var_base_int__add(i, i2) ((i) += (i2))
#define FCS_var_base_int__mod_div(i, i_mod, base)                              \
    (i_mod) = (i) % (base);                                                    \
    (i) /= (base)
#define FCS_var_base_int__get_ui(i) (i)
#define FCS_var_base_int__clear(i)
#define FCS_var_base_int__addmul_ui(i, i2, ui) ((i) += (i2) * (ui))
#define FCS_var_base_int__mul_ui(i, ui) (i) *= (ui)
#define FCS_var_base_int__not_zero(i) ((i) != 0)

#else /* FCS_USE_INT128_FOR_VAR_BASE */

/* Use GMP instead */

#include <gmp.h>

typedef mpz_t fcs_var_base_int_t;
#define FCS_var_base_int__init(i) mpz_init(i)
#define FCS_var_base_int__set_ui(i, val) mpz_set_ui((i), (val))
#define FCS_var_base_int__left_shift(i, shift) mpz_mul_2exp((i), (i), (shift))
#define FCS_var_base_int__add(i, i2) mpz_add((i), (i), (i2))
#define FCS_var_base_int__mod_div(i, i_mod, base)                              \
    mpz_fdiv_qr_ui(i, i_mod, i, base)
#define FCS_var_base_int__get_ui(i) mpz_get_ui(i)
#define FCS_var_base_int__clear(i) mpz_clear(i)
#define FCS_var_base_int__addmul_ui(i, i2, ui) mpz_addmul_ui(i, i2, ui)
#define FCS_var_base_int__mul_ui(i, ui) mpz_mul_ui((i), (i), (ui))
#define FCS_var_base_int__not_zero(i) (mpz_cmp_ui(i, 0) != 0)

#endif /* FCS_USE_INT128_FOR_VAR_BASE */
