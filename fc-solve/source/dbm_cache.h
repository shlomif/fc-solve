// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2012 Shlomi Fish
// dbm_cache.h - contains the implementation of the DBM solver cache routines.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "meta_alloc.h"
#include "freecell-solver/fcs_enums.h"

#include "freecell-solver/fcs_dllexport.h"
#include "delta_states.h"
#include "dbm_calc_derived_iface.h"
#include "dbm_lru_cache.h"

static inline int fc_solve_compare_lru_cache_keys__noctx(
    const void *const void_a, const void *const void_b)
{
#define GET_PARAM(p) ((((const fcs_cache_key_info *)(p))->key))
    return memcmp(
        &(GET_PARAM(void_a)), &(GET_PARAM(void_b)), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static int fc_solve_compare_lru_cache_keys(const void *const void_a,
    const void *const void_b, void *context GCC_UNUSED)
{
    return fc_solve_compare_lru_cache_keys__noctx(void_a, void_b);
}

#ifdef __cplusplus
}
#endif
