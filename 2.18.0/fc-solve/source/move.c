/*
 * move.c - move and move stacks routines for Freecell Solver
 *
 * Written by Shlomi Fish ( http://www.shlomifish.org/ ), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "move.h"
#include "state.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#include "inline.h"

#if 0
/* This variable was used for debugging. */
int msc_counter=0;
#endif

#if 0
/* This function allocates an empty move stack */
fcs_move_stack_t * fcs_move_stack_create(void)
{
    fcs_move_stack_t * ret;

    /* Allocate the data structure itself */
    ret = (fcs_move_stack_t *)malloc(sizeof(fcs_move_stack_t));

    ret->max_num_moves = FCS_MOVE_STACK_GROW_BY;
    ret->num_moves = 0;
    /* Allocate some space for the moves */
    ret->moves = (fcs_move_t *)malloc(sizeof(fcs_move_t)*ret->max_num_moves);

    return ret;
}
#endif

#if 0
int fcs_move_stack_push(fcs_move_stack_t * stack, fcs_move_t move)
{
    /* If all the moves inside the stack are taken then
       resize the move vector */

    if (stack->num_moves == stack->max_num_moves)
    {
        int a, b;
        a = (stack->max_num_moves >> 3);
        b = FCS_MOVE_STACK_GROW_BY;
        stack->max_num_moves += max(a,b);
        stack->moves = realloc(
            stack->moves,
            stack->max_num_moves * sizeof(fcs_move_t)
            );
    }
    stack->moves[stack->num_moves++] = move;

    return 0;
}
#endif

int fc_solve_move_stack_pop(fcs_move_stack_t * stack, fcs_move_t * move)
{
    if (stack->num_moves > 0)
    {
        *move = stack->moves[--stack->num_moves];
        return 0;
    }
    else
    {
        return 1;
    }
}

#if 0
void fcs_move_stack_destroy(fcs_move_stack_t * stack)
{
    free(stack->moves);
    free(stack);
}
#endif

void fc_solve_move_stack_swallow_stack(
    fcs_move_stack_t * stack,
    fcs_move_stack_t * src_stack
    )
{
    fcs_move_t move;
    while (!fcs_move_stack_pop(src_stack, &move))
    {
        fcs_move_stack_push(stack, move);
    }
    fcs_move_stack_destroy(src_stack);
}

#if 0
void fcs_move_stack_reset(
    fcs_move_stack_t * stack
    )
{
    stack->num_moves = 0;
}
#endif

int fc_solve_move_stack_get_num_moves(
    fcs_move_stack_t * stack
    )
{
    return stack->num_moves;
}

#if 0
/*
    This function duplicates a move stack
*/
fcs_move_stack_t * fcs_move_stack_duplicate(
    fcs_move_stack_t * stack
    )
{
    fcs_move_stack_t * ret;

    ret = (fcs_move_stack_t *)malloc(sizeof(fcs_move_stack_t));

    ret->max_num_moves = stack->max_num_moves;
    ret->num_moves = stack->num_moves;
    ret->moves = (fcs_move_t *)malloc(sizeof(fcs_move_t) * ret->max_num_moves);
    memcpy(ret->moves, stack->moves, sizeof(fcs_move_t) * ret->max_num_moves);

    return ret;
}
#endif

/*
    This function performs a given move on a state

  */
