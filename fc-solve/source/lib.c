/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// lib.c - library interface functions of Freecell Solver.
#include "instance_for_lib.h"
#include "preset.h"
#include "freecell-solver/fcs_user.h"
#include "move_funcs_order.h"
#include "fcs_user_internal.h"
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
#include "fc_pro_iface_pos.h"
#endif

#ifdef DEBUG
static void verify_state_sanity(const fcs_state *const ptr_state)
{
#ifndef NDEBUG
    for (int i = 0; i < 8; ++i)
    {
        const int l = fcs_state_col_len(*(ptr_state), i);
        assert((l >= 0) && (l <= 7 + 12));
    }
#endif
}
#endif

#ifndef FCS_SINGLE_HARD_THREAD
static inline fcs_soft_thread *new_hard_thread(fcs_instance *const instance)
{
    fcs_hard_thread *ret;
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

    ++instance->num_hard_threads;

    return &(ret->soft_threads[0]);
}
#endif

#ifndef FCS_FREECELL_ONLY
static inline void apply_preset_by_name(
    fcs_instance *const instance, const char *const name)
{
    const fcs_preset *preset_ptr;
    const_AUTO(ret, fc_solve_get_preset_by_name(name, &preset_ptr));
    if (ret == FCS_PRESET_CODE_OK)
    {
        fc_solve_apply_preset_by_ptr(instance, preset_ptr);
    }
}
#endif

/*
    This function allocates a Freecell Solver instance struct and set the
    default values in it. After the call to this function, the program can
    set parameters in it which are different from the default.

    Afterwards fc_solve_init_instance() should be called in order
    to really prepare it for solving.
  */
static inline void alloc_instance(
    fcs_instance *const instance, meta_allocator *const meta_alloc)
{
    *(instance) = (fcs_instance){
        .meta_alloc = meta_alloc,
        .i__num_checked_states = 0,
#ifndef FCS_WITHOUT_MAX_NUM_STATES
        .effective_max_num_checked_states = FCS_INT_LIMIT_MAX,
#endif
#ifndef FCS_DISABLE_NUM_STORED_STATES
        .num_states_in_collection = 0,
#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
        .active_num_states_in_collection = 0,
        .effective_trim_states_in_collection_from = FCS_INT_LIMIT_MAX,
#endif
        .effective_max_num_states_in_collection = FCS_INT_LIMIT_MAX,
#endif
        .instance_moves_order =
            {
                .num = 0,
                .groups = NULL,
            },
        .list_of_vacant_states = NULL,
#ifdef FCS_WITH_MOVES
        .opt_moves =
            {
                .num = 0,
                .groups = NULL,
            },
        .solution_moves = (fcs_move_stack){.moves = NULL, .num_moves = 0},
        .FCS_RUNTIME_OPTIMIZE_SOLUTION_PATH = FALSE,
        .FCS_RUNTIME_IN_OPTIMIZATION_THREAD = FALSE,
        .FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET = FALSE,
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
#ifdef FCS_RCS_STATES
        .rcs_states_cache.max_num_elements_in_cache = 10000,
#endif
#ifndef FCS_DISABLE_SIMPLE_SIMON
        .is_simple_simon = FALSE,
#endif
    };

#ifndef FCS_FREECELL_ONLY
    apply_preset_by_name(instance, "freecell");
#else
    FCS__DECL_ERR_BUF(no_use);
    fc_solve_apply_moves_order(&(instance->instance_moves_order),
        "[01][23456789]" FCS__PASS_ERR_STR(no_use));
#endif

/****************************************/
#ifdef FCS_SINGLE_HARD_THREAD
    fc_solve_instance__init_hard_thread(instance);
#else
    new_hard_thread(instance);
#endif
}

#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
static inline void compile_prelude(fcs_hard_thread *const hard_thread)
{
    bool last_one = FALSE;
    size_t num_items = 0;
    fc_solve_prelude_item *prelude = NULL;

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
        p_scan_copy[p - p_scan] = LAST(p_scan_copy) = '\0';
        p++;

        ST_LOOP_START()
        {
            if (!strcmp(soft_thread->name, p_scan_copy))
            {
                break;
            }
        }
        if (soft_thread == end_soft_thread)
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
            .scan_idx = soft_thread - ht_soft_threads, .quota = p_quota};
    }

    HT_FIELD(hard_thread, prelude) = SREALLOC(prelude, num_items);
    HT_FIELD(hard_thread, prelude_num_items) = num_items;
}
#endif

static inline void set_next_soft_thread(fcs_hard_thread *const hard_thread,
    const int_fast32_t scan_idx, const fcs_int_limit_t quota,
    int_fast32_t *const st_idx_ptr)
{
    (*st_idx_ptr) = scan_idx;
    /*
     * Calculate a soft thread-wise limit for this hard
     * thread to run.
     * */
    HT_FIELD(hard_thread, ht__max_num_checked_states) =
        NUM_CHECKED_STATES + quota;
}

static inline void init_instance(fcs_instance *const instance)
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
#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
        if (HT_FIELD(hard_thread, prelude_as_string) &&
            !HT_FIELD(hard_thread, prelude))
        {
            compile_prelude(hard_thread);
        }
#endif
        set_next_soft_thread(hard_thread, 0,
            HT_FIELD(hard_thread, soft_threads)[0].checked_states_step,
            &(HT_FIELD(hard_thread, st_idx)));
    }

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
        fcs_move_func *move_funcs =
            SMALLOC(move_funcs, sizeof(total_move_funcs_bitmask) * 8);

        for (size_t bit_idx = 0; total_move_funcs_bitmask != 0;
             bit_idx++, total_move_funcs_bitmask >>= 1)
        {
            if ((total_move_funcs_bitmask & 0x1) != 0)
            {
                move_funcs[num_move_funcs++].idx = bit_idx;
            }
        }
        move_funcs = SREALLOC(move_funcs,
            ((num_move_funcs & (~(MOVES_GROW_BY - 1))) + MOVES_GROW_BY));
        instance->opt_moves = (typeof(instance->opt_moves)){
            .num = 1,
            .groups = SMALLOC(instance->opt_moves.groups, MOVES_GROW_BY),
        };
        instance->opt_moves.groups[0] = (typeof(instance->opt_moves.groups[0])){
            .move_funcs = move_funcs,
            .num = num_move_funcs,
            .shuffling_type = FCS_NO_SHUFFLING,
        };
        STRUCT_TURN_ON_FLAG(instance, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET);
    }
#endif

#ifdef FCS_RCS_STATES
    fcs_lru_cache *const cache = &(instance->rcs_states_cache);
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
#endif
}

// These are all stack comparison functions to be used for the stacks
// cache when using INDIRECT_STACK_STATES
#ifdef INDIRECT_STACK_STATES
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
static guint fc_solve_glib_hash_stack_hash_function(gconstpointer key)
{
    guint hash_value_int = 0;
    /* This hash function was ripped from the Perl source code.
     * (It is not derived work however). */
    const char *s_ptr = (const char *)key;
    const char *const s_end = s_ptr + fcs_col_len((const fcs_card *)key) + 1;
    while (s_ptr < s_end)
    {
        hash_value_int += (hash_value_int << 5) + *(s_ptr++);
    }
    hash_value_int += (hash_value_int >> 5);

    return hash_value_int;
}

static gint fc_solve_glib_hash_stack_compare(gconstpointer a, gconstpointer b)
{
    return !(fc_solve_stack_compare_for_comparison(a, b));
}
#endif

#if ((FCS_STACK_STORAGE != FCS_STACK_STORAGE_GLIB_TREE) &&                     \
     (FCS_STACK_STORAGE != FCS_STACK_STORAGE_GLIB_HASH) &&                     \
     (FCS_STACK_STORAGE != FCS_STACK_STORAGE_JUDY))
#if (((FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH) &&                \
         (defined(FCS_WITH_CONTEXT_VARIABLE)) &&                               \
         (!defined(FCS_INLINED_HASH_COMPARISON))) ||                           \
     (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE) ||                  \
     (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE))

static int cmp_stacks_w_context(const void *const v_s1, const void *const v_s2,
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

static inline fcs_state *rcs_states_get_state(
    fcs_instance *const instance, const fcs_collectible_state *const state)
{
    return ((state == instance->tree_new_state)
                ? instance->tree_new_state_key
                : fc_solve_lookup_state_key_from_val(instance, state));
}

static int rcs_cmp_states(
    const void *const void_a, const void *const void_b, void *const context)
{
    fcs_instance *const instance = (fcs_instance *)context;

    return fc_solve_state_compare(
        rcs_states_get_state(instance, (const fcs_collectible_state *)void_a),
        rcs_states_get_state(instance, (const fcs_collectible_state *)void_b));
}

#endif

#endif

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE) ||                  \
     (FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE) ||                      \
     (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE))

#ifdef FCS_RCS_STATES
#define STATE_STORAGE_TREE_COMPARE() rcs_cmp_states
#define STATE_STORAGE_TREE_CONTEXT() instance
#else
#define STATE_STORAGE_TREE_COMPARE() fc_solve_state_compare_with_context
#define STATE_STORAGE_TREE_CONTEXT() NULL
#endif

#endif

static inline void set_next_prelude_item(fcs_hard_thread *const hard_thread,
    const fc_solve_prelude_item *const prelude, int_fast32_t *const st_idx_ptr)
{
    const fc_solve_prelude_item next_item =
        prelude[HT_FIELD(hard_thread, prelude_idx)++];
    set_next_soft_thread(
        hard_thread, next_item.scan_idx, next_item.quota, st_idx_ptr);
}

static inline void update_initial_cards_val(fcs_instance *const instance)
{
#ifdef FCS_FREECELL_ONLY
#define SEQS_BUILT_BY
#else
    const int sequences_are_built_by =
        GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance);
#define SEQS_BUILT_BY sequences_are_built_by,
#endif
    // We cannot use typeof here because clang complains about double const.
    const fcs_state *const s = &(instance->state_copy.s);

    fcs_seq_cards_power_type cards_under_sequences = 0;
    for (int a = 0; a < INSTANCE_STACKS_NUM; a++)
    {
        const_AUTO(col, fcs_state_get_col(*s, a));
        const_AUTO(col_len, fcs_col_len(col));
        if (col_len <= 1)
        {
            continue;
        }
        cards_under_sequences += FCS_SEQS_OVER_RENEGADE_POWER(
            update_col_cards_under_sequences(SEQS_BUILT_BY col, col_len - 1));
    }
    instance->initial_cards_under_sequences_value = cards_under_sequences;
}

// This function associates a board with an fcs_instance and
// does other initialisations. After it, you must call resume_instance()
// repeatedly.
static inline void start_process_with_board(fcs_instance *const instance,
    fcs_state_keyval_pair *const init_state,
    fcs_state_keyval_pair *const initial_non_canonized_state GCC_UNUSED)
{
#ifndef FCS_DISABLE_PATSOLVE
    instance->initial_non_canonized_state = initial_non_canonized_state;
#endif

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
#error FCS_STATE_STORAGE is not defined
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
        cmp_stacks_w_context, NULL
#else
        fc_solve_stack_compare_for_comparison
#endif
#endif
    );
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)
    instance->stacks_tree =
        fcs_libavl2_stacks_tree_create(cmp_stacks_w_context, NULL, NULL);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    instance->stacks_tree = rbinit(cmp_stacks_w_context, NULL);
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
    // Initialize the first state to init_state
    instance->state_copy = (typeof(instance->state_copy)){.s = init_state->s,
        .info = {
#ifdef INDIRECT_STACK_STATES
            .stacks_copy_on_write_flags = ~0,
#endif
// Initialize the state to be a base state for the game tree
#ifndef FCS_WITHOUT_DEPTH_FIELD
            .depth = 0,
#endif
#ifdef FCS_WITH_MOVES
            .moves_to_parent = NULL,
#endif
            .visited = 0,
            .parent = NULL,
            .scan_visited = {0}}};
    update_initial_cards_val(instance);

    fcs_kv_state no_use,
        pass_copy = FCS_STATE_keyval_pair_to_kv(&instance->state_copy);
    fc_solve_check_and_add_state(
#ifdef FCS_SINGLE_HARD_THREAD
        instance,
#else
        instance->hard_threads,
#endif
        &pass_copy, &no_use);

#ifndef FCS_SINGLE_HARD_THREAD
    instance->current_hard_thread = instance->hard_threads;
#endif
    {
        HT_LOOP_START()
        {
            HT_FIELD(hard_thread, prelude_idx) = 0;
            if (HT_FIELD(hard_thread, prelude))
            {
                set_next_prelude_item(hard_thread,
                    HT_FIELD(hard_thread, prelude),
                    &(HT_FIELD(hard_thread, st_idx)));
            }
        }
    }
#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
    STRUCT_SET_FLAG_TO(instance, FCS_RUNTIME_TO_REPARENT_STATES_REAL,
        STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_TO_REPARENT_STATES_PROTO));
#endif
}

static inline void free_hard_thread(fcs_hard_thread *const hard_thread)
{
#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
    free(HT_FIELD(hard_thread, prelude_as_string));
    free(HT_FIELD(hard_thread, prelude));
#endif
    fcs_move_stack_static_destroy(HT_FIELD(hard_thread, reusable_move_stack));
    free(HT_FIELD(hard_thread, soft_threads));
    fc_solve_compact_allocator_finish(&(HT_FIELD(hard_thread, allocator)));
}

static inline void free_instance(fcs_instance *const instance)
{
    fc_solve_foreach_soft_thread(
        instance, FOREACH_SOFT_THREAD_FREE_INSTANCE, NULL);

    HT_LOOP_START() { free_hard_thread(hard_thread); }

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
        free_hard_thread(instance->optimization_thread);
        free(instance->optimization_thread);
    }
#endif
#endif
    moves_order__free(&(instance->instance_moves_order));
#ifdef FCS_WITH_MOVES
    if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET))
    {
        moves_order__free(&(instance->opt_moves));
    }
    instance_free_solution_moves(instance);
#endif
}

