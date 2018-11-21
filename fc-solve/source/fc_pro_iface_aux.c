/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
#include "fc_pro_iface_pos.h"
#include "move.h"

#ifdef FCS_WITH_MOVES
void fc_solve_moves_processed_render_move(
    const fcs_extended_move move, char *string)
{
    switch (fcs_move_get_type(move.move))
    {
    case FCS_MOVE_TYPE_STACK_TO_STACK:
        string += sprintf(string, "%d%d", 1 + fcs_move_get_src_stack(move.move),
            1 + fcs_move_get_dest_stack(move.move));
        if (move.to_empty_stack &&
            (fcs_move_get_num_cards_in_seq(move.move) > 1))
        {
            sprintf(string, "v%x", fcs_move_get_num_cards_in_seq(move.move));
        }
        break;

    case FCS_MOVE_TYPE_FREECELL_TO_STACK:
    case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
    case FCS_MOVE_TYPE_STACK_TO_FREECELL:
    case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
    case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
    case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
        fc_solve_move_to_string_w_state(
            string, NULL, move.move, FC_SOLVE__STANDARD_NOTATION_EXTENDED);
        break;

    default:
        string[0] = '\0';
        break;
    }
}
#endif
