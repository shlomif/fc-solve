/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * lib.c - library interface functions of Freecell Solver.
 */
#include "dll_thunk.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "config.h"

#include "state.h"
#include "instance_for_lib.h"
#include "preset.h"
#include "fcs_user.h"
#include "move_funcs_order.h"
#include "fcs_user_internal.h"
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
#include "fc_pro_iface_pos.h"
#endif

#define FCS_MAX_FLARE_NAME_LEN 30

typedef struct
{
    fcs_int_limit_t num_checked_states;
#ifndef FCS_DISABLE_NUM_STORED_STATES
    fcs_int_limit_t num_states_in_collection;
#endif
} fcs_stats_t;

static const fcs_stats_t initial_stats = {.num_checked_states = 0,
#ifndef FCS_DISABLE_NUM_STORED_STATES
    .num_states_in_collection = 0
#endif
};

/* A flare is an alternative scan algorithm to be tried. All flares in
 * a single instance are being evaluated and then one picks the shortest
 * solution out of all of them. (see fc-solve/docs/flares-functional-spec.txt )
 * */

typedef struct
{
    fc_solve_instance_t obj;
    int ret_code;
    /* Whether the instance is ready to be input with (i.e:
     * was recycled already.) */
    fcs_bool_t instance_is_ready;
    int limit;
    char name[FCS_MAX_FLARE_NAME_LEN];
#ifdef FCS_WITH_MOVES
    uint_fast32_t next_move_idx;
    fcs_moves_sequence_t moves_seq;
#endif
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    fcs_moves_processed_t fc_pro_moves;
#endif
    fcs_stats_t obj_stats;
    fcs_bool_t was_solution_traced;
    fcs_state_locs_struct_t trace_solution_state_locs;
} fcs_flare_item_t;

#ifdef FCS_WITH_FLARES
typedef enum {
    FLARES_PLAN_RUN_INDEFINITELY,
    FLARES_PLAN_RUN_COUNT_ITERS,
    FLARES_PLAN_CHECKPOINT,
} flares_plan_type_t;

#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
typedef enum {
    FLARES_CHOICE_FC_SOLVE_SOLUTION_LEN,
    FLARES_CHOICE_FCPRO_SOLUTION_LEN
} flares_choice_type_t;
#endif

typedef fcs_int_limit_t flare_iters_quota_t;

static inline flare_iters_quota_t normalize_iters_quota(
    const flare_iters_quota_t i)
{
    return max(i, 0);
}

typedef struct
{
    fcs_flare_item_t *flare;
    flares_plan_type_t type;
    flare_iters_quota_t remaining_quota, initial_quota;
    int_fast32_t count_iters;
} flares_plan_item;

#endif

typedef struct
{
#ifdef FCS_WITH_FLARES
    fcs_flare_item_t *flares, *end_of_flares, *minimal_flare;
    flares_plan_item *plan;
    size_t num_plan_items, current_plan_item_idx;
    char *flares_plan_string;
    /*
     * The default flares_plan_compiled is "False", which means that the
     * flares_plan_string was set and needs to be processed. Once
     * the compile function is called, it is set to "True" and it is set
     * to "False" if the flares_plan_string is set to a different value.
     *
     * Upon starting to run, one checks if flares_plan_compiled is false
     * and if so, compiles the flares plan, and sets the flares_plan_compiled
     * string to true.
     */
    fcs_bool_t flares_plan_compiled;
    fcs_bool_t all_plan_items_finished_so_far;
#else
    fcs_flare_item_t single_flare;
    fcs_bool_t was_flare_found, was_flare_finished;
#endif
    int limit;
} fcs_instance_item_t;

typedef struct
{
    /*
     * This is a list of several consecutive instances that are run
     * one after the other in case the previous ones could not solve
     * the board
     * */
    fcs_instance_item_t *current_instance, *instances_list,
        *end_of_instances_list;
    /*
     * The global (sequence-wide) limit of the iterations. Used
     * by limit_iterations() and friends
     * */
    fcs_int_limit_t current_iterations_limit;
    /*
     * The number of iterations this board started at.
     * */
    fcs_stats_t iterations_board_started_at;
    /*
     * The number of iterations that the current instance started solving from.
     * */
    fcs_stats_t init_num_checked_states;
    /*
     * A pointer to the currently active instance out of the sequence
     * */
    fcs_flare_item_t *active_flare;
    fcs_state_keyval_pair_t state;
    fcs_state_keyval_pair_t running_state;
#if defined(FCS_WITH_FLARES) && !defined(FCS_DISABLE_PATSOLVE)
    fcs_state_keyval_pair_t initial_non_canonized_state;
#endif
    fcs_state_locs_struct_t state_locs;
    fcs_state_locs_struct_t initial_state_locs;
    int ret_code;
    fcs_bool_t all_instances_were_suspended;
    fcs_state_validity_ret_t state_validity_ret;
    fcs_card_t state_validity_card;
#ifndef FCS_WITHOUT_ITER_HANDLER
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    freecell_solver_user_iter_handler_t iter_handler;
#endif
    freecell_solver_user_long_iter_handler_t long_iter_handler;
    void *iter_handler_context;
#endif
#ifdef FCS_WITH_FLARES
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    flares_choice_type_t flares_choice;
#endif
    double flares_iters_factor;
#endif

    fc_solve_soft_thread_t *soft_thread;

    DECLARE_IND_BUF_T(indirect_stacks_buffer)
#define MAX_STATE_STRING_COPY_LEN 2048
    char state_string_copy[MAX_STATE_STRING_COPY_LEN];

#ifndef FCS_FREECELL_ONLY
    fcs_preset_t common_preset;
#endif

#ifdef FCS_WITH_ERROR_STRS
#define MAX_ERROR_STRING_LEN 160
    char error_string[MAX_ERROR_STRING_LEN];
#endif

    fcs_meta_compact_allocator_t meta_alloc;
} fcs_user_t;

static inline fc_solve_instance_t *user_obj(fcs_user_t *const user)
{
    return &user->active_flare->obj;
}

static inline fc_solve_instance_t *active_obj(void *const api_instance)
{
    return user_obj((fcs_user_t *)api_instance);
}

#ifndef FCS_WITHOUT_ITER_HANDLER
static void iter_handler_wrapper(void *api_instance, fcs_int_limit_t iter_num,
    int depth, void *lp_instance GCC_UNUSED, fcs_kv_state_t *ptr_state,
    fcs_int_limit_t parent_iter_num);
#endif

