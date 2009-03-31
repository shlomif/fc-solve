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
/* fcs_isa.c - Freecell Solver Indirect State Allocation Routines

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

void fc_solve_state_ia_init(fc_solve_hard_thread_t * hard_thread)
{
    hard_thread->max_num_state_packs = IA_STATE_PACKS_GROW_BY;
    hard_thread->state_packs = (fcs_state_keyval_pair_t * *)malloc(sizeof(fcs_state_keyval_pair_t *) * hard_thread->max_num_state_packs);
    hard_thread->num_state_packs = 1;
    /*
     * All the states should fit in one 64KB segment. Now, we allocate as
     * many states as possible, minus one, so we would be certain that there
     * would be place for the overhead required by the malloc algorithm.
     * */
    hard_thread->state_pack_len = (0x010000 / sizeof(fcs_state_keyval_pair_t)) - 1;
    hard_thread->state_packs[0] = malloc(hard_thread->state_pack_len*sizeof(fcs_state_keyval_pair_t));

    hard_thread->num_states_in_last_pack = 0;
}

#if 0
fcs_state_keyval_pair_t * fcs_state_ia_alloc(fc_solve_hard_thread_t * hard_thread)
{
    if (hard_thread->num_states_in_last_pack == hard_thread->state_pack_len)
    {
        if (hard_thread->num_state_packs == hard_thread->max_num_state_packs)
        {
            hard_thread->max_num_state_packs += IA_STATE_PACKS_GROW_BY;
            hard_thread->state_packs = (fcs_state_keyval_pair_t * *)realloc(hard_thread->state_packs, sizeof(fcs_state_keyval_pair_t *) * hard_thread->max_num_state_packs);
        }
        hard_thread->state_packs[hard_thread->num_state_packs] = malloc(hard_thread->state_pack_len * sizeof(fcs_state_keyval_pair_t));
        hard_thread->num_state_packs++;
        hard_thread->num_states_in_last_pack = 0;
    }
    return &(hard_thread->state_packs[hard_thread->num_state_packs-1][hard_thread->num_states_in_last_pack++]);
}
#endif

#if 0
void fcs_state_ia_release(fc_solve_hard_thread_t * hard_thread)
{
    hard_thread->num_states_in_last_pack--;
}
#endif

void fc_solve_state_ia_finish(fc_solve_hard_thread_t * hard_thread)
{
    int a;
    for(a=0;a<hard_thread->num_state_packs;a++)
    {
        free(hard_thread->state_packs[a]);
    }
    free(hard_thread->state_packs);
    hard_thread->state_packs = NULL;
}

void fc_solve_state_ia_foreach(
        fc_solve_hard_thread_t * hard_thread,
        void (*ptr_function)(fcs_state_t *, fcs_state_extra_info_t *, void *),
        void * context
        )
{
    int p;
    for(p=0;p<hard_thread->num_state_packs-1;p++)
    {
        fcs_state_keyval_pair_t * state_ptr = hard_thread->state_packs[p];
        fcs_state_keyval_pair_t * state_end_ptr =
            state_ptr + hard_thread->state_pack_len;

        for(; state_ptr < state_end_ptr ; state_ptr++)
        {
            ptr_function(&(state_ptr->s), &(state_ptr->info), context);
        }
    }
    {
        fcs_state_keyval_pair_t * state_ptr = hard_thread->state_packs[p];
        fcs_state_keyval_pair_t * state_end_ptr =
            state_ptr + hard_thread->num_states_in_last_pack;

        for(; state_ptr < state_end_ptr ; state_ptr++)
        {
            ptr_function(&(state_ptr->s), &(state_ptr->info), context);
        }
    }
}
