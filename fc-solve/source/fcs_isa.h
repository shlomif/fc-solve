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

#include "inline.h"

extern void fc_solve_state_ia_init(fc_solve_hard_thread_t * hard_thread);

#define fcs_state_ia_alloc_into_var(ret_val, hard_thread) \
{           \
    if ((hard_thread)->num_states_in_last_pack == (hard_thread)->state_pack_len)     \
    {            \
        if (! ((hard_thread->num_state_packs+1) & (IA_STATE_PACKS_GROW_BY-1) ))    \
        {        \
            hard_thread->state_packs = (fcs_state_keyval_pair_t * *)realloc(hard_thread->state_packs, sizeof(hard_thread->state_packs[0]) * (hard_thread->num_state_packs + IA_STATE_PACKS_GROW_BY));      \
        }          \
        hard_thread->state_packs[hard_thread->num_state_packs++] = malloc(hard_thread->state_pack_len * sizeof(fcs_state_keyval_pair_t));     \
        hard_thread->num_states_in_last_pack = 0;       \
    }         \
    {          \
        fcs_state_keyval_pair_t * ret_helper; \
        ret_helper = &(hard_thread->state_packs[hard_thread->num_state_packs-1][hard_thread->num_states_in_last_pack++]);       \
        ret_val = &(ret_helper->info); \
        ret_val->key = &(ret_helper->s);  \
    } \
}

#define fcs_state_ia_release(hard_thread)      \
{                                           \
    (hard_thread)->num_states_in_last_pack--;    \
}

static GCC_INLINE void fc_solve_state_ia_finish(fc_solve_hard_thread_t * hard_thread)
{
    int a;
    for(a=0;a<hard_thread->num_state_packs;a++)
    {
        free(hard_thread->state_packs[a]);
    }
    free(hard_thread->state_packs);
    hard_thread->state_packs = NULL;
}

#ifdef __cplusplus
}
#endif

#endif
