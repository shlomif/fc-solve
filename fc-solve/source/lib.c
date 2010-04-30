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

/* A flare is an alternative scan algorithm to be tried. All flares in
 * a single instance are being evaluated and then one picks the shortest 
 * solution out of all of them. (see fc-solve/docs/flares-functional-spec.txt )
 * */

typedef struct
{
    fc_solve_instance_t * obj;
    int ret_code;
    int limit;
} fcs_flare_item_t;

typedef struct 
{
    int num_flares;
    fcs_flare_item_t flares[1];
    int ret_code;
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
    fc_solve_instance_t * fc_solve_obj;
    fcs_state_keyval_pair_t state;
    fcs_state_keyval_pair_t running_state;
    int ret_code;
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
    void * api_instance,
    int iter_num,
    int depth,
    void * lp_instance GCC_UNUSED,
    fcs_state_extra_info_t * ptr_state_val,
    int parent_iter_num
    );

#define FLARES_LOOP_DECLARE_VARS() \
    int user_inst_idx

#define FLARES_LOOP_START() \
    for(user_inst_idx = 0 ; user_inst_idx < user->num_instances ; user_inst_idx++) \
    { \
        fcs_instance_item_t * instance_item; \
        int flare_idx; \
           \
        instance_item = &(user->instances_list[user_inst_idx]); \
                  \
        for(flare_idx = 0; flare_idx < instance_item->num_flares; flare_idx++) \
        {      \
            fcs_flare_item_t * flare; \
                       \
            flare = &(instance_item->flares[flare_idx]);

#define FLARES_LOOP_END() \
        } \
    }

static void user_initialize(
        fcs_user_t * user
        )
{
    fcs_instance_item_t * instance_item;
    fcs_flare_item_t * flare;
#ifndef FCS_FREECELL_ONLY
    const fcs_preset_t * freecell_preset;

    fc_solve_get_preset_by_name(
        "freecell",
        &freecell_preset
        );

    fcs_duplicate_preset(user->common_preset, *freecell_preset);
#endif

    user->instances_list = malloc(sizeof(user->instances_list[0]));
    user->num_instances = 1;
    user->current_instance_idx = 0;
    user->iter_handler = NULL;
    user->fc_solve_obj = fc_solve_alloc_instance();
    user->fc_solve_obj->debug_iter_output_context = user;
#ifndef FCS_FREECELL_ONLY
    fc_solve_apply_preset_by_ptr(user->fc_solve_obj, &(user->common_preset));
#endif
    instance_item = &(user->instances_list[user->current_instance_idx]);
    instance_item->num_flares = 1;
    /* TODO : Later - allocate the flares array. */

    flare = &(instance_item->flares[instance_item->num_flares-1]);

    flare->obj = user->fc_solve_obj;
    flare->ret_code =
        instance_item->ret_code =
        user->ret_code =
        FCS_STATE_NOT_BEGAN_YET;
    flare->limit = instance_item->limit = -1;
    user->current_iterations_limit = -1;

    user->soft_thread =
        fc_solve_instance_get_first_soft_thread(user->fc_solve_obj);

    user->state_string_copy = NULL;
    user->iterations_board_started_at = 0;
}

void DLLEXPORT * freecell_solver_user_alloc(void)
{
    fcs_user_t * ret;

    ret = (fcs_user_t *)malloc(sizeof(fcs_user_t));

    user_initialize(ret);

    return (void*)ret;
}

int DLLEXPORT freecell_solver_user_apply_preset(
    void * from_user_instance,
    const char * preset_name)
{
#ifdef FCS_FREECELL_ONLY
    return FCS_PRESET_CODE_OK;
#else
    const fcs_preset_t * new_preset_ptr;
    fcs_user_t * user;
    int status;
    FLARES_LOOP_DECLARE_VARS();

    user = (fcs_user_t*)from_user_instance;

    status =
        fc_solve_get_preset_by_name(
            preset_name,
            &new_preset_ptr
            );

    if (status != FCS_PRESET_CODE_OK)
    {
        return status;
    }

    FLARES_LOOP_START()
        status = fc_solve_apply_preset_by_ptr(
            instance_item->flares[flare_idx].obj,
            new_preset_ptr
            );

        if (status != FCS_PRESET_CODE_OK)
        {
            return status;
        }
    FLARES_LOOP_END()

    fcs_duplicate_preset(user->common_preset, *new_preset_ptr);

    return FCS_PRESET_CODE_OK;
#endif
}

