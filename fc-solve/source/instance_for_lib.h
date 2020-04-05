// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// instance_for_lib.h - header file that contains declarations
// and definitions that depend on instance.h functions and are only used only
// by lib.c.
// This is done to speed up compilation.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include "scans_impl.h"
#include "preset.h"
#include "move_funcs_order.h"

#ifndef FCS_SINGLE_HARD_THREAD
static GCC_INLINE fc_solve_soft_thread_t *fc_solve_new_hard_thread(
    fc_solve_instance_t *const instance)
{
    fc_solve_hard_thread_t *ret;
    /* Make sure we are not exceeding the maximal number of soft threads
     * for an instance. */
    if (instance->next_soft_thread_id == MAX_NUM_SCANS)
    {
        return NULL;
    }

    instance->hard_threads =
        SREALLOC(instance->hard_threads, instance->num_hard_threads + 1);

    /* Since we SREALLOC()ed the hard_threads, their addresses changed,
     * so we need to update it.
     * */
    HT_LOOP_START()
    {
        ST_LOOP_START() { soft_thread->hard_thread = hard_thread; }
    }

    fc_solve_instance__init_hard_thread(instance,
        (ret = &(instance->hard_threads[instance->num_hard_threads])));

    instance->num_hard_threads++;

    return &(ret->soft_threads[0]);
}
#endif

/*
    This function allocates a Freecell Solver instance struct and set the
    default values in it. After the call to this function, the program can
    set parameters in it which are different from the default.

    Afterwards fc_solve_init_instance() should be called in order
    to really prepare it for solving.
  */
static GCC_INLINE void fc_solve_alloc_instance(
    fc_solve_instance_t *const instance,
    fcs_meta_compact_allocator_t *const meta_alloc)
{
    *(instance) = (fc_solve_instance_t){
        .meta_alloc = meta_alloc,
        .i__num_checked_states = 0,
        .num_states_in_collection = 0,
#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
        .active_num_states_in_collection = 0,
#endif
        .effective_max_num_checked_states = FCS_INT_LIMIT_MAX,
        .effective_max_num_states_in_collection = FCS_INT_LIMIT_MAX,
#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
        .effective_trim_states_in_collection_from = FCS_INT_LIMIT_MAX,
#endif
        .instance_tests_order =
            {
                .num_groups = 0, .groups = NULL,
            },
        .list_of_vacant_states = NULL,
#ifdef FCS_WITH_MOVES
        .opt_tests_order =
            {
                .num_groups = 0, .groups = NULL,
            },
#endif
#ifdef FCS_SINGLE_HARD_THREAD
#ifdef FCS_WITH_MOVES
        .is_optimization_st = FALSE,
#endif
#else
        .num_hard_threads = 0,
        .hard_threads = NULL,
#ifdef FCS_WITH_MOVES
        .optimization_thread = NULL,
#endif
#endif
        .next_soft_thread_id = 0,
#ifndef FCS_WITHOUT_ITER_HANDLER
        .debug_iter_output_func = NULL,
#endif
#ifdef FCS_WITH_MOVES
        .solution_moves = (fcs_move_stack_t){.moves = NULL, .num_moves = 0},
#endif
        .num_hard_threads_finished = 0,
/* Make the 1 the default, because otherwise scans will not cooperate
 * with one another. */
#ifndef FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE
        .FCS_RUNTIME_CALC_REAL_DEPTH = FALSE,
#endif
#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
        .FCS_RUNTIME_TO_REPARENT_STATES_REAL = FALSE,
        .FCS_RUNTIME_TO_REPARENT_STATES_PROTO = FALSE,
#endif
#ifndef FCS_HARD_CODE_SCANS_SYNERGY_AS_TRUE
        .FCS_RUNTIME_SCANS_SYNERGY = TRUE,
#endif
#ifdef FCS_WITH_MOVES
        .FCS_RUNTIME_OPTIMIZE_SOLUTION_PATH = FALSE,
        .FCS_RUNTIME_IN_OPTIMIZATION_THREAD = FALSE,
        .FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET = FALSE,
#endif

#ifdef FCS_RCS_STATES

#define DEFAULT_MAX_NUM_ELEMENTS_IN_CACHE 10000

        .rcs_states_cache.max_num_elements_in_cache =
            DEFAULT_MAX_NUM_ELEMENTS_IN_CACHE,

#undef DEFAULT_MAX_NUM_ELEMENTS_IN_CACHE

#endif
#ifndef FCS_DISABLE_SIMPLE_SIMON
        .is_simple_simon = FALSE,
#endif
    };

#ifndef FCS_FREECELL_ONLY
    fc_solve_apply_preset_by_name(instance, "freecell");
#else
    {
#ifdef FCS_WITH_ERROR_STRS
        char no_use[120];
#endif
        fc_solve_apply_tests_order(&(instance->instance_tests_order),
            "[01][23456789]" FCS__PASS_ERR_STR(no_use));
    }
#endif

/****************************************/
#ifdef FCS_SINGLE_HARD_THREAD
    fc_solve_instance__init_hard_thread(instance);
#else
    fc_solve_new_hard_thread(instance);
#endif
}

