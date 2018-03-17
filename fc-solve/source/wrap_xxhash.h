/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// wrap_xxhash.h
#pragma once

#include "config.h"
#define XXH_PRIVATE_API
#include "xxhash.h"

#if SIZEOF_VOID_P == 4
#define DO_XXH(b, l) XXH32((b), (l), 0)
#else
#define DO_XXH(b, l) XXH64((b), (l), 0)
#endif
