/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// instance.h - header file of fcs_instance / fcs_hard_thread /
// fcs_soft_thread .
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

#include "move.h"
#include "freecell-solver/fcs_enums.h"
#include "freecell-solver/fcs_user.h"
#include "rate_state.h"
#include "indirect_buffer.h"
#include "rand.h"
#include "game_type_params.h"

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE) ||               \
    (defined(INDIRECT_STACK_STATES) &&                                         \
        (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE))
#include <redblack.h>
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE)
#include "fcs_libavl2_state_storage.h"
#endif

#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)
#include "fcs_libavl2_stack_storage.h"
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE) ||                      \
    (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH) ||                      \
    (defined(INDIRECT_STACK_STATES) &&                                         \
        ((FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE) ||                 \
            (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)))
#include <glib.h>
#endif

#if ((defined(FCS_RCS_STATES) &&                                               \
         (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)) ||             \
     (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY) ||                          \
     (defined(INDIRECT_STACK_STATES) &&                                        \
         (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)))
#include <Judy.h>
#endif

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) ||                 \
     (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH))
#include "fcs_hash.h"
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH) ||              \
    (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)
#include "google_hash.h"
#endif

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE) ||                      \
     (defined(FCS_RCS_STATES) &&                                               \
         (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)))
#include "kaz_tree.h"
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
#include <db.h>
#endif

#include "pqueue.h"
#include "meta_alloc.h"

/* We need 2 chars per card - one for the column_idx and one
 * for the card_idx.
 *
 * We also need it times 13 for each of the ranks.
 *
 * We need (4*LOCAL_DECKS_NUM+1) slots to hold the cards plus a
 * (-1,-1) (= end) padding.
 * */
#define FCS_POS_BY_RANK_WIDTH (MAX_NUM_DECKS << 3)

// We don't keep track of kings
#define FCS_POS_BY_RANK_LEN (FCS_RANK_KING * FCS_POS_BY_RANK_WIDTH)
typedef struct
{
    char col, height;
} fcs_pos_by_rank;

#ifndef FCS_DISABLE_SIMPLE_SIMON
#define FCS_SS_POS_BY_RANK_WIDTH (FCS_RANK_KING + 1)
#define FCS_SS_POS_BY_RANK_LEN (FCS_SS_POS_BY_RANK_WIDTH * 4)
#define FCS_BOTH__POS_BY_RANK__SIZE                                            \
    (max(FCS_SS_POS_BY_RANK_LEN * sizeof(fcs_pos_by_rank), FCS_POS_BY_RANK_LEN))
#else
#define FCS_BOTH__POS_BY_RANK__SIZE FCS_POS_BY_RANK_LEN
#endif

typedef int8_t fcs__positions_by_rank[FCS_BOTH__POS_BY_RANK__SIZE];

/*
 * This is a linked list item that is used to implement a queue for the BFS
 * scan.
 * */
typedef struct fcs_states_linked_list_item_struct
{
    fcs_collectible_state *s;
    struct fcs_states_linked_list_item_struct *next;
} fcs_states_linked_list_item;

/*
 * Declare these structures because they will be used within
 * fc_solve_instance, and they will contain a pointer to it.
 * */
struct fc_solve_hard_thread_struct;
struct fc_solve_soft_thread_struct;
struct fc_solve_instance_struct;

typedef void (*fc_solve_solve_for_state_move_func)(
    struct fc_solve_soft_thread_struct *, fcs_kv_state,
    fcs_derived_states_list *);

#ifdef FCS_SINGLE_HARD_THREAD
#define HT_FIELD(ht, field) (ht)->hard_thread.field
#define HT_INSTANCE(hard_thread) (hard_thread)
#define INST_HT0(instance) ((instance)->hard_thread)
#define NUM_CHECKED_STATES (HT_INSTANCE(hard_thread)->i__num_checked_states)
typedef struct fc_solve_instance_struct fcs_hard_thread;
extern void fc_solve_init_soft_thread(fcs_hard_thread *const hard_thread,
    struct fc_solve_soft_thread_struct *const soft_thread);
