/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2016 Shlomi Fish
 */
// var_base_int.h - abstract/wrap either mpz_t or the faster unsigned __int128.
#pragma once
#include "fcs_conf.h"
#include "rinutils.h"

#ifdef FCS_USE_INT128_FOR_VAR_BASE

typedef unsigned __int128 var_base_int;
#define FCS_var_base_int__init(i)
#define FCS_var_base_int__set_ui(i, val) ((i) = (val))
#define FCS_var_base_int__left_shift(i, shift) ((i) <<= (shift))
#define FCS_var_base_int__add(i, diff) ((i) += (diff))
#define FCS_var_base_int__mod_div(i, i_mod, base)                              \
    (i_mod) = (i) % (base);                                                    \
    (i) /= (base)
#define FCS_var_base_int__get_ui(i) (i)
#define FCS_var_base_int__clear(i)
#define FCS_var_base_int__addmul_ui(i, i_mult, ul) ((i) += (i_mult) * (ul))
#define FCS_var_base_int__mul_ui(i, ul) (i) *= (ul)
#define FCS_var_base_int__not_zero(i) ((i) != 0)

#else

// Use GMP instead
#include <gmp.h>

typedef mpz_t var_base_int;
#define FCS_var_base_int__init(i) mpz_init(i)
#define FCS_var_base_int__set_ui(i, val) mpz_set_ui((i), (val))
#define FCS_var_base_int__left_shift(i, shift) mpz_mul_2exp((i), (i), (shift))
#define FCS_var_base_int__add(i, diff) mpz_add((i), (i), (diff))
#define FCS_var_base_int__mod_div(i, i_mod, base)                              \
    mpz_fdiv_qr_ui(i, i_mod, i, base)
#define FCS_var_base_int__get_ui(i) mpz_get_ui(i)
#define FCS_var_base_int__clear(i) mpz_clear(i)
#define FCS_var_base_int__addmul_ui(i, i_mult, ul) mpz_addmul_ui(i, i_mult, ul)
#define FCS_var_base_int__mul_ui(i, ul) mpz_mul_ui((i), (i), (ul))
#define FCS_var_base_int__not_zero(i) (mpz_cmp_ui(i, 0) != 0)

#endif
