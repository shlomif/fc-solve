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

struct fcs_instance_item_struct
{
    freecell_solver_instance_t * instance;
    int ret;
    int limit;
};

typedef struct fcs_instance_item_struct fcs_instance_item_t;

struct fcs_user_struct
{
    /* 
     * This is a list of several consecutive instances that are run
     * one after the other in case the previous ones could not solve
     * the board 
     * */
    fcs_instance_item_t * instances_list;
    int num_instances;
    int max_num_instances;
    
    int current_instance_idx;
    /* 
     * The global (sequence-wide) limit of the iterations. Used
     * by limit_iterations() and friends
     * */
    int current_iterations_limit;
    /*
     * The number of iterations this board started at.
     * */
    int iterations_board_started_at;
    /*
     * The number of iterations that the current instance started solving from.
     * */
    int init_num_times;
    /*
     * A pointer to the currently active instance out of the sequence
     * */
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
    char * state_string_copy;

    fcs_preset_t common_preset;
};

typedef struct fcs_user_struct fcs_user_t;

static void user_initialize(
        fcs_user_t * ret
        )
{
    const fcs_preset_t * freecell_preset;
    
    freecell_solver_get_preset_by_name(
        "freecell",
        &freecell_preset
        );

    fcs_duplicate_preset(ret->common_preset, *freecell_preset);
        
    ret->max_num_instances = 10;
    ret->instances_list = malloc(sizeof(ret->instances_list[0]) * ret->max_num_instances);
    ret->num_instances = 1;
    ret->current_instance_idx = 0;
    ret->instance = freecell_solver_alloc_instance();
    freecell_solver_apply_preset_by_ptr(ret->instance, &(ret->common_preset));
    ret->instances_list[ret->current_instance_idx].instance = ret->instance;
    ret->instances_list[ret->current_instance_idx].ret = ret->ret = FCS_STATE_NOT_BEGAN_YET;
    ret->instances_list[ret->current_instance_idx].limit = -1;
    ret->current_iterations_limit = -1;

    ret->soft_thread =
        freecell_solver_instance_get_soft_thread(
            ret->instance, 0,0
            );

    ret->state_string_copy = NULL;
    ret->iterations_board_started_at = 0;
}

void * freecell_solver_user_alloc(void)
{
    fcs_user_t * ret;

    ret = (fcs_user_t *)malloc(sizeof(fcs_user_t));

    user_initialize(ret);

    return (void*)ret;
}

int freecell_solver_user_apply_preset(
    void * user_instance,
    const char * preset_name)
{
    const fcs_preset_t * new_preset_ptr;
    fcs_user_t * user;
    int status;
    int i;

    user = (fcs_user_t*)user_instance;

    status = 
        freecell_solver_get_preset_by_name(
            preset_name,
            &new_preset_ptr
            );

    if (status != FCS_PRESET_CODE_OK)
    {
        return status;
    }

    for(i = 0 ; i < user->num_instances ; i++)
    {
        status = freecell_solver_apply_preset_by_ptr(
            user->instances_list[i].instance,
            new_preset_ptr
            );

        if (status != FCS_PRESET_CODE_OK)
        {
            return status;
        }
    }

    fcs_duplicate_preset(user->common_preset, *new_preset_ptr);

    return FCS_PRESET_CODE_OK;
}

void freecell_solver_user_limit_iterations(
    void * user_instance,
    int max_iters
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    user->current_iterations_limit = max_iters;
}

void freecell_solver_user_limit_current_instance_iterations(
    void * user_instance,
    int max_iters
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    user->instances_list[user->current_instance_idx].limit = max_iters;
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

    user->state_string_copy = strdup(state_as_string);

    user->current_instance_idx = 0;

    return freecell_solver_user_resume_solution(user_instance);
}

