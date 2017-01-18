/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2016 Shlomi Fish
 */
#pragma once
#include "fcs_user.h"
#include "output_to_file.h"
#include "iter_handler_base.h"

static void my_iter_handler(void *const user_instance,
    const fcs_int_limit_t iter_num, const int depth, void *const ptr_state,
    const fcs_int_limit_t parent_iter_num, void *const context)
{
}
