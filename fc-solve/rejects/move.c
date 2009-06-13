/*
 * =====================================================================================
 *
 *       Filename:  move.c
 *
 *    Description:  rejects from move.c
 *
 *        Version:  1.0
 *        Created:  16/05/09 10:18:44
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

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

int fc_solve_move_stack_get_num_moves(
    fcs_move_stack_t * stack
    )
{
    return stack->num_moves;
}

#endif

#if 0
/* This function allocates an empty move stack */
fcs_move_stack_t * fcs_move_stack_create(void)
{
    fcs_move_stack_t * ret;

    /* Allocate the data structure itself */
    ret = (fcs_move_stack_t *)malloc(sizeof(fcs_move_stack_t));

    ret->num_moves = 0;
    /* Allocate some space for the moves */
    ret->moves = (fcs_move_t *)malloc(sizeof(fcs_move_t)*FCS_MOVE_STACK_GROW_BY);

    return ret;
}
#endif

#if 0
void fcs_move_stack_destroy(fcs_move_stack_t * stack)
{
    free(stack->moves);
    free(stack);
}
#endif