#define INSTANCES_LOOP_START()                                                 \
    const_SLOT(end_of_instances_list, user);                                   \
    for (fcs_instance_item_t *instance_item = user->instances_list;            \
         instance_item < end_of_instances_list; instance_item++)               \
    {

#ifdef FCS_WITH_FLARES

#define INSTANCE_ITEM_FLARES_LOOP_START()                                      \
    const fcs_flare_item_t *const end_of_flares =                              \
        instance_item->end_of_flares;                                          \
    for (fcs_flare_item_t *flare = instance_item->flares;                      \
         flare < end_of_flares; flare++)                                       \
    {

#else

#define INSTANCE_ITEM_FLARES_LOOP_START()                                      \
    fcs_flare_item_t *const flare = &(instance_item->single_flare);            \
    {

#endif

#define INSTANCE_ITEM_FLARES_LOOP_END() }

#define INSTANCES_LOOP_END() }

#define FLARES_LOOP_START()                                                    \
    INSTANCES_LOOP_START()                                                     \
    INSTANCE_ITEM_FLARES_LOOP_START()
#define FLARES_LOOP_END()                                                      \
    INSTANCE_ITEM_FLARES_LOOP_END()                                            \
    INSTANCES_LOOP_END()

static int user_next_instance(fcs_user_t *user);

#ifdef FCS_WITH_ERROR_STRS
#define ALLOC_ERROR_STRING(var, s) *(var) = strdup(s)
#define SET_ERROR_VAR(var, s) *(var) = (((s)[0]) ? strdup(s) : NULL)
#define SET_ERROR(s) strcpy(user->error_string, s)
static inline void clear_error(fcs_user_t *const user)
{
    user->error_string[0] = '\0';
}
#else
#define ALLOC_ERROR_STRING(var, s)
#define SET_ERROR_VAR(var, s)
#define SET_ERROR(s)
#define clear_error(user)
#endif

static void user_initialize(fcs_user_t *const user)
{
#ifndef FCS_FREECELL_ONLY
    const fcs_preset_t *freecell_preset;

    fc_solve_get_preset_by_name("freecell", &freecell_preset);

    fcs_duplicate_preset(user->common_preset, *freecell_preset);
#endif

    fc_solve_meta_compact_allocator_init(&(user->meta_alloc));

    user->instances_list = NULL;
    user->end_of_instances_list = NULL;
#ifndef FCS_WITHOUT_ITER_HANDLER
    user->long_iter_handler = NULL;
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    user->iter_handler = NULL;
#endif
#endif
    user->current_iterations_limit = -1;

    user->iterations_board_started_at = initial_stats;
    user->all_instances_were_suspended = TRUE;
#ifdef FCS_WITH_FLARES
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    user->flares_choice = FLARES_CHOICE_FC_SOLVE_SOLUTION_LEN;
#endif
    user->flares_iters_factor = 1.0;
#endif

    clear_error(user);

    user_next_instance(user);

    return;
}

void DLLEXPORT *freecell_solver_user_alloc(void)
{
    fcs_user_t *const ret = (fcs_user_t *)SMALLOC1(ret);

    user_initialize(ret);

    return (void *)ret;
}

#ifndef FCS_FREECELL_ONLY
int DLLEXPORT freecell_solver_user_apply_preset(
    void *const api_instance, const char *const preset_name)
{
    const fcs_preset_t *new_preset_ptr;
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    const_AUTO(
        status1, fc_solve_get_preset_by_name(preset_name, &new_preset_ptr));
    if (status1 != FCS_PRESET_CODE_OK)
    {
        return status1;
    }

    FLARES_LOOP_START()
    const_AUTO(
        status2, fc_solve_apply_preset_by_ptr(&(flare->obj), new_preset_ptr));

    if (status2 != FCS_PRESET_CODE_OK)
    {
        return status2;
    }
    FLARES_LOOP_END()

    fcs_duplicate_preset(user->common_preset, *new_preset_ptr);

    return FCS_PRESET_CODE_OK;
}
#endif

void DLLEXPORT freecell_solver_user_limit_iterations_long(
    void *const api_instance, const fcs_int_limit_t max_iters)
{
    ((fcs_user_t * const)api_instance)->current_iterations_limit = max_iters;
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
void DLLEXPORT freecell_solver_user_limit_iterations(
    void *const api_instance, const int max_iters)
{
    return freecell_solver_user_limit_iterations_long(
        api_instance, (fcs_int_limit_t)max_iters);
}

void DLLEXPORT freecell_solver_user_limit_current_instance_iterations(
    void *const api_instance, const int max_iters)
{
    ((fcs_user_t *)api_instance)->current_instance->limit = max_iters;
}
#endif

static inline fc_solve_soft_thread_t *api_soft_thread(void *const api_instance)
{
    return ((fcs_user_t * const)api_instance)->soft_thread;
}

int DLLEXPORT freecell_solver_user_set_depth_tests_order(
    void *const api_instance, const int min_depth,
    const char *const tests_order FCS__PASS_ERR_STR(char **const error_string))
{
    int depth_idx;

    fc_solve_soft_thread_t *const soft_thread = api_soft_thread(api_instance);

    if (min_depth < 0)
    {
        ALLOC_ERROR_STRING(error_string, "Depth is negative.");
        return 1;
    }

    if (min_depth == 0)
    {
        depth_idx = 0;
    }
    else
    {
        for (depth_idx = 0;; depth_idx++)
        {
            if (depth_idx == soft_thread->by_depth_tests_order.num - 1)
            {
                break;
            }
            else if (min_depth <
                     soft_thread->by_depth_tests_order.by_depth_tests[depth_idx]
                         .max_depth)
            {
                break;
            }
        }

        depth_idx++;
    }

    if (depth_idx == soft_thread->by_depth_tests_order.num)
    {
        soft_thread->by_depth_tests_order.by_depth_tests =
            SREALLOC(soft_thread->by_depth_tests_order.by_depth_tests,
                ++soft_thread->by_depth_tests_order.num);

        soft_thread->by_depth_tests_order.by_depth_tests[depth_idx]
            .tests_order.num_groups = 0;
        soft_thread->by_depth_tests_order.by_depth_tests[depth_idx]
            .tests_order.groups = NULL;
    }

    if (depth_idx > 0)
    {
        soft_thread->by_depth_tests_order.by_depth_tests[depth_idx - 1]
            .max_depth = min_depth;
    }

    soft_thread->by_depth_tests_order.by_depth_tests[depth_idx].max_depth =
        SSIZE_MAX;

#ifdef FCS_WITH_ERROR_STRS
    char static_error_string[120];
#endif
    const int ret_code = fc_solve_apply_tests_order(
        &(soft_thread->by_depth_tests_order.by_depth_tests[depth_idx]
                .tests_order),
        tests_order FCS__PASS_ERR_STR(static_error_string));

    SET_ERROR_VAR(error_string, static_error_string);

    for (int further_depth_idx = depth_idx + 1;
         further_depth_idx < soft_thread->by_depth_tests_order.num;
         further_depth_idx++)
    {
        fc_solve_free_tests_order(&(
            soft_thread->by_depth_tests_order.by_depth_tests[further_depth_idx]
                .tests_order));
    }

    soft_thread->by_depth_tests_order.by_depth_tests =
        SREALLOC(soft_thread->by_depth_tests_order.by_depth_tests,
            soft_thread->by_depth_tests_order.num = depth_idx + 1);

    return ret_code;
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
int DLLEXPORT freecell_solver_user_set_tests_order(void *api_instance,
    const char *tests_order FCS__PASS_ERR_STR(char **error_string))
{
    return freecell_solver_user_set_depth_tests_order(
        api_instance, 0, tests_order FCS__PASS_ERR_STR(error_string));
}
#endif

typedef enum {
    FCS_COMPILE_FLARES_RET_OK = 0,
    FCS_COMPILE_FLARES_RET_COLON_NOT_FOUND,
    FCS_COMPILE_FLARES_RET_RUN_AT_SIGN_NOT_FOUND,
    FCS_COMPILE_FLARES_RET_UNKNOWN_FLARE_NAME,
    FCS_COMPILE_FLARES_RET_JUNK_AFTER_CP,
    FCS_COMPILE_FLARES_RET_UNKNOWN_COMMAND,
    FCS_COMPILE_FLARES_RUN_JUNK_AFTER_LAST_RUN_INDEF
} fcs_compile_flares_ret_t;

#ifdef FCS_WITH_FLARES
static inline flares_plan_item create_plan_item(const flares_plan_type_t mytype,
    fcs_flare_item_t *const flare, const int_fast32_t count_iters)
{
    return (const flares_plan_item){
        .type = mytype, .flare = flare, .count_iters = count_iters};
}

static inline void add_to_plan(fcs_instance_item_t *const instance_item,
    const flares_plan_type_t mytype, fcs_flare_item_t *const flare,
    const int_fast32_t count_iters)
{
    const int next_item = instance_item->num_plan_items;

    instance_item->plan =
        SREALLOC(instance_item->plan, ++(instance_item->num_plan_items));

    instance_item->plan[next_item] =
        create_plan_item(mytype, flare, count_iters);
}

static inline void add_count_iters_to_plan(
    fcs_instance_item_t *const instance_item, fcs_flare_item_t *const flare,
    const int_fast32_t count_iters)
{
    add_to_plan(instance_item, FLARES_PLAN_RUN_COUNT_ITERS, flare, count_iters);
}

static inline void add_checkpoint_to_plan(
    fcs_instance_item_t *const instance_item)
{
    add_to_plan(instance_item, FLARES_PLAN_CHECKPOINT, NULL, -1);
}

static inline void add_run_indef_to_plan(
    fcs_instance_item_t *const instance_item, fcs_flare_item_t *const flare)
{
    add_to_plan(instance_item, FLARES_PLAN_RUN_INDEFINITELY, flare, -1);
}

static inline fcs_flare_item_t *find_flare(fcs_flare_item_t *const flares,
    const fcs_flare_item_t *const end_of_flares, const char *const proto_name,
    const size_t name_len)
{
    char name[name_len + 1];
    strncpy(name, proto_name, name_len);
    name[name_len] = '\0';

    for (fcs_flare_item_t *flare = flares; flare < end_of_flares; flare++)
    {
        if (!strcmp(flare->name, name))
        {
            return flare;
        }
    }
    return NULL;
}

static inline fcs_compile_flares_ret_t user_compile_all_flares_plans(
    fcs_user_t *const user)
{
    {
        const_SLOT(end_of_instances_list, user);
        for (fcs_instance_item_t *instance_item = user->instances_list;
             instance_item < end_of_instances_list; instance_item++)
        {
            if (instance_item->flares_plan_compiled)
            {
                continue;
            }
            fcs_flare_item_t *const flares = instance_item->flares;
            const_SLOT(end_of_flares, instance_item);

            /* If the plan string is NULL or empty, then set the plan
             * to run only the first flare indefinitely. (And then have
             * an implicit checkpoint for good measure.) */
            if ((!instance_item->flares_plan_string) ||
                (!instance_item->flares_plan_string[0]))
            {
                if (instance_item->plan)
                {
                    free(instance_item->plan);
                }
                instance_item->num_plan_items = 2;
                instance_item->plan =
                    SMALLOC(instance_item->plan, instance_item->num_plan_items);
                /* Set to the first flare. */
                instance_item->plan[0] = create_plan_item(
                    FLARES_PLAN_RUN_INDEFINITELY, instance_item->flares, -1);
                instance_item->plan[1] =
                    create_plan_item(FLARES_PLAN_CHECKPOINT, NULL, -1);

                instance_item->flares_plan_compiled = TRUE;
                continue;
            }

            /* Tough luck - gotta parse the string. ;-) */
            const char *cmd_end, *item_end;
            const char *item_start = instance_item->flares_plan_string;
            if (instance_item->plan)
            {
                free(instance_item->plan);
                instance_item->plan = NULL;
                instance_item->num_plan_items = 0;
            }
            do
            {
                cmd_end = strchr(item_start, ':');

                if (!cmd_end)
                {
                    SET_ERROR("Could not find a \":\" for a command.");
                    return FCS_COMPILE_FLARES_RET_COLON_NOT_FOUND;
                }

                if (string_starts_with(item_start, "Run", cmd_end))
                {

                    /* It's a Run item - handle it. */
                    cmd_end++;
                    const int_fast32_t count_iters = atoi(cmd_end);

                    const char *at_sign = cmd_end;

                    while ((*at_sign) && isdigit(*at_sign))
                    {
                        at_sign++;
                    }

                    if (*at_sign != '@')
                    {
                        SET_ERROR("Could not find a \"@\" directly after "
                                  "the digits after the 'Run:' command.");
                        return FCS_COMPILE_FLARES_RET_RUN_AT_SIGN_NOT_FOUND;
                    }
                    const char *const after_at_sign = at_sign + 1;

                    /*
                     * Position item_end at the end of item (designated by
                     * ",")
                     * or alternatively the end of the string.
                     * */
                    item_end = strchr(after_at_sign, ',');
                    if (!item_end)
                    {
                        item_end = strchr(after_at_sign, '\0');
                    }

                    fcs_flare_item_t *const flare = find_flare(flares,
                        end_of_flares, after_at_sign, item_end - after_at_sign);

                    if (!flare)
                    {
                        SET_ERROR("Unknown flare name.");
                        return FCS_COMPILE_FLARES_RET_UNKNOWN_FLARE_NAME;
                    }

                    add_count_iters_to_plan(instance_item, flare, count_iters);
                }
                else if (string_starts_with(item_start, "CP", cmd_end))
                {
                    item_end = cmd_end + 1;
                    if (!(((*item_end) == ',') || (!(*item_end))))
                    {
                        SET_ERROR("Junk after CP (Checkpoint) command.");
                        return FCS_COMPILE_FLARES_RET_JUNK_AFTER_CP;
                    }

                    add_checkpoint_to_plan(instance_item);
                }
                else if (string_starts_with(item_start, "RunIndef", cmd_end))
                {

                    cmd_end++;
                    item_end = strchr(cmd_end, ',');
                    if (item_end)
                    {
                        SET_ERROR("Junk after last RunIndef command. Must "
                                  "be the final command.");
                        return FCS_COMPILE_FLARES_RUN_JUNK_AFTER_LAST_RUN_INDEF;
                    }
                    item_end = cmd_end + strlen(cmd_end);

                    fcs_flare_item_t *const flare = find_flare(
                        flares, end_of_flares, cmd_end, item_end - cmd_end);
                    if (!flare)
                    {
                        SET_ERROR("Unknown flare name in RunIndef command.");
                        return FCS_COMPILE_FLARES_RET_UNKNOWN_FLARE_NAME;
                    }

                    add_run_indef_to_plan(instance_item, flare);
                }
                else
                {
                    SET_ERROR("Unknown command.");
                    return FCS_COMPILE_FLARES_RET_UNKNOWN_COMMAND;
                }
                item_start = item_end + 1;
            } while (*item_end);

            if ((!instance_item->plan) ||
                instance_item->plan[instance_item->num_plan_items - 1].type !=
                    FLARES_PLAN_CHECKPOINT)
            {
                add_checkpoint_to_plan(instance_item);
            }

            instance_item->flares_plan_compiled = TRUE;
            continue;
        }
    }

    const_SLOT(flares_iters_factor, user);
    INSTANCES_LOOP_START()
    const_SLOT(num_plan_items, instance_item);
    const_SLOT(plan, instance_item);
    for (int i = 0; i < num_plan_items; i++)
    {
        flares_plan_item *const item = plan + i;
        switch (item->type)
        {
        case FLARES_PLAN_RUN_COUNT_ITERS:
            item->initial_quota = normalize_iters_quota((typeof(
                item->initial_quota))(flares_iters_factor * item->count_iters));
            break;

        case FLARES_PLAN_CHECKPOINT:
        case FLARES_PLAN_RUN_INDEFINITELY:
            item->initial_quota = -1;
            break;
        }
    }
    INSTANCES_LOOP_END()
    clear_error(user);

    return FCS_COMPILE_FLARES_RET_OK;
}
#endif

/*
 * Add a trailing newline to the string if it does not exist.
 */
#define MY_MARGIN 3
#define TRAILING_CHAR '\n'
static inline fcs_bool_t duplicate_string_while_adding_a_trailing_newline(
    char *const s, const char *const orig_str)
{
    const int len = strlen(orig_str);
    /*
     * If orig_str is the empty string then there is no
     * penultimate character.
     * */
    if (len)
    {
        if (len >= MAX_STATE_STRING_COPY_LEN - MY_MARGIN)
        {
            return FALSE;
        }
        strcpy(s, orig_str);
        char *s_end = s + len - 1;
        if ((*s_end) != TRAILING_CHAR)
        {
            *(++s_end) = TRAILING_CHAR;
            *(++s_end) = '\0';
        }
    }
    else
    {
        strcpy(s, "\n");
    }
    return TRUE;
}
#undef TRAILING_CHAR
#undef MY_MARGIN

static inline void recycle_flare(fcs_flare_item_t *const flare)
{
    if (!flare->instance_is_ready)
    {
        fc_solve_recycle_instance(&(flare->obj));
        flare->instance_is_ready = TRUE;
    }
}

static void recycle_instance(
    fcs_user_t *const user, fcs_instance_item_t *const instance_item)
{
    INSTANCE_ITEM_FLARES_LOOP_START()
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    fc_solve_moves_processed_free(&(flare->fc_pro_moves));
#endif

    if (flare->ret_code != FCS_STATE_NOT_BEGAN_YET)
    {
        recycle_flare(flare);
        /*
         * We have to initialize init_num_checked_states to 0 here, because it
         * may
         * not get initialized again, and now the num_checked_states of the
         * instance
         * is equal to 0.
         * */
        user->init_num_checked_states = initial_stats;

        flare->ret_code = FCS_STATE_NOT_BEGAN_YET;
    }

#ifdef FCS_WITH_MOVES
    if (flare->moves_seq.moves)
    {
        free(flare->moves_seq.moves);
        flare->moves_seq.moves = NULL;
        flare->moves_seq.num_moves = 0;
        flare->next_move_idx = 0;
    }
#endif

    flare->obj_stats = initial_stats;
    INSTANCE_ITEM_FLARES_LOOP_END()

#ifdef FCS_WITH_FLARES
    instance_item->current_plan_item_idx = 0;
    instance_item->minimal_flare = NULL;
#else
    instance_item->was_flare_found = instance_item->was_flare_finished = FALSE;
#endif

    return;
}

#ifndef FCS_USE_COMPACT_MOVE_TOKENS
#define internal_move_to_user_move(x) (x)
#else
static inline const fcs_move_t internal_move_to_user_move(
    const fcs_internal_move_t internal_move)
{
    fcs_move_t user_move;

    /* Convert the internal_move to a user move. */
    fcs_move_set_src_stack(
        user_move, fcs_int_move_get_src_stack(internal_move));
    fcs_move_set_dest_stack(
        user_move, fcs_int_move_get_dest_stack(internal_move));
    fcs_move_set_type(user_move, fcs_int_move_get_type(internal_move));
    fcs_move_set_num_cards_in_seq(
        user_move, fcs_int_move_get_num_cards_in_seq(internal_move));

    return user_move;
}
#endif

#ifndef FCS_USE_COMPACT_MOVE_TOKENS
#define user_move_to_internal_move(x) (x)
#else
static inline const fcs_internal_move_t user_move_to_internal_move(
    const fcs_move_t user_move)
{
    fcs_internal_move_t internal_move;

    /* Convert the internal_move to a user move. */
    fcs_int_move_set_src_stack(
        internal_move, fcs_move_get_src_stack(user_move));
    fcs_int_move_set_dest_stack(
        internal_move, fcs_move_get_dest_stack(user_move));
    fcs_int_move_set_type(internal_move, fcs_move_get_type(user_move));
    fcs_int_move_set_num_cards_in_seq(
        internal_move, fcs_move_get_num_cards_in_seq(user_move));

    return internal_move;
}
#endif

static inline void calc_moves_seq(const fcs_move_stack_t *const solution_moves,
    fcs_moves_sequence_t *const moves_seq)
{
    moves_seq->num_moves = 0;
    moves_seq->moves = NULL;

    const_SLOT(num_moves, solution_moves);
    fcs_internal_move_t *next_move_ptr = solution_moves->moves + num_moves;

    fcs_move_t *const ret_moves = SMALLOC(ret_moves, num_moves);

    if (!ret_moves)
    {
        return;
    }

    for (size_t i = 0; i < num_moves; i++)
    {
        ret_moves[i] = internal_move_to_user_move(*(--next_move_ptr));
    }

    moves_seq->num_moves = num_moves;
    moves_seq->moves = ret_moves;
}

#if defined(FCS_WITH_MOVES) || defined(FCS_WITH_FLARES)
static void trace_flare_solution(
    fcs_user_t *const user, fcs_flare_item_t *const flare)
{
    if (flare->was_solution_traced)
    {
        return;
    }

    fc_solve_instance_t *const instance = &(flare->obj);
#ifdef FCS_WITH_MOVES

    fc_solve_trace_solution(instance);
    flare->trace_solution_state_locs = user->state_locs;
    fc_solve_move_stack_normalize(&(instance->solution_moves), &(user->state),
        &(flare->trace_solution_state_locs)PASS_FREECELLS(
            INSTANCE_FREECELLS_NUM) PASS_STACKS(INSTANCE_STACKS_NUM));

    calc_moves_seq(&(instance->solution_moves), &(flare->moves_seq));
    instance_free_solution_moves(instance);
    flare->next_move_idx = 0;
#endif
    flare->obj_stats.num_checked_states = instance->i__num_checked_states;
#ifndef FCS_DISABLE_NUM_STORED_STATES
    flare->obj_stats.num_states_in_collection =
        instance->num_states_in_collection;
#endif

    recycle_flare(flare);
    flare->was_solution_traced = TRUE;
}
#endif

#ifdef FCS_WITH_FLARES
static int get_flare_move_count(
    fcs_user_t *const user, fcs_flare_item_t *const flare)
{
    trace_flare_solution(user, flare);
#define RET() return flare->moves_seq.num_moves
#ifdef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    RET();
#else
    if (user->flares_choice == FLARES_CHOICE_FC_SOLVE_SOLUTION_LEN)
    {
        RET();
    }
    else
    {
        if (!flare->fc_pro_moves.moves)
        {
            fc_solve_moves_processed_gen(&(flare->fc_pro_moves),
                &(user->initial_non_canonized_state),
#ifdef FCS_FREECELL_ONLY
                4,
#else
                user->common_preset.game_params.freecells_num,
#endif
                &(flare->moves_seq));
        }

        return fc_solve_moves_processed_get_moves_left(&(flare->fc_pro_moves));
    }
#endif

#undef RET
}
#endif

static inline fcs_instance_item_t *get_current_instance_item(
    fcs_user_t const *user)
{
    return (user->current_instance);
}

static inline int resume_solution(fcs_user_t *const user)
{
    fcs_stats_t init_num_checked_states;

    int ret = FCS_STATE_IS_NOT_SOLVEABLE;

    const_SLOT(end_of_instances_list, user);
    /*
     * I expect user->current_instance to be initialized with some value.
     * */
    do
    {
        fcs_instance_item_t *const instance_item =
            get_current_instance_item(user);

#ifdef FCS_WITH_FLARES
        if (instance_item->current_plan_item_idx ==
            instance_item->num_plan_items)
        {
            /*
             * If all the plan items finished so far, it means this instance
             * cannot be reused, because it will always yield a cannot
             * be found result. So instead of looping infinitely,
             * move to the next instance, or exit. */
            if (instance_item->all_plan_items_finished_so_far)
            {
                recycle_instance(user, instance_item);
                user->current_instance++;
                continue;
            }
            /* Otherwise - restart the plan again. */
            else
            {
                instance_item->all_plan_items_finished_so_far = TRUE;
                instance_item->current_plan_item_idx = 0;
            }
        }

        flares_plan_item *const current_plan_item =
            &(instance_item->plan[instance_item->current_plan_item_idx++]);
        if (current_plan_item->type == FLARES_PLAN_CHECKPOINT)
        {
            if (instance_item->minimal_flare)
            {
                user->active_flare = instance_item->minimal_flare;
                user->init_num_checked_states = user->active_flare->obj_stats;

                ret = user->ret_code = FCS_STATE_WAS_SOLVED;

                break;
            }
            else
            {
                continue;
            }
        }

        const flare_iters_quota_t flare_iters_quota =
            current_plan_item->remaining_quota;

        fcs_flare_item_t *const flare = current_plan_item->flare;
#else
        fcs_flare_item_t *const flare = &(instance_item->single_flare);

        if (instance_item->was_flare_finished)
        {
            if (instance_item->was_flare_found)
            {
                user->init_num_checked_states = flare->obj_stats;

                ret = user->ret_code = FCS_STATE_WAS_SOLVED;

                break;
            }
            else
            {
                recycle_instance(user, instance_item);
                user->current_instance++;
                continue;
            }
        }
#endif
        fc_solve_instance_t *const instance = &(flare->obj);

        user->active_flare = flare;
        const fcs_bool_t is_start_of_flare_solving =
            (flare->ret_code == FCS_STATE_NOT_BEGAN_YET);

        if (is_start_of_flare_solving)
        {
            if (!fc_solve_initial_user_state_to_c(user->state_string_copy,
                    &(user->state), INSTANCE_FREECELLS_NUM, INSTANCE_STACKS_NUM,
                    INSTANCE_DECKS_NUM, user->indirect_stacks_buffer))
            {
                user->state_validity_ret =
                    FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT;
                return (user->ret_code = FCS_STATE_INVALID_STATE);
            }

#ifndef FCS_DISABLE_STATE_VALIDITY_CHECK
            if (FCS_STATE_VALIDITY__OK !=
                (user->state_validity_ret = fc_solve_check_state_validity(
                     &(user->state)PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
                         PASS_STACKS(INSTANCE_STACKS_NUM)
                             PASS_DECKS(INSTANCE_DECKS_NUM),
                     &(user->state_validity_card))))
            {
                return (user->ret_code = FCS_STATE_INVALID_STATE);
            }
#endif

            fc_solve_init_locs(&(user->initial_state_locs));
            user->state_locs = user->initial_state_locs;

            /* running_state and initial_non_canonized_state are
             * normalized states. So We're duplicating
             * state to it before user->state is canonized.
             * */
            FCS_STATE__DUP_keyval_pair(user->running_state, user->state);
#if defined(FCS_WITH_FLARES) && !defined(FCS_DISABLE_PATSOLVE)
            FCS_STATE__DUP_keyval_pair(
                user->initial_non_canonized_state, user->state);
#endif

            fc_solve_canonize_state_with_locs(&(user->state.s),
                &(user->state_locs)PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
                    PASS_STACKS(INSTANCE_STACKS_NUM));
            fc_solve_init_instance(instance);
        }

#define PARAMETERIZED_FIXED_LIMIT(increment)                                   \
    (user->iterations_board_started_at.num_checked_states + increment)
#define PARAMETERIZED_LIMIT(increment)                                         \
    (((increment) < 0) ? (-1) : PARAMETERIZED_FIXED_LIMIT(increment))
#define local_limit() (instance_item->limit)
#ifdef FCS_WITH_FLARES
#define NUM_ITERS_LIMITS 3
#else
#define NUM_ITERS_LIMITS 2
#endif
#define NUM_ITERS_LIMITS_MINUS_1 (NUM_ITERS_LIMITS - 1)
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

        {
            const fcs_int_limit_t limits[NUM_ITERS_LIMITS_MINUS_1] = {
                user->current_iterations_limit
#ifdef FCS_WITH_FLARES
                ,
                PARAMETERIZED_LIMIT(flare_iters_quota)
#endif
            };

            fcs_int_limit_t mymin = local_limit();
            for (size_t limit_idx = 0; limit_idx < NUM_ITERS_LIMITS_MINUS_1;
                 limit_idx++)
            {
                const_AUTO(new_lim, limits[limit_idx]);
                if (new_lim >= 0)
                {
                    mymin = (mymin < 0) ? new_lim : min(mymin, new_lim);
                }
            }

            instance->effective_max_num_checked_states =
                ((mymin < 0) ? FCS_INT_LIMIT_MAX
                             : (instance->i__num_checked_states + mymin -
                                   user->iterations_board_started_at
                                       .num_checked_states));
        }

        user->init_num_checked_states.num_checked_states =
            init_num_checked_states.num_checked_states =
                instance->i__num_checked_states;
#ifndef FCS_DISABLE_NUM_STORED_STATES
        user->init_num_checked_states.num_states_in_collection =
            init_num_checked_states.num_states_in_collection =
                instance->num_states_in_collection;
#endif

        if (is_start_of_flare_solving)
        {
            fc_solve_start_instance_process_with_board(instance, &(user->state),
#if defined(FCS_WITH_FLARES) && !defined(FCS_DISABLE_PATSOLVE)
                &(user->initial_non_canonized_state)
#else
                NULL
#endif
                    );
        }

        const fcs_bool_t was_run_now =
            ((flare->ret_code == FCS_STATE_SUSPEND_PROCESS) ||
                (flare->ret_code == FCS_STATE_NOT_BEGAN_YET));

        if (was_run_now)
        {
            ret = user->ret_code = flare->ret_code =
                fc_solve_resume_instance(instance);
            flare->instance_is_ready = FALSE;
        }

        if (ret != FCS_STATE_SUSPEND_PROCESS)
        {
            user->all_instances_were_suspended = FALSE;
        }

        flare->obj_stats.num_checked_states = instance->i__num_checked_states;
#ifndef FCS_DISABLE_NUM_STORED_STATES
        flare->obj_stats.num_states_in_collection =
            instance->num_states_in_collection;
#endif
        const_AUTO(delta, flare->obj_stats.num_checked_states -
                              init_num_checked_states.num_checked_states);
        user->iterations_board_started_at.num_checked_states += delta;
#ifdef FCS_WITH_FLARES
        if (flare_iters_quota >= 0)
        {
            current_plan_item->remaining_quota =
                normalize_iters_quota(flare_iters_quota - delta);
        }
#endif
#ifndef FCS_DISABLE_NUM_STORED_STATES
        user->iterations_board_started_at.num_states_in_collection +=
            flare->obj_stats.num_states_in_collection -
            init_num_checked_states.num_states_in_collection;
#endif
        user->init_num_checked_states = flare->obj_stats;

        if (user->ret_code == FCS_STATE_WAS_SOLVED)
        {
            flare->was_solution_traced = FALSE;
#ifdef FCS_WITH_FLARES
            if ((!(instance_item->minimal_flare)) ||
                (get_flare_move_count(user, instance_item->minimal_flare) >
                    get_flare_move_count(user, flare)))
            {
                instance_item->minimal_flare = flare;
            }
#else
            instance_item->was_flare_found = TRUE;
            instance_item->was_flare_finished = TRUE;
#endif
            ret = user->ret_code = FCS_STATE_IS_NOT_SOLVEABLE;
        }
        else if (user->ret_code == FCS_STATE_IS_NOT_SOLVEABLE)
        {
            if (was_run_now)
            {
                fc_solve_recycle_instance(instance);
                flare->instance_is_ready = TRUE;
            }
#ifndef FCS_WITH_FLARES
            instance_item->was_flare_finished = TRUE;
#endif
        }
        else if (user->ret_code == FCS_STATE_SUSPEND_PROCESS)
        {
            /*
             * First - check if we exceeded our limit. If so - we must terminate
             * and return now.
             * */
            if (((user->current_iterations_limit >= 0) &&
                    (user->iterations_board_started_at.num_checked_states >=
                        user->current_iterations_limit))
#ifndef FCS_DISABLE_NUM_STORED_STATES
                || (instance->num_states_in_collection >=
                       instance->effective_max_num_states_in_collection)
#endif
                    )
            {
/* Bug fix:
 * We need to resume from the last flare in case we exceed
 * the board iterations limit.
 * */
#ifdef FCS_WITH_FLARES
                instance_item->current_plan_item_idx--;
#endif
                break;
            }

#ifdef FCS_WITH_FLARES
            current_plan_item->remaining_quota =
                current_plan_item->initial_quota;
#endif
            ret = FCS_STATE_IS_NOT_SOLVEABLE;
            /*
             * Determine if we exceeded the instance-specific quota and if
             * so, designate it as unsolvable.
             * */
            if ((local_limit() >= 0) &&
                (instance->i__num_checked_states >= local_limit()))
            {
                flare->obj_stats.num_checked_states =
                    instance->i__num_checked_states;
#ifndef FCS_DISABLE_NUM_STORED_STATES
                flare->obj_stats.num_states_in_collection =
                    instance->num_states_in_collection;
#endif
                recycle_instance(user, instance_item);
                user->current_instance++;
                continue;
            }
#ifdef FCS_WITH_FLARES
            instance_item->all_plan_items_finished_so_far = FALSE;
#else
            instance_item->was_flare_finished = TRUE;
#endif
        }

    } while ((user->current_instance < end_of_instances_list) &&
             (ret == FCS_STATE_IS_NOT_SOLVEABLE));

    return (
        user->all_instances_were_suspended ? FCS_STATE_SUSPEND_PROCESS : ret);
}

#ifndef FCS_WITHOUT_EXPORTED_RESUME_SOLUTION
int DLLEXPORT freecell_solver_user_resume_solution(void *const api_instance)
{
    return resume_solution((fcs_user_t *)api_instance);
}
#endif

int DLLEXPORT freecell_solver_user_solve_board(
    void *const api_instance, const char *const state_as_string)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    if (!duplicate_string_while_adding_a_trailing_newline(
            user->state_string_copy, state_as_string))
    {
        return FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT;
    }

    user->current_instance = user->instances_list;

#ifndef FCS_FREECELL_ONLY
    {
        FLARES_LOOP_START()
        fc_solve_apply_preset_by_ptr(&(flare->obj), &(user->common_preset));
        FLARES_LOOP_END()
    }
#endif
#ifdef FCS_WITH_FLARES
    if (user_compile_all_flares_plans(user) != FCS_COMPILE_FLARES_RET_OK)
    {
        return FCS_STATE_FLARES_PLAN_ERROR;
    }
    INSTANCES_LOOP_START()
    const_SLOT(num_plan_items, instance_item);
    const_SLOT(plan, instance_item);
    for (int i = 0; i < num_plan_items; i++)
    {
        flares_plan_item *item = plan + i;
        item->remaining_quota = item->initial_quota;
    }
    INSTANCES_LOOP_END()
#endif

#ifdef FCS_WITHOUT_EXPORTED_RESUME_SOLUTION
    return resume_solution(user);
#else
    return freecell_solver_user_resume_solution(api_instance);
#endif
}

#ifdef FCS_WITH_MOVES
static inline fcs_flare_item_t *calc_moves_flare(fcs_user_t *const user)
{
#ifdef FCS_WITH_FLARES
    fcs_flare_item_t *const flare =
        get_current_instance_item(user)->minimal_flare;
#else
    fcs_flare_item_t *const flare =
        &(get_current_instance_item(user)->single_flare);
#endif
    trace_flare_solution(user, flare);
    return flare;
}
#endif

#ifdef FCS_WITH_MOVES
int DLLEXPORT freecell_solver_user_get_next_move(
    void *const api_instance, fcs_move_t *const user_move)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    if (user->ret_code != FCS_STATE_WAS_SOLVED)
    {
        return 1;
    }
    fcs_flare_item_t *const flare = calc_moves_flare(user);
    if (flare->next_move_idx == flare->moves_seq.num_moves)
    {
        return 1;
    }

#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) &&  \
         defined(HARD_CODED_NUM_DECKS)))
    var_AUTO(instance, user_obj(user));
#endif

    fc_solve_apply_move(&(user->running_state.s), NULL,
        user_move_to_internal_move(
                *user_move = flare->moves_seq.moves[flare->next_move_idx++])
            PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
                PASS_STACKS(INSTANCE_STACKS_NUM));

    return 0;
}
#endif

DLLEXPORT void freecell_solver_user_current_state_stringify(void *api_instance,
    char *const output_string FC_SOLVE__PASS_PARSABLE(int parseable_output),
    int canonized_order_output FC_SOLVE__PASS_T(int display_10_as_t))
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) &&  \
         defined(HARD_CODED_NUM_DECKS)))
    var_AUTO(instance, user_obj(user));
