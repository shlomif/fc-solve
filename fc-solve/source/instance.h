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
 * instance.h - header file of fc_solve_instance_t / fc_solve_hard_thread_t /
 * fc_solve_soft_thread_t .
 *
 */

#ifndef FC_SOLVE__INSTANCE_H
#define FC_SOLVE__INSTANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

#include "config.h"
#include "state.h"
#include "move.h"
#include "fcs_enums.h"

#include "rate_state.h"
#include "inline.h"
#include "unused.h"
#include "fcs_limit.h"
#include "likely.h"
#include "count.h"

#include "indirect_buffer.h"
#include "rand.h"
#include "game_type_params.h"

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE) || (defined(INDIRECT_STACK_STATES) && (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE))

#include <redblack.h>

#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE)

#include "fcs_libavl2_state_storage.h"

#endif

#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)

#include "fcs_libavl2_stack_storage.h"

#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH) || (defined(INDIRECT_STACK_STATES) && ((FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE) || (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)))

#include <glib.h>

#endif

#if ((defined(FCS_RCS_STATES) && (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY) || (defined(INDIRECT_STACK_STATES) && (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)))

#include <Judy.h>

#endif


#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)||(FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH))

#include "fcs_hash.h"

#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)

#include "google_hash.h"

#endif

#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)

#include "google_hash.h"

#endif

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE) || (defined(FCS_RCS_STATES) && (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)))

#include "kaz_tree.h"

#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
#include <sys/types.h>
#include <limits.h>
#include <db.h>
#endif

#include "pqueue.h"

#include "meta_alloc.h"

/*
 * This is a linked list item that is used to implement a queue for the BFS
 * scan.
 * */
struct fcs_states_linked_list_item_struct
{
    fcs_collectible_state_t * s;
    struct fcs_states_linked_list_item_struct * next;
};

typedef struct fcs_states_linked_list_item_struct fcs_states_linked_list_item_t;

/*
 * Declare these structures because they will be used within
 * fc_solve_instance, and they will contain a pointer to it.
 * */
struct fc_solve_hard_thread_struct;
struct fc_solve_soft_thread_struct;


typedef void (*fc_solve_solve_for_state_test_t)(
    struct fc_solve_soft_thread_struct *,
    fcs_kv_state_t *,
    fcs_derived_states_list_t *
);

typedef struct fc_solve_hard_thread_struct fc_solve_hard_thread_t;

extern fcs_bool_t fc_solve_check_and_add_state(
    fc_solve_hard_thread_t * const hard_thread,
    fcs_kv_state_t * const new_state_val,
    fcs_kv_state_t * const existing_state_val
    );

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
extern guint fc_solve_hash_function(gconstpointer key);
#endif
#include "move_funcs_maps.h"

/* HT_LOOP == hard threads' loop - macros to abstract it. */
#define HT_LOOP_START() \
    fc_solve_hard_thread_t * hard_thread = instance->hard_threads; \
    fc_solve_hard_thread_t * const end_hard_thread = hard_thread+instance->num_hard_threads; \
    for ( ; \
         hard_thread < end_hard_thread ;  \
         hard_thread++ \
    )

/* ST_LOOP == soft threads' loop - macros to abstract it. */
#define ST_LOOP_START() \
    typeof(hard_thread->soft_threads[0]) * const ht_soft_threads = hard_thread->soft_threads; \
    fc_solve_soft_thread_t * soft_thread = ht_soft_threads; \
    fc_solve_soft_thread_t * const end_soft_thread = ht_soft_threads + hard_thread->num_soft_threads; \
    for ( ; \
         soft_thread < end_soft_thread ;  \
         soft_thread++ \
    )

#define ST_LOOP__WAS_FINISHED() (soft_thread == end_soft_thread)

#define ST_LOOP__GET_INDEX() (soft_thread - ht_soft_threads)

#define TESTS_ORDER_GROW_BY 16

typedef struct
{
    fcs_bool_t should_go_over_stacks;
    double max_sequence_move_factor,
           cards_under_sequences_factor,
           seqs_over_renegade_cards_factor,
           depth_factor,
           num_cards_not_on_parents_factor;

    double num_cards_out_lookup_table[14];
    /*
     * The BeFS weights of the different BeFS tests. Those
     * weights determine the commulative priority of the state.
     * */
    double befs_weights[FCS_NUM_BEFS_WEIGHTS];
} fc_solve_state_weighting_t;

typedef enum
{
    FCS_NO_SHUFFLING,
    FCS_RAND,
    FCS_WEIGHTING,
} fcs_tests_group_type_t;

typedef struct
{
    int num;
    int * tests;
    fcs_tests_group_type_t shuffling_type;
    fc_solve_state_weighting_t weighting;
} fcs_tests_order_group_t;

typedef struct
{
    int num_groups;
    fcs_tests_order_group_t * groups;
} fcs_tests_order_t;

typedef struct
{
    int max_depth;
    fcs_tests_order_t tests_order;
} fcs_by_depth_tests_order_t;

