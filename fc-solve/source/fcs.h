/*
 * fcs.h - header file of freecell_solver_instance and of user-level
 * functions for Freecell Solver
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#ifndef __FCS_H
#define __FCS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "state.h"
#include "move.h"
#include "ds_order.h"
#include "fcs_enums.h"

#include "rand.h"

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE) || (defined(INDIRECT_STACK_STATES) && (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE))

#include <redblack.h>

#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE) || (defined(INDIRECT_STACK_STATES) && (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_AVL_TREE))

#include <avl.h>

#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE) || (defined(INDIRECT_STACK_STATES) && (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE))

#include <rb.h>

/* #define TREE_IMP_PREFIX(func_name) rb_##func_name */

#endif


#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH) || (defined(INDIRECT_STACK_STATES) && ((FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE) || (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)))

#include <glib.h>

#endif


#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)

#include "fcs_hash.h"

#endif

#ifdef INDIRECT_STACK_STATES
#include "fcs_hash.h"

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
    fcs_state_with_locations_t * s;
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
enum FCS_TESTS_ORDER_FLAGS
{
    FCS_TEST_ORDER_NO_FLAGS_MASK = 0xFFFFFF,
    FCS_TEST_ORDER_FLAG_RANDOM = 0x1000000,
    FCS_TEST_ORDER_FLAG_START_RANDOM_GROUP = 0x2000000
};

#ifdef FCS_WITH_TALONS
#define FCS_TESTS_NUM 27
#else
#define FCS_TESTS_NUM 25
#endif

/*
 * Declare these structures because they will be used within
 * freecell_solver_instance, and they will contain a pointer to it.
 * */
struct freecell_solver_hard_thread_struct;
struct freecell_solver_soft_thread_struct;

typedef struct freecell_solver_hard_thread_struct freecell_solver_hard_thread_t;

struct fcs_tests_order_struct
{
    int num;
    int * tests;
    fcs_derived_states_order_t * * tests_orders_pre;
    fcs_derived_states_order_instance_t * * tests_orders;
    int max_num;
};

typedef struct fcs_tests_order_struct fcs_tests_order_t;

typedef struct freecell_solver_instance
{
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    /* The sort-margin */
    fcs_state_with_locations_t * indirect_prev_states_margin[PREV_STATES_SORT_MARGIN];

    /* The number of states in the sort margin */
    int num_prev_states_margin;

    /* The sorted cached states, their number and their maximal size.
     * max_num_indirect_prev_states may increase as the
     * indirect_prev_states is realloced.
     * */
    fcs_state_with_locations_t * * indirect_prev_states;
    int num_indirect_prev_states;
    int max_num_indirect_prev_states;
#endif

    /* The number of states that were checked by the solving algorithm.
     * Badly named, should be renamed to num_iters or num_checked_states */
    int num_times;

    /*
     * A move stack that contains the moves leading to the solution.
     *
     * It is created only after the solution was found by swallowing
     * all the stacks of each depth.
     * */
    fcs_move_stack_t * solution_moves;

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

    /*
     * The debug_iter_output variables provide a programmer programmable way
     * to debug the algorithm while it is running. This works well for DFS
     * and Soft-DFS scans but at present support for A* and BFS is not
     * too good, as its hard to tell which state came from which parent state.
     *
     * debug_iter_output is a flag that indicates whether to use this feature
     * at all.
     *
     * debug_iter_output_func is a pointer to the function that performs the
     * debugging.
     *
     * debug_iter_output_context is a user-specified context for it, that
     * may include data that is not included in the instance structure.
     *
     * This feature is used by the "-s" and "-i" flags of fc-solve-debug.
     * */
    int debug_iter_output;
    void (*debug_iter_output_func)(
        void * context,
        int iter_num,
        int depth,
        void * instance,
        fcs_state_with_locations_t * state,
        int parent_iter_num
        );
    void * debug_iter_output_context;

    /*
     * tree is the balanced binary tree that is used to store and index
     * the checked states.
     *
     * */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    struct rbtree * tree;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE)
    avl_tree * tree;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE)
    rb_tree * tree;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    GTree * tree;
