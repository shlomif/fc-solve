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

#ifndef FC_SOLVE__FCS_H
#define FC_SOLVE__FCS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "state.h"
#include "move.h"
#include "fcs_enums.h"

#include "inline.h"
#include "unused.h"

#include "rand.h"

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

#include "alloc.h"

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
 * Conventions for use of the tests' order flags:
 * A test that should be scanned sequentially should have both flags cleared.
 * The first test in its random group should have both flags set. All the
 * other tests in the group should contain the FLAG_RANDOM flag.
 *
 * For instance: 123(45)(67)8 translates into:
 * 1 , 2, 3, 4|RANDOM|START_RANDOM_GROUP, 5|RANDOM,
 * 6|RANDOM_START_RANDOM_GROUP, 7|RANDOM, 8
 *
 * */
enum
{
    FCS_TEST_ORDER_NO_FLAGS_MASK = 0xFFFFFF,
    FCS_TEST_ORDER_FLAG_RANDOM = 0x1000000,
    FCS_TEST_ORDER_FLAG_START_RANDOM_GROUP = 0x2000000
};

/*
 * Declare these structures because they will be used within
 * fc_solve_instance, and they will contain a pointer to it.
 * */
struct fc_solve_hard_thread_struct;
struct fc_solve_soft_thread_struct;


typedef void (*fc_solve_solve_for_state_test_t)(
        struct fc_solve_soft_thread_struct *,
#ifdef FCS_RCS_STATES
        fcs_state_t *,
#endif
        fcs_collectible_state_t *,
        fcs_derived_states_list_t *
        );

typedef struct fc_solve_hard_thread_struct fc_solve_hard_thread_t;

#include "move_funcs_maps.h"

/* HT_LOOP == hard threads' loop - macros to abstract it. */
#define HT_LOOP_DECLARE_VARS() \
    fc_solve_hard_thread_t * hard_thread, * end_hard_thread

#define HT_LOOP_START() \
    for ( end_hard_thread = ( \
              (hard_thread = instance->hard_threads) \
              + instance->num_hard_threads \
          ) ; \
         hard_thread < end_hard_thread ;  \
         hard_thread++ \
         )

/* ST_LOOP == soft threads' loop - macros to abstract it. */
#define ST_LOOP_DECLARE_VARS() \
    fc_solve_soft_thread_t * soft_thread, * end_soft_thread

#define ST_LOOP_START() \
    for ( end_soft_thread = ( \
              (soft_thread = hard_thread->soft_threads) \
              + hard_thread->num_soft_threads \
          ) ; \
         soft_thread < end_soft_thread ;  \
         soft_thread++ \
         )

#define ST_LOOP_FINISHED() (soft_thread == end_soft_thread)

#define ST_LOOP_INDEX() (soft_thread - hard_thread->soft_threads)

