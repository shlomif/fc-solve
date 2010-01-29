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
 * lib.c - library interface functions of Freecell Solver.
 *
 */
#define BUILDING_DLL 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "card.h"
#include "instance.h"
#include "preset.h"
#include "fcs_user.h"
#include "move_funcs_order.h"

#include "unused.h"

typedef struct 
{
    fc_solve_instance_t * instance;
    int ret;
    int limit;
}  fcs_instance_item_t;

typedef struct
{
    /*
     * This is a list of several consecutive instances that are run
     * one after the other in case the previous ones could not solve
     * the board
     * */
    fcs_instance_item_t * instances_list;
    int num_instances;

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
    fc_solve_instance_t * instance;
    fcs_state_keyval_pair_t state;
    fcs_state_keyval_pair_t running_state;
    int ret;
    int state_validity_ret;
    fcs_card_t state_validity_card;
    freecell_solver_user_iter_handler_t iter_handler;
    void * iter_handler_context;

    fc_solve_soft_thread_t * soft_thread;

#ifdef INDIRECT_STACK_STATES
    char indirect_stacks_buffer[MAX_NUM_STACKS << 7];
#endif
    char * state_string_copy;

#ifndef FCS_FREECELL_ONLY
    fcs_preset_t common_preset;
#endif
} fcs_user_t;


static void iter_handler_wrapper(
    void * user_instance,
    int iter_num,
    int depth,
    void * lp_instance GCC_UNUSED,
    fcs_state_extra_info_t * ptr_state_val,
    int parent_iter_num
    );

static void user_initialize(
        fcs_user_t * ret
        )
{
#ifndef FCS_FREECELL_ONLY
    const fcs_preset_t * freecell_preset;

    fc_solve_get_preset_by_name(
        "freecell",
        &freecell_preset
        );

    fcs_duplicate_preset(ret->common_preset, *freecell_preset);
#endif

    ret->instances_list = malloc(sizeof(ret->instances_list[0]));
    ret->num_instances = 1;
    ret->current_instance_idx = 0;
    ret->iter_handler = NULL;
    ret->instance = fc_solve_alloc_instance();
    ret->instance->debug_iter_output_context = ret;
#ifndef FCS_FREECELL_ONLY
    fc_solve_apply_preset_by_ptr(ret->instance, &(ret->common_preset));
#endif
    ret->instances_list[ret->current_instance_idx].instance = ret->instance;
    ret->instances_list[ret->current_instance_idx].ret = ret->ret = FCS_STATE_NOT_BEGAN_YET;
    ret->instances_list[ret->current_instance_idx].limit = -1;
    ret->current_iterations_limit = -1;

    ret->soft_thread =
        fc_solve_instance_get_first_soft_thread(ret->instance);

    ret->state_string_copy = NULL;
    ret->iterations_board_started_at = 0;
}

void DLLEXPORT * freecell_solver_user_alloc(void)
{
    fcs_user_t * ret;

    ret = (fcs_user_t *)malloc(sizeof(fcs_user_t));

    user_initialize(ret);

    return (void*)ret;
}

