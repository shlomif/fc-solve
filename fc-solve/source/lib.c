/*
 * lib.c - library interface functions of Freecell Solver.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "card.h"
#include "fcs.h"
#include "preset.h"
#include "fcs_user.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif


struct fcs_user_struct
{
    freecell_solver_instance_t * instance;
    fcs_state_with_locations_t state;
    fcs_state_with_locations_t running_state;
    int ret;
    int state_validity_ret;
    fcs_card_t state_validity_card;
    freecell_solver_user_iter_handler_t iter_handler;
    void * iter_handler_context;

    freecell_solver_soft_thread_t * soft_thread;

#ifdef INDIRECT_STACK_STATES
    char indirect_stacks_buffer[MAX_NUM_STACKS << 7];
#endif
};

typedef struct fcs_user_struct fcs_user_t;

void * freecell_solver_user_alloc(void)
{
    fcs_user_t * ret;

    ret = (fcs_user_t *)malloc(sizeof(fcs_user_t));
    ret->instance = freecell_solver_alloc_instance();
    ret->ret = FCS_STATE_NOT_BEGAN_YET;

    ret->soft_thread =
        freecell_solver_instance_get_soft_thread(
            ret->instance, 0,0
            );
    return (void*)ret;
}

int freecell_solver_user_apply_preset(
    void * user_instance,
    const char * preset_name)
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    return freecell_solver_apply_preset_by_name(
        user->instance,
        preset_name
        );
}

void freecell_solver_user_limit_iterations(
    void * user_instance,
    int max_iters
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    user->instance->max_num_times = max_iters;
}

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

int freecell_solver_user_set_tests_order(
    void * user_instance,
    const char * tests_order,
    char * * error_string
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    return
        freecell_solver_apply_tests_order(
            &(user->soft_thread->tests_order),
            tests_order,
            error_string
            );
}

int freecell_solver_user_solve_board(
    void * user_instance,
    const char * state_as_string
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    user->state = freecell_solver_initial_user_state_to_c(
        state_as_string,
        user->instance->freecells_num,
        user->instance->stacks_num,
        user->instance->decks_num
#ifdef FCS_WITH_TALONS
        ,user->instance->talon_type
#endif
        );

    user->state_validity_ret = freecell_solver_check_state_validity(
        &user->state,
        user->instance->freecells_num,
        user->instance->stacks_num,
        user->instance->decks_num,
#ifdef FCS_WITH_TALONS
        FCS_TALON_NONE,
#endif
        &(user->state_validity_card));

    if (user->state_validity_ret != 0)
    {
        user->ret = FCS_STATE_INVALID_STATE;
        return user->ret;
    }

    /* running_state is a normalized state. So I'm duplicating
     * state to it before state is canonized
     * */
    fcs_duplicate_state(user->running_state, user->state);

    fcs_canonize_state(
        &user->state,
        user->instance->freecells_num,
        user->instance->stacks_num
        );

    freecell_solver_init_instance(user->instance);

    user->ret = freecell_solver_solve_instance(user->instance, &user->state);

    if (user->ret == FCS_STATE_WAS_SOLVED)
    {
        freecell_solver_move_stack_normalize(
            user->instance->solution_moves,
            &(user->state),
            user->instance->freecells_num,
            user->instance->stacks_num,
            user->instance->decks_num
            );
    }

    return user->ret;
}

int freecell_solver_user_resume_solution(
    void * user_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->ret = freecell_solver_resume_instance(user->instance);
    if (user->ret == FCS_STATE_WAS_SOLVED)
    {
        freecell_solver_move_stack_normalize(
            user->instance->solution_moves,
            &(user->state),
            user->instance->freecells_num,
            user->instance->stacks_num,
            user->instance->decks_num
            );
    }

    return user->ret;
}

int freecell_solver_user_get_next_move(
    void * user_instance,
    fcs_move_t * move
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;
    if (user->ret == FCS_STATE_WAS_SOLVED)
    {
        int ret;

        ret = fcs_move_stack_pop(
            user->instance->solution_moves,
            move
            );

        if (ret == 0)
        {
            freecell_solver_apply_move(
                &(user->running_state),
                *move,
                user->instance->freecells_num,
                user->instance->stacks_num,
                user->instance->decks_num
                );
        }
        return ret;
    }
    else
    {
        return 1;
    }
}

char * freecell_solver_user_current_state_as_string(
    void * user_instance,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return
        freecell_solver_state_as_string(
            &(user->running_state),
            user->instance->freecells_num,
            user->instance->stacks_num,
            user->instance->decks_num,
            parseable_output,
            canonized_order_output,
            display_10_as_t
            );
}