#endif

    fc_solve_state_as_string(output_string, &(user->running_state.s),
        &(user->initial_state_locs)PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
            PASS_STACKS(INSTANCE_STACKS_NUM) PASS_DECKS(INSTANCE_DECKS_NUM)
                FC_SOLVE__PASS_PARSABLE(parseable_output),
        canonized_order_output FC_SOLVE__PASS_T(display_10_as_t));
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT char *freecell_solver_user_current_state_as_string(
    void *api_instance FC_SOLVE__PASS_PARSABLE(int parseable_output),
    int canonized_order_output FC_SOLVE__PASS_T(int display_10_as_t))
{
    char *state_as_string = SMALLOC(state_as_string, 1000);

    freecell_solver_user_current_state_stringify(api_instance,
        state_as_string FC_SOLVE__PASS_PARSABLE(parseable_output),
        canonized_order_output FC_SOLVE__PASS_T(display_10_as_t));

    return state_as_string;
}
#endif

static void user_free_resources(fcs_user_t *const user)
{
    FLARES_LOOP_START()
    {
        const int ret_code = flare->ret_code;
        fc_solve_instance_t *const instance = &(flare->obj);

        if ((ret_code != FCS_STATE_NOT_BEGAN_YET) &&
            (ret_code != FCS_STATE_INVALID_STATE) &&
            (!flare->instance_is_ready))
        {
            fc_solve_finish_instance(instance);
        }

        fc_solve_free_instance(instance);

        flare->name[0] = '\0';

#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
        fc_solve_moves_processed_free(&(flare->fc_pro_moves));
#endif

#ifdef FCS_WITH_MOVES
        if (flare->moves_seq.moves)
        {
            free(flare->moves_seq.moves);
            flare->moves_seq.moves = NULL;
            flare->moves_seq.num_moves = 0;
        }
#endif
    }
    INSTANCE_ITEM_FLARES_LOOP_END()
#ifdef FCS_WITH_FLARES
    free(instance_item->flares);
    if (instance_item->flares_plan_string)
    {
        free(instance_item->flares_plan_string);
    }
    if (instance_item->plan)
    {
        free(instance_item->plan);
    }
#endif
    INSTANCES_LOOP_END()

    free(user->instances_list);
    fc_solve_meta_compact_allocator_finish(&(user->meta_alloc));
}