int DLLEXPORT freecell_solver_user_apply_preset(
    void * user_instance,
    const char * preset_name)
{
#ifdef FCS_FREECELL_ONLY
    return FCS_PRESET_CODE_OK;
#else
    const fcs_preset_t * new_preset_ptr;
    fcs_user_t * user;
    int status;
    int i;

    user = (fcs_user_t*)user_instance;

    status =
        fc_solve_get_preset_by_name(
            preset_name,
            &new_preset_ptr
            );

    if (status != FCS_PRESET_CODE_OK)
    {
        return status;
    }

    for(i = 0 ; i < user->num_instances ; i++)
    {
        status = fc_solve_apply_preset_by_ptr(
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
#endif
}

void DLLEXPORT freecell_solver_user_limit_iterations(
    void * user_instance,
    int max_iters
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    user->current_iterations_limit = max_iters;
}

void DLLEXPORT freecell_solver_user_limit_current_instance_iterations(
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

int DLLEXPORT freecell_solver_user_set_tests_order(
    void * user_instance,
    const char * tests_order,
    char * * error_string
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    return
        fc_solve_apply_tests_order(
            &(user->soft_thread->tests_order),
            tests_order,
            error_string
            );
}

int DLLEXPORT freecell_solver_user_solve_board(
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
        fcs_move_stack_static_destroy(user->instance->solution_moves);
        user->instance->solution_moves.moves = NULL;
    }
    /* fc_solve_unresume_instance is empty. */
#if 0
    else if (user->instances_list[i].ret == FCS_STATE_SUSPEND_PROCESS)
    {
        fc_solve_unresume_instance(user->instances_list[i].instance);
    }
#endif

    if (user->instances_list[i].ret != FCS_STATE_NOT_BEGAN_YET)
    {
        fc_solve_recycle_instance(user->instances_list[i].instance);
        /*
         * We have to initialize init_num_times to 0 here, because it may not
         * get initialized again, and now the num_times of the instance
         * is equal to 0.
         * */
        user->init_num_times = 0;
    }

    user->instances_list[i].ret = FCS_STATE_NOT_BEGAN_YET;
}

int DLLEXPORT freecell_solver_user_resume_solution(
    void * user_instance
    )
{
    int init_num_times;
    int run_for_first_iteration = 1;
    int ret;
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    ret = FCS_STATE_IS_NOT_SOLVEABLE;

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
            int status;
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
            fc_solve_instance_t * instance = user->instance;
#endif

            status = fc_solve_initial_user_state_to_c(
                user->state_string_copy,
                &(user->state.s),
                &(user->state.info),
                INSTANCE_FREECELLS_NUM,
                INSTANCE_STACKS_NUM,
                INSTANCE_DECKS_NUM
#ifdef FCS_WITH_TALONS
                ,user->instance->talon_type
#endif
#ifdef INDIRECT_STACK_STATES
                ,user->indirect_stacks_buffer
#endif
                );

            if (status != FCS_USER_STATE_TO_C__SUCCESS)
            {
                user->ret = FCS_STATE_INVALID_STATE;
                user->state_validity_ret = FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT;
                return user->ret;
            }

            user->state_validity_ret = fc_solve_check_state_validity(
                &(user->state.info),
                INSTANCE_FREECELLS_NUM,
                INSTANCE_STACKS_NUM,
                INSTANCE_DECKS_NUM,
#ifdef FCS_WITH_TALONS
                FCS_TALON_NONE,
#endif
                &(user->state_validity_card));

            if (user->state_validity_ret != FCS_STATE_VALIDITY__OK)
            {
                user->ret = FCS_STATE_INVALID_STATE;
                return user->ret;
            }


            /* running_state is a normalized state. So I'm duplicating
             * state to it before state is canonized
             * */
            fcs_duplicate_state(
                &(user->running_state.s),
                &(user->running_state.info),
                &(user->state.s),
                &(user->state.info)
                );

            fc_solve_canonize_state(
                &user->state.info,
                INSTANCE_FREECELLS_NUM,
                INSTANCE_STACKS_NUM
                );

            fc_solve_init_instance(user->instance);

#define global_limit() \
        (user->instance->num_times + user->current_iterations_limit - user->iterations_board_started_at)
#define local_limit()  \
        (user->instances_list[user->current_instance_idx].limit)
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
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
                fc_solve_solve_instance(user->instance, &user->state.info);
        }
        else
        {

            calc_max_iters();

            user->init_num_times = init_num_times = user->instance->num_times;

            ret = user->ret =
                user->instances_list[user->current_instance_idx].ret =
                fc_solve_resume_instance(user->instance);
        }

        user->iterations_board_started_at += user->instance->num_times - init_num_times;
        user->init_num_times = user->instance->num_times;

        if (user->ret == FCS_STATE_WAS_SOLVED)
        {
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
            fc_solve_instance_t * instance = user->instance;
#endif
            fc_solve_move_stack_normalize(
                &(user->instance->solution_moves),
                &(user->state.info),
                INSTANCE_FREECELLS_NUM,
                INSTANCE_STACKS_NUM,
                INSTANCE_DECKS_NUM
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

int DLLEXPORT freecell_solver_user_get_next_move(
    void * user_instance,
    fcs_move_t * user_move
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    {
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
        fc_solve_instance_t * instance = user->instance;
#endif
        if (user->ret == FCS_STATE_WAS_SOLVED)
        {
            int ret;
#ifdef FCS_USE_COMPACT_MOVE_TOKENS
            fcs_internal_move_t internal_move;
#endif

            ret = fc_solve_move_stack_pop(
                &(user->instance->solution_moves),
#ifdef FCS_USE_COMPACT_MOVE_TOKENS
                &internal_move
#else
                user_move
#endif
                );

#ifdef FCS_USE_COMPACT_MOVE_TOKENS
            /* Convert the internal_move to a user move. */
            fcs_move_set_src_stack(*user_move, fcs_int_move_get_src_stack(internal_move));
            fcs_move_set_dest_stack(*user_move, fcs_int_move_get_dest_stack(internal_move));
            fcs_move_set_type(*user_move, fcs_int_move_get_type(internal_move));
            fcs_move_set_num_cards_in_seq(*user_move, fcs_int_move_get_num_cards_in_seq(internal_move));
#endif

            if (ret == 0)
            {
                fc_solve_apply_move(
                    &(user->running_state.info),
#ifdef FCS_USE_COMPACT_MOVE_TOKENS
                    internal_move,
#else
                    *user_move,
#endif
                    INSTANCE_FREECELLS_NUM,
                    INSTANCE_STACKS_NUM,
                    INSTANCE_DECKS_NUM
                    );
            }
            return ret;
        }
        else
        {
            return 1;
        }
    }
}

DLLEXPORT char * freecell_solver_user_current_state_as_string(
    void * user_instance,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    {
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
        fc_solve_instance_t * instance = user->instance;
#endif

        return
            fc_solve_state_as_string(
                &(user->running_state.info),
                INSTANCE_FREECELLS_NUM,
                INSTANCE_STACKS_NUM,
                INSTANCE_DECKS_NUM,
                parseable_output,
                canonized_order_output,
                display_10_as_t
                );
    }
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
            fcs_move_stack_static_destroy(user->instance->solution_moves);
            user->instance->solution_moves.moves = NULL;
        }
        /* fc_solve_unresume_instance is empty. */
#if 0
        else if (ret_code == FCS_STATE_SUSPEND_PROCESS)
        {
            fc_solve_unresume_instance(user->instances_list[i].instance);
        }
#endif

        if (ret_code != FCS_STATE_NOT_BEGAN_YET)
        {
            if (ret_code != FCS_STATE_INVALID_STATE)
            {
                fc_solve_finish_instance(user->instances_list[i].instance);
            }
        }

        fc_solve_free_instance(user->instances_list[i].instance);
    }

    free(user->instances_list);

    if (user->state_string_copy != NULL)
    {
        free(user->state_string_copy);
        user->state_string_copy = NULL;
    }
}

void DLLEXPORT freecell_solver_user_free(
    void * user_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user_free_resources(user);

    free(user);
}

int DLLEXPORT freecell_solver_user_get_current_depth(
    void * user_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return (user->soft_thread->method_specific.soft_dfs.depth);
}

void DLLEXPORT freecell_solver_user_set_solving_method(
    void * user_instance,
    int method
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->soft_thread->method = method;

    switch (method)
    {
        case FCS_METHOD_A_STAR:
        {
            double * my_a_star_weights;
            int a;

            my_a_star_weights = user->soft_thread->method_specific.befs.meth.befs.a_star_weights;
            for(a = 0;
                a<(sizeof(fc_solve_a_star_default_weights)/
                    sizeof(fc_solve_a_star_default_weights[0]));
                a++)
            {
                my_a_star_weights[a] = fc_solve_a_star_default_weights[a];
            }

            user->soft_thread->method_specific.befs.meth.befs.a_star_pqueue.Elements = NULL;
        }
        break;
        case FCS_METHOD_OPTIMIZE:
        case FCS_METHOD_BFS:
        {
            user->soft_thread->method_specific.befs.meth.brfs.bfs_queue =
            user->soft_thread->method_specific.befs.meth.brfs.bfs_queue_last_item =
            NULL;
        }
        break;
    }
}

static void apply_game_params_for_all_instances(
        fcs_user_t * user
        )
{
    int i;

    for(i = 0 ; i < user->num_instances ; i++)
    {
        user->instances_list[i].instance->game_params = 
            user->common_preset.game_params;
    }

    return;
}

#ifndef HARD_CODED_NUM_FREECELLS
int DLLEXPORT freecell_solver_user_set_num_freecells(
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

    user->common_preset.game_params.freecells_num = freecells_num;

    apply_game_params_for_all_instances(user);

    return 0;
}
#else
int DLLEXPORT freecell_solver_user_set_num_freecells(
    void * user_instance GCC_UNUSED,
    int freecells_num GCC_UNUSED
    )
{
    return 0;
}
#endif

#ifndef HARD_CODED_NUM_STACKS
int DLLEXPORT freecell_solver_user_set_num_stacks(
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
    user->common_preset.game_params.stacks_num = stacks_num;
    apply_game_params_for_all_instances(user);

    return 0;
}
#else
int DLLEXPORT freecell_solver_user_set_num_stacks(
    void * user_instance GCC_UNUSED,
    int stacks_num GCC_UNUSED
    )
{
    return 0;
}
#endif

#ifndef HARD_CODED_NUM_DECKS
int DLLEXPORT freecell_solver_user_set_num_decks(
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

    user->common_preset.game_params.decks_num = decks_num;
    apply_game_params_for_all_instances(user);

    return 0;
}
#else
int DLLEXPORT freecell_solver_user_set_num_decks(
    void * user_instance GCC_UNUSED,
    int decks_num GCC_UNUSED
    )
{
    return 0;
}
#endif

int DLLEXPORT freecell_solver_user_set_game(
    void * user_instance,
    int freecells_num,
    int stacks_num,
    int decks_num,
    int sequences_are_built_by,
    int unlimited_sequence_move,
    int empty_stacks_fill
    )
{
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

int DLLEXPORT freecell_solver_user_get_num_times(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return user->iterations_board_started_at + user->instance->num_times - user->init_num_times;
}

int DLLEXPORT freecell_solver_user_get_limit_iterations(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return user->instance->max_num_times;
}

int DLLEXPORT freecell_solver_user_get_moves_left(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;
    if (user->ret == FCS_STATE_WAS_SOLVED)
        return user->instance->solution_moves.num_moves;
    else
        return 0;
}

void DLLEXPORT freecell_solver_user_set_solution_optimization(
    void * user_instance,
    int optimize
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->optimize_solution_path = optimize;
}

DLLEXPORT char * freecell_solver_user_move_to_string(
    fcs_move_t move,
    int standard_notation
    )
{
    return fc_solve_move_to_string(move, standard_notation);
}

DLLEXPORT char * freecell_solver_user_move_to_string_w_state(
    void * user_instance,
    fcs_move_t move,
    int standard_notation
    )
{
    fcs_user_t * user;
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
    fc_solve_instance_t * instance;
#endif

    user = (fcs_user_t *)user_instance;
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))    
    instance = user->instance;
#endif

    return
        fc_solve_move_to_string_w_state(
            &(user->running_state.info),
            INSTANCE_FREECELLS_NUM,
            INSTANCE_STACKS_NUM,
            INSTANCE_DECKS_NUM,
            move,
            standard_notation
            );
}

void DLLEXPORT freecell_solver_user_limit_depth(
    void * user_instance,
    int max_depth
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->max_depth = max_depth;
}

int DLLEXPORT freecell_solver_user_get_max_num_freecells(void)
{
    return MAX_NUM_FREECELLS;
}

int DLLEXPORT freecell_solver_user_get_max_num_stacks(void)
{
    return MAX_NUM_STACKS;
}

int DLLEXPORT freecell_solver_user_get_max_num_decks(void)
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

    if (user->state_validity_ret == FCS_STATE_VALIDITY__OK)
    {
        return strdup("");
    }
    fc_solve_card_perl2user(user->state_validity_card, card_str, print_ts);

    if (user->state_validity_ret == FCS_STATE_VALIDITY__EMPTY_SLOT)
    {
        sprintf(string, "%s",
            "There's an empty slot in one of the stacks."
            );
    }
    else if ((user->state_validity_ret == FCS_STATE_VALIDITY__EXTRA_CARD) ||
           (user->state_validity_ret == FCS_STATE_VALIDITY__MISSING_CARD)
          )
    {
        sprintf(string, "%s%s.",
            ((user->state_validity_ret == FCS_STATE_VALIDITY__EXTRA_CARD)? "There's an extra card: " : "There's a missing card: "),
            card_str
        );
    }
    else if (user->state_validity_ret == FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT)
    {
        sprintf(string, "%s.", "Not enough input");
    }
    return strdup(string);
}

