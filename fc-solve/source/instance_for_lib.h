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
 * instance_for_lib.h - header file that contains declarations
 * and definitions that depend on instance.h functions and are only used only
 * by lib.c.
 *
 * This is done to speed compilation.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "instance.h"
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
        .stats = initial_stats,
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
            HT_FIELD(hard_thread, soft_threads)[0].num_checked_states_step,
            &(HT_FIELD(hard_thread, st_idx)));
    }

    {
        size_t total_tests = 0;
        fc_solve_foreach_soft_thread(
            instance, FOREACH_SOFT_THREAD_ACCUM_TESTS_ORDER, &total_tests);
        fc_solve_foreach_soft_thread(instance,
            FOREACH_SOFT_THREAD_DETERMINE_SCAN_COMPLETENESS, &total_tests);
#ifdef FCS_WITH_MOVES
        if (!STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET))
        {
            /*
             *
             * What this code does is convert the bit map of total_tests
             * to a valid tests order.
             *
             * */
            size_t num_tests = 0;
            size_t *tests = SMALLOC(tests, sizeof(total_tests) * 8);

            for (size_t bit_idx = 0; total_tests != 0;
                 bit_idx++, total_tests >>= 1)
            {
                if ((total_tests & 0x1) != 0)
                {
                    tests[num_tests++] = bit_idx;
                }
            }
            tests =
                SREALLOC(tests, ((num_tests & (~(TESTS_ORDER_GROW_BY - 1))) +
                                    TESTS_ORDER_GROW_BY));
            instance->opt_tests_order = (typeof(instance->opt_tests_order)){
                .num_groups = 1,
                .groups = SMALLOC(
                    instance->opt_tests_order.groups, TESTS_ORDER_GROW_BY),
            };
            instance->opt_tests_order.groups[0] =
                (typeof(instance->opt_tests_order.groups[0])){
                    .order_group_tests = tests,
                    .num = num_tests,
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
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)
    instance->hash = fc_solve_states_google_hash_new();
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
/* Do nothing because it is allocated elsewhere. */
#else
#error not defined
#endif

/****************************************************/

#ifdef INDIRECT_STACK_STATES
/* Initialize the data structure that will manage the stack
   collection */
#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH
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
    free(BEFS_M_VAR(soft_thread, tests_list));
    BEFS_M_VAR(soft_thread, tests_list) = NULL;

    /* Free the DFS data. */
    fcs_tests_by_depth_array_t *const arr =
        &(DFS_VAR(soft_thread, tests_by_depth_array));
    for (int unit_idx = 0; unit_idx < arr->num_units; unit_idx++)
    {
        if (arr->by_depth_units[unit_idx].tests.lists)
        {
            fcs_tests_list_t *const lists =
                arr->by_depth_units[unit_idx].tests.lists;
            const int num_lists = arr->by_depth_units[unit_idx].tests.num_lists;

            for (int i = 0; i < num_lists; i++)
            {
                free(lists[i].tests);
            }
            free(lists);
        }
    }
    free(arr->by_depth_units);
    arr->by_depth_units = NULL;
}

static GCC_INLINE void fc_solve_free_instance_soft_thread_callback(
    fc_solve_soft_thread_t *const soft_thread)
{
    fc_solve_st_free_pq(soft_thread);

    fc_solve_release_tests_list(soft_thread);

    fc_solve_free_soft_thread_by_depth_test_array(soft_thread);

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

#ifdef FCS_WITH_MOVES
static GCC_INLINE void instance_free_solution_moves(
    fc_solve_instance_t *const instance)
{
    if (instance->solution_moves.moves)
    {
        fcs_move_stack_static_destroy(instance->solution_moves);
        instance->solution_moves.moves = NULL;
    }
}
#endif

static GCC_INLINE void fc_solve_free_instance(
    fc_solve_instance_t *const instance)
{
    fc_solve_foreach_soft_thread(
        instance, FOREACH_SOFT_THREAD_FREE_INSTANCE, NULL);

    HT_LOOP_START() { free_instance_hard_thread_callback(hard_thread); }

#ifdef FCS_SINGLE_HARD_THREAD
#ifdef FCS_WITH_MOVES
    if (instance->is_optimization_st)
    {
        fc_solve_free_instance_soft_thread_callback(
            &(instance->optimization_soft_thread));
        instance->is_optimization_st = FALSE;
    }
#endif
#else
    free(instance->hard_threads);

#ifdef FCS_WITH_MOVES
    if (instance->optimization_thread)
    {
        free_instance_hard_thread_callback(instance->optimization_thread);
        free(instance->optimization_thread);
    }
#endif
#endif
    fc_solve_free_tests_order(&(instance->instance_tests_order));
#ifdef FCS_WITH_MOVES
    if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET))
    {
        fc_solve_free_tests_order(&(instance->opt_tests_order));
    }
    instance_free_solution_moves(instance);
