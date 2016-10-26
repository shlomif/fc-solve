/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2016 Shlomi Fish
 */
/*
 * This is a small thunk to trigger the building of the DLL in the relevant
 * source files.
 * */
#pragma once

#ifdef _MSC_VER
#ifndef BUILDING_DLL
#define BUILDING_DLL
#endif
#endif