#endif

    /*
     * hash is the hash table that is used to store the previous
     * states of the scan.
     * */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    GHashTable * hash;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    SFO_hash_t * hash;
#endif

#if defined(INDIRECT_STACK_STATES)
    /*
     * The storage mechanism for the stacks assuming INDIRECT_STACK_STATES is
     * used.
     * */    
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH)
    SFO_hash_t * stacks_hash;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_AVL_TREE)
    avl_tree * stacks_tree;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE)
    rb_tree * stacks_tree;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    struct rbtree * stacks_tree;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
    GTree * stacks_tree;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
    GHashTable * stacks_hash;
#endif
#endif

    /*
     * Storing using Berkeley DB is not operational for some reason so
     * pay no attention to it for the while
     * */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    DB * db;
#endif

    /*
     * The number of Freecells, Stacks and Foundations present in the game.
     *
     * freecells_num and stacks_num are variable and may be specified at
     * the beginning of the execution of the algorithm. However, there
     * is a maximal limit to them which is set in config.h.
     *
     * decks_num can be 4 or 8
     * */
    int freecells_num;
    int stacks_num;
    int decks_num;

    /* What two adjacent cards in the same sequence can be: */
    int sequences_are_built_by;
    /* Whether an entire sequence can be moved from one place to the
     * other regardless of the number of unoccupied Freecells there are. */
    int unlimited_sequence_move;
    /*
     * With what cards can empty stacks be filled with.
     * */
    int empty_stacks_fill;

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
    SFO_hash_t * talons_hash;

#endif

    /* A flag that indicates whether to optimize the solution path
       at the end of the scan */
    int optimize_solution_path;

    /* This is a place-holder for the initial state */
    fcs_state_with_locations_t * state_copy_ptr;

    /* This is the final state that the scan recommends to the
     * interface
     * */
    fcs_state_with_locations_t * final_state;

    /*
     * This is the number of states in the state collection.
     *
     * It gives a rough estimate of the memory occupied by the instance.
     * */
    int num_states_in_collection;

    /*
     * A limit to the above.
     * */
    int max_num_states_in_collection;

    int num_hard_threads;
    struct freecell_solver_hard_thread_struct * * hard_threads;

    /*
     * The next ID to allocate for a soft-thread.
     * */
    int next_soft_thread_id;

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
    struct freecell_solver_hard_thread_struct * optimization_thread;

    /*
     * A counter that determines how many of the hard threads that belong
     * to this hard thread have already finished. If it becomes num_hard_threads
     * the instance terminates.
     * */
    int num_hard_threads_finished;

    /*
     * A flag that indicates whether or not to explicitly calculate
     * the depth of a state that was reached.
     * */
    int calc_real_depth;

    /*
     * The tests order for the optimization scan as specified by the user.
     * */
    int opt_tests_order_set;
    
    fcs_tests_order_t opt_tests_order;

    /*
     * This flag indicates whether scans should or should not reparent the
     * states their encounter to a lower depth in the depth tree
     * */
    int to_reparent_states;

    /* 
     * This variable determines how the scans cooperate with each other.
     * 
     * A value of 0 indicates that they don't and only share the same 
     * states collection.
     * 
     * A value of 1 indicates that they mark states as dead-end,
     * which may help or hinder other scans.
     * */
    int scans_synergy;

} freecell_solver_instance_t;




/***************************************************/


struct fcs_prelude_item_struct
{
    int scan_idx;
    int quota;
};

typedef struct fcs_prelude_item_struct fcs_prelude_item_t;


struct freecell_solver_hard_thread_struct
{
    freecell_solver_instance_t * instance;