int DLLEXPORT freecell_solver_user_set_sequences_are_built_by_type(
    void * user_instance,
    int sequences_are_built_by
    )
{
#ifndef FCS_FREECELL_ONLY
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if ((sequences_are_built_by < 0) || (sequences_are_built_by > 2))
    {
        return 1;
    }
    
    user->common_preset.game_params.game_flags &= (~0x3);
    user->common_preset.game_params.game_flags |= sequences_are_built_by;

    apply_game_params_for_all_instances(user);
#endif

    return 0;
}

int DLLEXPORT freecell_solver_user_set_sequence_move(
    void * user_instance,
    int unlimited_sequence_move
    )
{
#ifndef FCS_FREECELL_ONLY
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->common_preset.game_params.game_flags &= (~(1 << 4));
    user->common_preset.game_params.game_flags |=
        ((unlimited_sequence_move != 0)<< 4);

    apply_game_params_for_all_instances(user);
#endif
    return 0;
}

int DLLEXPORT freecell_solver_user_set_empty_stacks_filled_by(
    void * user_instance,
    int empty_stacks_fill
    )
{
#ifndef FCS_FREECELL_ONLY
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    if ((empty_stacks_fill < 0) || (empty_stacks_fill > 2))
    {
        return 1;
    }

    user->common_preset.game_params.game_flags &= (~(0x3 << 2));
    user->common_preset.game_params.game_flags |=
        (empty_stacks_fill << 2);

    apply_game_params_for_all_instances(user);
#endif
    return 0;
}

