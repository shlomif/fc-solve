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
 * scans.c - The code that relates to the various scans.
 * Currently Hard DFS, Soft-DFS, Random-DFS, A* and BFS are implemented.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>

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

static pq_rating_t fc_solve_a_star_rate_state(
    fc_solve_soft_thread_t * soft_thread,
    fcs_state_extra_info_t * ptr_state_val
    );

#define fc_solve_bfs_enqueue_state(soft_thread, state_val) \
    {    \
        fcs_states_linked_list_item_t * last_item_next;      \
        last_item_next = bfs_queue_last_item->next = (fcs_states_linked_list_item_t*)malloc(sizeof(fcs_states_linked_list_item_t));      \
        bfs_queue_last_item->s = state_val;     \
        last_item_next->next = NULL;     \
        bfs_queue_last_item = last_item_next; \
    }

static void fc_solve_increase_dfs_max_depth(
    fc_solve_soft_thread_t * soft_thread
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
        soft_thread->soft_dfs_info[d].state_val = NULL;
        soft_thread->soft_dfs_info[d].derived_states_list.max_num_states = 0;
        soft_thread->soft_dfs_info[d].test_index = 0;
        soft_thread->soft_dfs_info[d].current_state_index = 0;
        soft_thread->soft_dfs_info[d].derived_states_list.num_states = 0;
        soft_thread->soft_dfs_info[d].derived_states_list.states = NULL;
        soft_thread->soft_dfs_info[d].derived_states_random_indexes = NULL;
        soft_thread->soft_dfs_info[d].derived_states_random_indexes_max_size = 0;
        soft_thread->soft_dfs_info[d].positions_by_rank = NULL;
    }

    soft_thread->dfs_max_depth = new_dfs_max_depth;
}

void fc_solve_derived_states_list_add_state(
        fcs_derived_states_list_t * list,
        fcs_state_extra_info_t * state_val,
        int context
        )
{
    if ((list)->num_states == (list)->max_num_states)
    {
        (list)->max_num_states += 16;
        (list)->states = realloc((list)->states, sizeof((list)->states[0]) * (list)->max_num_states);
    }
    (list)->states[(list)->num_states].state_ptr = state_val;
    (list)->states[(list)->num_states++].context.i = context;
}

/*
    fc_solve_soft_dfs_do_solve is the event loop of the
    Random-DFS scan. DFS which is recursive in nature is handled here
    without procedural recursion
    by using some dedicated stacks for the traversal.
  */
#define the_state (*ptr_state_key)

#define myreturn(ret_value) \
    return (ret_value);

#ifdef DEBUG
#define TRACE0(message) \
        { \
            if (getenv("FCS_TRACE")) \
            { \
            printf("%s. Depth=%d ; the_soft_Depth=%d ; Iters=%d ; test_index=%d ; current_state_index=%d ; num_states=%d\n", \
                    message, \
                    soft_thread->depth, (the_soft_dfs_info-soft_thread->soft_dfs_info), \
                    instance->num_times, the_soft_dfs_info->test_index, \
                    the_soft_dfs_info->current_state_index, \
                    (derived_states_list ? derived_states_list->num_states : -1) \
                    );  \
            fflush(stdout); \
            } \
        }
#else
#define TRACE0(no_use) {}
#endif

void fc_solve_soft_thread_init_soft_dfs(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_instance_t * instance = soft_thread->hard_thread->instance;

    fcs_state_extra_info_t * ptr_orig_state_val = instance->state_copy_ptr_val;
    /*
        Allocate some space for the states at depth 0.
    */
    soft_thread->depth = 0;

    fc_solve_increase_dfs_max_depth(soft_thread);

    /* Initialize the initial state to indicate it is the first */
    ptr_orig_state_val->parent_val = NULL;
    ptr_orig_state_val->moves_to_parent = NULL;
    ptr_orig_state_val->depth = 0;

    soft_thread->soft_dfs_info[0].state_val = ptr_orig_state_val;

    return;
}