#else
#define HT_FIELD(hard_thread, field) (hard_thread)->field
#define HT_INSTANCE(hard_thread) ((hard_thread)->instance)
#define INST_HT0(instance) ((instance)->hard_threads[0])
#define NUM_CHECKED_STATES HT_FIELD(hard_thread, ht__num_checked_states)
typedef struct fc_solve_hard_thread_struct fcs_hard_thread;
#endif
extern bool fc_solve_check_and_add_state(
    fcs_hard_thread *, fcs_kv_state *, fcs_kv_state *);

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
extern guint fc_solve_hash_function(gconstpointer key);
#endif
#include "move_funcs_maps.h"

/* HT_LOOP == hard threads' loop - macros to abstract it. */
#ifdef FCS_SINGLE_HARD_THREAD

#define HT_LOOP_START() fcs_hard_thread *const hard_thread = instance;

#else
#define HT_LOOP_START()                                                        \
    fcs_hard_thread *hard_thread = instance->hard_threads;                     \
    fcs_hard_thread *const end_hard_thread =                                   \
        hard_thread + instance->num_hard_threads;                              \
    for (; hard_thread < end_hard_thread; ++hard_thread)
#endif

/* ST_LOOP == soft threads' loop - macros to abstract it. */
#define ST_LOOP_START()                                                        \
    fcs_soft_thread *const ht_soft_threads =                                   \
        HT_FIELD(hard_thread, soft_threads);                                   \
    fcs_soft_thread *soft_thread = ht_soft_threads;                            \
    fcs_soft_thread *const end_soft_thread =                                   \
        ht_soft_threads + HT_FIELD(hard_thread, num_soft_threads);             \
    for (; soft_thread < end_soft_thread; ++soft_thread)
#define MOVES_GROW_BY 16

typedef struct
{
    fc_solve_weighting_float weights[FCS_NUM_BEFS_WEIGHTS];
} fcs_default_weights;
typedef struct
{
    bool should_go_over_stacks;
    fc_solve_weighting_float max_sequence_move_factor,
        cards_under_sequences_factor, seqs_over_renegade_cards_factor,
        depth_factor, num_cards_not_on_parents_factor;

    fc_solve_weighting_float num_cards_out_lookup_table[14];
    // The BeFS weights of the different BeFS tests. Those
    // weights determine the commulative priority of the state.
    fcs_default_weights befs_weights;
} fcs_state_weighting;

typedef enum
{
    FCS_NO_SHUFFLING,
    FCS_RAND,
    FCS_WEIGHTING,
} fcs_moves_group_kind;

typedef union {
    fc_solve_solve_for_state_move_func f;
    uint_fast32_t idx;
} fcs_move_func;

typedef struct
{
    fcs_move_func *move_funcs;
    uint_fast32_t num;
    fcs_moves_group_kind shuffling_type;
    fcs_state_weighting weighting;
} fcs_moves_group;

typedef struct
{
    uint_fast32_t num;
    fcs_moves_group *groups;
} fcs_moves_order;

typedef struct
{
    ssize_t max_depth;
    fcs_moves_order moves_order;
} fcs_by_depth_moves_order;

#define STRUCT_CLEAR_FLAG(instance, flag) (instance)->flag = FALSE
#define STRUCT_TURN_ON_FLAG(instance, flag) (instance)->flag = TRUE
#define STRUCT_QUERY_FLAG(instance, flag) ((instance)->flag)
#define STRUCT_SET_FLAG_TO(instance, flag, value) (instance)->flag = (value)

#ifdef FCS_RCS_STATES
struct fcs_cache_key_info_struct
{
    const fcs_collectible_state *val_ptr;
    fcs_state key;
    /* lower_pri and higher_pri form a doubly linked list.
     *
     * pri == priority.
     * */
    struct fcs_cache_key_info_struct *lower_pri, *higher_pri;
};

typedef struct fcs_cache_key_info_struct fcs_cache_key_info;

typedef struct
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    Pvoid_t states_values_to_keys_map;
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
    dict_t *kaz_tree;
#else
#error Unknown FCS_RCS_CACHE_STORAGE
#endif
    compact_allocator states_values_to_keys_allocator;
    fcs_int_limit_t count_elements_in_cache, max_num_elements_in_cache;

    fcs_cache_key_info *lowest_pri, *highest_pri, *recycle_bin;
} fcs_lru_cache;

#endif

#ifndef FCS_WITHOUT_ITER_HANDLER
typedef void (*instance_debug_iter_output_func)(
    void *, fcs_int_limit_t, int, void *, fcs_kv_state *, fcs_int_limit_t);
