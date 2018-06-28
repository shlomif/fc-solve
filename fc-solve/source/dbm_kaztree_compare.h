/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
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

#if defined(FCS_DBM_RECORD_POINTER_REPR) && (!defined(FCS_DEBONDT_DELTA_STATES))
static int compare_records(
    const void *const void_a, const void *const void_b, void *const context)
{
#define GET_PARAM(p) (&(((const fcs_dbm_record *)(p))->key))
    const fcs_encoded_state_buffer *const a = GET_PARAM(void_a),
                                          *const b = GET_PARAM(void_b);
#undef GET_PARAM

    if (a->s[0] < b->s[0])
    {
        return -1;
    }
    else if (a->s[0] > b->s[0])
    {
        return 1;
    }
    else
    {
        return memcmp(a->s, b->s, a->s[0] + 1);
    }
}
#else
static int compare_records(const void *const void_a, const void *const void_b,
    void *const context GCC_UNUSED)
{
#define GET_PARAM(p) (((const fcs_dbm_record *)(p))->key)
    return memcmp(
        &(GET_PARAM(void_a)), &(GET_PARAM(void_b)), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}
#endif

#ifdef __cplusplus
}
#endif
