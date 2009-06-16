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
#include "instance.h"

#include "fcs_isa.h"

void fc_solve_state_ia_init(fc_solve_state_packs_t * state_packs)
{
    state_packs->state_packs = (fcs_state_keyval_pair_t * *)malloc(sizeof(fcs_state_keyval_pair_t *) * IA_STATE_PACKS_GROW_BY);
    state_packs->num_state_packs = 1;
    /*
     * All the states should fit in one 64KB segment. Now, we allocate as
     * many states as possible, minus one, so we would be certain that there
     * would be place for the overhead required by the malloc algorithm.
     * */
    state_packs->state_pack_len = (0x010000 / sizeof(fcs_state_keyval_pair_t)) - 1;
    state_packs->state_packs[0] = malloc(state_packs->state_pack_len*sizeof(fcs_state_keyval_pair_t));

    state_packs->num_states_in_last_pack = 0;
}

