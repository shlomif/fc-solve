/* fcs_isa.c - Freecell Solver Indirect State Allocation Routines

   Written by Shlomi Fish, 2000
   This file is distributed under the public domain.
*/

#include <stdlib.h>
#include <stdio.h>

#include "config.h"


#include "state.h"
#include "fcs.h"

#include "fcs_isa.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

void freecell_solver_state_ia_init(freecell_solver_hard_thread_t * hard_thread)
{
    hard_thread->max_num_state_packs = IA_STATE_PACKS_GROW_BY;
    hard_thread->state_packs = (fcs_state_with_locations_t * *)malloc(sizeof(fcs_state_with_locations_t *) * hard_thread->max_num_state_packs);
    hard_thread->num_state_packs = 1;
    hard_thread->state_pack_len = 0x010000 / sizeof(fcs_state_with_locations_t);
    hard_thread->state_packs[0] = malloc(hard_thread->state_pack_len*sizeof(fcs_state_with_locations_t));

    hard_thread->num_states_in_last_pack = 0;
}

#if 0
fcs_state_with_locations_t * fcs_state_ia_alloc(freecell_solver_hard_thread_t * hard_thread)
{
    if (hard_thread->num_states_in_last_pack == hard_thread->state_pack_len)
    {
        if (hard_thread->num_state_packs == hard_thread->max_num_state_packs)
        {
            hard_thread->max_num_state_packs += IA_STATE_PACKS_GROW_BY;
            hard_thread->state_packs = (fcs_state_with_locations_t * *)realloc(hard_thread->state_packs, sizeof(fcs_state_with_locations_t *) * hard_thread->max_num_state_packs);
        }
        hard_thread->state_packs[hard_thread->num_state_packs] = malloc(hard_thread->state_pack_len * sizeof(fcs_state_with_locations_t));
        hard_thread->num_state_packs++;
        hard_thread->num_states_in_last_pack = 0;
    }
    return &(hard_thread->state_packs[hard_thread->num_state_packs-1][hard_thread->num_states_in_last_pack++]);
}
#endif

#if 0
void fcs_state_ia_release(freecell_solver_hard_thread_t * hard_thread)
{
    hard_thread->num_states_in_last_pack--;
}
#endif

void freecell_solver_state_ia_finish(freecell_solver_hard_thread_t * hard_thread)
{
    int a;
    for(a=0;a<hard_thread->num_state_packs;a++)
    {
        free(hard_thread->state_packs[a]);
    }
    free(hard_thread->state_packs);
}

void freecell_solver_state_ia_foreach(freecell_solver_hard_thread_t * hard_thread, void (*ptr_function)(fcs_state_with_locations_t *, void *), void * context)
{
    int p,s;
    for(p=0;p<hard_thread->num_state_packs-1;p++)
    {
        for(s=0 ; s < hard_thread->state_pack_len ; s++)
        {
            ptr_function(&(hard_thread->state_packs[p][s]), context);
        }
    }
    for(s=0; s < hard_thread->num_states_in_last_pack ; s++)
    {
        ptr_function(&(hard_thread->state_packs[p][s]), context);
    }
}
