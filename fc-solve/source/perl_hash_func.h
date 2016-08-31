#pragma once

#include "rinutils.h"

typedef unsigned long ul;
typedef unsigned char ub1;

static GCC_INLINE ul perl_hash_function(
    register const ub1 *s_ptr, register const ul len)
{
    register ul hash_value_int = 0;
    register const ub1 *const s_end = s_ptr + len;

    while (s_ptr < s_end)
    {
        hash_value_int += (hash_value_int << 5) + *(s_ptr++);
    }
    hash_value_int += (hash_value_int >> 5);

    return hash_value_int;
}
