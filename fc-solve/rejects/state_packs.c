/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */

/*
 * state_packs.c - Freecell Solver State Packs Allocation Routines.
 */

#include <stdlib.h>
#include <stdio.h>

#include "config.h"


#include "state.h"
#include "instance.h"

#include "state_packs.h"

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

