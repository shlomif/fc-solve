/*
 * move.h - header file for the move and move stacks functions of
 * Freecell Solver
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#ifndef __MOVE_H
#define __MOVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "state.h"
#include "fcs_move.h"


#if 0
fcs_move_stack_t * fcs_move_stack_create(void);
int fcs_move_stack_push(fcs_move_stack_t * stack, fcs_move_t move);
#endif

#define fcs_move_stack_pop(stack,move) (freecell_solver_move_stack_pop(stack,move))
extern int freecell_solver_move_stack_pop(fcs_move_stack_t * stack, fcs_move_t * move);

#if 0
void fcs_move_stack_destroy(fcs_move_stack_t * stack);
#endif

#define fcs_move_stack_destroy(stack) \
{     \
    free((stack)->moves);  \
    free(stack); \
}

extern void freecell_solver_move_stack_swallow_stack(fcs_move_stack_t * stack, fcs_move_stack_t * src_stack);
#if 0
void fcs_move_stack_reset(fcs_move_stack_t * stack);
#endif
#define fcs_move_stack_reset(stack) \
{      \
    (stack)->num_moves = 0;   \
}



#define fcs_move_stack_get_num_moves(stack) (freecell_solver_move_stack_get_num_moves(stack))
extern int freecell_solver_move_stack_get_num_moves(fcs_move_stack_t * stack);

#if 0
fcs_move_stack_t * fcs_move_stack_duplicate(fcs_move_stack_t * stack);
#endif
#define fcs_move_stack_duplicate_into_var(final_ret,stack) \
{        \
    fcs_move_stack_t * ret;     \
    fcs_move_stack_t * temp_stack=(stack) ; \
           \
    ret = (fcs_move_stack_t *)malloc(sizeof(fcs_move_stack_t));     \
                 \
    ret->max_num_moves = temp_stack->max_num_moves;      \
    ret->num_moves = temp_stack->num_moves;         \
    ret->moves = (fcs_move_t *)malloc(sizeof(fcs_move_t) * ret->max_num_moves);     \
    memcpy(ret->moves, temp_stack->moves, sizeof(fcs_move_t) * ret->max_num_moves);    \
        \
    (final_ret) = ret;       \
}



void freecell_solver_apply_move(fcs_state_with_locations_t * state_with_locations, fcs_move_t move, int freecells_num, int stacks_num, int decks_num);

void freecell_solver_move_stack_normalize(
    fcs_move_stack_t * moves,
    fcs_state_with_locations_t * init_state,
    int freecells_num,
    int stacks_num,
    int decks_num
    );

extern char * freecell_solver_move_to_string(fcs_move_t move, int standard_notation);

extern char * freecell_solver_move_to_string_w_state(fcs_state_with_locations_t * state, int freecells_num, int stacks_num, int decks_num, fcs_move_t move, int standard_notation);

struct fcs_derived_state_struct
{
    fcs_state_with_locations_t * ptr_state;
    fcs_move_stack_t * move_stack;
};

typedef struct fcs_derived_state_struct fcs_derived_state_t;

struct fcs_derived_states_list_struct
{
    int num_states;
    int max_num_states;
    fcs_derived_state_t * states;
};

typedef struct fcs_derived_states_list_struct fcs_derived_states_list_t;

#if 0
extern void fcs_derived_states_list_add_state(
    fcs_derived_states_list_t * list,
    fcs_state_with_locations_t * state,
    fcs_move_stack_t * move_stack
    );
#endif

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
    ret->max_num_moves = FCS_MOVE_STACK_GROW_BY;      \
    ret->num_moves = 0;        \
    /* Allocate some space for the moves */     \
    ret->moves = (fcs_move_t *)malloc(sizeof(fcs_move_t)*ret->max_num_moves);  \
                \
    (final_ret) = ret;       \
}


#define fcs_move_stack_push(stack, move) \
{           \
    /* If all the moves inside the stack are taken then    \
       resize the move vector */       \
              \
    if (stack->num_moves == stack->max_num_moves) \
    {      \
        int a, b;   \
        a = (stack->max_num_moves >> 3);      \
        b = FCS_MOVE_STACK_GROW_BY;    \
        stack->max_num_moves += max(a,b);    \
        stack->moves = realloc(     \
            stack->moves,     \
            stack->max_num_moves * sizeof(fcs_move_t)   \
            );     \
    }       \
    stack->moves[stack->num_moves++] = move;    \
            \
}

#define fcs_derived_states_list_add_state(list,state_ptr,move_stack_ptr) \
    \
{       \
    if ((list)->num_states == (list)->max_num_states)  \
    {        \
        (list)->max_num_states += 16;     \
        (list)->states = realloc((list)->states, sizeof((list)->states[0]) * (list)->max_num_states); \
    }        \
    (list)->states[(list)->num_states].ptr_state = (state_ptr);    \
    (list)->states[(list)->num_states].move_stack = (move_stack_ptr);    \
    (list)->num_states++;        \
}

#define fcs_derived_states_list_free_move_stacks(the_list)         \
{                                                                 \
    num_states = (the_list)->num_states;                 \
    derived_states = (the_list)->states;                 \
    for(a=0;a<num_states;a++)                                     \
    {                                                             \
        if (derived_states[a].move_stack)                         \
        {                                                         \
            fcs_move_stack_destroy(derived_states[a].move_stack); \
        }                                                         \
        derived_states[a].move_stack = NULL;                      \
    }                                                             \
}


#ifdef __cplusplus
}
#endif

#endif  /* __MOVE_H */