int DLLEXPORT freecell_solver_user_set_a_star_weight(
    void * user_instance,
    int index,
    double weight
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

#define my_a_star_weights soft_thread->method_specific.befs.meth.befs.a_star_weights
    if ((index < 0) || (index >= (sizeof(user->my_a_star_weights)/sizeof(user->my_a_star_weights[0]))))
    {
        return 1;
    }
    if (weight < 0)
    {
        return 2;
    }

    user->my_a_star_weights[index] = weight;

    return 0;

}


#ifdef FCS_COMPILE_DEBUG_FUNCTIONS
double DLLEXPORT fc_solve_user_INTERNAL_get_befs_weight(
    void * user_instance,
    int index
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

#define my_a_star_weights soft_thread->method_specific.befs.meth.befs.a_star_weights

    return user->my_a_star_weights[index];
}

#endif

static void iter_handler_wrapper(
    void * user_instance,
    int iter_num,
    int depth,
    void * lp_instance GCC_UNUSED,
    fcs_state_extra_info_t * ptr_state_val,
    int parent_iter_num
    )
{
    fcs_user_t * user;

    fcs_standalone_state_ptrs_t state;
    state.key = ptr_state_val->key;
    state.val = ptr_state_val;

    user = (fcs_user_t *)user_instance;

#ifdef DEBUG
    {
        fc_solve_instance_t * instance = user->instance;
        HT_LOOP_DECLARE_VARS();

        HT_LOOP_START()
        {
            ST_LOOP_DECLARE_VARS();

            ST_LOOP_START()
            {
                if (!strcmp(soft_thread->name, "11"))
                {
                    double * w =
                        soft_thread->method_specific.befs.meth.
                                    befs.a_star_weights
                        ;

                    printf("BeFS-Weights[\"11\"]=(%f,%f,%f,%f,%f)\n",
                            w[0], w[1], w[2], w[3], w[4]
                          );

                    goto myend;
                }
            }
        }
                
    }
myend:
#endif
    user->iter_handler(
        user_instance,
        iter_num,
        depth,
        (void *)&state,
        parent_iter_num,
        user->iter_handler_context
        );

    return;
}