#endif

typedef struct fc_solve_soft_thread_struct fcs_soft_thread;
typedef struct fc_solve_instance_struct fcs_instance;

struct fc_solve_hard_thread_struct
{
#ifndef FCS_SINGLE_HARD_THREAD
    fcs_instance *instance;
#endif

    struct fc_solve_soft_thread_struct *soft_threads;

#ifndef FCS_SINGLE_HARD_THREAD
    /*
     * The hard thread count of how many states he checked himself. The
     * instance num_checked_states can be confusing because other threads
     * modify it too.
     *
     * Thus, the soft thread switching should be done based on this variable
     * */
    fcs_int_limit_t ht__num_checked_states;

#endif
    /*
     * The maximal limit for num_checked_states.
     * */
    fcs_int_limit_t ht__max_num_checked_states;

    /*
     * The index for the soft-thread that is currently processed
     * */
    int_fast32_t st_idx;
    /*
     * This is the mechanism used to allocate memory for stacks, states
     * and move stacks.
     * */
    compact_allocator allocator;

#ifdef FCS_WITH_MOVES
    /*
     * This is a move stack that is used and re-used by the
     * moves functions of this hard thread
     * */
    fcs_move_stack reusable_move_stack;
#endif

    /*
     * This is a buffer used to temporarily store the stacks of the
     * duplicated state.
     * */
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    size_t prelude_num_items;
    size_t prelude_idx;
#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
    fc_solve_prelude_item *prelude;
#else
    const fc_solve_prelude_item *prelude;
#endif

    bool allocated_from_list;
    int_fast32_t num_soft_threads;

    /*
     * A counter that determines how many of the soft threads that belong
     * to this hard thread have already finished. If it becomes
     * num_soft_threads this thread is skipped.
     * */
    int num_soft_threads_finished;

#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
    char *prelude_as_string;
#endif
};

/********************************************/

typedef struct
{
    int idx;
    pq_rating rating;
} rating_with_index;

typedef struct
{
    fcs_collectible_state *state;
    fcs_derived_states_list derived_states_list;
    size_t move_func_list_idx;
    size_t current_state_index;
    size_t move_func_idx;
    size_t derived_states_random_indexes_max_size;
    rating_with_index *derived_states_random_indexes;
    fcs__positions_by_rank positions_by_rank;
    fcs_game_limit num_vacant_stacks;
    fcs_game_limit num_vacant_freecells;
} fcs_soft_dfs_stack_item;

typedef struct
{
    ssize_t max_depth;
    fcs_moves_order move_funcs;
} moves_by_depth_unit;

typedef struct
{
    size_t num_units;
    moves_by_depth_unit *by_depth_units;
} fcs_moves_by_depth_array;

typedef enum
{
    FCS_SUPER_METHOD_DFS,
    FCS_SUPER_METHOD_BEFS_BRFS,
#ifndef FCS_DISABLE_PATSOLVE
    FCS_SUPER_METHOD_PATSOLVE,
#endif
} fcs_super_method_type;

struct fc_solve__patsolve_thread_struct;
struct fc_solve_soft_thread_struct
{
    fcs_hard_thread *hard_thread;

    /*
     * The ID of the soft thread inside the instance.
     * Used for the state-specific flags.
     * */
    int id;

    // The moves' order indicates which move funcs to run.
    struct
    {
        size_t num;
        fcs_by_depth_moves_order *by_depth_moves;
    } by_depth_moves_order;

    /* The super-method type - can be  */
    fcs_super_method_type super_method_type;

    struct
    {
        struct
        {
            /*
             * The (temporary) max depth of the Soft-DFS scans)
             * */
            ssize_t dfs_max_depth;

            /*
             * Soft-DFS uses a stack of fcs_soft_dfs_stack_item s.
             *
             * derived_states_list - a list of states to be checked next.
             * Not all of them will be checked because it is possible that
             * future states already visited them.
             *
             * current_state_index - the index of the last checked state
             * in depth i.
             *
             * move_func_list_idx - the index of the move list that is
             * performed. FCS performs each move separately, so
             * states_to_check and friends will not be overpopulated.
             *
             * num_vacant_stacks - the number of unoccpied stacks that
             * correspond
             * to solution_states.
             *
             * num_vacant_freecells - ditto for the freecells.
             *
             * */
            fcs_soft_dfs_stack_item *soft_dfs_info;