void DLLEXPORT freecell_solver_user_free(void *const api_instance)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    user_free_resources(user);

    free(user);
}

int DLLEXPORT freecell_solver_user_get_current_depth(void *const api_instance)
{
    return (DFS_VAR(api_soft_thread(api_instance), depth));
}

#ifndef FCS_DISABLE_PATSOLVE
extern int DLLEXPORT freecell_solver_user_set_patsolve_x_param(
    void *const api_instance, const int position,
    const int x_param_val FCS__PASS_ERR_STR(char **const error_string))
{
    const_SLOT(pats_scan, api_soft_thread(api_instance));

    if (!pats_scan)
    {
        ALLOC_ERROR_STRING(error_string, "Not using the \"patsolve\" scan.");
        return 1;
    }
    if ((position < 0) ||
        (position >= (int)(COUNT(pats_scan->pats_solve_params.x))))
    {
        ALLOC_ERROR_STRING(error_string, "Position out of range.");
        return 2;
    }

    pats_scan->pats_solve_params.x[position] = x_param_val;
    fc_solve_pats__set_cut_off(pats_scan);
    return 0;
}
#endif

#ifndef FCS_DISABLE_PATSOLVE
extern int DLLEXPORT freecell_solver_user_set_patsolve_y_param(
    void *const api_instance, const int position,
    const double y_param_val FCS__PASS_ERR_STR(char **const error_string))
{
    const_SLOT(pats_scan, api_soft_thread(api_instance));

    if (!pats_scan)
    {
        ALLOC_ERROR_STRING(error_string, "Not using the \"patsolve\" scan.");
        return 1;
    }
    if ((position < 0) ||
        (position >= (int)(COUNT(pats_scan->pats_solve_params.y))))
    {
        ALLOC_ERROR_STRING(error_string, "Position out of range.");
        return 2;
    }

    pats_scan->pats_solve_params.y[position] = y_param_val;
    return 0;
}
#endif