#define TESTS_ORDER_GROW_BY 16
typedef struct
{
    int num;
    int * tests;
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

typedef struct {
    /*
     * The number of Freecells, Stacks and Foundations present in the game.
     *
     * freecells_num and stacks_num are variable and may be specified at
     * the beginning of the execution of the algorithm. However, there
     * is a maximal limit to them which is set in config.h.
     *
     * decks_num can be 1 or 2 .
     * */

#define DECLARE_GAME_PARAMS() \
    fcs_game_type_params_t game_params

#define SET_INSTANCE_GAME_PARAMS(inst) \
    game_params = (inst)->game_params

#define SET_GAME_PARAMS() \
    SET_INSTANCE_GAME_PARAMS(instance)

#define DECLARE_AND_SET_GAME_PARAMS() \
    fcs_game_type_params_t game_params = instance->game_params

#ifndef HARD_CODED_NUM_FREECELLS
    fcs_game_limit_t freecells_num;
#define INSTANCE_FREECELLS_NUM (instance->game_params.freecells_num)
#define LOCAL_FREECELLS_NUM (game_params.freecells_num)
#else
#define INSTANCE_FREECELLS_NUM HARD_CODED_NUM_FREECELLS
#define LOCAL_FREECELLS_NUM HARD_CODED_NUM_FREECELLS
#endif

#ifndef HARD_CODED_NUM_STACKS
    fcs_game_limit_t stacks_num;
#define INSTANCE_STACKS_NUM (instance->game_params.stacks_num)
#define LOCAL_STACKS_NUM (game_params.stacks_num)
#else
#define INSTANCE_STACKS_NUM HARD_CODED_NUM_STACKS
#define LOCAL_STACKS_NUM HARD_CODED_NUM_STACKS
#endif

#ifndef HARD_CODED_NUM_DECKS
    fcs_game_limit_t decks_num;
#define INSTANCE_DECKS_NUM (instance->game_params.decks_num)
#define LOCAL_DECKS_NUM (game_params.decks_num)
#else
#define INSTANCE_DECKS_NUM HARD_CODED_NUM_DECKS
#define LOCAL_DECKS_NUM HARD_CODED_NUM_DECKS
#endif

#ifndef FCS_FREECELL_ONLY
    /* sequences_are_built_by - (bits 0:1) - what two adjacent cards in the
     * same sequence can be.
     *
     * empty_stacks_fill (bits 2:3) - with what cards can empty stacks be
     * filled with.
     *
     * unlimited_sequence_move - (bit 4) - whether an entire sequence can be
     * moved from one place to the other regardless of the number of
     * unoccupied Freecells there are.
     * */
    fcs_game_limit_t game_flags;

#define INSTANCE_GAME_FLAGS  (instance->game_params.game_flags)
#define GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance) \
    ((instance)->game_params.game_flags & 0x3)

#define INSTANCE_UNLIMITED_SEQUENCE_MOVE  (INSTANCE_GAME_FLAGS & (1 << 4))
#define INSTANCE_EMPTY_STACKS_FILL   ((INSTANCE_GAME_FLAGS >> 2) & 0x3)

#endif

} fcs_game_type_params_t;

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
    long count_elements_in_cache, max_num_elements_in_cache;

    fcs_cache_key_info_t * lowest_pri, * highest_pri;

    fcs_cache_key_info_t * recycle_bin;
} fcs_lru_cache_t;

#endif

typedef void * fcs_instance_debug_iter_output_context_t;

typedef void (*fcs_instance_debug_iter_output_func_t)(
        fcs_instance_debug_iter_output_context_t,
        int iter_num,
        int depth,
        void * instance,
#ifdef FCS_RCS_STATES
        fcs_state_t * state_key,
#endif
        fcs_collectible_state_t * state_val,
        int parent_iter_num
        );


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
    int num_indirect_prev_states;
#endif

    /*
     * The parameters of the game - see the declaration of
     * fcs_game_type_params_t .
     *
     * */
    fcs_game_type_params_t game_params;

    /* The number of states that were checked by the solving algorithm.
     * Badly named, should be renamed to num_iters or num_checked_states */
    int num_times;

    /*
     * Like max_num_times only defaults to MAX_INT if below zero so it will
     * work without checking if it's zero.
     *
     * Normally should be used instead.
     * */
    int effective_max_num_times, effective_max_num_states_in_collection;
    long effective_trim_states_in_collection_from;
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
#ifdef FCS_WITH_TALONS
    /*
     * The talon for Gypsy-like games. Since only the position changes from
     * state to state.
     * We can keep it here.
     *
     * */
    fcs_card_t * gypsy_talon;

    /*
     * The length of the Gypsy talon
     * */
    int gypsy_talon_len;

    int talon_type;

    /* The Klondike Talons' Cache */
    fc_solve_hash_t * talons_hash;

#endif

    fcs_runtime_flags_t runtime_flags;

    /* This is a place-holder for the initial state */
    fcs_state_keyval_pair_t * state_copy_ptr;

    /* This is the final state that the scan recommends to the
     * interface
     * */
    fcs_collectible_state_t * final_state;

    /*
     * This is the number of states in the state collection.
     *
     * It gives a rough estimate of the memory occupied by the instance.
     * */
    int active_num_states_in_collection, num_states_in_collection;

    /*
     * A limit to the above.
     * */
    int max_num_states_in_collection;

    int num_hard_threads;
    struct fc_solve_hard_thread_struct * hard_threads;
    /*
     * A persistent counters that os used to iterate over the
     * threads one by one
     * */
    int ht_idx;

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
     * states. max_num_times is useful because it enables the process to
     * stop before it consumes too much memory.
     *
     * max_depth is quite dangerous because it blocks some intermediate moves
     * and doesn't allow a program to fully reach its solution.
     *
     * */
    int max_depth;
    int max_num_times;
    long trim_states_in_collection_from;

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

    /*
     * A move stack that contains the moves leading to the solution.
     *
     * It is created only after the solution was found by swallowing
     * all the stacks of each depth.
     * */
    fcs_move_stack_t solution_moves;

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

    int num_soft_threads;
    struct fc_solve_soft_thread_struct * soft_threads;

    /*
     * The hard thread count of how many states he checked himself. The
     * instance num_times can be confusing because other threads modify it too.
     *
     * Thus, the soft thread switching should be done based on this variable
     * */
    int num_times;

    /*
     * The maximal limit for this variable.
     * */
    int max_num_times;

    /*
     * The Hard-Thread's global limit for the number of iterations
     * to process
     * */
    int ht_max_num_times;

    int num_times_step;

    /*
     * This is the number of iterations that still have to be done for
     * soft_threads[st_idx]. It is reset to (st_idx+1)->num_times_step
     * when st_idx is incremented.
     * */
    int num_times_left_for_soft_thread;

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
     * A counter that determines how many of the soft threads that belong
     * to this hard thread have already finished. If it becomes num_soft_threads
     * this thread is skipped.
     * */
    int num_soft_threads_finished;

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
    char indirect_stacks_buffer[MAX_NUM_STACKS << 7];
