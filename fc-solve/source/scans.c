/*
 * scans.c - The code that relates to the various scans.
 * Currently Hard DFS, Soft-DFS, Random-DFS, A* and BFS are implemented.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000-2001
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#if FCS_STATE_STORAGE==FCS_STATE_STORAGE_LIBREDBLACK_TREE
#include <search.h>
#endif

#include "config.h"
#include "state.h"
#include "card.h"
#include "fcs_dm.h"
#include "fcs.h"

#include "fcs_isa.h"

#include "test_arr.h"
#include "caas.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static pq_rating_t freecell_solver_a_star_rate_state(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations);

#define freecell_solver_a_star_enqueue_state(soft_thread,ptr_state_with_locations) \
    {        \
        freecell_solver_PQueuePush(        \
            a_star_pqueue,       \
            ptr_state_with_locations,            \
            freecell_solver_a_star_rate_state(soft_thread, ptr_state_with_locations)   \
            );       \
    }

#if 0
static void freecell_solver_a_star_enqueue_state(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations
    )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    freecell_solver_PQueuePush(
        soft_thread->a_star_pqueue,
        ptr_state_with_locations,
        freecell_solver_a_star_rate_state(soft_thread, ptr_state_with_locations)
        );
}
#endif

#define freecell_solver_bfs_enqueue_state(soft_thread, state) \
    {    \
        fcs_states_linked_list_item_t * last_item_next;      \
        last_item_next = bfs_queue_last_item->next = (fcs_states_linked_list_item_t*)malloc(sizeof(fcs_states_linked_list_item_t));      \
        bfs_queue_last_item->s = state;     \
        last_item_next->next = NULL;     \
        bfs_queue_last_item = last_item_next; \
    }

#if 0
static void freecell_solver_bfs_enqueue_state(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * state
    )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    soft_thread->bfs_queue_last_item->next = (fcs_states_linked_list_item_t*)malloc(sizeof(fcs_states_linked_list_item_t));
    soft_thread->bfs_queue_last_item->s = state;
    soft_thread->bfs_queue_last_item->next->next = NULL;
    soft_thread->bfs_queue_last_item = soft_thread->bfs_queue_last_item->next;
}
#endif

    

#define the_state (ptr_state_with_locations->s)

int freecell_solver_hard_dfs_solve_for_state(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations,
    int depth,
    int ignore_osins
    )

{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    int a;
    int check;

    int num_freestacks, num_freecells;

    int iter_num = instance->num_times;

    fcs_derived_states_list_t derived;

    int derived_state_index;

    int ret_value;

    int freecells_num, stacks_num;

    int calc_real_depth, scans_synergy;

    freecells_num = instance->freecells_num;
    stacks_num = instance->stacks_num;

    derived.num_states = derived.max_num_states = 0;
    derived.states = NULL;

    calc_real_depth = instance->calc_real_depth;
    scans_synergy = instance->scans_synergy;

    /*
     * If this state has not been visited before - increase the number of
     * iterations this program has seen, and output this state again.
     *
     * I'm doing this in order to make the output of a stopped and
     * resumed run consistent with the output of a normal (all-in-one-time)
     * run.
     * */
    if (!is_scan_visited(ptr_state_with_locations, soft_thread->id))
    {
        if (instance->debug_iter_output)
        {
            instance->debug_iter_output_func(
                    (void*)instance->debug_iter_output_context,
                    iter_num,
                    depth,
                    (void*)instance,
                    ptr_state_with_locations,
                    0  /* It's a temporary kludge */
                    );
        }
        /* Increase the number of iterations */
        instance->num_times++;
        hard_thread->num_times++;
        ptr_state_with_locations->visited_iter = iter_num;
    }

    /* Mark this state as visited, so it won't be recursed into again. */
    set_scan_visited(ptr_state_with_locations, soft_thread->id);

    /* Count the free-cells */
    num_freecells = 0;
    for(a=0;a<freecells_num;a++)
    {
        if (fcs_freecell_card_num(the_state, a) == 0)
        {
            num_freecells++;
        }
    }

    /* Count the number of unoccupied stacks */

    num_freestacks = 0;
    for(a=0;a<stacks_num;a++)
    {
        if (fcs_stack_len(the_state, a) == 0)
        {
            num_freestacks++;
        }
    }


    /* Let's check if this state is finished, and if so return 0; */
    if ((num_freestacks == stacks_num) && (num_freecells == freecells_num))
    {
        instance->final_state = ptr_state_with_locations;

        ret_value = FCS_STATE_WAS_SOLVED;
        goto free_derived;
    }

    calculate_real_depth(ptr_state_with_locations);

    for(a=0 ;
        a < soft_thread->tests_order.num;
        a++)
    {
        derived.num_states = 0;

        check =
            freecell_solver_sfs_tests[soft_thread->tests_order.tests[a] & FCS_TEST_ORDER_NO_FLAGS_MASK ] (
                soft_thread,
                ptr_state_with_locations,
                num_freestacks,
                num_freecells,
                &derived,
                0
                );

        if ((check == FCS_STATE_BEGIN_SUSPEND_PROCESS) ||
                 (check == FCS_STATE_SUSPEND_PROCESS))
        {
            if (check == FCS_STATE_BEGIN_SUSPEND_PROCESS)
            {
                soft_thread->num_solution_states = depth+1;

                soft_thread->soft_dfs_info = malloc(sizeof(soft_thread->soft_dfs_info[0]) * soft_thread->num_solution_states);
            }

            soft_thread->soft_dfs_info[depth].state = ptr_state_with_locations;

            ret_value = FCS_STATE_SUSPEND_PROCESS;

            goto free_derived;
        }

        for(derived_state_index=0;derived_state_index<derived.num_states;derived_state_index++)
        {
            if (
                (! (derived.states[derived_state_index]->visited &
                    FCS_VISITED_DEAD_END)
                ) &&
                (! is_scan_visited(
                    derived.states[derived_state_index],
                    soft_thread->id)
                )
               )
            {
                check =
                    freecell_solver_hard_dfs_solve_for_state(
                        soft_thread,
                        derived.states[derived_state_index],
                        depth+1,
                        ignore_osins
                        );

                if ((check == FCS_STATE_SUSPEND_PROCESS) ||
                    (check == FCS_STATE_BEGIN_SUSPEND_PROCESS))
                {

                    soft_thread->soft_dfs_info[depth].state = ptr_state_with_locations;

                    ret_value =  FCS_STATE_SUSPEND_PROCESS;

                    goto free_derived;
                }

                if (check == FCS_STATE_WAS_SOLVED)
                {
                    ret_value = FCS_STATE_WAS_SOLVED;

                    goto free_derived;
                }
            }
        }
    }

    if (check_if_limits_exceeded())
    {
        soft_thread->num_solution_states = depth+1;

        soft_thread->soft_dfs_info = malloc(sizeof(soft_thread->soft_dfs_info[0]) * soft_thread->num_solution_states);
        

        soft_thread->soft_dfs_info[depth].state = ptr_state_with_locations;

        ret_value = FCS_STATE_SUSPEND_PROCESS;

        goto free_derived;
    }

    ret_value = FCS_STATE_IS_NOT_SOLVEABLE;

    if (soft_thread->is_a_complete_scan)
    {
        mark_as_dead_end(ptr_state_with_locations);
    }


