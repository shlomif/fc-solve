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
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#if defined(__unix__) || defined(__unix) ||                                    \
    (defined(__APPLE__) && defined(__MACH__))
#define FCS_UNIX
#endif
#ifdef FCS_UNIX
#include <unistd.h>
#endif

#include "rinutils/alloc_wrap.h"
#include "rinutils/count.h"
#include "rinutils/dllexport.h"
#include "rinutils/exit_error.h"
#include "rinutils/likely.h"
#include "rinutils/min_and_max.h"
#include "rinutils/str_utils.h"
#include "rinutils/typeof_wrap.h"
#include "rinutils/unused.h"
