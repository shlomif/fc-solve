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
 * portable_int32.h - the Freecell Solver (mostly) portable 32-bit/etc. int
 * typedefs.
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

/* Taken from:
 * http://www.binarytides.com/data-type-u_int8_t-u_int16_t-u_int32_t-on-windows/
 * */
#if defined(_WIN32)

#include <stdint.h>

typedef unsigned char u_char;
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;

#endif

#ifdef __cplusplus
};
#endif
