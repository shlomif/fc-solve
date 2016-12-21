/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#pragma once

#ifdef _MSC_VER
#ifdef BUILDING_DLL
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif
#define DLLLOCAL
#elif defined(__EMSCRIPTEN__)
#include "emscripten.h"
#define DLLEXPORT EMSCRIPTEN_KEEPALIVE
#define DLLLOCAL __attribute__((visibility("hidden")))
#elif defined(__GNUC__)
#define DLLEXPORT __attribute__((visibility("default")))
#define DLLLOCAL __attribute__((visibility("hidden")))
#else
#define DLLEXPORT
#define DLLLOCAL
#endif
