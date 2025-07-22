// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2012 Shlomi Fish
// dbm_kaztree_compare.h - contains the comparison routine. The record_t
// definition can be found in delta_states.h.
//
// Intended for dbm_kaztree.c. Should only be included by it and programs
// that test it.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "delta_states.h"
#include "dbm_avl_key_type.h"

static inline int fcs_dbm__compare_records__noctx(
    const void *const void_a, const void *const void_b)
{
#define GET_PARAM(p) (((const avl_key_type *)(p))->key)
    return memcmp(
        &(GET_PARAM(void_a)), &(GET_PARAM(void_b)), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static int fcs_dbm__compare_records(const void *const void_a,
    const void *const void_b, void *const context GCC_UNUSED)
{
    return fcs_dbm__compare_records__noctx(void_a, void_b);
}

#ifdef __cplusplus
}
#endif
