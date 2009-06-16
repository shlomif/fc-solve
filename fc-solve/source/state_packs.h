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
#ifndef FC_SOLVE__STATE_PACKS_H
#define FC_SOLVE__STATE_PACKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "state.h"
#include "instance.h"

#include "inline.h"

extern void fc_solve_state_ia_init(fc_solve_state_packs_t * state_packs);

static GCC_INLINE fcs_state_extra_info_t * fcs_state_ia_alloc_into_var(fc_solve_state_packs_t * state_packs)
{
    if ((state_packs)->num_states_in_last_pack == (state_packs)->state_pack_len)
    {
        if (! ((state_packs->num_state_packs+1) & (IA_STATE_PACKS_GROW_BY-1) ))
        {
            state_packs->state_packs = (fcs_state_keyval_pair_t * *)realloc(state_packs->state_packs, sizeof(state_packs->state_packs[0]) * (state_packs->num_state_packs + IA_STATE_PACKS_GROW_BY));
        }
        state_packs->state_packs[state_packs->num_state_packs++] = malloc(state_packs->state_pack_len * sizeof(fcs_state_keyval_pair_t));
        state_packs->num_states_in_last_pack = 0;
    }
    {
        register fcs_state_keyval_pair_t * ret_helper;
        ret_helper = &(state_packs->state_packs[state_packs->num_state_packs-1][state_packs->num_states_in_last_pack++]);
        ret_helper->info.key = &(ret_helper->s);
        return &(ret_helper->info);
    }
}

#define fcs_state_ia_release(state_packs)      \
{                                           \
    (state_packs)->num_states_in_last_pack--;    \
}

static GCC_INLINE void fc_solve_state_ia_finish(fc_solve_state_packs_t * state_packs)
{
    int a;
    for(a=0;a<state_packs->num_state_packs;a++)
    {
        free(state_packs->state_packs[a]);
    }
    free(state_packs->state_packs);
    state_packs->state_packs = NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__STATE_PACKS_H */