free_derived:
    if (derived.states != NULL)
    {
        free(derived.states);
    }

    return ret_value;
}


int freecell_solver_hard_dfs_resume_solution(
    freecell_solver_soft_thread_t * soft_thread,
    int depth
    )
{
    fcs_state_with_locations_t * ptr_state_with_locations;
    int check;

    ptr_state_with_locations = soft_thread->soft_dfs_info[depth].state;

    if (depth < soft_thread->num_solution_states-1)
    {
        check = freecell_solver_hard_dfs_resume_solution(
            soft_thread,
            depth+1
            );
    }
    else
    {
        free(soft_thread->soft_dfs_info);
        soft_thread->soft_dfs_info = NULL;
        check = FCS_STATE_IS_NOT_SOLVEABLE;
    }

    if (check == FCS_STATE_IS_NOT_SOLVEABLE)
    {
        check = freecell_solver_hard_dfs_solve_for_state(
            soft_thread,
            ptr_state_with_locations,
            depth,
            1);
    }
    else if (check == FCS_STATE_WAS_SOLVED)
    {
        /* Do nothing - fall back to return check. */
    }
    else
    {
        if ((check == FCS_STATE_SUSPEND_PROCESS) || (check == FCS_STATE_WAS_SOLVED))
        {

            soft_thread->soft_dfs_info[depth].state = ptr_state_with_locations;
        }
    }

    return check;
}

