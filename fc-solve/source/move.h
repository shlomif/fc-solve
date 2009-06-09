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
#include "fcs_move.h"

extern const fcs_move_t fc_solve_empty_move;

#if 0
int fcs_move_stack_push(fcs_move_stack_t * stack, fcs_move_t move);
#endif

#define fcs_move_stack_pop(stack,move) (fc_solve_move_stack_pop(stack,move))
extern int fc_solve_move_stack_pop(fcs_move_stack_t * stack, fcs_move_t * move);

#if 0
void fcs_move_stack_destroy(fcs_move_stack_t * stack);
#endif

#define fcs_move_stack_destroy(stack) \
{     \
    free((stack)->moves);  \
    free(stack); \
}

extern void fc_solve_move_stack_swallow_stack(fcs_move_stack_t * stack, fcs_move_stack_t * src_stack);
#if 0
void fcs_move_stack_reset(fcs_move_stack_t * stack);
#endif
#define fcs_move_stack_reset(stack) \
{      \
    (stack)->num_moves = 0;   \
}

void fc_solve_apply_move(
        fcs_state_extra_info_t * state_val,
        fcs_move_t move,
        int freecells_num,
        int stacks_num,
        int decks_num
);

void fc_solve_move_stack_normalize(
    fcs_move_stack_t * moves,
    fcs_state_extra_info_t * init_state_val,
    int freecells_num,
    int stacks_num,
    int decks_num
    );

extern char * fc_solve_move_to_string(fcs_move_t move, int standard_notation);

extern char * fc_solve_move_to_string_w_state(
        fcs_state_extra_info_t * state_val,
        int freecells_num,
        int stacks_num,
        int decks_num,
        fcs_move_t move,
        int standard_notation
        );

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

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

#define FCS_MOVE_STACK_GROW_BY 16

/* This macro allocates an empty move stack */
#define fcs_move_stack_alloc_into_var(final_ret) \
{       \
    fcs_move_stack_t * ret; \
        \
    /* Allocate the data structure itself */      \
    ret = (fcs_move_stack_t *)malloc(sizeof(fcs_move_stack_t));    \
       \
    ret->num_moves = 0;        \
    /* Allocate some space for the moves */     \
    ret->moves = (fcs_move_t *)malloc(sizeof(ret->moves[0])*FCS_MOVE_STACK_GROW_BY);  \
                \
    (final_ret) = ret;       \
}

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

extern void fc_solve_derived_states_list_add_state(
        fcs_derived_states_list_t * list,
        fcs_state_extra_info_t * state_val,
        int context
        );

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__MOVE_H */
