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

#ifdef FCS_DBM__STORE_KEYS_ONLY
#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
#error FCS_LIBAVL_STORE_WHOLE_KEYS must be defined if FCS_DBM__STORE_KEYS_ONLY
#endif
#endif

#ifdef FCS_LIBAVL_STORE_WHOLE_KEYS
#ifdef FCS_DBM__STORE_KEYS_ONLY
typedef struct
{
    fcs_encoded_state_buffer key;
} avl_key_type;
#else
typedef fcs_dbm_record avl_key_type;
#endif
#else
typedef void *avl_key_type;
#endif

#ifdef __cplusplus
}
#endif