#undef state





static void freecell_solver_increase_dfs_max_depth(
    freecell_solver_soft_thread_t * soft_thread
    )
{
    int new_dfs_max_depth = soft_thread->dfs_max_depth + 16;
    int d;

#define MYREALLOC(what) \
    soft_thread->what = realloc( \
        soft_thread->what,       \
        sizeof(soft_thread->what[0])*new_dfs_max_depth \
        ); \

    MYREALLOC(soft_dfs_info);
#undef MYREALLOC

    for(d=soft_thread->dfs_max_depth ; d<new_dfs_max_depth; d++)
    {
        soft_thread->soft_dfs_info[d].state = NULL;
        soft_thread->soft_dfs_info[d].derived_states_list.max_num_states = 0;
        soft_thread->soft_dfs_info[d].test_index = 0;
        soft_thread->soft_dfs_info[d].current_state_index = 0;
        soft_thread->soft_dfs_info[d].derived_states_list.num_states = 0;
        soft_thread->soft_dfs_info[d].derived_states_list.states = NULL;
        soft_thread->soft_dfs_info[d].derived_states_random_indexes = NULL;
        soft_thread->soft_dfs_info[d].derived_states_random_indexes_max_size = 0;
    }

    soft_thread->dfs_max_depth = new_dfs_max_depth;
}

/*
    freecell_solver_soft_dfs_or_random_dfs_do_solve_or_resume is the event loop of the
    Random-DFS scan. DFS which is recursive in nature is handled here
    without procedural recursion
    by using some dedicated stacks for the traversal.
  */
#define the_state (ptr_state_with_locations->s)

#define myreturn(ret_value) \
    soft_thread->num_solution_states = depth+1;     \
    return (ret_value);

