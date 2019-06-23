/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2016 Shlomi Fish
 */
#pragma once
#include <assert.h>

#ifdef NDEBUG
#define my2assert(x)                                                           \
    if (!(x))                                                                  \
    {                                                                          \
        abort();                                                               \
    }
#else
#define my2assert(x) assert(x)
#endif
#define USER_STATE_SIZE 1024

typedef struct
{
    char s[USER_STATE_SIZE];
} fcs_user_state_str;

static inline fcs_user_state_str read_state(FILE *f)
{
    fcs_user_state_str user_state;
    memset(user_state.s, '\0', sizeof(user_state));
    if (f)
    {
        if (fread(user_state.s, sizeof(user_state.s[0]), USER_STATE_SIZE - 1,
                f) == 0)
        {
            user_state.s[0] = 0;
        }
        fclose(f);
    }

    return user_state;
}