#else
    char indirect_stacks_buffer[1];
#endif

    char * prelude_as_string;

    int prelude_num_items;
    int prelude_idx;
    fcs_prelude_item_t * prelude;

    fcs_bool_t allocated_from_list;
};


/********************************************/


typedef struct
{
    fcs_collectible_state_t * state;
    fcs_derived_states_list_t derived_states_list;
    int current_state_index;
    int tests_list_index;
    int test_index;
    int derived_states_random_indexes_max_size;
    int * derived_states_random_indexes;
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
    int to_randomize;
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
     * The method (i.e: DFS, Soft-DFS, BFS or BeFS) that is used by this
     * instance.
     *
     * */
    int method;

    union
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
            union
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
                    double initial_cards_under_sequences_value;

                    /*
                     * The BeFS weights of the different BeFS tests. Those
                     * weights determine the commulative priority of the state.
                     * */
                    double befs_weights[5];
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
    int num_times_step;

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
};

typedef struct fc_solve_soft_thread_struct fc_solve_soft_thread_t;

#define FC_SOLVE_IS_DFS(soft_thread) \
    ((soft_thread->method == FCS_METHOD_SOFT_DFS) ||  \
     (soft_thread->method == FCS_METHOD_RANDOM_DFS) \
    )

/*
 * An enum that specifies the meaning of each BeFS weight.
 * */
#define FCS_BEFS_WEIGHT_CARDS_OUT 0
#define FCS_BEFS_WEIGHT_MAX_SEQUENCE_MOVE 1
#define FCS_BEFS_WEIGHT_CARDS_UNDER_SEQUENCES 2
#define FCS_BEFS_WEIGHT_SEQS_OVER_RENEGADE_CARDS 3
#define FCS_BEFS_WEIGHT_DEPTH 4

fc_solve_instance_t * fc_solve_alloc_instance(void);

extern void fc_solve_init_instance(
    fc_solve_instance_t * instance
    );

extern void fc_solve_free_instance(
    fc_solve_instance_t * instance
    );

extern void fc_solve_finish_instance(
    fc_solve_instance_t * instance
    );

extern void fc_solve_start_instance_process_with_board(
    fc_solve_instance_t * instance,
    fcs_state_keyval_pair_t * init_state_val
    );

extern int fc_solve_befs_or_bfs_do_solve(
    fc_solve_soft_thread_t * soft_thread
    );

