/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */

/*
 * header file for fc_solve_check_and_add_state() . Deprecated - now it's
 * in instance.h.
 */
#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

/* #define FCS_USE_INLINE */
#include "instance.h"

/*
 * check_and_add_state is defined in check_and_add_state.c.
 *
 * DFS stands for Depth First Search which is the type of scan Freecell
 * Solver uses to solve a given board.
 * */


#ifdef __cplusplus
}
#endif