void DLLEXPORT freecell_solver_user_set_solving_method(
    void *const api_instance, const int int_method)
{
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    if (int_method == FCS_METHOD_HARD_DFS)
    {
        return freecell_solver_user_set_solving_method(
            api_instance, FCS_METHOD_SOFT_DFS);
    }
#endif
    fcs_super_method_type_t super_method_type = FCS_SUPER_METHOD_BEFS_BRFS;
    fc_solve_soft_thread_t *const soft_thread = api_soft_thread(api_instance);
    switch (int_method)
    {
    case FCS_METHOD_BFS:
        soft_thread->is_befs = FALSE;
        break;

    case FCS_METHOD_A_STAR:
        soft_thread->is_befs = TRUE;
        break;

    case FCS_METHOD_RANDOM_DFS:
    case FCS_METHOD_SOFT_DFS:
    {
        super_method_type = FCS_SUPER_METHOD_DFS;
        soft_thread->master_to_randomize =
            (int_method == FCS_METHOD_RANDOM_DFS);
    }
    break;

#ifndef FCS_DISABLE_PATSOLVE
    case FCS_METHOD_PATSOLVE:
    {
        super_method_type = FCS_SUPER_METHOD_PATSOLVE;

        if (!soft_thread->pats_scan)
        {
            typeof(soft_thread->pats_scan) pats_scan = soft_thread->pats_scan =
                SMALLOC1(soft_thread->pats_scan);
            fc_solve_pats__init_soft_thread(
                pats_scan, HT_INSTANCE(soft_thread->hard_thread));

            pats_scan->to_stack = TRUE;

            pats_scan->pats_solve_params =
                (freecell_solver_pats__x_y_params_preset
                        [FC_SOLVE_PATS__PARAM_PRESET__FreecellSpeed]);
            fc_solve_pats__set_cut_off(pats_scan);
        }
    }
    break;
#endif
    }

    soft_thread->super_method_type = super_method_type;
}

#ifndef FCS_FREECELL_ONLY
static inline void calc_variant_suit_mask_and_desired_suit_value(
    fc_solve_instance_t *const instance)
{
#ifndef FCS_DISABLE_PATSOLVE
    instance->game_variant_suit_mask = FCS_PATS__COLOR;
    instance->game_variant_desired_suit_value = FCS_PATS__COLOR;
    if ((GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance) ==
            FCS_SEQ_BUILT_BY_SUIT))
    {
        instance->game_variant_suit_mask = FCS_PATS__SUIT;
        instance->game_variant_desired_suit_value = 0;
    }
#endif
}

static void apply_game_params_for_all_instances(fcs_user_t *const user)
{
    FLARES_LOOP_START()
    {
        fc_solve_instance_t *const instance = &(flare->obj);
        instance->game_params = user->common_preset.game_params;
        calc_variant_suit_mask_and_desired_suit_value(instance);
    }
    FLARES_LOOP_END()

    return;
}

#endif

#ifndef HARD_CODED_NUM_FREECELLS

int DLLEXPORT freecell_solver_user_set_num_freecells(
    void *const api_instance, const int freecells_num)
{
    if ((freecells_num < 0) || (freecells_num > MAX_NUM_FREECELLS))
    {
        return 1;
    }

    fcs_user_t *const user = (fcs_user_t *)api_instance;
    user->common_preset.game_params.freecells_num = freecells_num;
    apply_game_params_for_all_instances(user);

    return 0;
}

#ifdef FC_SOLVE_JAVASCRIPT_QUERYING
int DLLEXPORT freecell_solver_user_get_num_freecells(void *const api_instance)
{
    return (((fcs_user_t * const)api_instance)
                ->common_preset.game_params.freecells_num);
}
#endif