extern void fc_solve_increase_dfs_max_depth(
    fc_solve_soft_thread_t * soft_thread
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


static GCC_INLINE void fc_solve_soft_thread_init_soft_dfs(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_instance_t * instance = soft_thread->hard_thread->instance;

    fcs_state_keyval_pair_t * ptr_orig_state = instance->state_copy_ptr;
    /*
        Allocate some space for the states at depth 0.
    */
    soft_thread->method_specific.soft_dfs.depth = 0;

    fc_solve_increase_dfs_max_depth(soft_thread);

#ifdef FCS_RCS_STATES
    soft_thread->method_specific.soft_dfs.soft_dfs_info[0].state = &(ptr_orig_state->info);
#else
    soft_thread->method_specific.soft_dfs.soft_dfs_info[0].state = ptr_orig_state;
#endif
    fc_solve_rand_init(
            &(soft_thread->method_specific.soft_dfs.rand_gen),
            soft_thread->method_specific.soft_dfs.rand_seed
    );

    if (! soft_thread->method_specific.soft_dfs.tests_by_depth_array.by_depth_units)
    {
        fcs_tests_list_of_lists * tests_list_of_lists;
        fc_solve_solve_for_state_test_t * tests_list, * next_test;
        fcs_tests_list_t * tests_list_struct_ptr;
        fcs_tests_by_depth_array_t * arr_ptr;

        int tests_order_num;
        int * tests_order_tests;
        int start_i;
        fcs_bool_t master_to_randomize =
            (soft_thread->method == FCS_METHOD_RANDOM_DFS)
            ;
        fcs_bool_t do_first_iteration;
        int depth_idx;
        fcs_by_depth_tests_order_t * by_depth_tests_order;

        arr_ptr = &(soft_thread->method_specific.soft_dfs.tests_by_depth_array);
        arr_ptr->by_depth_units =
            malloc(
                sizeof(arr_ptr->by_depth_units[0])
                * (arr_ptr->num_units = soft_thread->by_depth_tests_order.num)
            );

        by_depth_tests_order =
            soft_thread->by_depth_tests_order.by_depth_tests;

        for (depth_idx = 0 ;
            depth_idx < soft_thread->by_depth_tests_order.num ;
            depth_idx++)
        {
            arr_ptr->by_depth_units[depth_idx].max_depth =
                by_depth_tests_order[depth_idx].max_depth;

            tests_order_tests = by_depth_tests_order[depth_idx].tests_order.tests;

            tests_order_num = by_depth_tests_order[depth_idx].tests_order.num;

            tests_list_of_lists =
                &(arr_ptr->by_depth_units[depth_idx].tests);

            tests_list_of_lists->num_lists = 0;
            tests_list_of_lists->lists =
                malloc(
                        sizeof(tests_list_of_lists->lists[0]) * tests_order_num
                      );

            tests_list = malloc(sizeof(tests_list[0]) * tests_order_num);

            start_i = 0;
            while (start_i < tests_order_num)
            {
                int i;

                do_first_iteration = TRUE;

                for (i = start_i, next_test = tests_list ;
                        (i < tests_order_num) &&
                        (do_first_iteration ||
                         ((!master_to_randomize) ||
                          (
                           /* We are still on a random group */
                           (tests_order_tests[ i ] & FCS_TEST_ORDER_FLAG_RANDOM) &&
                           /* A new random group did not start */
                           (! (tests_order_tests[ i ] & FCS_TEST_ORDER_FLAG_START_RANDOM_GROUP))

                          )
                         )
                        )
                        ;
                        i++)
                {
                    do_first_iteration = FALSE;
                    *(next_test++) =
                        fc_solve_sfs_tests[
                        tests_order_tests[i] & FCS_TEST_ORDER_NO_FLAGS_MASK
                        ];
                }

                        tests_list_struct_ptr =
                            &(tests_list_of_lists->lists[tests_list_of_lists->num_lists++])
                            ;

                        ;
                        tests_list_struct_ptr->tests =
                            memdup(tests_list,
                                    sizeof(tests_list[0])
                                    * (tests_list_struct_ptr->num_tests = i-start_i)
                                  );

                        tests_list_struct_ptr->to_randomize =
                            master_to_randomize &&
                            (i > start_i) &&
                            (tests_order_tests[ i-1 ] & FCS_TEST_ORDER_FLAG_RANDOM)
                            ;

                        start_i = i;
            }

            tests_list_of_lists->lists =
                realloc(
                        tests_list_of_lists->lists,
                        sizeof(tests_list_of_lists->lists[0]) *
                        tests_list_of_lists->num_lists
                       );

            free(tests_list);
        }
    }

    return;
}

extern int fc_solve_soft_dfs_do_solve(fc_solve_soft_thread_t * soft_thread);

extern void fc_solve_soft_thread_init_befs_or_bfs(
    fc_solve_soft_thread_t * soft_thread
    );


static GCC_INLINE int run_hard_thread(fc_solve_hard_thread_t * hard_thread)
{
    fc_solve_soft_thread_t * soft_thread;
    int num_times_started_at;
    int ret;
    fc_solve_instance_t * instance = hard_thread->instance;
    /*
     * Again, making sure that not all of the soft_threads in this
     * hard thread are finished.
     * */

    ret = FCS_STATE_SUSPEND_PROCESS;
    while(hard_thread->num_soft_threads_finished < hard_thread->num_soft_threads)
    {
        soft_thread = &(hard_thread->soft_threads[hard_thread->st_idx]);
        /*
         * Move to the next thread if it's already finished
         * */
        if (STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_IS_FINISHED))
        {
            /*
             * Hmmpf - duplicate code. That's ANSI C for you.
             * A macro, anyone?
             * */

#define switch_to_next_soft_thread() \
            /*      \
             * Switch to the next soft thread in the hard thread,   \
             * since we are going to call continue and this is     \
             * a while loop     \
                             * */    \
            if ((hard_thread->prelude != NULL) &&    \
                (hard_thread->prelude_idx < hard_thread->prelude_num_items))   \
            {      \
                hard_thread->st_idx = hard_thread->prelude[hard_thread->prelude_idx].scan_idx; \
                hard_thread->num_times_left_for_soft_thread = hard_thread->prelude[hard_thread->prelude_idx].quota; \
                hard_thread->prelude_idx++; \
            }    \
            else       \
            {       \
                hard_thread->st_idx++;      \
                if (hard_thread->st_idx == hard_thread->num_soft_threads)     \
                {       \
                    hard_thread->st_idx = 0;    \
                }      \
                hard_thread->num_times_left_for_soft_thread = hard_thread->soft_threads[hard_thread->st_idx].num_times_step;  \
            }



            switch_to_next_soft_thread();

            continue;
        }

        /*
         * Keep record of the number of iterations since this
         * thread started.
         * */
        num_times_started_at = hard_thread->num_times;
        /*
         * Calculate a soft thread-wise limit for this hard
         * thread to run.
         * */
        hard_thread->max_num_times = hard_thread->num_times + hard_thread->num_times_left_for_soft_thread;



        /*
         * Call the resume or solving function that is specific
         * to each scan
         *
         * This switch-like construct calls for declaring a class
         * that will abstract a scan. But it's not critical since
         * I don't support user-defined scans.
         * */
        switch(soft_thread->method)
        {
            case FCS_METHOD_SOFT_DFS:
            case FCS_METHOD_HARD_DFS:
            case FCS_METHOD_RANDOM_DFS:

            if (! STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED))
            {
                fc_solve_soft_thread_init_soft_dfs(soft_thread);
                STRUCT_TURN_ON_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED);
            }

            ret = fc_solve_soft_dfs_do_solve(soft_thread);

            break;

            case FCS_METHOD_BFS:
            case FCS_METHOD_A_STAR:
            case FCS_METHOD_OPTIMIZE:

            if (! STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED))
            {
                fc_solve_soft_thread_init_befs_or_bfs(soft_thread);

                STRUCT_TURN_ON_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED);
            }

            ret = fc_solve_befs_or_bfs_do_solve(soft_thread);

            break;

            default:
            ret = FCS_STATE_IS_NOT_SOLVEABLE;
            break;
        }
        /*
         * Determine how much iterations we still have left
         * */
        hard_thread->num_times_left_for_soft_thread -= (hard_thread->num_times - num_times_started_at);

        /*
         * I use <= instead of == because it is possible that
         * there will be a few more iterations than what this
         * thread was allocated, due to the fact that
         * check_and_add_state is only called by the test
         * functions.
         *
         * It's a kludge, but it works.
         * */
        if (hard_thread->num_times_left_for_soft_thread <= 0)
        {
            switch_to_next_soft_thread();
            /*
             * Reset num_times_left_for_soft_thread
             * */

        }
