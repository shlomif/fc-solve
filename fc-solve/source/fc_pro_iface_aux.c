#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "fc_pro_iface_pos.h"

#include "rinutils.h"

static GCC_INLINE int Cvtf89(const int fcn)
{
    return (fcn >= 7) ? (fcn + 3) : fcn;
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
        sprintf(string, "%c%i",
            ('a' + Cvtf89(fcs_move_get_src_freecell(move.move))),
            1 + fcs_move_get_dest_stack(move.move));
        break;

    case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
    {
        char c1, c2;

        c1 = (char)('a' + Cvtf89(fcs_move_get_src_freecell(move.move)));
        c2 = (char)('a' + Cvtf89(fcs_move_get_dest_freecell(move.move)));
        sprintf(string, "%c%c", c1, c2);
    }
    break;

    case FCS_MOVE_TYPE_STACK_TO_FREECELL:
        sprintf(string, "%i%c", 1 + fcs_move_get_src_stack(move.move),
            ('a' + Cvtf89(fcs_move_get_dest_freecell(move.move))));
        break;

    case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
        sprintf(string, "%ih", 1 + fcs_move_get_src_stack(move.move));
        break;

    case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
        sprintf(string, "%ch",
            ('a' + Cvtf89(fcs_move_get_src_freecell(move.move))));
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
