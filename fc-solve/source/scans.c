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

#define BUILDING_DLL 1

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>

#include "config.h"

#include "state.h"
#include "card.h"
#include "scans.h"
#include "alloc.h"
#include "check_and_add_state.h"
#include "move_stack_compact_alloc.h"
#include "freecell.h"
#include "simpsim.h"
#include "move_funcs_maps.h"

#include "check_limits.h"
#include "inline.h"

void fc_solve_increase_dfs_max_depth(
    fc_solve_soft_thread_t * soft_thread
    )
{
    int new_dfs_max_depth = soft_thread->method_specific.soft_dfs.dfs_max_depth + 16;
    int d;

    soft_thread->method_specific.soft_dfs.soft_dfs_info = realloc(
        soft_thread->method_specific.soft_dfs.soft_dfs_info,
        sizeof(soft_thread->method_specific.soft_dfs.soft_dfs_info[0])*new_dfs_max_depth
        );

    for(d=soft_thread->method_specific.soft_dfs.dfs_max_depth ; d<new_dfs_max_depth; d++)
    {
        soft_thread->method_specific.soft_dfs.soft_dfs_info[d].state_val = NULL;
        soft_thread->method_specific.soft_dfs.soft_dfs_info[d].test_index = 0;
        soft_thread->method_specific.soft_dfs.soft_dfs_info[d].current_state_index = 0;
        soft_thread->method_specific.soft_dfs.soft_dfs_info[d].derived_states_list.num_states = 0;
        soft_thread->method_specific.soft_dfs.soft_dfs_info[d].derived_states_list.states = NULL;
        soft_thread->method_specific.soft_dfs.soft_dfs_info[d].derived_states_random_indexes = NULL;
        soft_thread->method_specific.soft_dfs.soft_dfs_info[d].derived_states_random_indexes_max_size = 0;
        soft_thread->method_specific.soft_dfs.soft_dfs_info[d].positions_by_rank = NULL;
    }

    soft_thread->method_specific.soft_dfs.dfs_max_depth = new_dfs_max_depth;
}

/*
    fc_solve_soft_dfs_do_solve is the event loop of the
    Random-DFS scan. DFS which is recursive in nature is handled here
    without procedural recursion
    by using some dedicated stacks for the traversal.
  */
#define the_state (*ptr_state_key)

