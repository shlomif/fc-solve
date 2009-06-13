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
/*
 * move.c - move and move stacks routines for Freecell Solver
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "move.h"
#include "state.h"

#include "inline.h"
#include "unused.h"

const fcs_move_t fc_solve_empty_move = {"\0\0\0\0"};

#if 0
int fcs_move_stack_push(fcs_move_stack_t * stack, fcs_move_t move)
{
    /* If all the moves inside the stack are taken then
       resize the move vector */

    if (! ((stack->num_moves+1) & (FCS_MOVE_STACK_GROW_BY-1)))
    {
        stack->moves = realloc(
            stack->moves,
            (stack->num_moves+1 + FCS_MOVE_STACK_GROW_BY) *
                sizeof(stack->moves[0])
            );
    }
    stack->moves[stack->num_moves++] = move;
}
#endif

#if 0
void fcs_move_stack_reset(
    fcs_move_stack_t * stack
    )
{
    stack->num_moves = 0;
}
#endif

/*
 * This function performs a given move on a state
 */
void fc_solve_apply_move(
        fcs_state_extra_info_t * state_val,
        fcs_move_t move,
        int freecells_num,
        int stacks_num,
        int decks_num GCC_UNUSED
        )
{
    fcs_card_t card;
    fcs_cards_column_t col;

    fcs_state_t * state_key = state_val->key;

#define src fcs_move_get_src_stack(move)
#define dest fcs_move_get_dest_freecell(move)
    switch(fcs_move_get_type(move))
    {
        case FCS_MOVE_TYPE_STACK_TO_STACK:
        {
            fcs_cards_column_t dest_col;
            int i;

            col = fcs_state_get_col(*state_key, src);
            dest_col = fcs_state_get_col(*state_key, dest);
            for(i=0 ; i<fcs_move_get_num_cards_in_seq(move) ; i++)
            {
                fcs_col_push_col_card(
                    dest_col,
                    col, 
                    fcs_col_len(col) - fcs_move_get_num_cards_in_seq(move)+i
                );
            }
            for(i=0 ; i<fcs_move_get_num_cards_in_seq(move) ; i++)
            {
                fcs_col_pop_top(col);
            }
        }
        break;
        case FCS_MOVE_TYPE_FREECELL_TO_STACK:
        {
            col = fcs_state_get_col(*state_key, dest);
            fcs_col_push_card(col, fcs_freecell_card(*state_key, src));
            fcs_empty_freecell(*state_key, src);
        }
        break;
        case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
        {
            card = fcs_freecell_card(*state_key, src);
            fcs_put_card_in_freecell(*state_key, dest, card);
            fcs_empty_freecell(*state_key, src);
        }
        break;

        case FCS_MOVE_TYPE_STACK_TO_FREECELL:
        {
            col = fcs_state_get_col(*state_key, src);
            fcs_col_pop_card(col, card);
            fcs_put_card_in_freecell(*state_key, dest, card);
        }
        break;

        case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
        {
            col = fcs_state_get_col(*state_key, src);
            fcs_col_pop_top(col);
            fcs_increment_foundation(*state_key, fcs_move_get_foundation(move));
        }
        break;

        case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
        {
            fcs_empty_freecell(*state_key, src);
            fcs_increment_foundation(*state_key, fcs_move_get_foundation(move));
        }
        break;

        case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
        {
            int i;

            col = fcs_state_get_col(*state_key, src);
            for (i=0 ; i<13 ; i++)
            {
                fcs_col_pop_top(col);
                fcs_increment_foundation(*state_key, fcs_move_get_foundation(move));
            }
        }
        break;

#ifndef FCS_WITHOUT_CARD_FLIPPING
        case FCS_MOVE_TYPE_FLIP_CARD:
        {
            col = fcs_state_get_col(*state_key, src);
            fcs_col_flip_card(col, fcs_col_len(col)-1);
        }
        break;
#endif

        case FCS_MOVE_TYPE_CANONIZE:
        {
            fc_solve_canonize_state(
                state_val,
                freecells_num, stacks_num
            );
        }
        break;

    }
#undef dest
#undef src
}

GCC_INLINE int convert_freecell_num(int fcn)
{
    if (fcn >= 7)
        return (fcn+3);
    else
        return fcn;
}


char * fc_solve_move_to_string_w_state(
        fcs_state_extra_info_t * state_val,
        int freecells_num GCC_UNUSED, 
        int stacks_num GCC_UNUSED,
        int decks_num GCC_UNUSED,
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
                (fcs_col_len(fcs_state_get_col(*state_key, fcs_move_get_dest_stack(move))) ==
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
                register char src_c = (char)('a'+(char)convert_freecell_num(fcs_move_get_src_freecell(move)));
                register char dest_c = (char)('a'+(char)convert_freecell_num(fcs_move_get_dest_freecell(move)));

                sprintf(string, "%c%c", src_c, dest_c);
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

#define DERIVED_STATES_LIST_GROW_BY 16
void fc_solve_derived_states_list_add_state(
        fcs_derived_states_list_t * list,
        fcs_state_extra_info_t * state_val,
        int context
        )
{
    if (
        (!(
           (list->num_states+(list->states != NULL))
           & (DERIVED_STATES_LIST_GROW_BY-1)
          )
        )
       )
    {
        (list)->states = realloc(
            (list)->states, 
            (
                sizeof((list)->states[0])
                * (list->num_states
                    + (list->states!=NULL)
                    + DERIVED_STATES_LIST_GROW_BY
                  )
            )
        );
    }
    (list)->states[(list)->num_states].state_ptr = state_val;
    (list)->states[(list)->num_states++].context.i = context;
}