int freecell_solver_soft_dfs_or_random_dfs_do_solve_or_resume(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations_orig,
    int resume,
    int to_randomize
    )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    int depth;
    fcs_state_with_locations_t * ptr_state_with_locations,
        * ptr_recurse_into_state_with_locations;
    int a;
    int check;
    int do_first_iteration;
    fcs_soft_dfs_stack_item_t * the_soft_dfs_info;
    int freecells_num, stacks_num;
    int dfs_max_depth;

    int tests_order_num = soft_thread->tests_order.num;
    int * tests_order_tests = soft_thread->tests_order.tests;
    int calc_real_depth = instance->calc_real_depth;
    int is_a_complete_scan = soft_thread->is_a_complete_scan;
    int soft_thread_id = soft_thread->id;
    int test_index, current_state_index;
    fcs_derived_states_list_t * derived_states_list;
    int to_reparent_states, scans_synergy;

    freecells_num = instance->freecells_num;
    stacks_num = instance->stacks_num;
    to_reparent_states = instance->to_reparent_states;
    scans_synergy = instance->scans_synergy;

    if (!resume)
    {
        /*
            Allocate some space for the states at depth 0.
        */
        depth=0;

        freecell_solver_increase_dfs_max_depth(soft_thread);

        /* Initialize the initial state to indicate it is the first */
        ptr_state_with_locations_orig->parent = NULL;
        ptr_state_with_locations_orig->moves_to_parent = NULL;
        ptr_state_with_locations_orig->depth = 0;

        soft_thread->soft_dfs_info[0].state = ptr_state_with_locations_orig;
    }
    else
    {
        /*
            Set the initial depth to that of the last state encountered.
        */
        depth = soft_thread->num_solution_states - 1;
    }

    the_soft_dfs_info = &(soft_thread->soft_dfs_info[depth]);


    dfs_max_depth = soft_thread->dfs_max_depth;
    test_index = the_soft_dfs_info->test_index;
    current_state_index = the_soft_dfs_info->current_state_index;
    ptr_state_with_locations = the_soft_dfs_info->state;
    derived_states_list = &(the_soft_dfs_info->derived_states_list);

    calculate_real_depth(ptr_state_with_locations);
    
    /*
        The main loop.
    */
    while (depth >= 0)
    {
        /*
            Increase the "maximal" depth if it about to be exceeded.
        */
        if (depth+1 >= dfs_max_depth)
        {
            freecell_solver_increase_dfs_max_depth(soft_thread);

            /* Because the address of soft_thread->soft_dfs_info may
             * be changed
             * */
            the_soft_dfs_info = &(soft_thread->soft_dfs_info[depth]);
            dfs_max_depth = soft_thread->dfs_max_depth;
            /* This too has to be re-synced */
            derived_states_list = &(the_soft_dfs_info->derived_states_list);
        }

        /* All the resultant states in the last test conducted were covered */
        if (current_state_index == derived_states_list->num_states)
        {
            if (test_index >= tests_order_num)
            {
                /* Backtrack to the previous depth. */

                if (is_a_complete_scan)
                {
                    ptr_state_with_locations->visited |= FCS_VISITED_ALL_TESTS_DONE;
                    mark_as_dead_end(ptr_state_with_locations);
                }

                depth--;

                if (check_if_limits_exceeded())
                {
                    the_soft_dfs_info->test_index = test_index;
                    the_soft_dfs_info->current_state_index = current_state_index;
                    myreturn(FCS_STATE_SUSPEND_PROCESS);
                }

                the_soft_dfs_info--;
                test_index = the_soft_dfs_info->test_index;
                current_state_index = the_soft_dfs_info->current_state_index;
                derived_states_list = &(the_soft_dfs_info->derived_states_list);
                ptr_state_with_locations = the_soft_dfs_info->state;
                continue; /* Just to make sure depth is not -1 now */
            }

            derived_states_list->num_states = 0;

            /* If this is the first test, then count the number of unoccupied
               freeceels and stacks and check if we are done. */
            if (test_index == 0)
            {
                int num_freestacks, num_freecells;

                if (instance->debug_iter_output)
                {
#ifdef DEBUG
                    printf("ST Name: %s\n", soft_thread->name);
#endif
                    instance->debug_iter_output_func(
                        (void*)instance->debug_iter_output_context,
                        instance->num_times,
                        depth,
                        (void*)instance,
                        ptr_state_with_locations,
                        ((depth == 0) ?
                            0 :
                            soft_thread->soft_dfs_info[depth-1].state->visited_iter
                        )
                        );
                }

                /* Count the free-cells */
                num_freecells = 0;
                for(a=0;a<freecells_num;a++)
                {
                    if (fcs_freecell_card_num(the_state, a) == 0)
                    {
                        num_freecells++;
                    }
                }

                /* Count the number of unoccupied stacks */

                num_freestacks = 0;
                for(a=0;a<stacks_num;a++)
                {
                    if (fcs_stack_len(the_state, a) == 0)
                    {
                        num_freestacks++;
                    }
                }

                /* Check if we have reached the empty state */
                if ((num_freestacks == stacks_num) && (num_freecells == freecells_num))
                {
                    instance->final_state = ptr_state_with_locations;

                    myreturn(FCS_STATE_WAS_SOLVED);
                }
                /*
                    Cache num_freecells and num_freestacks in their
                    appropriate stacks, so they won't be calculated over and over
                    again.
                  */
                the_soft_dfs_info->num_freecells = num_freecells;
                the_soft_dfs_info->num_freestacks = num_freestacks;
            }

            /* Always do the first test */
            do_first_iteration = 1;

            while (
                    /* Make sure we do not exceed the number of tests */
                    (test_index < tests_order_num) &&
                    (
                        /* Always do the first test */
                        do_first_iteration ||
                        (
                            /* This is a randomized scan. Else - quit after the first iteration */
                            to_randomize &&
                            /* We are still on a random group */
                            (tests_order_tests[ test_index ] & FCS_TEST_ORDER_FLAG_RANDOM) &&
                            /* A new random group did not start */
                            (! (tests_order_tests[ test_index ] & FCS_TEST_ORDER_FLAG_START_RANDOM_GROUP))
                         )
                    )
                 )
            {
                do_first_iteration = 0;

                check = freecell_solver_sfs_tests[tests_order_tests[
                        test_index
                    ] & FCS_TEST_ORDER_NO_FLAGS_MASK] (
                        soft_thread,
                        ptr_state_with_locations,
                        the_soft_dfs_info->num_freestacks,
                        the_soft_dfs_info->num_freecells,
                        derived_states_list,
                        to_reparent_states
                    );

                if ((check == FCS_STATE_BEGIN_SUSPEND_PROCESS) ||
                    (check == FCS_STATE_EXCEEDS_MAX_NUM_TIMES) ||
                    (check == FCS_STATE_SUSPEND_PROCESS))
                {
                    /* Have this test be re-performed */
                    derived_states_list->num_states = 0;
                    the_soft_dfs_info->current_state_index = 0;
                    the_soft_dfs_info->test_index = test_index;
                    myreturn(FCS_STATE_SUSPEND_PROCESS);
                }

                /* Move the counter to the next test */
                test_index++;
            }


            {
                int a, j;
                int swap_save;
                int * rand_array;
                int num_states = derived_states_list->num_states;

                if (num_states >
                        the_soft_dfs_info->derived_states_random_indexes_max_size)
                {
                    the_soft_dfs_info->derived_states_random_indexes_max_size =
                        num_states;
                    the_soft_dfs_info->derived_states_random_indexes =
                        realloc(
                            the_soft_dfs_info->derived_states_random_indexes,
                            sizeof(the_soft_dfs_info->derived_states_random_indexes[0]) * the_soft_dfs_info->derived_states_random_indexes_max_size
                            );
                }
                rand_array = the_soft_dfs_info->derived_states_random_indexes;

                for(a=0 ; a < num_states ; a++)
                {
                    rand_array[a] = a;
                }
                /* If we just conducted the tests for a random group -
                 * randomize. Else - keep those indexes as the unity vector.
                 *
                 * Also, do not randomize if this is a pure soft-DFS scan.
                 * */
                if (to_randomize && tests_order_tests[ test_index-1 ] & FCS_TEST_ORDER_FLAG_RANDOM)
                {
                    a = num_states-1;
                    while (a > 0)
                    {
                        j =
                            (
                                freecell_solver_rand_get_random_number(
                                soft_thread->rand_gen
                            )
                            % (a+1)
                        );

                        swap_save = rand_array[a];
                        rand_array[a] = rand_array[j];
                        rand_array[j] = swap_save;
                        a--;
                    }
                }
            }

            /* We just performed a test, so the index of the first state that
               ought to be checked in this depth is 0.
               */
            current_state_index = 0;
        }

        {
            int num_states = derived_states_list->num_states;
            fcs_state_with_locations_t * * derived_states = derived_states_list->states;
            int * rand_array = the_soft_dfs_info->derived_states_random_indexes;

            while (current_state_index <
                   num_states)
            {
                ptr_recurse_into_state_with_locations =
                    (derived_states[
                        rand_array[
                            current_state_index
                        ]
                    ]);

                current_state_index++;
                if (
                    (! (ptr_recurse_into_state_with_locations->visited &
                        FCS_VISITED_DEAD_END)
                    ) &&
                    (! is_scan_visited(
                        ptr_recurse_into_state_with_locations,
                        soft_thread_id)
                    )
                   )
                {
                    instance->num_times++;
                    hard_thread->num_times++;

                    the_soft_dfs_info->test_index = test_index;
                    the_soft_dfs_info->current_state_index = current_state_index;

                    set_scan_visited(ptr_recurse_into_state_with_locations, soft_thread_id);

                    ptr_recurse_into_state_with_locations->visited_iter = instance->num_times;
#if 0
                    ptr_recurse_into_state_with_locations->parent = ptr_state_with_locations;
#endif

                    /*
                        I'm using current_state_indexes[depth]-1 because we already
                        increased it by one, so now it refers to the next state.
                    */
                    depth++;
                    the_soft_dfs_info++;
                    the_soft_dfs_info->state = 
                        ptr_state_with_locations = 
                        ptr_recurse_into_state_with_locations;
                    test_index = 0;
                    current_state_index = 0;
                    derived_states_list = &(the_soft_dfs_info->derived_states_list);
                    derived_states_list->num_states = 0;

                    calculate_real_depth(ptr_recurse_into_state_with_locations);

                    break;
                }
            }
        }
    }

    soft_thread->num_solution_states = 0;

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


