// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2021 Shlomi Fish
// fcs_ocl_exports.h - opencl find_seed shared library exports (in order
// to silence "clang -Weverything" warnings)

#pragma once
#include "freecell-solver/fcs_dllexport.h"
DLLEXPORT void *fc_solve_user__opencl_create(void);
DLLEXPORT long long fc_solve_user__opencl_find_deal(
    void *const proto_obj, const int first_int, const int *myints);
DLLEXPORT long long fc_solve_user__opencl_release(void *const proto_obj);
