/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// multi_fc_solve_main.c - implement the "fc-solve-multi" command line
// executable, which accepts multiple starting layout filenames and solves them
// all.
//
// It is documented in the documents "README", "USAGE", etc. in the
// Freecell Solver distribution from http://fc-solve.shlomifish.org/ .
#include "multi_cl_callback.h"
#include "default_iter_handler.h"
