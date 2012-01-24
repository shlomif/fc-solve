/* Copyright (c) 2012 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * dbm_common.h - some common definitions (constants/etc.) for the DBM solver.
 */
#ifndef FC_SOLVE_DBM_COMMON_H
#define FC_SOLVE_DBM_COMMON_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "config.h"

#include "bool.h"
#include "inline.h"
#include "state.h"

#define STACKS_NUM 8
#define LOCAL_STACKS_NUM STACKS_NUM
#define INSTANCE_DECKS_NUM 1
#define LOCAL_FREECELLS_NUM 2
#define FREECELLS_NUM LOCAL_FREECELLS_NUM
#define DECKS_NUM INSTANCE_DECKS_NUM
#define RANK_KING 13

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_DBM_COMMON_H */