int fc_solve_soft_dfs_do_solve(
    fc_solve_soft_thread_t * soft_thread,
    int to_randomize
    )
{
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;

    fcs_state_t * ptr_state_key;
    fcs_state_extra_info_t * ptr_state_val;
    int a;
    int check;
    int do_first_iteration;
    fcs_soft_dfs_stack_item_t * the_soft_dfs_info;
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    int dfs_max_depth;

    int tests_order_num = soft_thread->tests_order.num;
    int * tests_order_tests = soft_thread->tests_order.tests;
    int calc_real_depth = instance->calc_real_depth;
    int is_a_complete_scan = soft_thread->is_a_complete_scan;
    int soft_thread_id = soft_thread->id;
    fcs_derived_states_list_t * derived_states_list;
    int to_reparent_states, scans_synergy;

#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif
    to_reparent_states = instance->to_reparent_states_real;
    scans_synergy = instance->scans_synergy;

    the_soft_dfs_info = &(soft_thread->soft_dfs_info[soft_thread->depth]);

    dfs_max_depth = soft_thread->dfs_max_depth;
    ptr_state_val = the_soft_dfs_info->state_val;
    derived_states_list = &(the_soft_dfs_info->derived_states_list);
    
    calculate_real_depth(
        ptr_state_val
    );

    TRACE0("Before depth loop");
    /*
        The main loop.
    */
    while (soft_thread->depth >= 0)
    {
        /*
            Increase the "maximal" depth if it is about to be exceeded.
        */
        if (soft_thread->depth+1 >= dfs_max_depth)
        {
            fc_solve_increase_dfs_max_depth(soft_thread);

            /* Because the address of soft_thread->soft_dfs_info may
             * be changed
             * */
            the_soft_dfs_info = &(soft_thread->soft_dfs_info[soft_thread->depth]);
            dfs_max_depth = soft_thread->dfs_max_depth;
            /* This too has to be re-synced */
            derived_states_list = &(the_soft_dfs_info->derived_states_list);
        }

        TRACE0("Before current_state_index check");
        /* All the resultant states in the last test conducted were covered */
        if (the_soft_dfs_info->current_state_index ==
            derived_states_list->num_states
           )
        {
            if (the_soft_dfs_info->test_index >= tests_order_num)
            {
                /* Backtrack to the previous depth. */

                if (is_a_complete_scan)
                {
                    ptr_state_val->visited |= FCS_VISITED_ALL_TESTS_DONE;
                    mark_as_dead_end(
                        ptr_state_val
                    );
                }

                free(the_soft_dfs_info->positions_by_rank);
                if (--soft_thread->depth < 0)
                {
                    break;
                }
                else
                {
                    the_soft_dfs_info--;
                    derived_states_list = &(the_soft_dfs_info->derived_states_list);
                    ptr_state_val = the_soft_dfs_info->state_val;
                    ptr_state_key = ptr_state_val->key;
                    soft_thread->num_vacant_freecells = the_soft_dfs_info->num_vacant_freecells;
                    soft_thread->num_vacant_stacks = the_soft_dfs_info->num_vacant_stacks;
                }

                continue; /* Just to make sure depth is not -1 now */
            }

            derived_states_list->num_states = 0;

            TRACE0("Before iter_handler");
            /* If this is the first test, then count the number of unoccupied
               freeceels and stacks and check if we are done. */
            if (the_soft_dfs_info->test_index == 0)
            {
                int num_vacant_stacks, num_vacant_freecells;

                TRACE0("In iter_handler");

                if (instance->debug_iter_output)
                {
#ifdef DEBUG
                    printf("ST Name: %s\n", soft_thread->name);
#endif
                    instance->debug_iter_output_func(
                        (void*)instance->debug_iter_output_context,
                        instance->num_times,
                        soft_thread->depth,
                        (void*)instance,
                        ptr_state_val,
                        ((soft_thread->depth == 0) ?
                            0 :
                            soft_thread->soft_dfs_info[soft_thread->depth-1].state_val->visited_iter
                        )
                        );
                }

                ptr_state_key = ptr_state_val->key;

                /* Count the free-cells */
                num_vacant_freecells = 0;
                for(a=0;a<LOCAL_FREECELLS_NUM;a++)
                {
                    if (fcs_freecell_card_num(the_state, a) == 0)
                    {
                        num_vacant_freecells++;
                    }
                }

                /* Count the number of unoccupied stacks */

                num_vacant_stacks = 0;
                for(a=0;a<LOCAL_STACKS_NUM;a++)
                {
                    if (fcs_col_len(fcs_state_get_col(the_state, a)) == 0)
                    {
                        num_vacant_stacks++;
                    }
                }

                /* Check if we have reached the empty state */
                if ((num_vacant_stacks == LOCAL_STACKS_NUM) &&
                    (num_vacant_freecells  == LOCAL_FREECELLS_NUM))
                {
                    instance->final_state_val = ptr_state_val;

                    TRACE0("Returning FCS_STATE_WAS_SOLVED");
                    myreturn(FCS_STATE_WAS_SOLVED);
                }
                /*
                    Cache num_vacant_freecells and num_vacant_stacks in their
                    appropriate stacks, so they won't be calculated over and over
                    again.
                  */
                soft_thread->num_vacant_freecells =
                    the_soft_dfs_info->num_vacant_freecells =
                    num_vacant_freecells;
                soft_thread->num_vacant_stacks =
                    the_soft_dfs_info->num_vacant_stacks =
                    num_vacant_stacks;
            }

            TRACE0("After iter_handler");
            /* Always do the first test */
            do_first_iteration = 1;

            while (
                    /* Make sure we do not exceed the number of tests */
                    (the_soft_dfs_info->test_index < tests_order_num) &&
                    (
                        /* Always do the first test */
                        do_first_iteration ||
                        (
                            /* This is a randomized scan. Else - quit after the first iteration */
                            to_randomize &&
                            /* We are still on a random group */
                            (tests_order_tests[ the_soft_dfs_info->test_index ] & FCS_TEST_ORDER_FLAG_RANDOM) &&
                            /* A new random group did not start */
                            (! (tests_order_tests[ the_soft_dfs_info->test_index ] & FCS_TEST_ORDER_FLAG_START_RANDOM_GROUP))
                         )
                    )
                 )
            {
                do_first_iteration = 0;

                check = fc_solve_sfs_tests[tests_order_tests[
                        the_soft_dfs_info->test_index
                    ] & FCS_TEST_ORDER_NO_FLAGS_MASK] (
                        soft_thread,
                        ptr_state_val,
                        derived_states_list
                    );

                if ((check == FCS_STATE_BEGIN_SUSPEND_PROCESS) ||
                    (check == FCS_STATE_EXCEEDS_MAX_NUM_TIMES) ||
                    (check == FCS_STATE_SUSPEND_PROCESS))
                {
                    /* Have this test be re-performed */
                    derived_states_list->num_states = 0;
                    the_soft_dfs_info->current_state_index = 0;
                    TRACE0("Returning FCS_STATE_SUSPEND_PROCESS (after sfs_tests)");
                    myreturn(FCS_STATE_SUSPEND_PROCESS);
                }

                /* Move the counter to the next test */
                the_soft_dfs_info->test_index++;
            }


            {
                int a, j;
                int swap_save;
                int * rand_array, * ra_ptr;
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

                for(a=0, ra_ptr = rand_array; a < num_states ; a++)
                {
                    *(ra_ptr++) = a;
                }
                /* If we just conducted the tests for a random group -
                 * randomize. Else - keep those indexes as the unity vector.
                 *
                 * Also, do not randomize if this is a pure soft-DFS scan.
                 * */
                if (to_randomize && tests_order_tests[ the_soft_dfs_info->test_index-1 ] & FCS_TEST_ORDER_FLAG_RANDOM)
                {
                    a = num_states-1;
                    while (a > 0)
                    {
                        j =
                            (
                                fc_solve_rand_get_random_number(
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
            the_soft_dfs_info->current_state_index = 0;
        }

        {
            int num_states = derived_states_list->num_states;
            fcs_derived_states_list_item_t * derived_states =
                derived_states_list->states;
            int * rand_array = the_soft_dfs_info->derived_states_random_indexes;
            fcs_state_extra_info_t * single_derived_state;

            while (the_soft_dfs_info->current_state_index <
                   num_states)
            {
                single_derived_state = derived_states[
                        rand_array[
                            the_soft_dfs_info->current_state_index++
                        ]
                    ].state_ptr;

                if (
                    (! (single_derived_state->visited &
                        FCS_VISITED_DEAD_END)
                    ) &&
                    (! is_scan_visited(
                        single_derived_state,
                        soft_thread_id)
                    )
                   )
                {
                    instance->num_times++;
                    hard_thread->num_times++;

                    set_scan_visited(
                        single_derived_state,
                        soft_thread_id
                    );

                    single_derived_state->visited_iter = instance->num_times;

                    /*
                        I'm using current_state_indexes[depth]-1 because we already
                        increased it by one, so now it refers to the next state.
                    */
                    soft_thread->depth++;
                    the_soft_dfs_info++;

                    the_soft_dfs_info->state_val =
                        ptr_state_val =
                        single_derived_state;

                    the_soft_dfs_info->test_index = 0;
                    the_soft_dfs_info->current_state_index = 0;
                    the_soft_dfs_info->positions_by_rank = NULL;
                    derived_states_list = &(the_soft_dfs_info->derived_states_list);
                    derived_states_list->num_states = 0;

                    calculate_real_depth(
                        ptr_state_val
                    );

                    if (check_if_limits_exceeded())
                    {
                        TRACE0("Returning FCS_STATE_SUSPEND_PROCESS (inside current_state_index)");
                        myreturn(FCS_STATE_SUSPEND_PROCESS);
                    }

                    break;
                }
            }
        }
    }

    /*
     * We need to bump the number of iterations so it will be ready with
     * a fresh iterations number for the next scan that takes place.
     * */
    instance->num_times++;
    hard_thread->num_times++;

    soft_thread->depth = -1;

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


#undef state
#undef myreturn

#define FCS_A_STAR_CARDS_UNDER_SEQUENCES_EXPONENT 1.3
#define FCS_A_STAR_SEQS_OVER_RENEGADE_CARDS_EXPONENT 1.3

static void initialize_a_star_rater(
    fc_solve_soft_thread_t * soft_thread,
    fcs_state_extra_info_t * ptr_state_val
    )
{
#ifndef HARD_CODED_NUM_STACKS
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;
#endif
    fcs_state_t * ptr_state_key = ptr_state_val->key;

    int a, c, cards_num;
    fcs_card_t this_card, prev_card;
    double cards_under_sequences;
    fcs_cards_column_t col;
#ifndef FCS_FREECELL_ONLY
    int sequences_are_built_by = instance->sequences_are_built_by;
#endif

    cards_under_sequences = 0;
    for(a=0;a<INSTANCE_STACKS_NUM;a++)
    {
        col = fcs_state_get_col(*ptr_state_key, a);
        cards_num = fcs_col_len(col);
        if (cards_num <= 1)
        {
            continue;
        }

        c = cards_num-2;
        this_card = fcs_col_get_card(col, c+1);
        prev_card = fcs_col_get_card(col, c);
        while (fcs_is_parent_card(this_card,prev_card) && (c >= 0))
        {
            c--;
            this_card = prev_card;
            if (c>=0)
            {
                prev_card = fcs_col_get_card(col, c);
            }
        }
        cards_under_sequences += pow(c+1, FCS_A_STAR_CARDS_UNDER_SEQUENCES_EXPONENT);
    }
    soft_thread->a_star_initial_cards_under_sequences = cards_under_sequences;
}

#undef TRACE0

#ifdef DEBUG

#define TRACE0(message) \
        { \
            if (getenv("FCS_TRACE")) \
            { \
            printf("BestFS(rate_state) - %s ; rating=%.40f .\n", \
                    message, \
                    ret \
                    );  \
            fflush(stdout); \
            } \
        }

#else

#define TRACE0(no_use) {}

#endif

static pq_rating_t fc_solve_a_star_rate_state(
    fc_solve_soft_thread_t * soft_thread,
    fcs_state_extra_info_t * ptr_state_val
    )
{
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;
    fcs_state_t * ptr_state_key = ptr_state_val->key;

    double ret=0;
    int a, c, cards_num, num_cards_in_founds;
    int num_vacant_stacks, num_vacant_freecells;
    fcs_card_t this_card, prev_card;
    double cards_under_sequences, temp;
    double seqs_over_renegade_cards;
    fcs_cards_column_t col;
#ifndef FCS_FREECELL_ONLY
    int sequences_are_built_by = instance->sequences_are_built_by;
#endif
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num = instance->freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num = instance->stacks_num;
#endif
    double * a_star_weights = soft_thread->a_star_weights;
#ifndef FCS_FREECELL_ONLY
    int unlimited_sequence_move = instance->unlimited_sequence_move;
#else
    #define unlimited_sequence_move 0
#endif
#ifndef HARD_CODED_NUM_DECKS
    int decks_num = instance->decks_num;
#endif

    cards_under_sequences = 0;
    num_vacant_stacks = 0;
    seqs_over_renegade_cards = 0;
    for(a=0;a<LOCAL_STACKS_NUM;a++)
    {
        col = fcs_state_get_col(*(ptr_state_key), a);
        cards_num = fcs_col_len(col);

        if (cards_num == 0)
        {
            num_vacant_stacks++;
        }

        if (cards_num <= 1)
        {
            continue;
        }

        c = cards_num-2;
        this_card = fcs_col_get_card(col, c+1);
        prev_card = fcs_col_get_card(col, c);
        while ((c >= 0) && fcs_is_parent_card(this_card,prev_card))
        {
            c--;
            this_card = prev_card;
            if (c>=0)
            {
                prev_card = fcs_col_get_card(col, c);
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
               pow(LOCAL_DECKS_NUM*52, FCS_A_STAR_SEQS_OVER_RENEGADE_CARDS_EXPONENT) )
           * a_star_weights[FCS_A_STAR_WEIGHT_SEQS_OVER_RENEGADE_CARDS];

    num_cards_in_founds = 0;
    for(a=0;a<(LOCAL_DECKS_NUM<<2);a++)
    {
        num_cards_in_founds += fcs_foundation_value((*ptr_state_key), a);
    }

    ret += ((double)num_cards_in_founds/(LOCAL_DECKS_NUM*52)) * a_star_weights[FCS_A_STAR_WEIGHT_CARDS_OUT];

    num_vacant_freecells = 0;
    for(a=0;a<LOCAL_FREECELLS_NUM;a++)
    {
        if (fcs_freecell_card_num((*ptr_state_key),a) == 0)
        {
            num_vacant_freecells++;
        }
    }

    if (instance->empty_stacks_fill == FCS_ES_FILLED_BY_ANY_CARD)
    {
        if (unlimited_sequence_move)
        {
            temp = (((double)num_vacant_freecells+num_vacant_stacks)/(LOCAL_FREECELLS_NUM+INSTANCE_STACKS_NUM));
        }
        else
        {
            temp = (((double)((num_vacant_freecells+1)<<num_vacant_stacks)) / ((LOCAL_FREECELLS_NUM+1)<<(INSTANCE_STACKS_NUM)));
        }
    }
    else
    {
        if (unlimited_sequence_move)
        {
            temp = (((double)num_vacant_freecells)/LOCAL_FREECELLS_NUM);
        }
        else
        {
            temp = 0;
        }
    }

    ret += (temp * a_star_weights[FCS_A_STAR_WEIGHT_MAX_SEQUENCE_MOVE]);

    if (ptr_state_val->depth <= 20000)
    {
        ret += ((20000 - ptr_state_val->depth)/20000.0) * a_star_weights[FCS_A_STAR_WEIGHT_DEPTH];
    }

    TRACE0("Before return");

    return (int)(ret*INT_MAX);
}

#ifdef FCS_FREECELL_ONLY
#undef unlimited_sequence_move
#endif

/*
    fc_solve_a_star_or_bfs_do_solve() is the main event
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
    if (soft_thread->a_star_positions_by_rank)                  \
    {                                                           \
        free(soft_thread->a_star_positions_by_rank);            \
        soft_thread->a_star_positions_by_rank = NULL;           \
    }                                                           \
                                                                \
    return (ret_value);


#undef TRACE0

#ifdef DEBUG

#define TRACE0(message) \
        { \
            if (getenv("FCS_TRACE")) \
            { \
            printf("BestFS - %s ; Iters=%d.\n", \
                    message, \
                    instance->num_times \
                    );  \
            fflush(stdout); \
            } \
        }

#else

#define TRACE0(no_use) {}

#endif

extern void fc_solve_soft_thread_init_a_star_or_bfs(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_instance_t * instance = soft_thread->hard_thread->instance;

    fcs_state_extra_info_t * ptr_orig_state_val = instance->state_copy_ptr_val;

    if (soft_thread->method == FCS_METHOD_A_STAR)
    {
        initialize_a_star_rater(
            soft_thread,
            ptr_orig_state_val
            );
    }

    /* Initialize the first element to indicate it is the first */
    ptr_orig_state_val->parent_val = NULL;
    ptr_orig_state_val->moves_to_parent = NULL;
    ptr_orig_state_val->depth = 0;

    soft_thread->first_state_to_check_val = ptr_orig_state_val;

    return;
}

int fc_solve_a_star_or_bfs_do_solve(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;

    fcs_state_t * ptr_state_key;
    fcs_state_extra_info_t * ptr_state_val, * ptr_new_state_val;
    int num_vacant_stacks, num_vacant_freecells;
    fcs_states_linked_list_item_t * save_item;
    int a;
    int check;
    fcs_derived_states_list_t derived;
    int derived_index;

    int method;
#ifndef HARD_CODED_NUM_FREECELLS
    int freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    int stacks_num;
#endif
    int tests_order_num;
    int * tests_order_tests;
    int calc_real_depth = instance->calc_real_depth;
    int soft_thread_id = soft_thread->id;
    int is_a_complete_scan = soft_thread->is_a_complete_scan;

    int scans_synergy = instance->scans_synergy;
    fcs_states_linked_list_item_t * bfs_queue = soft_thread->bfs_queue;
    PQUEUE * a_star_pqueue = soft_thread->a_star_pqueue;
    fcs_states_linked_list_item_t * bfs_queue_last_item = soft_thread->bfs_queue_last_item;

    derived.num_states = 0;
    derived.max_num_states = 0;
    derived.states = NULL;

    tests_order_num = soft_thread->tests_order.num;
    tests_order_tests = soft_thread->tests_order.tests;

    ptr_state_val = soft_thread->first_state_to_check_val;

    method = soft_thread->method;
#ifndef HARD_CODED_NUM_FREECELLS
    freecells_num = instance->freecells_num;
#endif

#ifndef HARD_CODED_NUM_STACKS
    stacks_num = instance->stacks_num;
#endif

    /* Continue as long as there are states in the queue or
       priority queue. */
    while ( ptr_state_val != NULL)
    {
         TRACE0("Start of loop");        /*
         * If this is an optimization scan and the state being checked is not
         * in the original solution path - move on to the next state
         * */
        if ((method == FCS_METHOD_OPTIMIZE) && (!(ptr_state_val->visited & FCS_VISITED_IN_SOLUTION_PATH)))
        {
            goto label_next_state;
        }

        /*
         * It the state has already been visited - move on to the next
         * state.
         * */
        if ((method == FCS_METHOD_OPTIMIZE) ?
                (ptr_state_val->visited & FCS_VISITED_IN_OPTIMIZED_PATH) :
                ((ptr_state_val->visited & FCS_VISITED_DEAD_END) ||
                 (is_scan_visited(ptr_state_val, soft_thread_id)))
                )
        {
            goto label_next_state;
        }
        TRACE0("Counting cells");

        ptr_state_key = ptr_state_val->key;
        /* Count the free-cells */
        num_vacant_freecells = 0;
        for(a=0;a<LOCAL_FREECELLS_NUM;a++)
        {
            if (fcs_freecell_card_num((*ptr_state_key), a) == 0)
            {
                num_vacant_freecells++;
            }
        }

        /* Count the number of unoccupied stacks */

        num_vacant_stacks = 0;
        for(a=0;a<LOCAL_STACKS_NUM;a++)
        {
            if (fcs_col_len(fcs_state_get_col(the_state, a)) == 0)
            {
                num_vacant_stacks++;
            }
        }


        if (check_if_limits_exceeded())
        {
            soft_thread->first_state_to_check_val = ptr_state_val;

            TRACE0("myreturn - FCS_STATE_SUSPEND_PROCESS");
            myreturn(FCS_STATE_SUSPEND_PROCESS);
        }

        TRACE0("debug_iter_output");
        if (instance->debug_iter_output)
        {
#ifdef DEBUG
            printf("ST Name: %s\n", soft_thread->name);
#endif
            instance->debug_iter_output_func(
                    (void*)instance->debug_iter_output_context,
                    instance->num_times,
                    ptr_state_val->depth,
                    (void*)instance,
                    ptr_state_val,
                    ((ptr_state_val->parent_val == NULL) ?
                        0 :
                        ptr_state_val->parent_val->visited_iter
                    )
                    );
        }


        if ((num_vacant_stacks == LOCAL_STACKS_NUM) && (num_vacant_freecells == LOCAL_FREECELLS_NUM))
        {
            instance->final_state_val = ptr_state_val;

            myreturn(FCS_STATE_WAS_SOLVED);
        }

        calculate_real_depth(
            ptr_state_val
        );

        soft_thread->num_vacant_freecells = num_vacant_freecells;
        soft_thread->num_vacant_stacks = num_vacant_stacks;

        if (soft_thread->a_star_positions_by_rank)
        {
            free(soft_thread->a_star_positions_by_rank);
            soft_thread->a_star_positions_by_rank = NULL;
        }

        TRACE0("perform_tests");
        /* Do all the tests at one go, because that the way it should be
           done for BFS and A*
        */
        derived.num_states = 0;
        for(a=0 ;
            a < tests_order_num;
            a++)
        {
            check = fc_solve_sfs_tests[tests_order_tests[a] & FCS_TEST_ORDER_NO_FLAGS_MASK] (
                    soft_thread,
                    ptr_state_val,
                    &derived
                    );
            if ((check == FCS_STATE_BEGIN_SUSPEND_PROCESS) ||
                (check == FCS_STATE_EXCEEDS_MAX_NUM_TIMES) ||
                (check == FCS_STATE_SUSPEND_PROCESS))
            {
                /* Save the current position in the scan */
                soft_thread->first_state_to_check_val = ptr_state_val;

                myreturn(FCS_STATE_SUSPEND_PROCESS);
            }
        }

        if (is_a_complete_scan)
        {
            ptr_state_val->visited |= FCS_VISITED_ALL_TESTS_DONE;
        }

        /* Increase the number of iterations by one .
         * */
        {
            instance->num_times++;
            hard_thread->num_times++;
        }

        TRACE0("Insert all states");
        /* Insert all the derived states into the PQ or Queue */

        for(derived_index = 0 ; derived_index < derived.num_states ; derived_index++)
        {
            ptr_new_state_val = derived.states[derived_index].state_ptr;

            if (method == FCS_METHOD_A_STAR)
            {
                fc_solve_PQueuePush(
                    a_star_pqueue,
                    ptr_new_state_val,
                    fc_solve_a_star_rate_state(soft_thread,
                        ptr_new_state_val
                        )
                    );
            }
            else
            {
                fc_solve_bfs_enqueue_state(
                    soft_thread,
                    ptr_new_state_val
                    );
            }
        }

        if (method == FCS_METHOD_OPTIMIZE)
        {
            ptr_state_val->visited |= FCS_VISITED_IN_OPTIMIZED_PATH;
        }
        else
        {
            set_scan_visited(
                    ptr_state_val,
                    soft_thread_id
                    );

            if (derived.num_states == 0)
            {
                if (is_a_complete_scan)
                {
                    mark_as_dead_end(
                            ptr_state_val
                            );
                }
            }
        }

        ptr_state_val->visited_iter = instance->num_times-1;

label_next_state:
        TRACE0("Label next state");
        /*
            Extract the next item in the queue/priority queue.
        */
        if ((method == FCS_METHOD_BFS) || (method == FCS_METHOD_OPTIMIZE))
        {
            save_item = bfs_queue->next;
            if (save_item != bfs_queue_last_item)
            {
                ptr_state_val = save_item->s;
                bfs_queue->next = save_item->next;
                free(save_item);
            }
            else
            {
                ptr_state_val = NULL;
            }
        }
        else
        {
            /* It is an A* scan */
            fc_solve_PQueuePop(a_star_pqueue,
                &ptr_state_val
                );
        }
    }

    myreturn(FCS_STATE_IS_NOT_SOLVEABLE);
}

#undef myreturn

/*
 * Calculate, cache and return the positions_by_rank meta-data
 * about the currently-evaluated state.
 */
extern char * fc_solve_get_the_positions_by_rank_data(
        fc_solve_soft_thread_t * soft_thread,
        fcs_state_extra_info_t * ptr_state_val
        )
{
    char * * positions_by_rank_location;
    switch(soft_thread->method)
    {
        case FCS_METHOD_SOFT_DFS:
        case FCS_METHOD_RANDOM_DFS:
            {
                positions_by_rank_location = &(
                    soft_thread->soft_dfs_info[
                        soft_thread->depth
                    ].positions_by_rank
                    );
            }
            break;
        default:
            {
                positions_by_rank_location = &(
                        soft_thread->a_star_positions_by_rank
                        );
            }
            break;
    }

    if (! *positions_by_rank_location)
    {
        char * positions_by_rank;
        fc_solve_instance_t * instance;
        fcs_state_t * ptr_state_key;

        ptr_state_key = ptr_state_val->key;
#ifndef HARD_CODED_NUM_DECKS
        int decks_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
        int stacks_num;
#endif
#ifndef FCS_FREECELL_ONLY
        int sequences_are_built_by;
#endif

        instance = soft_thread->hard_thread->instance;

#ifndef HARD_CODED_NUM_DECKS
        decks_num = instance->decks_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
        stacks_num = instance->stacks_num;
#endif
#ifndef FCS_FREECELL_ONLY
        sequences_are_built_by = instance->sequences_are_built_by;
#endif

        /* We don't keep track of kings (rank == 13). */
#define NUM_POS_BY_RANK_SLOTS 13
        /* We need 2 chars per card - one for the column_idx and one
         * for the card_idx.
         *
         * We also need it times 13 for each of the ranks.
         *
         * We need (4*LOCAL_DECKS_NUM+1) slots to hold the cards plus a
         * (-1,-1) (= end) padding.             * */
#define FCS_POS_BY_RANK_SIZE (sizeof(positions_by_rank[0]) * NUM_POS_BY_RANK_SLOTS * FCS_POS_BY_RANK_WIDTH)

        positions_by_rank = malloc(FCS_POS_BY_RANK_SIZE);

        memset(positions_by_rank, -1, FCS_POS_BY_RANK_SIZE);

        {
            char * positions_by_rank_slots[NUM_POS_BY_RANK_SLOTS];

            {
                int c;

                /* Initialize the pointers to the first available slots */
                for ( c=0 ; c < NUM_POS_BY_RANK_SLOTS ; c++ )
                {
                    positions_by_rank_slots[c] = &positions_by_rank[
                        (((LOCAL_DECKS_NUM << 2)+1) << 1) * c
                    ];
                }
            }

            /* Populate positions_by_rank by looping over the stacks and
             * indices looking for the cards and filling them. */

            {
                int ds;

                for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
                {
                    fcs_cards_column_t dest_col;
                    int dc;
                    int dest_cards_num;

                    dest_col = fcs_state_get_col(*(ptr_state_key), ds);
                    dest_cards_num = fcs_col_len(dest_col);
                    for(dc=0;dc<dest_cards_num;dc++)
                    {
                        int is_seq_in_dest;
                        fcs_card_t dest_below_card, dest_card;

                        dest_card = fcs_col_get_card(dest_col, dc);
                        is_seq_in_dest = 0;
                        if (dest_cards_num - 1 > dc)
                        {
                            dest_below_card = fcs_col_get_card(dest_col, dc+1);
                            if (fcs_is_parent_card(dest_below_card, dest_card))
                            {
                                is_seq_in_dest = 1;
                            }
                        }
                        if (!is_seq_in_dest)
                        {
                            *(positions_by_rank_slots[fcs_card_card_num(dest_card)-1]++) = ds;
                            *(positions_by_rank_slots[fcs_card_card_num(dest_card)-1]++) = dc;
                        }
                    }
                }
            }
        }

        *positions_by_rank_location = positions_by_rank;
    }

    return *positions_by_rank_location;
}

