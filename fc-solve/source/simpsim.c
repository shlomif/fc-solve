/*
 * simpsim.c - a module that contains Simple Simon Moves.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2001
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#include <stdio.h>

#include "fcs.h"

#include "tests.h"

#include "ms_ca.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif


#define fcs_is_ss_false_parent(parent, child) \
    (fcs_card_card_num(parent) == fcs_card_card_num(child)+1)

#define fcs_suit_is_ss_true_parent(parent, child)       \
    ((fcs_card_suit(parent)) == (fcs_card_suit(child)))

#define fcs_is_ss_true_parent(parent, child)            \
    (                                                   \
        fcs_is_ss_false_parent(parent,child) &&         \
        (fcs_suit_is_ss_true_parent(parent,child))      \
    )

/*
 *  Those are some macros to make it easier for the programmer.
 * */
#define state_with_locations (*ptr_state_with_locations)
#define state (ptr_state_with_locations->s)
#define new_state_with_locations (*ptr_new_state_with_locations)
#define new_state (ptr_new_state_with_locations->s)



int freecell_solver_sfs_simple_simon_move_sequence_to_founds(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    tests_declare_accessors();


    fcs_move_t temp_move;

    int check;

    fcs_card_t temp_card;

    /*
     * stack - the stack index from which to move cards to the founds.
     * a - the height of the card
     * */
    int stack, cards_num, a;
    /*
     * card - the current card (at height a)
     * above_card - the card above it.
     * */
    fcs_card_t card, above_card;

    int state_stacks_num;
    tests_define_accessors();

    state_stacks_num = instance->stacks_num;


    for(stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
		if (cards_num < 13)
		{
			continue;
		}

        card = fcs_stack_card(state,stack,cards_num-1);

        /* Check if the top 13 cards are a sequence */

        for(a=2;a<=13;a++)
        {
            above_card = fcs_stack_card(state,stack,cards_num-a);
            if (!fcs_is_ss_true_parent(above_card, card))
            {
				break;
            }

            card = above_card;
        }

		if (a <= 13)
		{
			continue;
		}

        /* We can move this sequence up there */

        sfs_check_state_begin();

        my_copy_stack(stack);

        for(a=0;a<13;a++)
        {
            fcs_pop_stack_card(new_state, stack, temp_card);
            fcs_increment_foundation(new_state, fcs_card_suit(card));
        }

        fcs_move_set_type(temp_move, FCS_MOVE_TYPE_SEQ_TO_FOUNDATION);
        fcs_move_set_src_stack(temp_move, stack);
        fcs_move_set_foundation(temp_move,fcs_card_suit(card));
        fcs_move_stack_push(moves,temp_move);

        sfs_check_state_end();

    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int freecell_solver_sfs_simple_simon_move_sequence_to_true_parent(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    tests_declare_accessors();


    fcs_move_t temp_move;

    int check;

    /*
     * stack - the source stack index on which the sequence currently resides.
     * cards_num - the number of cards in "stack".
     * a - a temporary variable that designates a card height
     * */
    int stack, cards_num, a;
    /*
     * h - the current height in stack
     * */
    int h;
    /*
     * card - the current card (at height h)
     * above_card - the card above it.
     * next_card - next card on the stack
     * */
    fcs_card_t card, temp_card, next_card;
    /*
     * num_true_seqs - the number of true sequences (i.e: sequences of a
     * unified suit) in the source sequence.
     * ds - the destination stack index.
     * dest_cards_num - the number of cards in "ds".
     * */
    int num_true_seqs, ds, dest_cards_num ;

    int state_stacks_num;
    tests_define_accessors();

    state_stacks_num = instance->stacks_num;

    for(stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
		if (cards_num <= 0)
		{
			continue;
		}

        /* Loop on the cards in the stack and try to look for a true
         * parent on top one of the stacks */
        card = fcs_stack_card(state,stack,cards_num-1);
        num_true_seqs = 1;

        for(h=cards_num-2;h>=-1;h--)
        {
            for(ds=0;ds<state_stacks_num;ds++)
            {
                if (ds == stack)
                {
                    continue;
                }

                dest_cards_num = fcs_stack_len(state,ds);

				if ((dest_cards_num > 0) && 
					(fcs_is_ss_true_parent(fcs_stack_card(state, ds, dest_cards_num-1), card)) &&
                    /* This is a suitable parent - let's check if we
                    * have enough empty stacks to make the move feasible */
					(calc_max_sequence_move(0, num_freestacks) >= num_true_seqs))
				{
                    /* We can do it - so let's move */

                    sfs_check_state_begin();

                    my_copy_stack(stack);
                    my_copy_stack(ds);


                    fcs_move_sequence(ds, stack, h+1, cards_num-1, a);
                    sfs_check_state_end();
                }
            }

            /* Stop if we reached the bottom of the stack */
            if (h == -1)
            {
                break;
            }

            next_card = fcs_stack_card(state,stack,h);
            /* If this is no longer a sequence - move to the next stack */
			if (!fcs_is_ss_false_parent(next_card, card))
			{
				break;
			}

			if (!fcs_suit_is_ss_true_parent(next_card, card))
            {
                num_true_seqs++;
            }

			card = next_card;
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}



int freecell_solver_sfs_simple_simon_move_whole_stack_sequence_to_false_parent(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    tests_declare_accessors();
    fcs_move_t temp_move;

    int check;

    /*
     * stack - the source stack index
     * cards_num - number of cards in "stack"
     * ds - the dest stack index
     * dest_cards_num - number of cards in "ds".
     * card - the current card
	 * next_card - the next card on the stack
     * h - the height of the current card on "stack"
     * num_true_seqs - the number of true sequences on the current
     *                 false sequence
     * */
    int stack, cards_num, a;
    fcs_card_t card, temp_card, next_card;
    int num_true_seqs, h, ds, dest_cards_num ;

    int state_stacks_num;
    tests_define_accessors();

    state_stacks_num = instance->stacks_num;

    for(stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
		if (cards_num <= 0)
		{
			continue;
		}

        card = fcs_stack_card(state,stack,cards_num-1);
        num_true_seqs = 1;

        /* Stop if we reached the bottom of the stack */
        for(h=cards_num-2;h>-1;h--)
        {
            next_card = fcs_stack_card(state,stack,h);
            /* If this is no longer a sequence - move to the next stack */

            if (!fcs_is_ss_false_parent(next_card, card))
            {
                break;
            }
            if (!fcs_suit_is_ss_true_parent(next_card, card))
            {
                num_true_seqs++;
            }

			card = next_card;
        }
        /* This means that the loop exited prematurely and the stack does
         * not contain a sequence. */
        if (h != -1)
        {
            continue;
        }

        for(ds=0;ds<state_stacks_num;ds++)
        {
			dest_cards_num = fcs_stack_len(state,ds);
            if (dest_cards_num <= 0)
			{
				continue;
			}

            if (!fcs_is_ss_false_parent(fcs_stack_card(state, ds, dest_cards_num-1), card))
			{
				continue;
			}

            /* This is a suitable parent - let's check if we
             * have enough empty stacks to make the move feasible */
            if (calc_max_sequence_move(0, num_freestacks) < num_true_seqs)
			{
				continue;
			}

			/* We can do it - so let's move */

            sfs_check_state_begin();

            my_copy_stack(stack);
            my_copy_stack(ds);

            fcs_move_sequence(ds, stack, h+1, cards_num-1, a);
            sfs_check_state_end();
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


int freecell_solver_sfs_simple_simon_move_sequence_to_true_parent_with_some_cards_above(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    tests_declare_accessors();


    fcs_move_t temp_move;
    int check;

    /*
     * stack - the source stack index
     * cards_num - the number of cards in "stack"
     * h - the height of the current card in "stack"
     * card - the card in height "h"
	 * next_card - the next card in the stack
     * ds - the destionation stack index
     * dest_cards_num - the number of cards in "ds"
     * dc - the index of the current card in "ds".
     * num_separate_false_seqs - this variable tells how many distinct false
     *      sequences exist above the true parent
     * above_num_true_seqs[] - the number of true sequences in each false
     *      sequence
     * seq_points[] - the separation points of the false sequences (i.e: where
     *      they begin and end)
     * stacks_map[] - a boolean map that indicates if one can place a card
     *      on this stack or is it already taken.
     * junk_move_to_stacks[] - the stacks to move each false sequence of the
     *      junk to.
     * false_seq_index - an iterator to hold the index of the current false
     *      sequence.
     * after_junk_num_freestacks - this variable holds the number of stacks
     *      that remained unoccupied during and after the process of moving
     *      the junk sequences to different stacks.
     *
     * */
    int stack, cards_num, a;
    fcs_card_t card, temp_card, next_card;
    int above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK], h, ds, dest_cards_num ;
    int dc;
    int seq_points[MAX_NUM_CARDS_IN_A_STACK];
    int num_separate_false_seqs;
    int false_seq_index;
    int num_true_seqs;
    int stacks_map[MAX_NUM_STACKS];
    int after_junk_num_freestacks;
    int junk_move_to_stacks[MAX_NUM_STACKS];

    /*
	 *  
     * above_c - the height of the card that is to be checked.
     * above_card - the card at height above_c+1
     * up_above_card - the card at height above_c
     *
     * */
    int above_c;
    fcs_card_t above_card, up_above_card;


    int state_stacks_num;
    tests_define_accessors();

    state_stacks_num = instance->stacks_num;

    for(stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        if (cards_num <= 0)
        {
			continue;
		}
        
		card = fcs_stack_card(state,stack,cards_num-1);
        num_true_seqs = 1;

        for(h=cards_num-2;h>=-1;h--)
        {
            for(ds=0;ds<state_stacks_num;ds++)
            {
                if (ds == stack)
                {
                    continue;
                }

                dest_cards_num = fcs_stack_len(state,ds);
                if (dest_cards_num <= 0)
                {
					continue;
				}

                for(dc=dest_cards_num-1;dc>=0;dc--)
                {
					if (!fcs_is_ss_true_parent(fcs_stack_card(state, ds, dc), card))
					{
						continue;
					}
				
					/* This is a suitable parent - let's check if there's a sequence above it. */
                    num_separate_false_seqs = 0;
                    above_card = fcs_stack_card(state, ds, dest_cards_num-1);
                    above_num_true_seqs[num_separate_false_seqs] = 1;
                    for(above_c = dest_cards_num-2; above_c > dc; above_c--)
                    {
                        up_above_card = fcs_stack_card(state, ds, above_c);
                        if (! fcs_is_ss_false_parent(up_above_card, above_card))
                        {
                            seq_points[num_separate_false_seqs++] = above_c+1;
                            above_num_true_seqs[num_separate_false_seqs] = 1;
                        }
                        above_num_true_seqs[num_separate_false_seqs] += ! (fcs_card_suit(up_above_card) == fcs_card_suit(above_card));
                        above_card = up_above_card;
                    }

                    if (dc < dest_cards_num - 1)
                    {
                        seq_points[num_separate_false_seqs++] = above_c+1;
                    }

                    for(a=0;a<state_stacks_num;a++)
                    {
                        stacks_map[a] = 0;
                    }
                    stacks_map[stack] = 1;
                    stacks_map[ds] = 1;

                    after_junk_num_freestacks = num_freestacks;

                    for(false_seq_index=0;false_seq_index<num_separate_false_seqs;false_seq_index++)
                    {
                        /* Find a suitable place to put it */

                        /*
                         * clear_junk_dest_stack is the stack to move this particular junk sequence to.
                         * */
                        int clear_junk_dest_stack;


                        /* Let's try to find a suitable parent on top one of the stacks */
                        for(clear_junk_dest_stack=0;
                            clear_junk_dest_stack < state_stacks_num;
                            clear_junk_dest_stack++
                           )
                        {
                            int clear_junk_stack_len = fcs_stack_len(state, clear_junk_dest_stack);

                            if ((clear_junk_stack_len > 0) && (stacks_map[clear_junk_dest_stack] == 0) &&
								(fcs_is_ss_false_parent(fcs_stack_card(state, clear_junk_dest_stack, clear_junk_stack_len-1), 
														fcs_stack_card(state, ds, seq_points[false_seq_index]))) &&
								(calc_max_sequence_move(0, after_junk_num_freestacks) >= above_num_true_seqs[false_seq_index]))

                            {
                                stacks_map[clear_junk_dest_stack] = 1;
                                break;
                            }
                        }

                        if (clear_junk_dest_stack == state_stacks_num)
                        {
                            clear_junk_dest_stack = -1;
                           /* Check if there is a vacant stack */
                            if (num_freestacks > 0)
                            {
                                if (calc_max_sequence_move(0, after_junk_num_freestacks-1) >= above_num_true_seqs[false_seq_index])
                                {
                                    /* Find an empty stack and designate it as the destination for the junk */
                                    for(
                                        clear_junk_dest_stack = 0;
                                        clear_junk_dest_stack < state_stacks_num;
                                        clear_junk_dest_stack++
                                       )
                                    {
                                        if ((fcs_stack_len(state, clear_junk_dest_stack) == 0) && (stacks_map[clear_junk_dest_stack] == 0))
                                        {
                                            stacks_map[clear_junk_dest_stack] = 1;
                                            break;
                                        }
                                    }
                                }
                                after_junk_num_freestacks--;
                            }

							if (clear_junk_dest_stack == -1)
							{
								break;
							}
 
                        }

                       junk_move_to_stacks[false_seq_index] = clear_junk_dest_stack;
                    }

                    if (false_seq_index != num_separate_false_seqs)
                    {
						continue;
					}

					if (calc_max_sequence_move(0, after_junk_num_freestacks) < num_true_seqs)
					{
						continue;
					}
                    /*
                     * We can do it - so let's move everything.
                     * Notice that we only put the child in a different stack
                     * then the parent and let it move to the parent in the
                     * next iteration of the program
                     * */

                    sfs_check_state_begin();

                    my_copy_stack(ds);
                    my_copy_stack(stack);


                    /* Move the junk cards to their place */

                    for(false_seq_index=0;
                        false_seq_index<num_separate_false_seqs;
                        false_seq_index++
                        )
                    {
                        /*
                         * start and end are the start and end heights of the sequence that is to be moved.
                         * */
                        int start = seq_points[false_seq_index];
                        int end = ((false_seq_index == 0) ? (dest_cards_num-1) : (seq_points[false_seq_index-1]-1));

                        my_copy_stack(junk_move_to_stacks[false_seq_index]);

                        fcs_move_sequence(junk_move_to_stacks[false_seq_index], ds, start, end, a);

                    }

                    /* Move the source seq on top of the dest seq */
                    fcs_move_sequence(ds, stack, h+1, cards_num-1, a);

                    sfs_check_state_end();
                }
            }

            /* Stop if we reached the bottom of the stack */
            if (h == -1)
            {
                break;
            }
            /* If this is no longer a sequence - move to the next stack */
			if (!fcs_is_ss_false_parent(fcs_stack_card(state,stack,h), card))
            {
                break;
            }
            
			next_card = fcs_stack_card(state,stack,h);
			if (!fcs_suit_is_ss_true_parent(next_card, card))
            {
                num_true_seqs++;
            }

            card = next_card;
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


int freecell_solver_sfs_simple_simon_move_sequence_with_some_cards_above_to_true_parent(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    tests_declare_accessors();


    fcs_move_t temp_move;

    int stack, cards_num, a;
    fcs_card_t card, temp_card, saved_card;
    int num_true_seqs, ds, dest_cards_num ;
    int check;
    int sc, num_separate_false_seqs, above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK];
    int seq_points[MAX_NUM_CARDS_IN_A_STACK];
    int stacks_map[MAX_NUM_STACKS];
    int after_junk_num_freestacks;
    int false_seq_index;
    int junk_move_to_stacks[MAX_NUM_CARDS_IN_A_STACK];

    int state_stacks_num;
    tests_define_accessors();

    state_stacks_num = instance->stacks_num;

    for(stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        if (cards_num <= 0)
		{
			continue;
		}

        for( sc = cards_num-1 ; sc >= 0 ; sc-- )
        {
            int above_c;
            fcs_card_t above_card, up_above_card;
            int end_of_src_seq;

            saved_card = card = fcs_stack_card(state, stack, sc);
            num_true_seqs = 1;

            for (end_of_src_seq = sc+1; end_of_src_seq < cards_num ; end_of_src_seq++)
            {
                above_card = fcs_stack_card(state, stack, end_of_src_seq);
                if (!fcs_is_ss_false_parent(card, above_card))
                {
                    break;
                }
				if (!fcs_suit_is_ss_true_parent(card, above_card))
                {
                    num_true_seqs++;
                }
                card = above_card;
            }

            if (end_of_src_seq == cards_num)
            {
                continue;
            }

            /* Split the cards above it into false sequences */

            num_separate_false_seqs = 0;
            above_card = fcs_stack_card(state, stack, cards_num-1);
            above_num_true_seqs[num_separate_false_seqs] = 1;
            for(above_c = cards_num-2 ;
                above_c > end_of_src_seq-1 ;
                above_c--
                )
            {
                up_above_card = fcs_stack_card(state, stack, above_c);
                if (! fcs_is_ss_false_parent(up_above_card, above_card))
                {
                    seq_points[num_separate_false_seqs++] = above_c+1;
                    above_num_true_seqs[num_separate_false_seqs] = 1;
                }
                above_num_true_seqs[num_separate_false_seqs] += ! (fcs_card_suit(up_above_card) == fcs_card_suit(above_card));
                above_card = up_above_card;
            }

            if (end_of_src_seq-1 < cards_num-1)
            {
                seq_points[num_separate_false_seqs++] = above_c+1;
            }

            for(ds=0;ds<state_stacks_num;ds++)
            {
                if (ds == stack)
                {
                    continue;
                }

                dest_cards_num = fcs_stack_len(state,ds);
                if (dest_cards_num <= 0)
				{
					continue;
                }

				if (!fcs_is_ss_true_parent(fcs_stack_card(state, ds, dest_cards_num-1), saved_card))
				{
					continue;
				}

                /* This is a suitable parent - let's check if we
                 * have enough empty stacks to make the move feasible */

                for(a=0;a<state_stacks_num;a++)
                {
                    stacks_map[a] = 0;
                }
                stacks_map[stack] = 1;
                stacks_map[ds] = 1;

                after_junk_num_freestacks = num_freestacks;

                for(false_seq_index=0;false_seq_index<num_separate_false_seqs;false_seq_index++)
                {
                    /* Find a suitable place to put it */
                    int clear_junk_dest_stack;

                    /* Let's try to find a suitable parent on top one of the stacks */
                    for(clear_junk_dest_stack=0;
                        clear_junk_dest_stack < state_stacks_num;
                        clear_junk_dest_stack++
                       )
                    {
                        int clear_junk_stack_len = fcs_stack_len(state, clear_junk_dest_stack);

                        if ((clear_junk_stack_len > 0) && (stacks_map[clear_junk_dest_stack] == 0) &&
							(fcs_is_ss_false_parent(fcs_stack_card(state, clear_junk_dest_stack, clear_junk_stack_len-1), fcs_stack_card(state, stack, seq_points[false_seq_index]))) &&
							(calc_max_sequence_move(0, after_junk_num_freestacks) >= above_num_true_seqs[false_seq_index]))
						{
							stacks_map[clear_junk_dest_stack] = 1;
                            break;
						}
                    }

                    if (clear_junk_dest_stack == state_stacks_num)
                    {
                        clear_junk_dest_stack = -1;

                        /* Check if there is a vacant stack */
                        if (num_freestacks > 0)
                        {
                            if (calc_max_sequence_move(0, after_junk_num_freestacks-1) >= above_num_true_seqs[false_seq_index])
                            {
                                /* Find an empty stack and designate it as the destination for the junk */
                                for(
                                    clear_junk_dest_stack = 0;
                                    clear_junk_dest_stack < state_stacks_num;
                                    clear_junk_dest_stack++
                                   )
                                {
                                    if ((fcs_stack_len(state, clear_junk_dest_stack) == 0) && (stacks_map[clear_junk_dest_stack] == 0))
                                    {
                                        stacks_map[clear_junk_dest_stack] = 1;
                                        break;
                                    }
                                }
                            }
                            after_junk_num_freestacks--;
                        }
						 
						if (clear_junk_dest_stack == -1)
	                    {
		                    break;
		                }

 
                    }

                    junk_move_to_stacks[false_seq_index] = clear_junk_dest_stack;
                }

                if (false_seq_index != num_separate_false_seqs)
                {
					continue;
				}

                if (calc_max_sequence_move(0, after_junk_num_freestacks) < num_true_seqs)
                {
					continue;
				}

                sfs_check_state_begin();

                my_copy_stack(stack);
                my_copy_stack(ds);



                /* Let's boogie - we can move everything */
                /* Move the junk cards to their place */
                for(false_seq_index=0; false_seq_index<num_separate_false_seqs; false_seq_index++)
                {
                    int end;

                    end = ((false_seq_index == 0) ? (cards_num-1) : (seq_points[false_seq_index-1]-1));
                    my_copy_stack(junk_move_to_stacks[false_seq_index]);

                    fcs_move_sequence(junk_move_to_stacks[false_seq_index], stack, seq_points[false_seq_index], end, a);
                }

                fcs_move_sequence(ds, stack, sc, end_of_src_seq-1, a);

                sfs_check_state_end();
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int freecell_solver_sfs_simple_simon_move_sequence_with_junk_seq_above_to_true_parent_with_some_cards_above(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    tests_declare_accessors();


    fcs_move_t temp_move;

    int check;

    /*
     * stack - the source stack index
     * cards_num - the number of cards in "stack"
     * h - the height of the current card in "stack".
     * card - the current card in "stack"
	 * saved_card - saved card in "stack"
     * ds - the index of the destination stack
     * dest_cards_num - the number of cards in "ds".
     * dc - the height of the current card in "ds".
     * num_separate_false_seqs - the number of false sequences
     * seq_points[] - the places in which the false sequences of the junk begin
     *      and end
     * false_seq_index - an iterator that marks the index of the current
     *      false sequence
     * stacks_map[] - a map of booleans that indicates if one can place a card
     *      on this stack or is already taken.
     * above_num_true_seqs[] - the number of true sequences in each false
     *      sequence
     * num_src_junk_true_seqs - the number of true seqs in the false seq above
     *      the source card.
     * end_of_junk - the height marking the end of the source junk.
     * num_true_seqs - the number of true sequences in the false seq which we
     *      wish to move.
     * */
    int stack, cards_num, a;
    fcs_card_t card, temp_card, saved_card;
    int above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK], h, ds, dest_cards_num ;

    int above_c;
    fcs_card_t above_card, up_above_card;

    int dc;
    int seq_points[MAX_NUM_CARDS_IN_A_STACK];
    int num_separate_false_seqs;
    int false_seq_index;
    int stacks_map[MAX_NUM_STACKS];
    int after_junk_num_freestacks;
    int junk_move_to_stacks[MAX_NUM_STACKS];
    int num_src_junk_true_seqs;
    int end_of_junk;
    int num_true_seqs;

    int state_stacks_num;
    tests_define_accessors();

    state_stacks_num = instance->stacks_num;

    for(stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        if (cards_num <= 0)
        {
			continue;
		}

        saved_card = card = fcs_stack_card(state,stack,cards_num-1);
        num_src_junk_true_seqs = 1;

        for(h=cards_num-2;h>-1;h--)
        {
            card = fcs_stack_card(state, stack, h);
			if (!fcs_is_ss_false_parent(card, saved_card))
            {
                break;
            }
			if (!fcs_suit_is_ss_true_parent(card, saved_card))
            {
                num_src_junk_true_seqs++;
            }
			
			saved_card = card;
        }

        if (h == -1)
        {
			continue;
		}

        end_of_junk = h;
        num_true_seqs = 1;

        for(;h>-1;h--)
        {
            card = fcs_stack_card(state,stack,h);
			if (!fcs_is_ss_false_parent(card, saved_card))
            {
                break;
            }
            if (!fcs_suit_is_ss_true_parent(card, saved_card))
            {
                num_true_seqs++;
            }

			saved_card = card;
        }

		saved_card = card;

        for(ds=0;ds<state_stacks_num;ds++)
        {
            if (ds == stack)
            {
                continue;
            }

            dest_cards_num = fcs_stack_len(state,ds);
            /* At least a card with another card above it */
            if (dest_cards_num <= 1)
            {
				continue;
			}

            /* Start at the card below the top one, so we will
             * make sure there's at least some junk above it
             * */
            for(dc=dest_cards_num-2;dc>=0;dc--)
            {
				if (!fcs_is_ss_true_parent(fcs_stack_card(state, ds, dc), saved_card))
                {
					continue;
				}

                /* This is a suitable parent - let's check if there's a sequence above it. */
                num_separate_false_seqs = 0;
                above_card = fcs_stack_card(state, ds, dest_cards_num-1);
                above_num_true_seqs[num_separate_false_seqs] = 1;
                for(above_c = dest_cards_num-2; above_c > dc; above_c--)
                {
                    up_above_card = fcs_stack_card(state, ds, above_c);
                    if (! fcs_is_ss_false_parent(up_above_card, above_card))
                    {
                        seq_points[num_separate_false_seqs++] = above_c+1;
                        above_num_true_seqs[num_separate_false_seqs] = 1;
                    }
                    above_num_true_seqs[num_separate_false_seqs] += ! (fcs_card_suit(up_above_card) == fcs_card_suit(above_card));
                    above_card = up_above_card;
                }

                if (dc < dest_cards_num - 1)
                {
                    seq_points[num_separate_false_seqs++] = above_c+1;
                }

                for(a=0;a<state_stacks_num;a++)
                {
                    stacks_map[a] = 0;
                }
                stacks_map[stack] = 1;
                stacks_map[ds] = 1;

                after_junk_num_freestacks = num_freestacks;

                for(false_seq_index=0;false_seq_index<num_separate_false_seqs+1;false_seq_index++)
                {
                    /* Find a suitable place to put it */
                    int clear_junk_dest_stack;

                    fcs_card_t the_card =
                        (
                            (false_seq_index == num_separate_false_seqs) ?
                                (fcs_stack_card(state, stack, end_of_junk+1)) :
                                (fcs_stack_card(state, ds, seq_points[false_seq_index]))
                        );

                    int the_num_true_seqs =
                        (
                            (false_seq_index == num_separate_false_seqs) ?
                                num_src_junk_true_seqs :
                                above_num_true_seqs[false_seq_index]
                        );

                    /* Let's try to find a suitable parent on top one of the stacks */
                    for(clear_junk_dest_stack=0;
                        clear_junk_dest_stack < state_stacks_num;
                        clear_junk_dest_stack++
                       )
                    {
                        int clear_junk_stack_len = fcs_stack_len(state, clear_junk_dest_stack);

                        if ((clear_junk_stack_len > 0) && (stacks_map[clear_junk_dest_stack] == 0) &&
							(fcs_is_ss_false_parent(fcs_stack_card(state, clear_junk_dest_stack, clear_junk_stack_len-1), the_card)) &&
							(calc_max_sequence_move(0, after_junk_num_freestacks) >= the_num_true_seqs))
                        {
                            stacks_map[clear_junk_dest_stack] = 1;
                            break;
                        }
                    }

                    if (clear_junk_dest_stack == state_stacks_num)
                    {
                        clear_junk_dest_stack = -1;

						/* Check if there is a vacant stack */
                        if (num_freestacks > 0)
                        {
                            if (calc_max_sequence_move(0, after_junk_num_freestacks-1) >= the_num_true_seqs)
                            {
                                /* Find an empty stack and designate it as the destination for the junk */
                                for(
                                    clear_junk_dest_stack = 0;
                                    clear_junk_dest_stack < state_stacks_num;
                                    clear_junk_dest_stack++
                                   )
                                {
                                    if ((fcs_stack_len(state, clear_junk_dest_stack) == 0) && (stacks_map[clear_junk_dest_stack] == 0))
                                    {
                                        stacks_map[clear_junk_dest_stack] = 1;
                                        break;
                                    }
                                }
                            }
                            after_junk_num_freestacks--;
                        }

	                    if (clear_junk_dest_stack == -1)
	                    {
	                        break;
		                }

                    }

                    junk_move_to_stacks[false_seq_index] = clear_junk_dest_stack;
                }

                if (false_seq_index != num_separate_false_seqs+1)
                {
					continue;
				}

                if (calc_max_sequence_move(0, after_junk_num_freestacks) < num_true_seqs)
                {
					continue;
                }

				/* We can do it - so let's move everything */
                sfs_check_state_begin();

                my_copy_stack(stack);
                my_copy_stack(ds);

                /* Move the junk cards to their place */
                for(false_seq_index=0; false_seq_index<num_separate_false_seqs+1; false_seq_index++)
                {
                    int start;
                    int end;
                    int src_stack;

                    if (false_seq_index == num_separate_false_seqs)
                    {
                        start = end_of_junk+1;
                        end = cards_num-1;
                        src_stack = stack;
                    }
                    else
                    {
                        start = seq_points[false_seq_index];
                        end = ((false_seq_index == 0) ? (dest_cards_num-1) : (seq_points[false_seq_index-1]-1));
                        src_stack = ds;
                    }

                    my_copy_stack(src_stack);

                    my_copy_stack(junk_move_to_stacks[false_seq_index]);

                    fcs_move_sequence(junk_move_to_stacks[false_seq_index], src_stack, start, end, a);
                }

                /* Move the source seq on top of the dest seq */
                fcs_move_sequence(ds, stack, h, end_of_junk, a);

                sfs_check_state_end();
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int freecell_solver_sfs_simple_simon_move_whole_stack_sequence_to_false_parent_with_some_cards_above(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    tests_declare_accessors();


    fcs_move_t temp_move;

    int check;

    /*
     * stack - the source stack index
     * cards_num - the number of cards in "stack"
     * h - the height of the current card in stack
     * card - the current card
	 * saved_card - saved card in "stack"
     * ds - the destination stack index.
     * dest_cards_num - the number of cards in it.
     * dc - the height of the card in "ds".
     * num_separate_false_seqs - this variable tells how many distinct false
     *      sequences exist above the false parent
     * above_num_true_seqs[] - the number of true sequences in each false
     *      sequence
     * seq_points[] - the separation points of the false sequences (i.e: where
     *      they begin and end)
     * stacks_map[] - a boolean map that indicates if one can place a card
     *      on this stack or is it already taken.
     * junk_move_to_stacks[] - the stacks to move each false sequence of the
     *      junk to.
     * false_seq_index - an iterator to hold the index of the current false
     *      sequence.
     * after_junk_num_freestacks - a variable that holds the number of stacks
     *      that are left unoccupied as part of the junk disposal process.
     *
     * */
    int stack, cards_num, a;
    fcs_card_t card, temp_card, saved_card;
    int num_true_seqs, h, ds, dest_cards_num ;

    int above_c;
    fcs_card_t above_card, up_above_card;

    int dc, num_separate_false_seqs, above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK];
    int seq_points[MAX_NUM_CARDS_IN_A_STACK];
    int stacks_map[MAX_NUM_STACKS];
    int after_junk_num_freestacks;
    int false_seq_index;
    int junk_move_to_stacks[MAX_NUM_STACKS];

    int state_stacks_num;
    tests_define_accessors();

    state_stacks_num = instance->stacks_num;

    for(stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        if (cards_num <= 0)
        {
			continue;
		}

		saved_card = card = fcs_stack_card(state,stack,cards_num-1);
        num_true_seqs = 1;

        for(h=cards_num-2;h>-1;h--)
        {
            card = fcs_stack_card(state,stack,h);
			if (!fcs_is_ss_false_parent(card, saved_card))
            {
                break;
            }
			if (!fcs_suit_is_ss_true_parent(card, saved_card))
            {
                num_true_seqs++;
            }

			saved_card = card;
        }
        if (h != -1)
        {
			continue;
		}

        for(ds=0;ds<state_stacks_num;ds++)
        {
            dest_cards_num = fcs_stack_len(state,ds);
            if (dest_cards_num <= 0)
            {
				continue;
			}

            for(dc=dest_cards_num-1;dc>=0;dc--)
            {
				if (!fcs_is_ss_false_parent(fcs_stack_card(state, ds, dc), saved_card))
				{
					continue;
				}

				/* This is a suitable parent - let's check if there's a sequence above it. */
                num_separate_false_seqs = 0;
                above_card = fcs_stack_card(state, ds, dest_cards_num-1);
                above_num_true_seqs[num_separate_false_seqs] = 1;
                for(above_c = dest_cards_num-2; above_c > dc; above_c--)
                {
                    up_above_card = fcs_stack_card(state, ds, above_c);
                    if (! fcs_is_ss_false_parent(up_above_card, above_card))
                    {
                        seq_points[num_separate_false_seqs++] = above_c+1;
                        above_num_true_seqs[num_separate_false_seqs] = 1;
                    }
                    above_num_true_seqs[num_separate_false_seqs] += ! (fcs_suit_is_ss_true_parent(up_above_card, above_card));
                    above_card = up_above_card;
                }

                if (dc < dest_cards_num - 1)
                {
                    seq_points[num_separate_false_seqs++] = above_c+1;
                }

                for(a=0;a<state_stacks_num;a++)
                {
                    stacks_map[a] = 0;
                }
                stacks_map[stack] = 1;
                stacks_map[ds] = 1;

                after_junk_num_freestacks = num_freestacks;

                for(false_seq_index=0;false_seq_index<num_separate_false_seqs;false_seq_index++)
                {
                    /* Find a suitable place to put it */
                    int clear_junk_dest_stack,
						the_num_true_seqs = above_num_true_seqs[false_seq_index];

                    /* Let's try to find a suitable parent on top one of the stacks */
                    for(clear_junk_dest_stack=0;
                        clear_junk_dest_stack < state_stacks_num;
                        clear_junk_dest_stack++
                       )
                    {
                        int clear_junk_stack_len = fcs_stack_len(state, clear_junk_dest_stack);
                        if ((clear_junk_stack_len > 0) && (stacks_map[clear_junk_dest_stack] == 0) &&
							(fcs_is_ss_false_parent(fcs_stack_card(state, clear_junk_dest_stack, clear_junk_stack_len-1), 
													fcs_stack_card(state, ds, seq_points[false_seq_index]))) &&
							(calc_max_sequence_move(0, after_junk_num_freestacks) >= the_num_true_seqs))


                        {
                            stacks_map[clear_junk_dest_stack] = 1;
                            break;
                        }
                    }

                    if (clear_junk_dest_stack == state_stacks_num)
                    {
						break;
                    }

                    junk_move_to_stacks[false_seq_index] = clear_junk_dest_stack;
                }

                if (false_seq_index != num_separate_false_seqs)
                {
					continue;
				}

                /* This is a suitable parent - let's check if we
                 * have enough empty stacks to make the move feasible */
                if (calc_max_sequence_move(0, num_freestacks) < num_true_seqs)
                {
					continue;
				}

                /* We can do it - so let's move */
                sfs_check_state_begin();

                my_copy_stack(stack);
                my_copy_stack(ds);


                /* Move the junk cards to their place */

                for(false_seq_index=0; false_seq_index<num_separate_false_seqs; false_seq_index++)
                {
                    int end = ((false_seq_index == 0) ? (dest_cards_num-1) : (seq_points[false_seq_index-1]-1));

                    my_copy_stack(ds);
                    my_copy_stack(junk_move_to_stacks[false_seq_index]);

                    fcs_move_sequence( junk_move_to_stacks[false_seq_index], ds, seq_points[false_seq_index], end, a);
                }

                fcs_move_sequence( ds, stack, h+1, cards_num-1, a);

                sfs_check_state_end();
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int freecell_solver_sfs_simple_simon_move_sequence_to_parent_on_the_same_stack(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    tests_declare_accessors();


    fcs_move_t temp_move;

    int check;

    int stack, cards_num, pc, cc;
    fcs_card_t parent_card, child_card;
    int a;
    int after_junk_num_freestacks;
    int false_seq_index;
    int child_seq_index;

    fcs_card_t temp_card;

    int above_num_true_seqs[MAX_NUM_CARDS_IN_A_STACK];
    int seq_points[MAX_NUM_CARDS_IN_A_STACK];
    int stacks_map[MAX_NUM_STACKS];
    int junk_move_to_stacks[MAX_NUM_STACKS];
    int num_separate_false_seqs;

    fcs_card_t above_card, up_above_card;
    int above_c;

    int end_of_child_seq;
    int child_num_true_seqs;
		
	int end2, start;

    int state_stacks_num;
    tests_define_accessors();

    state_stacks_num = instance->stacks_num;

    for(stack=0 ; stack < state_stacks_num ; stack++)
    {
        cards_num = fcs_stack_len(state, stack);
        if (cards_num <= 2)
        {
			continue;
		}

        /* Search for a parent card */
        for(pc=0; pc < cards_num-1 ; pc++)
        {
            parent_card = fcs_stack_card(state, stack, pc);
            if (fcs_is_ss_true_parent(parent_card, fcs_stack_card(state, stack, pc+1)))
            {
                continue;
            }

            for(cc = pc + 2 ; cc < cards_num ; cc++)
            {
                child_card = fcs_stack_card(state, stack, cc);
                if (!fcs_is_ss_true_parent(parent_card, child_card))
                {
					continue;
				}

				/* We have a matching parent and child cards */
#if 0
                printf("Stack %i, Parent %i, Child %i\n", stack, pc, cc);
                fflush(stdout);
#endif

                /*
                 * Now let's try to find stacks to place the cards above
                 * the child card.
                 * */

                end_of_child_seq = cc;
                child_num_true_seqs = 1;
                while ((end_of_child_seq+1 < cards_num) &&
                      fcs_is_ss_false_parent(
                            fcs_stack_card(state, stack, end_of_child_seq),
                            fcs_stack_card(state, stack, end_of_child_seq+1)
                       )
                      )
                {
                    child_num_true_seqs += (!fcs_is_ss_true_parent(
                            fcs_stack_card(state, stack, end_of_child_seq),
                            fcs_stack_card(state, stack, end_of_child_seq+1)
                       ));
                    end_of_child_seq++;
                }

                num_separate_false_seqs = 0;
                above_card = fcs_stack_card(state, stack, cards_num-1);
                above_num_true_seqs[num_separate_false_seqs] = 1;
                for(above_c = cards_num-2; above_c > end_of_child_seq; above_c--)
                {
                    up_above_card = fcs_stack_card(state, stack, above_c);
                    if (! fcs_is_ss_false_parent(up_above_card, above_card))
                    {
                        seq_points[num_separate_false_seqs++] = above_c+1;
                        above_num_true_seqs[num_separate_false_seqs] = 1;
                    }
                    above_num_true_seqs[num_separate_false_seqs] += ! (fcs_suit_is_ss_true_parent(up_above_card, above_card));
                    above_card = up_above_card;
                }

                if (end_of_child_seq < cards_num - 1)
                {
                    seq_points[num_separate_false_seqs++] = above_c+1;
                }

                /* Add the child to the seq_points */
                child_seq_index = num_separate_false_seqs;
                above_num_true_seqs[num_separate_false_seqs] = child_num_true_seqs;
                seq_points[num_separate_false_seqs++] = cc;

                /* Add the cards between the parent and the child to the seq_points */

                above_card = fcs_stack_card(state, stack, cc-1);
                above_num_true_seqs[num_separate_false_seqs] = 1;
                for(above_c = cc-2; above_c > pc; above_c--)
                {
                    up_above_card = fcs_stack_card(state, stack, above_c);
                    if (! fcs_is_ss_false_parent(up_above_card, above_card))
                    {
                        seq_points[num_separate_false_seqs++] = above_c+1;
                        above_num_true_seqs[num_separate_false_seqs] = 1;
                    }
                    above_num_true_seqs[num_separate_false_seqs] += ! (fcs_suit_is_ss_true_parent(up_above_card, above_card));
                    above_card = up_above_card;
                }

                if (pc < cc - 1)
                {
                    seq_points[num_separate_false_seqs++] = above_c+1;
                }

                for(a = 0 ; a < state_stacks_num ; a++)
                {
                    stacks_map[a] = 0;
                }
                stacks_map[stack] = 1;

                after_junk_num_freestacks = num_freestacks;

                for(false_seq_index=0;false_seq_index<num_separate_false_seqs;false_seq_index++)
                {
                    /* Find a suitable place to put it */
                    int clear_junk_dest_stack;

                    /* Let's try to find a suitable parent on top one of the stacks */
                    for(clear_junk_dest_stack=0;
                        clear_junk_dest_stack < state_stacks_num;
                        clear_junk_dest_stack++
                       )
                    {
                        int clear_junk_stack_len = fcs_stack_len(state, clear_junk_dest_stack);

                        if ((clear_junk_stack_len > 0) && (stacks_map[clear_junk_dest_stack] == 0) &&
							(fcs_is_ss_false_parent(fcs_stack_card(state, clear_junk_dest_stack, clear_junk_stack_len-1), fcs_stack_card(state, stack, seq_points[false_seq_index]))) &&
							(calc_max_sequence_move(0, after_junk_num_freestacks) >= above_num_true_seqs[false_seq_index]))
                        {
                            stacks_map[clear_junk_dest_stack] = 1;
                            break;
                        }
                    }

                    if (clear_junk_dest_stack == state_stacks_num)
                    {
                        clear_junk_dest_stack = -1;
                        /* Check if there is a vacant stack */
                        if (num_freestacks > 0)
                        {
                            if (calc_max_sequence_move(0, after_junk_num_freestacks-1) >= above_num_true_seqs[false_seq_index])
                            {
                                /* Find an empty stack and designate it as the destination for the junk */
                                for(
                                    clear_junk_dest_stack = 0;
                                    clear_junk_dest_stack < state_stacks_num;
                                    clear_junk_dest_stack++
                                   )
                                {
                                    if ((fcs_stack_len(state, clear_junk_dest_stack) == 0) && (stacks_map[clear_junk_dest_stack] == 0))
                                    {
                                        stacks_map[clear_junk_dest_stack] = 1;
                                        break;
                                    }
                                }
                            }
                            after_junk_num_freestacks--;
                        }

						if (clear_junk_dest_stack == -1)
						{
							break;
						}
                    }

					junk_move_to_stacks[false_seq_index] = clear_junk_dest_stack;
                }

                if (false_seq_index != num_separate_false_seqs)
                {
					continue;
				}

                /* Let's check if we can move the child after we are done moving all the junk cards */
                if (calc_max_sequence_move(0, after_junk_num_freestacks) < child_num_true_seqs)
                {
					continue;
                }

				/* We can do it - so let's move everything */

                sfs_check_state_begin();

                /* Move the junk cards to their place */

                my_copy_stack(stack);

                for(false_seq_index=0; false_seq_index<num_separate_false_seqs; false_seq_index++)
                {
                    int end = ((false_seq_index == 0) ? (cards_num-1) : (seq_points[false_seq_index-1]-1));

                    my_copy_stack(junk_move_to_stacks[false_seq_index]);
                    fcs_move_sequence ( junk_move_to_stacks[false_seq_index], stack, seq_points[false_seq_index], end, a);
                }

                end2 = fcs_stack_len(new_state, junk_move_to_stacks[child_seq_index])-1;
                start = end2-(end_of_child_seq-cc);

                my_copy_stack(junk_move_to_stacks[child_seq_index]);

                fcs_move_sequence( stack, junk_move_to_stacks[child_seq_index], start, end2, a);

                sfs_check_state_end();
            }
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

int freecell_solver_sfs_simple_simon_move_sequence_to_false_parent(
        freecell_solver_soft_thread_t * soft_thread,
        fcs_state_with_locations_t * ptr_state_with_locations,
        int num_freestacks,
        int num_freecells,
        fcs_derived_states_list_t * derived_states_list,
        int reparent
        )
{
    tests_declare_accessors();

    fcs_move_t temp_move;

    int check;

    /*
     * stack - the source stack index
     * cards_num - number of cards in "stack"
     * ds - the dest stack index
     * dest_cards_num - number of cards in "ds".
     * card - the current card
     * next_card - the next card on the stack.
     * h - the height of the current card on "stack"
     * num_true_seqs - the number of true sequences on the current
     *                 false sequence
     * */
    int stack, cards_num, a;
    fcs_card_t card, temp_card, next_card;
    int num_true_seqs, seq_size, h, ds, dest_cards_num;

    int state_stacks_num;
    tests_define_accessors();

    state_stacks_num = instance->stacks_num;

    for(stack=0;stack<state_stacks_num;stack++)
    {
        cards_num = fcs_stack_len(state, stack);
		if (cards_num <= 0)
		{
			continue;
		}

		card = fcs_stack_card(state,stack,cards_num-1);
        num_true_seqs = 1;
		seq_size = 1;

        /* Stop if we reached the bottom of the stack */
        for(h=cards_num-2;h>-1;h--)
        {
            next_card = fcs_stack_card(state,stack,h);
            /* If this is no longer a sequence - move to the next stack */
			if (!fcs_is_ss_false_parent(next_card, card))
			{
				break;
			}

			seq_size++;

			if (!fcs_suit_is_ss_true_parent(next_card, card))
			{
				num_true_seqs++;
			}

            card = next_card;
        }

		/* take the sequence and try and put it on another stack */
        for(ds=0;ds<state_stacks_num;ds++)
        {
            dest_cards_num = fcs_stack_len(state,ds);
            if (dest_cards_num <= 0)
            {
				continue;
			}

			if (!fcs_is_ss_false_parent(fcs_stack_card(state, ds, dest_cards_num-1), card))
			{
				continue;
			}

			/* This is a suitable parent - let's check if we
             * have enough empty stacks to make the move feasible */
            if (calc_max_sequence_move(0, num_freestacks) < num_true_seqs)
            {
				continue;
			}

            /* We can do it - so let's move */
            sfs_check_state_begin();

            my_copy_stack(stack);
            my_copy_stack(ds);

            fcs_move_sequence(ds, stack, h+1, h+seq_size, a);
            sfs_check_state_end();
        }
    }

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

#undef state_with_locations
#undef state
#undef new_state_with_locations
#undef new_state
