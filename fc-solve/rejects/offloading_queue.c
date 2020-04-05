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
 * offloading_queue.c - main module interface for the offloading-to-hard
 * -disk C implementation.
 * This is compiled into a shared library and then loaded by Inline::C,
 * ctypes, etc.
 */

#define BUILDING_DLL 1

#include <stdio.h>

#include "config.h"
#undef FCS_RCS_STATES

#include "fcs_dllexport.h"

#include "offloading_queue.h"
/* TODO : Remove later. */
void bar(void)
{
    printf("%s\n", "H");

    return;
}
