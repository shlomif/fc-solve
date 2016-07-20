#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "fc_pro_iface_pos.h"
#include "rinutils.h"
#include "move.h"

void fc_solve_moves_processed_render_move(
    const fcs_extended_move_t move, char *const string)
{
    switch (fcs_move_get_type(move.move))
    {
    case FCS_MOVE_TYPE_STACK_TO_STACK:
        if (move.to_empty_stack &&
            (fcs_move_get_num_cards_in_seq(move.move) > 1))
        {
            sprintf(string, "%i%iv%x", 1 + fcs_move_get_src_stack(move.move),
                1 + fcs_move_get_dest_stack(move.move),
                fcs_move_get_num_cards_in_seq(move.move));
        }
        else
        {
            sprintf(string, "%i%i", 1 + fcs_move_get_src_stack(move.move),
                1 + fcs_move_get_dest_stack(move.move));
        }
        break;

#ifdef FCS_WITH_MOVES
    case FCS_MOVE_TYPE_FREECELL_TO_STACK:
    case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
    case FCS_MOVE_TYPE_STACK_TO_FREECELL:
    case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
    case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
    case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
        fc_solve_move_to_string_w_state(
            string, NULL, move.move, FC_SOLVE__STANDARD_NOTATION_EXTENDED);
        break;
#endif

    default:
        string[0] = '\0';
        break;
    }
}