typedef struct
{
    int num;
    fcs_by_depth_tests_order_t * by_depth_tests;
} fcs_by_depth_tests_order_array_t;

typedef fcs_game_limit_t fcs_runtime_flags_t;

#define STRUCT_CLEAR_FLAG(instance, flag) \
    { (instance)->runtime_flags &= ~flag; }

#define STRUCT_TURN_ON_FLAG(instance, flag) \
    { (instance)->runtime_flags |= flag; }

#define STRUCT_QUERY_FLAG(instance, flag) \
    ((instance)->runtime_flags & flag)

#define STRUCT_SET_FLAG_TO(instance, flag, value) \
{ \
    STRUCT_CLEAR_FLAG(instance, flag); \
    if (value) \
    { \
        STRUCT_TURN_ON_FLAG(instance, flag); \
    } \
}

enum
{
    /* A flag that indicates whether to optimize the solution path
       at the end of the scan */
    FCS_RUNTIME_OPTIMIZE_SOLUTION_PATH = (1 << 0),
    /*
     * Specifies that we are now running the optimization thread.
     * */
    FCS_RUNTIME_IN_OPTIMIZATION_THREAD = (1 << 1),
    /*
     * A flag that indicates whether or not to explicitly calculate
     * the depth of a state that was reached.
     * */
    FCS_RUNTIME_CALC_REAL_DEPTH = (1 << 2),
    /*
     * A flag that indicates if instance->opt_tests_order was set.
     */
    FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET = (1 << 3),
    /*
     * This flag indicates whether scans should or should not reparent the
     * states their encounter to a lower depth in the depth tree
     *
     * _proto is the one inputted by the user.
     * _real is calculated based on other factors such as whether the
     * scan method is FCS_METHOD_OPTIMIZE.
     * */
    FCS_RUNTIME_TO_REPARENT_STATES_PROTO = (1 << 4),
    FCS_RUNTIME_TO_REPARENT_STATES_REAL  = (1 << 5),
    /*
     * This variable determines how the scans cooperate with each other.
     *
     * A value of 0 indicates that they don't and only share the same
     * states collection.
     *
     * A value of 1 indicates that they mark states as dead-end,
     * which may help or hinder other scans.
     * */
    FCS_RUNTIME_SCANS_SYNERGY  = (1 << 6),
};

#ifdef FCS_RCS_STATES
struct fcs_cache_key_info_struct
{
    fcs_collectible_state_t * val_ptr;
    fcs_state_t key;
    /* lower_pri and higher_pri form a doubly linked list.
     *
     * pri == priority.
     * */
    struct fcs_cache_key_info_struct * lower_pri, * higher_pri;
};

typedef struct fcs_cache_key_info_struct fcs_cache_key_info_t;

typedef struct
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    Pvoid_t states_values_to_keys_map;
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
    dict_t * kaz_tree;
#else
#error unknown FCS_RCS_CACHE_STORAGE
#endif
    fcs_compact_allocator_t states_values_to_keys_allocator;
    fcs_int_limit_t count_elements_in_cache, max_num_elements_in_cache;

    fcs_cache_key_info_t * lowest_pri, * highest_pri;

    fcs_cache_key_info_t * recycle_bin;
} fcs_lru_cache_t;

#endif

typedef void * fcs_instance_debug_iter_output_context_t;

typedef void (*fcs_instance_debug_iter_output_func_t)(
    fcs_instance_debug_iter_output_context_t,
    fcs_int_limit_t iter_num,
    int depth,
    void * instance,
    fcs_kv_state_t * state,
    fcs_int_limit_t parent_iter_num
);

typedef struct fc_solve_soft_thread_struct fc_solve_soft_thread_t;

struct fc_solve_instance_struct
{
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    /* The sort-margin */
    fcs_standalone_state_ptrs_t indirect_prev_states_margin[PREV_STATES_SORT_MARGIN];

    /* The number of states in the sort margin */
    int num_prev_states_margin;

    /* The sorted cached states, and their number. The maximal
     * size is calculated based on the number.
     * */
    fcs_standalone_state_ptrs_t * indirect_prev_states;
    fcs_int_limit_t num_indirect_prev_states;
#endif

    /*
     * The parameters of the game - see the declaration of
     * fcs_game_type_params_t .
     *
     * */
    fcs_game_type_params_t game_params;
#ifndef FCS_FREECELL_ONLY
    fcs_card_t game_variant_suit_mask;
    fcs_card_t game_variant_desired_suit_value;
#endif

    /* The number of states that were checked by the solving algorithm. */
    fcs_int_limit_t num_checked_states;

    /*
     * Like max_num_checked_states only defaults to MAX_INT if below zero so it will
     * work without checking if it's zero.
     *
     * Normally should be used instead.
     * */
    fcs_int_limit_t effective_max_num_checked_states, effective_max_num_states_in_collection;
    fcs_int_limit_t effective_trim_states_in_collection_from;
    /*
     * tree is the balanced binary tree that is used to store and index
     * the checked states.
     *
     * */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    struct rbtree * tree;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY)
    Pvoid_t judy_array;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE)
    fcs_libavl2_states_tree_table_t * tree;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    GTree * tree;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE)
    dict_t * tree;