    int num_soft_threads;
    struct freecell_solver_soft_thread_struct * * soft_threads;

    /*
     * The State Packs variables are used by all the state cache
     * management routines. A pack stores as many states as can fit
     * in a 64KB segment, and those variables manage an array of
     * such packs.
     *
     * Such allocation is possible, because at the worst situation
     * the last state is released.
     * */
    fcs_state_with_locations_t * * state_packs;
    int max_num_state_packs;
    int num_state_packs;
    int num_states_in_last_pack;
    int state_pack_len;

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

#ifdef INDIRECT_STACK_STATES
    /*
     * This is the mechanism used to allocate memory for the stacks.
     * */
    fcs_compact_allocator_t * stacks_allocator;
#endif

    /* 
     * This is a compact memory allocator for the move stacks associated
     * with the states in the states collection.
     * */
    fcs_compact_allocator_t * move_stacks_allocator;

    /* 
     * This is a move stack that is used and re-used by the 
     * tests functions of this hard thread 
     * */
    fcs_move_stack_t * reusable_move_stack;

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
    
};





/********************************************/








struct fcs_soft_dfs_stack_item_struct
{
    fcs_state_with_locations_t * state;
    fcs_derived_states_list_t derived_states_list;
    int current_state_index;
    int test_index;
    int num_freestacks;
    int num_freecells;
    int derived_states_random_indexes_max_size;
    int * derived_states_random_indexes;  
};

typedef struct fcs_soft_dfs_stack_item_struct fcs_soft_dfs_stack_item_t;

struct freecell_solver_soft_thread_struct
{
    freecell_solver_hard_thread_t * hard_thread;

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
    fcs_tests_order_t tests_order;


    /*
     * The (temporary) max depth of the Soft-DFS scans)
     * */
    int dfs_max_depth;
    /*
     * The method (i.e: DFS, Soft-DFS, BFS or A*) that is used by this
     * instance.
     *
     * */
    int method;

    /*
     * A place-holder for the original method of the scan in case
     * it is replaced by FCS_METHOD_OPTIMIZE
     *
     * */
    int orig_method;

    /*
     * A linked list that serves as the queue for the BFS scan.
     * */
    fcs_states_linked_list_item_t * bfs_queue;
    /*
     * The last item in the linked list, so new items can be added at it,
     * thus making it a queue.
     * */
    fcs_states_linked_list_item_t * bfs_queue_last_item;

    /*
     * The priority queue of the A* scan */
    PQUEUE * a_star_pqueue;
    double a_star_initial_cards_under_sequences;

    /*
     * The A* weights of the different A* tests. Those weights determine the
     * commulative weight of the state.
     *
     * */
    double a_star_weights[5];

    /*
     * The first state to be checked by the scan. It is a kind of bootstrap
     * for the algorithm.
     * */
    fcs_state_with_locations_t * first_state_to_check;

    /*
     * These are stacks used by the Soft-DFS for various uses.
     *
     * states_to_check[i] - an array of states to be checked next. Not all
     * of them will be checked because it is possible that future states
     * already visited them.
     *
     * states_to_check_move_stacks[i] - an array of move stacks that lead
     * to those states.
     *
     * num_states_to_check[i] - the size of states_to_check[i]
     *
     * max_num_states_to_check[i] - the limit of pointers that can be
     * placed in states_to_check[i] without resizing.
     *
     * current_state_indexes[i] - the index of the last checked state
     * in depth i.
     *
     * test_indexes[i] - the index of the test that was last performed.
     * FCS performs each test separately, so states_to_check[i] and
     * friends will not be overpopulated.
     *
     * num_freestacks[i] - the number of unoccpied stacks that correspond
     * to solution_states[i].
     *
     * num_freecells[i] - ditto for the freecells.
     *
     * */
    
    fcs_soft_dfs_stack_item_t * soft_dfs_info;

    /* The depth of the DFS stacks */
    int num_solution_states;