void fc_solve_apply_move(
        fcs_state_extra_info_t * state_val,
        fcs_move_t move,
        int freecells_num,
        int stacks_num,
        int decks_num
        )
{
    fcs_card_t temp_card;
    int a;
    int src_stack, dest_stack;
    int src_freecell, dest_freecell;
    int src_stack_len;

    fcs_state_t * state_key = state_val->key;

    dest_stack = fcs_move_get_dest_stack(move);
    src_stack = fcs_move_get_src_stack(move);
    dest_freecell = fcs_move_get_dest_freecell(move);
    src_freecell = fcs_move_get_src_freecell(move);


    switch(fcs_move_get_type(move))
    {
        case FCS_MOVE_TYPE_STACK_TO_STACK:
        {
            src_stack_len = fcs_stack_len(*state_key, src_stack);
            for(a=0 ; a<fcs_move_get_num_cards_in_seq(move) ; a++)
            {
                fcs_push_stack_card_into_stack(
                    *state_key,
                    dest_stack,
                    src_stack,
                    src_stack_len - fcs_move_get_num_cards_in_seq(move)+a
                    );
            }
            for(a=0 ; a<fcs_move_get_num_cards_in_seq(move) ; a++)
            {
                fcs_pop_stack_card(
                    *state_key,
                    src_stack,
                    temp_card
                    );
            }
        }
        break;
        case FCS_MOVE_TYPE_FREECELL_TO_STACK:
        {
            temp_card = fcs_freecell_card(*state_key, src_freecell);
            fcs_push_card_into_stack(*state_key, dest_stack, temp_card);
            fcs_empty_freecell(*state_key, src_freecell);
        }
        break;
        case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
        {
            temp_card = fcs_freecell_card(*state_key, src_freecell);
            fcs_put_card_in_freecell(*state_key, dest_freecell, temp_card);
            fcs_empty_freecell(*state_key, src_freecell);
        }
        break;
        case FCS_MOVE_TYPE_STACK_TO_FREECELL:
        {
            fcs_pop_stack_card(*state_key, src_stack, temp_card);
            fcs_put_card_in_freecell(*state_key, dest_freecell, temp_card);
        }
        break;
        case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
        {
            fcs_pop_stack_card(*state_key, src_stack, temp_card);
            fcs_increment_foundation(*state_key, fcs_move_get_foundation(move));
        }
        break;
        case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
        {
            fcs_empty_freecell(*state_key, src_freecell);
            fcs_increment_foundation(*state_key, fcs_move_get_foundation(move));
        }
        break;
        case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
        {
            for(a=0;a<13;a++)
            {
                fcs_pop_stack_card(*state_key, src_stack, temp_card);
                fcs_increment_foundation(*state_key, fcs_move_get_foundation(move));
            }
        }
        break;

        case FCS_MOVE_TYPE_FLIP_CARD:
        {
            fcs_flip_stack_card(*state_key, src_stack, fcs_stack_len(*state_key, src_stack)-1);
        }
        break;

        case FCS_MOVE_TYPE_CANONIZE:
        {
            fc_solve_canonize_state(
                state_val,
                freecells_num, stacks_num
            );
        }
        break;

    }
}

/*
    The purpose of this function is to convert the moves from using
    the canonized positions of the stacks and freecells to their
    user positions.
*/

void fc_solve_move_stack_normalize(
    fcs_move_stack_t * moves,
    fcs_state_extra_info_t * init_state_val,
    int freecells_num,
    int stacks_num,
    int decks_num
    )
{
    fcs_move_stack_t * temp_moves;
    fcs_move_t in_move, out_move;
    fcs_state_keyval_pair_t dynamic_state;
#ifdef INDIRECT_STACK_STATES
    char buffer[MAX_NUM_STACKS << 7];
    int a;
#endif


    fcs_move_stack_alloc_into_var(temp_moves);

    fcs_duplicate_state(
            dynamic_state.s, dynamic_state.info,
            *(init_state_val->key), *init_state_val
            );
#ifdef INDIRECT_STACK_STATES
    for(a=0;a<stacks_num;a++)
    {
        fcs_copy_stack(dynamic_state.s, dynamic_state.info, a, buffer);
    }
#endif

    while (
        fcs_move_stack_pop(
            moves,
            &in_move
            ) == 0)
    {
        fc_solve_apply_move(
            &dynamic_state.info,
            in_move,
            freecells_num,
            stacks_num,
            decks_num
            );
        if (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_CANONIZE)
        {
            /* Do Nothing */
        }
        else
        {
            fcs_move_set_type(out_move, fcs_move_get_type(in_move));
            if ((fcs_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_STACK) ||
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_FREECELL) ||
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_FOUNDATION) ||
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_SEQ_TO_FOUNDATION)
                )
            {
                fcs_move_set_src_stack(out_move,dynamic_state.info.stack_locs[(int)fcs_move_get_src_stack(in_move)]);
            }

            if (
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_STACK) ||
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_FREECELL) ||
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION))
            {
                fcs_move_set_src_freecell(out_move,dynamic_state.info.fc_locs[(int)fcs_move_get_src_freecell(in_move)]);
            }

            if (
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_STACK) ||
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_STACK)
                )
            {
                fcs_move_set_dest_stack(out_move,dynamic_state.info.stack_locs[(int)fcs_move_get_dest_stack(in_move)]);
            }

            if (
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_FREECELL) ||
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_FREECELL)
                )
            {
                fcs_move_set_dest_freecell(out_move,dynamic_state.info.fc_locs[(int)fcs_move_get_dest_freecell(in_move)]);
            }

            if ((fcs_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_FOUNDATION) ||
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION) ||
                (fcs_move_get_type(in_move) == FCS_MOVE_TYPE_SEQ_TO_FOUNDATION)

               )
            {
                fcs_move_set_foundation(out_move,fcs_move_get_foundation(in_move));
            }

            if ((fcs_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_STACK))
            {
                fcs_move_set_num_cards_in_seq(out_move,fcs_move_get_num_cards_in_seq(in_move));
            }

            fcs_move_stack_push(temp_moves, out_move);
        }
    }

    /*
     * temp_moves contain the needed moves in reverse order. So let's use
     * swallow_stack to put them in the original in the correct order.
     *
     * */
    fcs_move_stack_reset(moves);

    fc_solve_move_stack_swallow_stack(moves, temp_moves);
}

