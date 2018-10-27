/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2016 Shlomi Fish
 */
// rinutils.h - a common header for Rindolf's (= Shlomi Fish) utility headers.
#pragma once

#include <limits.h>
#include <time.h>
#ifdef __unix__
#include <unistd.h>
#endif

#include "alloc_wrap.h"
#include "bool.h"
#include "count.h"
#include "freecell-solver/fcs_dllexport.h"
#include "fcs_err.h"
#include "freecell-solver/fcs_limit.h"
#include "likely.h"
#include "min_and_max.h"
#include "portable_time.h"
#include "str_utils.h"
#include "typeof_wrap.h"
#include "unused.h"