void DLLEXPORT freecell_solver_user_limit_iterations(
    void * api_instance,
    int max_iters
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)api_instance;

    user->current_iterations_limit = max_iters;
}

void DLLEXPORT freecell_solver_user_limit_current_instance_iterations(
    void * api_instance,
    int max_iters
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)api_instance;

    user->instances_list[user->current_instance_idx].limit = max_iters;
}

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

int DLLEXPORT freecell_solver_user_set_tests_order(
    void * api_instance,
    const char * tests_order,
    char * * error_string
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)api_instance;

    return
        fc_solve_apply_tests_order(
            &(user->soft_thread->tests_order),
            tests_order,
            error_string
            );
}

int DLLEXPORT freecell_solver_user_solve_board(
    void * api_instance,
    const char * state_as_string
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)api_instance;

    user->state_string_copy = strdup(state_as_string);

    user->current_instance_idx = 0;

    return freecell_solver_user_resume_solution(api_instance);
}

static void recycle_instance(
    fcs_user_t * user,
    int i
    )
{
    int flare_idx;
    fcs_instance_item_t * instance_item;

    instance_item = &(user->instances_list[i]);

    if (instance_item->ret_code == FCS_STATE_WAS_SOLVED)
    {
        fcs_move_stack_static_destroy(user->fc_solve_obj->solution_moves);
        user->fc_solve_obj->solution_moves.moves = NULL;
    }
    /* fc_solve_unresume_instance is empty. */
#if 0
    else if (user->instances_list[i].ret == FCS_STATE_SUSPEND_PROCESS)
    {
        fc_solve_unresume_instance(user->instances_list[i].instance);
    }
#endif

    for(flare_idx = 0; flare_idx < instance_item->num_flares ; flare_idx++)
    {
        fcs_flare_item_t * flare;

        flare = &(instance_item->flares[flare_idx]);

        if (flare->ret_code != FCS_STATE_NOT_BEGAN_YET)
        {
            fc_solve_recycle_instance(flare->obj);
            /*
             * We have to initialize init_num_times to 0 here, because it may 
             * not get initialized again, and now the num_times of the instance
             * is equal to 0.
             * */
            user->init_num_times = 0;

            flare->ret_code = FCS_STATE_NOT_BEGAN_YET;
        }
    }

    instance_item->ret_code = FCS_STATE_NOT_BEGAN_YET;
}