#endif
}

static GCC_INLINE void fc_solve_instance__recycle_hard_thread(
    fc_solve_hard_thread_t *const hard_thread)
{
    fc_solve_reset_hard_thread(hard_thread);
    fc_solve_compact_allocator_recycle(&(HT_FIELD(hard_thread, allocator)));

    ST_LOOP_START()
    {
        fc_solve_st_free_pq(soft_thread);

        fc_solve_reset_soft_thread(soft_thread);

#ifndef FCS_DISABLE_PATSOLVE
        const_SLOT(pats_scan, soft_thread);

        if (pats_scan)
        {
            fc_solve_pats__recycle_soft_thread(pats_scan);
        }
#endif
    }

    return;
}

static GCC_INLINE fc_solve_soft_thread_t *
fc_solve_instance_get_first_soft_thread(fc_solve_instance_t *const instance)
{
    return &(INST_HT0(instance).soft_threads[0]);
}

extern void fc_solve_finish_instance(fc_solve_instance_t *const instance);

static GCC_INLINE void fc_solve_recycle_instance(
    fc_solve_instance_t *const instance)
{
    fc_solve_finish_instance(instance);
#ifdef FCS_WITH_MOVES
    instance_free_solution_moves(instance);
#endif

    instance->stats = initial_stats;

    instance->num_hard_threads_finished = 0;

#ifdef FCS_SINGLE_HARD_THREAD
    fc_solve_instance__recycle_hard_thread(instance);

#ifdef FCS_WITH_MOVES
    if (instance->is_optimization_st)
    {
        fc_solve_reset_soft_thread(&(instance->optimization_soft_thread));
    }
#endif
#else
    for (int ht_idx = 0; ht_idx < instance->num_hard_threads; ht_idx++)
    {
        fc_solve_instance__recycle_hard_thread(
            &(instance->hard_threads[ht_idx]));
    }

    if (instance->optimization_thread)
    {
        fc_solve_instance__recycle_hard_thread(instance->optimization_thread);
    }
#endif
#ifdef FCS_WITH_MOVES
    STRUCT_CLEAR_FLAG(instance, FCS_RUNTIME_IN_OPTIMIZATION_THREAD);
#endif
}

#ifdef FCS_WITH_MOVES
extern void fc_solve_trace_solution(fc_solve_instance_t *const instance);

static GCC_INLINE void fc_solve__setup_optimization_thread__helper(
    fc_solve_instance_t *const instance,
    fc_solve_soft_thread_t *const soft_thread)
{
    if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET))
    {
        if (soft_thread->by_depth_tests_order.by_depth_tests != NULL)
        {
            fc_solve_free_soft_thread_by_depth_test_array(soft_thread);
        }

        soft_thread->by_depth_tests_order =
            (typeof(soft_thread->by_depth_tests_order)){
                .num = 1,
                .by_depth_tests =
                    SMALLOC1(soft_thread->by_depth_tests_order.by_depth_tests),
            };
        soft_thread->by_depth_tests_order.by_depth_tests[0] =
            (typeof(soft_thread->by_depth_tests_order.by_depth_tests[0])){
                .max_depth = SSIZE_MAX,
                .tests_order = tests_order_dup(&(instance->opt_tests_order)),
            };
    }

    soft_thread->super_method_type = FCS_SUPER_METHOD_BEFS_BRFS;
    soft_thread->is_optimize_scan = TRUE;
    soft_thread->is_befs = FALSE;
    STRUCT_TURN_ON_FLAG(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN);
    fc_solve_soft_thread_init_befs_or_bfs(soft_thread);
    STRUCT_TURN_ON_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED);
    STRUCT_TURN_ON_FLAG(instance, FCS_RUNTIME_IN_OPTIMIZATION_THREAD);
}