#else

int DLLEXPORT freecell_solver_user_set_num_freecells(
    void *api_instance GCC_UNUSED, int freecells_num GCC_UNUSED)
{
    return 0;
}

#endif

#ifndef HARD_CODED_NUM_STACKS
int DLLEXPORT freecell_solver_user_set_num_stacks(
    void *const api_instance, const int stacks_num)
{
    if ((stacks_num < 0) || (stacks_num > MAX_NUM_STACKS))
    {
        return 1;
    }

    fcs_user_t *const user = (fcs_user_t *)api_instance;
    user->common_preset.game_params.stacks_num = stacks_num;
    apply_game_params_for_all_instances(user);

    return 0;
}

#ifdef FC_SOLVE_JAVASCRIPT_QUERYING
int DLLEXPORT freecell_solver_user_get_num_stacks(void *const api_instance)
{
    return (((fcs_user_t * const)api_instance)
                ->common_preset.game_params.stacks_num);
}
#endif

#else

int DLLEXPORT freecell_solver_user_set_num_stacks(
    void *api_instance GCC_UNUSED, int stacks_num GCC_UNUSED)
{
    return 0;
}

#endif

#ifndef HARD_CODED_NUM_DECKS
int DLLEXPORT freecell_solver_user_set_num_decks(
    void *api_instance, int decks_num)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

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
    void *api_instance GCC_UNUSED, int decks_num GCC_UNUSED)
{
    return 0;
}

#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
int DLLEXPORT freecell_solver_user_set_game(void *const api_instance,
    const int freecells_num, const int stacks_num, const int decks_num,
    const int sequences_are_built_by, const int unlimited_sequence_move,
    const int empty_stacks_fill)
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
    if (freecell_solver_user_set_sequences_are_built_by_type(
            api_instance, sequences_are_built_by))
    {
        return 4;
    }
    if (freecell_solver_user_set_sequence_move(
            api_instance, unlimited_sequence_move))
    {
        return 5;
    }
    if (freecell_solver_user_set_empty_stacks_filled_by(
            api_instance, empty_stacks_fill))
    {
        return 6;
    }

    return 0;
}
#endif

fcs_int_limit_t DLLEXPORT freecell_solver_user_get_num_times_long(
    void *api_instance)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    return user->iterations_board_started_at.num_checked_states +
           max(user->active_flare->obj_stats.num_checked_states,
               user_obj(user)->i__num_checked_states) -
           user->init_num_checked_states.num_checked_states;
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
int DLLEXPORT freecell_solver_user_get_num_times(void *const api_instance)
{
    return (int)freecell_solver_user_get_num_times_long(api_instance);
}

int DLLEXPORT freecell_solver_user_get_limit_iterations(
    void *const api_instance)
{
    return active_obj(api_instance)->effective_max_num_checked_states;
}
#endif

#ifdef FCS_WITH_MOVES
int DLLEXPORT freecell_solver_user_get_moves_left(
    void *const api_instance GCC_UNUSED)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;
    if (user->ret_code == FCS_STATE_WAS_SOLVED)
    {
        const fcs_flare_item_t *const flare = calc_moves_flare(user);
        return flare->moves_seq.num_moves - flare->next_move_idx;
    }
    else
    {
        return 0;
    }
}
#endif

#ifdef FCS_WITH_MOVES
void DLLEXPORT freecell_solver_user_set_solution_optimization(
    void *const api_instance, const int optimize)
{
    STRUCT_SET_FLAG_TO(
        active_obj(api_instance), FCS_RUNTIME_OPTIMIZE_SOLUTION_PATH, optimize);
}
#endif

#ifdef FCS_WITH_MOVES
DLLEXPORT extern void freecell_solver_user_stringify_move_w_state(
    void *const api_instance, char *const output_string, const fcs_move_t move,
    const int standard_notation)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    fc_solve_move_to_string_w_state(
        output_string, &(user->running_state), move, standard_notation);
}
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT char *freecell_solver_user_move_to_string(
    const fcs_move_t move, const int standard_notation)
{
    char *const ret = SMALLOC(ret, 256);
    fc_solve_move_to_string_w_state(
        ret, NULL, move, (standard_notation == 2) ? 1 : standard_notation);
    return ret;
}

DLLEXPORT char *freecell_solver_user_move_to_string_w_state(
    void *const api_instance, const fcs_move_t move,
    const int standard_notation)
{
    char *ret = SMALLOC(ret, 256);
    freecell_solver_user_stringify_move_w_state(
        api_instance, ret, move, standard_notation);
    return ret;
}
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
void DLLEXPORT freecell_solver_user_limit_depth(
    void *const api_instance GCC_UNUSED, const int max_depth GCC_UNUSED)
{
}
#endif

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

#ifdef FCS_WITH_ERROR_STRS
void freecell_solver_user_get_invalid_state_error_into_string(
    void *const api_instance, char *const string
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    ,
    const int print_ts
#endif
    )
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    const_AUTO(ret, user->state_validity_ret);
    switch (ret)
    {
    case FCS_STATE_VALIDITY__OK:
        string[0] = '\0';
        break;

    case FCS_STATE_VALIDITY__EMPTY_SLOT:
        strcpy(string, "There's an empty slot in one of the stacks.");
        break;

    case FCS_STATE_VALIDITY__MISSING_CARD:
    case FCS_STATE_VALIDITY__EXTRA_CARD:
    {
        /*
         * user->state_validity_card is only valid for these states,
         * so we should call fc_solve_card_stringify on there only.
         * */
        char card_str[4];
        fc_solve_card_stringify(
            user->state_validity_card, card_str FC_SOLVE__PASS_T(print_ts));

        sprintf(string, "%s%s.", ((ret == FCS_STATE_VALIDITY__EXTRA_CARD)
                                         ? "There's an extra card: "
                                         : "There's a missing card: "),
            card_str);
    }
    break;

    case FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT:
        strcpy(string, "Not enough input.");
        break;
    }
}
#endif

#ifdef FCS_WITH_ERROR_STRS
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
char *freecell_solver_user_get_invalid_state_error_string(
    void *const api_instance
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    ,
    const int print_ts
#endif
    )
{
    char *ret = malloc(80);
    freecell_solver_user_get_invalid_state_error_into_string(
        api_instance, ret PASS_T(print_ts));
    return ret;
}
#endif
#endif

#ifndef FCS_FREECELL_ONLY
int DLLEXPORT freecell_solver_user_set_sequences_are_built_by_type(
    void *const api_instance, const int sequences_are_built_by)
{
    if ((sequences_are_built_by < 0) || (sequences_are_built_by > 2))
    {
        return 1;
    }
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    user->common_preset.game_params.game_flags &= (~0x3);
    user->common_preset.game_params.game_flags |= sequences_are_built_by;

    apply_game_params_for_all_instances(user);

    return 0;
}
#endif

#ifndef FCS_FREECELL_ONLY
int DLLEXPORT freecell_solver_user_set_sequence_move(
    void *const api_instance, const int unlimited_sequence_move)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    user->common_preset.game_params.game_flags &= (~(1 << 4));
    user->common_preset.game_params.game_flags |=
        ((unlimited_sequence_move != 0) << 4);

    apply_game_params_for_all_instances(user);
    return 0;
}
#endif

#ifndef FCS_FREECELL_ONLY
int DLLEXPORT freecell_solver_user_set_empty_stacks_filled_by(
    void *const api_instance, const int empty_stacks_fill)
{
    if ((empty_stacks_fill < 0) || (empty_stacks_fill > 2))
    {
        return 1;
    }

    fcs_user_t *const user = (fcs_user_t * const)api_instance;
    user->common_preset.game_params.game_flags &= (~(0x3 << 2));
    user->common_preset.game_params.game_flags |= (empty_stacks_fill << 2);
    apply_game_params_for_all_instances(user);
    return 0;
}
#endif

int DLLEXPORT freecell_solver_user_set_a_star_weight(
    void *const api_instance, const int my_index, const double weight)
{
    fc_solve_soft_thread_t *const soft_thread = api_soft_thread(api_instance);
    if ((my_index < 0) || (my_index >= (int)(COUNT(BEFS_VAR(soft_thread,
                                           weighting).befs_weights.weights))))
    {
        return 1;
    }
    if (weight < 0)
    {
        return 2;
    }

    BEFS_VAR(soft_thread, weighting).befs_weights.weights[my_index] = weight;

    return 0;
}

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS
double DLLEXPORT fc_solve_user_INTERNAL_get_befs_weight(
    void *const api_instance, const int my_index)
{
    return BEFS_VAR(api_soft_thread(api_instance), weighting)
        .befs_weights.weights[my_index];
}

#endif

typedef struct
{
    fcs_state_t *key;
#if 0
    fcs_state_extra_info_t * val;
    fcs_collectible_state_t * s;
#endif
    fcs_state_locs_struct_t locs;
} fcs_standalone_state_ptrs_t;

#ifndef FCS_WITHOUT_ITER_HANDLER
static void iter_handler_wrapper(void *const api_instance,
    const fcs_int_limit_t iter_num, const int depth,
    void *lp_instance GCC_UNUSED, fcs_kv_state_t *const ptr_state,
    const fcs_int_limit_t parent_iter_num)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    fcs_standalone_state_ptrs_t state_raw = {
        .key = ptr_state->key,
    };
    fc_solve_init_locs(&(state_raw.locs));

#define CALL(func_ptr)                                                         \
    (func_ptr)(api_instance, iter_num, depth, (void *)&state_raw,              \
        parent_iter_num, user->iter_handler_context)

#ifdef FCS_BREAK_BACKWARD_COMPAT_1
    CALL(user->long_iter_handler);
#else
    if (user->long_iter_handler)
    {
        CALL(user->long_iter_handler);
    }
    else
    {
        CALL(user->iter_handler);
    }
#endif
#undef CALL
}

static inline void set_debug_iter_output_func_to_val(
    fcs_user_t *const user, const fcs_instance_debug_iter_output_func_t value)
{
    FLARES_LOOP_START()
    flare->obj.debug_iter_output_func = value;
    FLARES_LOOP_END()
}