#endif

    /*
     * hash is the hash table that is used to store the previous
     * states of the scan.
     * */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    GHashTable * hash;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    fc_solve_hash_t hash;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)
    fcs_states_google_hash_handle_t hash;
#endif

#if defined(INDIRECT_STACK_STATES)
    /*
     * The storage mechanism for the stacks assuming INDIRECT_STACK_STATES is
     * used.
     * */
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH)
    fc_solve_hash_t stacks_hash;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)
    fcs_libavl2_stacks_tree_table_t * stacks_tree;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    struct rbtree * stacks_tree;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
    GTree * stacks_tree;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
    GHashTable * stacks_hash;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)
    fcs_columns_google_hash_handle_t stacks_hash;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)
    Pvoid_t stacks_judy_array;
#else
#error FCS_STACK_STORAGE is not set to a good value.
#endif
#endif

    fcs_collectible_state_t * list_of_vacant_states;
    /*
     * Storing using Berkeley DB is not operational for some reason so
     * pay no attention to it for the while
     * */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    DB * db;
#endif

    fcs_runtime_flags_t runtime_flags;

    /*
     * This is the number of states in the state collection.
     *
     * It gives a rough estimate of the memory occupied by the instance.
     * */
    fcs_int_limit_t active_num_states_in_collection, num_states_in_collection;

    /*
     * A limit to the above.
     * */
    fcs_int_limit_t max_num_states_in_collection;

    int num_hard_threads;
    struct fc_solve_hard_thread_struct * hard_threads;
    /*
     * An iterator over the hard threads.
     * */
    fc_solve_hard_thread_t * current_hard_thread;

    /*
     * This is the master tests order. It is used to initialize all
     * the new Soft-Threads.
     * */
    fcs_tests_order_t instance_tests_order;

    /*
     * This is the hard-thread used for the optimization scan.
     * */
    struct fc_solve_hard_thread_struct * optimization_thread;

    /*
     * A counter that determines how many of the hard threads that belong
     * to this hard thread have already finished. If it becomes num_hard_threads
     * the instance terminates.
     * */
    int num_hard_threads_finished;

    /*
     * The tests order for the optimization scan as specified by the user.
     * */
    fcs_tests_order_t opt_tests_order;

#ifdef FCS_RCS_STATES
    fcs_lru_cache_t rcs_states_cache;

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE))
    fcs_state_t * tree_new_state_key;
    fcs_collectible_state_t * tree_new_state;
#endif

#endif
    /*
     * Limits for the maximal depth and for the maximal number of checked
     * states. max_num_checked_states is useful because it enables the process to
     * stop before it consumes too much memory.
     *
     * max_depth is quite dangerous because it blocks some intermediate moves
     * and doesn't allow a program to fully reach its solution.
     *
     * */
    int max_depth;
    fcs_int_limit_t max_num_checked_states;
    fcs_int_limit_t trim_states_in_collection_from;

    /*
     * The debug_iter_output variables provide a programmer programmable way
     * to debug the algorithm while it is running. This works well for DFS
     * and Soft-DFS scans but at present support for BeFS and BFS is not
     * too good, as its hard to tell which state came from which parent state.
     *
     * debug_iter_output_func is a pointer to the function that performs the
     * debugging. If NULL, this feature is not used.
     *
     * debug_iter_output_context is a user-specified context for it, that
     * may include data that is not included in the instance structure.
     *
     * This feature is used by the "-s" and "-i" flags of fc-solve-debug.
     * */
    fcs_instance_debug_iter_output_func_t debug_iter_output_func;
    fcs_instance_debug_iter_output_context_t debug_iter_output_context;

    /*
     * The next ID to allocate for a soft-thread.
     * */
    int next_soft_thread_id;

    /* This is a place-holder for the initial state */
    fcs_state_keyval_pair_t * state_copy_ptr;

    /* This is the final state that the scan recommends to the
     * interface
     * */
    fcs_collectible_state_t * final_state;


    /*
     * A move stack that contains the moves leading to the solution.
     *
     * It is created only after the solution was found by swallowing
     * all the stacks of each depth.
     * */
    fcs_move_stack_t solution_moves;

    /*
     * The meta allocator - see meta_alloc.h.
     * */
    fcs_meta_compact_allocator_t * meta_alloc;

    /*
     * The soft_thread that solved the state.
     *
     * Needed to trace the patsolve solutions.
     * */
    fc_solve_soft_thread_t * solving_soft_thread;

    /*
     * This is intended to be used by the patsolve scan which is
     * sensitive to the ordering of the columns/stacks. This is an ugly hack
     * but hopefully it will work.
     * */
    fcs_state_keyval_pair_t * initial_non_canonized_state;
};

typedef struct fc_solve_instance_struct fc_solve_instance_t;




/***************************************************/


