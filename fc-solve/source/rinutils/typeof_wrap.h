/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
// typeof_wrap.h - convenient wrappers for GCC's typeof
#pragma once

// These emulate the C++ auto keyword.
#define const_AUTO(myvar, expr) const typeof(expr) myvar = (expr)
#define var_AUTO(myvar, expr) typeof(expr) myvar = (expr)
#define var_PTR(myvar, expr) typeof((expr)[0]) *myvar = (expr)
#define const_PTR(myvar, expr) typeof((expr)[0]) *const myvar = (expr)

// Common macros for object slots.
#define const_SLOT(myslot, obj) const_AUTO(myslot, (obj)->myslot)
#define var_S_SLOT(myslot, obj) const_AUTO(myslot, (obj).myslot)