static inline void set_any_iter_handler(void *const api_instance,
    const freecell_solver_user_long_iter_handler_t long_iter_handler,
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    const freecell_solver_user_iter_handler_t iter_handler,
#endif
    void *const iter_handler_context)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    user->long_iter_handler = long_iter_handler;
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    user->iter_handler = iter_handler;
#endif

    fcs_instance_debug_iter_output_func_t cb = NULL;
    if (
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
        iter_handler ||
#endif
        long_iter_handler)
    {
        user->iter_handler_context = iter_handler_context;
        cb = iter_handler_wrapper;
    }
    set_debug_iter_output_func_to_val(user, cb);
}
#endif

/* TODO : Add an compile-time option to remove the iteration handler and all
 * related code. */
#ifndef FCS_WITHOUT_ITER_HANDLER
void DLLEXPORT freecell_solver_user_set_iter_handler_long(void *api_instance,
    freecell_solver_user_long_iter_handler_t long_iter_handler,
    void *iter_handler_context)
{
    set_any_iter_handler(api_instance, long_iter_handler,
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
        NULL,
#endif
        iter_handler_context);
}
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
void DLLEXPORT freecell_solver_user_set_iter_handler(void *const api_instance,
    const freecell_solver_user_iter_handler_t iter_handler,
    void *const iter_handler_context)
{
#ifndef FCS_WITHOUT_ITER_HANDLER
    set_any_iter_handler(
        api_instance, NULL, iter_handler, iter_handler_context);
#endif
}
#endif

#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) &&  \
         defined(HARD_CODED_NUM_DECKS)))
#define HARD_CODED_UNUSED
#else
#define HARD_CODED_UNUSED GCC_UNUSED
#endif

DLLEXPORT extern void freecell_solver_user_iter_state_stringify(
    void *const api_instance HARD_CODED_UNUSED, char *output_string,
    void *const ptr_state_void FC_SOLVE__PASS_PARSABLE(
        const int parseable_output),
    const int canonized_order_output PASS_T(const int display_10_as_t))
{
#if (!(defined(HARD_CODED_NUM_FREECELLS) && defined(HARD_CODED_NUM_STACKS) &&  \
         defined(HARD_CODED_NUM_DECKS)))
    fc_solve_instance_t *const instance = active_obj(api_instance);
#endif

    const_AUTO(
        ptr_state, ((const fcs_standalone_state_ptrs_t *const)ptr_state_void));

    fc_solve_state_as_string(output_string, ptr_state->key,
        &(ptr_state->locs)PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
            PASS_STACKS(INSTANCE_STACKS_NUM) PASS_DECKS(INSTANCE_DECKS_NUM)
                FC_SOLVE__PASS_PARSABLE(parseable_output),
        canonized_order_output PASS_T(display_10_as_t));
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT char *freecell_solver_user_iter_state_as_string(
    void *const api_instance,
    void *const ptr_state_void FC_SOLVE__PASS_PARSABLE(
        const int parseable_output),
    const int canonized_order_output PASS_T(const int display_10_as_t))
{
    char *state_as_string = SMALLOC(state_as_string, 1000);
    freecell_solver_user_iter_state_stringify(api_instance, state_as_string,
        ptr_state_void FC_SOLVE__PASS_PARSABLE(parseable_output),
        canonized_order_output PASS_T(display_10_as_t));
    return state_as_string;
}
#endif

void DLLEXPORT freecell_solver_user_set_random_seed(
    void *const api_instance, const int seed)
{
    DFS_VAR(api_soft_thread(api_instance), rand_seed) = seed;
}

#ifndef FCS_DISABLE_NUM_STORED_STATES
fcs_int_limit_t DLLEXPORT
freecell_solver_user_get_num_states_in_collection_long(void *api_instance)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    return user->iterations_board_started_at.num_states_in_collection +
           user->active_flare->obj_stats.num_states_in_collection -
           user->init_num_checked_states.num_states_in_collection;
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
int DLLEXPORT freecell_solver_user_get_num_states_in_collection(
    void *const api_instance)
{
    return (int)freecell_solver_user_get_num_states_in_collection_long(
        api_instance);
}
#endif

void DLLEXPORT freecell_solver_user_limit_num_states_in_collection_long(
    void *api_instance, fcs_int_limit_t max_num_states)
{
    active_obj(api_instance)->effective_max_num_states_in_collection =
        ((max_num_states < 0) ? FCS_INT_LIMIT_MAX : max_num_states);
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
void DLLEXPORT freecell_solver_user_limit_num_states_in_collection(
    void *const api_instance, const int max_num_states)
{
    return freecell_solver_user_limit_num_states_in_collection_long(
        api_instance, (fcs_int_limit_t)max_num_states);
}
#endif

#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
DLLEXPORT extern void freecell_solver_set_stored_states_trimming_limit(
    void *const api_instance GCC_UNUSED, const long max_num_states GCC_UNUSED)
{
    active_obj(api_instance)->effective_trim_states_in_collection_from =
        ((max_num_states < 0) ? FCS_INT_LIMIT_MAX : max_num_states);
}
#endif
#endif

int DLLEXPORT freecell_solver_user_next_soft_thread(void *const api_instance)
{
    fcs_user_t *const user = (fcs_user_t * const)api_instance;

    fc_solve_soft_thread_t *const soft_thread =
        fc_solve_new_soft_thread(user->soft_thread->hard_thread);

    if (soft_thread == NULL)
    {
        return 1;
    }

    user->soft_thread = soft_thread;

    return 0;
}

extern void DLLEXPORT freecell_solver_user_set_soft_thread_step(
    void *const api_instance, const int checked_states_step)
{
    api_soft_thread(api_instance)->checked_states_step = checked_states_step;
}

int DLLEXPORT freecell_solver_user_next_hard_thread(void *const api_instance)
{
#ifdef FCS_SINGLE_HARD_THREAD
    return freecell_solver_user_next_soft_thread(api_instance);
#else
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    fc_solve_soft_thread_t *const soft_thread =
        fc_solve_new_hard_thread(user_obj(user));

    if (soft_thread == NULL)
    {
        return 1;
    }

    user->soft_thread = soft_thread;

    return 0;
#endif
}

int DLLEXPORT freecell_solver_user_get_num_soft_threads_in_instance(
    void *const api_instance)
{
    return active_obj(api_instance)->next_soft_thread_id;
}

#ifndef FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE
void DLLEXPORT freecell_solver_user_set_calc_real_depth(
    void *const api_instance, const int calc_real_depth)
{
    STRUCT_SET_FLAG_TO(
        active_obj(api_instance), FCS_RUNTIME_CALC_REAL_DEPTH, calc_real_depth);
}
#endif

void DLLEXPORT freecell_solver_user_set_soft_thread_name(
    void *const api_instance, const freecell_solver_str_t name)
{
    fc_solve_soft_thread_t *const soft_thread = api_soft_thread(api_instance);

    strncpy(soft_thread->name, name, COUNT(soft_thread->name));
    soft_thread->name[COUNT(soft_thread->name) - 1] = '\0';
}

#ifdef FCS_WITH_FLARES
void DLLEXPORT freecell_solver_user_set_flare_name(
    void *const api_instance GCC_UNUSED,
    const freecell_solver_str_t name GCC_UNUSED)
{
    fcs_flare_item_t *const flare =
        get_current_instance_item((fcs_user_t *)api_instance)->end_of_flares -
        1;

    strncpy(flare->name, name, COUNT(flare->name));
    flare->name[COUNT(flare->name) - 1] = '\0';
}
#endif

int DLLEXPORT freecell_solver_user_set_hard_thread_prelude(
    void *const api_instance, const char *const prelude)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;
    fc_solve_hard_thread_t *const hard_thread = user->soft_thread->hard_thread;

    free(HT_FIELD(hard_thread, prelude_as_string));
    HT_FIELD(hard_thread, prelude_as_string) = strdup(prelude);

    return 0;
}

#ifdef FCS_WITH_FLARES
int DLLEXPORT freecell_solver_user_set_flares_plan(
    void *const api_instance GCC_UNUSED,
    const char *const flares_plan_string GCC_UNUSED)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    fcs_instance_item_t *const instance_item = get_current_instance_item(user);

    free(instance_item->flares_plan_string);
    instance_item->flares_plan_string =
        (flares_plan_string ? strdup(flares_plan_string) : NULL);
    instance_item->flares_plan_compiled = FALSE;

    return 0;
}
#endif

void DLLEXPORT freecell_solver_user_recycle(void *api_instance)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    for (fcs_instance_item_t *instance_item = user->instances_list;
         instance_item < user->end_of_instances_list; instance_item++)
    {
        recycle_instance(user, instance_item);
    }
    user->iterations_board_started_at = initial_stats;
}

#ifdef FCS_WITH_MOVES
int DLLEXPORT freecell_solver_user_set_optimization_scan_tests_order(
    void *const api_instance,
    const char *const tests_order FCS__PASS_ERR_STR(char **const error_string))
{
    var_AUTO(obj, active_obj(api_instance));
    fc_solve_free_tests_order(&obj->opt_tests_order);
    STRUCT_CLEAR_FLAG(obj, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET);
#ifdef FCS_WITH_ERROR_STRS
    char static_error_string[120];
#endif
    const int ret = fc_solve_apply_tests_order(&(obj->opt_tests_order),
        tests_order FCS__PASS_ERR_STR(static_error_string));
    SET_ERROR_VAR(error_string, static_error_string);
    if (!ret)
    {
        STRUCT_TURN_ON_FLAG(obj, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET);
    }
    return ret;
}
#endif

extern int DLLEXPORT freecell_solver_user_set_pruning(void *api_instance,
    const char *pruning FCS__PASS_ERR_STR(char **error_string))
{
    fc_solve_soft_thread_t *const soft_thread = api_soft_thread(api_instance);

    if (!strcmp(pruning, "r:tf"))
    {
        soft_thread->enable_pruning = TRUE;
    }
    else if (pruning[0] == '\0')
    {
        soft_thread->enable_pruning = FALSE;
    }
    else
    {
        ALLOC_ERROR_STRING(
            error_string, "Unknown pruning value - must be \"r:tf\" or empty.");
        return 1;
    }
    return 0;
}