void freecell_solver_user_free(
    void * user_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if (user->ret == FCS_STATE_WAS_SOLVED)
    {
        fcs_move_stack_destroy(user->instance->solution_moves);
    }
    else if (user->ret == FCS_STATE_SUSPEND_PROCESS)
    {
        freecell_solver_unresume_instance(user->instance);
    }

    if (user->ret != FCS_STATE_NOT_BEGAN_YET)
    {
        fcs_clean_state(&(user->state));
        if (user->ret != FCS_STATE_INVALID_STATE)
        {
            fcs_clean_state(&(user->running_state));
            freecell_solver_finish_instance(user->instance);
        }
    }

    freecell_solver_free_instance(user->instance);

    free(user);
}

int freecell_solver_user_get_current_depth(
    void * user_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return (user->soft_thread->num_solution_states - 1);
}

void freecell_solver_user_set_solving_method(
    void * user_instance,
    int method
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->soft_thread->method = method;
}

int freecell_solver_user_set_num_freecells(
    void * user_instance,
    int freecells_num
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if ((freecells_num < 0) || (freecells_num > MAX_NUM_FREECELLS))
    {
        return 1;
    }
    user->instance->freecells_num = freecells_num;

    return 0;
}

int freecell_solver_user_set_num_stacks(
    void * user_instance,
    int stacks_num
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if ((stacks_num < 0) || (stacks_num > MAX_NUM_STACKS))
    {
        return 1;
    }
    user->instance->stacks_num = stacks_num;

    return 0;
}

int freecell_solver_user_set_num_decks(
    void * user_instance,
    int decks_num
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if ((decks_num < 0) || (decks_num > MAX_NUM_DECKS))
    {
        return 1;
    }
    user->instance->decks_num = decks_num;

    return 0;
}


int freecell_solver_user_set_game(
    void * user_instance,
    int freecells_num,
    int stacks_num,
    int decks_num,
    int sequences_are_built_by,
    int unlimited_sequence_move,
    int empty_stacks_fill
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if ((freecells_num < 0) || (freecells_num > MAX_NUM_FREECELLS))
        return 1;
    if ((stacks_num < 1) || (stacks_num > MAX_NUM_STACKS))
        return 2;
    if ((decks_num < 1) || (decks_num > MAX_NUM_DECKS))
        return 3;
    if ((sequences_are_built_by < 0) || (sequences_are_built_by > 2))
        return 4;
    if ((unlimited_sequence_move < 0) || (unlimited_sequence_move > 1))
        return 5;
    if ((empty_stacks_fill < 0) || (empty_stacks_fill > 2))
        return 6;

    user->instance->freecells_num = freecells_num;
    user->instance->stacks_num = stacks_num;
    user->instance->decks_num = decks_num;

    user->instance->sequences_are_built_by = sequences_are_built_by;
    user->instance->unlimited_sequence_move = unlimited_sequence_move;
    user->instance->empty_stacks_fill = empty_stacks_fill;

    return 0;
}

int freecell_solver_user_get_num_times(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return user->instance->num_times;
}

int freecell_solver_user_get_limit_iterations(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return user->instance->max_num_times;
}

int freecell_solver_user_get_moves_left(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;
    if (user->ret == FCS_STATE_WAS_SOLVED)
        return user->instance->solution_moves->num_moves;
    else
        return 0;
}

void freecell_solver_user_set_solution_optimization(
    void * user_instance,
    int optimize
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->optimize_solution_path = optimize;
}

char * freecell_solver_user_move_to_string(
    fcs_move_t move,
    int standard_notation
    )
{
    return freecell_solver_move_to_string(move, standard_notation);
}

void freecell_solver_user_limit_depth(
    void * user_instance,
    int max_depth
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->max_depth = max_depth;
}

int freecell_solver_user_get_max_num_freecells(void)
{
    return MAX_NUM_FREECELLS;
}

int freecell_solver_user_get_max_num_stacks(void)
{
    return MAX_NUM_STACKS;
}

int freecell_solver_user_get_max_num_decks(void)
{
    return MAX_NUM_DECKS;
}


char * freecell_solver_user_get_invalid_state_error_string(
    void * user_instance,
    int print_ts
    )
{
    fcs_user_t * user;
    char string[80], card_str[10];

    user = (fcs_user_t *)user_instance;

    if (user->state_validity_ret == 0)
    {
        return strdup("");
    }
    fcs_card_perl2user(user->state_validity_card, card_str, print_ts);

    if (user->state_validity_ret == 3)
    {
        sprintf(string, "%s",
            "There's an empty slot in one of the stacks."
            );
    }
    else
    {
        sprintf(string, "%s%s.",
            ((user->state_validity_ret == 2)? "There's an extra card: " : "There's a missing card: "),
            card_str
        );
    }
    return strdup(string);

}