#undef switch_to_next_soft_thread

        /*
         * It this thread indicated that the scan was finished,
         * disable the thread or even stop searching altogether.
         * */
        if (ret == FCS_STATE_IS_NOT_SOLVEABLE)
        {
            STRUCT_TURN_ON_FLAG(soft_thread, FCS_SOFT_THREAD_IS_FINISHED);
            hard_thread->num_soft_threads_finished++;
            if (hard_thread->num_soft_threads_finished == hard_thread->num_soft_threads)
            {
                instance->num_hard_threads_finished++;
            }
            /*
             * Check if this thread is a complete scan and if so,
             * terminate the search. Note that if the scans synergy is set,
             * then we may still need to continue running the other threads
             * which may have blocked some positions / states in the graph.
             * */
            if (STRUCT_QUERY_FLAG(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN) &&
                    (! STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_SCANS_SYNERGY))
               )
            {
                return FCS_STATE_IS_NOT_SOLVEABLE;
            }
            else
            {
                /*
                 * Else, make sure ret is something more sensible
                 * */
                ret = FCS_STATE_SUSPEND_PROCESS;
            }
        }

        if ((ret == FCS_STATE_WAS_SOLVED) ||
            (
                (ret == FCS_STATE_SUSPEND_PROCESS) &&
                /* There's a limit to the scan only
                 * if max_num_times is greater than 0 */
                (
                    (
                        (instance->num_times >= instance->effective_max_num_times)
                    ) ||
                    (instance->num_states_in_collection >=
                        instance->effective_max_num_states_in_collection
                    )
                )
            )
           )
        {
            return ret;
        }
        else if ((ret == FCS_STATE_SUSPEND_PROCESS) &&
            (hard_thread->num_times >= hard_thread->ht_max_num_times))
        {
            hard_thread->ht_max_num_times += hard_thread->num_times_step;
            break;
        }
    }

    return ret;
}