static GCC_INLINE void fc_solve__hard_thread__compile_prelude(
    fc_solve_hard_thread_t *const hard_thread)
{
    fcs_bool_t last_one = FALSE;
    size_t num_items = 0;
    fcs_prelude_item_t *prelude = NULL;

    const char *p = HT_FIELD(hard_thread, prelude_as_string);

    while (!last_one)
    {
        const size_t p_quota = (size_t)atol(p);
        while ((*p) && isdigit(*p))
        {
            p++;
        }
        if (*p != '@')
        {
            free(prelude);
            return;
        }
        p++;
        const char *const p_scan = p;
        while ((*p) && ((*p) != ','))
        {
            p++;
        }
        if ((*p) == '\0')
        {
            last_one = TRUE;
        }
        char p_scan_copy[FCS_MAX_IDENT_LEN];
        strncpy(p_scan_copy, p_scan, COUNT(p_scan_copy));
        p_scan_copy[p - p_scan] = p_scan_copy[COUNT(p_scan_copy) - 1] = '\0';
        p++;

        ST_LOOP_START()
        {
            if (!strcmp(soft_thread->name, p_scan_copy))
            {
                break;
            }
        }
        if (ST_LOOP__WAS_FINISHED())
        {
            free(prelude);
            return;
        }
#define PRELUDE_GROW_BY 16
        if (!(num_items & (PRELUDE_GROW_BY - 1)))
        {
            prelude = SREALLOC(prelude, num_items + PRELUDE_GROW_BY);
        }
#undef PRELUDE_GROW_BY
        prelude[num_items++] = (typeof(prelude[0])){
            .scan_idx = ST_LOOP__GET_INDEX(), .quota = p_quota};
    }

    HT_FIELD(hard_thread, prelude) = SREALLOC(prelude, num_items);
    HT_FIELD(hard_thread, prelude_num_items) = num_items;
    HT_FIELD(hard_thread, prelude_idx) = 0;
}

static GCC_INLINE void set_next_soft_thread(
    fc_solve_hard_thread_t *const hard_thread, const int scan_idx,
    const int quota, int *const st_idx_ptr)
{
    (*st_idx_ptr) = scan_idx;
    /*
     * Calculate a soft thread-wise limit for this hard
     * thread to run.
     * */
    HT_FIELD(hard_thread, ht__max_num_checked_states) =
        NUM_CHECKED_STATES + quota;
}

static GCC_INLINE void fc_solve_init_instance(
    fc_solve_instance_t *const instance)
{
    /* Initialize the state packs */
    HT_LOOP_START()
    {
/* The pointer to instance may change as the flares array get resized
 * so the pointers need to be reassigned to it.
 * */
#ifndef FCS_SINGLE_HARD_THREAD
        hard_thread->instance = instance;
#else
        ST_LOOP_START() { soft_thread->hard_thread = instance; }
#endif
        if (HT_FIELD(hard_thread, prelude_as_string))
        {
            if (!HT_FIELD(hard_thread, prelude))
            {
                fc_solve__hard_thread__compile_prelude(hard_thread);
            }
        }
        set_next_soft_thread(hard_thread, 0,
            HT_FIELD(hard_thread, soft_threads)[0].checked_states_step,
            &(HT_FIELD(hard_thread, st_idx)));
    }

    {
        size_t total_move_funcs_bitmask = 0;
        fc_solve_foreach_soft_thread(instance,
            FOREACH_SOFT_THREAD_ACCUM_TESTS_ORDER, &total_move_funcs_bitmask);
        fc_solve_foreach_soft_thread(instance,
            FOREACH_SOFT_THREAD_DETERMINE_SCAN_COMPLETENESS,
            &total_move_funcs_bitmask);
#ifdef FCS_WITH_MOVES
        if (!STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET))
        {
            /*
             *
             * What this code does is convert the bit map of
             * total_move_funcs_bitmask
             * to a valid tests order.
             *
             * */
            size_t num_move_funcs = 0;
            size_t *tests =
                SMALLOC(tests, sizeof(total_move_funcs_bitmask) * 8);

            for (size_t bit_idx = 0; total_move_funcs_bitmask != 0;
                 bit_idx++, total_move_funcs_bitmask >>= 1)
            {
                if ((total_move_funcs_bitmask & 0x1) != 0)
                {
                    tests[num_move_funcs++] = bit_idx;
                }
            }
            tests = SREALLOC(
                tests, ((num_move_funcs & (~(TESTS_ORDER_GROW_BY - 1))) +
                           TESTS_ORDER_GROW_BY));
            instance->opt_tests_order = (typeof(instance->opt_tests_order)){
                .num_groups = 1,
                .groups = SMALLOC(
                    instance->opt_tests_order.groups, TESTS_ORDER_GROW_BY),
            };
            instance->opt_tests_order.groups[0] =
                (typeof(instance->opt_tests_order.groups[0])){
                    .order_group_tests = tests,
                    .num = num_move_funcs,
                    .shuffling_type = FCS_NO_SHUFFLING,
                };
            STRUCT_TURN_ON_FLAG(instance, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET);
        }
#endif
    }