int DLLEXPORT freecell_solver_user_resume_solution(
    void * api_instance
    )
{
    int init_num_times;
    int run_for_first_iteration = 1;
    int ret;
    fcs_user_t * user;

    user = (fcs_user_t*)api_instance;

    ret = FCS_STATE_IS_NOT_SOLVEABLE;

    /*
     * I expect user->current_instance_idx to be initialized at some value.
     * */
    for( ;
        run_for_first_iteration || ((user->current_instance_idx < user->num_instances) && (ret == FCS_STATE_IS_NOT_SOLVEABLE)) ;
        recycle_instance(user, user->current_instance_idx), user->current_instance_idx++
       )
    {
        fcs_instance_item_t * instance_item;
        fcs_flare_item_t * flare;
        int flare_idx;

        run_for_first_iteration = 0;

        instance_item = &(user->instances_list[user->current_instance_idx]);
        /* TODO : For later - loop over the flares based on the flares plan. */
        user->fc_solve_obj = instance_item->flares[0].obj;

        flare_idx = 0;

        flare = &(instance_item->flares[flare_idx]);

        if (instance_item->ret_code == FCS_STATE_NOT_BEGAN_YET)
        {
            int status;

#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
            fc_solve_instance_t * instance = user->fc_solve_obj;
#endif

            status = fc_solve_initial_user_state_to_c(
                user->state_string_copy,
                &(user->state.s),
                &(user->state.info),
                INSTANCE_FREECELLS_NUM,
                INSTANCE_STACKS_NUM,
                INSTANCE_DECKS_NUM
#ifdef FCS_WITH_TALONS
                ,user->fc_solve_obj->talon_type
#endif
#ifdef INDIRECT_STACK_STATES
                ,user->indirect_stacks_buffer
#endif
                );

            if (status != FCS_USER_STATE_TO_C__SUCCESS)
            {
                user->ret_code = FCS_STATE_INVALID_STATE;
                user->state_validity_ret = FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT;
                return user->ret_code;
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
                user->ret_code = FCS_STATE_INVALID_STATE;
                return user->ret_code;
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

            fc_solve_init_instance(user->fc_solve_obj);

#define global_limit() \
        (user->fc_solve_obj->num_times + user->current_iterations_limit - user->iterations_board_started_at)
#define local_limit()  \
        (instance_item->limit)
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#define calc_max_iters() \
        {          \
            if (instance_item->limit < 0)  \
            {\
                if (user->current_iterations_limit < 0)\
                {\
                    user->fc_solve_obj->max_num_times = -1;\
                }\
                else\
                {\
                    user->fc_solve_obj->max_num_times = global_limit();\
                }\
            }\
            else\
            {\
                if (user->current_iterations_limit < 0)\
                {\
                    user->fc_solve_obj->max_num_times = local_limit();\
                }\
                else\
                {\
                    int a, b;\
                    \
                    a = global_limit();\
                    b = local_limit();\
        \
                    user->fc_solve_obj->max_num_times = min(a,b);\
                }\
            }\
        }


            calc_max_iters();

            user->init_num_times = init_num_times = user->fc_solve_obj->num_times;

            ret = user->ret_code =
                instance_item->ret_code =
                flare->ret_code =
                fc_solve_solve_instance(user->fc_solve_obj, &user->state.info);
        }
        else
        {

            calc_max_iters();

            user->init_num_times = init_num_times = user->fc_solve_obj->num_times;

            ret = user->ret_code =
                instance_item->ret_code =
                flare->ret_code =
                fc_solve_resume_instance(user->fc_solve_obj);
        }

        user->iterations_board_started_at += user->fc_solve_obj->num_times - init_num_times;
        user->init_num_times = user->fc_solve_obj->num_times;

        if (user->ret_code == FCS_STATE_WAS_SOLVED)
        {
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
            fc_solve_instance_t * instance = 
                user->fc_solve_obj;
#endif
            fc_solve_move_stack_normalize(
                &(user->fc_solve_obj->solution_moves),
                &(user->state.info),
                INSTANCE_FREECELLS_NUM,
                INSTANCE_STACKS_NUM,
                INSTANCE_DECKS_NUM
                );

            break;
        }
        else if (user->ret_code == FCS_STATE_SUSPEND_PROCESS)
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
                (user->fc_solve_obj->num_times >= local_limit())
               )
            {
                ret = FCS_STATE_IS_NOT_SOLVEABLE;
            }
        }
    }

    return ret;
}

