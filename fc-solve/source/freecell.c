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
 * freecell.c - The various movement tests performed by Freecell Solver
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>


#include "config.h"

#include "state.h"
#include "card.h"
#include "fcs_dm.h"
#include "fcs.h"

#include "fcs_isa.h"
#include "tests.h"
#include "ms_ca.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

/*
 * Throughout this code the following local variables are used to quickly
 * access the instance's members:
 *
 * LOCAL_STACKS_NUM - the number of stacks in the state
 * LOCAL_FREECELLS_NUM - the number of freecells in the state
 * sequences_are_built_by - the type of sequences of this board.
 * */

int fc_solve_sfs_check_state_begin(
    fc_solve_hard_thread_t * hard_thread,
    fcs_state_t * * out_ptr_new_state_key,
    fcs_state_extra_info_t * * out_ptr_new_state_val,
    fcs_state_extra_info_t * ptr_state_val,
    fcs_move_stack_t * moves
    )
{
    fcs_state_extra_info_t * ptr_new_state_val;

    fcs_state_ia_alloc_into_var(ptr_new_state_val, hard_thread);
    *(out_ptr_new_state_key) = ptr_new_state_val->key;
    fcs_duplicate_state(
            (*(out_ptr_new_state_key)),
            ptr_new_state_val,
            ptr_state_val->key, 
            ptr_state_val
    );
    /* Some A* and BFS parameters that need to be initialized in
     * the derived state.
     * */
    ptr_new_state_val->parent_val = ptr_state_val;
    ptr_new_state_val->moves_to_parent = moves;
    /* Make sure depth is consistent with the game graph.
     * I.e: the depth of every newly discovered state is derived from
     * the state from which it was discovered. */
    ptr_new_state_val->depth = ptr_new_state_val->depth + 1;
    /* Mark this state as a state that was not yet visited */
    ptr_new_state_val->visited = 0;
    /* It's a newly created state which does not have children yet. */
    ptr_new_state_val->num_active_children = 0;
    memset(ptr_new_state_val->scan_visited, '\0',
        sizeof(ptr_new_state_val->scan_visited)
        );
    fcs_move_stack_reset(moves);

    *out_ptr_new_state_val = ptr_new_state_val;

    return 0;
}

int fc_solve_sfs_check_state_end(
    fc_solve_soft_thread_t * soft_thread,
    fcs_state_extra_info_t * ptr_state_val,
    fcs_state_extra_info_t * ptr_new_state_val,
    int state_context_value,
    fcs_move_stack_t * moves,
    fcs_derived_states_list_t * derived_states_list
    )
{
    fcs_move_t temp_move;
    fc_solve_hard_thread_t * hard_thread;
    fc_solve_instance_t * instance;
    int check;
    int calc_real_depth;
    int scans_synergy;

    temp_move = fc_solve_empty_move;

    hard_thread = soft_thread->hard_thread;
    instance = hard_thread->instance;
    calc_real_depth = instance->calc_real_depth;
    scans_synergy = instance->scans_synergy;

    /* The last move in a move stack should be FCS_MOVE_TYPE_CANONIZE
     * because it indicates that the order of the stacks and freecells
     * need to be recalculated
     * */
    fcs_move_set_type(temp_move,FCS_MOVE_TYPE_CANONIZE);
    fcs_move_stack_push(moves, temp_move);

    {
        fcs_state_extra_info_t * existing_state_val;
        check = fc_solve_check_and_add_state(
            soft_thread,
            ptr_new_state_val,
            &existing_state_val
            );
        if ((check == FCS_STATE_BEGIN_SUSPEND_PROCESS) ||
            (check == FCS_STATE_SUSPEND_PROCESS))
        {
            /* This state is not going to be used, so
             * let's clean it. */
            fcs_state_ia_release(hard_thread);
        }
        else if (check == FCS_STATE_ALREADY_EXISTS)
        {
            fcs_state_ia_release(hard_thread);
            calculate_real_depth(existing_state_val);
            /* Re-parent the existing state to this one.
             *
             * What it means is that if the depth of the state if it
             * can be reached from this one is lower than what it
             * already have, then re-assign its parent to this state.
             * */
            if (instance->to_reparent_states_real &&
               (existing_state_val->depth > ptr_state_val->depth+1))   \
            {
                /* Make a copy of "moves" because "moves" will be destroyed */\
                existing_state_val->moves_to_parent =
                    fc_solve_move_stack_compact_allocate(
                        hard_thread, moves
                        );
                if (!(existing_state_val->visited & FCS_VISITED_DEAD_END))
                {
                    if ((--existing_state_val->parent_val->num_active_children) == 0)
                    {
                        mark_as_dead_end(
                            existing_state_val->parent_val
                            );
                    }
                    ptr_state_val->num_active_children++;
                }
                existing_state_val->parent_val = ptr_state_val;
                existing_state_val->depth = ptr_state_val->depth + 1;
            }
            fc_solve_derived_states_list_add_state(
                derived_states_list,
                existing_state_val,
                state_context_value
                );
        }
        else
        {
            fc_solve_derived_states_list_add_state(
                derived_states_list,
                ptr_new_state_val,
                state_context_value
                );
        }
    }

    return check;
}