int freecell_solver_user_set_sequences_are_built_by_type(
    void * user_instance,
    int sbb
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if ((sbb < 0) || (sbb > 2))
    {
        return 1;
    }
    user->instance->sequences_are_built_by = sbb;

    return 0;
}

int freecell_solver_user_set_sequence_move(
    void * user_instance,
    int unlimited
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->unlimited_sequence_move = (!!unlimited);

    return 0;
}

int freecell_solver_user_set_empty_stacks_filled_by(
    void * user_instance,
    int es_fill
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if ((es_fill < 0) || (es_fill > 2))
    {
        return 1;
    }
    user->instance->empty_stacks_fill = es_fill;

    return 0;
}

int freecell_solver_user_set_a_star_weight(
    void * user_instance,
    int index,
    double weight
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if ((index < 0) || (index >= (sizeof(user->soft_thread->a_star_weights)/sizeof(user->soft_thread->a_star_weights[0]))))
    {
        return 1;
    }
    if (weight < 0)
    {
        return 2;
    }

    user->soft_thread->a_star_weights[index] = weight;

    return 0;

}

static void freecell_solver_user_iter_handler_wrapper(
    void * user_instance,
    int iter_num,
    int depth,
    void * lp_instance,
    fcs_state_with_locations_t * ptr_state_with_locations,
    int parent_iter_num
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return
        user->iter_handler(
            user_instance,
            iter_num,
            depth,
            (void *)ptr_state_with_locations,
            parent_iter_num,
            user->iter_handler_context
            );
}

void freecell_solver_user_set_iter_handler(
    void * user_instance,
    freecell_solver_user_iter_handler_t iter_handler,
    void * iter_handler_context
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if (iter_handler == NULL)
    {
        user->instance->debug_iter_output = 0;
    }
    else
    {
        /* Disable it temporarily while we change the settings */
        user->instance->debug_iter_output = 0;
        user->iter_handler = iter_handler;
        user->iter_handler_context = iter_handler_context;
        user->instance->debug_iter_output_context = user;
        user->instance->debug_iter_output_func = freecell_solver_user_iter_handler_wrapper;
        user->instance->debug_iter_output = 1;
    }
}

char * freecell_solver_user_iter_state_as_string(
    void * user_instance,
    void * ptr_state,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return
        freecell_solver_state_as_string(
            ptr_state,
            user->instance->freecells_num,
            user->instance->stacks_num,
            user->instance->decks_num,
            parseable_output,
            canonized_order_output,
            display_10_as_t
            );
}

void freecell_solver_user_set_random_seed(
    void * user_instance,
    int seed
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    freecell_solver_rand_srand(user->soft_thread->rand_gen, seed);
}

int freecell_solver_user_get_num_states_in_collection(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return user->instance->num_states_in_collection;
}

void freecell_solver_user_limit_num_states_in_collection(
    void * user_instance,
    int max_num_states
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    user->instance->max_num_states_in_collection = max_num_states;
}

int freecell_solver_user_next_soft_thread(
    void * user_instance
    )
{
    fcs_user_t * user;
    freecell_solver_soft_thread_t * soft_thread;

    user = (fcs_user_t *)user_instance;

    soft_thread = freecell_solver_new_soft_thread(user->soft_thread);

    if (soft_thread == NULL)
    {
        return 1;
    }

    user->soft_thread = soft_thread;

    return 0;
}

extern void freecell_solver_user_set_soft_thread_step(
    void * user_instance,
    int num_times_step
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->soft_thread->num_times_step = num_times_step;
}

int freecell_solver_user_next_hard_thread(
    void * user_instance
    )
{
    fcs_user_t * user;
    freecell_solver_soft_thread_t * soft_thread;

    user = (fcs_user_t *)user_instance;

    soft_thread = freecell_solver_new_hard_thread(user->instance);

    if (soft_thread == NULL)
    {
        return 1;
    }

    user->soft_thread = soft_thread;

    return 0;
}

int freecell_solver_user_get_num_soft_threads_in_instance(
    void * user_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return user->instance->next_soft_thread_id;
}

void freecell_solver_user_set_calc_real_depth(
    void * user_instance,
    int calc_real_depth
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->calc_real_depth = calc_real_depth;
}

void freecell_solver_user_set_soft_thread_name(
    void * user_instance,
    char * name
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if (user->soft_thread->name != NULL)
    {
        free(user->soft_thread->name);
    }
    user->soft_thread->name = strdup(name);    
}
