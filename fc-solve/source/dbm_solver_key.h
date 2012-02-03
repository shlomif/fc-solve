#ifndef FC_SOLVE_DBM_SOLVER_KEY_H
#define FC_SOLVE_DBM_SOLVER_KEY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>

#include "config.h"

#include "bool.h"
#include "inline.h"

typedef struct { unsigned char s[24]; } fcs_encoded_state_buffer_t;

typedef struct
{
    fcs_encoded_state_buffer_t key;
    fcs_encoded_state_buffer_t parent_and_move;
} fcs_dbm_record_t;

static void GCC_INLINE fcs_init_encoded_state(fcs_encoded_state_buffer_t * enc_state)
{
    memset(enc_state, '\0', sizeof(*enc_state));
}

#ifdef __cplusplus
}
#endif

#endif /*  FC_SOLVE_DBM_SOLVER_KEY_H */