static void recycle_instance(
    fcs_user_t * user,
    int i
    )
{
    if (user->instances_list[i].ret == FCS_STATE_WAS_SOLVED)
    {
        fcs_move_stack_destroy(user->instance->solution_moves);
        user->instance->solution_moves = NULL;
    }
    else if (user->instances_list[i].ret == FCS_STATE_SUSPEND_PROCESS)
    {
        freecell_solver_unresume_instance(user->instances_list[i].instance);
    }

    if (user->instances_list[i].ret != FCS_STATE_NOT_BEGAN_YET)
    {
        freecell_solver_recycle_instance(user->instances_list[i].instance);
    }

    user->instances_list[i].ret = FCS_STATE_NOT_BEGAN_YET;    
}

int freecell_solver_user_resume_solution(
    void * user_instance
    )
{
    int init_num_times;
    int run_for_first_iteration = 1;
    int ret;
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;
    
    /* 
     * I expect user->current_instance_idx to be initialized at some value.
     * */
    for( ; 
        run_for_first_iteration || ((user->current_instance_idx < user->num_instances) && (ret == FCS_STATE_IS_NOT_SOLVEABLE)) ;
        recycle_instance(user, user->current_instance_idx), user->current_instance_idx++
       )
    {
        run_for_first_iteration = 0;
        
        user->instance = user->instances_list[user->current_instance_idx].instance;

        if (user->instances_list[user->current_instance_idx].ret == FCS_STATE_NOT_BEGAN_YET)
        {

            user->state = freecell_solver_initial_user_state_to_c(
                user->state_string_copy,
                user->instance->freecells_num,
                user->instance->stacks_num,
                user->instance->decks_num
#ifdef FCS_WITH_TALONS
                ,user->instance->talon_type
#endif
#ifdef INDIRECT_STACK_STATES
                ,user->indirect_stacks_buffer
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

#define global_limit() \
        (user->instance->num_times + user->current_iterations_limit - user->iterations_board_started_at)
#define local_limit()  \
        (user->instances_list[user->current_instance_idx].limit)
#define min(a,b) (((a)<(b))?(a):(b))
#define calc_max_iters() \
        {          \
            if (user->instances_list[user->current_instance_idx].limit < 0)  \
            {\
                if (user->current_iterations_limit < 0)\
                {\
                    user->instance->max_num_times = -1;\
                }\
                else\
                {\
                    user->instance->max_num_times = global_limit();\
                }\
            }\
            else\
            {\
                if (user->current_iterations_limit < 0)\
                {\
                    user->instance->max_num_times = local_limit();\
                }\
                else\
                {\
                    int a, b;\
                    \
                    a = global_limit();\
                    b = local_limit();\
        \
                    user->instance->max_num_times = min(a,b);\
                }\
            }\
        }


            calc_max_iters();

            user->init_num_times = init_num_times = user->instance->num_times;

            ret = user->ret = 
                user->instances_list[user->current_instance_idx].ret = 
                freecell_solver_solve_instance(user->instance, &user->state);
        }
        else
        {

            calc_max_iters();
    
            user->init_num_times = init_num_times = user->instance->num_times;
            
            ret = user->ret = 
                user->instances_list[user->current_instance_idx].ret =
                freecell_solver_resume_instance(user->instance);
        }
        
        user->iterations_board_started_at += user->instance->num_times - init_num_times;
        user->init_num_times = user->instance->num_times;

        if (user->ret == FCS_STATE_WAS_SOLVED)
        {
            freecell_solver_move_stack_normalize(
                user->instance->solution_moves,
                &(user->state),
                user->instance->freecells_num,
                user->instance->stacks_num,
                user->instance->decks_num
                );

            break;
        }
        else if (user->ret == FCS_STATE_SUSPEND_PROCESS)
        {
            /* 
             * First - check if we exceeded our limit. If so - we must terminate
             * and return now.
             * */
            if ((user->current_iterations_limit >= 0) &&
                (user->iterations_board_started_at >= user->current_iterations_limit))
            {
                break;
            }
            
            /* 
             * Determine if we exceeded the instance-specific quota and if
             * so, designate it as unsolvable.
             * */
            if ((local_limit() >= 0) &&
                (user->instance->num_times >= local_limit())
               )
            {
                ret = FCS_STATE_IS_NOT_SOLVEABLE;
            }
        }
    }

    return ret;
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

static void user_free_resources(
    fcs_user_t * user
    )
{
    int i;
    
    for(i=0;i<user->num_instances;i++)
    {
        int ret_code = user->instances_list[i].ret;
        
        if (ret_code == FCS_STATE_WAS_SOLVED)
        {
            fcs_move_stack_destroy(user->instance->solution_moves);
            user->instance->solution_moves = NULL;
        }
        else if (ret_code == FCS_STATE_SUSPEND_PROCESS)
        {
            freecell_solver_unresume_instance(user->instances_list[i].instance);
        }

        if (ret_code != FCS_STATE_NOT_BEGAN_YET)
        {
            if (ret_code != FCS_STATE_INVALID_STATE)
            {
                freecell_solver_finish_instance(user->instances_list[i].instance);
            }
        }

        freecell_solver_free_instance(user->instances_list[i].instance);
    }

    free(user->instances_list);

    if (user->state_string_copy != NULL)
    {
        free(user->state_string_copy);
        user->state_string_copy = NULL;
    }
}

void freecell_solver_user_free(
    void * user_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user_free_resources(user);

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

#define set_for_all_instances(what) \
    { \
    for(i = 0 ; i < user->num_instances ; i++)        \
    {                    \
        user->instances_list[i].instance->what = what;       \
    }        \
    user->common_preset.what = what;     \
    }
    
int freecell_solver_user_set_num_freecells(
    void * user_instance,
    int freecells_num
    )
{
    fcs_user_t * user;
    int i;

    user = (fcs_user_t *)user_instance;

    if ((freecells_num < 0) || (freecells_num > MAX_NUM_FREECELLS))
    {     
        return 1;     
    }    

    set_for_all_instances(freecells_num);

    return 0;
}

int freecell_solver_user_set_num_stacks(
    void * user_instance,
    int stacks_num
    )
{
    fcs_user_t * user;
    int i;

    user = (fcs_user_t *)user_instance;

    if ((stacks_num < 0) || (stacks_num > MAX_NUM_STACKS))
    {
        return 1;
    }
    set_for_all_instances(stacks_num);

    return 0;
}

int freecell_solver_user_set_num_decks(
    void * user_instance,
    int decks_num
    )
{
    fcs_user_t * user;
    int i;

    user = (fcs_user_t *)user_instance;

    if ((decks_num < 0) || (decks_num > MAX_NUM_DECKS))
    {
        return 1;
    }
    set_for_all_instances(decks_num);

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

    if (freecell_solver_user_set_num_freecells(user_instance, freecells_num))
    {
        return 1;
    }
    if (freecell_solver_user_set_num_stacks(user_instance, stacks_num))
    {
        return 2;
    }
    if (freecell_solver_user_set_num_decks(user_instance, decks_num))
    {
        return 3;
    }
    if (freecell_solver_user_set_sequences_are_built_by_type(user_instance, sequences_are_built_by))
    {
        return 4;
    }
    if (freecell_solver_user_set_sequence_move(user_instance, unlimited_sequence_move))
    {
        return 5;
    }
    if (freecell_solver_user_set_empty_stacks_filled_by(user_instance, empty_stacks_fill))
    {
        return 6;
    }

    return 0;
}

int freecell_solver_user_get_num_times(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return user->iterations_board_started_at + user->instance->num_times - user->init_num_times;
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

char * freecell_solver_user_move_to_string_w_state(
    void * user_instance,
    fcs_move_t move,
    int standard_notation
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;
    
    return 
        freecell_solver_move_to_string_w_state(
            &(user->running_state), 
            user->instance->freecells_num, 
            user->instance->stacks_num, 
            user->instance->decks_num, 
            move, 
            standard_notation
            );
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
    int sequences_are_built_by
    )
{
    fcs_user_t * user;
    int i;

    user = (fcs_user_t *)user_instance;

    if ((sequences_are_built_by < 0) || (sequences_are_built_by > 2))
    {
        return 1;
    }
    set_for_all_instances(sequences_are_built_by)

    return 0;
}

int freecell_solver_user_set_sequence_move(
    void * user_instance,
    int unlimited_sequence_move
    )
{
    fcs_user_t * user;
    int i;

    user = (fcs_user_t *)user_instance;

    set_for_all_instances(unlimited_sequence_move);

    return 0;
}

int freecell_solver_user_set_empty_stacks_filled_by(
    void * user_instance,
    int empty_stacks_fill
    )
{
    fcs_user_t * user;
    int i;

    user = (fcs_user_t *)user_instance;

    if ((empty_stacks_fill < 0) || (empty_stacks_fill > 2))
    {
        return 1;
    }
    set_for_all_instances(empty_stacks_fill);

    return 0;
}

int freecell_solver_user_set_a_star_weight(
    void * user_instance,
    int weight_index,
    double weight
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if ((weight_index < 0) || (weight_index >= (int)(sizeof(user->soft_thread->a_star_weights)/sizeof(user->soft_thread->a_star_weights[0]))))
    {
        return 1;
    }
    if (weight < 0)
    {
        return 2;
    }

    user->soft_thread->a_star_weights[weight_index] = weight;

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

    freecell_solver_rand_srand(user->soft_thread->rand_gen, (user->soft_thread->rand_seed = seed));
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
    const char * name
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

int freecell_solver_user_set_hard_thread_prelude(
    void * user_instance,
    const char * prelude
    )
{
    fcs_user_t * user;
    freecell_solver_hard_thread_t * hard_thread;

    user = (fcs_user_t *)user_instance;

    hard_thread = user->soft_thread->hard_thread;

    if (hard_thread->prelude_as_string != NULL)
    {
        free(hard_thread->prelude_as_string);
        hard_thread->prelude_as_string = NULL;
    }
    hard_thread->prelude_as_string = strdup(prelude);

    return 0;
}

void freecell_solver_user_recycle(
    void * user_instance
    )
{
    fcs_user_t * user;
    int i;

    user = (fcs_user_t *)user_instance;

    for(i=0;i<user->num_instances;i++)
    {
        recycle_instance(user, i);
    }
    user->current_iterations_limit = -1;
    user->iterations_board_started_at = 0;
    if (user->state_string_copy != NULL)
    {
        free(user->state_string_copy);
        user->state_string_copy = NULL;
    }
}

int freecell_solver_user_set_optimization_scan_tests_order(
    void * user_instance,
    const char * tests_order,
    char * * error_string
    )
{
    fcs_user_t * user;
    int ret;

    user = (fcs_user_t*)user_instance;

    if (user->instance->opt_tests_order.tests)
    {
        free(user->instance->opt_tests_order.tests);
        user->instance->opt_tests_order.tests = NULL;
    }

    user->instance->opt_tests_order_set = 0;

    ret = 
        freecell_solver_apply_tests_order(
            &(user->instance->opt_tests_order),
            tests_order,
            error_string
            );    

    if (!ret)
    {
        user->instance->opt_tests_order_set = 1;
    }

    return ret;
}

void freecell_solver_user_set_reparent_states(
    void * user_instance,
    int to_reparent_states
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->to_reparent_states = to_reparent_states;
}

void freecell_solver_user_set_scans_synergy(
    void * user_instance,
    int synergy
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->scans_synergy = synergy;
}

int freecell_solver_user_next_instance(
    void * user_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;
    
    user->num_instances++;
    if (user->num_instances == user->max_num_instances)
    {
        user->max_num_instances += 10;
        user->instances_list = 
            realloc(
                user->instances_list, 
                sizeof(user->instances_list[0])*user->max_num_instances
                );
    }
    user->current_instance_idx = user->num_instances-1;
    user->instance = freecell_solver_alloc_instance();

    freecell_solver_apply_preset_by_ptr(user->instance, &(user->common_preset));

    /* 
     * Switch the soft_thread variable so it won't refer to the old
     * instance
     * */
    user->soft_thread = 
        freecell_solver_instance_get_soft_thread(
            user->instance, 0, 0
            );

    user->instances_list[user->current_instance_idx].instance = user->instance;
    user->instances_list[user->current_instance_idx].ret = user->ret = FCS_STATE_NOT_BEGAN_YET;
    user->instances_list[user->current_instance_idx].limit = -1;
    
    return 0;
}

int freecell_solver_user_reset(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user_free_resources(user);

    user_initialize(user);

    return 0;
}