#ifdef FCS_RCS_STATES
    {
        fcs_lru_cache_t *cache = &(instance->rcs_states_cache);

#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
        cache->states_values_to_keys_map = ((Pvoid_t)NULL);
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
        cache->kaz_tree = fc_solve_kaz_tree_create(
            fc_solve_compare_lru_cache_keys, NULL, instance->meta_alloc);
#else
#error Unknown FCS_RCS_CACHE_STORAGE
#endif

        fc_solve_compact_allocator_init(
            &(cache->states_values_to_keys_allocator), instance->meta_alloc);
        cache->lowest_pri = NULL;
        cache->highest_pri = NULL;
        cache->recycle_bin = NULL;
        cache->count_elements_in_cache = 0;
    }

#endif
}

/* These are all stack comparison functions to be used for the stacks
   cache when using INDIRECT_STACK_STATES
*/
#if defined(INDIRECT_STACK_STATES)

#if ((FCS_STACK_STORAGE != FCS_STACK_STORAGE_GLIB_TREE) &&                     \
     (FCS_STACK_STORAGE != FCS_STACK_STORAGE_GLIB_HASH) &&                     \
     (FCS_STACK_STORAGE != FCS_STACK_STORAGE_JUDY))
#if (((FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH) &&                \
         (defined(FCS_WITH_CONTEXT_VARIABLE)) &&                               \
         (!defined(FCS_INLINED_HASH_COMPARISON))) ||                           \
     (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE) ||                  \
     (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE))

static int fcs_stack_compare_for_comparison_with_context(const void *const v_s1,
    const void *const v_s2,
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    const
#endif
    void *context GCC_UNUSED)
{
    return fc_solve_stack_compare_for_comparison(v_s1, v_s2);
}
#endif
#endif

#endif
#ifdef FCS_RCS_STATES

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE) ||                  \
     (FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE))

static GCC_INLINE fcs_state_t *rcs_states_get_state(
    fc_solve_instance_t *const instance, fcs_collectible_state_t *const state)
{
    return ((state == instance->tree_new_state)
                ? instance->tree_new_state_key
                : fc_solve_lookup_state_key_from_val(instance, state));
}

static int fc_solve_rcs_states_compare(
    const void *const void_a, const void *const void_b, void *const context)
{
    fc_solve_instance_t *const instance = (fc_solve_instance_t *)context;

    return fc_solve_state_compare(
        rcs_states_get_state(instance, (fcs_collectible_state_t *)void_a),
        rcs_states_get_state(instance, (fcs_collectible_state_t *)void_b));
}

#endif

#define STATE_STORAGE_TREE_COMPARE() fc_solve_rcs_states_compare
#define STATE_STORAGE_TREE_CONTEXT() instance

#else

#define STATE_STORAGE_TREE_COMPARE() fc_solve_state_compare_with_context
#define STATE_STORAGE_TREE_CONTEXT() NULL

#endif

static GCC_INLINE void set_next_prelude_item(
    fc_solve_hard_thread_t *const hard_thread,
    const fcs_prelude_item_t *const prelude, int *const st_idx_ptr)
{
    const fcs_prelude_item_t next_item =
        prelude[HT_FIELD(hard_thread, prelude_idx)++];
    set_next_soft_thread(
        hard_thread, next_item.scan_idx, next_item.quota, st_idx_ptr);
}

#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH
#ifdef FCS_USE_ANHOLT_HASH
static int fc_solve_stack_equal(const void *const v_s1, const void *const v_s2)
{
    return (!fc_solve_stack_compare_for_comparison(v_s1, v_s2));
}
#endif
#endif