            /* The depth of the DFS stacks */
            ssize_t depth;

            /*
             * A pseudo-random number generator for use in the random-DFS
             * scan
             * */
            fcs_rand_gen rand_gen;

            /*
             * The initial seed of this random number generator
             * */
            int rand_seed;

            /*
             * The moves to be performed in a preprocessed form.
             * */
            fcs_moves_by_depth_array moves_by_depth;
        } soft_dfs;
        struct
        {
            fcs__positions_by_rank befs_positions_by_rank;
            fcs_move_func *moves_list, *moves_list_end;
            struct
            {
                struct
                {
                    /*
                     * A linked list that serves as the queue for the BFS
                     * scan.
                     * */
                    fcs_states_linked_list_item *bfs_queue;
                    /*
                     * The last item in the linked list, so new items can be
                     * added at
                     * it, thus making it a queue.
                     * */
                    fcs_states_linked_list_item *bfs_queue_last_item;
                    /*
                     * A linked list of items that were freed from
                     * the queue and should be reused before allocating new
                     * items.
                     * */
                    fcs_states_linked_list_item *recycle_bin;
                } brfs;
                struct
                {
                    /*
                     * The priority queue of the BeFS scan
                     * */
                    pri_queue pqueue;
                    fcs_state_weighting weighting;
                } befs;
            } meth;
            /*
             * The first state to be checked by the scan. It is a kind of
             * bootstrap for the algorithm.
             * */
            fcs_collectible_state *first_state_to_check;
        } befs;
    } method_specific;

    bool FCS_SOFT_THREAD_IS_FINISHED, FCS_SOFT_THREAD_INITIALIZED,
        FCS_SOFT_THREAD_IS_A_COMPLETE_SCAN;

    // The numbers of vacant stacks and freecells in the current state - is
    // read by the move functions in freecell.c .
    fcs_game_limit num_vacant_stacks, num_vacant_freecells;

    /*
     * The number of iterations with which to process this scan
     * */
    fcs_int_limit_t checked_states_step;

#ifndef FCS_USE_PRECOMPILED_CMD_LINE_THEME
    // A string that serves as an identification for the user.
    char name[FCS_MAX_IDENT_LEN];
#endif

#ifndef FCS_ENABLE_PRUNE__R_TF__UNCOND
    /*
     * Whether pruning should be done.
     * This variable is temporary - there should be a better pruning
     * abstraction with several optional prunes.
     * */
    bool enable_pruning;
#endif

#ifndef FCS_DISABLE_PATSOLVE
    /*
     * The patsolve soft_thread that is associated with this soft_thread.
     * */
    struct fc_solve__patsolve_thread_struct *pats_scan;
#endif
    /*
     * Differentiates between SOFT_DFS and RANDOM_DFS.
     * */
    bool master_to_randomize;
    bool is_befs
#ifdef FCS_WITH_MOVES
        ,
        is_optimize_scan
#endif
        ;
};

struct fc_solve_instance_struct
{
// The parameters of the game - see the declaration of fcs_game_type_params_t .
#ifndef FCS_FREECELL_ONLY
    fcs_game_type_params game_params;
#ifndef FCS_DISABLE_PATSOLVE
    fcs_card game_variant_suit_mask;
    fcs_card game_variant_desired_suit_value;
#endif
#endif

    /* The number of states that were checked by the solving algorithm. */
    fcs_int_limit_t i__num_checked_states;

#ifndef FCS_WITHOUT_MAX_NUM_STATES
    /*
     * Limit for the maximal number of checked states.
     * max_num_checked_states is useful because it can limit the amount of
     * consumed memory (and time).
     *
     * This is the effective number that enables the process to work without
     * checking if it's zero.
     *
     * Normally should be used instead.
     * */
    fcs_int_limit_t effective_max_num_checked_states;
#endif
#ifndef FCS_DISABLE_NUM_STORED_STATES
    fcs_int_limit_t effective_max_num_states_in_collection;
#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
    fcs_int_limit_t effective_trim_states_in_collection_from;
#endif
#endif
    fcs_seq_cards_power_type initial_cards_under_sequences_value;
/*
 * tree is the balanced binary tree that is used to store and index
 * the checked states.
 *
 * */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    struct rbtree *tree;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY)
    Pvoid_t judy_array;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE)
    fcs_libavl2_states_tree_table *tree;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    GTree *tree;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE)
    dict_t *tree;