int DLLEXPORT freecell_solver_user_get_next_move(
    void * api_instance,
    fcs_move_t * user_move
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)api_instance;

    {
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
        fc_solve_instance_t * instance = 
            user->fc_solve_obj;
#endif
        if (user->ret_code == FCS_STATE_WAS_SOLVED)
        {
            int ret;
#ifdef FCS_USE_COMPACT_MOVE_TOKENS
            fcs_internal_move_t internal_move;
#endif

            ret = fc_solve_move_stack_pop(
                &(user->fc_solve_obj->solution_moves),
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
    void * api_instance,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    {
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
        fc_solve_instance_t * instance = user->fc_solve_obj;
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
    FLARES_LOOP_DECLARE_VARS();

    FLARES_LOOP_START()
    {
        int ret_code;

        ret_code = flare->ret_code;

        /*  TODO : for later It's possible two flares in a single-instance
         *  will be solved. Make sure the check is instance-wide.
         *  */
        if (ret_code == FCS_STATE_WAS_SOLVED)
        {
            fcs_move_stack_static_destroy(user->fc_solve_obj->solution_moves);
            user->fc_solve_obj->solution_moves.moves = NULL;
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
                fc_solve_finish_instance(flare->obj);
            }
        }

        fc_solve_free_instance(flare->obj);
    }
    FLARES_LOOP_END()

    free(user->instances_list);

    if (user->state_string_copy != NULL)
    {
        free(user->state_string_copy);
        user->state_string_copy = NULL;
    }
}

void DLLEXPORT freecell_solver_user_free(
    void * api_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    user_free_resources(user);

    free(user);
}

int DLLEXPORT freecell_solver_user_get_current_depth(
    void * api_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    return (user->soft_thread->method_specific.soft_dfs.depth);
}

void DLLEXPORT freecell_solver_user_set_solving_method(
    void * api_instance,
    int method
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

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
#ifndef FCS_FREECELL_ONLY
static void apply_game_params_for_all_instances(
        fcs_user_t * user
        )
{
    FLARES_LOOP_DECLARE_VARS();

    FLARES_LOOP_START()
        flare->obj->game_params = user->common_preset.game_params;
    FLARES_LOOP_END()

    return;
}
#endif

#ifndef HARD_CODED_NUM_FREECELLS
int DLLEXPORT freecell_solver_user_set_num_freecells(
    void * api_instance,
    int freecells_num
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

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
    void * api_instance GCC_UNUSED,
    int freecells_num GCC_UNUSED
    )
{
    return 0;
}
#endif

#ifndef HARD_CODED_NUM_STACKS
int DLLEXPORT freecell_solver_user_set_num_stacks(
    void * api_instance,
    int stacks_num
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

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
    void * api_instance GCC_UNUSED,
    int stacks_num GCC_UNUSED
    )
{
    return 0;
}
#endif

#ifndef HARD_CODED_NUM_DECKS
int DLLEXPORT freecell_solver_user_set_num_decks(
    void * api_instance,
    int decks_num
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

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
    void * api_instance GCC_UNUSED,
    int decks_num GCC_UNUSED
    )
{
    return 0;
}
#endif

int DLLEXPORT freecell_solver_user_set_game(
    void * api_instance,
    int freecells_num,
    int stacks_num,
    int decks_num,
    int sequences_are_built_by,
    int unlimited_sequence_move,
    int empty_stacks_fill
    )
{
    if (freecell_solver_user_set_num_freecells(api_instance, freecells_num))
    {
        return 1;
    }
    if (freecell_solver_user_set_num_stacks(api_instance, stacks_num))
    {
        return 2;
    }
    if (freecell_solver_user_set_num_decks(api_instance, decks_num))
    {
        return 3;
    }
    if (freecell_solver_user_set_sequences_are_built_by_type(api_instance, sequences_are_built_by))
    {
        return 4;
    }
    if (freecell_solver_user_set_sequence_move(api_instance, unlimited_sequence_move))
    {
        return 5;
    }
    if (freecell_solver_user_set_empty_stacks_filled_by(api_instance, empty_stacks_fill))
    {
        return 6;
    }

    return 0;
}

int DLLEXPORT freecell_solver_user_get_num_times(void * api_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    return user->iterations_board_started_at + user->fc_solve_obj->num_times - user->init_num_times;
}

int DLLEXPORT freecell_solver_user_get_limit_iterations(void * api_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    return user->fc_solve_obj->max_num_times;
}

int DLLEXPORT freecell_solver_user_get_moves_left(void * api_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;
    if (user->ret_code == FCS_STATE_WAS_SOLVED)
        return user->fc_solve_obj->solution_moves.num_moves;
    else
        return 0;
}

void DLLEXPORT freecell_solver_user_set_solution_optimization(
    void * api_instance,
    int optimize
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    STRUCT_SET_FLAG_TO(user->fc_solve_obj, FCS_RUNTIME_OPTIMIZE_SOLUTION_PATH, optimize);
}

DLLEXPORT char * freecell_solver_user_move_to_string(
    fcs_move_t move,
    int standard_notation
    )
{
    return fc_solve_move_to_string(move, standard_notation);
}

DLLEXPORT char * freecell_solver_user_move_to_string_w_state(
    void * api_instance,
    fcs_move_t move,
    int standard_notation
    )
{
    fcs_user_t * user;
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
    fc_solve_instance_t * instance;
#endif

    user = (fcs_user_t *)api_instance;
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))    
    instance = 
        user->fc_solve_obj;
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
    void * api_instance,
    int max_depth
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    user->fc_solve_obj->max_depth = max_depth;
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
    void * api_instance,
    int print_ts
    )
{
    fcs_user_t * user;
    char string[80], card_str[10];

    user = (fcs_user_t *)api_instance;

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
    void * api_instance,
    int sequences_are_built_by
    )
{
#ifndef FCS_FREECELL_ONLY
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

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
    void * api_instance,
    int unlimited_sequence_move
    )
{
#ifndef FCS_FREECELL_ONLY
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    user->common_preset.game_params.game_flags &= (~(1 << 4));
    user->common_preset.game_params.game_flags |=
        ((unlimited_sequence_move != 0)<< 4);

    apply_game_params_for_all_instances(user);
#endif
    return 0;
}

int DLLEXPORT freecell_solver_user_set_empty_stacks_filled_by(
    void * api_instance,
    int empty_stacks_fill
    )
{
#ifndef FCS_FREECELL_ONLY
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

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
    void * api_instance,
    int index,
    double weight
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

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
    void * api_instance,
    int index
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

#define my_a_star_weights soft_thread->method_specific.befs.meth.befs.a_star_weights

    return user->my_a_star_weights[index];
}

#endif

static void iter_handler_wrapper(
    void * api_instance,
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

    user = (fcs_user_t *)api_instance;

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
        api_instance,
        iter_num,
        depth,
        (void *)&state,
        parent_iter_num,
        user->iter_handler_context
        );

    return;
}

