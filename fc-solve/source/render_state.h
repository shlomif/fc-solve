/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#pragma once
#include "fcs_conf.h"

typedef char fcs_render_state_str[2000];
#ifdef FCS_WITH_MOVES
#define FCS__RENDER_STATE(state_str, state_ptr, locs)                          \
    fc_solve_state_as_string(state_str, state_ptr,                             \
        locs PASS_FREECELLS(FREECELLS_NUM) PASS_STACKS(STACKS_NUM)             \
            PASS_DECKS(DECKS_NUM) FC_SOLVE__PASS_PARSABLE(TRUE),               \
        FALSE FC_SOLVE__PASS_T(TRUE))
#else
#define FCS__RENDER_STATE(state_str, state_ptr, locs) ((state_str)[0] = '\0')
#endif

#ifdef DEBUG_OUT
#define FCS__OUTPUT_STATE(out_fh, prefix, state_ptr, locs)                     \
    {                                                                          \
        fcs_render_state_str state_str;                                        \
        FCS__RENDER_STATE(state_str, state_ptr, locs);                         \
        fprintf(out_fh, "%s<<<\n%s>>>\n\n", prefix, state_str);                \
        fflush(out_fh);                                                        \
    }
#else
#define FCS__OUTPUT_STATE(out_fh, prefix, state_ptr, locs)
#endif
