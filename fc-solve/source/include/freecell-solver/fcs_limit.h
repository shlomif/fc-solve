/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// fcs_limit.h - the purpose of this file is to define the fcs_int_limit_t
// type, and the FCS_INT_LIMIT_MAX constant.
#pragma once

#include <stdint.h>
#include "freecell-solver/fcs_back_compat.h"

typedef intptr_t fcs_int_limit_t;
#define FCS_INT_LIMIT_MAX INTPTR_MAX
#ifdef FCS_BREAK_BACKWARD_COMPAT_1
typedef unsigned long long fcs_iters_int;
#else
typedef fcs_int_limit_t fcs_iters_int;
#endif