void DLLEXPORT freecell_solver_user_set_iter_handler(
    void * user_instance,
    freecell_solver_user_iter_handler_t iter_handler,
    void * iter_handler_context
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->iter_handler = iter_handler;

    if (iter_handler == NULL)
    {
        int i;

        for (i=0; i < user->num_instances ; i++)
        {
            user->instances_list[i].instance->debug_iter_output_func = NULL;
        }
    }
    else
    {
        int i;

        user->iter_handler_context = iter_handler_context;
        for (i=0; i < user->num_instances ; i++)
        {
            user->instances_list[i].instance->debug_iter_output_func 
                = iter_handler_wrapper;
        }
    }
}

#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
#define HARD_CODED_UNUSED 
#else
#define HARD_CODED_UNUSED GCC_UNUSED
#endif

DLLEXPORT char * freecell_solver_user_iter_state_as_string(
    void * user_instance HARD_CODED_UNUSED,
    void * ptr_state_void,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
)
{
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
    fcs_user_t * user;
    fc_solve_instance_t * instance;
#endif
    fcs_standalone_state_ptrs_t * ptr_state;

#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
    user = (fcs_user_t *)user_instance;
    instance = user->instance;
#endif

    ptr_state = (fcs_standalone_state_ptrs_t *)ptr_state_void;

    return
        fc_solve_state_as_string(
            ptr_state->val,
            INSTANCE_FREECELLS_NUM,
            INSTANCE_STACKS_NUM,
            INSTANCE_DECKS_NUM,
            parseable_output,
            canonized_order_output,
            display_10_as_t
            );
}