/*
    This function optimizes the solution path using a BFS scan on the
    states in the solution path.
*/
#ifdef FCS_SINGLE_HARD_THREAD
static GCC_INLINE int fc_solve_optimize_solution(
    fc_solve_instance_t *const instance)
{
    fc_solve_soft_thread_t *const optimization_soft_thread =
        &(instance->optimization_soft_thread);

    if (!instance->solution_moves.moves)
    {
        fc_solve_trace_solution(instance);
    }

#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
    STRUCT_TURN_ON_FLAG(instance, FCS_RUNTIME_TO_REPARENT_STATES_REAL);
#endif

    if (!instance->is_optimization_st)
    {
        fc_solve_init_soft_thread(instance, optimization_soft_thread);
        /* Copy enable_pruning from the thread that reached the solution,
         * because otherwise -opt in conjunction with -sp r:tf will fail.
         * */
        optimization_soft_thread->enable_pruning =
            instance->hard_thread.soft_threads[instance->hard_thread.st_idx]
                .enable_pruning;
        instance->is_optimization_st = TRUE;
    }

    fc_solve__setup_optimization_thread__helper(
        instance, optimization_soft_thread);
    /* Instruct the optimization hard thread to run indefinitely AFA it
     * is concerned */
    instance->hard_thread.ht__max_num_checked_states = FCS_INT_LIMIT_MAX;
    return fc_solve_befs_or_bfs_do_solve(optimization_soft_thread);
}
#undef soft_thread
#else
static GCC_INLINE int fc_solve_optimize_solution(
    fc_solve_instance_t *const instance)
{
    fc_solve_soft_thread_t *soft_thread;
    fc_solve_hard_thread_t *optimization_thread;

    if (!instance->solution_moves.moves)
    {
        fc_solve_trace_solution(instance);
    }

    STRUCT_TURN_ON_FLAG(instance, FCS_RUNTIME_TO_REPARENT_STATES_REAL);

    if (!instance->optimization_thread)
    {
        instance->optimization_thread = optimization_thread =
            SMALLOC1(optimization_thread);

        fc_solve_instance__init_hard_thread(instance, optimization_thread);

        fc_solve_hard_thread_t *const old_hard_thread =
            instance->current_hard_thread;

        soft_thread = optimization_thread->soft_threads;

        /* Copy enable_pruning from the thread that reached the solution,
         * because otherwise -opt in conjunction with -sp r:tf will fail.
         * */
        soft_thread->enable_pruning =
            old_hard_thread->soft_threads[old_hard_thread->st_idx]
                .enable_pruning;
    }
    else
    {
        optimization_thread = instance->optimization_thread;
        soft_thread = optimization_thread->soft_threads;
    }

    fc_solve__setup_optimization_thread__helper(instance, soft_thread);
    /* Instruct the optimization hard thread to run indefinitely AFA it
     * is concerned */
    optimization_thread->ht__max_num_checked_states = FCS_INT_LIMIT_MAX;
    return fc_solve_befs_or_bfs_do_solve(soft_thread);
}
#endif
#endif

static GCC_INLINE int fc_solve__soft_thread__do_solve(
    fc_solve_soft_thread_t *const soft_thread)
{
    switch (soft_thread->super_method_type)
    {
    case FCS_SUPER_METHOD_DFS:
        return fc_solve_soft_dfs_do_solve(soft_thread);

    case FCS_SUPER_METHOD_BEFS_BRFS:
        return fc_solve_befs_or_bfs_do_solve(soft_thread);

#ifndef FCS_DISABLE_PATSOLVE
    case FCS_SUPER_METHOD_PATSOLVE:
        return fc_solve_patsolve_do_solve(soft_thread);
#endif

    default:
        return FCS_STATE_IS_NOT_SOLVEABLE;
    }
}

