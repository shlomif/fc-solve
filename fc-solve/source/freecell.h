// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// freecell.h - header file of the Freecell move functions

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "instance.h"

DECLARE_PURE_MOVE_FUNCTION(fc_solve_sfs_null_move_func);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_top_stack_cards_to_founds);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_to_founds);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_on_top_of_stacks);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_non_top_stack_cards_to_founds);
DECLARE_MOVE_FUNCTION(
    fc_solve_sfs_move_stack_cards_to_a_parent_on_the_same_stack);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_stack_cards_to_different_stacks);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_sequences_to_free_stacks);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_freecell_cards_to_empty_stack);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_cards_to_a_different_parent);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_empty_stack_into_freecells);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_empty_stack);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_parent);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_card_to_freecell);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_freecell_card_to_parent);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_atomic_move_freecell_card_to_empty_stack);
DECLARE_MOVE_FUNCTION(fc_solve_sfs_move_fc_to_empty_and_put_on_top);

#define FCS_ZERO_FREECELLS_MODE
#ifdef FCS_ZERO_FREECELLS_MODE
DECLARE_MOVE_FUNCTION(fc_solve_sfs_zerofc_0AB_atomic_all_moves);
#endif

#ifdef __cplusplus
}
#endif