void DLLEXPORT freecell_solver_user_set_random_seed(
    void * user_instance,
    int seed
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    fc_solve_rand_init(
            &(user->soft_thread->method_specific.soft_dfs.rand_gen), 
            (user->soft_thread->method_specific.soft_dfs.rand_seed = seed)
            );
}

int DLLEXPORT freecell_solver_user_get_num_states_in_collection(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return user->instance->num_states_in_collection;
}

void DLLEXPORT freecell_solver_user_limit_num_states_in_collection(
    void * user_instance,
    int max_num_states
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)user_instance;

    user->instance->max_num_states_in_collection = max_num_states;
}

int DLLEXPORT freecell_solver_user_next_soft_thread(
    void * user_instance
    )
{
    fcs_user_t * user;
    fc_solve_soft_thread_t * soft_thread;

    user = (fcs_user_t *)user_instance;

    soft_thread = fc_solve_new_soft_thread(user->soft_thread->hard_thread);

    if (soft_thread == NULL)
    {
        return 1;
    }

    user->soft_thread = soft_thread;

    return 0;
}

extern void DLLEXPORT freecell_solver_user_set_soft_thread_step(
    void * user_instance,
    int num_times_step
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->soft_thread->num_times_step = num_times_step;
}

int DLLEXPORT freecell_solver_user_next_hard_thread(
    void * user_instance
    )
{
    fcs_user_t * user;
    fc_solve_soft_thread_t * soft_thread;

    user = (fcs_user_t *)user_instance;

    soft_thread = fc_solve_new_hard_thread(user->instance);

    if (soft_thread == NULL)
    {
        return 1;
    }

    user->soft_thread = soft_thread;

    return 0;
}

