/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
// dbm_common.h - some common definitions (constants/etc.) for the DBM solver.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "state.h"

typedef enum
{
    FCS_DBM_VARIANT_2FC_FREECELL,
    FCS_DBM_VARIANT_BAKERS_DOZEN,
} fcs_dbm_variant_type;

#ifndef FCS_DBM_FREECELLS_NUM
#define FCS_DBM_FREECELLS_NUM 2
#endif

#ifdef FCS_FREECELL_ONLY

#define STACKS_NUM 8
#define LOCAL_FREECELLS_NUM FCS_DBM_FREECELLS_NUM

#else

#define LOCAL_VARIANT local_variant
#define STACKS_NUM ((LOCAL_VARIANT == FCS_DBM_VARIANT_BAKERS_DOZEN) ? 13 : 8)
#define LOCAL_FREECELLS_NUM                                                    \
    ((LOCAL_VARIANT == FCS_DBM_VARIANT_BAKERS_DOZEN) ? 0                       \
                                                     : FCS_DBM_FREECELLS_NUM)
#endif

#define LOCAL_STACKS_NUM STACKS_NUM
#define INSTANCE_DECKS_NUM 1
#define DECKS_NUM INSTANCE_DECKS_NUM
#define RANK_KING 13
#define FREECELLS_NUM LOCAL_FREECELLS_NUM

#ifdef __cplusplus
}
#endif