typedef struct
{
    int scan_idx;
    int quota;
} fcs_prelude_item_t;

struct fc_solve_hard_thread_struct
{
    fc_solve_instance_t * instance;

    struct fc_solve_soft_thread_struct * soft_threads;

    /*
     * The hard thread count of how many states he checked himself. The
     * instance num_checked_states can be confusing because other threads modify it too.
     *
     * Thus, the soft thread switching should be done based on this variable
     * */
    fcs_int_limit_t num_checked_states;

    /*
     * The maximal limit for num_checked_states.
     * */
    fcs_int_limit_t max_num_checked_states;

    fcs_int_limit_t num_checked_states_step;

    /*
     * This is the number of iterations that still have to be done for
     * soft_threads[st_idx]. It is reset to (st_idx+1)->num_checked_states_step
     * when st_idx is incremented.
     * */
    fcs_int_limit_t num_checked_states_left_for_soft_thread;

    /*
     * These variables are used to compute the MD5 checksum of a state
     * that is about to be checked. I decided to make them globals so
     * they won't have to be re-allocated and freed all the time.
     *
     * Notice that it is only used with my internal hash implmentation
     * as GLib requires a dedicated hash function, which cannot
     * access the instance.
     *
     * */

    /*
     * The index for the soft-thread that is currently processed
     * */
    int st_idx;
    /*
     * This is the mechanism used to allocate memory for stacks, states
     * and move stacks.
     * */
    fcs_compact_allocator_t allocator;

    /*
     * This is a move stack that is used and re-used by the
     * tests functions of this hard thread
     * */
    fcs_move_stack_t reusable_move_stack;

#ifdef INDIRECT_STACK_STATES
    /*
     * This is a buffer used to temporarily store the stacks of the duplicated
     * state.
     * */
    dll_ind_buf_t indirect_stacks_buffer;
#endif

    fcs_int_limit_t prelude_num_items;
    int prelude_idx;
    fcs_prelude_item_t * prelude;

    fcs_bool_t allocated_from_list;
    int num_soft_threads;

    /*
     * A counter that determines how many of the soft threads that belong
     * to this hard thread have already finished. If it becomes num_soft_threads
     * this thread is skipped.
     * */
    int num_soft_threads_finished;

    char * prelude_as_string;
};


/********************************************/

typedef struct {
    int idx;
    pq_rating_t rating;
} fcs_rating_with_index_t;

typedef struct
{
    fcs_collectible_state_t * state;
    fcs_derived_states_list_t derived_states_list;
    int current_state_index;
    int tests_list_index;
    int test_index;
    int derived_states_random_indexes_max_size;
    fcs_rating_with_index_t * derived_states_random_indexes;
    char * positions_by_rank;
    fcs_game_limit_t num_vacant_stacks;
    fcs_game_limit_t num_vacant_freecells;
} fcs_soft_dfs_stack_item_t;

enum
{

    /*
     * A flag that indicates if this soft thread has already been
     * initialized.
     * */
    FCS_SOFT_THREAD_INITIALIZED = (1 << 0),

    /*
     * A flag that indicates if this scan contains all the tests that
     * are accessible to all the other scans
     * */
    FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN = (1 << 1),

    /*
     * A flag that indicates if this scan has completed a scan. Used by
     * solve_instance() to skip to the next scan.
     * */
    FCS_SOFT_THREAD_IS_FINISHED = (1 << 2),

};

typedef struct {
    fc_solve_solve_for_state_test_t * tests;
    int num_tests;
    int shuffling_type;
    fc_solve_state_weighting_t weighting;
} fcs_tests_list_t;

typedef struct {
    int num_lists;
    fcs_tests_list_t * lists;
} fcs_tests_list_of_lists;

typedef struct {
    int max_depth;
    fcs_tests_list_of_lists tests;
} fcs_tests_by_depth_unit_t;

typedef struct {
    int num_units;
    fcs_tests_by_depth_unit_t * by_depth_units;
} fcs_tests_by_depth_array_t;

typedef enum
{
    FCS_SUPER_METHOD_DFS,
    FCS_SUPER_METHOD_BEFS_BRFS,
    FCS_SUPER_METHOD_PATSOLVE,
} fcs_super_method_type_t;

struct fc_solve__patsolve_thread_struct;

struct fc_solve_soft_thread_struct
{
    fc_solve_hard_thread_t * hard_thread;

    /*
     * The ID of the soft thread inside the instance.
     * Used for the state-specific flags.
     * */
    int id;

    /*
     * The tests' order indicates which tests (i.e: kinds of multi-moves) to
     * do at what order. This is most relevant to DFS and Soft-DFS.
     *
     * tests_order_num is the number of tests in the test's order. Notice
     * that it can be lower than FCS_TESTS_NUM, thus enabling several tests
     * to be removed completely.
     * */
    fcs_by_depth_tests_order_array_t by_depth_tests_order;


    /*
     * The method (i.e: Soft-DFS, Random-DFS, BFS or BeFS) that is used by this
     * instance.
     *
     * */
    int method;