#ifdef DEBUG
#define TRACE0(message) \
        { \
            if (getenv("FCS_TRACE")) \
            { \
            printf("%s. Depth=%d ; the_soft_Depth=%d ; Iters=%d ; test_index=%d ; current_state_index=%d ; num_states=%d\n", \
                    message, \
                    soft_thread->method_specific.soft_dfs.depth, (the_soft_dfs_info-soft_thread->method_specific.soft_dfs.soft_dfs_info), \
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

/*
 * This macro traces the path of the state up to the original state,
 * and thus calculates its real depth.
 *
 * It then assigns the newly updated depth throughout the path.
 *
 * */

#define calculate_real_depth(ptr_state_orig_val) \
{                                                                  \
    if (calc_real_depth)                                           \
    {                                                              \
        int this_real_depth = 0;                                   \
        fcs_state_extra_info_t * temp_state_val = ptr_state_orig_val; \
        /* Count the number of states until the original state. */ \
        while(temp_state_val != NULL)                                   \
        {                                                          \
            temp_state_val = temp_state_val->parent_val;             \
            this_real_depth++;                                     \
        }                                                          \
        this_real_depth--;                                         \
        temp_state_val = (ptr_state_orig_val);                      \
        /* Assign the new depth throughout the path */             \
        while (temp_state_val->depth != this_real_depth)            \
        {                                                          \
            temp_state_val->depth = this_real_depth;                \
            this_real_depth--;                                     \
            temp_state_val = temp_state_val->parent_val;             \
        }                                                          \
    }                                                              \
}

/*
 * This macro marks a state as a dead end, and afterwards propogates
 * this information to its parent and ancestor states.
 * */

#define mark_as_dead_end(ptr_state_input_val) \
{      \
    if (scans_synergy)      \
    {        \
        fcs_state_extra_info_t * temp_state_val = (ptr_state_input_val); \
        /* Mark as a dead end */        \
        temp_state_val->visited |= FCS_VISITED_DEAD_END; \
        temp_state_val = temp_state_val->parent_val;          \
        if (temp_state_val != NULL)                    \
        {           \
            /* Decrease the refcount of the state */    \
            temp_state_val->num_active_children--;   \
            while((temp_state_val->num_active_children == 0) && (temp_state_val->visited & FCS_VISITED_ALL_TESTS_DONE))  \
            {          \
                /* Mark as dead end */        \
                temp_state_val->visited |= FCS_VISITED_DEAD_END;  \
                /* Go to its parent state */       \
                temp_state_val = temp_state_val->parent_val;    \
                if (temp_state_val == NULL)         \
                {                \
                    break;             \
                }      \
                /* Decrease the refcount */       \
                temp_state_val->num_active_children--;     \
            }       \
        }   \
    }      \
}

#define BUMP_NUM_TIMES() \
{       \
    instance->num_times++; \
    hard_thread->num_times++; \
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
    int check;
    int do_first_iteration;
    fcs_soft_dfs_stack_item_t * the_soft_dfs_info;
#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif
    int dfs_max_depth;

    int tests_order_num = soft_thread->tests_order.num;
    int * tests_order_tests = soft_thread->tests_order.tests;
    int calc_real_depth = instance->calc_real_depth;
    int is_a_complete_scan = soft_thread->is_a_complete_scan;
    int soft_thread_id = soft_thread->id;
    fcs_derived_states_list_t * derived_states_list;
    int scans_synergy;
    fcs_rand_t * rand_gen;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    scans_synergy = instance->scans_synergy;

    the_soft_dfs_info = &(soft_thread->method_specific.soft_dfs.soft_dfs_info[soft_thread->method_specific.soft_dfs.depth]);

    dfs_max_depth = soft_thread->method_specific.soft_dfs.dfs_max_depth;
    ptr_state_val = the_soft_dfs_info->state_val;
    derived_states_list = &(the_soft_dfs_info->derived_states_list);
    
    rand_gen = &(soft_thread->method_specific.soft_dfs.rand_gen);
    
    calculate_real_depth(
        ptr_state_val
    );

    TRACE0("Before depth loop");
    /*
        The main loop.
    */
    while (soft_thread->method_specific.soft_dfs.depth >= 0)
    {
        /*
            Increase the "maximal" depth if it is about to be exceeded.
        */
        if (soft_thread->method_specific.soft_dfs.depth+1 >= dfs_max_depth)
        {
            fc_solve_increase_dfs_max_depth(soft_thread);

            /* Because the address of soft_thread->method_specific.soft_dfs.soft_dfs_info may
             * be changed
             * */
            the_soft_dfs_info = &(soft_thread->method_specific.soft_dfs.soft_dfs_info[soft_thread->method_specific.soft_dfs.depth]);
            dfs_max_depth = soft_thread->method_specific.soft_dfs.dfs_max_depth;
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
                if (--soft_thread->method_specific.soft_dfs.depth < 0)
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
                int i;

                TRACE0("In iter_handler");

                if (instance->debug_iter_output_func)
                {
#ifdef DEBUG
                    printf("ST Name: %s\n", soft_thread->name);
#endif
                    instance->debug_iter_output_func(
                        (void*)instance->debug_iter_output_context,
                        instance->num_times,
                        soft_thread->method_specific.soft_dfs.depth,
                        (void*)instance,
                        ptr_state_val,
                        ((soft_thread->method_specific.soft_dfs.depth == 0) ?
                            0 :
                            soft_thread->method_specific.soft_dfs.soft_dfs_info[soft_thread->method_specific.soft_dfs.depth-1].state_val->visited_iter
                        )
                        );
                }

                ptr_state_key = ptr_state_val->key;

                /* Count the free-cells */
                num_vacant_freecells = 0;
                for(i=0;i<LOCAL_FREECELLS_NUM;i++)
                {
                    if (fcs_freecell_card_num(the_state, i) == 0)
                    {
                        num_vacant_freecells++;
                    }
                }

                /* Count the number of unoccupied stacks */

                num_vacant_stacks = 0;
                for(i=0;i<LOCAL_STACKS_NUM;i++)
                {
                    if (fcs_col_len(fcs_state_get_col(the_state, i)) == 0)
                    {
                        num_vacant_stacks++;
                    }
                }

                /* Check if we have reached the empty state */
                if ((num_vacant_stacks == LOCAL_STACKS_NUM) &&
                    (num_vacant_freecells  == LOCAL_FREECELLS_NUM))
                {
                    instance->final_state_val = ptr_state_val;
                    
                    BUMP_NUM_TIMES();

                    TRACE0("Returning FCS_STATE_WAS_SOLVED");
                    return FCS_STATE_WAS_SOLVED;
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
                    return FCS_STATE_SUSPEND_PROCESS;
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
                                    rand_gen
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
                    BUMP_NUM_TIMES();

                    set_scan_visited(
                        single_derived_state,
                        soft_thread_id
                    );

                    single_derived_state->visited_iter = instance->num_times;

                    /*
                        I'm using current_state_indexes[depth]-1 because we already
                        increased it by one, so now it refers to the next state.
                    */
                    soft_thread->method_specific.soft_dfs.depth++;
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
                        return FCS_STATE_SUSPEND_PROCESS;
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
    BUMP_NUM_TIMES();

    soft_thread->method_specific.soft_dfs.depth = -1;

    return FCS_STATE_IS_NOT_SOLVEABLE;
}


#undef state
#undef myreturn

#define FCS_A_STAR_CARDS_UNDER_SEQUENCES_EXPONENT 1.3
#define FCS_A_STAR_SEQS_OVER_RENEGADE_CARDS_EXPONENT 1.3

#define FCS_SEQS_OVER_RENEGADE_POWER(n) pow(n, FCS_A_STAR_SEQS_OVER_RENEGADE_CARDS_EXPONENT)

static GCC_INLINE int update_col_cards_under_sequences(
        fc_solve_soft_thread_t * soft_thread,
        fcs_cards_column_t col,
        double * cards_under_sequences_ptr
        )
{
    int cards_num;
    int c;
    fcs_card_t this_card, prev_card;
#ifndef FCS_FREECELL_ONLY
    int sequences_are_built_by = 
        GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(soft_thread->hard_thread->instance)
        ;
#endif

    cards_num = fcs_col_len(col);
    c = cards_num - 2;
    this_card = fcs_col_get_card(col, c+1);
    prev_card = fcs_col_get_card(col, c);
    while ((c >= 0) && fcs_is_parent_card(this_card,prev_card))
    {
        this_card = prev_card;
        if (--c>=0)
        {
            prev_card = fcs_col_get_card(col, c);
        }
    }
    *cards_under_sequences_ptr += pow(c+1, FCS_A_STAR_CARDS_UNDER_SEQUENCES_EXPONENT);
    return c;
}

static GCC_INLINE void initialize_a_star_rater(
    fc_solve_soft_thread_t * soft_thread,
    fcs_state_extra_info_t * ptr_state_val
    )
{
#ifndef HARD_CODED_NUM_STACKS
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;
#endif
    fcs_state_t * ptr_state_key = ptr_state_val->key;

    int a;
    double cards_under_sequences;

    cards_under_sequences = 0;
    for(a=0;a<INSTANCE_STACKS_NUM;a++)
    {
        update_col_cards_under_sequences(soft_thread, fcs_state_get_col(*ptr_state_key, a), &cards_under_sequences);
    }
    soft_thread->method_specific.befs.meth.befs.a_star_initial_cards_under_sequences = cards_under_sequences;
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


static GCC_INLINE pq_rating_t fc_solve_a_star_rate_state(
    fc_solve_soft_thread_t * soft_thread,
    fcs_state_extra_info_t * ptr_state_val
    )
{
#ifndef FCS_FREECELL_ONLY
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;
#endif
    fcs_state_t * ptr_state_key = ptr_state_val->key;

    double ret=0;
    int a, c, cards_num, num_cards_in_founds;
    int num_vacant_stacks, num_vacant_freecells;
    double cards_under_sequences, temp;
    double seqs_over_renegade_cards;
    fcs_cards_column_t col;

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    DECLARE_GAME_PARAMS();
#endif
#define my_a_star_weights soft_thread->method_specific.befs.meth.befs.a_star_weights
    double * a_star_weights = my_a_star_weights;
#ifndef FCS_FREECELL_ONLY
    int unlimited_sequence_move = INSTANCE_UNLIMITED_SEQUENCE_MOVE;
#else
    #define unlimited_sequence_move 0
#endif

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
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

        c = update_col_cards_under_sequences(soft_thread, col, &cards_under_sequences);
        if (c >= 0)
        {
            seqs_over_renegade_cards +=
                ((unlimited_sequence_move) ?
                    1 :
                    FCS_SEQS_OVER_RENEGADE_POWER(cards_num-c-1)
                    );
        }
    }

    ret += ((soft_thread->method_specific.befs.meth.befs.a_star_initial_cards_under_sequences - cards_under_sequences)
            / soft_thread->method_specific.befs.meth.befs.a_star_initial_cards_under_sequences) * a_star_weights[FCS_A_STAR_WEIGHT_CARDS_UNDER_SEQUENCES];

    ret += (seqs_over_renegade_cards /
               FCS_SEQS_OVER_RENEGADE_POWER(LOCAL_DECKS_NUM*(13*4))
            )
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

#ifdef FCS_FREECELL_ONLY
#define is_filled_by_any_card() 1
#else
#define is_filled_by_any_card() (INSTANCE_EMPTY_STACKS_FILL == FCS_ES_FILLED_BY_ANY_CARD)
#endif
    if (is_filled_by_any_card())
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

#define my_brfs_queue (soft_thread->method_specific.befs.meth.brfs.bfs_queue)
#define my_brfs_queue_last_item \
    (soft_thread->method_specific.befs.meth.brfs.bfs_queue_last_item)

static void GCC_INLINE fc_solve_initialize_bfs_queue(fc_solve_soft_thread_t * soft_thread)
{
    /* Initialize the BFS queue. We have one dummy element at the beginning
       in order to make operations simpler. */
    my_brfs_queue = 
        (fcs_states_linked_list_item_t*)malloc(
            sizeof(fcs_states_linked_list_item_t)
        );
    my_brfs_queue->next = 
    my_brfs_queue_last_item = 
        my_brfs_queue->next =
        (fcs_states_linked_list_item_t*)
        malloc(sizeof(fcs_states_linked_list_item_t))
        ;
    my_brfs_queue_last_item->next = NULL;
}


static GCC_INLINE void normalize_a_star_weights(
    fc_solve_soft_thread_t * soft_thread
    )
{
    /* Normalize the A* Weights, so the sum of all of them would be 1. */
    double sum;
    int a;
    sum = 0;
    for(a=0;a<(sizeof(my_a_star_weights)/sizeof(my_a_star_weights[0]));a++)
    {
        if (my_a_star_weights[a] < 0)
        {
            my_a_star_weights[a] = fc_solve_a_star_default_weights[a];
        }
        sum += my_a_star_weights[a];
    }
    if (sum < 1e-6)
    {
        sum = 1;
    }
    for(a=0;a<(sizeof(my_a_star_weights)/sizeof(my_a_star_weights[0]));a++)
    {
        my_a_star_weights[a] /= sum;
    }
}

extern void fc_solve_soft_thread_init_a_star_or_bfs(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_instance_t * instance = soft_thread->hard_thread->instance;

    fcs_state_extra_info_t * ptr_orig_state_val = instance->state_copy_ptr_val;

    if (soft_thread->method == FCS_METHOD_A_STAR)
    {
        /* Initialize the priotity queue of the A* scan */
        fc_solve_PQueueInitialise(
            &(soft_thread->method_specific.befs.meth.befs.a_star_pqueue),
            1024
        );

        normalize_a_star_weights(soft_thread);

        initialize_a_star_rater(
            soft_thread,
            ptr_orig_state_val
            );
    }
    else
    {
        fc_solve_initialize_bfs_queue(soft_thread);
    }

    /* Initialize the first element to indicate it is the first */
    ptr_orig_state_val->parent_val = NULL;
    ptr_orig_state_val->moves_to_parent = NULL;
    ptr_orig_state_val->depth = 0;

    soft_thread->first_state_to_check_val = ptr_orig_state_val;

    return;
}

#define DEBUG
#ifdef DEBUG
#if 0
static void dump_pqueue (
    fc_solve_soft_thread_t * soft_thread,
    const char * stage_id,
    PQUEUE * pq
    )
{
    int i;
    char * s; 

    if (strcmp(soft_thread->name, "11"))
    {
        return;
    }

    printf("<pqueue_dump stage=\"%s\">\n\n", stage_id);

    for (i = PQ_FIRST_ENTRY ; i < pq->CurrentSize ; i++)
    {
        printf("Rating[%d] = %d\nState[%d] = <<<\n", i, pq->Elements[i].rating, i);
        s = fc_solve_state_as_string(pq->Elements[i].val, 
                soft_thread->hard_thread->INSTANCE_FREECELLS_NUM,
                soft_thread->hard_thread->INSTANCE_STACKS_NUM,
                soft_thread->hard_thread->INSTANCE_DECKS_NUM,
                1,
                0,
                1
                );

        printf("%s\n>>>\n\n", s);

        free(s);
    }

    printf("\n\n</pqueue_dump>\n\n");
}
#else
#define dump_pqueue(a,b,c) {}
#endif
#endif

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

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    DECLARE_GAME_PARAMS();
#endif

    int tests_order_num;
    int * tests_order_tests;
    int calc_real_depth = instance->calc_real_depth;
    int soft_thread_id = soft_thread->id;
    int is_a_complete_scan = soft_thread->is_a_complete_scan;

    int scans_synergy = instance->scans_synergy;
    union {
        struct {
            fcs_states_linked_list_item_t * queue;
            fcs_states_linked_list_item_t * queue_last_item;
        } brfs;
        PQUEUE * a_star_pqueue;
    } scan_specific;

    int error_code;

    derived.num_states = 0;
    derived.states = NULL;

    tests_order_num = soft_thread->tests_order.num;
    tests_order_tests = soft_thread->tests_order.tests;

    ptr_state_val = soft_thread->first_state_to_check_val;

    method = soft_thread->method;
    if (method == FCS_METHOD_A_STAR)
    {
        scan_specific.a_star_pqueue = &(soft_thread->method_specific.befs.meth.befs.a_star_pqueue);
    }
    else
    {
        scan_specific.brfs.queue = my_brfs_queue;
        scan_specific.brfs.queue_last_item = my_brfs_queue_last_item;
    }

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)))
    SET_GAME_PARAMS();
#endif

    /* Continue as long as there are states in the queue or
       priority queue. */
    while ( ptr_state_val != NULL)
    {
        TRACE0("Start of loop");

#ifdef DEBUG
        dump_pqueue(soft_thread, "loop_start", scan_specific.a_star_pqueue);
#endif

        {
             register int temp_visited = ptr_state_val->visited;

            /*
             * If this is an optimization scan and the state being checked is 
             * not in the original solution path - move on to the next state
             * */
            /*
             * It the state has already been visited - move on to the next
             * state.
             * */
            if ((method == FCS_METHOD_OPTIMIZE) ?
                    (
                        (!(temp_visited & FCS_VISITED_IN_SOLUTION_PATH))
                            ||
                        (temp_visited & FCS_VISITED_IN_OPTIMIZED_PATH)
                    )
                    :
                    (
                        (temp_visited & FCS_VISITED_DEAD_END)
                            ||
                        (is_scan_visited(ptr_state_val, soft_thread_id))
                    )
                )
            {
                goto label_next_state;
            }
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
            error_code = FCS_STATE_SUSPEND_PROCESS;
            goto my_return_label;
        }

        TRACE0("debug_iter_output");
        if (instance->debug_iter_output_func)
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

            BUMP_NUM_TIMES();

            error_code = FCS_STATE_WAS_SOLVED;
            goto my_return_label;
        }

        calculate_real_depth(
            ptr_state_val
        );

        soft_thread->num_vacant_freecells = num_vacant_freecells;
        soft_thread->num_vacant_stacks = num_vacant_stacks;

        if (soft_thread->method_specific.befs.a_star_positions_by_rank)
        {
            free(soft_thread->method_specific.befs.a_star_positions_by_rank);
            soft_thread->method_specific.befs.a_star_positions_by_rank = NULL;
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

                error_code = FCS_STATE_SUSPEND_PROCESS;
                goto my_return_label;
            }
        }

        if (is_a_complete_scan)
        {
            ptr_state_val->visited |= FCS_VISITED_ALL_TESTS_DONE;
        }

        /* Increase the number of iterations by one .
         * */
        BUMP_NUM_TIMES();


        TRACE0("Insert all states");
        /* Insert all the derived states into the PQ or Queue */

        for(derived_index = 0 ; derived_index < derived.num_states ; derived_index++)
        {
            ptr_new_state_val = derived.states[derived_index].state_ptr;

            if (method == FCS_METHOD_A_STAR)
            {
                fc_solve_PQueuePush(
                    scan_specific.a_star_pqueue,
                    ptr_new_state_val,
                    fc_solve_a_star_rate_state(
                        soft_thread,
                        ptr_new_state_val
                        )
                    );
            }
            else
            {
                /* Enqueue the new state. */
                fcs_states_linked_list_item_t * last_item_next;

                last_item_next =
                    scan_specific.brfs.queue_last_item->next = 
                    ((fcs_states_linked_list_item_t*)
                        malloc(sizeof(*last_item_next))
                    );

                scan_specific.brfs.queue_last_item->s = ptr_new_state_val;
                last_item_next->next = NULL;
                scan_specific.brfs.queue_last_item = last_item_next;
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
        if (method == FCS_METHOD_A_STAR)
        {

#ifdef DEBUG
        dump_pqueue(soft_thread, "before_pop", scan_specific.a_star_pqueue);
#endif
            /* It is an A* scan */
            fc_solve_PQueuePop(
                scan_specific.a_star_pqueue,
                &ptr_state_val
                );
        }
        else
        {
            save_item = scan_specific.brfs.queue->next;
            if (save_item != scan_specific.brfs.queue_last_item)
            {
                ptr_state_val = save_item->s;
                scan_specific.brfs.queue->next = save_item->next;
                free(save_item);
            }
            else
            {
                ptr_state_val = NULL;
            }
        }
    }

    error_code = FCS_STATE_IS_NOT_SOLVEABLE;
my_return_label:
    /* Free the memory that was allocated by the
     * derived states list */
    if (derived.states != NULL)
    {
        free(derived.states);
    }

    if (method != FCS_METHOD_A_STAR)
    {
        my_brfs_queue_last_item = scan_specific.brfs.queue_last_item;
    }

    if (soft_thread->method_specific.befs.a_star_positions_by_rank)
    {
        free(soft_thread->method_specific.befs.a_star_positions_by_rank);
        soft_thread->method_specific.befs.a_star_positions_by_rank = NULL;
    }

    return error_code;
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
                    soft_thread->method_specific.soft_dfs.soft_dfs_info[
                        soft_thread->method_specific.soft_dfs.depth
                    ].positions_by_rank
                    );
            }
            break;
        default:
            {
                positions_by_rank_location = &(
                        soft_thread->method_specific.befs.a_star_positions_by_rank
                        );
            }
            break;
    }

    if (! *positions_by_rank_location)
    {
        char * positions_by_rank;
#if (!(defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
        fc_solve_instance_t * instance;
        DECLARE_GAME_PARAMS();
#endif
        fcs_state_t * ptr_state_key;

#ifndef FCS_FREECELL_ONLY
        int sequences_are_built_by;
#endif

        ptr_state_key = ptr_state_val->key;

#if (!(defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
        instance = soft_thread->hard_thread->instance;
        SET_GAME_PARAMS();
#endif

#ifndef FCS_FREECELL_ONLY
        sequences_are_built_by = GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance);
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
                    int top_card_idx;
                    fcs_card_t dest_card;

                    dest_col = fcs_state_get_col(*(ptr_state_key), ds);
                    top_card_idx = fcs_col_len(dest_col);

                    if ((top_card_idx--) == 0)
                    {
                        continue;
                    }

                    {
                        fcs_card_t dest_below_card;
                        int dc;
                        for (
                              dc=0,
                              dest_card = fcs_col_get_card(dest_col, 0)
                                ;
                              dc < top_card_idx
                                ;
                              dc++,
                              dest_card = dest_below_card
                            )
                        {
                            dest_below_card = fcs_col_get_card(dest_col, dc+1);
                            if (!fcs_is_parent_card(dest_below_card, dest_card))
                            {
                                *(positions_by_rank_slots[fcs_card_card_num(dest_card)-1]++) = (char)ds;
                                *(positions_by_rank_slots[fcs_card_card_num(dest_card)-1]++) = (char)dc;
                            }
                        }
                    }
                    *(positions_by_rank_slots[fcs_card_card_num(dest_card)-1]++) = (char)ds;
                    *(positions_by_rank_slots[fcs_card_card_num(dest_card)-1]++) = (char)top_card_idx;
                }
            }
        }

        *positions_by_rank_location = positions_by_rank;
    }

    return *positions_by_rank_location;
}

/* 
 * These functions are used by the move functions in freecell.c and
 * simpsim.c.
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

    ptr_new_state_val =
        fcs_state_ia_alloc_into_var(
            &(hard_thread->allocator)
        );

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
    fcs_internal_move_t temp_move;
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
    fcs_int_move_set_type(temp_move,FCS_MOVE_TYPE_CANONIZE);
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
            fcs_compact_alloc_release(&(hard_thread->allocator));
        }
        else if (check == FCS_STATE_ALREADY_EXISTS)
        {
            fcs_compact_alloc_release(&(hard_thread->allocator));
            calculate_real_depth(existing_state_val);
            /* Re-parent the existing state to this one.
             *
             * What it means is that if the depth of the state if it
             * can be reached from this one is lower than what it
             * already have, then re-assign its parent to this state.
             * */
            if (instance->to_reparent_states_real &&
               (existing_state_val->depth > ptr_state_val->depth+1))
            {
                /* Make a copy of "moves" because "moves" will be destroyed */
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

