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
 * move.h - header file for the move and move stacks functions of
 * Freecell Solver
 *
 */

#ifndef FC_SOLVE__MOVE_H
#define FC_SOLVE__MOVE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This include is done to prevent a warning in case stdlib.h defines
 * max. (which is the case for the Microsoft C Compiler)
 * */
#include <stdlib.h>
#include <stdio.h>

#include "state.h"
#include "internal_move_struct.h"
#include "indirect_buffer.h"

#include "inline.h"
#include "bool.h"
#include "alloc_wrap.h"

extern const fcs_internal_move_t fc_solve_empty_move;

#define FCS_MOVE_STACK_GROW_BY 16

static GCC_INLINE void fcs_move_stack_push(fcs_move_stack_t * const stack, const fcs_internal_move_t move)
{
    /* If all the moves inside the stack are taken then
       resize the move vector */
    const int pos = ++stack->num_moves;

    if (! (pos & (FCS_MOVE_STACK_GROW_BY-1)))
    {
        stack->moves = SREALLOC(
            stack->moves,
            pos + FCS_MOVE_STACK_GROW_BY
            );
    }

    stack->moves[ pos-1 ] = move;
}

static GCC_INLINE void fcs_move_stack_params_push(fcs_move_stack_t * const stack, const int type, const int src, const int dest, const int num_cards_in_seq)
{
    fcs_internal_move_t temp_move;

    fcs_int_move_set_type(temp_move, type);
    fcs_int_move_set_src_stack(temp_move, src);
    fcs_int_move_set_dest_stack(temp_move, dest);
    fcs_int_move_set_num_cards_in_seq(temp_move, num_cards_in_seq);

    fcs_move_stack_push(stack, temp_move);
}

static GCC_INLINE void fcs_move_stack_non_seq_push(fcs_move_stack_t * const stack, const int type, const int src, const int dest)
{
    fcs_move_stack_params_push(stack, type, src, dest, 1);
}

static GCC_INLINE void fcs_push_1card_seq(fcs_move_stack_t * const stack,
    const int src, const int dest)
{
    fcs_move_stack_params_push(stack, FCS_MOVE_TYPE_STACK_TO_STACK, src, dest,
        1
    );
}