#undef state
#undef myreturn

#define FCS_A_STAR_CARDS_UNDER_SEQUENCES_EXPONENT 1.3
#define FCS_A_STAR_SEQS_OVER_RENEGADE_CARDS_EXPONENT 1.3

#define state (ptr_state_with_locations->s)

void freecell_solver_a_star_initialize_rater(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations
    )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    int a, c, cards_num;
    fcs_card_t this_card, prev_card;
    double cards_under_sequences;
    int sequences_are_built_by = instance->sequences_are_built_by;


    cards_under_sequences = 0;
    for(a=0;a<instance->stacks_num;a++)
    {
        cards_num = fcs_stack_len(state, a);
        if (cards_num <= 1)
        {
            continue;
        }

        c = cards_num-2;
        this_card = fcs_stack_card(state, a, c+1);
        prev_card = fcs_stack_card(state, a, c);
        while (fcs_is_parent_card(this_card,prev_card) && (c >= 0))
        {
            c--;
            this_card = prev_card;
            if (c>=0)
            {
                prev_card = fcs_stack_card(state, a, c);
            }
        }
        cards_under_sequences += pow(c+1, FCS_A_STAR_CARDS_UNDER_SEQUENCES_EXPONENT);
    }
    soft_thread->a_star_initial_cards_under_sequences = cards_under_sequences;
}