static GCC_INLINE void fc_solve_soft_thread_init_soft_dfs(
    fc_solve_soft_thread_t *const soft_thread)
{
    fc_solve_soft_thread_update_initial_cards_val(soft_thread);
    fc_solve_instance_t *const instance = HT_INSTANCE(soft_thread->hard_thread);

    /*
        Allocate some space for the states at depth 0.
    */
    DFS_VAR(soft_thread, depth) = 0;

    fc_solve_increase_dfs_max_depth(soft_thread);

    DFS_VAR(soft_thread, soft_dfs_info)
    [0].state = FCS_STATE_keyval_pair_to_collectible(instance->state_copy_ptr);

    fc_solve_rand_init(
        &(DFS_VAR(soft_thread, rand_gen)), DFS_VAR(soft_thread, rand_seed));

    if (!DFS_VAR(soft_thread, tests_by_depth_array).by_depth_units)
    {
        const_SLOT(master_to_randomize, soft_thread);
        fcs_tests_by_depth_array_t *const arr_ptr =
            &(DFS_VAR(soft_thread, tests_by_depth_array));
        arr_ptr->by_depth_units = SMALLOC(arr_ptr->by_depth_units,
            (arr_ptr->num_units = soft_thread->by_depth_tests_order.num));

        const fcs_by_depth_tests_order_t *const by_depth_tests_order =
            soft_thread->by_depth_tests_order.by_depth_tests;

        var_AUTO(unit, arr_ptr->by_depth_units);
        const_AUTO(depth_num, soft_thread->by_depth_tests_order.num);
        for (size_t depth_idx = 0; depth_idx < depth_num; depth_idx++, unit++)
        {
            unit->max_depth = by_depth_tests_order[depth_idx].max_depth;

            fcs_tests_order_group_t *const tests_order_groups =
                by_depth_tests_order[depth_idx].tests_order.groups;

            const_AUTO(tests_order_num,
                by_depth_tests_order[depth_idx].tests_order.num_groups);

            const_AUTO(tests_list_of_lists, &(unit->tests));

            *tests_list_of_lists = (typeof(*tests_list_of_lists)){
                .num_lists = 0,
                .lists = SMALLOC(tests_list_of_lists->lists, tests_order_num),
            };

            for (size_t group_idx = 0; group_idx < tests_order_num; group_idx++)
            {
                size_t num = 0;
                fc_solve_solve_for_state_move_func_t *tests_list = NULL;
                add_to_move_funcs_list(&tests_list, &num,
                    tests_order_groups[group_idx].order_group_tests,
                    tests_order_groups[group_idx].num);
                /* TODO : convert to C99 struct initializers. */
                const_AUTO(tests_list_struct_ptr,
                    &(tests_list_of_lists
                                   ->lists[tests_list_of_lists->num_lists++]));

                const fcs_tests_group_type_t shuffling_type =
                    (master_to_randomize
                            ? tests_order_groups[group_idx].shuffling_type
                            : FCS_NO_SHUFFLING);
                *tests_list_struct_ptr = (typeof(*tests_list_struct_ptr)){
                    .tests = tests_list,
                    .num_tests = num,
                    .shuffling_type = shuffling_type,
                };

                if (shuffling_type == FCS_WEIGHTING)
                {
                    tests_list_struct_ptr->weighting =
                        tests_order_groups[group_idx].weighting;

                    fc_solve_initialize_befs_rater(
                        soft_thread, &(tests_list_struct_ptr->weighting));
                }
            }

            tests_list_of_lists->lists = SREALLOC(
                tests_list_of_lists->lists, tests_list_of_lists->num_lists);
        }
    }

    return;
}

/*
 * Switch to the next soft thread in the hard thread,
 * since we are going to call continue and this is
 * a while loop
 * */
static GCC_INLINE void switch_to_next_soft_thread(
    fc_solve_hard_thread_t *const hard_thread, const int num_soft_threads,
    const fc_solve_soft_thread_t *const soft_threads,
    const fcs_prelude_item_t *const prelude,
    const fcs_int_limit_t prelude_num_items, int *const st_idx_ptr)
{
    if (HT_FIELD(hard_thread, prelude_idx) < prelude_num_items)
    {
        set_next_prelude_item(hard_thread, prelude, st_idx_ptr);
    }
    else
    {
        const int next_st_idx = ((1 + (*st_idx_ptr)) % num_soft_threads);
        set_next_soft_thread(hard_thread, next_st_idx,
            soft_threads[next_st_idx].num_checked_states_step, st_idx_ptr);
    }
}

static GCC_INLINE fcs_bool_t instance__check_exceeded_stats(
    const fc_solve_instance_t *const instance)
{
    return ((instance->stats.num_checked_states >=
                instance->effective_max_num_checked_states) ||
            (instance->stats.num_states_in_collection >=
                instance->effective_max_num_states_in_collection));
}