static GCC_INLINE const fcs_bool_t fc_solve_move_stack_pop(
    fcs_move_stack_t * const stack,
    fcs_internal_move_t * const move
    )
{
    if (stack->num_moves > 0)
    {
        *move = stack->moves[--stack->num_moves];
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

#define fcs_move_stack_static_destroy(stack) \
{ \
    free((stack).moves); \
}

#define fcs_move_stack_reset(stack) \
{      \
    (stack)->num_moves = 0;   \
}

void fc_solve_apply_move(
    fcs_state_t * const ptr_state_key,
    fcs_state_locs_struct_t * const locs,
    const fcs_internal_move_t move
    FREECELLS_AND_STACKS_ARGS()
);


/*
    The purpose of this function is to convert the moves from using
    the canonized positions of the stacks and freecells to their
    user positions.
*/

static GCC_INLINE fcs_move_stack_t fcs_move_stack__new(void)
{
    return (fcs_move_stack_t) {.num_moves = 0, .moves = malloc(sizeof(fcs_move_t) * FCS_MOVE_STACK_GROW_BY) };
}

static GCC_INLINE void fc_solve_move_stack_swallow_stack(
    fcs_move_stack_t * const stack,
    fcs_move_stack_t * const src_stack
    )
{
    fcs_internal_move_t move;
    while (!fc_solve_move_stack_pop(src_stack, &move))
    {
        fcs_move_stack_push(stack, move);
    }
}

static GCC_INLINE void fc_solve_move_stack_normalize(
    fcs_move_stack_t * const moves,
    const fcs_state_keyval_pair_t * const init_state,
    fcs_state_locs_struct_t * const locs
    FREECELLS_AND_STACKS_ARGS()
)
{
    fcs_internal_move_t in_move;

    fcs_state_keyval_pair_t s_and_info;

    DECLARE_IND_BUF_T(indirect_stacks_buffer)

#define FCS_S_FC_LOCS() (locs->fc_locs)
#define FCS_S_STACK_LOCS() (locs->stack_locs)

    fcs_internal_move_t out_move = fc_solve_empty_move;


    FCS_STATE__DUP_keyval_pair(s_and_info, *init_state);

#ifdef INDIRECT_STACK_STATES
    for (int i=0 ; i < STACKS_NUM__VAL; i++)
    {
        fcs_copy_stack(s_and_info.s, s_and_info.info, i, indirect_stacks_buffer);
    }
#endif

    fcs_move_stack_t temp_moves = fcs_move_stack__new();

    while (
        fc_solve_move_stack_pop(
            moves,
            &in_move
            ) == 0)
    {
        fc_solve_apply_move(
            &(s_and_info.s),
            locs,
            in_move
            PASS_FREECELLS(freecells_num)
            PASS_STACKS(stacks_num)
            );

        const int move_type = fcs_int_move_get_type(in_move);

        if (move_type == FCS_MOVE_TYPE_CANONIZE)
        {
            continue;
        }

        fcs_int_move_set_type(out_move, move_type);

        if ((move_type == FCS_MOVE_TYPE_STACK_TO_STACK) ||
            (move_type == FCS_MOVE_TYPE_STACK_TO_FREECELL) ||
            (move_type == FCS_MOVE_TYPE_STACK_TO_FOUNDATION) ||
            (move_type == FCS_MOVE_TYPE_SEQ_TO_FOUNDATION)
            )
        {
            fcs_int_move_set_src_stack(out_move,
                (FCS_S_STACK_LOCS())[
                    (int)fcs_int_move_get_src_stack(in_move)
                ]
            );
        }
        else if (
            (move_type == FCS_MOVE_TYPE_FREECELL_TO_STACK) ||
            (move_type == FCS_MOVE_TYPE_FREECELL_TO_FREECELL) ||
            (move_type == FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION))
        {
            fcs_int_move_set_src_freecell(out_move,
                (FCS_S_FC_LOCS())[
                    (int)fcs_int_move_get_src_freecell(in_move)
                ]
            );
        }

        if (
            (move_type == FCS_MOVE_TYPE_STACK_TO_STACK) ||
            (move_type == FCS_MOVE_TYPE_FREECELL_TO_STACK)
            )
        {
            fcs_int_move_set_dest_stack(out_move,
                (FCS_S_STACK_LOCS())[
                    (int)fcs_int_move_get_dest_stack(in_move)
                ]
            );
        }
        else if (
            (move_type == FCS_MOVE_TYPE_STACK_TO_FREECELL) ||
            (move_type == FCS_MOVE_TYPE_FREECELL_TO_FREECELL)
            )
        {
            fcs_int_move_set_dest_freecell(out_move,
                FCS_S_FC_LOCS()[
                    (int)fcs_int_move_get_dest_freecell(in_move)
                ]
            );
        }
        else if ((move_type == FCS_MOVE_TYPE_STACK_TO_FOUNDATION) ||
            (move_type == FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION) ||
            (move_type == FCS_MOVE_TYPE_SEQ_TO_FOUNDATION)

           )
        {
            fcs_int_move_set_foundation(out_move,
                fcs_int_move_get_foundation(in_move)
            );
        }

        if (move_type == FCS_MOVE_TYPE_STACK_TO_STACK)
        {
            fcs_int_move_set_num_cards_in_seq(out_move,
                fcs_int_move_get_num_cards_in_seq(in_move));
        }

        fcs_move_stack_push((&temp_moves), out_move);
    }

    /*
     * temp_moves contain the needed moves in reverse order. So let's use
     * swallow_stack to put them in the original in the correct order.
     *
     * */
    fcs_move_stack_reset(moves);

    fc_solve_move_stack_swallow_stack(moves, (&temp_moves));
    fcs_move_stack_static_destroy(temp_moves);
}

#undef FCS_S_FC_LOCS
#undef FCS_S_STACK_LOCS

static GCC_INLINE int fc_solve_move__convert_freecell_num(int fcn)
{
    if (fcn >= 7)
        return (fcn+3);
    else
        return fcn;
}

static GCC_INLINE void fc_solve_move_to_string_w_state(
    char * const string,
    fcs_state_keyval_pair_t * const state,
    const fcs_move_t move,
    const int standard_notation
)
{
#define state_key (&(state->s))
    switch(fcs_move_get_type(move))
    {
        case FCS_MOVE_TYPE_STACK_TO_STACK:
            if ((standard_notation == FC_SOLVE__STANDARD_NOTATION_EXTENDED) &&
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
                    ('a'+fc_solve_move__convert_freecell_num(fcs_move_get_src_freecell(move))),
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
( (char)('a'+(char)fc_solve_move__convert_freecell_num(fcs_move_get_src_freecell(move))) ),
( (char)('a'+(char)fc_solve_move__convert_freecell_num(fcs_move_get_dest_freecell(move))) )
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
                    ('a'+fc_solve_move__convert_freecell_num(fcs_move_get_dest_freecell(move)))
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
                sprintf(string, "%ch", ('a'+fc_solve_move__convert_freecell_num(fcs_move_get_src_freecell(move))));
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
#undef state_key
}

typedef struct {
    fcs_collectible_state_t * state_ptr;
    union
    {
        void * ptr;
        int i;
        char c[sizeof(void*)/sizeof(char)];
    } context;
} fcs_derived_states_list_item_t;

typedef struct
{
    int num_states;
    fcs_derived_states_list_item_t * states;
} fcs_derived_states_list_t;



extern void fc_solve_derived_states_list_add_state(
        fcs_derived_states_list_t * list,
        fcs_collectible_state_t * state_val,
        int context
        );

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__MOVE_H */