static pq_rating_t freecell_solver_a_star_rate_state(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations
    )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    double ret=0;
    int a, c, cards_num, num_cards_in_founds;
    int num_freestacks, num_freecells;
    fcs_card_t this_card, prev_card;
    double cards_under_sequences, temp;
    double seqs_over_renegade_cards;
    int sequences_are_built_by = instance->sequences_are_built_by;
    int freecells_num = instance->freecells_num;
    int stacks_num = instance->stacks_num;
    double * a_star_weights = soft_thread->a_star_weights;
    int unlimited_sequence_move = instance->unlimited_sequence_move;
    int decks_num = instance->decks_num;

    cards_under_sequences = 0;
    num_freestacks = 0;
    seqs_over_renegade_cards = 0;
    for(a=0;a<stacks_num;a++)
    {
        cards_num = fcs_stack_len(state, a);
        if (cards_num == 0)
        {
            num_freestacks++;
        }

        if (cards_num <= 1)
        {
            continue;
        }

        c = cards_num-2;
        this_card = fcs_stack_card(state, a, c+1);
        prev_card = fcs_stack_card(state, a, c);
        while ((c >= 0) && fcs_is_parent_card(this_card,prev_card))
        {
            c--;
            this_card = prev_card;
            if (c>=0)
            {
                prev_card = fcs_stack_card(state, a, c);
            }
        }
        cards_under_sequences += pow(c+1, FCS_A_STAR_CARDS_UNDER_SEQUENCES_EXPONENT);
        if (c >= 0)
        {
            seqs_over_renegade_cards +=
                ((unlimited_sequence_move) ?
                    1 :
                    pow(cards_num-c-1, FCS_A_STAR_SEQS_OVER_RENEGADE_CARDS_EXPONENT)
                    );
        }
    }

    ret += ((soft_thread->a_star_initial_cards_under_sequences - cards_under_sequences)
            / soft_thread->a_star_initial_cards_under_sequences) * a_star_weights[FCS_A_STAR_WEIGHT_CARDS_UNDER_SEQUENCES];

    ret += (seqs_over_renegade_cards /
               pow(decks_num*52, FCS_A_STAR_SEQS_OVER_RENEGADE_CARDS_EXPONENT) )
           * a_star_weights[FCS_A_STAR_WEIGHT_SEQS_OVER_RENEGADE_CARDS];

    num_cards_in_founds = 0;
    for(a=0;a<(decks_num<<2);a++)
    {
        num_cards_in_founds += fcs_foundation_value(state, a);
    }

    ret += ((double)num_cards_in_founds/(decks_num*52)) * a_star_weights[FCS_A_STAR_WEIGHT_CARDS_OUT];

    num_freecells = 0;
    for(a=0;a<freecells_num;a++)
    {
        if (fcs_freecell_card_num(state,a) == 0)
        {
            num_freecells++;
        }
    }

    if (instance->empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)
    {
        if (unlimited_sequence_move)
        {
            temp = (((double)num_freecells+num_freestacks)/(freecells_num+instance->stacks_num));
        }
        else
        {
            temp = (((double)((num_freecells+1)<<num_freestacks)) / ((freecells_num+1)<<(instance->stacks_num)));
        }
    }
    else
    {
        if (unlimited_sequence_move)
        {
            temp = (((double)num_freecells)/freecells_num);
        }
        else
        {
            temp = 0;
        }
    }

    ret += (temp * a_star_weights[FCS_A_STAR_WEIGHT_MAX_SEQUENCE_MOVE]);

    if (ptr_state_with_locations->depth <= 20000)
    {
        ret += ((20000 - ptr_state_with_locations->depth)/20000.0) * a_star_weights[FCS_A_STAR_WEIGHT_DEPTH];
    }

    return (int)(ret*INT_MAX);
}