#endif

/*
 * hash is the hash table that is used to store the previous
 * states of the scan.
 * */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    GHashTable *hash;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    hash_table hash;
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)
    fcs_states_google_hash_handle hash;
#endif

#if defined(INDIRECT_STACK_STATES)
/*
 * The storage mechanism for the stacks assuming INDIRECT_STACK_STATES is
 * used.
 * */
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH)
    hash_table stacks_hash;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)
    fcs_libavl2_stacks_tree_table *stacks_tree;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    struct rbtree *stacks_tree;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
    GTree *stacks_tree;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
    GHashTable *stacks_hash;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)
    fcs_columns_google_hash_handle stacks_hash;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)
    Pvoid_t stacks_judy_array;
#else
#error FCS_STACK_STORAGE is not set to a good value.
#endif
#endif

    fcs_collectible_state *list_of_vacant_states;
/*
 * Storing using Berkeley DB is not operational for some reason so
 * pay no attention to it for the while
 * */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    DB *db;
#endif

#ifndef FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE
    bool FCS_RUNTIME_CALC_REAL_DEPTH;
#endif
#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
    bool FCS_RUNTIME_TO_REPARENT_STATES_REAL;
#endif
#ifndef FCS_HARD_CODE_SCANS_SYNERGY_AS_TRUE
    bool FCS_RUNTIME_SCANS_SYNERGY;
#endif
#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
    bool FCS_RUNTIME_TO_REPARENT_STATES_PROTO;
#endif
    ;
#ifdef FCS_WITH_MOVES
    bool FCS_RUNTIME_OPTIMIZE_SOLUTION_PATH, FCS_RUNTIME_IN_OPTIMIZATION_THREAD,
        FCS_RUNTIME_OPT_TESTS_ORDER_WAS_SET;
#endif

/*
 * This is the number of states in the state collection.
 *
 * It gives a rough estimate of the memory occupied by the instance.
 * */
#ifndef FCS_DISABLE_NUM_STORED_STATES
#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
    fcs_int_limit_t active_num_states_in_collection;
#endif
    fcs_int_limit_t num_states_in_collection;
#endif

#ifdef FCS_SINGLE_HARD_THREAD
    struct fc_solve_hard_thread_struct hard_thread;
#ifdef FCS_WITH_MOVES
    bool is_optimization_st;
    struct fc_solve_soft_thread_struct optimization_soft_thread;
#endif
#else
    uint_fast32_t num_hard_threads;
    struct fc_solve_hard_thread_struct *hard_threads;
    /*
     * An iterator over the hard threads.
     * */
    fcs_hard_thread *current_hard_thread;

#ifdef FCS_WITH_MOVES
    /*
     * This is the hard-thread used for the optimization scan.
     * */
    struct fc_solve_hard_thread_struct *optimization_thread;
#endif
#endif

    // The master moves' order. It is used to initialize all the new
    // Soft-Threads.
    fcs_moves_order instance_moves_order;

    /*
     * A counter that determines how many of the hard threads that belong
     * to this hard thread have already finished. If it becomes
     * num_hard_threads the instance terminates.
     * */
    uint_fast32_t num_hard_threads_finished;

#ifdef FCS_WITH_MOVES
    // The moves for the optimization scan, as specified by the user.
    fcs_moves_order opt_moves;
#endif

#ifdef FCS_RCS_STATES
    fcs_lru_cache rcs_states_cache;

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE) ||                  \
     (FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE))
    fcs_state *tree_new_state_key;
    fcs_collectible_state *tree_new_state;
#endif

#endif

#ifndef FCS_WITHOUT_ITER_HANDLER
    /*
     * The debug_iter_output variables provide a programmer programmable way
     * to debug the algorithm while it is running. This works well for DFS
     * and Soft-DFS scans but at present support for BeFS and BFS is not
     * too good, as its hard to tell which state came from which parent
     * state.
     *
     * debug_iter_output_func is a pointer to the function that performs the
     * debugging. If NULL, this feature is not used.
     *
     * debug_iter_output_context is a user-specified context for it, that
     * may include data that is not included in the instance structure.
     *
     * This feature is used by the "-s" and "-i" flags of fc-solve-debug.
     * */
    instance_debug_iter_output_func debug_iter_output_func;
    void *debug_iter_output_context;
