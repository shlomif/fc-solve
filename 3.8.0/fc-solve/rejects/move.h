/*
 * =====================================================================================
 *
 *       Filename:  move.h
 *
 *    Description:  rejected (or old) code from move.h
 *
 *        Version:  1.0
 *        Created:  16/05/09 10:14:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

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

extern int fc_solve_move_stack_get_num_moves(fcs_move_stack_t * stack);

fcs_move_stack_t * fcs_move_stack_create(void);

#define fcs_move_stack_destroy(stack) \
{     \
    fcs_move_stack_static_destroy(*stack); \
    free(stack); \
}

