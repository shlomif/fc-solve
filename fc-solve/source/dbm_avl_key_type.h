#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "meta_alloc.h"

#ifdef FCS_LIBAVL_STORE_WHOLE_KEYS
#include "delta_states.h"
#endif

#ifdef FCS_LIBAVL_STORE_WHOLE_KEYS
#if 1
typedef fcs_dbm_record avl_key_type;
#else
typedef struct
{
    fcs_encoded_state_buffer key;
} avl_key_type;
#endif
#else
typedef void *avl_key_type;
#endif

#ifdef __cplusplus
}
#endif