#endif

    /*
     * The next ID to allocate for a soft-thread.
     * */
    int next_soft_thread_id;

    /* This is the initial state */
    fcs_state_keyval_pair state_copy;

#ifdef FCS_WITH_MOVES
    /* This is the final state that the scan recommends to the
     * interface
     * */
    fcs_collectible_state *final_state;

    /*
     * A move stack that contains the moves leading to the solution.
     *
     * It is created only after the solution was found by swallowing
     * all the stacks of each depth.
     * */
    fcs_move_stack solution_moves;
#endif

    /*
     * The meta allocator - see meta_alloc.h.
     * */
    meta_allocator *meta_alloc;

#if (defined(FCS_WITH_MOVES) && (!defined(FCS_DISABLE_PATSOLVE)))
    /*
     * The soft_thread that solved the state.
     *
     * Needed to trace the patsolve solutions.
     * */
    fcs_soft_thread *solving_soft_thread;
#endif
#ifndef FCS_DISABLE_PATSOLVE
    /*
     * This is intended to be used by the patsolve scan which is
     * sensitive to the ordering of the columns/stacks. This is an ugly hack
     * but hopefully it will work.
     * */
    fcs_state_keyval_pair *initial_non_canonized_state;
#endif

#ifndef FCS_DISABLE_SIMPLE_SIMON
    /*
     * Whether or not this is a Simple Simon-like game.
     * */
    bool is_simple_simon;
#endif
};

#define fcs_st_instance(soft_thread) HT_INSTANCE((soft_thread)->hard_thread)

#define DFS_VAR(soft_thread, var) (soft_thread)->method_specific.soft_dfs.var
#define BEFS_VAR(soft_thread, var)                                             \
    (soft_thread)->method_specific.befs.meth.befs.var
/* M is Methods-common. */
#define BEFS_M_VAR(soft_thread, var) (soft_thread)->method_specific.befs.var
#define BRFS_VAR(soft_thread, var)                                             \
    (soft_thread)->method_specific.befs.meth.brfs.var

/*
 * An enum that specifies the meaning of each BeFS weight.
 * */
#define FCS_BEFS_WEIGHT_CARDS_OUT 0
#define FCS_BEFS_WEIGHT_MAX_SEQUENCE_MOVE 1
#define FCS_BEFS_WEIGHT_CARDS_UNDER_SEQUENCES 2
#define FCS_BEFS_WEIGHT_SEQS_OVER_RENEGADE_CARDS 3
#define FCS_BEFS_WEIGHT_DEPTH 4
#define FCS_BEFS_WEIGHT_NUM_CARDS_NOT_ON_PARENTS 5

#ifndef FCS_DISABLE_PATSOLVE
#include "pat.h"
#endif

extern fc_solve_solve_process_ret_t fc_solve_befs_or_bfs_do_solve(
    fcs_soft_thread *const soft_thread);

static inline void *memdup(const void *const src, const size_t my_size)
{
    void *const dest = malloc(my_size);
    if (dest == NULL)
    {
        return NULL;
    }

    memcpy(dest, src, my_size);

    return dest;
}

static inline int update_col_cards_under_sequences(
#ifndef FCS_FREECELL_ONLY
    const int sequences_are_built_by,
#endif
    const fcs_const_cards_column col,
    int d /* One less than cards_num of col. */
)
{
    fcs_card this_card = fcs_col_get_card(col, d);
    fcs_card prev_card;
    for (; (d > 0) && ({
             prev_card = fcs_col_get_card(col, d - 1);
             fcs_is_parent_card(this_card, prev_card);
         });
         d--, this_card = prev_card)
    {
    }
    return d;
}

extern fcs_collectible_state *fc_solve_sfs_raymond_prune(
    fcs_soft_thread *, fcs_kv_state);

#ifdef FCS_RCS_STATES
fcs_state *fc_solve_lookup_state_key_from_val(
    fcs_instance *instance, const fcs_collectible_state *orig_ptr_state_val);

extern int fc_solve_compare_lru_cache_keys(const void *, const void *, void *);

