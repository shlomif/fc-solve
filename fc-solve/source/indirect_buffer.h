/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// indirect_buffer.h - declare the dll_ind_buf type.
#pragma once
#include "fcs_conf.h"

#ifdef INDIRECT_STACK_STATES
typedef char dll_ind_buf[MAX_NUM_STACKS << 7];
#define DECLARE_IND_BUF_T(ident) dll_ind_buf ident;
#define IND_BUF_T_PARAM(ident) , dll_ind_buf ident
#define PASS_IND_BUF_T(ident) , ident
#else
#define DECLARE_IND_BUF_T(ident)
#define IND_BUF_T_PARAM(ident)
#define PASS_IND_BUF_T(ident)
#endif
