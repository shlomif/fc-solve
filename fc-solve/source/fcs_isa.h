#ifndef FC_SOLVE__FCS_ISA_H
#define FC_SOLVE__FCS_ISA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "state.h"
#include "fcs.h"

extern void fc_solve_state_ia_init(fc_solve_hard_thread_t * hard_thread);
#if 0
extern fcs_state_with_locations_t * fcs_state_ia_alloc(fc_solve_instance_t * instance);
#else

#define fcs_state_ia_alloc_into_var(ret_key, ret_val, instance) \
{           \
    if ((instance)->num_states_in_last_pack == (instance)->state_pack_len)     \
    {            \
        if (instance->num_state_packs == instance->max_num_state_packs)    \
        {        \
            instance->max_num_state_packs += IA_STATE_PACKS_GROW_BY;      \
            instance->state_packs = (fcs_state_keyval_pair_t * *)realloc(instance->state_packs, sizeof(fcs_state_keyval_pair_t *) * instance->max_num_state_packs);      \
        }          \
        instance->state_packs[instance->num_state_packs] = malloc(instance->state_pack_len * sizeof(fcs_state_keyval_pair_t));     \
        instance->num_state_packs++;       \
        instance->num_states_in_last_pack = 0;       \
    }         \
    {          \
        fcs_state_keyval_pair_t * ret_helper; \
        ret_helper = &(instance->state_packs[instance->num_state_packs-1][instance->num_states_in_last_pack++]);       \
        ret_key = &(ret_helper->s); \
        ret_val = &(ret_helper->info); \
        ret_val->key = ret_key;  \
    } \
}

#endif


#if 0
extern void fcs_state_ia_release(fc_solve_instance_t * instance);
#else
#define fcs_state_ia_release(instance)      \
{                                           \
    (instance)->num_states_in_last_pack--;    \
}


#endif
extern void fc_solve_state_ia_finish(fc_solve_hard_thread_t * hard_thread);

extern void fc_solve_state_ia_foreach(
        fc_solve_hard_thread_t * hard_thread,
        void (*ptr_function)(fcs_state_t *, fcs_state_extra_info_t *, void *),
        void * context
        );
#ifdef __cplusplus
}
#endif

#endif