    /* The super-method type - can be  */
    fcs_super_method_type_t super_method_type;

    fc_solve_seq_cards_power_type_t initial_cards_under_sequences_value;

    struct
    {
        struct
        {
            /*
             * The (temporary) max depth of the Soft-DFS scans)
             * */
            int dfs_max_depth;

            /*
             * These are stacks used by the Soft-DFS for various uses.
             *
             * states_to_check - an array of states to be checked next. Not
             * all of them will be checked because it is possible that future
             * states already visited them.
             *
             * states_to_check_move_stacks - an array of move stacks that
             * lead to those states.
             *
             * num_states_to_check - the size of states_to_check[i]
             *
             * current_state_indexes - the index of the last checked state
             * in depth i.
             *
             * test_indexes - the index of the test that was last
             * performed. FCS performs each test separately, so
             * states_to_check and friends will not be overpopulated.
             *
             * num_vacant_stacks - the number of unoccpied stacks that
             * correspond
             * to solution_states.
             *
             * num_vacant_freecells - ditto for the freecells.
             *
             * */

            fcs_soft_dfs_stack_item_t * soft_dfs_info;

            /* The depth of the DFS stacks */
            int depth;

            /*
             * A pseudo-random number generator for use in the random-DFS scan
             * */
            fcs_rand_t rand_gen;

            /*
             * The initial seed of this random number generator
             * */
            int rand_seed;

            /*
             * The tests to be performed in a preprocessed form.
             * */
            fcs_tests_by_depth_array_t tests_by_depth_array;
        } soft_dfs;
        struct
        {
            char * befs_positions_by_rank;
            fc_solve_solve_for_state_test_t * tests_list, * tests_list_end;
            struct
            {
                struct
                {
                    /*
                     * A linked list that serves as the queue for the BFS scan.
                     * */
                    fcs_states_linked_list_item_t * bfs_queue;
                    /*
                     * The last item in the linked list, so new items can be added at
                     * it, thus making it a queue.
                     * */
                    fcs_states_linked_list_item_t * bfs_queue_last_item;
                    /*
                     * A linked list of items that were freed from
                     * the queue and should be reused before allocating new
                     * items.
                     * */
                    fcs_states_linked_list_item_t * recycle_bin;
                } brfs;
                struct
                {
                    /*
                     * The priority queue of the BeFS scan
                     * */
                    PQUEUE pqueue;
                    fc_solve_state_weighting_t weighting;
                } befs;
            } meth;
        } befs;
    } method_specific;


    /*
     * The first state to be checked by the scan. It is a kind of bootstrap
     * for the algorithm.
     * */
    fcs_collectible_state_t * first_state_to_check;


    fcs_runtime_flags_t runtime_flags;

    /*
     * The number of vacant stacks in the current state - is read from
     * the test functions in freecell.c .
     * */
     fcs_game_limit_t num_vacant_stacks;

    /*
     * The number of vacant freecells in the current state - is read
     * from the test functions in freecell.c .
     * */
    fcs_game_limit_t num_vacant_freecells;

    /*
     * The number of iterations with which to process this scan
     * */
    int num_checked_states_step;

    /*
     * A malloced string that serves as an identification for the user.
     * */
    char * name;

    /*
     * Whether pruning should be done.
     * This variable is temporary - there should be a better pruning
     * abstraction with several optional prunes.
     * */
    fcs_bool_t enable_pruning;

    /*
     * The patsolve soft_thread that is associated with this soft_thread.
     * */
    struct fc_solve__patsolve_thread_struct * pats_scan;
};

#define DFS_VAR(soft_thread,var) (soft_thread)->method_specific.soft_dfs.var
#define BEFS_VAR(soft_thread,var) (soft_thread)->method_specific.befs.meth.befs.var
/* M is Methods-common. */
#define BEFS_M_VAR(soft_thread,var) (soft_thread)->method_specific.befs.var
#define BRFS_VAR(soft_thread,var) (soft_thread)->method_specific.befs.meth.brfs.var

typedef struct fc_solve_soft_thread_struct fc_solve_soft_thread_t;

/*
 * An enum that specifies the meaning of each BeFS weight.
 * */
#define FCS_BEFS_WEIGHT_CARDS_OUT 0
#define FCS_BEFS_WEIGHT_MAX_SEQUENCE_MOVE 1
#define FCS_BEFS_WEIGHT_CARDS_UNDER_SEQUENCES 2
#define FCS_BEFS_WEIGHT_SEQS_OVER_RENEGADE_CARDS 3
#define FCS_BEFS_WEIGHT_DEPTH 4
#define FCS_BEFS_WEIGHT_NUM_CARDS_NOT_ON_PARENTS 5

#include "pat.h"

extern void fc_solve_init_instance(
    fc_solve_instance_t * const instance
);

extern void fc_solve_finish_instance(
    fc_solve_instance_t * const instance
);

extern int fc_solve_befs_or_bfs_do_solve(
    fc_solve_soft_thread_t * const soft_thread
);

