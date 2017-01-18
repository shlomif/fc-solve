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
 * main.c - the main() and related functions of the fc-solve command line
 * executable.
 *
 * It is documented in the documents "README", "USAGE", etc. in the
 * Freecell Solver distribution from http://fc-solve.shlomifish.org/ .
 */
#ifdef _MSC_VER
#ifdef BUILDING_DLL
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif
#define DLLLOCAL
#elif defined(__EMSCRIPTEN__)
#include "emscripten.h"
#define DLLEXPORT EMSCRIPTEN_KEEPALIVE
#define DLLLOCAL __attribute__((visibility("hidden")))
#elif defined(__GNUC__)
#define DLLEXPORT __attribute__((visibility("default")))
#define DLLLOCAL __attribute__((visibility("hidden")))
#else
#define DLLEXPORT
#define DLLLOCAL
#endif
#include "rinutils.h"
#include "instance_for_lib.h"

#define FCS_MAX_FLARE_NAME_LEN 30

typedef struct
{
    fcs_int_limit_t num_checked_states;
    fcs_int_limit_t num_states_in_collection;
} fcs_stats_t;

static const fcs_stats_t initial_stats = {
    .num_checked_states = 0, .num_states_in_collection = 0};

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
    int next_move;
    fcs_moves_sequence_t moves_seq;
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
    int count_iters;
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

static int user_next_instance(fcs_user_t *user);

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
    user->current_iterations_limit = -1;

    user->iterations_board_started_at = initial_stats;
    user->all_instances_were_suspended = TRUE;
#ifdef FCS_WITH_FLARES
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    user->flares_choice = FLARES_CHOICE_FC_SOLVE_SOLUTION_LEN;
#endif
    user->flares_iters_factor = 1.0;
#endif

    user_next_instance(user);

    return;
}
static inline fcs_instance_item_t *get_current_instance_item(
    fcs_user_t const *user)
{
    return (user->current_instance);
}
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

#ifdef FCS_WITH_MOVES
    flare->moves_seq.num_moves = 0;
    flare->moves_seq.moves = NULL;
#endif

    flare->name[0] = '\0';
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
static void *freecell_solver_user_alloc(void)
{
    fcs_user_t *const ret = (fcs_user_t *)SMALLOC1(ret);

    user_initialize(ret);

    return (void *)ret;
}

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
#ifndef FCS_FREECELL_ONLY
static int freecell_solver_user_apply_preset(
    void *const api_instance, const char *const preset_name)
{
    const fcs_preset_t *new_preset_ptr;
    const_AUTO(
        status1, fc_solve_get_preset_by_name(preset_name, &new_preset_ptr));
    if (status1 != FCS_PRESET_CODE_OK)
    {
        return status1;
    }
    return FCS_PRESET_CODE_OK;
}
#endif
int main(int argc, char *argv[])
{
    void * instance = freecell_solver_user_alloc();
    freecell_solver_user_apply_preset(instance, "notexist");
    return 0;
}