#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
void DLLEXPORT freecell_solver_user_set_reparent_states(
    void *const api_instance, const int to_reparent_states)
{
    STRUCT_SET_FLAG_TO(active_obj(api_instance),
        FCS_RUNTIME_TO_REPARENT_STATES_PROTO, to_reparent_states);
}
#endif

#ifndef FCS_HARD_CODE_SCANS_SYNERGY_AS_TRUE
void DLLEXPORT freecell_solver_user_set_scans_synergy(
    void *const api_instance, const int synergy)
{
    STRUCT_SET_FLAG_TO(
        active_obj(api_instance), FCS_RUNTIME_SCANS_SYNERGY, synergy);
}
#endif

int DLLEXPORT freecell_solver_user_next_instance(void *const api_instance)
{
    return user_next_instance((fcs_user_t *)api_instance);
}

static int user_next_flare(fcs_user_t *const user)
{
    fcs_instance_item_t *const instance_item = get_current_instance_item(user);
#ifdef FCS_WITH_FLARES
    const_AUTO(
        num_flares, instance_item->end_of_flares - instance_item->flares);
    instance_item->flares = SREALLOC(instance_item->flares, num_flares + 1);
    fcs_flare_item_t *const flare = instance_item->flares + num_flares;
    instance_item->end_of_flares = flare + 1;
#else
    fcs_flare_item_t *const flare = &(instance_item->single_flare);
#endif
    instance_item->limit = flare->limit = -1;
    fc_solve_instance_t *const instance = &(flare->obj);

    user->active_flare = flare;
    fc_solve_alloc_instance(instance, &(user->meta_alloc));

    /*
     * Switch the soft_thread variable so it won't refer to the old
     * instance
     * */
    user->soft_thread = fc_solve_instance_get_first_soft_thread(instance);

#ifndef FCS_FREECELL_ONLY
    fc_solve_apply_preset_by_ptr(instance, &(user->common_preset));
    calc_variant_suit_mask_and_desired_suit_value(instance);
#endif

    user->ret_code = flare->ret_code = FCS_STATE_NOT_BEGAN_YET;

#ifndef FCS_WITHOUT_ITER_HANDLER
    instance->debug_iter_output_func = ((
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
                                            user->iter_handler ||
#endif
                                            user->long_iter_handler)
                                            ? iter_handler_wrapper
                                            : NULL);
    instance->debug_iter_output_context = user;
#endif

#ifdef FCS_WITH_MOVES
    flare->moves_seq.num_moves = 0;
    flare->moves_seq.moves = NULL;
#endif

    flare->name[0] = '\0';
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    flare->fc_pro_moves.moves = NULL;
#endif
    flare->instance_is_ready = TRUE;
    flare->obj_stats = initial_stats;

    return 0;
}

static int user_next_instance(fcs_user_t *const user)
{
    const_AUTO(
        num_instances, user->end_of_instances_list - user->instances_list);
    user->instances_list = SREALLOC(user->instances_list, num_instances + 1);

    user->end_of_instances_list =
        (user->current_instance = user->instances_list + num_instances) + 1;

    *(get_current_instance_item(user)) = (fcs_instance_item_t){
#ifdef FCS_WITH_FLARES
        .flares = NULL,
        .end_of_flares = NULL,
        .plan = NULL,
        .num_plan_items = 0,
        .flares_plan_string = NULL,
        .flares_plan_compiled = FALSE,
        .current_plan_item_idx = 0,
        .minimal_flare = NULL,
        .all_plan_items_finished_so_far = TRUE,
#else
        .was_flare_finished = FALSE,
#endif
    };

    /* ret_code and limit are set at user_next_flare(). */

    return user_next_flare(user);
}

#ifdef FCS_WITH_FLARES
int DLLEXPORT freecell_solver_user_next_flare(
    void *const api_instance GCC_UNUSED)
{
    return user_next_flare((fcs_user_t *)api_instance);
}
#endif

int DLLEXPORT freecell_solver_user_reset(void *const api_instance)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    user_free_resources(user);

    user_initialize(user);

    return 0;
}

DLLEXPORT const char *freecell_solver_user_get_lib_version(
    void *api_instance GCC_UNUSED)
{
    return VERSION;
}

/* TODO : optionally Remove from the API */
DLLEXPORT const char *freecell_solver_user_get_current_soft_thread_name(
    void *const api_instance)
{
#ifdef FCS_SINGLE_HARD_THREAD
    const fc_solve_hard_thread_t *const hard_thread = active_obj(api_instance);
#else
    const fc_solve_hard_thread_t *const hard_thread =
        active_obj(api_instance)->current_hard_thread;
#endif

    return HT_FIELD(hard_thread, soft_threads)[HT_FIELD(hard_thread, st_idx)]
        .name;
}

#ifdef FCS_WITH_ERROR_STRS
DLLEXPORT const char *freecell_solver_user_get_last_error_string(
    void *const api_instance)
{
    return (((fcs_user_t * const)api_instance)->error_string);
}
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
#ifdef FCS_RCS_STATES
int DLLEXPORT freecell_solver_user_set_cache_limit(
    void *const api_instance GCC_UNUSED, const long limit GCC_UNUSED)
{
    if (limit <= 0)
    {
        return -1;
    }
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    FLARES_LOOP_START()
    flare->obj.rcs_states_cache.max_num_elements_in_cache = limit;
    FLARES_LOOP_END()

    return 0;
}
#endif
#endif

#ifdef FCS_WITH_MOVES
int DLLEXPORT freecell_solver_user_get_moves_sequence(
    void *const api_instance, fcs_moves_sequence_t *const moves_seq)
{
    const fcs_user_t *const user = (const fcs_user_t *)api_instance;
    if (user->ret_code != FCS_STATE_WAS_SOLVED)
    {
        return -2;
    }
#ifdef FCS_WITH_FLARES
    const fcs_moves_sequence_t *const src_moves_seq =
        &(get_current_instance_item(user)->minimal_flare->moves_seq);
#else
    const fcs_moves_sequence_t *const src_moves_seq =
        &(get_current_instance_item(user)->single_flare.moves_seq);
#endif

    moves_seq->moves = memdup(src_moves_seq->moves,
        (sizeof(src_moves_seq->moves[0]) *
            (moves_seq->num_moves = src_moves_seq->num_moves)));

    return 0;
}
#endif

#ifdef FCS_WITH_FLARES
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
DLLEXPORT extern int freecell_solver_user_set_flares_choice(
    void *api_instance GCC_UNUSED,
    const char *const new_flares_choice_string GCC_UNUSED)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    if (!strcmp(new_flares_choice_string, "fc_solve"))
    {
        user->flares_choice = FLARES_CHOICE_FC_SOLVE_SOLUTION_LEN;
    }
    else if (!strcmp(new_flares_choice_string, "fcpro"))
    {
        user->flares_choice = FLARES_CHOICE_FCPRO_SOLUTION_LEN;
    }
    else
    {
        return -1;
    }
    return 0;
}
#endif
#endif

#ifdef FCS_WITH_FLARES
DLLEXPORT extern void freecell_solver_user_set_flares_iters_factor(
    void *const api_instance GCC_UNUSED, const double new_factor GCC_UNUSED)
{
    fcs_user_t *const user = (fcs_user_t *)api_instance;

    user->flares_iters_factor = new_factor;
}
#endif

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS

int DLLEXPORT fc_solve_user_INTERNAL_compile_all_flares_plans(
    void *const api_instance GCC_UNUSED, char **const error_string)
{
#ifdef FCS_WITH_FLARES
    fcs_user_t *const user = (fcs_user_t *)api_instance;
    const fcs_compile_flares_ret_t ret = user_compile_all_flares_plans(user);
#ifdef FCS_WITH_ERROR_STRS
    SET_ERROR_VAR(error_string, user->error_string);
#else
    *error_string = NULL;
#endif
    return ret;
#else
    *error_string = NULL;
    return 0;
#endif
}

int DLLEXPORT fc_solve_user_INTERNAL_get_flares_plan_num_items(
    void *const api_instance GCC_UNUSED)
{
#ifdef FCS_WITH_FLARES
    return get_current_instance_item((fcs_user_t * const)api_instance)
        ->num_plan_items;
#else
    return 0;
#endif
}

const DLLEXPORT char *fc_solve_user_INTERNAL_get_flares_plan_item_type(
    void *const api_instance GCC_UNUSED, const int item_idx GCC_UNUSED)
{
#ifdef FCS_WITH_FLARES
    switch (get_current_instance_item((fcs_user_t * const)api_instance)
                ->plan[item_idx]
                .type)
    {
    case FLARES_PLAN_RUN_INDEFINITELY:
        return "RunIndef";
        break;
    case FLARES_PLAN_RUN_COUNT_ITERS:
        return "Run";
        break;
    case FLARES_PLAN_CHECKPOINT:
        return "CP";
        break;
    default:
        fc_solve_err("%s\n", "Unknown flares plan item type. Something is "
                             "Wrong on the Internet.");
    }
#else
    return "";
#endif
}

int DLLEXPORT fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(
    void *const api_instance GCC_UNUSED, const int item_idx GCC_UNUSED)
{
#ifdef FCS_WITH_FLARES
    fcs_instance_item_t *const instance_item =
        get_current_instance_item((fcs_user_t * const)api_instance);
    return instance_item->plan[item_idx].flare - instance_item->flares;
#else
    return 0;
#endif
}

int DLLEXPORT fc_solve_user_INTERNAL_get_flares_plan_item_iters_count(
    void *const api_instance GCC_UNUSED, const int item_idx GCC_UNUSED)
{
#ifdef FCS_WITH_FLARES
    return get_current_instance_item((fcs_user_t * const)api_instance)
        ->plan[item_idx]
        .count_iters;
#else
    return 0;
#endif
}

int DLLEXPORT fc_solve_user_INTERNAL_get_num_by_depth_tests_order(
    void *const api_instance)
{
    return api_soft_thread(api_instance)->by_depth_tests_order.num;
}

int DLLEXPORT fc_solve_user_INTERNAL_get_by_depth_tests_max_depth(
    void *const api_instance, const int depth_idx)
{
    return api_soft_thread(api_instance)
        ->by_depth_tests_order.by_depth_tests[depth_idx]
        .max_depth;
}

#endif
