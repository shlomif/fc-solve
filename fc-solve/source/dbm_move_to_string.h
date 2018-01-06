/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
// dbm_move_to_string.h - the move_to_string function.
#pragma once

#include "rinutils.h"

#ifdef __cplusplus
extern "C" {
#endif

static void move_to_string(unsigned char move, char *const move_buffer)
{
    char *s = move_buffer;

    for (int iter = 0; iter < 2; iter++)
    {
        int val_to_inspect = (move & 0xF);
        move >>= 4;

        if (val_to_inspect < 8)
        {
            s += sprintf(s, "Column %d", val_to_inspect);
        }
        else
        {
            val_to_inspect -= 8;
            if (val_to_inspect < 4)
            {
                s += sprintf(s, "Freecell %d", val_to_inspect);
            }
            else
            {
                val_to_inspect -= 4;
                s += sprintf(s, "Foundation %d", val_to_inspect);
            }
        }
        if (iter == 0)
        {
            s += sprintf(s, "%s", " -> ");
        }
    }
}

#ifdef __cplusplus
}
#endif