    /*
     * A pseudo-random number generator for use in the random-DFS scan
     * */
    fcs_rand_t * rand_gen;

    /*
     * The initial seed of this random number generator
     * */
    int rand_seed;
    

    /*
     * A flag that indicates if this soft thread have already been
     * initialized.
     * */
    int initialized;

    /*
     * The number of iterations with which to process this scan
     * */
    int num_times_step;

    /*
     * A flag that indicates if this scan contains all the tests that
     * are accessible to all the other scans
     * */
    int is_a_complete_scan;

    /*
     * A flag that indicates if this scan has completed a scan. Used by
     * solve_instance() to skip to the next scan.
     * */
    int is_finished;

    /*
     * A malloced string that serves as an identification for the user.
     * */
    char * name;
};


#define FCS_SOFT_DFS_STATES_TO_CHECK_GROW_BY 32

/*
 * An enum that specifies the meaning of each A* weight.
 * */
#define FCS_A_STAR_WEIGHT_CARDS_OUT 0
#define FCS_A_STAR_WEIGHT_MAX_SEQUENCE_MOVE 1
#define FCS_A_STAR_WEIGHT_CARDS_UNDER_SEQUENCES 2
#define FCS_A_STAR_WEIGHT_SEQS_OVER_RENEGADE_CARDS 3
#define FCS_A_STAR_WEIGHT_DEPTH 4

freecell_solver_instance_t * freecell_solver_alloc_instance(void);

extern void freecell_solver_init_instance(
    freecell_solver_instance_t * instance
    );

extern void freecell_solver_free_instance(
    freecell_solver_instance_t * instance
    );

extern void freecell_solver_finish_instance(
    freecell_solver_instance_t * instance
    );

extern int freecell_solver_solve_instance(
    freecell_solver_instance_t * instance,
    fcs_state_with_locations_t * init_state
    );

extern int freecell_solver_resume_instance(
    freecell_solver_instance_t * instance
    );

extern void freecell_solver_unresume_instance(
    freecell_solver_instance_t * instance
    );

extern freecell_solver_soft_thread_t * freecell_solver_instance_get_soft_thread(
    freecell_solver_instance_t * instance,
    int ht_idx,
    int st_idx
    );

extern freecell_solver_soft_thread_t * freecell_solver_new_soft_thread(
    freecell_solver_soft_thread_t * soft_thread
    );

extern freecell_solver_soft_thread_t * freecell_solver_new_hard_thread(
    freecell_solver_instance_t * instance
    );

extern int freecell_solver_hard_dfs_solve_for_state(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations,
    int depth,
    int ignore_osins
    );

extern int freecell_solver_soft_dfs_solve(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations_orig
    );

extern int freecell_solver_random_dfs_solve(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations_orig
    );


extern void freecell_solver_a_star_initialize_rater(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations
    );

extern int freecell_solver_a_star_or_bfs_do_solve_or_resume(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations_orig,
    int resume
    );

extern int freecell_solver_hard_dfs_resume_solution(
    freecell_solver_soft_thread_t * soft_thread,
    int depth
    );

extern int freecell_solver_soft_dfs_resume_solution(
    freecell_solver_soft_thread_t * soft_thread
    );

extern int freecell_solver_random_dfs_resume_solution(
    freecell_solver_soft_thread_t * soft_thread
    );


extern int freecell_solver_a_star_or_bfs_solve(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations_orig
    );

extern int freecell_solver_a_star_or_bfs_resume_solution(
    freecell_solver_soft_thread_t * soft_thread
    );

extern int freecell_solver_soft_dfs_or_random_dfs_do_solve_or_resume(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * ptr_state_with_locations_orig,
    int resume,
    int to_randomize
    );

extern void freecell_solver_recycle_instance(
    freecell_solver_instance_t * instance
        );

#ifdef __cplusplus
}
#endif

#endif /* __FCS_H */