static inline void recycle_ht(fcs_hard_thread *const hard_thread)
{
    fc_solve_reset_hard_thread(hard_thread);
    fc_solve_compact_allocator_recycle(&(HT_FIELD(hard_thread, allocator)));

    ST_LOOP_START()
    {
        st_free_pq(soft_thread);
        fc_solve_reset_soft_thread(soft_thread);

#ifndef FCS_DISABLE_PATSOLVE
        const_SLOT(pats_scan, soft_thread);

        if (pats_scan)
        {
            fc_solve_pats__recycle_soft_thread(pats_scan);
        }
#endif
    }
}

static inline void recycle_inst(fcs_instance *const instance)
{
    fc_solve_finish_instance(instance);
#ifdef FCS_WITH_MOVES
    instance_free_solution_moves(instance);
#endif
    instance->i__num_checked_states = 0;
    instance->num_hard_threads_finished = 0;
#ifdef FCS_SINGLE_HARD_THREAD
    recycle_ht(instance);
#ifdef FCS_WITH_MOVES
    if (instance->is_optimization_st)
    {
        fc_solve_reset_soft_thread(&(instance->optimization_soft_thread));
    }
#endif
#else
    for (uint_fast32_t ht_idx = 0; ht_idx < instance->num_hard_threads;
         ht_idx++)
    {
        recycle_ht(&(instance->hard_threads[ht_idx]));
    }
#ifdef FCS_WITH_MOVES
    if (instance->optimization_thread)
    {
        recycle_ht(instance->optimization_thread);
    }
#endif
#endif
#ifdef FCS_WITH_MOVES
    STRUCT_CLEAR_FLAG(instance, FCS_RUNTIME_IN_OPTIMIZATION_THREAD);
#endif
}

#ifdef FCS_WITH_MOVES
static inline void setup_opt_thread__helper(
    fcs_instance *const instance, fcs_soft_thread *const soft_thread)
{
    if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET))
    {
        if (soft_thread->by_depth_moves_order.by_depth_moves != NULL)
        {
            fc_solve_free_soft_thread_by_depth_move_array(soft_thread);
        }

        soft_thread->by_depth_moves_order =
            (typeof(soft_thread->by_depth_moves_order)){
                .num = 1,
                .by_depth_moves =
                    SMALLOC1(soft_thread->by_depth_moves_order.by_depth_moves),
            };
        soft_thread->by_depth_moves_order.by_depth_moves[0] =
            (typeof(soft_thread->by_depth_moves_order.by_depth_moves[0])){
                .max_depth = SSIZE_MAX,
                .moves_order = moves_order_dup(&(instance->opt_moves)),
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
static inline int optimize_solution(fcs_instance *const instance)
{
    fcs_soft_thread *const optimization_soft_thread =
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
#ifndef FCS_ENABLE_PRUNE__R_TF__UNCOND
        /* Copy enable_pruning from the thread that reached the solution,
         * because otherwise -opt in conjunction with -sp r:tf will fail.
         * */
        optimization_soft_thread->enable_pruning =
            instance->hard_thread.soft_threads[instance->hard_thread.st_idx]
                .enable_pruning;
#endif
        instance->is_optimization_st = TRUE;
    }

    setup_opt_thread__helper(instance, optimization_soft_thread);
    /* Instruct the optimization hard thread to run indefinitely AFA it
     * is concerned */
    instance->hard_thread.ht__max_num_checked_states = FCS_INT_LIMIT_MAX;
    return fc_solve_befs_or_bfs_do_solve(optimization_soft_thread);
}
#undef soft_thread
#else
static inline int optimize_solution(fcs_instance *const instance)
{
    fcs_soft_thread *soft_thread;
    fcs_hard_thread *optimization_thread;

    if (!instance->solution_moves.moves)
    {
        fc_solve_trace_solution(instance);
    }

#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
    STRUCT_TURN_ON_FLAG(instance, FCS_RUNTIME_TO_REPARENT_STATES_REAL);
#endif

    if (!instance->optimization_thread)
    {
        instance->optimization_thread = optimization_thread =
            SMALLOC1(optimization_thread);

        fc_solve_instance__init_hard_thread(instance, optimization_thread);

        fcs_hard_thread *const old_hard_thread = instance->current_hard_thread;

        soft_thread = optimization_thread->soft_threads;

#ifndef FCS_ENABLE_PRUNE__R_TF__UNCOND
        /* Copy enable_pruning from the thread that reached the solution,
         * because otherwise -opt in conjunction with -sp r:tf will fail.
         * */
        soft_thread->enable_pruning =
            old_hard_thread->soft_threads[old_hard_thread->st_idx]
                .enable_pruning;
#endif
    }
    else
    {
        optimization_thread = instance->optimization_thread;
        soft_thread = optimization_thread->soft_threads;
    }

    setup_opt_thread__helper(instance, soft_thread);
    /* Instruct the optimization hard thread to run indefinitely AFA it
     * is concerned */
    optimization_thread->ht__max_num_checked_states = FCS_INT_LIMIT_MAX;
    return fc_solve_befs_or_bfs_do_solve(soft_thread);
}
#endif
#endif

#ifdef DEBUG

#ifdef DEBUG_VERIFY_SOFT_DFS_STACK
static void verify_soft_dfs_stack(fcs_soft_thread *soft_thread)
{
    for (int depth = 0; depth < DFS_VAR(soft_thread, depth); ++depth)
    {
        var_AUTO(soft_dfs_info, &(DFS_VAR(soft_thread, soft_dfs_info)[depth]));
        int *const rand_indexes = soft_dfs_info->derived_states_random_indexes;

        const_AUTO(num_states, soft_dfs_info->derived_states_list.num_states);

        for (size_t i = soft_dfs_info->current_state_index; i < num_states; ++i)
        {
            verify_state_sanity(
                soft_dfs_info->derived_states_list.states[rand_indexes[i]]
                    .state_ptr);
        }
    }
}
#else
#define verify_soft_dfs_stack(soft_thread)
#endif

#define TRACE0(message)                                                        \
    fcs_trace("%s. Depth=%ld ; the_soft_Depth=%ld ; Iters=%ld ; "              \
              "move_func_list_idx=%ld ; move_func_idx=%ld ; "                  \
              "current_state_index=%ld ; num_states=%ld\n",                    \
        message, (long)DFS_VAR(soft_thread, depth),                            \
        (long)(the_soft_dfs_info - DFS_VAR(soft_thread, soft_dfs_info)),       \
        (long)(instance->i__num_checked_states),                               \
        (long)the_soft_dfs_info->move_func_list_idx,                           \
        (long)the_soft_dfs_info->move_func_idx,                                \
        (long)the_soft_dfs_info->current_state_index,                          \
        (long)(derived_list.num_states))

#define VERIFY_STATE_SANITY() verify_state_sanity(&FCS_SCANS_the_state)

#define VERIFY_PTR_STATE_AND_DERIVED_TRACE0(string)                            \
    {                                                                          \
        TRACE0(string);                                                        \
        VERIFY_STATE_SANITY();                                                 \
        VERIFY_DERIVED_STATE();                                                \
        verify_soft_dfs_stack(soft_thread);                                    \
    }

#else
#define TRACE0(no_use)
#define VERIFY_PTR_STATE_AND_DERIVED_TRACE0(no_use)
#endif

static inline bool fcs__is_state_a_dead_end(
    const fcs_collectible_state *const ptr_state)
{
    return (FCS_S_VISITED(ptr_state) & FCS_VISITED_DEAD_END);
}

#ifndef FCS_DISABLE_NUM_STORED_STATES
#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
#if (!((FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) ||               \
         (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)))
#define free_states(i)
#else
static inline void free_states_handle_soft_dfs_soft_thread(
    fcs_soft_thread *const soft_thread)
{
    var_AUTO(soft_dfs_info, DFS_VAR(soft_thread, soft_dfs_info));
    const_AUTO(end_soft_dfs_info, soft_dfs_info + DFS_VAR(soft_thread, depth));

    for (; soft_dfs_info < end_soft_dfs_info; soft_dfs_info++)
    {
        const_AUTO(rand_indexes, soft_dfs_info->derived_states_random_indexes);

        /*
         * We start from current_state_index instead of current_state_index+1
         * because that is the next state to be checked - it is referenced
         * by current_state_index++ instead of ++current_state_index .
         * */
        rating_with_index *dest_rand_index_ptr =
            rand_indexes + soft_dfs_info->current_state_index;
        const rating_with_index *rand_index_ptr = dest_rand_index_ptr;

        rating_with_index *const end_rand_index_ptr =
            rand_indexes + soft_dfs_info->derived_states_list.num_states;

        fcs_derived_states_list_item *const states =
            soft_dfs_info->derived_states_list.states;
        for (; rand_index_ptr < end_rand_index_ptr; rand_index_ptr++)
        {
            if (!fcs__is_state_a_dead_end(
                    states[rand_index_ptr->idx].state_ptr))
            {
                *(dest_rand_index_ptr++) = *(rand_index_ptr);
            }
        }
        soft_dfs_info->derived_states_list.num_states =
            dest_rand_index_ptr - rand_indexes;
    }
}

static bool free_states_should_delete(void *const key, void *const context)
{
    fcs_instance *const instance = (fcs_instance *const)context;
    fcs_collectible_state *const ptr_state = (fcs_collectible_state *const)key;

    if (fcs__is_state_a_dead_end(ptr_state))
    {
        FCS_S_NEXT(ptr_state) = instance->list_of_vacant_states;
        instance->list_of_vacant_states = ptr_state;

        --instance->active_num_states_in_collection;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static inline void free_states(fcs_instance *const instance)
{
    /* First of all, let's make sure the soft_threads will no longer
     * traverse to the freed states that are currently dead ends.
     * */

    HT_LOOP_START()
    {
        ST_LOOP_START()
        {
            if (soft_thread->super_method_type == FCS_SUPER_METHOD_DFS)
            {
                free_states_handle_soft_dfs_soft_thread(soft_thread);
            }
            else if (soft_thread->is_befs)
            {
                pri_queue new_pq;
                fc_solve_pq_init(&(new_pq));
                const_AUTO(elems, BEFS_VAR(soft_thread, pqueue).elems);
                const_AUTO(end_element,
                    elems + BEFS_VAR(soft_thread, pqueue).current_size);
                for (pq_element *next_element = elems + PQ_FIRST_ENTRY;
                     next_element <= end_element; next_element++)
                {
                    if (!fcs__is_state_a_dead_end((*next_element).val))
                    {
                        fc_solve_pq_push(&new_pq, (*next_element).val,
                            (*next_element).rating);
                    }
                }
                st_free_pq(soft_thread);
                BEFS_VAR(soft_thread, pqueue) = new_pq;
            }
        }
    }

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    /* Now let's recycle the states. */
    fc_solve_hash_foreach(
        &(instance->hash), free_states_should_delete, ((void *)instance));
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)
    /* Now let's recycle the states. */
    fc_solve_states_google_hash_foreach(
        instance->hash, free_states_should_delete, ((void *)instance));
#endif
}
#endif
#endif
#endif

#define SOFT_DFS_DEPTH_GROW_BY 64
static void increase_dfs_max_depth(fcs_soft_thread *const soft_thread)
{
    const_AUTO(new_dfs_max_depth,
        DFS_VAR(soft_thread, dfs_max_depth) + SOFT_DFS_DEPTH_GROW_BY);
    DFS_VAR(soft_thread, soft_dfs_info) =
        SREALLOC(DFS_VAR(soft_thread, soft_dfs_info), new_dfs_max_depth);
    memset(DFS_VAR(soft_thread, soft_dfs_info) +
               DFS_VAR(soft_thread, dfs_max_depth),
        '\0',
        SOFT_DFS_DEPTH_GROW_BY * sizeof(*DFS_VAR(soft_thread, soft_dfs_info)));

    DFS_VAR(soft_thread, dfs_max_depth) = new_dfs_max_depth;
}

// dfs_solve() is the event loop of the Random-DFS scan. DFS, which is
// recursive in nature, is handled here without procedural recursion by using
// some dedicated stacks for the traversal.
static inline int dfs_solve(fcs_soft_thread *const soft_thread)
{
    fcs_hard_thread *const hard_thread = soft_thread->hard_thread;
    fcs_instance *const instance = HT_INSTANCE(hard_thread);

    ssize_t by_depth_max_depth, by_depth_min_depth;

#ifndef FCS_DISABLE_SIMPLE_SIMON
    const bool is_simple_simon = instance->is_simple_simon;
#endif

#if !defined(FCS_WITHOUT_DEPTH_FIELD) &&                                       \
    !defined(FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE)
    const bool calc_real_depth = fcs_get_calc_real_depth(instance);
#endif
#ifndef FCS_HARD_CODE_SCANS_SYNERGY_AS_TRUE
    const bool scans_synergy =
        STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY);
#endif

    const bool is_a_complete_scan =
        STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN);
    const_AUTO(soft_thread_id, soft_thread->id);
    fcs_moves_order the_moves_list;
    fcs_moves_group_kind local_shuffling_type = FCS_NO_SHUFFLING;
#ifndef FCS_DISABLE_NUM_STORED_STATES
    const_SLOT(effective_max_num_states_in_collection, instance);
#endif
    FC__STACKS__SET_PARAMS();

#define DEPTH() (*depth_ptr)
    ssize_t *const depth_ptr = &(DFS_VAR(soft_thread, depth));

    var_AUTO(
        the_soft_dfs_info, &(DFS_VAR(soft_thread, soft_dfs_info)[DEPTH()]));

    ssize_t dfs_max_depth = DFS_VAR(soft_thread, dfs_max_depth);
#ifndef FCS_ENABLE_PRUNE__R_TF__UNCOND
    const_SLOT(enable_pruning, soft_thread);
#endif

    DECLARE_STATE();
    PTR_STATE = the_soft_dfs_info->state;
    FCS_ASSIGN_STATE_KEY();
    fcs_derived_states_list derived_list =
        the_soft_dfs_info->derived_states_list;
    fcs_rand_gen *const rand_gen = &(DFS_VAR(soft_thread, rand_gen));
    calculate_real_depth(calc_real_depth, PTR_STATE);
    const_AUTO(
        by_depth_units, DFS_VAR(soft_thread, moves_by_depth).by_depth_units);