extern void fc_solve_trace_solution(
    fc_solve_instance_t * instance
    );

extern void fc_solve_instance__init_hard_thread(
        fc_solve_instance_t * instance,
        fc_solve_hard_thread_t * hard_thread
        );

extern void fc_solve_free_soft_thread_by_depth_test_array(fc_solve_soft_thread_t * soft_thread);

static GCC_INLINE fcs_tests_order_t tests_order_dup(fcs_tests_order_t * orig)
{
    fcs_tests_order_t ret;

    ret.num = orig->num;

    ret.tests =
        malloc(sizeof(ret.tests[0]) *
            ((ret.num & (~(TESTS_ORDER_GROW_BY - 1)))+TESTS_ORDER_GROW_BY)
        );
    memcpy(ret.tests, orig->tests, sizeof(ret.tests[0]) * ret.num);

    return ret;
}
/*
    This function optimizes the solution path using a BFS scan on the
    states in the solution path.
*/
static GCC_INLINE int fc_solve_optimize_solution(
    fc_solve_instance_t * instance
    )
{
    fc_solve_soft_thread_t * soft_thread;
    fc_solve_hard_thread_t * optimization_thread;

    STRUCT_TURN_ON_FLAG(instance, FCS_RUNTIME_TO_REPARENT_STATES_REAL);

    if (! instance->optimization_thread)
    {
        instance->optimization_thread =
            optimization_thread =
            malloc(sizeof(*optimization_thread));

        fc_solve_instance__init_hard_thread(instance, optimization_thread);

    }
    else
    {
        optimization_thread = instance->optimization_thread;
    }

    soft_thread = optimization_thread->soft_threads;

    if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET))
    {
        if (soft_thread->by_depth_tests_order.by_depth_tests != NULL)
        {
            fc_solve_free_soft_thread_by_depth_test_array(soft_thread);
        }

        soft_thread->by_depth_tests_order.num = 1;
        soft_thread->by_depth_tests_order.by_depth_tests =
            malloc(sizeof(soft_thread->by_depth_tests_order.by_depth_tests[0]));

        soft_thread->by_depth_tests_order.by_depth_tests[0].max_depth = INT_MAX;
        soft_thread->by_depth_tests_order.by_depth_tests[0].tests_order =
            tests_order_dup(&(instance->opt_tests_order));
    }

    soft_thread->method = FCS_METHOD_OPTIMIZE;

    STRUCT_TURN_ON_FLAG(soft_thread, FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN);

    /* Initialize the optimization hard-thread and soft-thread */
    optimization_thread->num_times_left_for_soft_thread = 1000000;

    /* Instruct the optimization hard thread to run indefinitely AFA it
     * is concerned */
    optimization_thread->max_num_times = INT_MAX;
    optimization_thread->ht_max_num_times = INT_MAX;

    fc_solve_soft_thread_init_befs_or_bfs(soft_thread);
    STRUCT_TURN_ON_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED);

    STRUCT_TURN_ON_FLAG(instance, FCS_RUNTIME_IN_OPTIMIZATION_THREAD);

    return
        fc_solve_befs_or_bfs_do_solve(
            soft_thread
            );
}
/* Resume a solution process that was stopped in the middle */
static GCC_INLINE int fc_solve_resume_instance(
    fc_solve_instance_t * instance
    )
{
    int ret = FCS_STATE_SUSPEND_PROCESS;
    fc_solve_hard_thread_t * hard_thread;

    /*
     * If the optimization thread is defined, it means we are in the
     * optimization phase of the total scan. In that case, just call
     * its scanning function.
     *
     * Else, proceed with the normal total scan.
     * */
    if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_IN_OPTIMIZATION_THREAD))
    {
        ret =
            fc_solve_befs_or_bfs_do_solve(
                &(instance->optimization_thread->soft_threads[0])
                );
    }
    else
    {
        /*
         * instance->num_hard_threads_finished signals to us that
         * all the incomplete soft threads terminated. It is necessary
         * in case the scan only contains incomplete threads.
         *
         * I.e: 01235 and 01246, where no thread contains all tests.
         * */
        while(instance->num_hard_threads_finished < instance->num_hard_threads)
        {
            /*
             * A loop on the hard threads.
             * Note that we do not initialize instance->ht_idx because:
             * 1. It is initialized before the first call to this function.
             * 2. It is reset to zero below.
             * */
            for(;
                instance->ht_idx < instance->num_hard_threads ;
                    instance->ht_idx++)
            {
                hard_thread = &(instance->hard_threads[instance->ht_idx]);

                ret = run_hard_thread(hard_thread);
                if ((ret == FCS_STATE_IS_NOT_SOLVEABLE) ||
                    (ret == FCS_STATE_WAS_SOLVED) ||
                    (
                        (ret == FCS_STATE_SUSPEND_PROCESS) &&
                        /* There's a limit to the scan only
                         * if max_num_times is greater than 0 */
                        (
                            (instance->num_times >= instance->effective_max_num_times)
                            ||
                            (instance->num_states_in_collection >= instance->effective_max_num_states_in_collection)
                        )
                    )

                   )
                {
                    goto end_of_hard_threads_loop;
                }
            }
            /*
             * Avoid over-flow
             * */
            if (instance->ht_idx == instance->num_hard_threads)
            {
                instance->ht_idx = 0;
            }
        }

        end_of_hard_threads_loop:

        /*
         * If all the incomplete scans finished, then terminate.
         * */
        if (instance->num_hard_threads_finished == instance->num_hard_threads)
        {
            ret = FCS_STATE_IS_NOT_SOLVEABLE;
        }

        if (ret == FCS_STATE_WAS_SOLVED)
        {
            /* Create solution_moves in the first place */
            fc_solve_trace_solution(instance);
        }
    }


    if (ret == FCS_STATE_WAS_SOLVED)
    {
        if (STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_OPTIMIZE_SOLUTION_PATH))
        {
            /* Call optimize_solution only once. Make sure that if
             * it has already run - we retain the old ret. */
            if (! STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_IN_OPTIMIZATION_THREAD))
            {
                ret = fc_solve_optimize_solution(instance);
            }
            if (ret == FCS_STATE_WAS_SOLVED)
            {
                /* Create the solution_moves in the first place */
                fc_solve_trace_solution(instance);
            }
        }
    }

    return ret;
}

