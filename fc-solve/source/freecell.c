/*
 * freecell.c - The various movement tests performed by Freecell Solver
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000-2001
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#if FCS_STATE_STORAGE==FCS_STATE_STORAGE_LIBREDBLACK_TREE
#include <search.h>
#endif
#include <limits.h>


#include "config.h"
#include "state.h"
#include "card.h"
#include "fcs_dm.h"
#include "fcs.h"

#include "fcs_isa.h"
#include "tests.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#define state_with_locations (*ptr_state_with_locations)
#define state (ptr_state_with_locations->s)
#define new_state_with_locations (*ptr_new_state_with_locations)
#define new_state (ptr_new_state_with_locations->s)


#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

/*
 * Throughout this code the following local variables are used to quickly
 * access the instance's members:
 *
 * state_stacks_num - the number of stacks in the state
 * state_freecells_num - the number of freecells in the state
 * sequences_are_built_by - the type of sequences of this board.
 * */

/*
 * This function tries to move stack cards that are present at the
 * top of stacks to the foundations.
 * */
int freecell_solver_sfs_move_top_stack_cards_to_founds(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int stack;
    int cards_num;
    int deck;
    fcs_card_t card;
    fcs_card_t temp_card;
    int check;
    int state_stacks_num;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);

    state_stacks_num = instance->stacks_num;

    for(stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        if (cards_num)
        {
            /* Get the top card in the stack */
            card = fcs_stack_card(state,stack,cards_num-1);
            for(deck=0;deck<instance->decks_num;deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_card_num(card) - 1)
                {
                    /* We can put it there */

                    sfs_check_state_begin();

                    fcs_move_stack_reset(moves);

                    fcs_pop_stack_card(new_state, stack, temp_card);

                    fcs_increment_foundation(new_state, deck*4+fcs_card_suit(card));



                    fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FOUNDATION);
                    fcs_move_set_src_stack(temp_move,stack);
                    fcs_move_set_foundation(temp_move,deck*4+fcs_card_suit(card));

                    fcs_move_stack_push(moves, temp_move);

                    fcs_flip_top_card(stack);

                    /* The last move needs to be FCS_MOVE_TYPE_CANONIZE
                     * because it indicates that the internal order of the
                     * stacks
                     * and freecells may have changed. */
                    fcs_move_set_type(temp_move,FCS_MOVE_TYPE_CANONIZE);
                    fcs_move_stack_push(moves, temp_move);

                    sfs_check_state_end()
                    break;
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


/*
 * This test moves single cards that are present in the freecells to
 * the foundations.
 * */
int freecell_solver_sfs_move_freecell_cards_to_founds(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int fc;
    int deck;
    fcs_card_t card;
    int check;
    fcs_move_stack_t * moves;
    fcs_move_t temp_move;
    int state_freecells_num;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);

    state_freecells_num = instance->freecells_num;

    /* Now check the same for the free cells */
    for(fc=0;fc<state_freecells_num;fc++)
    {
        card = fcs_freecell_card(state, fc);
        if (fcs_card_card_num(card) != 0)
        {
            for(deck=0;deck<instance->decks_num;deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_card_num(card) - 1)
                {
                    /* We can put it there */
                    sfs_check_state_begin()

                    fcs_empty_freecell(new_state, fc);

                    fcs_increment_foundation(new_state, deck*4+fcs_card_suit(card));

                    fcs_move_stack_reset(moves);
                    fcs_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION);
                    fcs_move_set_src_freecell(temp_move,fc);
                    fcs_move_set_foundation(temp_move,deck*4+fcs_card_suit(card));

                    fcs_move_stack_push(moves, temp_move);

                    fcs_move_set_type(temp_move,FCS_MOVE_TYPE_CANONIZE);
                    fcs_move_stack_push(moves, temp_move);

                    sfs_check_state_end();
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int freecell_solver_sfs_move_freecell_cards_on_top_of_stacks(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int dest_cards_num;
    int ds, fc, dc;
    fcs_card_t dest_card, src_card, temp_card, dest_below_card;
    int check;
    fcs_move_stack_t * moves;
    fcs_move_t temp_move;
    int is_seq_in_dest;
    int num_cards_to_relocate;
    int freecells_to_fill, freestacks_to_fill;
    int a,b;
    int state_freecells_num, state_stacks_num, sequences_are_built_by;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);

    state_freecells_num = instance->freecells_num;
    state_stacks_num = instance->stacks_num;
    sequences_are_built_by = instance->sequences_are_built_by;

    /* Let's try to put cards in the freecells on top of stacks */

    /* ds stands for destination stack */
    for(ds=0;ds<state_stacks_num;ds++)
    {
        dest_cards_num = fcs_stack_len(state, ds);

        /* If the stack is not empty we can proceed */
        if (dest_cards_num > 0)
        {
            /*
             * Let's search for a suitable card in the stack
             * */
            for(dc=dest_cards_num-1;dc>=0;dc--)
            {
                dest_card = fcs_stack_card(state, ds, dc);

                /* Scan the freecells */
                for(fc=0;fc<state_freecells_num;fc++)
                {
                    src_card = fcs_freecell_card(state, fc);

                    /* If the freecell is not empty and dest_card is its parent
                     * */
                    if ( (fcs_card_card_num(src_card) != 0) &&
                         fcs_is_parent_card(src_card,dest_card)     )
                    {
                        /* Let's check if we can put it there */

                        /* Check if the destination card is already below a
                         * suitable card */
                        is_seq_in_dest = 0;
                        if (dest_cards_num - 1 > dc)
                        {
                            dest_below_card = fcs_stack_card(state, ds, dc+1);
                            if (fcs_is_parent_card(dest_below_card, dest_card))
                            {
                                is_seq_in_dest = 1;
                            }
                        }


                        if (! is_seq_in_dest)
                        {
                            num_cards_to_relocate = dest_cards_num - dc - 1;

                            freecells_to_fill = min(num_cards_to_relocate, num_freecells);

                            num_cards_to_relocate -= freecells_to_fill;

                            if (instance->empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)
                            {
                                freestacks_to_fill = min(num_cards_to_relocate, num_freestacks);

                                num_cards_to_relocate -= freestacks_to_fill;
                            }
                            else
                            {
                                freestacks_to_fill = 0;
                            }

                            if (num_cards_to_relocate == 0)
                            {
                                /* We can move it */

                                sfs_check_state_begin()

                                fcs_move_stack_reset(moves);

                                /* Fill the freecells with the top cards */

                                for(a=0 ; a<freecells_to_fill ; a++)
                                {
                                    /* Find a vacant freecell */
                                    for(b=0;b<state_freecells_num;b++)
                                    {
                                        if (fcs_freecell_card_num(new_state, b) == 0)
                                        {
                                            break;
                                        }
                                    }
                                    fcs_pop_stack_card(new_state, ds, temp_card);

                                    fcs_put_card_in_freecell(new_state, b, temp_card);

                                    fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                                    fcs_move_set_src_stack(temp_move,ds);
                                    fcs_move_set_dest_freecell(temp_move,b);
                                    fcs_move_stack_push(moves, temp_move);
                                }

                                /* Fill the free stacks with the cards below them */
                                for(a=0; a < freestacks_to_fill ; a++)
                                {
                                    /* Find a vacant stack */
                                    for(b=0;b<state_stacks_num;b++)
                                    {
                                        if (fcs_stack_len(new_state, b) == 0)
                                        {
                                            break;
                                        }
                                    }

                                    fcs_pop_stack_card(new_state, ds, temp_card);
                                    fcs_push_card_into_stack(new_state, b, temp_card);
                                    fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                    fcs_move_set_src_stack(temp_move,ds);
                                    fcs_move_set_dest_stack(temp_move,b);
                                    fcs_move_set_num_cards_in_seq(temp_move,1);
                                    fcs_move_stack_push(moves, temp_move);
                                }

                                /* Now put the freecell card on top of the stack */
                                fcs_push_card_into_stack(new_state, ds, src_card);
                                fcs_empty_freecell(new_state, fc);
                                fcs_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_STACK);
                                fcs_move_set_src_freecell(temp_move,fc);
                                fcs_move_set_dest_stack(temp_move,ds);
                                fcs_move_stack_push(moves, temp_move);

                                sfs_check_state_end()
                            }
                        }
                    }
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}



int freecell_solver_sfs_move_non_top_stack_cards_to_founds(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int check;

    int stack;
    int cards_num;
    int c, a, b;
    fcs_card_t temp_card, card;
    int deck;
    int state_freecells_num;
    int state_stacks_num;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    state_freecells_num = instance->freecells_num;
    state_stacks_num = instance->stacks_num;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);


    /* Now let's check if a card that is under some other cards can be placed
     * in the foundations. */

    for(stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        /*
         * We starts from cards_num-2 because the top card is already covered
         * by move_top_stack_cards_to_founds.
         * */
        for(c=cards_num-2 ; c >= 0 ; c--)
        {
            card = fcs_stack_card(state, stack, c);
            for(deck=0;deck<instance->decks_num;deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_card_num(card)-1)
                {
                    /* The card is foundation-able. Now let's check if we
                     * can move the cards above it to the freecells and
                     * stacks */

                    if ((num_freecells +
                        ((instance->empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD) ?
                            num_freestacks :
                            0
                        ))
                            >= cards_num-(c+1))
                    {
                        /* We can move it */

                        sfs_check_state_begin()

                        fcs_move_stack_reset(moves);

                        /* Fill the freecells with the top cards */
                        for(a=0 ; a<min(num_freecells, cards_num-(c+1)) ; a++)
                        {
                            /* Find a vacant freecell */
                            for(b=0; b<state_freecells_num; b++)
                            {
                                if (fcs_freecell_card_num(new_state, b) == 0)
                                {
                                    break;
                                }
                            }
                            fcs_pop_stack_card(new_state, stack, temp_card);

                            fcs_put_card_in_freecell(new_state, b, temp_card);

                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                            fcs_move_set_src_stack(temp_move,stack);
                            fcs_move_set_dest_freecell(temp_move,b);

                            fcs_move_stack_push(moves, temp_move);

                            fcs_flip_top_card(stack);
                        }

                        /* Fill the free stacks with the cards below them */
                        for(a=0; a < cards_num-(c+1) - min(num_freecells, cards_num-(c+1)) ; a++)
                        {
                            /* Find a vacant stack */
                            for(b=0;b<state_stacks_num;b++)
                            {
                                if (fcs_stack_len(new_state, b) == 0)
                                {
                                    break;
                                }
                            }

                            fcs_pop_stack_card(new_state, stack, temp_card);
                            fcs_push_card_into_stack(new_state, b, temp_card);

                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                            fcs_move_set_src_stack(temp_move,stack);
                            fcs_move_set_dest_stack(temp_move,b);
                            fcs_move_set_num_cards_in_seq(temp_move,1);

                            fcs_move_stack_push(moves, temp_move);

                            fcs_flip_top_card(stack);
                        }

                        fcs_pop_stack_card(new_state, stack, temp_card);
                        fcs_increment_foundation(new_state, deck*4+fcs_card_suit(temp_card));

                        fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FOUNDATION);
                        fcs_move_set_src_stack(temp_move,stack);
                        fcs_move_set_foundation(temp_move,deck*4+fcs_card_suit(temp_card));

                        fcs_move_stack_push(moves, temp_move);

                        fcs_flip_top_card(stack);

                        sfs_check_state_end()
                    }
                    break;
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}




int freecell_solver_sfs_move_stack_cards_to_a_parent_on_the_same_stack(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int check;

    int stack, c, cards_num, a, dc,b;
    int is_seq_in_dest;
    fcs_card_t card, temp_card, prev_card;
    fcs_card_t dest_below_card, dest_card;
    int freecells_to_fill, freestacks_to_fill;
    int dest_cards_num, num_cards_to_relocate;
    int state_freecells_num;
    int state_stacks_num;
    int sequences_are_built_by;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    state_freecells_num = instance->freecells_num;
    state_stacks_num = instance->stacks_num;
    sequences_are_built_by = instance->sequences_are_built_by;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);


    /*
     * Now let's try to move a stack card to a parent card which is found
     * on the same stack.
     * */
    for (stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);

        for (c=0 ; c<cards_num ; c++)
        {
            /* Find a card which this card can be put on; */

            card = fcs_stack_card(state, stack, c);


            /* Do not move cards that are already found above a suitable parent */
            a = 1;
            if (c != 0)
            {
                prev_card = fcs_stack_card(state, stack, c-1);
                if ((fcs_card_card_num(prev_card) == fcs_card_card_num(card)+1) &&
                    ((fcs_card_suit(prev_card) & 0x1) != (fcs_card_suit(card) & 0x1)))
                {
                   a = 0;
                }
            }
            if (a)
            {
#define ds stack
                /* Check if it can be moved to something on the same stack */
                dest_cards_num = fcs_stack_len(state, ds);
                for(dc=0;dc<c-1;dc++)
                {
                    dest_card = fcs_stack_card(state, ds, dc);
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* Corresponding cards - see if it is feasible to move
                           the source to the destination. */

                        is_seq_in_dest = 0;
                        dest_below_card = fcs_stack_card(state, ds, dc+1);
                        if (fcs_is_parent_card(dest_below_card, dest_card))
                        {
                            is_seq_in_dest = 1;
                        }

                        if (!is_seq_in_dest)
                        {
                            num_cards_to_relocate = dest_cards_num - dc - 1;

                            freecells_to_fill = min(num_cards_to_relocate, num_freecells);

                            num_cards_to_relocate -= freecells_to_fill;

                            if (instance->empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)
                            {
                                freestacks_to_fill = min(num_cards_to_relocate, num_freestacks);

                                num_cards_to_relocate -= freestacks_to_fill;
                            }
                            else
                            {
                                freestacks_to_fill = 0;
                            }

                            if (num_cards_to_relocate == 0)
                            {
                                /* We can move it */

                                sfs_check_state_begin()

                                fcs_move_stack_reset(moves);

                                {
                                    int i_card_pos;
                                    fcs_card_t moved_card;
                                    int source_type, source_index;

                                    i_card_pos = fcs_stack_len(new_state,stack)-1;
                                    a = 0;
                                    while(i_card_pos>c)
                                    {
                                        if (a < freecells_to_fill)
                                        {
                                            for(b=0;b<state_freecells_num;b++)
                                            {
                                                if (fcs_freecell_card_num(new_state, b) == 0)
                                                {
                                                    break;
                                                }
                                            }
                                            fcs_pop_stack_card(new_state, ds, temp_card);
                                            fcs_put_card_in_freecell(new_state, b, temp_card);

                                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                                            fcs_move_set_src_stack(temp_move,ds);
                                            fcs_move_set_dest_freecell(temp_move,b);

                                            fcs_move_stack_push(moves, temp_move);

                                        }
                                        else
                                        {

                                            /*  Find a vacant stack */
                                            for(b=0;b<state_stacks_num;b++)
                                            {
                                                if (fcs_stack_len(new_state, b) == 0)
                                                {
                                                    break;
                                                }
                                            }

                                            fcs_pop_stack_card(new_state, ds, temp_card);
                                            fcs_push_card_into_stack(new_state, b, temp_card);

                                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                            fcs_move_set_src_stack(temp_move,ds);
                                            fcs_move_set_dest_stack(temp_move,b);
                                            fcs_move_set_num_cards_in_seq(temp_move,1);

                                            fcs_move_stack_push(moves, temp_move);

                                        }
                                        a++;

                                        i_card_pos--;
                                    }
                                    fcs_pop_stack_card(new_state, ds, moved_card);
                                    if (a < freecells_to_fill)
                                    {
                                        for(b=0;b<state_freecells_num;b++)
                                        {
                                            if (fcs_freecell_card_num(new_state, b) == 0)
                                            {
                                                break;
                                            }
                                        }
                                        fcs_put_card_in_freecell(new_state, b, moved_card);
                                        fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                                        fcs_move_set_src_stack(temp_move,ds);
                                        fcs_move_set_dest_freecell(temp_move,b);
                                        fcs_move_stack_push(moves, temp_move);

                                        source_type = 0;
                                        source_index = b;
                                    }
                                    else
                                    {
                                        for(b=0;b<state_stacks_num;b++)
                                        {
                                            if (fcs_stack_len(new_state, b) == 0)
                                            {
                                                break;
                                            }
                                        }
                                        fcs_push_card_into_stack(new_state, b, moved_card);

                                        fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                        fcs_move_set_src_stack(temp_move,ds);
                                        fcs_move_set_dest_stack(temp_move,b);
                                        fcs_move_set_num_cards_in_seq(temp_move,1);
                                        fcs_move_stack_push(moves, temp_move);

                                        source_type = 1;
                                        source_index = b;
                                    }
                                    i_card_pos--;
                                    a++;

                                    while(i_card_pos>dc)
                                    {
                                        if (a < freecells_to_fill)
                                        {
                                            for(b=0;b<state_freecells_num;b++)
                                            {
                                                if (fcs_freecell_card_num(new_state, b) == 0)
                                                {
                                                    break;
                                                }
                                            }
                                            fcs_pop_stack_card(new_state, ds, temp_card);
                                            fcs_put_card_in_freecell(new_state, b, temp_card);

                                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                                            fcs_move_set_src_stack(temp_move,ds);
                                            fcs_move_set_dest_freecell(temp_move,b);

                                            fcs_move_stack_push(moves, temp_move);
                                        }
                                        else
                                        {

                                            /*  Find a vacant stack */
                                            for(b=0;b<state_stacks_num;b++)
                                            {
                                                if (fcs_stack_len(new_state, b) == 0)
                                                {
                                                    break;
                                                }
                                            }

                                            fcs_pop_stack_card(new_state, ds, temp_card);
                                            fcs_push_card_into_stack(new_state, b, temp_card);

                                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                            fcs_move_set_src_stack(temp_move,ds);
                                            fcs_move_set_dest_stack(temp_move,b);
                                            fcs_move_set_num_cards_in_seq(temp_move,1);

                                            fcs_move_stack_push(moves, temp_move);

                                        }
                                        a++;

                                        i_card_pos--;
                                    }

                                    if (source_type == 0)
                                    {
                                        moved_card = fcs_freecell_card(new_state, source_index);
                                        fcs_empty_freecell(new_state, source_index);

                                        fcs_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_STACK);
                                        fcs_move_set_src_freecell(temp_move,source_index);
                                        fcs_move_set_dest_stack(temp_move,ds);
                                        fcs_move_stack_push(moves, temp_move);
                                    }
                                    else
                                    {
                                        fcs_pop_stack_card(new_state, source_index, moved_card);

                                        fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                        fcs_move_set_src_stack(temp_move,source_index);
                                        fcs_move_set_dest_stack(temp_move,ds);
                                        fcs_move_set_num_cards_in_seq(temp_move,1);
                                        fcs_move_stack_push(moves, temp_move);
                                    }

                                    fcs_push_card_into_stack(new_state, ds, moved_card);
                                }

                                sfs_check_state_end()
                            }
                        }
                    }
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}
#undef ds


int freecell_solver_sfs_move_stack_cards_to_different_stacks(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int check;

    int stack, c, cards_num, a, dc, ds,b;
    int is_seq_in_dest;
    fcs_card_t card, temp_card, this_card, prev_card;
    fcs_card_t dest_below_card, dest_card;
    int freecells_to_fill, freestacks_to_fill;
    int dest_cards_num, num_cards_to_relocate;
    int seq_end;
    int state_freecells_num;
    int state_stacks_num;
    int sequences_are_built_by;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    state_freecells_num = instance->freecells_num;
    state_stacks_num = instance->stacks_num;
    sequences_are_built_by = instance->sequences_are_built_by;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);


    /* Now let's try to move a card from one stack to the other     *
     * Note that it does not involve moving cards lower than king   *
     * to empty stacks                                              */

    for (stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);

        for (c=0 ; c<cards_num ; c=seq_end+1)
        {
            /* Check if there is a sequence here. */
            for(seq_end=c ; seq_end<cards_num-1 ; seq_end++)
            {
                this_card = fcs_stack_card(state, stack, seq_end+1);
                prev_card = fcs_stack_card(state, stack, seq_end);

                if (fcs_is_parent_card(this_card,prev_card))
                {
                }
                else
                {
                    break;
                }
            }

            /* Find a card which this card can be put on; */

            card = fcs_stack_card(state, stack, c);

            /* Make sure the card is not flipped or else we can't move it */
            if (fcs_card_get_flipped(card) == 0)
            {
                for(ds=0 ; ds<state_stacks_num; ds++)
                {
                    if (ds != stack)
                    {
                        dest_cards_num = fcs_stack_len(state, ds);
                        for(dc=0;dc<dest_cards_num;dc++)
                        {
                            dest_card = fcs_stack_card(state, ds, dc);

                            if (fcs_is_parent_card(card, dest_card))
                            {
                                /* Corresponding cards - see if it is feasible to move
                                   the source to the destination. */

                                is_seq_in_dest = 0;
                                if (dest_cards_num - 1 > dc)
                                {
                                    dest_below_card = fcs_stack_card(state, ds, dc+1);
                                    if (fcs_is_parent_card(dest_below_card, dest_card))
                                    {
                                        is_seq_in_dest = 1;
                                    }
                                }

                                if (! is_seq_in_dest)
                                {
                                    num_cards_to_relocate = dest_cards_num - dc - 1 + cards_num - seq_end - 1;

                                    freecells_to_fill = min(num_cards_to_relocate, num_freecells);

                                    num_cards_to_relocate -= freecells_to_fill;

                                    if (instance->empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)
                                    {
                                        freestacks_to_fill = min(num_cards_to_relocate, num_freestacks);

                                        num_cards_to_relocate -= freestacks_to_fill;
                                    }
                                    else
                                    {
                                        freestacks_to_fill = 0;
                                    }

                                    if ((num_cards_to_relocate == 0) &&
                                       (calc_max_sequence_move(num_freecells-freecells_to_fill, num_freestacks-freestacks_to_fill) >=
                                        seq_end - c + 1))
                                    {
                                        /* We can move it */
                                        int from_which_stack;

                                        sfs_check_state_begin()

                                        fcs_move_stack_reset(moves);

                                        /* Fill the freecells with the top cards */

                                        for(a=0 ; a<freecells_to_fill ; a++)
                                        {
                                            /* Find a vacant freecell */
                                            for(b=0;b<state_freecells_num;b++)
                                            {
                                                if (fcs_freecell_card_num(new_state, b) == 0)
                                                {
                                                    break;
                                                }
                                            }

                                            if (fcs_stack_len(new_state, ds) == dc+1)
                                            {
                                                from_which_stack = stack;
                                            }
                                            else
                                            {
                                                from_which_stack = ds;
                                            }
                                            fcs_pop_stack_card(new_state, from_which_stack, temp_card);

                                            fcs_put_card_in_freecell(new_state, b, temp_card);

                                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                                            fcs_move_set_src_stack(temp_move,from_which_stack);
                                            fcs_move_set_dest_freecell(temp_move,b);
                                            fcs_move_stack_push(moves, temp_move);

                                            fcs_flip_top_card(from_which_stack);
                                        }

                                        /* Fill the free stacks with the cards below them */
                                        for(a=0; a < freestacks_to_fill ; a++)
                                        {
                                            /*  Find a vacant stack */
                                            for(b=0;b<state_stacks_num;b++)
                                            {
                                                if (fcs_stack_len(new_state, b) == 0)
                                                {
                                                    break;
                                                }
                                            }

                                            if (fcs_stack_len(new_state, ds) == dc+1)
                                            {
                                                from_which_stack = stack;
                                            }
                                            else
                                            {
                                                from_which_stack = ds;
                                            }


                                            fcs_pop_stack_card(new_state, from_which_stack, temp_card);
                                            fcs_push_card_into_stack(new_state, b, temp_card);
                                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                            fcs_move_set_src_stack(temp_move,from_which_stack);
                                            fcs_move_set_dest_stack(temp_move,b);
                                            fcs_move_set_num_cards_in_seq(temp_move,1);
                                            fcs_move_stack_push(moves, temp_move);

                                            fcs_flip_top_card(from_which_stack);
                                        }

                                        for(a=c ; a <= seq_end ; a++)
                                        {
                                            fcs_push_stack_card_into_stack(new_state, ds, stack, a);
                                        }

                                        for(a=0; a < seq_end-c+1 ;a++)
                                        {
                                            fcs_pop_stack_card(new_state, stack, temp_card);
                                        }
                                        fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                        fcs_move_set_src_stack(temp_move,stack);
                                        fcs_move_set_dest_stack(temp_move,ds);
                                        fcs_move_set_num_cards_in_seq(temp_move,seq_end-c+1);
                                        fcs_move_stack_push(moves, temp_move);

                                        fcs_flip_top_card(stack);

                                        sfs_check_state_end()
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}



int freecell_solver_sfs_move_sequences_to_free_stacks(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int check;

    int stack, cards_num, c, ds, a, b, seq_end;
    fcs_card_t this_card, prev_card, temp_card;
    int max_sequence_len;
    int num_cards_to_relocate, freecells_to_fill, freestacks_to_fill;
    int state_freecells_num;
    int state_stacks_num;
    int sequences_are_built_by;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    if (instance->empty_stacks_fill == FCS_ES_FILLED_BY_NONE)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    state_freecells_num = instance->freecells_num;
    state_stacks_num = instance->stacks_num;
    sequences_are_built_by = instance->sequences_are_built_by;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);

    max_sequence_len = calc_max_sequence_move(num_freecells, num_freestacks-1);

    /* Now try to move sequences to empty stacks */

    if (num_freestacks > 0)
    {
        for(stack=0;stack<state_stacks_num;stack++)
        {
            cards_num = fcs_stack_len(state, stack);

            for(c=0; c<cards_num; c=seq_end+1)
            {
                /* Check if there is a sequence here. */
                for(seq_end=c ; seq_end<cards_num-1; seq_end++)
                {
                    this_card = fcs_stack_card(state, stack, seq_end+1);
                    prev_card = fcs_stack_card(state, stack, seq_end);

                    if (! fcs_is_parent_card(this_card, prev_card))
                    {
                        break;
                    }
                }

                if ((fcs_stack_card_num(state, stack, c) != 13) &&
                    (instance->empty_stacks_fill == FCS_ES_FILLED_BY_KINGS_ONLY))
                {
                    continue;
                }

                if (seq_end == cards_num -1)
                {
                    /* One stack is the destination stack, so we have one     *
                     * less stack in that case                                */
                    while ((max_sequence_len < cards_num -c) && (c > 0))
                    {
                        c--;
                    }

                    if (
                        (c > 0) &&
                        ((instance->empty_stacks_fill == FCS_ES_FILLED_BY_KINGS_ONLY) ?
                            (fcs_card_card_num(fcs_stack_card(state, stack, c)) == 13) :
                            1
                        )
                       )
                    {
                        sfs_check_state_begin();

                        fcs_move_stack_reset(moves);

                        for(ds=0;ds<state_stacks_num;ds++)
                        {
                            if (fcs_stack_len(state, ds) == 0)
                                break;
                        }

                        for(a=c ; a <= cards_num-1 ; a++)
                        {
                            fcs_push_stack_card_into_stack(new_state, ds, stack, a);
                        }

                        for(a=0;a<cards_num-c;a++)
                        {
                            fcs_pop_stack_card(new_state, stack, temp_card);
                        }

                        fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                        fcs_move_set_src_stack(temp_move,stack);
                        fcs_move_set_dest_stack(temp_move,ds);
                        fcs_move_set_num_cards_in_seq(temp_move,cards_num-c);

                        fcs_move_stack_push(moves, temp_move);


                        sfs_check_state_end()
                    }
                }
                else
                {
                    num_cards_to_relocate = cards_num - seq_end - 1;

                    freecells_to_fill = min(num_cards_to_relocate, num_freecells);

                    num_cards_to_relocate -= freecells_to_fill;

                    if (instance->empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)
                    {
                        freestacks_to_fill = min(num_cards_to_relocate, num_freestacks);

                        num_cards_to_relocate -= freestacks_to_fill;
                    }
                    else
                    {
                        freestacks_to_fill = 0;
                    }

                    if ((num_cards_to_relocate == 0) && (num_freestacks-freestacks_to_fill > 0))
                    {
                        /* We can move it */
                        int seq_start = c;
                        while (
                            (calc_max_sequence_move(
                                num_freecells-freecells_to_fill,
                                num_freestacks-freestacks_to_fill-1) < seq_end-seq_start+1)
                                &&
                            (seq_start <= seq_end)
                            )
                        {
                            seq_start++;
                        }
                        if ((seq_start <= seq_end) &&
                            ((instance->empty_stacks_fill == FCS_ES_FILLED_BY_KINGS_ONLY) ?
                                (fcs_card_card_num(fcs_stack_card(state, stack, seq_start)) == 13) :
                                1
                            )
                        )
                        {
                            sfs_check_state_begin();

                            fcs_move_stack_reset(moves);

                            /* Fill the freecells with the top cards */

                            for(a=0; a<freecells_to_fill ; a++)
                            {
                                /* Find a vacant freecell */
                                for(b=0;b<state_freecells_num;b++)
                                {
                                    if (fcs_freecell_card_num(new_state, b) == 0)
                                    {
                                        break;
                                    }
                                }
                                fcs_pop_stack_card(new_state, stack, temp_card);
                                fcs_put_card_in_freecell(new_state, b, temp_card);

                                fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                                fcs_move_set_src_stack(temp_move,stack);
                                fcs_move_set_dest_freecell(temp_move,b);
                                fcs_move_stack_push(moves, temp_move);
                            }

                            /* Fill the free stacks with the cards below them */
                            for(a=0; a < freestacks_to_fill ; a++)
                            {
                                /* Find a vacant stack */
                                for(b=0; b<state_stacks_num; b++)
                                {
                                    if (fcs_stack_len(new_state, b) == 0)
                                    {
                                        break;
                                    }
                                }
                                fcs_pop_stack_card(new_state, stack, temp_card);
                                fcs_push_card_into_stack(new_state, b, temp_card);
                                fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                fcs_move_set_src_stack(temp_move,stack);
                                fcs_move_set_dest_stack(temp_move,b);
                                fcs_move_set_num_cards_in_seq(temp_move,1);
                                fcs_move_stack_push(moves, temp_move);
                            }

                            /* Find a vacant stack */
                            for(b=0; b<state_stacks_num; b++)
                            {
                                if (fcs_stack_len(new_state, b) == 0)
                                {
                                    break;
                                }
                            }

                            for(a=seq_start ; a <= seq_end ; a++)
                            {
                                fcs_push_stack_card_into_stack(new_state, b, stack, a);
                            }
                            for(a=seq_start ; a <= seq_end ; a++)
                            {
                                fcs_pop_stack_card(new_state, stack, temp_card);
                            }

                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                            fcs_move_set_src_stack(temp_move,stack);
                            fcs_move_set_dest_stack(temp_move,b);
                            fcs_move_set_num_cards_in_seq(temp_move,seq_end-seq_start+1);
                            fcs_move_stack_push(moves, temp_move);

                            sfs_check_state_end();
                        }
                    }
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}



int freecell_solver_sfs_move_freecell_cards_to_empty_stack(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int check;
    int fc, stack;
    fcs_card_t card;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    int state_freecells_num;
    int state_stacks_num;

    /* Let's try to put cards that occupy freecells on an empty stack */

    if (instance->empty_stacks_fill == FCS_ES_FILLED_BY_NONE)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    state_freecells_num = instance->freecells_num;
    state_stacks_num = instance->stacks_num;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);

    for(fc=0;fc<state_freecells_num;fc++)
    {
        card = fcs_freecell_card(state, fc);
        if (
            (instance->empty_stacks_fill == FCS_ES_FILLED_BY_KINGS_ONLY) ?
                (fcs_card_card_num(card) == 13) :
                (fcs_card_card_num(card) != 0)
           )
        {
            for(stack=0;stack<state_stacks_num;stack++)
            {
                if (fcs_stack_len(state, stack) == 0)
                {
                    break;
                }
            }
            if (stack != state_stacks_num)
            {
                /* We can move it */

                sfs_check_state_begin();

                fcs_move_stack_reset(moves);

                fcs_push_card_into_stack(new_state, stack, card);
                fcs_empty_freecell(new_state, fc);

                fcs_move_set_type(temp_move,FCS_MOVE_TYPE_FREECELL_TO_STACK);
                fcs_move_set_src_freecell(temp_move,fc);
                fcs_move_set_dest_stack(temp_move,stack);
                fcs_move_stack_push(moves, temp_move);

                sfs_check_state_end()
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int freecell_solver_sfs_move_cards_to_a_different_parent(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int check;

    int stack, cards_num, c, a, b, ds, dc;
    int is_seq_in_src, is_seq_in_dest;
    int num_cards_to_relocate;
    int dest_cards_num;
    fcs_card_t card, this_card, prev_card, temp_card;
    fcs_card_t dest_card, dest_below_card;
    int freecells_to_fill, freestacks_to_fill;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    int state_freecells_num;
    int state_stacks_num;
    int sequences_are_built_by;

    state_freecells_num = instance->freecells_num;
    state_stacks_num = instance->stacks_num;
    sequences_are_built_by = instance->sequences_are_built_by;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);


    /* This time try to move cards that are already on top of a parent to a different parent */

    for (stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);

        for (c=0 ; c<cards_num ; c++)
        {
            /* Check if there is a sequence here. */
            is_seq_in_src = 1;
            for(a=c+1 ; a<cards_num ; a++)
            {
                this_card = fcs_stack_card(state, stack, a);
                prev_card = fcs_stack_card(state, stack, a-1);

                if (fcs_is_parent_card(this_card,prev_card))
                {
                }
                else
                {
                    is_seq_in_src = 0;
                    break;
                }
            }

            /* Find a card which this card can be put on; */

            card = fcs_stack_card(state, stack, c);


            /* Do not move cards that are already found above a suitable parent */
            a = 1;
            if (c != 0)
            {
                prev_card = fcs_stack_card(state, stack, c-1);
                if (fcs_is_parent_card(card,prev_card))
                {
                   a = 0;
                }
            }
            /* And do not move cards that are flipped */
            if (!a)
            {
                this_card = fcs_stack_card(state,stack,c);
                if (fcs_card_get_flipped(this_card))
                {
                    a = 0;
                }
            }
            if (!a)
            {
                for(ds=0 ; ds<state_stacks_num; ds++)
                {
                    if (ds != stack)
                    {
                        dest_cards_num = fcs_stack_len(state, ds);
                        for(dc=0;dc<dest_cards_num;dc++)
                        {
                            dest_card = fcs_stack_card(state, ds, dc);

                            if (fcs_is_parent_card(card,dest_card))
                            {
                                /* Corresponding cards - see if it is feasible to move
                                   the source to the destination. */

                                is_seq_in_dest = 0;
                                if (dest_cards_num - 1 > dc)
                                {
                                    dest_below_card = fcs_stack_card(state, ds, dc+1);
                                    if (fcs_is_parent_card(dest_below_card,dest_card))
                                    {
                                        is_seq_in_dest = 1;
                                    }
                                }

                                if (! is_seq_in_dest)
                                {
                                    if (is_seq_in_src)
                                    {
                                        num_cards_to_relocate = dest_cards_num - dc - 1;

                                        freecells_to_fill = min(num_cards_to_relocate, num_freecells);

                                        num_cards_to_relocate -= freecells_to_fill;

                                        if (instance->empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)
                                        {
                                            freestacks_to_fill = min(num_cards_to_relocate, num_freestacks);

                                            num_cards_to_relocate -= freestacks_to_fill;
                                        }
                                        else
                                        {
                                            freestacks_to_fill = 0;
                                        }

                                        if ((num_cards_to_relocate == 0) &&
                                           (calc_max_sequence_move(num_freecells-freecells_to_fill, num_freestacks-freestacks_to_fill) >=
                                            cards_num - c))
                                        {
                                            /* We can move it */

                                            sfs_check_state_begin()

                                            fcs_move_stack_reset(moves);

                                            /* Fill the freecells with the top cards */
                                            for(a=0 ; a<freecells_to_fill ; a++)
                                            {
                                                /* Find a vacant freecell */
                                                for(b=0;b<state_freecells_num;b++)
                                                {
                                                    if (fcs_freecell_card_num(new_state, b) == 0)
                                                    {
                                                        break;
                                                    }
                                                }

                                                fcs_pop_stack_card(new_state, ds, temp_card);

                                                fcs_put_card_in_freecell(new_state, b, temp_card);

                                                fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                                                fcs_move_set_src_stack(temp_move,ds);
                                                fcs_move_set_dest_freecell(temp_move,b);
                                                fcs_move_stack_push(moves, temp_move);
                                            }

                                            /* Fill the free stacks with the cards below them */
                                            for(a=0; a < freestacks_to_fill ; a++)
                                            {
                                                /*  Find a vacant stack */
                                                for(b=0;b<state_stacks_num;b++)
                                                {
                                                    if (fcs_stack_len(new_state, b) == 0)
                                                    {
                                                        break;
                                                    }
                                                }

                                                fcs_pop_stack_card(new_state, ds, temp_card);
                                                fcs_push_card_into_stack(new_state, b, temp_card);

                                                fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                                fcs_move_set_src_stack(temp_move,ds);
                                                fcs_move_set_dest_stack(temp_move,b);
                                                fcs_move_set_num_cards_in_seq(temp_move,1);
                                                fcs_move_stack_push(moves, temp_move);
                                            }

                                            for(a=c ; a <= cards_num-1 ; a++)
                                            {
                                                fcs_push_stack_card_into_stack(new_state, ds, stack, a);
                                            }

                                            for(a=0;a<cards_num-c;a++)
                                            {
                                                fcs_pop_stack_card(new_state, stack, temp_card);
                                            }

                                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                            fcs_move_set_src_stack(temp_move,stack);
                                            fcs_move_set_dest_stack(temp_move,ds);
                                            fcs_move_set_num_cards_in_seq(temp_move,cards_num-c);
                                            fcs_move_stack_push(moves, temp_move);

                                            sfs_check_state_end()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}




int freecell_solver_sfs_empty_stack_into_freecells(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int check;

    int stack, cards_num, c, b;
    fcs_card_t temp_card;
    int state_stacks_num;
    int state_freecells_num;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    if (instance->empty_stacks_fill == FCS_ES_FILLED_BY_NONE)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    state_stacks_num = instance->stacks_num;
    state_freecells_num = instance->freecells_num;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);



    /* Now, let's try to empty an entire stack into the freecells, so other cards can
     * inhabit it */

    if (num_freestacks == 0)
    {
        for(stack=0;stack<state_stacks_num;stack++)
        {
            cards_num = fcs_stack_len(state, stack);
            if (cards_num <= num_freecells)
            {
                /* We can empty it */

                sfs_check_state_begin()

                fcs_move_stack_reset(moves);

                for(c=0;c<cards_num;c++)
                {
                    /* Find a vacant freecell */
                    for(b=0; b<state_freecells_num; b++)
                    {
                        if (fcs_freecell_card_num(new_state, b) == 0)
                        {
                            break;
                        }
                    }
                    fcs_pop_stack_card(new_state, stack, temp_card);

                    fcs_put_card_in_freecell(new_state, b, temp_card);

                    fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_FREECELL);
                    fcs_move_set_src_stack(temp_move,stack);
                    fcs_move_set_dest_freecell(temp_move,b);
                    fcs_move_stack_push(moves, temp_move);
                }

                sfs_check_state_end()
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;

}

int freecell_solver_sfs_yukon_do_nothing(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int freecell_solver_sfs_yukon_move_card_to_parent(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int check;

    int stack, cards_num, c, a, ds;
    int dest_cards_num;
    fcs_card_t card, temp_card;
    fcs_card_t dest_card;

    int state_stacks_num;
    int sequences_are_built_by;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    state_stacks_num = instance->stacks_num;
    sequences_are_built_by = instance->sequences_are_built_by;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);

    for( ds=0 ; ds < state_stacks_num ; ds++ )
    {
        dest_cards_num = fcs_stack_len(state, ds);
        if (dest_cards_num > 0)
        {
            dest_card = fcs_stack_card(state, ds, dest_cards_num-1);
            for( stack=0 ; stack < state_stacks_num ; stack++)
            {
                if (stack == ds)
                {
                    continue;
                }
                cards_num = fcs_stack_len(state, stack);
                for( c=cards_num-1 ; c >= 0 ; c--)
                {
                    card = fcs_stack_card(state, stack, c);
                    if (fcs_card_get_flipped(card))
                    {
                        break;
                    }
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* We can move it there - now let's check to see
                         * if it is already above a suitable parent. */
                        if ((c == 0) ||
                            (! fcs_is_parent_card(card, fcs_stack_card(state, stack, c-1))))
                        {
                            /* Let's move it */
                            sfs_check_state_begin();

                            fcs_move_stack_reset(moves);

                            fcs_move_sequence(ds, stack, c, cards_num-1, a);

                            fcs_flip_top_card(stack);

                            sfs_check_state_end();
                        }

                    }
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int freecell_solver_sfs_yukon_move_kings_to_empty_stack(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int check;

    int stack, cards_num, c, a, ds;
    fcs_card_t card, temp_card;

    int state_stacks_num;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    if (num_freestacks == 0)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    state_stacks_num = instance->stacks_num;

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);


    for( stack=0 ; stack < state_stacks_num ; stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        for( c=cards_num-1 ; c >= 1 ; c--)
        {
            card = fcs_stack_card(state, stack, c);
            if (fcs_card_get_flipped(card))
            {
                break;
            }
            if (fcs_card_card_num(card) == 13)
            {
                /* It's a King - so let's move it */
                sfs_check_state_begin();

                fcs_move_stack_reset(moves);

                for( ds=0 ; ds < state_stacks_num ; ds++)
                {
                    if (fcs_stack_len(state, ds) == 0)
                    {
                        break;
                    }
                }
                fcs_move_sequence(ds, stack, c, cards_num-1, a);


                fcs_flip_top_card(stack);

                sfs_check_state_end();
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}



#ifdef FCS_WITH_TALONS
/*
    Let's try to deal the Gypsy-type Talon.

  */
int freecell_solver_sfs_deal_gypsy_talon(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_new_state_with_locations;
    int check;

    fcs_card_t temp_card;
    int a;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    if (instance->talon_type != FCS_TALON_GYPSY)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);

    if (fcs_talon_pos(state) < fcs_talon_len(state))
    {
        sfs_check_state_begin()
        for(a=0;a<state_stacks_num;a++)
        {
            temp_card = fcs_get_talon_card(new_state, fcs_talon_pos(new_state)+a);
            fcs_push_card_into_stack(new_state,a,temp_card);
        }
        fcs_talon_pos(new_state) += state_stacks_num;
        fcs_move_set_type(temp_move, FCS_MOVE_TYPE_DEAL_GYPSY_TALON);
        fcs_move_stack_push(moves, temp_move);

        sfs_check_state_end()
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


int freecell_solver_sfs_get_card_from_klondike_talon(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int depth,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;


    fcs_state_with_locations_t * ptr_new_state_with_locations;
    fcs_state_with_locations_t * talon_temp;

    fcs_move_stack_t * moves;
    fcs_move_t temp_move;

    int check;
    int num_redeals_left, num_redeals_done, num_cards_moved[2];
    int first_iter;
    fcs_card_t card_to_check, top_card;
    int s;
    int cards_num;
    int a;

    if (instance->talon_type != FCS_TALON_KLONDIKE)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    moves = hard_thread->reusable_move_stack;
    fcs_move_stack_reset(moves);

    /* Duplicate the talon and its parameters into talon_temp */
    talon_temp = malloc(sizeof(fcs_state_with_locations_t));
    talon_temp->s.talon = malloc(fcs_klondike_talon_len(state)+1);
    memcpy(
        talon_temp->s.talon,
        ptr_state_with_locations->s.talon,
        fcs_klondike_talon_len(state)+1
        );
    memcpy(
        talon_temp->s.talon_params,
        ptr_state_with_locations->s.talon_params,
        sizeof(ptr_state_with_locations->s.talon_params)
        );

    /* Make sure we redeal the talon only once */
    num_redeals_left = fcs_klondike_talon_num_redeals_left(state);
    if ((num_redeals_left > 0) || (num_redeals_left < 0))
    {
        num_redeals_left = 1;
    }
    num_redeals_done = 0;
    num_cards_moved[0] = 0;
    num_cards_moved[1] = 0;

    first_iter = 1;
    while (num_redeals_left >= 0)
    {
        if ((fcs_klondike_talon_stack_pos(talon_temp->s) == -1) &&
            (fcs_klondike_talon_queue_pos(talon_temp->s) == fcs_klondike_talon_len(talon_temp->s)))
        {
            break;
        }
        if ((!first_iter) || (fcs_klondike_talon_stack_pos(talon_temp->s) == -1))
        {
            if (fcs_klondike_talon_queue_pos(talon_temp->s) == fcs_klondike_talon_len(talon_temp->s))
            {
                if (num_redeals_left > 0)
                {
                    fcs_klondike_talon_len(talon_temp->s) = fcs_klondike_talon_stack_pos(talon_temp->s);
                    fcs_klondike_talon_redeal_bare(talon_temp->s);

                    num_redeals_left--;
                    num_redeals_done++;
                }
                else
                {
                    break;
                }
            }
            fcs_klondike_talon_queue_to_stack(talon_temp->s);
            num_cards_moved[num_redeals_done]++;
        }
        first_iter = 0;

        card_to_check = fcs_klondike_talon_get_top_card(talon_temp->s);
        for(s=0 ; s<state_stacks_num ; s++)
        {
            cards_num = fcs_stack_len(state,s);
            top_card = fcs_stack_card(state,s,cards_num-1);
            if (fcs_is_parent_card(card_to_check, top_card))
            {
                /* We have a card in the talon that we can move
                to the stack, so let's move it */
                sfs_check_state_begin()

                new_state.talon = malloc(fcs_klondike_talon_len(talon_temp->s)+1);
                memcpy(
                    new_state.talon,
                    talon_temp->s.talon,
                    fcs_klondike_talon_len(talon_temp->s)+1
                    );

                memcpy(
                    ptr_new_state_with_locations->s.talon_params,
                    talon_temp->s.talon_params,
                    sizeof(ptr_state_with_locations->s.talon_params)
                );

                for(a=0;a<=num_redeals_done;a++)
                {
                    fcs_move_set_type(temp_move, FCS_MOVE_TYPE_KLONDIKE_FLIP_TALON);
                    fcs_move_set_num_cards_flipped(temp_move, num_cards_moved[a]);
                    fcs_move_stack_push(moves, temp_move);
                    if (a != num_redeals_done)
                    {
                        fcs_move_set_type(temp_move, FCS_MOVE_TYPE_KLONDIKE_REDEAL_TALON);
                        fcs_move_stack_push(moves,temp_move);
                    }
                }
                fcs_push_card_into_stack(new_state, s, fcs_klondike_talon_get_top_card(new_state));
                fcs_move_set_type(temp_move, FCS_MOVE_TYPE_KLONDIKE_TALON_TO_STACK);
                fcs_move_set_dest_stack(temp_move, s);
                fcs_klondike_talon_decrement_stack(new_state);

                sfs_check_state_end()
            }
        }
    }



#if 0
 cleanup:
#endif
    free(talon_temp->s.talon);
    free(talon_temp);

    return FCS_STATE_IS_NOT_SOLVEABLE;

}


#endif

#undef state_with_locations
#undef state
#undef new_state_with_locations
#undef new_state


freecell_solver_solve_for_state_test_t freecell_solver_sfs_tests[FCS_TESTS_NUM] =
{
    freecell_solver_sfs_move_top_stack_cards_to_founds,
    freecell_solver_sfs_move_freecell_cards_to_founds,
    freecell_solver_sfs_move_freecell_cards_on_top_of_stacks,
    freecell_solver_sfs_move_non_top_stack_cards_to_founds,
    freecell_solver_sfs_move_stack_cards_to_different_stacks,
    freecell_solver_sfs_move_stack_cards_to_a_parent_on_the_same_stack,
    freecell_solver_sfs_move_sequences_to_free_stacks,
    freecell_solver_sfs_move_freecell_cards_to_empty_stack,
    freecell_solver_sfs_move_cards_to_a_different_parent,
    freecell_solver_sfs_empty_stack_into_freecells,
    freecell_solver_sfs_simple_simon_move_sequence_to_founds,
    freecell_solver_sfs_simple_simon_move_sequence_to_true_parent,
    freecell_solver_sfs_simple_simon_move_whole_stack_sequence_to_false_parent,
    freecell_solver_sfs_simple_simon_move_sequence_to_true_parent_with_some_cards_above,
    freecell_solver_sfs_simple_simon_move_sequence_with_some_cards_above_to_true_parent,
    freecell_solver_sfs_simple_simon_move_sequence_with_junk_seq_above_to_true_parent_with_some_cards_above,
    freecell_solver_sfs_simple_simon_move_whole_stack_sequence_to_false_parent_with_some_cards_above,
    freecell_solver_sfs_simple_simon_move_sequence_to_parent_on_the_same_stack,
    freecell_solver_sfs_move_top_stack_cards_to_founds,
    freecell_solver_sfs_yukon_move_card_to_parent,
    freecell_solver_sfs_yukon_move_kings_to_empty_stack,
    freecell_solver_sfs_yukon_do_nothing,
    freecell_solver_sfs_yukon_do_nothing,
    freecell_solver_sfs_yukon_do_nothing,
    freecell_solver_sfs_yukon_do_nothing
#ifdef FCS_WITH_TALONS
        ,
    freecell_solver_sfs_deal_gypsy_talon,
    freecell_solver_sfs_get_card_from_klondike_talon
#endif
};