#define THE_MOVE_FUNCS_LIST the_moves_list
    TRACE0("Before depth loop");

#define GET_DEPTH(ptr) ((ptr)->max_depth)

#define RECALC_BY_DEPTH_LIMITS()                                               \
    {                                                                          \
        by_depth_max_depth = GET_DEPTH(curr_by_depth_unit);                    \
        by_depth_min_depth = (curr_by_depth_unit == by_depth_units)            \
                                 ? 0                                           \
                                 : GET_DEPTH(curr_by_depth_unit - 1);          \
        the_moves_list = curr_by_depth_unit->move_funcs;                       \
    }

    fcs_int_limit_t *const instance_num_checked_states_ptr =
        &(instance->i__num_checked_states);
#ifndef FCS_SINGLE_HARD_THREAD
    fcs_int_limit_t *const hard_thread_num_checked_states_ptr =
        &(HT_FIELD(hard_thread, ht__num_checked_states));
#endif
    const_AUTO(max_num_states, calc_ht_max_num_states(instance, hard_thread));
#ifndef FCS_WITHOUT_ITER_HANDLER
    const_SLOT(debug_iter_output_func, instance);
    const_SLOT(debug_iter_output_context, instance);
#endif

    const moves_by_depth_unit *curr_by_depth_unit = by_depth_units;
    for (; (DEPTH() >= GET_DEPTH(curr_by_depth_unit)); ++curr_by_depth_unit)
    {
    }
    RECALC_BY_DEPTH_LIMITS();

    /*
        The main loop.
        We exit out of it when DEPTH() is decremented below zero.
    */
    while (1)
    {
    main_loop:
        /*
            Increase the "maximal" depth if it is about to be exceeded.
        */
        if (unlikely(DEPTH() + 1 >= dfs_max_depth))
        {
            increase_dfs_max_depth(soft_thread);

            /* Because the address of DFS_VAR(soft_thread, soft_dfs_info) may
             * be changed
             * */
            the_soft_dfs_info = &(DFS_VAR(soft_thread, soft_dfs_info)[DEPTH()]);
            dfs_max_depth = DFS_VAR(soft_thread, dfs_max_depth);
            /* This too has to be re-synced */
            derived_list = the_soft_dfs_info->derived_states_list;
        }

        TRACE0("Before current_state_index check");
        /* All the resultant states in the last test conducted were covered */
        if (the_soft_dfs_info->current_state_index == derived_list.num_states)
        {
            /* Check if we already tried all the tests here. */
            if (the_soft_dfs_info->move_func_list_idx ==
                THE_MOVE_FUNCS_LIST.num)
            {
                /* Backtrack to the previous depth. */

                if (is_a_complete_scan)
                {
                    FCS_S_VISITED(PTR_STATE) |= FCS_VISITED_ALL_TESTS_DONE;
                    MARK_AS_DEAD_END(PTR_STATE);
                }

                /* Set it now in case DEPTH() == 0 and we break */
                the_soft_dfs_info->derived_states_list = derived_list;
                if (unlikely(--DEPTH() < 0))
                {
                    break;
                }
                --the_soft_dfs_info;
                derived_list = the_soft_dfs_info->derived_states_list;
                PTR_STATE = the_soft_dfs_info->state;
                FCS_ASSIGN_STATE_KEY();
                soft_thread->num_vacant_freecells =
                    the_soft_dfs_info->num_vacant_freecells;
                soft_thread->num_vacant_stacks =
                    the_soft_dfs_info->num_vacant_stacks;

                if (unlikely(DEPTH() < by_depth_min_depth))
                {
                    curr_by_depth_unit--;
                    RECALC_BY_DEPTH_LIMITS();
                }

                continue; /* Just to make sure depth is not -1 now */
            }

            derived_list.num_states = 0;

            TRACE0("Before iter_handler");
            /* If this is the first test, then count the number of unoccupied
               freecells and stacks and check if we are done. */
            if ((the_soft_dfs_info->move_func_idx == 0) &&
                (the_soft_dfs_info->move_func_list_idx == 0))
            {
#ifndef FCS_WITHOUT_ITER_HANDLER
                TRACE0("In iter_handler");

                if (debug_iter_output_func)
                {
                    debug_iter_output_func(debug_iter_output_context,
                        *(instance_num_checked_states_ptr), DEPTH(),
                        (void *)instance, &pass,
#ifdef FCS_WITHOUT_VISITED_ITER
                        0
#else
                        ((DEPTH() == 0)
                                ? 0
                                : FCS_S_VISITED_ITER(DFS_VAR(
                                      soft_thread, soft_dfs_info)[DEPTH() - 1]
                                                         .state))
#endif
                    );
                }
#endif
                /* Perform the pruning. */
                bool was_pruned = FALSE;
                if (fcs__should_state_be_pruned(enable_pruning, PTR_STATE))
                {
                    fcs_collectible_state *const derived =
                        fc_solve_sfs_raymond_prune(soft_thread, pass);
                    if (derived)
                    {
                        was_pruned = TRUE;
                        the_soft_dfs_info->move_func_list_idx =
                            THE_MOVE_FUNCS_LIST.num;
                        fc_solve_derived_states_list_add_state(
                            &derived_list, derived, 0);
                        if (the_soft_dfs_info
                                ->derived_states_random_indexes_max_size < 1)
                        {
                            the_soft_dfs_info
                                ->derived_states_random_indexes_max_size = 1;
                            the_soft_dfs_info
                                ->derived_states_random_indexes = SREALLOC(
                                the_soft_dfs_info
                                    ->derived_states_random_indexes,
                                the_soft_dfs_info
                                    ->derived_states_random_indexes_max_size);
                        }

                        the_soft_dfs_info->derived_states_random_indexes[0]
                            .idx = 0;
                    }
                }

                if (!was_pruned)
                {
                    const fcs_game_limit num_vacant_freecells =
                        count_num_vacant_freecells(
                            LOCAL_FREECELLS_NUM, &FCS_SCANS_the_state);
                    const fcs_game_limit num_vacant_stacks =
                        count_num_vacant_stacks(
                            LOCAL_STACKS_NUM, &FCS_SCANS_the_state);
                    /* Check if we have reached the empty state */
                    if (unlikely((num_vacant_stacks == LOCAL_STACKS_NUM) &&
                                 (num_vacant_freecells == LOCAL_FREECELLS_NUM)))
                    {
#ifdef FCS_WITH_MOVES
                        instance->final_state = PTR_STATE;
#endif
                        BUMP_NUM_CHECKED_STATES();
                        TRACE0("Returning FCS_STATE_WAS_SOLVED");
                        return FCS_STATE_WAS_SOLVED;
                    }
                    /*
                        Cache num_vacant_freecells and num_vacant_stacks in
                       their appropriate stacks, so they won't be calculated
                       over and over again.
                      */
                    soft_thread->num_vacant_freecells =
                        the_soft_dfs_info->num_vacant_freecells =
                            num_vacant_freecells;
                    soft_thread->num_vacant_stacks =
                        the_soft_dfs_info->num_vacant_stacks =
                            num_vacant_stacks;
                    fc_solve__calc_positions_by_rank_data(
                        soft_thread, &FCS_SCANS_the_state,
                        (the_soft_dfs_info->positions_by_rank)
#ifndef FCS_DISABLE_SIMPLE_SIMON
                            ,
                        is_simple_simon
#endif
                    );
                }
            }

            TRACE0("After iter_handler");
            const_AUTO(orig_idx, the_soft_dfs_info->move_func_list_idx);
            const fcs_state_weighting *const weighting =
                &(THE_MOVE_FUNCS_LIST.groups[orig_idx].weighting);

            if (the_soft_dfs_info->move_func_list_idx < THE_MOVE_FUNCS_LIST.num)
            {
                /* Always do the first test */
                local_shuffling_type =
                    THE_MOVE_FUNCS_LIST
                        .groups[the_soft_dfs_info->move_func_list_idx]
                        .shuffling_type;

                do
                {
                    THE_MOVE_FUNCS_LIST
                        .groups[the_soft_dfs_info->move_func_list_idx]
                        .move_funcs[the_soft_dfs_info->move_func_idx]
                        .f(soft_thread, pass, &derived_list);

                    /* Move the counter to the next test */
                    if ((++the_soft_dfs_info->move_func_idx) ==
                        THE_MOVE_FUNCS_LIST
                            .groups[the_soft_dfs_info->move_func_list_idx]
                            .num)
                    {
                        the_soft_dfs_info->move_func_list_idx++;
                        the_soft_dfs_info->move_func_idx = 0;
                        break;
                    }
                } while ((local_shuffling_type != FCS_NO_SHUFFLING) ||
                         (derived_list.num_states == 0));
            }

            const_AUTO(num_states, derived_list.num_states);
            if (num_states >
                the_soft_dfs_info->derived_states_random_indexes_max_size)
            {
                the_soft_dfs_info->derived_states_random_indexes_max_size =
                    num_states;
                the_soft_dfs_info->derived_states_random_indexes = SREALLOC(
                    the_soft_dfs_info->derived_states_random_indexes,
                    the_soft_dfs_info->derived_states_random_indexes_max_size);
            }
            rating_with_index *const rand_array =
                the_soft_dfs_info->derived_states_random_indexes;

            for (size_t i = 0; i < num_states; i++)
            {
                rand_array[i].idx = i;
            }
            /* If we just conducted the tests for a random group -
             * randomize. Else - keep those indexes as the unity vector.
             *
             * Also, do not randomize if this is a pure soft-DFS scan.
             *
             * Also, do not randomize/sort if there's only one derived
             * state or less, because in that case, there is nothing
             * to reorder.
             * */
            if (num_states > 1)
            {
                switch (local_shuffling_type)
                {
                case FCS_RAND:
                {
                    for (size_t i = num_states - 1; i > 0; i--)
                    {
                        const typeof(i) j =
                            (fc_solve_rand_get_random_number(rand_gen) %
                                (i + 1));

                        const_AUTO(swap_save, rand_array[i]);
                        rand_array[i] = rand_array[j];
                        rand_array[j] = swap_save;
                    }
                }
                break;

                case FCS_WEIGHTING:
                    if (orig_idx < THE_MOVE_FUNCS_LIST.num)
                    {
                        fcs_derived_states_list_item *const derived_states =
                            derived_list.states;
                        for (size_t i = 0; i < num_states; i++)
                        {
                            rand_array[i].rating = befs_rate_state(soft_thread,
                                weighting,
#ifdef FCS_RCS_STATES
                                fc_solve_lookup_state_key_from_val(
                                    instance, derived_states[i].state_ptr),
#else
                                &(derived_states[i].state_ptr->s),
#endif
                                BEFS_MAX_DEPTH -
                                    calc_depth(derived_states[i].state_ptr));
                        }

                        const_AUTO(end, rand_array + num_states);
                        // Insertion-sort rand_array
                        for (var_PTR(p, rand_array + 1); p < end; ++p)
                        {
                            var_AUTO(move, p);
                            while (move > rand_array &&
                                   move->rating < move[-1].rating)
                            {
                                const_AUTO(temp, *move);
                                *move = move[-1];
                                *(--move) = temp;
                            }
                        }
                    }
                    break;

                case FCS_NO_SHUFFLING:
                    break;
                }
            }
            // We just performed a test, so the index of the first state that
            // ought to be checked in this depth is 0.
            the_soft_dfs_info->current_state_index = 0;
        }

        const_AUTO(num_states, derived_list.num_states);
        fcs_derived_states_list_item *const derived_states =
            derived_list.states;
        var_AUTO(state_idx, the_soft_dfs_info->current_state_index - 1);
        const rating_with_index *rand_int_ptr =
            the_soft_dfs_info->derived_states_random_indexes + state_idx;

        while (++state_idx < num_states)
        {
            fcs_collectible_state *const single_derived_state =
                derived_states[(*(++rand_int_ptr)).idx].state_ptr;

            VERIFY_PTR_STATE_AND_DERIVED_TRACE0("Verify [Before BUMP]");

            if ((!fcs__is_state_a_dead_end(single_derived_state)) &&
                (!is_scan_visited(single_derived_state, soft_thread_id)))
            {
                BUMP_NUM_CHECKED_STATES();
                VERIFY_PTR_STATE_AND_DERIVED_TRACE0("Verify [After BUMP]");
                set_scan_visited(single_derived_state, soft_thread_id);
#ifndef FCS_WITHOUT_VISITED_ITER
                FCS_S_VISITED_ITER(single_derived_state) =
                    instance->i__num_checked_states;
#endif
                VERIFY_PTR_STATE_AND_DERIVED_TRACE0("Verify [aft set_visit]");
                // I'm using current_state_indexes[depth]-1 because we already
                // increased it by one, so now it refers to the next state.
                if (unlikely(++DEPTH() >= by_depth_max_depth))
                {
                    curr_by_depth_unit++;
                    RECALC_BY_DEPTH_LIMITS();
                }
                the_soft_dfs_info->current_state_index = state_idx;
                the_soft_dfs_info->derived_states_list = derived_list;
                ++the_soft_dfs_info;
                the_soft_dfs_info->state = PTR_STATE = single_derived_state;
                FCS_ASSIGN_STATE_KEY();
                VERIFY_PTR_STATE_AND_DERIVED_TRACE0("Verify after recurse");

                the_soft_dfs_info->move_func_list_idx = 0;
                the_soft_dfs_info->move_func_idx = 0;
                the_soft_dfs_info->current_state_index = 0;
                derived_list = the_soft_dfs_info->derived_states_list;
                derived_list.num_states = 0;

                calculate_real_depth(calc_real_depth, PTR_STATE);

#ifndef FCS_DISABLE_NUM_STORED_STATES
#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
                if (instance->active_num_states_in_collection >=
                    instance->effective_trim_states_in_collection_from)
                {
                    free_states(instance);
                }
#endif
#endif
                if (check_if_limits_exceeded())
                {
                    the_soft_dfs_info->derived_states_list = derived_list;
                    TRACE0("Returning FCS_STATE_SUSPEND_PROCESS (inside "
                           "current_state_index)");
#ifdef FCS_WITH_MOVES
                    instance->final_state = PTR_STATE;
#endif
                    return FCS_STATE_SUSPEND_PROCESS;
                }
                goto main_loop;
            }
        }
        the_soft_dfs_info->current_state_index = num_states;
    }
    // We need to bump the number of iterations so it will be ready with
    // a fresh iterations number for the next scan that takes place.
    BUMP_NUM_CHECKED_STATES();
    DEPTH() = -1;

    return FCS_STATE_IS_NOT_SOLVEABLE;
}