/*
    Clean up a solving process that was terminated in the middle.
    This function does not substitute for later calling
    finish_instance() and free_instance().
  */
static GCC_INLINE void fc_solve_unresume_instance(
    fc_solve_instance_t * instance GCC_UNUSED
    )
{
    /*
     * Do nothing - since finish_instance() can take care of solution_states
     * and proto_solution_moves as they were created by these scans, then
     * I don't need to do it here, too
     *
     * */
}


static GCC_INLINE fc_solve_soft_thread_t *
    fc_solve_instance_get_first_soft_thread(
        fc_solve_instance_t * instance
        )
{
    return &(instance->hard_threads[0].soft_threads[0]);
}

extern fc_solve_soft_thread_t * fc_solve_new_soft_thread(
    fc_solve_hard_thread_t * hard_thread
    );

static GCC_INLINE fc_solve_soft_thread_t * fc_solve_new_hard_thread(
    fc_solve_instance_t * instance
    )
{
    fc_solve_hard_thread_t * ret;
    HT_LOOP_DECLARE_VARS();

    /* Make sure we are not exceeding the maximal number of soft threads
     * for an instance. */
    if (instance->next_soft_thread_id == MAX_NUM_SCANS)
    {
        return NULL;
    }

    instance->hard_threads =
        realloc(
            instance->hard_threads,
            (sizeof(instance->hard_threads[0]) * (instance->num_hard_threads+1))
            );

    /* Since we realloc()ed the hard_threads, their addresses changed,
     * so we need to update it.
     * */
    HT_LOOP_START()
    {
        ST_LOOP_DECLARE_VARS();
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
    fc_solve_hard_thread_t * hard_thread
    )
{
    hard_thread->num_times = 0;
    hard_thread->ht_max_num_times = hard_thread->num_times_step;
    hard_thread->max_num_times = INT_MAX;
    hard_thread->num_soft_threads_finished = 0;
}

static GCC_INLINE void fc_solve_reset_soft_thread(
    fc_solve_soft_thread_t * soft_thread
    )
{
    STRUCT_CLEAR_FLAG(soft_thread, FCS_SOFT_THREAD_IS_FINISHED);
    STRUCT_CLEAR_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED);
}

