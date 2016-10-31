/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2010 Shlomi Fish
 */
/*
 * portable_int64.h - the Freecell Solver (mostly) portable 64-bit int
 * typedefs.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
typedef __int64 fcs_int64_t;

#define FCS_INT64_FORMAT "%I64i"
#else
typedef long long fcs_int64_t;

#define FCS_INT64_FORMAT "%lli"
#endif

#ifdef __cplusplus
};
#endif