/*
    freecell_solver_a_star_or_bfs_do_solve_or_resume() is the main event
    loop of the A* And BFS scans. It is quite simple as all it does is
    extract elements out of the queue or priority queue and run all the test
    of them.

    It goes on in this fashion until the final state was reached or
    there are no more states in the queue.
*/

#define myreturn(ret_value)                                     \
    /* Free the memory that was allocated by the                \
     * derived states list */                                   \
    if (derived.states != NULL)                                 \
    {                                                           \
        free(derived.states);                                   \
    }                                                           \
                                                                \
    soft_thread->bfs_queue_last_item = bfs_queue_last_item;     \
                                                                \
    return (ret_value);


int freecell_solver_a_star_or_bfs_do_solve_or_resume(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations_orig,
    int resume
    )
{
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    fcs_state_with_locations_t * ptr_state_with_locations;
    int num_freestacks, num_freecells;
    fcs_states_linked_list_item_t * save_item;
    int a;
    int check;
    fcs_derived_states_list_t derived;
    int derived_index;

    int method;
    int freecells_num, stacks_num;
    int tests_order_num;
    int * tests_order_tests;
    int calc_real_depth = instance->calc_real_depth;
    int soft_thread_id = soft_thread->id;
    int is_a_complete_scan = soft_thread->is_a_complete_scan;
    int to_reparent_states = 
        (instance->to_reparent_states || 
         (soft_thread->method == FCS_METHOD_OPTIMIZE)
        );
    int scans_synergy = instance->scans_synergy;
    fcs_states_linked_list_item_t * bfs_queue = soft_thread->bfs_queue;
    PQUEUE * a_star_pqueue = soft_thread->a_star_pqueue;
    fcs_states_linked_list_item_t * bfs_queue_last_item = soft_thread->bfs_queue_last_item;

    derived.num_states = 0;
    derived.max_num_states = 0;
    derived.states = NULL;

    tests_order_num = soft_thread->tests_order.num;
    tests_order_tests = soft_thread->tests_order.tests;

    if (!resume)
    {
        /* Initialize the first element to indicate it is the first */
        ptr_state_with_locations_orig->parent = NULL;
        ptr_state_with_locations_orig->moves_to_parent = NULL;
        ptr_state_with_locations_orig->depth = 0;
    }

    ptr_state_with_locations = ptr_state_with_locations_orig;

    method = soft_thread->method;
    freecells_num = instance->freecells_num;
    stacks_num = instance->stacks_num;

    /* Continue as long as there are states in the queue or
       priority queue. */
    while ( ptr_state_with_locations != NULL)
    {
        /*
         * If this is an optimization scan and the state being checked is not
         * in the original solution path - move on to the next state
         * */
        if ((method == FCS_METHOD_OPTIMIZE) && (!(ptr_state_with_locations->visited & FCS_VISITED_IN_SOLUTION_PATH)))
        {
            goto label_next_state;
        }

        /*
         * It the state has already been visited - move on to the next
         * state.
         * */
        if ((method == FCS_METHOD_OPTIMIZE) ?
                (ptr_state_with_locations->visited & FCS_VISITED_IN_OPTIMIZED_PATH) :
                ((ptr_state_with_locations->visited & FCS_VISITED_DEAD_END) ||
                 (is_scan_visited(ptr_state_with_locations, soft_thread_id)))
                )
        {
            goto label_next_state;
        }

        /* Count the free-cells */
        num_freecells = 0;
        for(a=0;a<freecells_num;a++)
        {
            if (fcs_freecell_card_num(state, a) == 0)
            {
                num_freecells++;
            }
        }

        /* Count the number of unoccupied stacks */

        num_freestacks = 0;
        for(a=0;a<stacks_num;a++)
        {
            if (fcs_stack_len(state, a) == 0)
            {
                num_freestacks++;
            }
        }

        if ((instance->debug_iter_output) && (!resume))
        {
#ifdef DEBUG
            printf("ST Name: %s\n", soft_thread->name);
#endif
            instance->debug_iter_output_func(
                    (void*)instance->debug_iter_output_context,
                    instance->num_times,
                    ptr_state_with_locations->depth,
                    (void*)instance,
                    ptr_state_with_locations,
                    ((ptr_state_with_locations->parent == NULL) ?
                        0 :
                        ptr_state_with_locations->parent->visited_iter
                    )
                    );
        }


        if ((num_freestacks == stacks_num) && (num_freecells == freecells_num))
        {
            instance->final_state = ptr_state_with_locations;

            myreturn(FCS_STATE_WAS_SOLVED);
        }

        calculate_real_depth(ptr_state_with_locations);

        /* Do all the tests at one go, because that the way it should be
           done for BFS and A*
        */
        derived.num_states = 0;
        for(a=0 ;
            a < tests_order_num;
            a++)
        {
            check = freecell_solver_sfs_tests[tests_order_tests[a] & FCS_TEST_ORDER_NO_FLAGS_MASK] (
                    soft_thread,
                    ptr_state_with_locations,
                    num_freestacks,
                    num_freecells,
                    &derived,
                    /*
                     * We want to reparent the new states, only if this
                     * is an optimization scan.
                     * */
                    to_reparent_states
                    );
            if ((check == FCS_STATE_BEGIN_SUSPEND_PROCESS) ||
                (check == FCS_STATE_EXCEEDS_MAX_NUM_TIMES) ||
                (check == FCS_STATE_SUSPEND_PROCESS))
            {
                /* Save the current position in the scan */
                soft_thread->first_state_to_check = ptr_state_with_locations;

                myreturn(FCS_STATE_SUSPEND_PROCESS);
            }
        }

        if (check_if_limits_exceeded())
                    
        {
            soft_thread->first_state_to_check = ptr_state_with_locations;

            myreturn(FCS_STATE_SUSPEND_PROCESS);
        }
        

        if (is_a_complete_scan)
        {
            ptr_state_with_locations->visited |= FCS_VISITED_ALL_TESTS_DONE;
        }

        /* Increase the number of iterations by one . 
         * */
        {
            instance->num_times++;
            hard_thread->num_times++;
        }

        /* Insert all the derived states into the PQ or Queue */

        for(derived_index = 0 ; derived_index < derived.num_states ; derived_index++)
        {
            if (method == FCS_METHOD_A_STAR)
            {
                freecell_solver_a_star_enqueue_state(
                    soft_thread,
                    derived.states[derived_index]
                    );
            }
            else
            {
                freecell_solver_bfs_enqueue_state(
                    soft_thread,
                    derived.states[derived_index]
                    );
            }
        }

        if (method == FCS_METHOD_OPTIMIZE)
        {
            ptr_state_with_locations->visited |= FCS_VISITED_IN_OPTIMIZED_PATH;
        }
        else
        {
            set_scan_visited(ptr_state_with_locations, soft_thread_id);

            if (derived.num_states == 0)
            {
                if (is_a_complete_scan)
                {
                    mark_as_dead_end(ptr_state_with_locations);
                }
            }
        }

        ptr_state_with_locations->visited_iter = instance->num_times-1;

label_next_state:

        /*
            Extract the next item in the queue/priority queue.
        */
        if ((method == FCS_METHOD_BFS) || (method == FCS_METHOD_OPTIMIZE))
        {
            save_item = bfs_queue->next;
            if (save_item != bfs_queue_last_item)
            {
                ptr_state_with_locations = save_item->s;
                bfs_queue->next = save_item->next;
                free(save_item);
            }
            else
            {
                ptr_state_with_locations = NULL;
            }
        }
        else
        {
            /* It is an A* scan */
            ptr_state_with_locations = freecell_solver_PQueuePop(a_star_pqueue);
        }
        resume = 0;
    }

    myreturn(FCS_STATE_IS_NOT_SOLVEABLE);
}

#undef myreturn

#undef state