void set_debug_iter_output_func_to_val(fcs_user_t * user,
        void (*value)(
    void * api_instance,
    int iter_num,
    int depth,
    void * lp_instance GCC_UNUSED,
    fcs_state_extra_info_t * ptr_state_val,
    int parent_iter_num
    ))
{
    FLARES_LOOP_DECLARE_VARS();
    FLARES_LOOP_START()
       flare->obj->debug_iter_output_func = value; 
    FLARES_LOOP_END()
}

void DLLEXPORT freecell_solver_user_set_iter_handler(
    void * api_instance,
    freecell_solver_user_iter_handler_t iter_handler,
    void * iter_handler_context
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    user->iter_handler = iter_handler;

    if (iter_handler == NULL)
    {
        set_debug_iter_output_func_to_val(user, NULL);
    }
    else
    {
        user->iter_handler_context = iter_handler_context;
        set_debug_iter_output_func_to_val(user, iter_handler_wrapper);
    }
}

#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) && defined(HARD_CODED_NUM_DECKS)))
#define HARD_CODED_UNUSED 
#else
#define HARD_CODED_UNUSED GCC_UNUSED
#endif

DLLEXPORT char * freecell_solver_user_iter_state_as_string(
    void * api_instance HARD_CODED_UNUSED,
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
    user = (fcs_user_t *)api_instance;
    instance = 
        user->fc_solve_obj;
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
    void * api_instance,
    int seed
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    fc_solve_rand_init(
            &(user->soft_thread->method_specific.soft_dfs.rand_gen), 
            (user->soft_thread->method_specific.soft_dfs.rand_seed = seed)
            );
}

int DLLEXPORT freecell_solver_user_get_num_states_in_collection(void * api_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    return user->fc_solve_obj->num_states_in_collection;
}

void DLLEXPORT freecell_solver_user_limit_num_states_in_collection(
    void * api_instance,
    int max_num_states
    )
{
    fcs_user_t * user;

    user = (fcs_user_t*)api_instance;

    user->fc_solve_obj->max_num_states_in_collection = max_num_states;
}

int DLLEXPORT freecell_solver_user_next_soft_thread(
    void * api_instance
    )
{
    fcs_user_t * user;
    fc_solve_soft_thread_t * soft_thread;

    user = (fcs_user_t *)api_instance;

    soft_thread = fc_solve_new_soft_thread(user->soft_thread->hard_thread);

    if (soft_thread == NULL)
    {
        return 1;
    }

    user->soft_thread = soft_thread;

    return 0;
}

extern void DLLEXPORT freecell_solver_user_set_soft_thread_step(
    void * api_instance,
    int num_times_step
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    user->soft_thread->num_times_step = num_times_step;
}

int DLLEXPORT freecell_solver_user_next_hard_thread(
    void * api_instance
    )
{
    fcs_user_t * user;
    fc_solve_soft_thread_t * soft_thread;

    user = (fcs_user_t *)api_instance;

    soft_thread = fc_solve_new_hard_thread(user->fc_solve_obj);

    if (soft_thread == NULL)
    {
        return 1;
    }

    user->soft_thread = soft_thread;

    return 0;
}

int DLLEXPORT freecell_solver_user_get_num_soft_threads_in_instance(
    void * api_instance
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    return user->fc_solve_obj->next_soft_thread_id;
}

void DLLEXPORT freecell_solver_user_set_calc_real_depth(
    void * api_instance,
    int calc_real_depth
)
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    STRUCT_SET_FLAG_TO(user->fc_solve_obj, FCS_RUNTIME_CALC_REAL_DEPTH, calc_real_depth);
}

