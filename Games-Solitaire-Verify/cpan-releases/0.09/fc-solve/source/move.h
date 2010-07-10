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

#include "state.h"
#include "internal_move_struct.h"

#include "inline.h"
#include "bool.h"

extern const fcs_internal_move_t fc_solve_empty_move;

#define fcs_move_stack_push(stack, move) \
{           \
    /* If all the moves inside the stack are taken then    \
       resize the move vector */       \
              \
    if (! ((stack->num_moves+1) & (FCS_MOVE_STACK_GROW_BY-1))) \
    {      \
        stack->moves = realloc(     \
            stack->moves,     \
            (stack->num_moves+1 + FCS_MOVE_STACK_GROW_BY) * \
                sizeof(stack->moves[0])   \
            );     \
    }       \
    stack->moves[stack->num_moves++] = move;    \
            \
}

static GCC_INLINE fcs_bool_t fc_solve_move_stack_pop(fcs_move_stack_t * stack, fcs_internal_move_t * move)
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
        fcs_state_extra_info_t * state_val,
        fcs_internal_move_t move,
        int freecells_num,
        int stacks_num,
        int decks_num
);


/*
    The purpose of this function is to convert the moves from using
    the canonized positions of the stacks and freecells to their
    user positions.
*/

#define FCS_MOVE_STACK_GROW_BY 16
/* This macro initialises an empty move stack */
#define fcs_move_stack_init(ret) \
{       \
    ret.num_moves = 0;        \
    /* Allocate some space for the moves */     \
    ret.moves = (fcs_internal_move_t *)malloc(sizeof(ret.moves[0])*FCS_MOVE_STACK_GROW_BY);  \
}

static GCC_INLINE void fc_solve_move_stack_swallow_stack(
    fcs_move_stack_t * stack,
    fcs_move_stack_t * src_stack
    )
{
    fcs_internal_move_t move;
    while (!fc_solve_move_stack_pop(src_stack, &move))
    {
        fcs_move_stack_push(stack, move);
    }
}

static GCC_INLINE void fc_solve_move_stack_normalize(
    fcs_move_stack_t * moves,
    fcs_state_extra_info_t * init_state_val,
    int freecells_num,
    int stacks_num,
    int decks_num
    )
{
    fcs_move_stack_t temp_moves;
    fcs_internal_move_t in_move, out_move;
    fcs_state_keyval_pair_t dynamic_state;
#ifdef INDIRECT_STACK_STATES
    char buffer[MAX_NUM_STACKS << 7];
    int i;
#endif

    out_move = fc_solve_empty_move;

    fcs_move_stack_init(temp_moves);

    fcs_duplicate_state(
            &(dynamic_state.s), &(dynamic_state.info),
            (init_state_val->key), init_state_val
            );
#ifdef INDIRECT_STACK_STATES
    for (i=0 ; i < stacks_num ; i++)
    {
        fcs_copy_stack(dynamic_state.s, dynamic_state.info, i, buffer);
    }
#endif

    while (
        fc_solve_move_stack_pop(
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
        if (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_CANONIZE)
        {
            /* Do Nothing */
        }
        else
        {
            fcs_int_move_set_type(out_move, fcs_int_move_get_type(in_move));
            if ((fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_STACK) ||
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_FREECELL) ||
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_FOUNDATION) ||
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_SEQ_TO_FOUNDATION)
                )
            {
                fcs_int_move_set_src_stack(out_move,dynamic_state.info.stack_locs[(int)fcs_int_move_get_src_stack(in_move)]);
            }

            if (
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_STACK) ||
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_FREECELL) ||
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION))
            {
                fcs_int_move_set_src_freecell(out_move,dynamic_state.info.fc_locs[(int)fcs_int_move_get_src_freecell(in_move)]);
            }

            if (
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_STACK) ||
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_STACK)
                )
            {
                fcs_int_move_set_dest_stack(out_move,dynamic_state.info.stack_locs[(int)fcs_int_move_get_dest_stack(in_move)]);
            }

            if (
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_FREECELL) ||
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_FREECELL)
                )
            {
                fcs_int_move_set_dest_freecell(out_move,dynamic_state.info.fc_locs[(int)fcs_int_move_get_dest_freecell(in_move)]);
            }

            if ((fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_FOUNDATION) ||
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION) ||
                (fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_SEQ_TO_FOUNDATION)

               )
            {
                fcs_int_move_set_foundation(out_move,fcs_int_move_get_foundation(in_move));
            }

            if ((fcs_int_move_get_type(in_move) == FCS_MOVE_TYPE_STACK_TO_STACK))
            {
                fcs_int_move_set_num_cards_in_seq(out_move,fcs_int_move_get_num_cards_in_seq(in_move));
            }

            fcs_move_stack_push((&temp_moves), out_move);
        }
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

extern char * fc_solve_move_to_string_w_state(
        fcs_state_extra_info_t * state_val,
        int freecells_num,
        int stacks_num,
        int decks_num,
        fcs_move_t move,
        int standard_notation
        );

static GCC_INLINE char * fc_solve_move_to_string(fcs_move_t move, int standard_notation)
{
    return
        fc_solve_move_to_string_w_state(
            NULL, 4, 8, 1,
            move,
            (standard_notation == 2)?1:standard_notation
            );
}


typedef struct {
    fcs_state_extra_info_t * state_ptr;
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
        fcs_state_extra_info_t * state_val,
        int context
        );

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__MOVE_H */