extern void fc_solve_increase_dfs_max_depth(
    fc_solve_soft_thread_t * const soft_thread
);

static GCC_INLINE void * memdup(void * src, size_t mysize)
{
    void * dest;

    dest = malloc(mysize);
    if (dest == NULL)
    {
        return NULL;
    }

    memcpy(dest, src, mysize);

    return dest;
}

static GCC_INLINE int update_col_cards_under_sequences(
#ifndef FCS_FREECELL_ONLY
    const int sequences_are_built_by,
#endif
    const fcs_const_cards_column_t col,
    int d /* One less than cards_num of col. */
)
{
    fcs_card_t this_card = fcs_col_get_card(col, d);
    fcs_card_t prev_card = fcs_col_get_card(col, d-1);
    for (; (d > 0) && ({ prev_card=fcs_col_get_card(col, d-1); fcs_is_parent_card(this_card, prev_card); }) ; d--, this_card = prev_card)
    {
    }
    return d;
}

static GCC_INLINE void fc_solve_soft_thread_update_initial_cards_val(
    fc_solve_soft_thread_t * const soft_thread
)
{
    fc_solve_instance_t * const instance = soft_thread->hard_thread->instance;
#ifndef FCS_FREECELL_ONLY
    const int sequences_are_built_by = GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance);
#endif
    fcs_kv_state_t pass;

    pass.key = &(instance->state_copy_ptr->s);
    pass.val = &(instance->state_copy_ptr->info);

    fc_solve_seq_cards_power_type_t cards_under_sequences = 0;
    for (int a = 0 ; a < INSTANCE_STACKS_NUM ; a++)
    {
        const fcs_cards_column_t col = fcs_state_get_col(*pass.key, a);
        cards_under_sequences += FCS_SEQS_OVER_RENEGADE_POWER(update_col_cards_under_sequences(
#ifndef FCS_FREECELL_ONLY
                sequences_are_built_by,
#endif
                col,
                fcs_col_len(col)-1
        )
        );
    }
    soft_thread->initial_cards_under_sequences_value = cards_under_sequences;

    return;
}

#define BEFS_MAX_DEPTH 20000

extern const double fc_solve_default_befs_weights[FCS_NUM_BEFS_WEIGHTS];

#ifdef FCS_FREECELL_ONLY
#define is_filled_by_any_card() 1
#else
#define is_filled_by_any_card() (INSTANCE_EMPTY_STACKS_FILL == FCS_ES_FILLED_BY_ANY_CARD)
#endif
static GCC_INLINE void fc_solve_initialize_befs_rater(
    fc_solve_soft_thread_t * const soft_thread,
    fc_solve_state_weighting_t * weighting
)
{
    double * const befs_weights = weighting->befs_weights;
    double normalized_befs_weights[COUNT(weighting->befs_weights)];

    /* Normalize the BeFS Weights, so the sum of all of them would be 1. */
    double sum = 0;
    for (int i = 0 ; i < FCS_NUM_BEFS_WEIGHTS; i++)
    {
        if (befs_weights[i] < 0)
        {
            befs_weights[i] = fc_solve_default_befs_weights[i];
        }
        sum += befs_weights[i];
    }
    if (sum < 1e-6)
    {
        sum = 1;
    }
    for (int i=0 ; i < FCS_NUM_BEFS_WEIGHTS ; i++)
    {
        normalized_befs_weights[i] = ((befs_weights[i] /= sum) * INT_MAX);
    }
#define pass (*raw_pass_raw)
#define ptr_state_key (raw_pass_raw->key)

#ifndef HARD_CODED_NUM_STACKS
    fc_solve_hard_thread_t * const hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * const instance = hard_thread->instance;
#endif

#if ((!defined(HARD_CODED_NUM_FREECELLS)) || (!defined(HARD_CODED_NUM_STACKS)) || (!defined(HARD_CODED_NUM_DECKS)))
    SET_GAME_PARAMS();
#endif

#ifndef FCS_FREECELL_ONLY
    const fcs_bool_t bool_unlimited_sequence_move = INSTANCE_UNLIMITED_SEQUENCE_MOVE;
#define unlimited_sequence_move bool_unlimited_sequence_move
#else
#define unlimited_sequence_move FALSE
#endif

    const double num_cards_out_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_CARDS_OUT] / (LOCAL_DECKS_NUM*52);

    double out_sum = 0.0;
    for (int i=0 ; i <= 13 ; i++, out_sum += num_cards_out_factor)
    {
        weighting->num_cards_out_lookup_table[i] = out_sum;
    }

    weighting->max_sequence_move_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_MAX_SEQUENCE_MOVE] /
        (is_filled_by_any_card()
         ? (unlimited_sequence_move
            ? (LOCAL_FREECELLS_NUM+INSTANCE_STACKS_NUM)
            : ((LOCAL_FREECELLS_NUM+1)<<(INSTANCE_STACKS_NUM))
         )
         :
           (unlimited_sequence_move
            ? LOCAL_FREECELLS_NUM
            : 1
           )
        );


    weighting->cards_under_sequences_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_CARDS_UNDER_SEQUENCES] / soft_thread->initial_cards_under_sequences_value;

    weighting->seqs_over_renegade_cards_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_SEQS_OVER_RENEGADE_CARDS] / FCS_SEQS_OVER_RENEGADE_POWER(LOCAL_DECKS_NUM*(13*4));

    weighting->depth_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_DEPTH] / BEFS_MAX_DEPTH;

    weighting->num_cards_not_on_parents_factor =
        normalized_befs_weights[FCS_BEFS_WEIGHT_NUM_CARDS_NOT_ON_PARENTS] / (LOCAL_DECKS_NUM * 52);


    weighting->should_go_over_stacks =
    (
        weighting->max_sequence_move_factor
        || weighting->cards_under_sequences_factor
        || weighting->seqs_over_renegade_cards_factor
    );
}

