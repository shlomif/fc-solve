#ifndef __FCS_ISA_H
#define __FCS_ISA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "state.h"
#include "fcs.h"

extern void freecell_solver_state_ia_init(freecell_solver_hard_thread_t * hard_thread);
#if 0
extern fcs_state_with_locations_t * fcs_state_ia_alloc(freecell_solver_instance_t * instance);
#else

#define fcs_state_ia_alloc_into_var(ret, instance) \
{           \
    if ((instance)->num_states_in_last_pack == (instance)->state_pack_len)     \
    {            \
        if (instance->num_state_packs == instance->max_num_state_packs)    \
        {        \
            instance->max_num_state_packs += IA_STATE_PACKS_GROW_BY;      \
            instance->state_packs = (fcs_state_with_locations_t * *)realloc(instance->state_packs, sizeof(fcs_state_with_locations_t *) * instance->max_num_state_packs);      \
        }          \
        instance->state_packs[instance->num_state_packs] = malloc(instance->state_pack_len * sizeof(fcs_state_with_locations_t));     \
        instance->num_state_packs++;       \
        instance->num_states_in_last_pack = 0;       \
    }         \
    ret = &(instance->state_packs[instance->num_state_packs-1][instance->num_states_in_last_pack++]);       \
}

#endif


#if 0
extern void fcs_state_ia_release(freecell_solver_instance_t * instance);
#else
#define fcs_state_ia_release(instance)      \
{                                           \
    (instance)->num_states_in_last_pack--;    \
}


#endif
extern void freecell_solver_state_ia_finish(freecell_solver_hard_thread_t * hard_thread);

extern void freecell_solver_state_ia_foreach(
        freecell_solver_hard_thread_t * hard_thread,
        void (*ptr_function)(fcs_state_with_locations_t *, void *),
        void * context
        );
#ifdef __cplusplus
}
#endif

#endif