static GCC_INLINE void fc_solve_release_tests_list(
    fc_solve_soft_thread_t * soft_thread,
    fcs_bool_t is_scan_befs_or_bfs
)
{
    if (is_scan_befs_or_bfs)
    {
        free (soft_thread->method_specific.befs.tests_list);
        soft_thread->method_specific.befs.tests_list = NULL;
    }
    else
    {
        /* A DFS Scan. */
        int unit_idx;
        fcs_tests_by_depth_array_t * arr;

        arr = &(soft_thread->method_specific.soft_dfs.tests_by_depth_array);
        for (unit_idx = 0 ; unit_idx < arr->num_units ; unit_idx++)
        {
            if (arr->by_depth_units[unit_idx].tests.lists)
            {
                fcs_tests_list_t * lists = arr->by_depth_units[unit_idx].tests.lists;
                int num_lists = arr->by_depth_units[unit_idx].tests.num_lists;
                int i;

                for (i=0 ;
                    i < num_lists ;
                    i++)
                {
                    free (lists[i].tests);
                }
                free (lists);
            }
        }
        free(arr->by_depth_units);
        arr->by_depth_units = NULL;
    }
}

static GCC_INLINE void fc_solve_instance__recycle_hard_thread(
    fc_solve_hard_thread_t * hard_thread
    )
{
    ST_LOOP_DECLARE_VARS();

    fc_solve_reset_hard_thread(hard_thread);
    fc_solve_compact_allocator_recycle(&(hard_thread->allocator));

    ST_LOOP_START()
    {
        if (soft_thread->method == FCS_METHOD_A_STAR)
        {
            fc_solve_PQueueFree(
                &(soft_thread->method_specific.befs.meth.befs.pqueue)
            );
        }

        fc_solve_reset_soft_thread(soft_thread);
    }

    return;
}

static GCC_INLINE void fc_solve_recycle_instance(
    fc_solve_instance_t * instance
        )
{
    int ht_idx;

    fc_solve_finish_instance(instance);

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    fc_solve_hash_recycle(&(instance->hash));
#endif
#ifdef INDIRECT_STACK_STATES
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH)
    fc_solve_hash_recycle(&(instance->stacks_hash));
#endif
#endif

    instance->num_times = 0;

    instance->num_hard_threads_finished = 0;

    for(ht_idx = 0;  ht_idx < instance->num_hard_threads; ht_idx++)
    {
        fc_solve_instance__recycle_hard_thread(&(instance->hard_threads[ht_idx]));
    }

    if (instance->optimization_thread)
    {
        fc_solve_instance__recycle_hard_thread(instance->optimization_thread);
    }
    STRUCT_CLEAR_FLAG(instance, FCS_RUNTIME_IN_OPTIMIZATION_THREAD);
}

extern const double fc_solve_default_befs_weights[5];


#ifdef FCS_RCS_STATES
fcs_state_t * fc_solve_lookup_state_key_from_val(
        fc_solve_instance_t * instance,
        fcs_collectible_state_t * ptr_state_val
        );

extern int fc_solve_compare_lru_cache_keys(
    const void * void_a, const void * void_b, void * param
);

#endif

#ifdef FCS_RCS_STATES
#define DECLARE_MOVE_FUNCTION(name) \
extern void name( \
        fc_solve_soft_thread_t * soft_thread, \
        fcs_state_t * key_ptr_state_key, \
        fcs_collectible_state_t * val_ptr_state_val,  \
        fcs_derived_states_list_t * derived_states_list \
)
#else
#define DECLARE_MOVE_FUNCTION(name) \
extern void name( \
        fc_solve_soft_thread_t * soft_thread, \
        fcs_state_keyval_pair_t * ptr_state, \
        fcs_derived_states_list_t * derived_states_list \
        )
#endif

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__FCS_H */