#endif

extern void fc_solve_soft_thread_init_befs_or_bfs(
    fcs_soft_thread *const soft_thread);

extern void fc_solve_instance__init_hard_thread(
#ifndef FCS_SINGLE_HARD_THREAD
    fcs_instance *const instance,
#endif
    fcs_hard_thread *const hard_thread);

extern void fc_solve_free_soft_thread_by_depth_move_array(
    fcs_soft_thread *const soft_thread);

static inline fcs_moves_order moves_order_dup(fcs_moves_order *const orig)
{
    const_SLOT(num, orig);
    fcs_moves_order ret = (fcs_moves_order){.num = num,
        .groups = memdup(orig->groups,
            sizeof(orig->groups[0]) *
                ((num & (~(MOVES_GROW_BY - 1))) + MOVES_GROW_BY))};

    for (size_t i = 0; i < num; ++i)
    {
        ret.groups[i].move_funcs = memdup(ret.groups[i].move_funcs,
            sizeof(ret.groups[i].move_funcs[0]) *
                ((ret.groups[i].num & (~(MOVES_GROW_BY - 1))) + MOVES_GROW_BY));
    }

    return ret;
}

extern fcs_soft_thread *fc_solve_new_soft_thread(
    fcs_hard_thread *const hard_thread);

/* This is the commmon code from fc_solve_instance__init_hard_thread() and
 * recycle_hard_thread() */
static inline void fc_solve_reset_hard_thread(
    fcs_hard_thread *const hard_thread)
{
#ifndef FCS_SINGLE_HARD_THREAD
    HT_FIELD(hard_thread, ht__num_checked_states) = 0;
#endif
    HT_FIELD(hard_thread, ht__max_num_checked_states) = FCS_INT_LIMIT_MAX;
    HT_FIELD(hard_thread, num_soft_threads_finished) = 0;
}

static inline void fc_solve_reset_soft_thread(
    fcs_soft_thread *const soft_thread)
{
    STRUCT_CLEAR_FLAG(soft_thread, FCS_SOFT_THREAD_IS_FINISHED);
    STRUCT_CLEAR_FLAG(soft_thread, FCS_SOFT_THREAD_INITIALIZED);
}

typedef enum
{
    FOREACH_SOFT_THREAD_CLEAN_SOFT_DFS,
    FOREACH_SOFT_THREAD_FREE_INSTANCE,
    FOREACH_SOFT_THREAD_ACCUM_TESTS_ORDER,
    FOREACH_SOFT_THREAD_DETERMINE_SCAN_COMPLETENESS
} foreach_st_callback_choice;

extern void fc_solve_foreach_soft_thread(fcs_instance *const instance,
    const foreach_st_callback_choice callback_choice, void *const context);

/*
    This function is the last function that should be called in the
    sequence of operations on instance, and it is meant for de-allocating
    whatever memory was allocated by alloc_instance().
  */

static inline void moves_order__free(fcs_moves_order *moves_order)
{
    const_SLOT(groups, moves_order);
    const_SLOT(num, moves_order);
    for (size_t group_idx = 0; group_idx < num; ++group_idx)
    {
        free(groups[group_idx].move_funcs);
    }
    free(groups);
    moves_order->groups = NULL;
    moves_order->num = 0;
}

/***********************************************************/
#define MOVE_FUNC_ARGS                                                         \
    fcs_soft_thread *const soft_thread GCC_UNUSED,                             \
        fcs_kv_state raw_state_raw GCC_UNUSED,                                 \
        fcs_derived_states_list *const derived_states_list GCC_UNUSED

#define DECLARE_MOVE_FUNCTION(name) extern void name(MOVE_FUNC_ARGS)
#define DECLARE_PURE_MOVE_FUNCTION(name) extern void name(MOVE_FUNC_ARGS)

#ifndef FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE
static inline bool fcs_get_calc_real_depth(const fcs_instance *const instance)
{
    return STRUCT_QUERY_FLAG(instance, FCS_RUNTIME_CALC_REAL_DEPTH);
}
#endif

#ifdef FCS_WITH_MOVES
extern void fc_solve_trace_solution(fcs_instance *const instance);
#endif
extern void fc_solve_finish_instance(fcs_instance *const instance);

#ifdef __cplusplus
}
#endif
