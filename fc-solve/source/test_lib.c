#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fcs_user.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif


char * my_fcs_move_to_string(fcs_move_t move)
{
    char string[256];
    switch(move.type)
    {
        case FCS_MOVE_TYPE_STACK_TO_STACK:
            sprintf(string, "Move %i cards from stack %i to stack %i",
                move.num_cards_in_sequence,
                move.src_stack,
                move.dest_stack
                );

        break;

        case FCS_MOVE_TYPE_FREECELL_TO_STACK:
            sprintf(string, "Move a card from freecell %i to stack %i",
                move.src_freecell,
                move.dest_stack
                );

        break;

        case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
            sprintf(string, "Move a card from freecell %i to freecell %i",
                move.src_freecell,
                move.dest_freecell
                );

        break;

        case FCS_MOVE_TYPE_STACK_TO_FREECELL:
            sprintf(string, "Move a card from stack %i to freecell %i",
                move.src_stack,
                move.dest_freecell
                );

        break;

        case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
            sprintf(string, "Move a card from stack %i to the foundations",
                move.src_stack
                );

        break;


        case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
            sprintf(string, "Move a card from freecell %i to the foundations",
                move.src_freecell
                );

        break;

        default:
            string[0] = '\0';
        break;
    }

    return strdup(string);

}

int main(int argc, char * argv[])
{
    void * user;
    FILE * in;
    char buffer[2048];
    int ret;
    fcs_move_t move;

    user = freecell_solver_user_alloc();

    in = fopen("/home/shlomi/progs/freecell-test/0000.board", "rt");
    fread(buffer, sizeof(buffer[0]), sizeof(buffer)/sizeof(buffer[0]), in);
    fclose(in);

    freecell_solver_user_limit_iterations(user, 10000);

    ret = freecell_solver_user_solve_board(user, buffer);

    if (ret == FCS_STATE_SUSPEND_PROCESS)
    {
        freecell_solver_user_limit_iterations(user, -1);
        ret = freecell_solver_user_resume_solution(user);
    }

    if (ret == FCS_STATE_WAS_SOLVED)
    {
        while (freecell_solver_user_get_next_move(user, &move) == 0)
        {
            char * string;

            string = my_fcs_move_to_string(move);

            printf("\n%s\n", string);

            free(string);
        }
    }

    freecell_solver_user_free(user);

    return 0;
}