void DLLEXPORT freecell_solver_user_set_soft_thread_name(
    void * api_instance,
    freecell_solver_str_t name
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    if (user->soft_thread->name != NULL)
    {
        free(user->soft_thread->name);
    }
    user->soft_thread->name = strdup(name);
}

int DLLEXPORT freecell_solver_user_set_hard_thread_prelude(
    void * api_instance,
    const char * prelude
    )
{
    fcs_user_t * user;
    fc_solve_hard_thread_t * hard_thread;

    user = (fcs_user_t *)api_instance;

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
    void * api_instance
    )
{
    fcs_user_t * user;
    int i;

    user = (fcs_user_t *)api_instance;

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
    void * api_instance,
    const char * tests_order,
    char * * error_string
    )
{
    fcs_user_t * user;
    int ret;

    user = (fcs_user_t*)api_instance;

    if (user->fc_solve_obj->opt_tests_order.tests)
    {
        free(user->fc_solve_obj->opt_tests_order.tests);
        user->fc_solve_obj->opt_tests_order.tests = NULL;
    }

    STRUCT_CLEAR_FLAG(user->fc_solve_obj, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET );

    ret =
        fc_solve_apply_tests_order(
            &(user->fc_solve_obj->opt_tests_order),
            tests_order,
            error_string
            );

    if (!ret)
    {
        STRUCT_TURN_ON_FLAG(user->fc_solve_obj, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET);
    }

    return ret;
}

void DLLEXPORT freecell_solver_user_set_reparent_states(
    void * api_instance,
    int to_reparent_states
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    STRUCT_SET_FLAG_TO(user->fc_solve_obj,
            FCS_RUNTIME_TO_REPARENT_STATES_PROTO, to_reparent_states);
}

void DLLEXPORT freecell_solver_user_set_scans_synergy(
    void * api_instance,
    int synergy
    )
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    STRUCT_SET_FLAG_TO(user->fc_solve_obj, FCS_RUNTIME_SCANS_SYNERGY, synergy);
}

int DLLEXPORT freecell_solver_user_next_instance(
    void * from_user_instance
    )
{
    fcs_user_t * user;
    fcs_instance_item_t * instance_item;
    fcs_flare_item_t * flare;    

    user = (fcs_user_t *)from_user_instance;

    user->instances_list =
        realloc(
            user->instances_list,
            sizeof(user->instances_list[0])
            * (++user->num_instances)
            );

    user->current_instance_idx = user->num_instances-1;
    user->fc_solve_obj = fc_solve_alloc_instance();

#ifndef FCS_FREECELL_ONLY
    fc_solve_apply_preset_by_ptr(user->fc_solve_obj, &(user->common_preset));
#endif

    instance_item = &(user->instances_list[user->current_instance_idx]);
    instance_item->num_flares = 1;

    /*
     * Switch the soft_thread variable so it won't refer to the old
     * instance
     * */
    user->soft_thread =
        fc_solve_instance_get_first_soft_thread(user->fc_solve_obj);

    flare = &(instance_item->flares[instance_item->num_flares-1]);

    flare->obj = user->fc_solve_obj;
    flare->ret_code =
        instance_item->ret_code =
        user->ret_code =
        FCS_STATE_NOT_BEGAN_YET;
    flare->limit = instance_item->limit = -1;
    
    user->fc_solve_obj->debug_iter_output_func =
        (user->iter_handler ? iter_handler_wrapper : NULL);
    user->fc_solve_obj->debug_iter_output_context = user;

    return 0;
}

int DLLEXPORT freecell_solver_user_reset(void * api_instance)
{
    fcs_user_t * user;

    user = (fcs_user_t *)api_instance;

    user_free_resources(user);

    user_initialize(user);

    return 0;
}

DLLEXPORT const char * freecell_solver_user_get_lib_version(
    void * api_instance GCC_UNUSED
    )
{
    return VERSION;
}

DLLEXPORT const char * freecell_solver_user_get_current_soft_thread_name(
    void * api_instance
    )
{
    fcs_user_t * user;
    fc_solve_hard_thread_t * hard_thread;
    fc_solve_instance_t * instance;

    user = (fcs_user_t *)api_instance;

    instance = 
        user->fc_solve_obj;

    hard_thread = &(instance->hard_threads[instance->ht_idx]);

    return hard_thread->soft_threads[hard_thread->st_idx].name;
}
