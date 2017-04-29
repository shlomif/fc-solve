/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
/*
 * dbm_trace.h - dbm-solvers trace macros.
 */

#pragma once

#define T

#ifdef T
#define TRACE(my_format, ...)                                                  \
    fprintf(out_fh, my_format, __VA_ARGS__);                                   \
    fflush(out_fh)
#else
#define TRACE(a, ...)
#endif
