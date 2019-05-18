/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2009 Shlomi Fish
 */
// unused.h - define the GCC_UNUSED macro to suppress unused parameters
// warnings / remarks.
#pragma once

#if defined(__GNUC__)
#define GCC_UNUSED __attribute__((unused))
#else
#define GCC_UNUSED
#endif
