/* Copyright (c) 2000 Shlomi Fish
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
 * fcs_isa.h - the Freecell Solver indirect-states-allocation. Compactly
 * allocates (i.e: with less overhead) vectors of states.
 * */
#ifndef FC_SOLVE__FCS_ISA_H
#define FC_SOLVE__FCS_ISA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "state.h"
#include "instance.h"

extern void fc_solve_state_ia_init(fc_solve_hard_thread_t * hard_thread);

#define fcs_state_ia_alloc_into_var(ret_val, instance) \
{           \
    if ((instance)->num_states_in_last_pack == (instance)->state_pack_len)     \
    {            \
        if (! ((instance->num_state_packs+1) & (IA_STATE_PACKS_GROW_BY-1) ))    \
        {        \
            instance->state_packs = (fcs_state_keyval_pair_t * *)realloc(instance->state_packs, sizeof(instance->state_packs[0]) * (instance->num_state_packs + IA_STATE_PACKS_GROW_BY));      \
        }          \
        instance->state_packs[instance->num_state_packs++] = malloc(instance->state_pack_len * sizeof(fcs_state_keyval_pair_t));     \
        instance->num_states_in_last_pack = 0;       \
    }         \
    {          \
        fcs_state_keyval_pair_t * ret_helper; \
        ret_helper = &(instance->state_packs[instance->num_state_packs-1][instance->num_states_in_last_pack++]);       \
        ret_val = &(ret_helper->info); \
        ret_val->key = &(ret_helper->s);  \
    } \
}

#if 0
extern void fcs_state_ia_release(fc_solve_instance_t * instance);
#else
#define fcs_state_ia_release(instance)      \
{                                           \
    (instance)->num_states_in_last_pack--;    \
}


#endif
extern void fc_solve_state_ia_finish(fc_solve_hard_thread_t * hard_thread);

#ifdef __cplusplus
}
#endif

#endif