/*
    This function associates a board with an fc_solve_instance_t and
    does other initialisations. After it, you must call
    fc_solve_resume_instance() repeatedly.
  */
static GCC_INLINE void fc_solve_start_instance_process_with_board(
    fc_solve_instance_t *const instance,
    fcs_state_keyval_pair_t *const init_state,
    fcs_state_keyval_pair_t *const initial_non_canonized_state)
{
    instance->initial_non_canonized_state = initial_non_canonized_state;
    /* Allocate the first state and initialize it to init_state */
    fcs_state_keyval_pair_t *const state_copy_ptr =
        (fcs_state_keyval_pair_t *)fcs_compact_alloc_ptr(
            &(INST_HT0(instance).allocator), sizeof(*state_copy_ptr));

    FCS_STATE__DUP_keyval_pair(*state_copy_ptr, *init_state);

#ifdef INDIRECT_STACK_STATES
    {
        int i;
        char *buffer;

        buffer = INST_HT0(instance).indirect_stacks_buffer;

        for (i = 0; i < INSTANCE_STACKS_NUM; i++)
        {
            fcs_copy_stack(state_copy_ptr->s, state_copy_ptr->info, i, buffer);
        }
    }
#endif

/* Initialize the state to be a base state for the game tree */
#ifndef FCS_WITHOUT_DEPTH_FIELD
    state_copy_ptr->info.depth = 0;
#endif
#ifdef FCS_WITH_MOVES
    state_copy_ptr->info.moves_to_parent = NULL;
#endif
    state_copy_ptr->info.visited = 0;
    state_copy_ptr->info.parent = NULL;
    memset(&(state_copy_ptr->info.scan_visited), '\0',
        sizeof(state_copy_ptr->info.scan_visited));

    instance->state_copy_ptr = state_copy_ptr;

/* Initialize the data structure that will manage the state collection */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    instance->tree =
        rbinit(STATE_STORAGE_TREE_COMPARE(), STATE_STORAGE_TREE_CONTEXT());
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE)

    instance->tree = fcs_libavl2_states_tree_create(
        STATE_STORAGE_TREE_COMPARE(), STATE_STORAGE_TREE_CONTEXT(), NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE)

    instance->tree = fc_solve_kaz_tree_create(
        STATE_STORAGE_TREE_COMPARE(), STATE_STORAGE_TREE_CONTEXT());

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    instance->tree = g_tree_new(fc_solve_state_compare);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY)
    instance->judy_array = ((Pvoid_t)NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    instance->hash =
        g_hash_table_new(fc_solve_hash_function, fc_solve_state_compare_equal);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
#ifdef FCS_USE_ANHOLT_HASH
    set_create(&(instance->hash), NULL, fc_solve_state_compare_equal);
#else
    fc_solve_hash_init(instance->meta_alloc, &(instance->hash),
#ifdef FCS_INLINED_HASH_COMPARISON
        FCS_INLINED_HASH__STATES
#else
#ifdef FCS_WITH_CONTEXT_VARIABLE
        fc_solve_state_compare_with_context,

        NULL
#else
        fc_solve_state_compare
#endif
#endif
        );
#ifdef FCS_RCS_STATES
    instance->hash.instance = instance;
#endif
#endif
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)
    instance->hash = fc_solve_states_google_hash_new();
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
/* Do nothing because it is allocated elsewhere. */
#else
#error FCS_STATE_STORAGE is not defined
#endif

/****************************************************/

#ifdef INDIRECT_STACK_STATES
/* Initialize the data structure that will manage the stack
   collection */
#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH
#ifdef FCS_USE_ANHOLT_HASH
    set_create(&(instance->stacks_hash), NULL, fc_solve_stack_equal);
#else
    fc_solve_hash_init(instance->meta_alloc, &(instance->stacks_hash),
#ifdef FCS_INLINED_HASH_COMPARISON
        FCS_INLINED_HASH__COLUMNS
#else
#ifdef FCS_WITH_CONTEXT_VARIABLE
        fcs_stack_compare_for_comparison_with_context, NULL
#else
        fc_solve_stack_compare_for_comparison
#endif
#endif
        );
#endif
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)
    instance->stacks_tree = fcs_libavl2_stacks_tree_create(
        fcs_stack_compare_for_comparison_with_context, NULL, NULL);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    instance->stacks_tree =
        rbinit(fcs_stack_compare_for_comparison_with_context, NULL);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
    instance->stacks_tree = g_tree_new(fc_solve_stack_compare_for_comparison);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
    instance->stacks_hash =
        g_hash_table_new(fc_solve_glib_hash_stack_hash_function,
            fc_solve_glib_hash_stack_compare);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)
    instance->stacks_hash = fc_solve_columns_google_hash_new();
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)
    instance->stacks_judy_array = NULL;
