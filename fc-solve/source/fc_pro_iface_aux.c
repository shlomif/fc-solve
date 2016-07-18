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

static GCC_INLINE char freecell_to_char(const int fc_idx)
{
    return 'a' + Cvtf89(fc_idx);
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
            freecell_to_char(fcs_move_get_src_freecell(move.move)),
            1 + fcs_move_get_dest_stack(move.move));
        break;

    case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
        sprintf(string, "%c%c",
            freecell_to_char(fcs_move_get_src_freecell(move.move)),
            freecell_to_char(fcs_move_get_dest_freecell(move.move)));
        break;

    case FCS_MOVE_TYPE_STACK_TO_FREECELL:
        sprintf(string, "%i%c", 1 + fcs_move_get_src_stack(move.move),
            freecell_to_char(fcs_move_get_dest_freecell(move.move)));
        break;

    case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
        sprintf(string, "%ih", 1 + fcs_move_get_src_stack(move.move));
        break;

    case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
        sprintf(string, "%ch",
            freecell_to_char(fcs_move_get_src_freecell(move.move)));
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
