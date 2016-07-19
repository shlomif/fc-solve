#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "fc_pro_iface_pos.h"
#include "rinutils.h"
#include "move.h"

static GCC_INLINE char dest_fc_to_char(const fcs_extended_move_t move)
{
    return fc_solve__freecell_to_char(fcs_move_get_dest_freecell(move.move));
}

static GCC_INLINE char src_fc_to_char(const fcs_extended_move_t move)
{
    return fc_solve__freecell_to_char(fcs_move_get_src_freecell(move.move));
}

char *fc_solve_moves_processed_render_move(
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

    case FCS_MOVE_TYPE_FREECELL_TO_STACK:
        sprintf(string, "%c%i", src_fc_to_char(move),
            1 + fcs_move_get_dest_stack(move.move));
        break;

    case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
        sprintf(string, "%c%c", src_fc_to_char(move), dest_fc_to_char(move));
        break;

    case FCS_MOVE_TYPE_STACK_TO_FREECELL:
        sprintf(string, "%i%c", 1 + fcs_move_get_src_stack(move.move),
            dest_fc_to_char(move));
        break;

    case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
        sprintf(string, "%ih", 1 + fcs_move_get_src_stack(move.move));
        break;

    case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
        sprintf(string, "%ch", src_fc_to_char(move));
        break;

    case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
        sprintf(string, "%ih", fcs_move_get_src_stack(move.move));
        break;

    default:
        string[0] = '\0';
        break;
    }
    return string + strlen(string);
}