#undef ptr_state_key
#undef pass
#undef unlimited_sequence_move

static GCC_INLINE void fc_solve_soft_thread_init_soft_dfs(
    fc_solve_soft_thread_t * const soft_thread
)
{
    fc_solve_soft_thread_update_initial_cards_val(soft_thread);
    fc_solve_instance_t * const instance = soft_thread->hard_thread->instance;

    /*
        Allocate some space for the states at depth 0.
    */
    DFS_VAR(soft_thread, depth) = 0;

    fc_solve_increase_dfs_max_depth(soft_thread);

    DFS_VAR(soft_thread, soft_dfs_info)[0].state
        = FCS_STATE_keyval_pair_to_collectible(instance->state_copy_ptr);

    fc_solve_rand_init(
            &(DFS_VAR(soft_thread, rand_gen)),
            DFS_VAR(soft_thread, rand_seed)
    );

    if (! DFS_VAR(soft_thread, tests_by_depth_array).by_depth_units)
    {
        const fcs_bool_t master_to_randomize =
            (soft_thread->method == FCS_METHOD_RANDOM_DFS)
            ;

        fcs_tests_by_depth_array_t * const arr_ptr = &(DFS_VAR(soft_thread, tests_by_depth_array));
        arr_ptr->by_depth_units =
            SMALLOC(
                arr_ptr->by_depth_units,
                (arr_ptr->num_units = soft_thread->by_depth_tests_order.num)
            );

        const fcs_by_depth_tests_order_t * const by_depth_tests_order =
            soft_thread->by_depth_tests_order.by_depth_tests;

        for (int depth_idx = 0 ;
            depth_idx < soft_thread->by_depth_tests_order.num ;
            depth_idx++)
        {
            arr_ptr->by_depth_units[depth_idx].max_depth =
                by_depth_tests_order[depth_idx].max_depth;

            fcs_tests_order_group_t * const tests_order_groups
                = by_depth_tests_order[depth_idx].tests_order.groups;

            const int tests_order_num = by_depth_tests_order[depth_idx].tests_order.num_groups;

            fcs_tests_list_of_lists * const tests_list_of_lists =
                &(arr_ptr->by_depth_units[depth_idx].tests);

            tests_list_of_lists->num_lists = 0;
            tests_list_of_lists->lists =
                SMALLOC( tests_list_of_lists->lists, tests_order_num );

            for (int group_idx = 0 ; group_idx < tests_order_num ; group_idx++)
            {
                const int num = tests_order_groups[group_idx].num;
                const int * const tests_order_tests = tests_order_groups[group_idx].tests;
                fc_solve_solve_for_state_test_t * const tests_list = SMALLOC(tests_list, num);
                for (int i = 0; i < num ; i++)
                {
                    tests_list[i] = fc_solve_sfs_tests[ tests_order_tests[i] ];
                }
                /* TODO : convert to C99 struct initializers. */
                fcs_tests_list_t * const tests_list_struct_ptr =
                    &(tests_list_of_lists->lists[tests_list_of_lists->num_lists++])
                    ;

                tests_list_struct_ptr->tests = tests_list;
                tests_list_struct_ptr->num_tests = num;

                tests_list_struct_ptr->shuffling_type =
                    master_to_randomize
                    ? tests_order_groups[group_idx].shuffling_type
                    : FCS_NO_SHUFFLING;

                if (tests_list_struct_ptr->shuffling_type == FCS_WEIGHTING)
                {
                    tests_list_struct_ptr->weighting =
                        tests_order_groups[group_idx].weighting;

                    fc_solve_initialize_befs_rater(
                        soft_thread,
                        &(tests_list_struct_ptr->weighting)
                    );
                }
            }

            tests_list_of_lists->lists =
                SREALLOC(
                        tests_list_of_lists->lists,
                        tests_list_of_lists->num_lists
                );

        }
    }

    return;
}

typedef enum
{
    PRUNE_RET_NOT_FOUND,
    PRUNE_RET_FOLLOW_STATE
} fcs_prune_ret_t;