static GCC_INLINE int run_hard_thread(fc_solve_hard_thread_t *const hard_thread)
{
    const fcs_int_limit_t prelude_num_items =
        HT_FIELD(hard_thread, prelude_num_items);
#ifdef FCS_SINGLE_HARD_THREAD
#define instance hard_thread
#else
    fc_solve_instance_t *const instance = hard_thread->instance;
#endif
    int *const st_idx_ptr = &(HT_FIELD(hard_thread, st_idx));
    /*
     * Again, making sure that not all of the soft_threads in this
     * hard thread are finished.
     * */

    int ret = FCS_STATE_SUSPEND_PROCESS;
    const int num_soft_threads = HT_FIELD(hard_thread, num_soft_threads);
    const fcs_prelude_item_t *const prelude = HT_FIELD(hard_thread, prelude);
    fc_solve_soft_thread_t *const soft_threads =
        HT_FIELD(hard_thread, soft_threads);

    while (HT_FIELD(hard_thread, num_soft_threads_finished) < num_soft_threads)
    {
        fc_solve_soft_thread_t *const soft_thread =
            &(soft_threads[*st_idx_ptr]);
        /*
         * Move to the next thread if it's already finished
         * */
        if (STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_IS_FINISHED))
        {
            switch_to_next_soft_thread(hard_thread, num_soft_threads,
                soft_threads, prelude, prelude_num_items, st_idx_ptr);

            continue;
        }

        /*
         * Call the resume or solving function that is specific
         * to each scan
         *
         * This switch-like construct calls for declaring a class
         * that will abstract a scan. But it's not critical since
         * I don't support user-defined scans.
         * */
        if (!STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED))
        {
            fc_solve_soft_thread_init_soft_dfs(soft_thread);
            fc_solve_soft_thread_init_befs_or_bfs(soft_thread);

#ifndef FCS_DISABLE_PATSOLVE
            const_SLOT(pats_scan, soft_thread);
            if (pats_scan)
            {
                fc_solve_pats__init_buckets(pats_scan);
                fc_solve_pats__init_clusters(pats_scan);

                pats_scan->current_pos.s =
                    instance->initial_non_canonized_state->s;
#ifdef INDIRECT_STACK_STATES

                memset(pats_scan->current_pos.indirect_stacks_buffer, '\0',
                    sizeof(pats_scan->current_pos.indirect_stacks_buffer));

#ifndef HARD_CODED_NUM_STACKS
                const int stacks_num = INSTANCE_STACKS_NUM;
#endif

                for (int i = 0; i < STACKS_NUM__VAL; i++)
                {
                    fcs_cards_column_t src_col =
                        fcs_state_get_col(pats_scan->current_pos.s, i);
                    char *dest = &(
                        pats_scan->current_pos.indirect_stacks_buffer[i << 7]);
                    memmove(dest, src_col, fcs_col_len(src_col) + 1);
                    fcs_state_get_col(pats_scan->current_pos.s, i) = dest;
                }
#endif
                fc_solve_pats__initialize_solving_process(pats_scan);
            }
#endif
            STRUCT_TURN_ON_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED);
        }
        ret = fc_solve__soft_thread__do_solve(soft_thread);
        /*
         * I use <= instead of == because it is possible that
         * there will be a few more iterations than what this
         * thread was allocated, due to the fact that
         * check_and_add_state is only called by the test
         * functions.
         *
         * It's a kludge, but it works.
         * */
        if (NUM_CHECKED_STATES >=
            HT_FIELD(hard_thread, ht__max_num_checked_states))
        {
            switch_to_next_soft_thread(hard_thread, num_soft_threads,
                soft_threads, prelude, prelude_num_items, st_idx_ptr);
        }

        /*
         * It this thread indicated that the scan was finished,
         * disable the thread or even stop searching altogether.
         * */
        if (ret == FCS_STATE_IS_NOT_SOLVEABLE)
        {
            STRUCT_TURN_ON_FLAG(soft_thread, FCS_SOFT_THREAD_IS_FINISHED);
            if (++(HT_FIELD(hard_thread, num_soft_threads_finished)) ==
                num_soft_threads)
            {
                instance->num_hard_threads_finished++;
            }
/*
 * Check if this thread is a complete scan and if so,
 * terminate the search. Note that if the scans synergy is set,
 * then we may still need to continue running the other threads
 * which may have blocked some positions / states in the graph.
 * */
#ifndef FCS_HARD_CODE_SCANS_SYNERGY_AS_TRUE
            if (STRUCT_QUERY_FLAG(
                    soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN) &&
                (!STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY)))
            {
                return FCS_STATE_IS_NOT_SOLVEABLE;
            }
            else