#else
#error FCS_STACK_STORAGE is not set to a good value.
#endif
#endif

/***********************************************/

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    /* Not working - ignore */
    db_open(
        NULL, DB_BTREE, O_CREAT | O_RDWR, 0777, NULL, NULL, &(instance->db));
#endif

    {
        fcs_kv_state_t no_use;
        fcs_kv_state_t pass_copy = FCS_STATE_keyval_pair_to_kv(state_copy_ptr);

        fc_solve_check_and_add_state(
#ifdef FCS_SINGLE_HARD_THREAD
            instance,
#else
            instance->hard_threads,
#endif
            &pass_copy, &no_use);
    }

#ifndef FCS_SINGLE_HARD_THREAD
    instance->current_hard_thread = instance->hard_threads;
#endif
    {
        HT_LOOP_START()
        {
            if (HT_FIELD(hard_thread, prelude) != NULL)
            {
                HT_FIELD(hard_thread, prelude_idx) = 0;
                set_next_prelude_item(hard_thread,
                    HT_FIELD(hard_thread, prelude),
                    &(HT_FIELD(hard_thread, st_idx)));
            }
            else
            {
                HT_FIELD(hard_thread, prelude_num_items) = 0;
            }
        }
    }

#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
    STRUCT_SET_FLAG_TO(instance, FCS_RUNTIME_TO_REPARENT_STATES_REAL,
        STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_TO_REPARENT_STATES_PROTO));
#endif

    return;
}

static GCC_INLINE void free_instance_hard_thread_callback(
    fc_solve_hard_thread_t *const hard_thread)
{
    if (likely(HT_FIELD(hard_thread, prelude_as_string)))
    {
        free(HT_FIELD(hard_thread, prelude_as_string));
    }
    if (likely(HT_FIELD(hard_thread, prelude)))
    {
        free(HT_FIELD(hard_thread, prelude));
    }
    fcs_move_stack_static_destroy(HT_FIELD(hard_thread, reusable_move_stack));

    free(HT_FIELD(hard_thread, soft_threads));

    fc_solve_compact_allocator_finish(&(HT_FIELD(hard_thread, allocator)));
}

static GCC_INLINE void fc_solve_release_tests_list(
    fc_solve_soft_thread_t *const soft_thread)
{
    /* Free the BeFS data. */
    free(BEFS_M_VAR(soft_thread, moves_list));
    BEFS_M_VAR(soft_thread, moves_list) = NULL;

    /* Free the DFS data. */
    fcs_moves_by_depth_array *const arr =
        &(DFS_VAR(soft_thread, moves_by_depth));
    const_SLOT(num_units, arr);
    for (size_t unit_idx = 0; unit_idx < num_units; unit_idx++)
    {
        if (arr->by_depth_units[unit_idx].move_funcs.groups)
        {
            fcs_moves_group *const groups =
                arr->by_depth_units[unit_idx].move_funcs.groups;
            const_AUTO(num, arr->by_depth_units[unit_idx].move_funcs.num);

            for (typeof(arr->by_depth_units[unit_idx].move_funcs.num) i = 0;
                 i < num; i++)
            {
                free(groups[i].move_funcs);
            }
            free(groups);
        }
    }
    free(arr->by_depth_units);
    arr->by_depth_units = NULL;
}

#ifdef FCS_WITH_MOVES
static inline void instance_free_solution_moves(fcs_instance *const instance)
{
    if (instance->solution_moves.moves)
    {
        fcs_move_stack_static_destroy(instance->solution_moves);
        instance->solution_moves.moves = NULL;
    }
}
#endif

static inline void st_free_pq(fcs_soft_thread *const soft_thread)
{
    fc_solve_PQueueFree(&(BEFS_VAR(soft_thread, pqueue)));
}

static inline void fc_solve_free_instance_soft_thread_callback(
    fcs_soft_thread *const soft_thread)
{
    st_free_pq(soft_thread);
    fcs_free_moves_list(soft_thread);
    fc_solve_free_soft_thread_by_depth_move_array(soft_thread);
#ifndef FCS_DISABLE_PATSOLVE
    const_SLOT(pats_scan, soft_thread);

    if (pats_scan)
    {
        fc_solve_pats__recycle_soft_thread(pats_scan);
        fc_solve_pats__destroy_soft_thread(pats_scan);
        free(pats_scan);
        soft_thread->pats_scan = NULL;
    }
#endif
}

#ifdef __cplusplus
}
#endif