static inline void init_dfs(fcs_soft_thread *const soft_thread)
{
    fcs_instance *const instance = fcs_st_instance(soft_thread);
    // Allocate some space for the states at depth 0.
    DFS_VAR(soft_thread, depth) = 0;
    increase_dfs_max_depth(soft_thread);
    DFS_VAR(soft_thread, soft_dfs_info)
    [0].state = FCS_STATE_keyval_pair_to_collectible(&instance->state_copy);
    fc_solve_rand_init(
        &(DFS_VAR(soft_thread, rand_gen)), DFS_VAR(soft_thread, rand_seed));

    if (!DFS_VAR(soft_thread, moves_by_depth).by_depth_units)
    {
        const_SLOT(master_to_randomize, soft_thread);
        fcs_moves_by_depth_array *const arr_ptr =
            &(DFS_VAR(soft_thread, moves_by_depth));
        arr_ptr->by_depth_units = SMALLOC(arr_ptr->by_depth_units,
            (arr_ptr->num_units = soft_thread->by_depth_moves_order.num));

        const fcs_by_depth_moves_order *const by_depth_moves_order =
            soft_thread->by_depth_moves_order.by_depth_moves;

        var_AUTO(unit, arr_ptr->by_depth_units);
        const_AUTO(depth_num, soft_thread->by_depth_moves_order.num);
        for (size_t depth_idx = 0; depth_idx < depth_num; ++depth_idx, ++unit)
        {
            unit->max_depth = by_depth_moves_order[depth_idx].max_depth;

            fcs_moves_group *const tests_order_groups =
                by_depth_moves_order[depth_idx].moves_order.groups;

            const_AUTO(tests_order_num,
                by_depth_moves_order[depth_idx].moves_order.num);

            const_AUTO(moves_list_of_lists, &(unit->move_funcs));

            *moves_list_of_lists = (typeof(*moves_list_of_lists)){
                .num = 0,
                .groups = SMALLOC(moves_list_of_lists->groups, tests_order_num),
            };

            for (size_t group_idx = 0; group_idx < tests_order_num; ++group_idx)
            {
                size_t num = 0;
                fcs_move_func *tests_list = NULL;
                add_to_move_funcs_list(&tests_list, &num,
                    tests_order_groups[group_idx].move_funcs,
                    tests_order_groups[group_idx].num);
                /* TODO : convert to C99 struct initializers. */
                const_AUTO(tests_list_struct_ptr,
                    &(moves_list_of_lists->groups[moves_list_of_lists->num++]));

                const fcs_moves_group_kind shuffling_type =
                    (master_to_randomize
                            ? tests_order_groups[group_idx].shuffling_type
                            : FCS_NO_SHUFFLING);
                *tests_list_struct_ptr = (typeof(*tests_list_struct_ptr)){
                    .move_funcs = tests_list,
                    .num = num,
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

            moves_list_of_lists->groups =
                SREALLOC(moves_list_of_lists->groups, moves_list_of_lists->num);
        }
    }
}

/*
 * Switch to the next soft thread in the hard thread,
 * since we are going to call continue and this is
 * a while loop
 * */
static inline void switch_to_next_soft_thread(
    fcs_hard_thread *const hard_thread, const int_fast32_t num_soft_threads,
    const fcs_soft_thread *const soft_threads,
    const fc_solve_prelude_item *const prelude, const size_t prelude_num_items,
    int_fast32_t *const st_idx_ptr)
{
    if (HT_FIELD(hard_thread, prelude_idx) < prelude_num_items)
    {
        set_next_prelude_item(hard_thread, prelude, st_idx_ptr);
    }
    else
    {
        const int_fast32_t next_st_idx =
            ((1 + (*st_idx_ptr)) % num_soft_threads);
        set_next_soft_thread(hard_thread, next_st_idx,
            soft_threads[next_st_idx].checked_states_step, st_idx_ptr);
    }
}

#ifndef FCS_DISABLE_PATSOLVE
static inline fc_solve_solve_process_ret_t do_patsolve(
    fcs_soft_thread *const soft_thread)
{
    const_SLOT(hard_thread, soft_thread);
    const_SLOT(pats_scan, soft_thread);
    const_AUTO(start_from, pats_scan->num_checked_states);

    pats_scan->max_num_checked_states =
        start_from + (HT_FIELD(hard_thread, ht__max_num_checked_states) -
                         NUM_CHECKED_STATES);
    pats_scan->status = FCS_PATS__NOSOL;
    fc_solve_pats__do_it(pats_scan);

    const_AUTO(after_scan_delta, pats_scan->num_checked_states - start_from);
#ifndef FCS_SINGLE_HARD_THREAD
    HT_FIELD(hard_thread, ht__num_checked_states) += after_scan_delta;
#endif
    HT_INSTANCE(hard_thread)->i__num_checked_states += after_scan_delta;

    switch (pats_scan->status)
    {
    case FCS_PATS__WIN:
        return FCS_STATE_WAS_SOLVED;

    case FCS_PATS__NOSOL:
        return FCS_STATE_IS_NOT_SOLVEABLE;

    case FCS_PATS__FAIL:
        return FCS_STATE_SUSPEND_PROCESS;

#ifndef __clang__
    default:
        __builtin_unreachable();
#endif
    }
}
#endif

static inline int solve(fcs_soft_thread *const soft_thread)
{
    switch (soft_thread->super_method_type)
    {
    case FCS_SUPER_METHOD_DFS:
        return dfs_solve(soft_thread);

    case FCS_SUPER_METHOD_BEFS_BRFS:
        return fc_solve_befs_or_bfs_do_solve(soft_thread);

#ifndef FCS_DISABLE_PATSOLVE
    case FCS_SUPER_METHOD_PATSOLVE:
        return do_patsolve(soft_thread);
#endif

#ifndef __clang__
    default:
        __builtin_unreachable();
#endif
    }
}

/* instance__check_exceeded_stats() cannot be an inline function because if
 * it is, the code becomes considerably slower (at least on gcc-5.4.0 on x86-64
 * Linux). */
#ifdef FCS_WITHOUT_MAX_NUM_STATES
#define instance__check_exceeded_stats(instance) FALSE
#else
#ifdef FCS_DISABLE_NUM_STORED_STATES
#define instance_check_exceeded__num_states(instance)
#else
#define instance_check_exceeded__num_states(instance)                          \
    || (instance->num_states_in_collection >=                                  \
           instance->effective_max_num_states_in_collection)
#endif
#define instance__check_exceeded_stats(instance)                               \
    ((ret == FCS_STATE_SUSPEND_PROCESS) &&                                     \
        ((instance->i__num_checked_states >=                                   \
            instance->effective_max_num_checked_states)                        \
                instance_check_exceeded__num_states(instance)))
#endif

static inline fc_solve_solve_process_ret_t run_hard_thread(
    fcs_hard_thread *const hard_thread)
{
    const size_t prelude_num_items = HT_FIELD(hard_thread, prelude_num_items);
#ifdef FCS_SINGLE_HARD_THREAD
#define instance hard_thread
#else
    fcs_instance *const instance = hard_thread->instance;
#endif
    int_fast32_t *const st_idx_ptr = &(HT_FIELD(hard_thread, st_idx));
    /*
     * Again, making sure that not all of the soft_threads in this
     * hard thread are finished.
     * */

    fc_solve_solve_process_ret_t ret = FCS_STATE_SUSPEND_PROCESS;
    const int_fast32_t num_soft_threads =
        HT_FIELD(hard_thread, num_soft_threads);
    const fc_solve_prelude_item *const prelude = HT_FIELD(hard_thread, prelude);
    fcs_soft_thread *const soft_threads = HT_FIELD(hard_thread, soft_threads);

    while (HT_FIELD(hard_thread, num_soft_threads_finished) < num_soft_threads)
    {
        fcs_soft_thread *const soft_thread = &(soft_threads[*st_idx_ptr]);
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
            init_dfs(soft_thread);
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
                const size_t stacks_num = INSTANCE_STACKS_NUM;
#endif
                for (size_t i = 0; i < STACKS_NUM__VAL; ++i)
                {
                    var_AUTO(src_col,
                        fcs_state_get_col(pats_scan->current_pos.s, i));
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
        ret = solve(soft_thread);
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
                ++instance->num_hard_threads_finished;
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

        const bool was_solved = (ret == FCS_STATE_WAS_SOLVED);
#if (defined(FCS_WITH_MOVES) && (!defined(FCS_DISABLE_PATSOLVE)))
        instance->solving_soft_thread = soft_thread;
#endif
        if (was_solved || instance__check_exceeded_stats(instance))
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
static inline fc_solve_solve_process_ret_t resume_instance(
    fcs_instance *const instance)
{
    fc_solve_solve_process_ret_t ret = FCS_STATE_SUSPEND_PROCESS;
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
        fcs_hard_thread *const end_of_hard_threads =
            instance->hard_threads + instance->num_hard_threads;

        fcs_hard_thread *hard_thread = instance->current_hard_thread;
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
            for (; hard_thread < end_of_hard_threads; ++hard_thread)
#endif
            {
                ret = run_hard_thread(hard_thread);
                if ((ret == FCS_STATE_IS_NOT_SOLVEABLE) ||
                    (ret == FCS_STATE_WAS_SOLVED) ||
                    instance__check_exceeded_stats(instance))
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
    // Call optimize_solution only once. Make sure that if it has already
    // run - we retain the old ret.
    if (ret == FCS_STATE_WAS_SOLVED &&
        STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_OPTIMIZE_SOLUTION_PATH) &&
        !STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_IN_OPTIMIZATION_THREAD))
    {
        ret = optimize_solution(instance);
    }
#endif
    return ret;
}
#ifdef FCS_SINGLE_HARD_THREAD
#undef hard_thread
#endif

typedef struct
{
    fcs_int_limit_t num_checked_states;
#ifndef FCS_DISABLE_NUM_STORED_STATES
    fcs_int_limit_t num_states_in_collection;
#endif
} fcs_stats;

static const fcs_stats initial_stats = {.num_checked_states = 0,
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
    fcs_instance obj;
    fc_solve_solve_process_ret_t ret_code;
    /* Whether the instance is ready to be input with (i.e:
     * was recycled already.) */
    bool instance_is_ready;
#ifdef FCS_WITH_FLARES
    char name[30];
#endif
#ifdef FCS_WITH_MOVES
    uint_fast32_t next_move_idx;
    fcs_moves_sequence_t moves_seq;
#endif
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    fcs_moves_processed fc_pro_moves;
#endif
    fcs_stats obj_stats;
#if defined(FCS_WITH_MOVES) || defined(FCS_WITH_FLARES)
    bool was_solution_traced;
#endif
#ifdef FCS_WITH_MOVES
    fcs_state_locs_struct trace_solution_state_locs;
#endif
} flare_item;

#ifdef FCS_WITH_FLARES
typedef enum
{
    FLARES_PLAN_RUN_INDEFINITELY,
    FLARES_PLAN_RUN_COUNT_ITERS,
    FLARES_PLAN_CHECKPOINT,
} flares_plan_type;

#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
typedef enum
{
    FLARES_CHOICE_FC_SOLVE_SOLUTION_LEN,
    FLARES_CHOICE_FCPRO_SOLUTION_LEN
} flares_choice_type;
#endif

typedef fcs_int_limit_t flare_iters_quota;

static inline flare_iters_quota normalize_iters_quota(const flare_iters_quota i)
{
    return max(i, 0);
}

typedef struct
{
    flare_item *flare;
    flares_plan_type type;
    flare_iters_quota remaining_quota, initial_quota;
    int_fast32_t count_iters;
} flares_plan_item;

#endif

typedef struct
{
#ifdef FCS_WITH_FLARES
    flare_item *flares, *end_of_flares, *minimal_flare, *intract_minimal_flare;
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
    bool flares_plan_compiled;
    bool all_plan_items_finished_so_far;
#else
    flare_item single_flare;
#endif
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    fcs_int_limit_t limit;
#endif
} fcs_instance_item;

typedef struct
{
#ifdef FCS_WITH_NI
    /*
     * This is a list of several consecutive instances that are run
     * one after the other in case the previous ones could not solve
     * the board
     * */
    fcs_instance_item *current_instance, *instances_list,
        *end_of_instances_list;
#else
    fcs_instance_item single_inst;
#endif
#ifndef FCS_WITHOUT_MAX_NUM_STATES
    // The global (sequence-wide) limit of the iterations. Used
    // by limit_iterations() and friends
    fcs_int_limit_t current_iterations_limit;
#endif
    fcs_stats iterations_board_started_at;
    // The number of iterations that the current instance started solving from.
    fcs_stats init_num_checked_states;
    // A pointer to the currently active flare out of the sequence
#if defined(FCS_WITH_NI) || defined(FCS_WITH_FLARES)
#define ACTIVE_FLARE(user) ((user)->active_flare)
#define SET_ACTIVE_FLARE(user, flare) ((user)->active_flare = (flare))
    flare_item *active_flare;
#else
#define ACTIVE_FLARE(user) (&((user)->single_inst.single_flare))
#define SET_ACTIVE_FLARE(user, flare)
#endif
#define OBJ_STATS(user) (ACTIVE_FLARE(user)->obj_stats)
    fcs_state_keyval_pair state;
#ifdef FCS_WITH_MOVES
    fcs_state_keyval_pair running_state;
#endif
#if defined(FCS_WITH_FLARES) || !defined(FCS_DISABLE_PATSOLVE)
    fcs_state_keyval_pair initial_non_canonized_state;
#endif
#ifdef FCS_WITH_MOVES
    fcs_state_locs_struct state_locs;
    fcs_state_locs_struct initial_state_locs;
#endif
    fc_solve_solve_process_ret_t ret_code;
#ifdef FCS_WITH_NI
    bool all_instances_were_suspended;
#endif
#ifdef FCS_WITH_ERROR_STRS
    state_validity_ret state_validity_ret;
    fcs_card state_validity_card;
#endif
#ifndef FCS_WITHOUT_ITER_HANDLER
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    freecell_solver_user_iter_handler_t iter_handler;
#endif
    freecell_solver_user_long_iter_handler_t long_iter_handler;
    void *iter_handler_context;
#endif
#ifdef FCS_WITH_FLARES
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    flares_choice_type flares_choice;
#endif
    double flares_iters_factor;
#endif
    fcs_soft_thread *soft_thread;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
#define MAX_STATE_STRING_COPY_LEN 2048
    char state_string_copy[MAX_STATE_STRING_COPY_LEN];
    FCS_ON_NOT_FC_ONLY(fcs_preset common_preset);
    FCS__DECL_ERR_BUF(error_string);
    meta_allocator meta_alloc;
} fcs_user;

static inline fcs_instance *user_obj(fcs_user *const user)
{
    return &(ACTIVE_FLARE(user)->obj);
}

static inline fcs_instance *active_obj(void *const api_instance)
{
    return user_obj((fcs_user *)api_instance);
}

static inline fcs_instance_item *curr_inst(fcs_user *const user)
{
#ifdef FCS_WITH_NI
    return user->current_instance;
#else
    return &user->single_inst;
#endif
}

#ifdef FCS_WITH_NI
#define INSTANCES_LOOP_START()                                                 \
    {                                                                          \
        const_SLOT(end_of_instances_list, user);                               \
        for (fcs_instance_item *instance_item = user->instances_list;          \
             instance_item < end_of_instances_list; ++instance_item)           \
        {
#else
#define INSTANCES_LOOP_START()                                                 \
    do                                                                         \
    {                                                                          \
        const_AUTO(instance_item, curr_inst(user));
#endif

#ifdef FCS_WITH_FLARES

#define INSTANCE_ITEM_FLARES_LOOP_START()                                      \
    const flare_item *const end_of_flares = instance_item->end_of_flares;      \
    for (flare_item *flare = instance_item->flares; flare < end_of_flares;     \
         ++flare)                                                              \
    {
#else

#define INSTANCE_ITEM_FLARES_LOOP_START()                                      \
    flare_item *const flare = &(instance_item->single_flare);                  \
    {
#endif

#define INSTANCE_ITEM_FLARES_LOOP_END() }

#ifdef FCS_WITH_NI
#define INSTANCES_LOOP_END()                                                   \
    }                                                                          \
    }
#else
#define INSTANCES_LOOP_END()                                                   \
    }                                                                          \
    while (0)                                                                  \
        ;
#endif

#define FLARES_LOOP_START()                                                    \
    INSTANCES_LOOP_START()                                                     \
    INSTANCE_ITEM_FLARES_LOOP_START()

#ifdef FCS_WITH_FLARES
#define FLARE_INLINE
#else
#define FLARE_INLINE inline
#endif

#ifndef FCS_FREECELL_ONLY
static inline void calc_variant_suit_mask_and_desired_suit_value(
    fcs_instance *const instance GCC_UNUSED)
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

static void apply_game_params_for_all_instances(fcs_user *const user)
{
    FLARES_LOOP_START()
    {
        fcs_instance *const instance = &(flare->obj);
        instance->game_params = user->common_preset.game_params;
        calc_variant_suit_mask_and_desired_suit_value(instance);
    }
    INSTANCE_ITEM_FLARES_LOOP_END()
    INSTANCES_LOOP_END()
}
#endif

typedef struct
{
    fcs_state *key;
    fcs_state_locs_struct locs;
} standalone_state_ptrs;

#ifndef FCS_WITHOUT_ITER_HANDLER
static void iter_handler_wrapper(void *const api_instance,
    const fcs_int_limit_t iter_num, const int depth,
    void *lp_instance GCC_UNUSED, fcs_kv_state *const ptr_state,
    const fcs_int_limit_t parent_iter_num)
{
    fcs_user *const user = (fcs_user *)api_instance;

    standalone_state_ptrs state_raw = {
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
    fcs_user *const user, const instance_debug_iter_output_func value)
{
    FLARES_LOOP_START()
    flare->obj.debug_iter_output_func = value;
    INSTANCE_ITEM_FLARES_LOOP_END()
    INSTANCES_LOOP_END()
}

static inline void set_any_iter_handler(void *const api_instance,
    const freecell_solver_user_long_iter_handler_t long_iter_handler,
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    const freecell_solver_user_iter_handler_t iter_handler,
#endif
    void *const iter_handler_context)
{
    fcs_user *const user = (fcs_user *)api_instance;

    user->long_iter_handler = long_iter_handler;
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    user->iter_handler = iter_handler;
#endif

    instance_debug_iter_output_func cb = NULL;
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

static FLARE_INLINE void user_next_flare(fcs_user *const user)
{
    const_AUTO(instance_item, curr_inst(user));
#ifdef FCS_WITH_FLARES
    const_AUTO(
        num_flares, instance_item->end_of_flares - instance_item->flares);
    instance_item->flares = SREALLOC(instance_item->flares, num_flares + 1);
    flare_item *const flare = instance_item->flares + num_flares;
    instance_item->end_of_flares = flare + 1;
#else
    flare_item *const flare = &(instance_item->single_flare);
#endif
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    instance_item->limit = -1;
#endif
    fcs_instance *const instance = &(flare->obj);

    SET_ACTIVE_FLARE(user, flare);
    alloc_instance(instance, &(user->meta_alloc));
    // Switch the soft_thread variable so it won't refer to the old instance
    user->soft_thread = &(INST_HT0(instance).soft_threads[0]);

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

#ifdef FCS_WITH_FLARES
    flare->name[0] = '\0';
#endif
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    flare->fc_pro_moves.moves = NULL;
#endif
    flare->instance_is_ready = TRUE;
    flare->obj_stats = initial_stats;
}

#ifdef FCS_WITH_NI
#define NI_INLINE
#else
#define NI_INLINE inline
#endif

static NI_INLINE void user_next_instance(fcs_user *const user)
{
#ifdef FCS_WITH_NI
    const_AUTO(
        num_instances, user->end_of_instances_list - user->instances_list);
    user->instances_list = SREALLOC(user->instances_list, num_instances + 1);

    user->end_of_instances_list =
        (user->current_instance = user->instances_list + num_instances) + 1;
#endif

#ifdef FCS_WITH_FLARES
    *(curr_inst(user)) = (fcs_instance_item){
        .flares = NULL,
        .end_of_flares = NULL,
        .plan = NULL,
        .num_plan_items = 0,
        .flares_plan_string = NULL,
        .flares_plan_compiled = FALSE,
        .current_plan_item_idx = 0,
        .minimal_flare = NULL,
        .intract_minimal_flare = NULL,
        .all_plan_items_finished_so_far = TRUE,
    };
#endif

    /* ret_code and limit are set at user_next_flare(). */
    user_next_flare(user);
}

#ifdef FCS_WITH_ERROR_STRS
#define ALLOC_ERROR_STRING(var, s) *(var) = strdup(s)
#define SET_ERROR_VAR(var, s) *(var) = (((s)[0]) ? strdup(s) : NULL)
static inline void clear_error(fcs_user *const user)
{
    user->error_string[0] = '\0';
}
#else
#define ALLOC_ERROR_STRING(var, s)
#define SET_ERROR_VAR(var, s)
#define clear_error(user)
#endif

#ifdef FCS_BREAK_BACKWARD_COMPAT_1
#define MYINLINE inline
#else
#define MYINLINE
#endif

static MYINLINE void user_initialize(fcs_user *const user)
{
#ifndef FCS_FREECELL_ONLY
    const fcs_preset *freecell_preset;
    fc_solve_get_preset_by_name("freecell", &freecell_preset);
    fcs_duplicate_preset(user->common_preset, *freecell_preset);
#endif

    fc_solve_meta_compact_allocator_init(&(user->meta_alloc));

#ifdef FCS_WITH_NI
    user->instances_list = NULL;
    user->end_of_instances_list = NULL;
#endif
#ifndef FCS_WITHOUT_ITER_HANDLER
    user->long_iter_handler = NULL;
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
    user->iter_handler = NULL;
#endif
#endif
#ifndef FCS_WITHOUT_MAX_NUM_STATES
    user->current_iterations_limit = -1;
#endif

    user->iterations_board_started_at = initial_stats;
#ifdef FCS_WITH_NI
    user->all_instances_were_suspended = TRUE;
#endif
#ifdef FCS_WITH_FLARES
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
    user->flares_choice = FLARES_CHOICE_FC_SOLVE_SOLUTION_LEN;
#endif
    user->flares_iters_factor = 1.0;
#endif
    clear_error(user);
    user_next_instance(user);
}

void DLLEXPORT *freecell_solver_user_alloc(void)
{
    fcs_user *const ret = (fcs_user *)SMALLOC1(ret);
    user_initialize(ret);
    return (void *)ret;
}

#ifndef FCS_FREECELL_ONLY
int DLLEXPORT freecell_solver_user_apply_preset(
    void *const api_instance, const char *const preset_name)
{
    const fcs_preset *new_preset_ptr;
    fcs_user *const user = (fcs_user *)api_instance;

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
    INSTANCE_ITEM_FLARES_LOOP_END()
    INSTANCES_LOOP_END()

    fcs_duplicate_preset(user->common_preset, *new_preset_ptr);

    return FCS_PRESET_CODE_OK;
}
#endif

#ifndef FCS_WITHOUT_MAX_NUM_STATES
void DLLEXPORT freecell_solver_user_limit_iterations_long(
    void *const api_instance, const fcs_int_limit_t max_iters)
{
    ((fcs_user *const)api_instance)->current_iterations_limit = max_iters;
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
void DLLEXPORT freecell_solver_user_limit_iterations(
    void *const api_instance, const int max_iters)
{
    freecell_solver_user_limit_iterations_long(
        api_instance, (fcs_int_limit_t)max_iters);
}

void DLLEXPORT freecell_solver_user_limit_current_instance_iterations(
    void *const api_instance, const int max_iters)
{
    curr_inst((fcs_user *)api_instance)->limit = max_iters;
}
#endif
#endif

static inline fcs_soft_thread *api_soft_thread(void *const api_instance)
{
    return ((fcs_user *const)api_instance)->soft_thread;
}

int DLLEXPORT freecell_solver_user_set_depth_tests_order(
    void *const api_instance, const int min_depth,
    const char *const moves_order FCS__PASS_ERR_STR(char **const error_string))
{
    fcs_soft_thread *const soft_thread = api_soft_thread(api_instance);

    if (min_depth < 0)
    {
        ALLOC_ERROR_STRING(error_string, "Depth is negative.");
        return 1;
    }

    size_t depth_idx = 0;
    if (min_depth > 0)
    {
        for (;; ++depth_idx)
        {
            if (depth_idx == soft_thread->by_depth_moves_order.num - 1)
            {
                break;
            }
            else if (min_depth <=
                     soft_thread->by_depth_moves_order.by_depth_moves[depth_idx]
                         .max_depth)
            {
                break;
            }
        }

        ++depth_idx;
    }

    if (depth_idx == soft_thread->by_depth_moves_order.num)
    {
        soft_thread->by_depth_moves_order.by_depth_moves =
            SREALLOC(soft_thread->by_depth_moves_order.by_depth_moves,
                ++soft_thread->by_depth_moves_order.num);

        soft_thread->by_depth_moves_order.by_depth_moves[depth_idx]
            .moves_order.num = 0;
        soft_thread->by_depth_moves_order.by_depth_moves[depth_idx]
            .moves_order.groups = NULL;
    }

    if (depth_idx > 0)
    {
        soft_thread->by_depth_moves_order.by_depth_moves[depth_idx - 1]
            .max_depth = min_depth;
    }

    soft_thread->by_depth_moves_order.by_depth_moves[depth_idx].max_depth =
        SSIZE_MAX;

    FCS__DECL_ERR_BUF(static_error_string);
    const int ret_code = fc_solve_apply_moves_order(
        &(soft_thread->by_depth_moves_order.by_depth_moves[depth_idx]
                .moves_order),
        moves_order FCS__PASS_ERR_STR(static_error_string));

    SET_ERROR_VAR(error_string, static_error_string);

    for (size_t d = depth_idx + 1; d < soft_thread->by_depth_moves_order.num;
         ++d)
    {
        moves_order__free(
            &(soft_thread->by_depth_moves_order.by_depth_moves[d].moves_order));
    }

    soft_thread->by_depth_moves_order.by_depth_moves =
        SREALLOC(soft_thread->by_depth_moves_order.by_depth_moves,
            soft_thread->by_depth_moves_order.num = depth_idx + 1);

    return ret_code;
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
int DLLEXPORT freecell_solver_user_set_tests_order(void *api_instance,
    const char *moves_order FCS__PASS_ERR_STR(char **error_string))
{
    return freecell_solver_user_set_depth_tests_order(
        api_instance, 0, moves_order FCS__PASS_ERR_STR(error_string));
}
#endif

typedef enum
{
    FCS_COMPILE_FLARES_RET_OK = 0,
    FCS_COMPILE_FLARES_RET_COLON_NOT_FOUND,
    FCS_COMPILE_FLARES_RET_RUN_AT_SIGN_NOT_FOUND,
    FCS_COMPILE_FLARES_RET_UNKNOWN_FLARE_NAME,
    FCS_COMPILE_FLARES_RET_JUNK_AFTER_CP,
    FCS_COMPILE_FLARES_RET_UNKNOWN_COMMAND,
    FCS_COMPILE_FLARES_RUN_JUNK_AFTER_LAST_RUN_INDEF
} fcs_compile_flares_ret;

#ifdef FCS_WITH_FLARES
static inline flares_plan_item create_plan_item(const flares_plan_type mytype,
    flare_item *const flare, const int_fast32_t count_iters)
{
    return (const flares_plan_item){
        .type = mytype, .flare = flare, .count_iters = count_iters};
}

static inline void add_to_plan(fcs_instance_item *const instance_item,
    const flares_plan_type mytype, flare_item *const flare,
    const int_fast32_t count_iters)
{
    const_AUTO(next_item, instance_item->num_plan_items);

    instance_item->plan =
        SREALLOC(instance_item->plan, ++(instance_item->num_plan_items));

    instance_item->plan[next_item] =
        create_plan_item(mytype, flare, count_iters);
}

static inline void add_checkpoint_to_plan(
    fcs_instance_item *const instance_item)
{
    add_to_plan(instance_item, FLARES_PLAN_CHECKPOINT, NULL, -1);
}

#define MAX_FLARE_LEN_NAME 32
static inline flare_item *find_flare(flare_item *const flares,
    const flare_item *const end_of_flares, const char *const proto_name,
    const size_t name_len)
{
    char name[MAX_FLARE_LEN_NAME];
    strncpy(name, proto_name, MAX_FLARE_LEN_NAME - 1);
    name[min(MAX_FLARE_LEN_NAME - 1, name_len)] = '\0';

    for (flare_item *flare = flares; flare < end_of_flares; flare++)
    {
        if (!strcmp(flare->name, name))
        {
            return flare;
        }
    }
    return NULL;
}

#ifdef FCS_WITH_ERROR_STRS
#define SET_ERROR(s) strcpy(user->error_string, s)
#else
#define SET_ERROR(s)
#endif

static inline fcs_compile_flares_ret user_compile_all_flares_plans(
    fcs_user *const user)
{
    INSTANCES_LOOP_START()
    if (instance_item->flares_plan_compiled)
    {
        continue;
    }
    flare_item *const flares = instance_item->flares;
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
    const char *item_end;
    const char *item_start = instance_item->flares_plan_string;
    if (instance_item->plan)
    {
        free(instance_item->plan);
        instance_item->plan = NULL;
        instance_item->num_plan_items = 0;
    }
    do
    {
        const char *cmd_end = strchr(item_start, ':');
        if (!cmd_end)
        {
            SET_ERROR("Could not find a \":\" for a command.");
            return FCS_COMPILE_FLARES_RET_COLON_NOT_FOUND;
        }

        if (string_starts_with(item_start, "Run", cmd_end))
        {
            /* It's a Run item - handle it. */
            const int_fast32_t count_iters = atoi(++cmd_end);
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
            if (!((item_end = strchr(after_at_sign, ','))))
            {
                item_end = strchr(after_at_sign, '\0');
            }

            flare_item *const flare = find_flare(
                flares, end_of_flares, after_at_sign, item_end - after_at_sign);

            if (!flare)
            {
                SET_ERROR("Unknown flare name.");
                return FCS_COMPILE_FLARES_RET_UNKNOWN_FLARE_NAME;
            }

            add_to_plan(
                instance_item, FLARES_PLAN_RUN_COUNT_ITERS, flare, count_iters);
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
            if (strchr(++cmd_end, ','))
            {
                SET_ERROR("Junk after last RunIndef command. Must "
                          "be the final command.");
                return FCS_COMPILE_FLARES_RUN_JUNK_AFTER_LAST_RUN_INDEF;
            }
            item_end = strchr(cmd_end, '\0');

            flare_item *const flare =
                find_flare(flares, end_of_flares, cmd_end, item_end - cmd_end);
            if (!flare)
            {
                SET_ERROR("Unknown flare name in RunIndef command.");
                return FCS_COMPILE_FLARES_RET_UNKNOWN_FLARE_NAME;
            }
            add_to_plan(instance_item, FLARES_PLAN_RUN_INDEFINITELY, flare, -1);
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
    INSTANCES_LOOP_END()

    const_SLOT(flares_iters_factor, user);
    INSTANCES_LOOP_START()
    const_SLOT(num_plan_items, instance_item);
    const_SLOT(plan, instance_item);
    for (size_t i = 0; i < num_plan_items; i++)
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

#define MY_MARGIN 3
#define TRAILING_CHAR '\n'
static inline bool duplicate_string_while_adding_a_trailing_newline(
    char *const s, const char *const orig_str)
{
    const size_t len = strlen(orig_str);
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
        s[0] = '\n';
        s[1] = '\0';
    }
    return TRUE;
}
#undef TRAILING_CHAR
#undef MY_MARGIN

static inline void recycle_flare(flare_item *const flare)
{
    if (!flare->instance_is_ready)
    {
        recycle_inst(&(flare->obj));
        flare->instance_is_ready = TRUE;
    }
}

static void recycle_instance(
    fcs_user *const user, fcs_instance_item *const instance_item)
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
    instance_item->intract_minimal_flare = NULL;
#endif
}

#ifdef FCS_WITH_MOVES

#ifndef FCS_USE_COMPACT_MOVE_TOKENS
#define internal_move_to_user_move(x) (x)
#define user_move_to_internal_move(x) (x)
#else
static inline fcs_move_t internal_move_to_user_move(
    const fcs_internal_move internal_move)
{
    fcs_move_t user_move;

    /* Convert the internal_move to a user move. */
    fcs_move_set_src_stack(user_move, fcs_int_move_get_src(internal_move));
    fcs_move_set_dest_stack(user_move, fcs_int_move_get_dest(internal_move));
    fcs_move_set_type(user_move, fcs_int_move_get_type(internal_move));
    fcs_move_set_num_cards_in_seq(
        user_move, fcs_int_move_get_num_cards_in_seq(internal_move));

    return user_move;
}
static inline fcs_internal_move user_move_to_internal_move(
    const fcs_move_t user_move)
{
    fcs_internal_move internal_move;

    /* Convert the internal_move to a user move. */
    fcs_int_move_set_src(internal_move, fcs_move_get_src_stack(user_move));
    fcs_int_move_set_dest(internal_move, fcs_move_get_dest_stack(user_move));
    fcs_int_move_set_type(internal_move, fcs_move_get_type(user_move));
    fcs_int_move_set_num_cards_in_seq(
        internal_move, fcs_move_get_num_cards_in_seq(user_move));

    return internal_move;
}
#endif

static inline void calc_moves_seq(const fcs_move_stack *const solution_moves,
    fcs_moves_sequence_t *const moves_seq)
{
    moves_seq->num_moves = 0;
    moves_seq->moves = NULL;

    const_SLOT(num_moves, solution_moves);
    fcs_internal_move *next_move_ptr = solution_moves->moves + num_moves;
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
#endif

#ifdef FCS_WITH_MOVES
static void trace_flare_solution(fcs_user *const user, flare_item *const flare)
{
    if (flare->was_solution_traced)
    {
        return;
    }

    fcs_instance *const instance = &(flare->obj);
    fc_solve_trace_solution(instance);
    flare->trace_solution_state_locs = user->state_locs;
    fc_solve_move_stack_normalize(&(instance->solution_moves), &(user->state),
        &(flare->trace_solution_state_locs)PASS_FREECELLS(
            INSTANCE_FREECELLS_NUM) PASS_STACKS(INSTANCE_STACKS_NUM));

    calc_moves_seq(&(instance->solution_moves), &(flare->moves_seq));
    instance_free_solution_moves(instance);
    flare->next_move_idx = 0;
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
    fcs_user *const user GCC_UNUSED, flare_item *const flare GCC_UNUSED)
{
#ifndef FCS_WITH_MOVES
    return 0;
#else
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
#endif
}
#endif
#ifdef FCS_WITH_NI
#define BUMP_CURR_INST()                                                       \
    user->current_instance++;                                                  \
    continue
#else
#define BUMP_CURR_INST() break
#endif
static inline fc_solve_solve_process_ret_t resume_solution(fcs_user *const user)
{
    fc_solve_solve_process_ret_t ret = FCS_STATE_IS_NOT_SOLVEABLE;

#ifdef FCS_WITH_NI
    const_SLOT(end_of_instances_list, user);
#endif
    /*
     * I expect user->current_instance to be initialized with some value.
     * */
    do
    {
        const_AUTO(instance_item, curr_inst(user));

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
                BUMP_CURR_INST();
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
                SET_ACTIVE_FLARE(user, instance_item->minimal_flare);
                user->init_num_checked_states = OBJ_STATS(user);

                ret = user->ret_code = FCS_STATE_WAS_SOLVED;
                break;
            }
            else
            {
                continue;
            }
        }

        const flare_iters_quota iters_quota =
            current_plan_item->remaining_quota;

        flare_item *const flare = current_plan_item->flare;
#else
        flare_item *const flare = &(instance_item->single_flare);

        if (flare->ret_code == FCS_STATE_WAS_SOLVED)
        {
            user->init_num_checked_states = flare->obj_stats;

            ret = user->ret_code = FCS_STATE_WAS_SOLVED;
            break;
        }
        else if (flare->ret_code == FCS_STATE_IS_NOT_SOLVEABLE)
        {
            recycle_instance(user, instance_item);
            BUMP_CURR_INST();
        }
#endif
        fcs_instance *const instance = &(flare->obj);

        SET_ACTIVE_FLARE(user, flare);
        const bool is_start_of_flare_solving =
            (flare->ret_code == FCS_STATE_NOT_BEGAN_YET);

        if (is_start_of_flare_solving)
        {
            if (!fc_solve_initial_user_state_to_c(user->state_string_copy,
                    &(user->state), INSTANCE_FREECELLS_NUM, INSTANCE_STACKS_NUM,
                    INSTANCE_DECKS_NUM, user->indirect_stacks_buffer))
            {
#ifdef FCS_WITH_ERROR_STRS
                user->state_validity_ret =
                    FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT;
#endif
                return (user->ret_code = FCS_STATE_INVALID_STATE);
            }

#ifndef FCS_DISABLE_STATE_VALIDITY_CHECK
#ifndef FCS_WITH_ERROR_STRS
            fcs_card state_validity_card;
#endif
            if (FCS_STATE_VALIDITY__OK !=
                (
#ifdef FCS_WITH_ERROR_STRS
                    user->state_validity_ret =
#endif
                        fc_solve_check_state_validity(
                            &(user->state)PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
                                PASS_STACKS(INSTANCE_STACKS_NUM)
                                    PASS_DECKS(INSTANCE_DECKS_NUM),
#ifdef FCS_WITH_ERROR_STRS
                            &(user->state_validity_card)
#else
                        &state_validity_card
#endif
                                )))
            {
                return (user->ret_code = FCS_STATE_INVALID_STATE);
            }
#endif
#ifdef FCS_WITH_MOVES
            fc_solve_init_locs(&(user->initial_state_locs));
            user->state_locs = user->initial_state_locs;
            /* running_state and initial_non_canonized_state are
             * normalized states. So We're duplicating
             * state to it before user->state is canonized.
             * */
            FCS_STATE__DUP_keyval_pair(user->running_state, user->state);
#endif
#if defined(FCS_WITH_FLARES) || !defined(FCS_DISABLE_PATSOLVE)
            FCS_STATE__DUP_keyval_pair(
                user->initial_non_canonized_state, user->state);
#endif
#ifdef FCS_WITH_MOVES
            fc_solve_canonize_state_with_locs(&(user->state.s),
                &(user->state_locs)PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
                    PASS_STACKS(INSTANCE_STACKS_NUM));
#else
            fc_solve_canonize_state(&(user->state.s)PASS_FREECELLS(
                INSTANCE_FREECELLS_NUM) PASS_STACKS(INSTANCE_STACKS_NUM));
#endif
            init_instance(instance);
        }

#ifndef FCS_WITHOUT_MAX_NUM_STATES
#ifdef FCS_BREAK_BACKWARD_COMPAT_1
#define local_limit() (-1)
#else
#define local_limit() (instance_item->limit)
#endif
#ifdef FCS_WITH_FLARES
#define NUM_ITERS_LIMITS 3
#else
#define NUM_ITERS_LIMITS 2
#endif
#define NUM_ITERS_LIMITS_MINUS_1 (NUM_ITERS_LIMITS - 1)
        {
            const fcs_int_limit_t limits[NUM_ITERS_LIMITS_MINUS_1] = {
                user->current_iterations_limit
#ifdef FCS_WITH_FLARES
#define PARAMETERIZED_FIXED_LIMIT(increment)                                   \
    (user->iterations_board_started_at.num_checked_states + increment)
#define PARAMETERIZED_LIMIT(increment)                                         \
    (((increment) < 0) ? (-1) : PARAMETERIZED_FIXED_LIMIT(increment))
                ,
                PARAMETERIZED_LIMIT(iters_quota)
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
#endif

        user->init_num_checked_states.num_checked_states =
            instance->i__num_checked_states;
#ifndef FCS_DISABLE_NUM_STORED_STATES
        user->init_num_checked_states.num_states_in_collection =
            instance->num_states_in_collection;
#endif

        if (is_start_of_flare_solving)
        {
            start_process_with_board(instance, &(user->state),
#if defined(FCS_WITH_FLARES) || !defined(FCS_DISABLE_PATSOLVE)
                &(user->initial_non_canonized_state)
#else
                NULL
#endif
            );
        }

        const bool was_run_now =
            ((flare->ret_code == FCS_STATE_SUSPEND_PROCESS) ||
                (flare->ret_code == FCS_STATE_NOT_BEGAN_YET));

        if (was_run_now)
        {
            ret = user->ret_code = flare->ret_code = resume_instance(instance);
            flare->instance_is_ready = FALSE;
        }
#ifdef FCS_WITH_NI
        if (ret != FCS_STATE_SUSPEND_PROCESS)
        {
            user->all_instances_were_suspended = FALSE;
        }
#endif

        flare->obj_stats.num_checked_states = instance->i__num_checked_states;
#ifndef FCS_DISABLE_NUM_STORED_STATES
        flare->obj_stats.num_states_in_collection =
            instance->num_states_in_collection;
#endif
        const_AUTO(delta, flare->obj_stats.num_checked_states -
                              user->init_num_checked_states.num_checked_states);
        user->iterations_board_started_at.num_checked_states += delta;
#ifdef FCS_WITH_FLARES
        if (iters_quota >= 0)
        {
            current_plan_item->remaining_quota =
                normalize_iters_quota(iters_quota - delta);
        }
#endif
#ifndef FCS_DISABLE_NUM_STORED_STATES
        user->iterations_board_started_at.num_states_in_collection +=
            flare->obj_stats.num_states_in_collection -
            user->init_num_checked_states.num_states_in_collection;
#endif
        user->init_num_checked_states = flare->obj_stats;

        if (user->ret_code == FCS_STATE_WAS_SOLVED)
        {
#if defined(FCS_WITH_MOVES) || defined(FCS_WITH_FLARES)
            flare->was_solution_traced = FALSE;
#endif
#ifdef FCS_WITH_FLARES
            if ((!(instance_item->minimal_flare)) ||
                (get_flare_move_count(user, instance_item->minimal_flare) >
                    get_flare_move_count(user, flare)))
            {
                instance_item->minimal_flare = flare;
            }
#endif
            ret = user->ret_code = FCS_STATE_IS_NOT_SOLVEABLE;
        }
        else if (user->ret_code == FCS_STATE_IS_NOT_SOLVEABLE)
        {
            if (was_run_now)
            {
                recycle_inst(instance);
                flare->instance_is_ready = TRUE;
            }
        }
#ifndef FCS_WITHOUT_MAX_NUM_STATES
        else if (user->ret_code == FCS_STATE_SUSPEND_PROCESS)
        {
#ifdef FCS_WITH_FLARES
            instance_item->intract_minimal_flare = flare;
#endif
#if defined(FCS_WITH_MOVES) || defined(FCS_WITH_FLARES)
            flare->was_solution_traced = FALSE;
#endif
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
#ifndef FCS_BREAK_BACKWARD_COMPAT_1
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
                BUMP_CURR_INST();
            }
#endif
#ifdef FCS_WITH_FLARES
            instance_item->all_plan_items_finished_so_far = FALSE;
#else
            flare->ret_code = FCS_STATE_IS_NOT_SOLVEABLE;
#endif
        }
#endif

    } while (
#ifdef FCS_WITH_NI
        (user->current_instance < end_of_instances_list) &&
#endif
        ret == FCS_STATE_IS_NOT_SOLVEABLE);

#ifdef FCS_WITH_NI
    return (
        user->all_instances_were_suspended ? FCS_STATE_SUSPEND_PROCESS : ret);
#else
    return ret;
#endif
}

#ifndef FCS_WITHOUT_EXPORTED_RESUME_SOLUTION
int DLLEXPORT freecell_solver_user_resume_solution(void *const api_instance)
{
    return resume_solution((fcs_user *)api_instance);
}
#endif

int DLLEXPORT freecell_solver_user_solve_board(
    void *const api_instance, const char *const state_as_string)
{
    fcs_user *const user = (fcs_user *)api_instance;

    if (!duplicate_string_while_adding_a_trailing_newline(
            user->state_string_copy, state_as_string))
    {
        return FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT;
    }
#ifdef FCS_WITH_NI
    user->current_instance = user->instances_list;
#endif
#ifndef FCS_FREECELL_ONLY
    {
        FLARES_LOOP_START()
        fc_solve_apply_preset_by_ptr(&(flare->obj), &(user->common_preset));
        INSTANCE_ITEM_FLARES_LOOP_END()
        INSTANCES_LOOP_END()
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
    for (size_t i = 0; i < num_plan_items; i++)
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
#ifdef FCS_WITH_FLARES
static inline flare_item *SINGLE_FLARE(fcs_user *user)
{
    var_AUTO(inst, curr_inst(user));
    return inst->minimal_flare ? inst->minimal_flare
                               : inst->intract_minimal_flare;
}
#else
#define SINGLE_FLARE(user) (&(curr_inst(user)->single_flare))
#endif
static inline flare_item *calc_moves_flare(fcs_user *const user)
{
    flare_item *const flare = SINGLE_FLARE(user);
    trace_flare_solution(user, flare);
    return flare;
}

int DLLEXPORT freecell_solver_user_get_next_move(
    void *const api_instance, fcs_move_t *const user_move)
{
    fcs_user *const user = (fcs_user *)api_instance;

    if (!((user->ret_code == FCS_STATE_WAS_SOLVED) ||
            (user->ret_code == FCS_STATE_SUSPEND_PROCESS)))
    {
        return 1;
    }
    flare_item *const flare = calc_moves_flare(user);
    if (flare->next_move_idx == (uint_fast32_t)flare->moves_seq.num_moves)
    {
        return 1;
    }

#ifndef HARD_CODED_ALL
    var_AUTO(instance, user_obj(user));
#endif

    fc_solve_apply_move(&(user->running_state.s), NULL,
        user_move_to_internal_move(
                *user_move = flare->moves_seq.moves[flare->next_move_idx++])
            PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
                PASS_STACKS(INSTANCE_STACKS_NUM));

    return 0;
}

DLLEXPORT void freecell_solver_user_current_state_stringify(void *api_instance,
    char *const output_string FC_SOLVE__PASS_PARSABLE(int parseable_output),
    int canonized_order_output FC_SOLVE__PASS_T(int display_10_as_t))
{
    fcs_user *const user = (fcs_user *)api_instance;
#ifndef HARD_CODED_ALL
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
#endif

static MYINLINE void user_free_resources(fcs_user *const user)
{
    FLARES_LOOP_START()
    {
        const_SLOT(ret_code, flare);
        fcs_instance *const instance = &(flare->obj);

        if ((ret_code != FCS_STATE_NOT_BEGAN_YET) &&
            (ret_code != FCS_STATE_INVALID_STATE) &&
            (!flare->instance_is_ready))
        {
            fc_solve_finish_instance(instance);
        }
        free_instance(instance);
#ifdef FCS_WITH_FLARES
        flare->name[0] = '\0';
#endif
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

#ifdef FCS_WITH_NI
    free(user->instances_list);
#endif
    fc_solve_meta_compact_allocator_finish(&(user->meta_alloc));
}

void DLLEXPORT freecell_solver_user_free(void *const api_instance)
{
    fcs_user *const user = (fcs_user *)api_instance;
    user_free_resources(user);
    free(user);
}

int DLLEXPORT __attribute__((pure))
freecell_solver_user_get_current_depth(void *const api_instance)
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
        freecell_solver_user_set_solving_method(
            api_instance, FCS_METHOD_SOFT_DFS);
        return;
    }
#endif
    fcs_super_method_type super_method_type = FCS_SUPER_METHOD_BEFS_BRFS;
    fcs_soft_thread *const soft_thread = api_soft_thread(api_instance);
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
                pats_scan, fcs_st_instance(soft_thread));

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

#if !(defined(FCS_BREAK_BACKWARD_COMPAT_1) && defined(FCS_FREECELL_ONLY))
#ifndef HARD_CODED_NUM_FREECELLS

int DLLEXPORT freecell_solver_user_set_num_freecells(
    void *const api_instance, const int freecells_num)
{
    if ((freecells_num < 0) || (freecells_num > MAX_NUM_FREECELLS))
    {
        return 1;
    }

    fcs_user *const user = (fcs_user *)api_instance;
    user->common_preset.game_params.freecells_num = freecells_num;
    apply_game_params_for_all_instances(user);

    return 0;
}

#ifdef FC_SOLVE_JAVASCRIPT_QUERYING
int DLLEXPORT freecell_solver_user_get_num_freecells(void *const api_instance)
{
    return (((fcs_user *const)api_instance)
                ->common_preset.game_params.freecells_num);
}
#endif

#else

int DLLEXPORT __attribute__((const)) freecell_solver_user_set_num_freecells(
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

    fcs_user *const user = (fcs_user *)api_instance;
    user->common_preset.game_params.stacks_num = stacks_num;
    apply_game_params_for_all_instances(user);

    return 0;
}

#ifdef FC_SOLVE_JAVASCRIPT_QUERYING
int DLLEXPORT freecell_solver_user_get_num_stacks(void *const api_instance)
{
    return (
        ((fcs_user *const)api_instance)->common_preset.game_params.stacks_num);
}
#endif

#else

int DLLEXPORT __attribute__((const)) freecell_solver_user_set_num_stacks(
    void *api_instance GCC_UNUSED, int stacks_num GCC_UNUSED)
{
    return 0;
}

#endif

#ifndef HARD_CODED_NUM_DECKS
int DLLEXPORT freecell_solver_user_set_num_decks(
    void *api_instance, int decks_num)
{
    fcs_user *const user = (fcs_user *)api_instance;

    if ((decks_num < 0) || (decks_num > MAX_NUM_DECKS))
    {
        return 1;
    }

    user->common_preset.game_params.decks_num = decks_num;
    apply_game_params_for_all_instances(user);

    return 0;
}

#else

int DLLEXPORT __attribute__((const)) freecell_solver_user_set_num_decks(
    void *api_instance GCC_UNUSED, int decks_num GCC_UNUSED)
{
    return 0;
}

#endif
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
int DLLEXPORT
#if ((!defined(FCS_FREECELL_ONLY)) || (!defined(HARD_CODED_NUM_STACKS)) ||     \
     (!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_DECKS)))
#else
    __attribute__((const))
#endif
freecell_solver_user_set_game(void *const api_instance, const int freecells_num,
    const int stacks_num, const int decks_num,
    const int sequences_are_built_by GCC_UNUSED,
    const int unlimited_sequence_move GCC_UNUSED,
    const int empty_stacks_fill GCC_UNUSED)
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
#ifndef FCS_FREECELL_ONLY
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
#endif
    return 0;
}
#endif

fcs_int_limit_t DLLEXPORT __attribute__((pure))
freecell_solver_user_get_num_times_long(void *api_instance)
{
    fcs_user *const user = (fcs_user *)api_instance;

    return user->iterations_board_started_at.num_checked_states +
           max(OBJ_STATS(user).num_checked_states,
               user_obj(user)->i__num_checked_states) -
           user->init_num_checked_states.num_checked_states;
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
int DLLEXPORT __attribute__((pure))
freecell_solver_user_get_num_times(void *const api_instance)
{
    return (int)freecell_solver_user_get_num_times_long(api_instance);
}

int DLLEXPORT __attribute__((const))
freecell_solver_user_get_limit_iterations(void *const api_instance GCC_UNUSED)
{
#ifndef FCS_WITHOUT_MAX_NUM_STATES
    return active_obj(api_instance)->effective_max_num_checked_states;
#else
    return 0;
#endif
}
#endif

#ifdef FCS_WITH_MOVES
int DLLEXPORT freecell_solver_user_get_moves_left(
    void *const api_instance GCC_UNUSED)
{
    fcs_user *const user = (fcs_user *)api_instance;
    if (user->ret_code == FCS_STATE_WAS_SOLVED)
    {
        const flare_item *const flare = calc_moves_flare(user);
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

DLLEXPORT extern void freecell_solver_user_stringify_move_w_state(
    void *const api_instance, char *const output_string, const fcs_move_t move,
    const int standard_notation)
{
    fcs_user *const user = (fcs_user *)api_instance;

    fc_solve_move_to_string_w_state(
        output_string, &(user->running_state), move, standard_notation);
}
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT char *freecell_solver_user_move_to_string(
    const fcs_move_t move GCC_UNUSED, const int standard_notation GCC_UNUSED)
{
    char *const ret = SMALLOC(ret, 256);
#ifdef FCS_WITH_MOVES
    fc_solve_move_to_string_w_state(
        ret, NULL, move, (standard_notation == 2) ? 1 : standard_notation);
#else
    ret[0] = '\0';
#endif
    return ret;
}

DLLEXPORT char *freecell_solver_user_move_to_string_w_state(
    void *const api_instance GCC_UNUSED, const fcs_move_t move GCC_UNUSED,
    const int standard_notation GCC_UNUSED)
{
    char *ret = SMALLOC(ret, 256);
#ifdef FCS_WITH_MOVES
    freecell_solver_user_stringify_move_w_state(
        api_instance, ret, move, standard_notation);
#else
    ret[0] = '\0';
#endif
    return ret;
}
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
void DLLEXPORT freecell_solver_user_limit_depth(
    void *const api_instance GCC_UNUSED, const int max_depth GCC_UNUSED)
{
}
#endif

#if !(defined(FCS_BREAK_BACKWARD_COMPAT_1) && defined(FCS_FREECELL_ONLY))
int DLLEXPORT __attribute__((const))
freecell_solver_user_get_max_num_freecells(void)
{
    return MAX_NUM_FREECELLS;
}

int DLLEXPORT __attribute__((const))
freecell_solver_user_get_max_num_stacks(void)
{
    return MAX_NUM_STACKS;
}

int DLLEXPORT __attribute__((const))
freecell_solver_user_get_max_num_decks(void)
{
    return MAX_NUM_DECKS;
}
#endif

#ifdef FCS_WITH_ERROR_STRS
void freecell_solver_user_get_invalid_state_error_into_string(
    void *const api_instance, char *const string
#ifndef FC_SOLVE_IMPLICIT_T_RANK
    ,
    const int print_ts GCC_UNUSED
#endif
)
{
    fcs_user *const user = (fcs_user *)api_instance;

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
#ifdef FCS_WITH_MOVES
        fc_solve_card_stringify(
            user->state_validity_card, card_str FC_SOLVE__PASS_T(print_ts));
#else
        card_str[0] = '\0';
#endif

        sprintf(string, "%s%s.",
            ((ret == FCS_STATE_VALIDITY__EXTRA_CARD)
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
    fcs_user *const user = (fcs_user *)api_instance;

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
    fcs_user *const user = (fcs_user *)api_instance;

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

    fcs_user *const user = (fcs_user *const)api_instance;
    user->common_preset.game_params.game_flags &= (~(0x3 << 2));
    user->common_preset.game_params.game_flags |= (empty_stacks_fill << 2);
    apply_game_params_for_all_instances(user);
    return 0;
}
#endif

int DLLEXPORT freecell_solver_user_set_a_star_weight(
    void *const api_instance, const int my_index, const double weight)
{
    fcs_soft_thread *const soft_thread = api_soft_thread(api_instance);
    if ((my_index < 0) ||
        (my_index >=
            (int)(COUNT(
                BEFS_VAR(soft_thread, weighting).befs_weights.weights))))
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
double DLLEXPORT __attribute__((pure)) fc_solve_user_INTERNAL_get_befs_weight(
    void *const api_instance, const int my_index)
{
    return BEFS_VAR(api_soft_thread(api_instance), weighting)
        .befs_weights.weights[my_index];
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
#ifndef FCS_WITHOUT_ITER_HANDLER
void DLLEXPORT freecell_solver_user_set_iter_handler(void *const api_instance,
    const freecell_solver_user_iter_handler_t iter_handler,
    void *const iter_handler_context)
{
    set_any_iter_handler(
        api_instance, NULL, iter_handler, iter_handler_context);
}
#else
void DLLEXPORT freecell_solver_user_set_iter_handler(
    void *const api_instance GCC_UNUSED,
    const freecell_solver_user_iter_handler_t iter_handler GCC_UNUSED,
    void *const iter_handler_context GCC_UNUSED)
{
}
#endif
#endif

#ifdef FCS_WITH_MOVES

#ifdef HARD_CODED_ALL
#define HARD_CODED_UNUSED GCC_UNUSED
#else
#define HARD_CODED_UNUSED
#endif

DLLEXPORT extern void freecell_solver_user_iter_state_stringify(
    void *const api_instance HARD_CODED_UNUSED, char *output_string,
    void *const ptr_state_void FC_SOLVE__PASS_PARSABLE(
        const int parseable_output),
    const int canonized_order_output PASS_T(const int display_10_as_t))
{
#ifndef HARD_CODED_ALL
    fcs_instance *const instance = active_obj(api_instance);
#endif

    const_AUTO(ptr_state, ((const standalone_state_ptrs *const)ptr_state_void));

    fc_solve_state_as_string(output_string, ptr_state->key,
        &(ptr_state->locs)PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
            PASS_STACKS(INSTANCE_STACKS_NUM) PASS_DECKS(INSTANCE_DECKS_NUM)
                FC_SOLVE__PASS_PARSABLE(parseable_output),
        canonized_order_output PASS_T(display_10_as_t));
}
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
DLLEXPORT char *freecell_solver_user_iter_state_as_string(
    void *const api_instance GCC_UNUSED,
    void *const ptr_state_void GCC_UNUSED FC_SOLVE__PASS_PARSABLE(
        const int parseable_output GCC_UNUSED),
    const int canonized_order_output GCC_UNUSED PASS_T(
        const int display_10_as_t GCC_UNUSED))
{
    char *state_as_string = SMALLOC(state_as_string, 1000);
#ifdef FCS_WITH_MOVES
    freecell_solver_user_iter_state_stringify(api_instance, state_as_string,
        ptr_state_void FC_SOLVE__PASS_PARSABLE(parseable_output),
        canonized_order_output PASS_T(display_10_as_t));
#else
    state_as_string[0] = '\0';
#endif
    return state_as_string;
}
#endif

void DLLEXPORT freecell_solver_user_set_random_seed(
    void *const api_instance, const int seed)
{
    DFS_VAR(api_soft_thread(api_instance), rand_seed) = seed;
}

#ifndef FCS_DISABLE_NUM_STORED_STATES
fcs_int_limit_t DLLEXPORT __attribute__((pure))
freecell_solver_user_get_num_states_in_collection_long(void *api_instance)
{
    fcs_user *const user = (fcs_user *)api_instance;

    return user->iterations_board_started_at.num_states_in_collection +
           OBJ_STATS(user).num_states_in_collection -
           user->init_num_checked_states.num_states_in_collection;
}

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
int DLLEXPORT __attribute__((pure))
freecell_solver_user_get_num_states_in_collection(void *const api_instance)
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
    freecell_solver_user_limit_num_states_in_collection_long(
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
    fcs_user *const user = (fcs_user *const)api_instance;
    const_AUTO(
        soft_thread, fc_solve_new_soft_thread(user->soft_thread->hard_thread));
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

#if (!(defined(FCS_SINGLE_HARD_THREAD) && defined(FCS_BREAK_BACKWARD_COMPAT_1)))
int DLLEXPORT freecell_solver_user_next_hard_thread(void *const api_instance)
{
#ifdef FCS_SINGLE_HARD_THREAD
    return freecell_solver_user_next_soft_thread(api_instance);
#else
    fcs_user *const user = (fcs_user *)api_instance;

    fcs_soft_thread *const soft_thread = new_hard_thread(user_obj(user));

    if (soft_thread == NULL)
    {
        return 1;
    }

    user->soft_thread = soft_thread;

    return 0;
#endif
}
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
int DLLEXPORT __attribute__((pure))
freecell_solver_user_get_num_soft_threads_in_instance(void *const api_instance)
{
    return active_obj(api_instance)->next_soft_thread_id;
}
#endif

#ifndef FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE
void DLLEXPORT freecell_solver_user_set_calc_real_depth(
    void *const api_instance, const int calc_real_depth)
{
    STRUCT_SET_FLAG_TO(
        active_obj(api_instance), FCS_RUNTIME_CALC_REAL_DEPTH, calc_real_depth);
}
#endif

#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
void DLLEXPORT freecell_solver_user_set_soft_thread_name(
    void *const api_instance, const freecell_solver_str_t name)
{
    fcs_soft_thread *const soft_thread = api_soft_thread(api_instance);
    strncpy(soft_thread->name, name, COUNT(soft_thread->name));
    LAST(soft_thread->name) = '\0';
}
#endif

#ifdef FCS_WITH_FLARES
void DLLEXPORT freecell_solver_user_set_flare_name(
    void *const api_instance GCC_UNUSED,
    const freecell_solver_str_t name GCC_UNUSED)
{
    flare_item *const flare =
        curr_inst((fcs_user *)api_instance)->end_of_flares - 1;
    strncpy(flare->name, name, COUNT(flare->name));
    LAST(flare->name) = '\0';
}
#endif

#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
int DLLEXPORT freecell_solver_user_set_hard_thread_prelude(
    void *const api_instance, const char *const prelude)
{
    fcs_user *const user = (fcs_user *)api_instance;
    fcs_hard_thread *const hard_thread = user->soft_thread->hard_thread;

    free(HT_FIELD(hard_thread, prelude_as_string));
    HT_FIELD(hard_thread, prelude_as_string) = strdup(prelude);

    return 0;
}
#else
void DLLEXPORT fc_solve_user_set_ht_compiled_prelude(void *const api_instance,
    const size_t num, const fc_solve_prelude_item *const prelude)
{
    fcs_user *const user = (fcs_user *)api_instance;
    fcs_hard_thread *const hard_thread = user->soft_thread->hard_thread;

    HT_FIELD(hard_thread, prelude_num_items) = num;
    HT_FIELD(hard_thread, prelude) = prelude;
}

#endif

#ifdef FCS_WITH_FLARES
int DLLEXPORT freecell_solver_user_set_flares_plan(
    void *const api_instance GCC_UNUSED,
    const char *const flares_plan_string GCC_UNUSED)
{
    fcs_user *const user = (fcs_user *)api_instance;
    const_AUTO(instance_item, curr_inst(user));
    free(instance_item->flares_plan_string);
    instance_item->flares_plan_string =
        (flares_plan_string ? strdup(flares_plan_string) : NULL);
    instance_item->flares_plan_compiled = FALSE;

    return 0;
}
#endif

void DLLEXPORT freecell_solver_user_recycle(void *api_instance)
{
    fcs_user *const user = (fcs_user *)api_instance;

    INSTANCES_LOOP_START()
    recycle_instance(user, instance_item);
    INSTANCES_LOOP_END()
    user->iterations_board_started_at = initial_stats;
}

#ifdef FCS_WITH_MOVES
int DLLEXPORT freecell_solver_user_set_optimization_scan_tests_order(
    void *const api_instance,
    const char *const moves_order FCS__PASS_ERR_STR(char **const error_string))
{
    var_AUTO(obj, active_obj(api_instance));
    moves_order__free(&obj->opt_moves);
    STRUCT_CLEAR_FLAG(obj, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET);
    FCS__DECL_ERR_BUF(static_error_string);
    const int ret = fc_solve_apply_moves_order(
        &(obj->opt_moves), moves_order FCS__PASS_ERR_STR(static_error_string));
    SET_ERROR_VAR(error_string, static_error_string);
    if (!ret)
    {
        STRUCT_TURN_ON_FLAG(obj, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET);
    }
    return ret;
}
#endif

#ifndef FCS_ENABLE_PRUNE__R_TF__UNCOND
extern int DLLEXPORT freecell_solver_user_set_pruning(void *api_instance,
    const char *pruning FCS__PASS_ERR_STR(char **error_string))
{
    fcs_soft_thread *const soft_thread = api_soft_thread(api_instance);

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
#endif

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

#ifdef FCS_WITH_NI
int DLLEXPORT freecell_solver_user_next_instance(void *const api_instance)
{
    user_next_instance((fcs_user *)api_instance);

    return 0;
}
#endif

#ifdef FCS_WITH_FLARES
int DLLEXPORT freecell_solver_user_next_flare(void *const api_instance)
{
    user_next_flare((fcs_user *)api_instance);

    return 0;
}
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
int DLLEXPORT freecell_solver_user_reset(void *const api_instance)
{
    fcs_user *const user = (fcs_user *)api_instance;
    user_free_resources(user);
    user_initialize(user);
    return 0;
}

DLLEXPORT const char *__attribute__((const))
freecell_solver_user_get_lib_version(void *api_instance GCC_UNUSED)
{
    return VERSION;
}
#endif

#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
/* TODO : optionally Remove from the API */
DLLEXPORT __attribute__((pure)) const char *
freecell_solver_user_get_current_soft_thread_name(void *const api_instance)
{
#ifdef FCS_SINGLE_HARD_THREAD
    const fcs_hard_thread *const hard_thread = active_obj(api_instance);
#else
    const fcs_hard_thread *const hard_thread =
        active_obj(api_instance)->current_hard_thread;
#endif

    return HT_FIELD(hard_thread, soft_threads)[HT_FIELD(hard_thread, st_idx)]
        .name;
}
#endif

#ifdef FCS_WITH_ERROR_STRS
DLLEXPORT __attribute__((const)) const char *
freecell_solver_user_get_last_error_string(void *const api_instance)
{
    return (((fcs_user *const)api_instance)->error_string);
}
#endif

#ifndef FCS_BREAK_BACKWARD_COMPAT_1
#ifdef FCS_RCS_STATES
int DLLEXPORT freecell_solver_user_set_cache_limit(
    void *const api_instance, const long limit)
{
    if (limit <= 0)
    {
        return -1;
    }
    fcs_user *const user = (fcs_user *)api_instance;

    FLARES_LOOP_START()
    flare->obj.rcs_states_cache.max_num_elements_in_cache = limit;
    INSTANCE_ITEM_FLARES_LOOP_END()
    INSTANCES_LOOP_END()

    return 0;
}
#endif
#endif

#ifdef FCS_WITH_MOVES
int DLLEXPORT freecell_solver_user_get_moves_sequence(
    void *const api_instance, fcs_moves_sequence_t *const moves_seq)
{
    fcs_user *const user = (fcs_user *)api_instance;
    if (user->ret_code != FCS_STATE_WAS_SOLVED)
    {
        return -2;
    }
    const_AUTO(src_moves_seq, &(SINGLE_FLARE(user)->moves_seq));
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
    fcs_user *const user = (fcs_user *)api_instance;

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
    fcs_user *const user = (fcs_user *)api_instance;

    user->flares_iters_factor = new_factor;
}
#endif

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS

int DLLEXPORT fc_solve_user_INTERNAL_compile_all_flares_plans(
    void *const api_instance GCC_UNUSED, char **const error_string)
{
#ifdef FCS_WITH_FLARES
    fcs_user *const user = (fcs_user *)api_instance;
    const fcs_compile_flares_ret ret = user_compile_all_flares_plans(user);
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

#ifdef FCS_WITH_FLARES
#define FLARES_ATTR __attribute__((const))
#else
#define FLARES_ATTR __attribute__((pure))
#endif
int DLLEXPORT FLARES_ATTR fc_solve_user_INTERNAL_get_flares_plan_num_items(
    void *const api_instance GCC_UNUSED)
{
#ifdef FCS_WITH_FLARES
    return curr_inst((fcs_user *const)api_instance)->num_plan_items;
#else
    return 0;
#endif
}

const DLLEXPORT FLARES_ATTR char *
fc_solve_user_INTERNAL_get_flares_plan_item_type(
    void *const api_instance GCC_UNUSED, const int item_idx GCC_UNUSED)
{
#ifdef FCS_WITH_FLARES
    switch (curr_inst((fcs_user *const)api_instance)->plan[item_idx].type)
    {
    case FLARES_PLAN_RUN_INDEFINITELY:
        return "RunIndef";
    case FLARES_PLAN_RUN_COUNT_ITERS:
        return "Run";
    case FLARES_PLAN_CHECKPOINT:
        return "CP";
#ifndef __clang__
    default:
        __builtin_unreachable();
#endif
    }
#else
    return "";
#endif
}

int DLLEXPORT FLARES_ATTR fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(
    void *const api_instance GCC_UNUSED, const int item_idx GCC_UNUSED)
{
#ifdef FCS_WITH_FLARES
    const_AUTO(instance_item, curr_inst((fcs_user *const)api_instance));
    return instance_item->plan[item_idx].flare - instance_item->flares;
#else
    return 0;
#endif
}

int DLLEXPORT FLARES_ATTR
fc_solve_user_INTERNAL_get_flares_plan_item_iters_count(
    void *const api_instance GCC_UNUSED, const int item_idx GCC_UNUSED)
{
#ifdef FCS_WITH_FLARES
    return curr_inst((fcs_user *const)api_instance)->plan[item_idx].count_iters;
#else
    return 0;
#endif
}

int DLLEXPORT __attribute__((pure))
fc_solve_user_INTERNAL_get_num_by_depth_tests_order(void *const api_instance)
{
    return api_soft_thread(api_instance)->by_depth_moves_order.num;
}

int DLLEXPORT __attribute__((pure))
fc_solve_user_INTERNAL_get_by_depth_tests_max_depth(
    void *const api_instance, const int depth_idx)
{
    return api_soft_thread(api_instance)
        ->by_depth_moves_order.by_depth_moves[depth_idx]
        .max_depth;
}

#endif