extern fcs_prune_ret_t fc_solve_sfs_raymond_prune(
    fc_solve_soft_thread_t * const soft_thread,
    fcs_kv_state_t * const raw_ptr_state_raw,
    fcs_collectible_state_t * * const ptr_ptr_next_state
);


#ifdef FCS_RCS_STATES
fcs_state_t * fc_solve_lookup_state_key_from_val(
    fc_solve_instance_t * const instance,
    fcs_collectible_state_t * ptr_state_val
);

extern int fc_solve_compare_lru_cache_keys(
    const void * void_a, const void * void_b, void * param
);

#endif

#include "scans_impl.h"

extern void fc_solve_soft_thread_init_befs_or_bfs(
    fc_solve_soft_thread_t * const soft_thread
);

extern void fc_solve_trace_solution(
    fc_solve_instance_t * const instance
);

extern void fc_solve_instance__init_hard_thread(
    fc_solve_instance_t * const instance,
    fc_solve_hard_thread_t * const hard_thread
);

extern void fc_solve_free_soft_thread_by_depth_test_array(fc_solve_soft_thread_t * const soft_thread);

static GCC_INLINE fcs_tests_order_t tests_order_dup(fcs_tests_order_t * orig)
{
    fcs_tests_order_t ret;

    ret.num_groups = orig->num_groups;

    ret.groups = memdup(orig->groups, sizeof(orig->groups[0]) *
                        ((ret.num_groups & (~(TESTS_ORDER_GROW_BY - 1)))+TESTS_ORDER_GROW_BY)
    );

    for (int i = 0 ; i < ret.num_groups ; i++)
    {
        ret.groups[i].tests = memdup(
            ret.groups[i].tests,
            sizeof(ret.groups[i].tests[0]) *
            ((ret.groups[i].num & (~(TESTS_ORDER_GROW_BY - 1)))+TESTS_ORDER_GROW_BY)
        );
    }

    return ret;
}

extern fc_solve_soft_thread_t * fc_solve_new_soft_thread(
    fc_solve_hard_thread_t * const hard_thread
);

static GCC_INLINE fc_solve_soft_thread_t * fc_solve_new_hard_thread(
    fc_solve_instance_t * const instance
)
{
    fc_solve_hard_thread_t * ret;
    /* Make sure we are not exceeding the maximal number of soft threads
     * for an instance. */
    if (instance->next_soft_thread_id == MAX_NUM_SCANS)
    {
        return NULL;
    }

    instance->hard_threads =
        SREALLOC( instance->hard_threads, instance->num_hard_threads+1 );

    /* Since we SREALLOC()ed the hard_threads, their addresses changed,
     * so we need to update it.
     * */
    HT_LOOP_START()
    {
        ST_LOOP_START()
        {
            soft_thread->hard_thread = hard_thread;
        }
    }

    fc_solve_instance__init_hard_thread(
        instance,
        (ret = &(instance->hard_threads[instance->num_hard_threads]))
    );

    instance->num_hard_threads++;

    return &(ret->soft_threads[0]);
}

/* This is the commmon code from fc_solve_instance__init_hard_thread() and
 * recycle_hard_thread() */
static GCC_INLINE void fc_solve_reset_hard_thread(
    fc_solve_hard_thread_t * const hard_thread
)
{
    hard_thread->num_checked_states = 0;
    hard_thread->max_num_checked_states = INT_MAX;
    hard_thread->num_soft_threads_finished = 0;
}

static GCC_INLINE void fc_solve_reset_soft_thread(
    fc_solve_soft_thread_t * const soft_thread
)
{
    STRUCT_CLEAR_FLAG(soft_thread, FCS_SOFT_THREAD_IS_FINISHED);
    STRUCT_CLEAR_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED);
}


enum
{
    FOREACH_SOFT_THREAD_CLEAN_SOFT_DFS,
    FOREACH_SOFT_THREAD_FREE_INSTANCE,
    FOREACH_SOFT_THREAD_ACCUM_TESTS_ORDER,
    FOREACH_SOFT_THREAD_DETERMINE_SCAN_COMPLETENESS
};
extern void fc_solve_foreach_soft_thread(
    fc_solve_instance_t * const instance,
    int callback_choice,
    void * context
);

/*
    This function is the last function that should be called in the
    sequence of operations on instance, and it is meant for de-allocating
    whatever memory was allocated by alloc_instance().
  */

static GCC_INLINE void fc_solve_free_tests_order(fcs_tests_order_t * tests_order)
{
    for (int group_idx = 0 ; group_idx < tests_order->num_groups ; group_idx++)
    {
        free (tests_order->groups[group_idx].tests);
    }
    free (tests_order->groups);
    tests_order->groups = NULL;
    tests_order->num_groups = 0;
}

/***********************************************************/

#define DECLARE_MOVE_FUNCTION(name) \
extern void name( \
        fc_solve_soft_thread_t * const soft_thread, \
        fcs_kv_state_t * const raw_ptr_state_raw, \
        fcs_derived_states_list_t * const derived_states_list \
)

#ifdef __cplusplus
}
#endif


#endif /* FC_SOLVE__INSTANCE_H */