#endif
            {
                /*
                 * Else, make sure ret is something more sensible
                 * */
                ret = FCS_STATE_SUSPEND_PROCESS;
            }
        }

        const fcs_bool_t was_solved = (ret == FCS_STATE_WAS_SOLVED);
        if (was_solved)
        {
            instance->solving_soft_thread = soft_thread;
        }

        if (was_solved || ((ret == FCS_STATE_SUSPEND_PROCESS) &&
                              /* There's a limit to the scan only
                               * if max_num_checked_states is greater than 0 */
                              instance__check_exceeded_stats(instance)))
        {
            return ret;
        }
    }

    return ret;
}
#ifdef FCS_SINGLE_HARD_THREAD
#undef instance
#endif

/* Resume a solution process that was stopped in the middle */
static GCC_INLINE int fc_solve_resume_instance(
    fc_solve_instance_t *const instance)
{
    int ret = FCS_STATE_SUSPEND_PROCESS;

/*
 * If the optimization thread is defined, it means we are in the
 * optimization phase of the total scan. In that case, just call
 * its scanning function.
 *
 * Else, proceed with the normal total scan.
 * */
#ifdef FCS_WITH_MOVES
    if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_IN_OPTIMIZATION_THREAD))
    {
        ret = fc_solve_befs_or_bfs_do_solve(
#ifdef FCS_SINGLE_HARD_THREAD
            &(instance->optimization_soft_thread)
#else
            &(instance->optimization_thread->soft_threads[0])
#endif
                );
    }
    else
#endif
    {
#ifdef FCS_SINGLE_HARD_THREAD
#define hard_thread instance
#define NUM_HARD_THREADS() 1
#else
        fc_solve_hard_thread_t *const end_of_hard_threads =
            instance->hard_threads + instance->num_hard_threads;

        fc_solve_hard_thread_t *hard_thread = instance->current_hard_thread;
#define NUM_HARD_THREADS() (instance->num_hard_threads)
#endif
        /*
         * instance->num_hard_threads_finished signals to us that
         * all the incomplete soft threads terminated. It is necessary
         * in case the scan only contains incomplete threads.
         *
         * I.e: 01235 and 01246, where no thread contains all tests.
         * */
        while (instance->num_hard_threads_finished < NUM_HARD_THREADS())
        {
/*
 * A loop on the hard threads.
 * Note that we do not initialize instance->ht_idx because:
 * 1. It is initialized before the first call to this function.
 * 2. It is reset to zero below.
 * */
#ifndef FCS_SINGLE_HARD_THREAD
            for (; hard_thread < end_of_hard_threads; hard_thread++)
#endif
            {
                ret = run_hard_thread(hard_thread);
                if ((ret == FCS_STATE_IS_NOT_SOLVEABLE) ||
                    (ret == FCS_STATE_WAS_SOLVED) ||
                    ((ret == FCS_STATE_SUSPEND_PROCESS) &&
                        /* There's a limit to the scan only
                         * if max_num_checked_states is greater than 0 */
                        instance__check_exceeded_stats(instance)))
                {
                    goto end_of_hard_threads_loop;
                }
            }
#ifndef FCS_SINGLE_HARD_THREAD
            hard_thread = instance->hard_threads;
#endif
        }

    end_of_hard_threads_loop:
#ifndef FCS_SINGLE_HARD_THREAD
        instance->current_hard_thread = hard_thread;
#endif

        /*
         * If all the incomplete scans finished, then terminate.
         * */
        if (instance->num_hard_threads_finished == NUM_HARD_THREADS())
        {
            ret = FCS_STATE_IS_NOT_SOLVEABLE;
        }
    }

#ifdef FCS_WITH_MOVES
    if (ret == FCS_STATE_WAS_SOLVED)
    {
        if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_OPTIMIZE_SOLUTION_PATH))
        {
            /* Call optimize_solution only once. Make sure that if
             * it has already run - we retain the old ret. */
            if (!STRUCT_QUERY_FLAG(
                    instance, FCS_RUNTIME_IN_OPTIMIZATION_THREAD))
            {
                ret = fc_solve_optimize_solution(instance);
            }
        }
    }
#endif

    return ret;
}
#ifdef FCS_SINGLE_HARD_THREAD
#undef hard_thread
#endif

#ifdef __cplusplus
}
#endif