GCC_INLINE int convert_freecell_num(int fcn)
{
    if (fcn >= 7)
        return (fcn+3);
    else
        return fcn;
}

char * fc_solve_move_to_string(fcs_move_t move, int standard_notation)
{
    return
        fc_solve_move_to_string_w_state(
            NULL, 4, 8, 1,
            move,
            (standard_notation == 2)?1:standard_notation
            );
}

char * fc_solve_move_to_string_w_state(
        fcs_state_extra_info_t * state_val,
        int freecells_num,
        int stacks_num,
        int decks_num,
        fcs_move_t move,
        int standard_notation
        )
{
    char string[256];

    fcs_state_t * state_key = state_val->key;

    switch(fcs_move_get_type(move))
    {
        case FCS_MOVE_TYPE_STACK_TO_STACK:
            if ((standard_notation == 2) &&
                /* More than one card was moved */
                (fcs_move_get_num_cards_in_seq(move) > 1) &&
                /* It was a move to an empty stack */
                (fcs_stack_len(*state_key, fcs_move_get_dest_stack(move)) ==
                 fcs_move_get_num_cards_in_seq(move))
               )
            {
                sprintf(string, "%i%iv%x",
                    1+fcs_move_get_src_stack(move),
                    1+fcs_move_get_dest_stack(move),
                    fcs_move_get_num_cards_in_seq(move)
                   );
            }
            else if (standard_notation)
            {
                sprintf(string, "%i%i",
                    1+fcs_move_get_src_stack(move),
                    1+fcs_move_get_dest_stack(move)
                    );
            }
            else
            {
                sprintf(string, "Move %i cards from stack %i to stack %i",
                    fcs_move_get_num_cards_in_seq(move),
                    fcs_move_get_src_stack(move),
                    fcs_move_get_dest_stack(move)
                );
            }
        break;

        case FCS_MOVE_TYPE_FREECELL_TO_STACK:
            if (standard_notation)
            {
                sprintf(string, "%c%i",
                    ('a'+convert_freecell_num(fcs_move_get_src_freecell(move))),
                    1+fcs_move_get_dest_stack(move)
                    );
            }
            else
            {
                sprintf(string, "Move a card from freecell %i to stack %i",
                    fcs_move_get_src_freecell(move),
                    fcs_move_get_dest_stack(move)
                    );
            }

        break;

        case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
            if (standard_notation)
            {
                sprintf(string, "%c%c",
                    ('a'+convert_freecell_num(fcs_move_get_src_freecell(move))),
                    ('a'+convert_freecell_num(fcs_move_get_dest_freecell(move)))
                    );
            }
            else
            {
                sprintf(string, "Move a card from freecell %i to freecell %i",
                    fcs_move_get_src_freecell(move),
                    fcs_move_get_dest_freecell(move)
                    );
            }

        break;

        case FCS_MOVE_TYPE_STACK_TO_FREECELL:
            if (standard_notation)
            {
                sprintf(string, "%i%c",
                    1+fcs_move_get_src_stack(move),
                    ('a'+convert_freecell_num(fcs_move_get_dest_freecell(move)))
                    );
            }
            else
            {
                sprintf(string, "Move a card from stack %i to freecell %i",
                    fcs_move_get_src_stack(move),
                    fcs_move_get_dest_freecell(move)
                    );
            }

        break;

        case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
            if (standard_notation)
            {
                sprintf(string, "%ih", 1+fcs_move_get_src_stack(move));
            }
            else
            {
                sprintf(string, "Move a card from stack %i to the foundations",
                    fcs_move_get_src_stack(move)
                    );
            }

        break;


        case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
            if (standard_notation)
            {
                sprintf(string, "%ch", ('a'+convert_freecell_num(fcs_move_get_src_freecell(move))));
            }
            else
            {
                sprintf(string,
                    "Move a card from freecell %i to the foundations",
                    fcs_move_get_src_freecell(move)
                    );
            }

        break;

        case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
            if (standard_notation)
            {
                sprintf(string, "%ih", fcs_move_get_src_stack(move));
            }
            else
            {
                sprintf(string,
                    "Move the sequence on top of Stack %i to the foundations",
                    fcs_move_get_src_stack(move)
                    );
            }
        break;

        default:
            string[0] = '\0';
        break;
    }

    return strdup(string);
}