int DLLEXPORT freecell_solver_user_get_num_soft_threads_in_instance(
    void * user_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    return user->instance->next_soft_thread_id;
}

void DLLEXPORT freecell_solver_user_set_calc_real_depth(
    void * user_instance,
    int calc_real_depth
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->calc_real_depth = calc_real_depth;
}

void DLLEXPORT freecell_solver_user_set_soft_thread_name(
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

int DLLEXPORT freecell_solver_user_set_hard_thread_prelude(
    void * user_instance,
    char * prelude
    )
{
    fcs_user_t * user;
    fc_solve_hard_thread_t * hard_thread;

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

void DLLEXPORT freecell_solver_user_recycle(
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

int DLLEXPORT freecell_solver_user_set_optimization_scan_tests_order(
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
        fc_solve_apply_tests_order(
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

void DLLEXPORT freecell_solver_user_set_reparent_states(
    void * user_instance,
    int to_reparent_states
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->to_reparent_states_proto = to_reparent_states;
}

void DLLEXPORT freecell_solver_user_set_scans_synergy(
    void * user_instance,
    int synergy
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instance->scans_synergy = synergy;
}

int DLLEXPORT freecell_solver_user_next_instance(
    void * user_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user->instances_list =
        realloc(
            user->instances_list,
            sizeof(user->instances_list[0])
            * (++user->num_instances)
            );

    user->current_instance_idx = user->num_instances-1;
    user->instance = fc_solve_alloc_instance();

#ifndef FCS_FREECELL_ONLY
    fc_solve_apply_preset_by_ptr(user->instance, &(user->common_preset));
#endif

    /*
     * Switch the soft_thread variable so it won't refer to the old
     * instance
     * */
    user->soft_thread =
        fc_solve_instance_get_first_soft_thread(user->instance);

    user->instances_list[user->current_instance_idx].instance = user->instance;
    user->instances_list[user->current_instance_idx].ret = user->ret = FCS_STATE_NOT_BEGAN_YET;
    user->instances_list[user->current_instance_idx].limit = -1;

    user->instance->debug_iter_output_func =
        (user->iter_handler ? iter_handler_wrapper : NULL);
    user->instance->debug_iter_output_context = user;

    return 0;
}

int DLLEXPORT freecell_solver_user_reset(void * user_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)user_instance;

    user_free_resources(user);

    user_initialize(user);

    return 0;
}

DLLEXPORT const char * freecell_solver_user_get_lib_version(
    void * user_instance GCC_UNUSED
    )
{
    return VERSION;
}

DLLEXPORT const char * freecell_solver_user_get_current_soft_thread_name(
    void * user_instance
    )
{
    fcs_user_t * user;
    fc_solve_hard_thread_t * hard_thread;
    fc_solve_instance_t * instance;

    user = (fcs_user_t *)user_instance;

    instance = user->instance;

    hard_thread = &(instance->hard_threads[instance->ht_idx]);

    return hard_thread->soft_threads[hard_thread->st_idx].name;
}