/*
 * This function tries to move stack cards that are present at the
 * top of stacks to the foundations.
 * */
int fc_solve_sfs_move_top_stack_cards_to_founds(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();
    int stack;
    int cards_num;
    int deck;
    fcs_card_t card;
    fcs_card_t temp_card;
    int check;
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif

    fcs_move_t temp_move;

    tests_define_accessors();

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif

    temp_move = fc_solve_empty_move;

    for( stack=0 ; stack < LOCAL_STACKS_NUM ; stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        if (cards_num)
        {
            /* Get the top card in the stack */
            card = fcs_stack_card(state,stack,cards_num-1);
            for(deck=0;deck < INSTANCE_DECKS_NUM;deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_card_num(card) - 1)
                {
                    /* We can put it there */

                    sfs_check_state_begin();

                    my_copy_stack(stack);
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
int fc_solve_sfs_move_freecell_cards_to_founds(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();
    int fc;
    int deck;
    fcs_card_t card;
    int check;
    fcs_move_t temp_move;
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif

    tests_define_accessors();

#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif

    temp_move = fc_solve_empty_move;

    /* Now check the same for the free cells */
    for( fc=0 ; fc < LOCAL_FREECELLS_NUM ; fc++)
    {
        card = fcs_freecell_card(state, fc);
        if (fcs_card_card_num(card) != 0)
        {
            for(deck=0;deck<INSTANCE_DECKS_NUM;deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_card_num(card) - 1)
                {
                    /* We can put it there */
                    sfs_check_state_begin()

                    fcs_empty_freecell(new_state, fc);

                    fcs_increment_foundation(new_state, deck*4+fcs_card_suit(card));

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

int fc_solve_sfs_move_freecell_cards_on_top_of_stacks(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();

    int dest_cards_num;
    int ds, fc, dc;
    fcs_card_t dest_card, src_card, temp_card, dest_below_card;
    int check;

    fcs_move_t temp_move;
    int is_seq_in_dest;
    int num_cards_to_relocate;
    int freecells_to_fill, freestacks_to_fill;
    int a,b;
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;

    tests_define_accessors();

#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif

    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    /* Let's try to put cards in the freecells on top of stacks */

    /* ds stands for destination stack */
    for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
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
                for(fc=0;fc<LOCAL_FREECELLS_NUM;fc++)
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

                            freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                            num_cards_to_relocate -= freecells_to_fill;

                            if (tests__is_filled_by_any_card())
                            {
                                freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

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


                                /* Fill the freecells with the top cards */

                                my_copy_stack(ds);

                                for(a=0 ; a<freecells_to_fill ; a++)
                                {
                                    /* Find a vacant freecell */
                                    for(b=0;b<LOCAL_FREECELLS_NUM;b++)
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
                                    for(b=0;b<LOCAL_STACKS_NUM;b++)
                                    {
                                        if (fcs_stack_len(new_state, b) == 0)
                                        {
                                            break;
                                        }
                                    }
                                    my_copy_stack(b);

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



int fc_solve_sfs_move_non_top_stack_cards_to_founds(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();

    int check;

    int stack;
    int cards_num;
    int c, a, b;
    fcs_card_t temp_card, card;
    int deck;
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;

    fcs_move_t temp_move;

    tests_define_accessors();

#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    /* Now let's check if a card that is under some other cards can be placed
     * in the foundations. */

    for(stack=0;stack<LOCAL_STACKS_NUM;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        /*
         * We starts from cards_num-2 because the top card is already covered
         * by move_top_stack_cards_to_founds.
         * */
        for(c=cards_num-2 ; c >= 0 ; c--)
        {
            card = fcs_stack_card(state, stack, c);
            for(deck=0;deck<INSTANCE_DECKS_NUM;deck++)
            {
                if (fcs_foundation_value(state, deck*4+fcs_card_suit(card)) == fcs_card_card_num(card)-1)
                {
                    /* The card is foundation-able. Now let's check if we
                     * can move the cards above it to the freecells and
                     * stacks */

                    if ((num_vacant_freecells +
                        ((tests__is_filled_by_any_card()) ?
                            num_vacant_stacks :
                            0
                        ))
                            >= cards_num-(c+1))
                    {
                        /* We can move it */

                        sfs_check_state_begin()

                        my_copy_stack(stack);


                        /* Fill the freecells with the top cards */
                        for(a=0 ; a<min(num_vacant_freecells, cards_num-(c+1)) ; a++)
                        {
                            /* Find a vacant freecell */
                            for(b=0; b<LOCAL_FREECELLS_NUM; b++)
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
                        for(a=0; a < cards_num-(c+1) - min(num_vacant_freecells, cards_num-(c+1)) ; a++)
                        {
                            /* Find a vacant stack */
                            for(b=0;b<LOCAL_STACKS_NUM;b++)
                            {
                                if (fcs_stack_len(new_state, b) == 0)
                                {
                                    break;
                                }
                            }

                            my_copy_stack(b);

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




int fc_solve_sfs_move_stack_cards_to_a_parent_on_the_same_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();
    int check;

    int stack, c, cards_num, a, dc,b;
    int is_seq_in_dest;
    fcs_card_t card, temp_card, prev_card;
    fcs_card_t dest_below_card, dest_card;
    int freecells_to_fill, freestacks_to_fill;
    int dest_cards_num, num_cards_to_relocate;
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;

    fcs_move_t temp_move;

    tests_define_accessors();

#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    /*
     * Now let's try to move a stack card to a parent card which is found
     * on the same stack.
     * */
    for (stack=0;stack<LOCAL_STACKS_NUM;stack++)
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

                            freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                            num_cards_to_relocate -= freecells_to_fill;

                            if (tests__is_filled_by_any_card())
                            {
                                freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

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


                                {
                                    int i_card_pos;
                                    fcs_card_t moved_card;
                                    int source_type, source_index;

                                    i_card_pos = fcs_stack_len(new_state,stack)-1;
                                    a = 0;

                                    my_copy_stack(ds);
                                    while(i_card_pos>c)
                                    {
                                        if (a < freecells_to_fill)
                                        {
                                            for(b=0;b<LOCAL_FREECELLS_NUM;b++)
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
                                            for(b=0;b<LOCAL_STACKS_NUM;b++)
                                            {
                                                if (fcs_stack_len(new_state, b) == 0)
                                                {
                                                    break;
                                                }
                                            }

                                            my_copy_stack(b);

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
                                        for(b=0;b<LOCAL_FREECELLS_NUM;b++)
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
                                        for(b=0;b<LOCAL_STACKS_NUM;b++)
                                        {
                                            if (fcs_stack_len(new_state, b) == 0)
                                            {
                                                break;
                                            }
                                        }

                                        my_copy_stack(b);
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
                                            for(b=0;b<LOCAL_FREECELLS_NUM;b++)
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
                                            for(b=0;b<LOCAL_STACKS_NUM;b++)
                                            {
                                                if (fcs_stack_len(new_state, b) == 0)
                                                {
                                                    break;
                                                }
                                            }

                                            fcs_pop_stack_card(new_state, ds, temp_card);
                                            my_copy_stack(b);
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
                                        my_copy_stack(source_index);
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


int fc_solve_sfs_move_stack_cards_to_different_stacks(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();

    int check;

    int stack, c, cards_num, a, dc, ds,b;
    fcs_card_t card, temp_card, this_card, prev_card;
    fcs_card_t dest_card;
    int freecells_to_fill, freestacks_to_fill;
    int dest_cards_num, num_cards_to_relocate;
    int seq_end;
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    char * positions_by_rank;
    char * pos_idx_to_check;
#ifndef HARD_CODED_NUM_DECKS
    int decks_num;
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;

    fcs_move_t temp_move;

    tests_define_accessors();

#ifndef HARD_CODED_NUM_DECKS
    decks_num = INSTANCE_DECKS_NUM;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    temp_move = fc_solve_empty_move;

    positions_by_rank = 
        fc_solve_get_the_positions_by_rank_data(
            soft_thread,
            ptr_state_val
        );

    /* Now let's try to move a card from one stack to the other     *
     * Note that it does not involve moving cards lower than king   *
     * to empty stacks                                              */

    for (stack=0;stack<LOCAL_STACKS_NUM;stack++)
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
            if (fcs_card_get_flipped(card))
            {
                continue;
            }

            /* Skip if it's a King - nothing to put it on. */
            if (fcs_card_card_num(card) == 13)
            {
                continue;
            }

            for(pos_idx_to_check = &positions_by_rank[
                (((LOCAL_DECKS_NUM<< 2)+1) << 1) * (fcs_card_card_num(card))
                ]
                ;
                ((*pos_idx_to_check) >= 0)
                ;
               )
            {
                ds = *(pos_idx_to_check++);
                dc = *(pos_idx_to_check++);

                if (ds == stack)
                {
                    continue;
                }
                dest_cards_num = fcs_stack_len(state, ds);
                dest_card = fcs_stack_card(state, ds, dc);

                if (! fcs_is_parent_card(card, dest_card))
                {
                    continue;
                }

                num_cards_to_relocate = dest_cards_num - dc - 1 + cards_num - seq_end - 1;

                freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                num_cards_to_relocate -= freecells_to_fill;

                if (tests__is_filled_by_any_card())
                {
                    freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

                    num_cards_to_relocate -= freestacks_to_fill;
                }
                else
                {
                    freestacks_to_fill = 0;
                }

                if ((num_cards_to_relocate == 0) &&
                   (calc_max_sequence_move(num_vacant_freecells-freecells_to_fill, num_vacant_stacks-freestacks_to_fill) >=
                    seq_end - c + 1))
                {
                    /* We can move it */
                    int from_which_stack;

                    sfs_check_state_begin()


                    /* Fill the freecells with the top cards */

                    my_copy_stack(stack);
                    my_copy_stack(ds);

                    for(a=0 ; a<freecells_to_fill ; a++)
                    {
                        /* Find a vacant freecell */
                        for(b=0;b<LOCAL_FREECELLS_NUM;b++)
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
                        my_copy_stack(from_which_stack);

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
                        for(b=0;b<LOCAL_STACKS_NUM;b++)
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

                        my_copy_stack(b);
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

    return FCS_STATE_IS_NOT_SOLVEABLE;
}



int fc_solve_sfs_move_sequences_to_free_stacks(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();
    int check;

    int stack, cards_num, c, ds, a, b, seq_end;
    fcs_card_t this_card, prev_card, temp_card;
    int max_sequence_len;
    int num_cards_to_relocate, freecells_to_fill, freestacks_to_fill;
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;

    fcs_move_t temp_move;

    tests_define_accessors();

    if (tests__is_filled_by_none())
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    temp_move = fc_solve_empty_move;

#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    max_sequence_len = calc_max_sequence_move(num_vacant_freecells, num_vacant_stacks-1);

    /* Now try to move sequences to empty stacks */

    if (num_vacant_stacks > 0)
    {
        for(stack=0;stack<LOCAL_STACKS_NUM;stack++)
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
                    (tests__is_filled_by_kings_only()))
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
                        ((tests__is_filled_by_kings_only()) ?
                            (fcs_card_card_num(fcs_stack_card(state, stack, c)) == 13) :
                            1
                        )
                       )
                    {
                        sfs_check_state_begin();


                        for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
                        {
                            if (fcs_stack_len(state, ds) == 0)
                                break;
                        }

                        my_copy_stack(ds);

                        for(a=c ; a <= cards_num-1 ; a++)
                        {
                            fcs_push_stack_card_into_stack(new_state, ds, stack, a);
                        }

                        my_copy_stack(stack);

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

                    freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                    num_cards_to_relocate -= freecells_to_fill;

                    if (tests__is_filled_by_any_card())
                    {
                        freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

                        num_cards_to_relocate -= freestacks_to_fill;
                    }
                    else
                    {
                        freestacks_to_fill = 0;
                    }

                    if ((num_cards_to_relocate == 0) && (num_vacant_stacks-freestacks_to_fill > 0))
                    {
                        /* We can move it */
                        int seq_start = c;
                        while (
                            (calc_max_sequence_move(
                                num_vacant_freecells-freecells_to_fill,
                                num_vacant_stacks-freestacks_to_fill-1) < seq_end-seq_start+1)
                                &&
                            (seq_start <= seq_end)
                            )
                        {
                            seq_start++;
                        }
                        if ((seq_start <= seq_end) &&
                            ((tests__is_filled_by_kings_only()) ?
                                (fcs_card_card_num(fcs_stack_card(state, stack, seq_start)) == 13) :
                                1
                            )
                        )
                        {
                            sfs_check_state_begin();


                            /* Fill the freecells with the top cards */

                            my_copy_stack(stack);

                            for(a=0; a<freecells_to_fill ; a++)
                            {
                                /* Find a vacant freecell */
                                for(b=0;b<LOCAL_FREECELLS_NUM;b++)
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

                            my_copy_stack(stack);

                            /* Fill the free stacks with the cards below them */
                            for(a=0; a < freestacks_to_fill ; a++)
                            {
                                /* Find a vacant stack */
                                for(b=0; b<LOCAL_STACKS_NUM; b++)
                                {
                                    if (fcs_stack_len(new_state, b) == 0)
                                    {
                                        break;
                                    }
                                }

                                my_copy_stack(b);
                                fcs_pop_stack_card(new_state, stack, temp_card);
                                fcs_push_card_into_stack(new_state, b, temp_card);
                                fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                fcs_move_set_src_stack(temp_move,stack);
                                fcs_move_set_dest_stack(temp_move,b);
                                fcs_move_set_num_cards_in_seq(temp_move,1);
                                fcs_move_stack_push(moves, temp_move);
                            }

                            /* Find a vacant stack */
                            for(b=0; b<LOCAL_STACKS_NUM; b++)
                            {
                                if (fcs_stack_len(new_state, b) == 0)
                                {
                                    break;
                                }
                            }

                            my_copy_stack(b);

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



int fc_solve_sfs_move_freecell_cards_to_empty_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();
    int check;
    int fc, stack;
    fcs_card_t card;

    fcs_move_t temp_move;

#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif

    /* Let's try to put cards that occupy freecells on an empty stack */

    tests_define_accessors();

    if (tests__is_filled_by_none())
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
    
    temp_move = fc_solve_empty_move;

    for(fc=0;fc<LOCAL_FREECELLS_NUM;fc++)
    {
        card = fcs_freecell_card(state, fc);
        if (
            (tests__is_filled_by_kings_only()) ?
                (fcs_card_card_num(card) == 13) :
                (fcs_card_card_num(card) != 0)
           )
        {
            for(stack=0;stack<LOCAL_STACKS_NUM;stack++)
            {
                if (fcs_stack_len(state, stack) == 0)
                {
                    break;
                }
            }
            if (stack != LOCAL_STACKS_NUM)
            {
                /* We can move it */

                sfs_check_state_begin();

                my_copy_stack(stack);

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

int fc_solve_sfs_move_cards_to_a_different_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();

    int check;

    int stack, cards_num, c, a, b, ds, dc;
    int is_seq_in_src, is_seq_in_dest;
    int num_cards_to_relocate;
    int dest_cards_num;
    fcs_card_t card, this_card, prev_card, temp_card;
    fcs_card_t dest_card, dest_below_card;
    int freecells_to_fill, freestacks_to_fill;

    fcs_move_t temp_move;

#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;

    tests_define_accessors();

    temp_move = fc_solve_empty_move;

#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
    num_vacant_freecells = soft_thread->num_vacant_freecells;
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    /* This time try to move cards that are already on top of a parent to a different parent */

    for (stack=0;stack<LOCAL_STACKS_NUM;stack++)
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
                for(ds=0 ; ds<LOCAL_STACKS_NUM; ds++)
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

                                        freecells_to_fill = min(num_cards_to_relocate, num_vacant_freecells);

                                        num_cards_to_relocate -= freecells_to_fill;

                                        if (tests__is_filled_by_any_card())
                                        {
                                            freestacks_to_fill = min(num_cards_to_relocate, num_vacant_stacks);

                                            num_cards_to_relocate -= freestacks_to_fill;
                                        }
                                        else
                                        {
                                            freestacks_to_fill = 0;
                                        }

                                        if ((num_cards_to_relocate == 0) &&
                                           (calc_max_sequence_move(num_vacant_freecells-freecells_to_fill, num_vacant_stacks-freestacks_to_fill) >=
                                            cards_num - c))
                                        {
                                            /* We can move it */

                                            sfs_check_state_begin()


                                            /* Fill the freecells with the top cards */

                                            my_copy_stack(ds);
                                            for(a=0 ; a<freecells_to_fill ; a++)
                                            {
                                                /* Find a vacant freecell */
                                                for(b=0;b<LOCAL_FREECELLS_NUM;b++)
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
                                                for(b=0;b<LOCAL_STACKS_NUM;b++)
                                                {
                                                    if (fcs_stack_len(new_state, b) == 0)
                                                    {
                                                        break;
                                                    }
                                                }

                                                my_copy_stack(b);

                                                fcs_pop_stack_card(new_state, ds, temp_card);
                                                fcs_push_card_into_stack(new_state, b, temp_card);

                                                fcs_move_set_type(temp_move,FCS_MOVE_TYPE_STACK_TO_STACK);
                                                fcs_move_set_src_stack(temp_move,ds);
                                                fcs_move_set_dest_stack(temp_move,b);
                                                fcs_move_set_num_cards_in_seq(temp_move,1);
                                                fcs_move_stack_push(moves, temp_move);
                                            }

                                            my_copy_stack(stack);

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




int fc_solve_sfs_empty_stack_into_freecells(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();

    int check;

    int stack, cards_num, c, b;
    fcs_card_t temp_card;
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
    int num_vacant_freecells;
    int num_vacant_stacks;

    fcs_move_t temp_move;

    tests_define_accessors();

    if (tests__is_filled_by_none())
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    temp_move = fc_solve_empty_move;

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif
    num_vacant_stacks = soft_thread->num_vacant_stacks;
    num_vacant_freecells = soft_thread->num_vacant_freecells;


    /* Now, let's try to empty an entire stack into the freecells, so other cards can
     * inhabit it */

    if (num_vacant_stacks == 0)
    {
        for(stack=0;stack<LOCAL_STACKS_NUM;stack++)
        {
            cards_num = fcs_stack_len(state, stack);
            if (cards_num <= num_vacant_freecells)
            {
                /* We can empty it */

                sfs_check_state_begin()

                my_copy_stack(stack);

                for(c=0;c<cards_num;c++)
                {
                    /* Find a vacant freecell */
                    for(b=0; b<LOCAL_FREECELLS_NUM; b++)
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

int fc_solve_sfs_yukon_do_nothing(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_yukon_move_card_to_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();

    int check;

    int stack, cards_num, c, a, ds;
    int dest_cards_num;
    fcs_card_t card, temp_card;
    fcs_card_t dest_card;

#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif

    fcs_move_t temp_move;

    tests_define_accessors();

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif

    for( ds=0 ; ds < LOCAL_STACKS_NUM ; ds++ )
    {
        dest_cards_num = fcs_stack_len(state, ds);
        if (dest_cards_num > 0)
        {
            dest_card = fcs_stack_card(state, ds, dest_cards_num-1);
            for( stack=0 ; stack < LOCAL_STACKS_NUM ; stack++)
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

                            my_copy_stack(stack);
                            my_copy_stack(ds);

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

int fc_solve_sfs_yukon_move_kings_to_empty_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();

    int check;

    int stack, cards_num, c, a, ds;
    fcs_card_t card, temp_card;

#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    int num_vacant_stacks;

    fcs_move_t temp_move;

    tests_define_accessors();

    num_vacant_stacks = soft_thread->num_vacant_stacks;

    if (num_vacant_stacks == 0)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif

    for( stack=0 ; stack < LOCAL_STACKS_NUM ; stack++)
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


                for( ds=0 ; ds < LOCAL_STACKS_NUM ; ds++)
                {
                    if (fcs_stack_len(state, ds) == 0)
                    {
                        break;
                    }
                }
                my_copy_stack(stack);
                my_copy_stack(ds);
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
int fc_solve_sfs_deal_gypsy_talon(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();

    int check;

    fcs_card_t temp_card;
    int a;

    fcs_move_t temp_move;

    tests_define_accessors();

    if (instance->talon_type != FCS_TALON_GYPSY)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    if (fcs_talon_pos(state) < fcs_talon_len(state))
    {
        sfs_check_state_begin()
        for(a=0;a<LOCAL_STACKS_NUM;a++)
        {
            temp_card = fcs_get_talon_card(new_state, fcs_talon_pos(new_state)+a);
            fcs_push_card_into_stack(new_state,a,temp_card);
        }
        fcs_talon_pos(new_state) += LOCAL_STACKS_NUM;
        fcs_move_set_type(temp_move, FCS_MOVE_TYPE_DEAL_GYPSY_TALON);
        fcs_move_stack_push(moves, temp_move);

        sfs_check_state_end()
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


int fc_solve_sfs_get_card_from_klondike_talon(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();


    fcs_state_with_locations_t * talon_temp;

    fcs_move_t temp_move;

    int check;
    int num_redeals_left, num_redeals_done, num_cards_moved[2];
    int first_iter;
    fcs_card_t card_to_check, top_card;
    int s;
    int cards_num;
    int a;

    tests_define_accessors();

    if (instance->talon_type != FCS_TALON_KLONDIKE)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

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
        for(s=0 ; s<LOCAL_STACKS_NUM ; s++)
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

int fc_solve_sfs_atomic_move_card_to_empty_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    int stack, cards_num;
    fcs_card_t card, temp_card;
    fcs_move_t temp_move;
    int check;
    int empty_stack_idx;
    int num_vacant_stacks;

    tests_define_accessors();

    num_vacant_stacks = soft_thread->num_vacant_stacks;

    if (num_vacant_stacks == 0)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif

    for(empty_stack_idx=0;empty_stack_idx<LOCAL_STACKS_NUM;empty_stack_idx++)
    {
        if (fcs_stack_len(state, empty_stack_idx) == 0)
        {
            break;
        }
    }

    if (tests__is_filled_by_none())
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }



    for(stack=0;stack<LOCAL_STACKS_NUM;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        if (cards_num > 0)
        {
            card = fcs_stack_card(state, stack, cards_num-1);
            if (tests__is_filled_by_kings_only() &&
                (fcs_card_card_num(card) != 13))
            {
                continue;
            }
            /* Let's move it */
            {
                sfs_check_state_begin();

                my_copy_stack(stack);

                fcs_pop_stack_card(new_state, stack, temp_card);


                my_copy_stack(empty_stack_idx);

                fcs_push_card_into_stack(new_state, empty_stack_idx, card);

                fcs_move_set_type(temp_move, FCS_MOVE_TYPE_STACK_TO_STACK);
                fcs_move_set_src_stack(temp_move, stack);
                fcs_move_set_dest_stack(temp_move, empty_stack_idx);
                fcs_move_set_num_cards_in_seq(temp_move, 1);

                fcs_move_stack_push(moves, temp_move);

                fcs_move_set_type(temp_move,FCS_MOVE_TYPE_CANONIZE);
                fcs_move_stack_push(moves, temp_move);

                sfs_check_state_end()
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_atomic_move_card_to_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    int stack, cards_num, ds, ds_cards_num;
    fcs_card_t card, dest_card, temp_card;
    fcs_move_t temp_move;
    int check;

    tests_define_accessors();

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif

    for(stack=0;stack<LOCAL_STACKS_NUM;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        if (cards_num > 0)
        {
            card = fcs_stack_card(state, stack, cards_num-1);

            for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
            {
                if (ds == stack)
                {
                    continue;
                }

                ds_cards_num = fcs_stack_len(state, ds);
                if (ds_cards_num > 0)
                {
                    dest_card = fcs_stack_card(state, ds, ds_cards_num-1);
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* Let's move it */
                        {
                            sfs_check_state_begin();

                            my_copy_stack(stack);
                            my_copy_stack(ds);

                            fcs_pop_stack_card(new_state, stack, temp_card);

                            fcs_push_card_into_stack(new_state, ds, card);

                            fcs_move_set_type(temp_move, FCS_MOVE_TYPE_STACK_TO_STACK);
                            fcs_move_set_src_stack(temp_move, stack);
                            fcs_move_set_dest_stack(temp_move, ds);
                            fcs_move_set_num_cards_in_seq(temp_move, 1);

                            fcs_move_stack_push(moves, temp_move);

                            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_CANONIZE);
                            fcs_move_stack_push(moves, temp_move);

                            sfs_check_state_end()
                        }
                    }
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_atomic_move_card_to_freecell(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
    int stack, cards_num, ds;
    fcs_card_t card, temp_card;
    fcs_move_t temp_move;
    int check;
    int num_vacant_freecells;

    tests_define_accessors();

    temp_move = fc_solve_empty_move;

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif
    num_vacant_freecells = soft_thread->num_vacant_freecells;

    if (num_vacant_freecells == 0)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    for(ds=0;ds<LOCAL_FREECELLS_NUM;ds++)
    {
        if (fcs_freecell_card_num(state, ds) == 0)
        {
            break;
        }
    }



    for(stack=0;stack<LOCAL_STACKS_NUM;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        if (cards_num > 0)
        {
            card = fcs_stack_card(state, stack, cards_num-1);

            /* Let's move it */
            {
                sfs_check_state_begin();

                my_copy_stack(stack);

                fcs_pop_stack_card(new_state, stack, temp_card);

                fcs_put_card_in_freecell(new_state, ds, card);

                fcs_move_set_type(temp_move, FCS_MOVE_TYPE_STACK_TO_FREECELL);
                fcs_move_set_src_stack(temp_move, stack);
                fcs_move_set_dest_freecell(temp_move, ds);

                fcs_move_stack_push(moves, temp_move);

                fcs_move_set_type(temp_move,FCS_MOVE_TYPE_CANONIZE);
                fcs_move_stack_push(moves, temp_move);

                sfs_check_state_end()
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_atomic_move_freecell_card_to_parent(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
    int fc, ds, ds_cards_num;
    fcs_card_t card, dest_card;
    fcs_move_t temp_move;
    int check;

    tests_define_accessors();

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif

    for(fc=0;fc<LOCAL_FREECELLS_NUM;fc++)
    {
        card = fcs_freecell_card(state, fc);
        if (fcs_card_card_num(card) == 0)
        {
            continue;
        }

        for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
        {
            ds_cards_num = fcs_stack_len(state, ds);
            if (ds_cards_num > 0)
            {
                dest_card = fcs_stack_card(state, ds, ds_cards_num-1);
                if (fcs_is_parent_card(card, dest_card))
                {
                    /* Let's move it */
                    {
                        sfs_check_state_begin();

                        my_copy_stack(ds);

                        fcs_empty_freecell(new_state, fc);

                        fcs_push_card_into_stack(new_state, ds, card);

                        fcs_move_set_type(temp_move, FCS_MOVE_TYPE_FREECELL_TO_STACK);
                        fcs_move_set_src_freecell(temp_move, fc);
                        fcs_move_set_dest_stack(temp_move, ds);
                        fcs_move_set_num_cards_in_seq(temp_move, 1);

                        fcs_move_stack_push(moves, temp_move);

                        fcs_move_set_type(temp_move,FCS_MOVE_TYPE_CANONIZE);
                        fcs_move_stack_push(moves, temp_move);

                        sfs_check_state_end()
                    }
                }
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int fc_solve_sfs_atomic_move_freecell_card_to_empty_stack(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val,
        fcs_derived_states_list_t * derived_states_list
        )
{
    tests_declare_accessors();
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
    int fc, ds;
    fcs_card_t card;
    fcs_move_t temp_move;
    int check;
    int num_vacant_stacks;

    tests_define_accessors();

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif
    num_vacant_stacks = soft_thread->num_vacant_stacks;

    if (num_vacant_stacks == 0)
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    if (tests__is_filled_by_none())
    {
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }

    for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
    {
        if (fcs_stack_len(state, ds) == 0)
        {
            break;
        }
    }

    for(fc=0;fc<LOCAL_FREECELLS_NUM;fc++)
    {
        card = fcs_freecell_card(state, fc);
        if (fcs_card_card_num(card) == 0)
        {
            continue;
        }

        if (tests__is_filled_by_kings_only() &&
            (fcs_card_card_num(card) != 13))
        {
            continue;
        }

        {
            sfs_check_state_begin();

            my_copy_stack(ds);

            fcs_empty_freecell(new_state, fc);

            fcs_push_card_into_stack(new_state, ds, card);

            fcs_move_set_type(temp_move, FCS_MOVE_TYPE_FREECELL_TO_STACK);
            fcs_move_set_src_freecell(temp_move, fc);
            fcs_move_set_dest_stack(temp_move, ds);
            fcs_move_set_num_cards_in_seq(temp_move, 1);

            fcs_move_stack_push(moves, temp_move);

            fcs_move_set_type(temp_move,FCS_MOVE_TYPE_CANONIZE);
            fcs_move_stack_push(moves, temp_move);

            sfs_check_state_end()
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


#undef state_with_locations
#undef state
#undef new_state_with_locations
#undef new_state


fc_solve_solve_for_state_test_t fc_solve_sfs_tests[FCS_TESTS_NUM] =
{
    fc_solve_sfs_move_top_stack_cards_to_founds,
    fc_solve_sfs_move_freecell_cards_to_founds,
    fc_solve_sfs_move_freecell_cards_on_top_of_stacks,
    fc_solve_sfs_move_non_top_stack_cards_to_founds,
    fc_solve_sfs_move_stack_cards_to_different_stacks,
    fc_solve_sfs_move_stack_cards_to_a_parent_on_the_same_stack,
    fc_solve_sfs_move_sequences_to_free_stacks,
    fc_solve_sfs_move_freecell_cards_to_empty_stack,
    fc_solve_sfs_move_cards_to_a_different_parent,
    fc_solve_sfs_empty_stack_into_freecells,
    fc_solve_sfs_simple_simon_move_sequence_to_founds,
    fc_solve_sfs_simple_simon_move_sequence_to_true_parent,
    fc_solve_sfs_simple_simon_move_whole_stack_sequence_to_false_parent,
    fc_solve_sfs_simple_simon_move_sequence_to_true_parent_with_some_cards_above,
    fc_solve_sfs_simple_simon_move_sequence_with_some_cards_above_to_true_parent,
    fc_solve_sfs_simple_simon_move_sequence_with_junk_seq_above_to_true_parent_with_some_cards_above,
    fc_solve_sfs_simple_simon_move_whole_stack_sequence_to_false_parent_with_some_cards_above,
    fc_solve_sfs_simple_simon_move_sequence_to_parent_on_the_same_stack,
    fc_solve_sfs_atomic_move_card_to_empty_stack,
    fc_solve_sfs_atomic_move_card_to_parent,
    fc_solve_sfs_atomic_move_card_to_freecell,
    fc_solve_sfs_atomic_move_freecell_card_to_parent,
    fc_solve_sfs_atomic_move_freecell_card_to_empty_stack,
#if 0
    fc_solve_sfs_move_top_stack_cards_to_founds,
    fc_solve_sfs_yukon_move_card_to_parent,
    fc_solve_sfs_yukon_move_kings_to_empty_stack,
    fc_solve_sfs_yukon_do_nothing,
    fc_solve_sfs_yukon_do_nothing,
#endif
    fc_solve_sfs_yukon_do_nothing,
    fc_solve_sfs_yukon_do_nothing
#ifdef FCS_WITH_TALONS
        ,
    fc_solve_sfs_deal_gypsy_talon,
    fc_solve_sfs_get_card_from_klondike_talon
#endif
};
