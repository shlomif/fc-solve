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
 * instance.c - instance/hard_thread/soft_thread functions for 
 * Freecell Solver.
 */

#define BUILDING_DLL 1

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NUM_TIMES_STEP 50

#include "config.h"

#include "state.h"
#include "card.h"
#include "instance.h"
#include "scans.h"

#include "preset.h"
#include "unused.h"
#include "check_and_add_state.h"
#include "move_funcs_order.h"

#include "inline.h"

/*
    General use of this interface:
    1. fc_solve_alloc_instance()
    2. Set the parameters of the game
    3. If you wish to revert, go to step #11.
    4. fc_solve_init_instance()
    5. Call fc_solve_solve_instance() with the initial board.
    6. If it returns FCS_STATE_SUSPEND_PROCESS and you wish to proceed,
       then increase the iteration limit and call
       fc_solve_resume_instance().
    7. Repeat Step #6 zero or more times.
    8. If the last call to solve_instance() or resume_instance() returned
       FCS_STATE_SUSPEND_PROCESS then call
       fc_solve_unresume_instance().
    9. If the solving was successful you can use the move stacks or the
       intermediate stacks. (Just don't destory them in any way).
    10. Call fc_solve_finish_instance().
    11. Call fc_solve_free_instance().

    The library functions inside lib.c (a.k.a fcs_user()) give an
    easier approach for embedding Freecell Solver into your library. The
    intent of this comment is to document the code, rather than to be
    a guideline for the user.
*/

const double fc_solve_a_star_default_weights[5] = {0.5,0,0.3,0,0.2};



static GCC_INLINE void normalize_a_star_weights(
    fc_solve_soft_thread_t * soft_thread
    )
{
    /* Normalize the A* Weights, so the sum of all of them would be 1. */
    double sum;
    int a;
    sum = 0;
#define my_a_star_weights soft_thread->method_specific.befs.meth.befs.a_star_weights
    for(a=0;a<(sizeof(my_a_star_weights)/sizeof(my_a_star_weights[0]));a++)
    {
        if (my_a_star_weights[a] < 0)
        {
            my_a_star_weights[a] = fc_solve_a_star_default_weights[a];
        }
        sum += my_a_star_weights[a];
    }
    if (sum < 1e-6)
    {
        sum = 1;
    }
    for(a=0;a<(sizeof(my_a_star_weights)/sizeof(my_a_star_weights[0]));a++)
    {
        my_a_star_weights[a] /= sum;
    }
}

static GCC_INLINE void soft_thread_clean_soft_dfs(
    fc_solve_soft_thread_t * soft_thread
    )
{
    int max_depth;
    int dfs_max_depth;
    fcs_soft_dfs_stack_item_t * soft_dfs_info, * info_ptr;
    /* Check if a Soft-DFS-type scan was called in the first place */
    if (!(
            FC_SOLVE_IS_DFS(soft_thread)
        && soft_thread->method_specific.soft_dfs.soft_dfs_info
       ))
    {
        /* If not - do nothing */
        return;
    }

    soft_dfs_info = soft_thread->method_specific.soft_dfs.soft_dfs_info;
    max_depth = soft_thread->method_specific.soft_dfs.depth;
    dfs_max_depth = soft_thread->method_specific.soft_dfs.dfs_max_depth;
    /* De-allocate the Soft-DFS specific stacks */
    {
        int depth;
        info_ptr = soft_dfs_info;
        for(depth=0;depth<max_depth;depth++)
        {
            free(info_ptr->derived_states_list.states);
            free(info_ptr->derived_states_random_indexes);
            free(info_ptr->positions_by_rank);
            info_ptr++;
        }
        for(;depth<dfs_max_depth;depth++)
        {
            if (info_ptr->derived_states_list.states)
            {
                free(info_ptr->derived_states_list.states);
                free(info_ptr->derived_states_random_indexes);
            }
            info_ptr++;
        }

        free(soft_dfs_info);

        soft_thread->method_specific.soft_dfs.soft_dfs_info = NULL;

        soft_thread->method_specific.soft_dfs.dfs_max_depth = 0;

    }
}


static GCC_INLINE void free_instance_soft_thread_callback(
        fc_solve_soft_thread_t * soft_thread
        )
{
    switch (soft_thread->method)
    {
        case FCS_METHOD_BFS:
        case FCS_METHOD_OPTIMIZE:
            fc_solve_free_bfs_queue(soft_thread);
            break;
        case FCS_METHOD_A_STAR:
            fc_solve_PQueueFree(
                    &(soft_thread->method_specific.befs.meth.befs.a_star_pqueue)
            );
            break;
    }
    free(soft_thread->tests_order.tests);

    if (soft_thread->name != NULL)
    {
        free(soft_thread->name);
    }
}

static GCC_INLINE void accumulate_tests_order(
    fc_solve_soft_thread_t * soft_thread,
    void * context
    )
{
    int * tests_order = (int *)context;
    int a;
    for(a=0;a<soft_thread->tests_order.num;a++)
    {
        *tests_order |= (1 << (soft_thread->tests_order.tests[a] & FCS_TEST_ORDER_NO_FLAGS_MASK));
    }
}

static GCC_INLINE void determine_scan_completeness(
    fc_solve_soft_thread_t * soft_thread,
    void * global_tests_order
    )
{
    int tests_order = 0;
    int a;
    for(a=0;a<soft_thread->tests_order.num;a++)
    {
        tests_order |= (1 << (soft_thread->tests_order.tests[a] & FCS_TEST_ORDER_NO_FLAGS_MASK));
    }
    soft_thread->is_a_complete_scan = (tests_order == *(int *)global_tests_order);
}

enum
{
    FOREACH_SOFT_THREAD_CLEAN_SOFT_DFS,
    FOREACH_SOFT_THREAD_FREE_INSTANCE,
    FOREACH_SOFT_THREAD_ACCUM_TESTS_ORDER,
    FOREACH_SOFT_THREAD_DETERMINE_SCAN_COMPLETENESS
};

static void foreach_soft_thread(
    fc_solve_instance_t * instance,
    int callback_choice,
    void * context
    )
{
    int ht_idx, st_idx;
    fc_solve_hard_thread_t * hard_thread;
    int num_soft_threads;
    for(ht_idx = 0 ; ht_idx<=instance->num_hard_threads; ht_idx++)
    {
        register fc_solve_soft_thread_t * soft_thread;
        if (ht_idx < instance->num_hard_threads)
        {
            hard_thread = instance->hard_threads[ht_idx];
        }
        else if (instance->optimization_thread)
        {
            hard_thread = instance->optimization_thread;
        }
        else
        {
            break;
        }
        num_soft_threads = hard_thread->num_soft_threads;
        soft_thread = hard_thread->soft_threads;
        for(st_idx = 0 ; st_idx < num_soft_threads; st_idx++, soft_thread++)
        {
            switch (callback_choice)
            {
                case FOREACH_SOFT_THREAD_CLEAN_SOFT_DFS:
                    soft_thread_clean_soft_dfs( 
                        soft_thread
                    );
                    break;

                case FOREACH_SOFT_THREAD_FREE_INSTANCE:
                    free_instance_soft_thread_callback( 
                        soft_thread
                    );
                    break;

                case FOREACH_SOFT_THREAD_ACCUM_TESTS_ORDER:
                    accumulate_tests_order(
                        soft_thread,
                        context
                    );
                    break;

                case FOREACH_SOFT_THREAD_DETERMINE_SCAN_COMPLETENESS:
                    determine_scan_completeness(
                        soft_thread,
                        context
                    );
                    break;
            }
        }
    }
}




static GCC_INLINE void clean_soft_dfs(
        fc_solve_instance_t * instance
        )
{
    foreach_soft_thread(instance, FOREACH_SOFT_THREAD_CLEAN_SOFT_DFS, NULL);
}


static GCC_INLINE void init_soft_thread(
        fc_solve_hard_thread_t * hard_thread,
        fc_solve_soft_thread_t * soft_thread
        )
{
    soft_thread->hard_thread = hard_thread;

    soft_thread->id = (hard_thread->instance->next_soft_thread_id)++;

    soft_thread->method_specific.soft_dfs.dfs_max_depth = 0;

    soft_thread->tests_order.num = 0;
    soft_thread->tests_order.tests = NULL;

    /* Initialize all the Soft-DFS stacks to NULL */
    soft_thread->method_specific.soft_dfs.soft_dfs_info = NULL;

    /* The default solving method */
    soft_thread->method = FCS_METHOD_SOFT_DFS;

    soft_thread->method_specific.befs.a_star_positions_by_rank = NULL;

    soft_thread->method_specific.soft_dfs.rand_seed = 24;

    soft_thread->num_times_step = NUM_TIMES_STEP;

#if 0
    {
        char * no_use;
        fc_solve_apply_tests_order(soft_thread, "[01][23456789]", &no_use);
    }
#else
    soft_thread->tests_order.num = soft_thread->hard_thread->instance->instance_tests_order.num;
    /* Bound the maximal number up to the next product of 
     * TESTS_ORDER_GROW_BY . 
     * */
    soft_thread->tests_order.tests =
        malloc(sizeof(soft_thread->tests_order.tests[0]) * 
            ((soft_thread->tests_order.num & (~(TESTS_ORDER_GROW_BY - 1)))+TESTS_ORDER_GROW_BY)
        );
    memcpy(soft_thread->tests_order.tests,
        soft_thread->hard_thread->instance->instance_tests_order.tests,
        sizeof(soft_thread->tests_order.tests[0]) * soft_thread->tests_order.num
        );
#endif

    fc_solve_reset_soft_thread(soft_thread);

    soft_thread->name = NULL;
}

fc_solve_hard_thread_t * fc_solve_instance__alloc_hard_thread(
        fc_solve_instance_t * instance
        )
{
    fc_solve_hard_thread_t * hard_thread;

    /* Make sure we are not exceeding the maximal number of soft threads
     * for an instance. */
    if (instance->next_soft_thread_id == MAX_NUM_SCANS)
    {
        return NULL;
    }

    hard_thread = malloc(sizeof(fc_solve_hard_thread_t));

    hard_thread->instance = instance;

    hard_thread->num_soft_threads = 0;

    hard_thread->soft_threads = NULL;

    fc_solve_new_soft_thread(hard_thread);

    /* Set a limit on the Hard-Thread's scan. */
    hard_thread->num_times_step = NUM_TIMES_STEP;


    hard_thread->prelude_as_string = NULL;
    hard_thread->prelude = NULL;
    hard_thread->prelude_num_items = 0;
    hard_thread->prelude_idx = 0;

    fc_solve_reset_hard_thread(hard_thread);

    fcs_move_stack_init(hard_thread->reusable_move_stack);

    return hard_thread;
}


/*
    This function allocates a Freecell Solver instance struct and set the
    default values in it. After the call to this function, the program can
    set parameters in it which are different from the default.

    Afterwards fc_solve_init_instance() should be called in order
    to really prepare it for solving.
  */
fc_solve_instance_t * fc_solve_alloc_instance(void)
{
    fc_solve_instance_t * instance;

    instance = malloc(sizeof(fc_solve_instance_t));

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    instance->num_indirect_prev_states = 0;
#endif

    instance->num_times = 0;

    instance->num_states_in_collection = 0;

    instance->max_num_times = -1;
    instance->max_depth = -1;
    instance->max_num_states_in_collection = -1;

    instance->instance_tests_order.num = 0;
    instance->instance_tests_order.tests = NULL;

    instance->opt_tests_order_set = 0;

    instance->opt_tests_order.num = 0;
    instance->opt_tests_order.tests = NULL;



#ifdef FCS_WITH_TALONS
    instance->talon_type = FCS_TALON_NONE;
#endif

    instance->num_hard_threads = 0;

#ifndef FCS_FREECELL_ONLY
    fc_solve_apply_preset_by_name(instance, "freecell");
#else
    {
        char * no_use;
        fc_solve_apply_tests_order(
            &(instance->instance_tests_order),
            "[01][23456789]",
            &no_use
        );
    }
#endif

    /****************************************/

    instance->debug_iter_output = 0;

    instance->next_soft_thread_id = 0;

    instance->num_hard_threads = 1;

    instance->hard_threads = malloc(sizeof(instance->hard_threads[0]) * instance->num_hard_threads);

    instance->hard_threads[0] = fc_solve_instance__alloc_hard_thread(instance);

    instance->solution_moves.moves = NULL;

    instance->optimize_solution_path = 0;
    instance->optimization_thread = NULL;
    instance->in_optimization_thread = 0;

    instance->num_hard_threads_finished = 0;

    instance->calc_real_depth = 0;

    instance->to_reparent_states_proto = 0;

    /* Make the 1 the default, because otherwise scans will not cooperate
     * with one another. */
    instance->scans_synergy = 1;

    return instance;
}

static GCC_INLINE void free_instance_hard_thread_callback(fc_solve_hard_thread_t * hard_thread)
{
    if (hard_thread->prelude_as_string)
    {
        free (hard_thread->prelude_as_string);
    }
    if (hard_thread->prelude)
    {
        free (hard_thread->prelude);
    }
    fcs_move_stack_static_destroy(hard_thread->reusable_move_stack);

    free(hard_thread->soft_threads);

    if (hard_thread->allocator.packs)
    {
        fc_solve_compact_allocator_finish(&(hard_thread->allocator));
        hard_thread->allocator.packs = NULL;
    }
    free(hard_thread);
}

/*
    This function is the last function that should be called in the
    sequence of operations on instance, and it is meant for de-allocating
    whatever memory was allocated by alloc_instance().
  */
void fc_solve_free_instance(fc_solve_instance_t * instance)
{
    int ht_idx;

    foreach_soft_thread(instance, FOREACH_SOFT_THREAD_FREE_INSTANCE, NULL);

    for(ht_idx=0; ht_idx < instance->num_hard_threads; ht_idx++)
    {
        free_instance_hard_thread_callback(instance->hard_threads[ht_idx]);
    }
    free(instance->hard_threads);
    if (instance->optimization_thread)
    {
        free_instance_hard_thread_callback(instance->optimization_thread);
    }

    free(instance->instance_tests_order.tests);

    if (instance->opt_tests_order_set)
    {
        free(instance->opt_tests_order.tests);
    }

    free(instance);
}



enum
{
    FCS_COMPILE_PRELUDE_OK,
    FCS_COMPILE_PRELUDE_NO_AT_SIGN,
    FCS_COMPILE_PRELUDE_UNKNOWN_SCAN_ID
};

static GCC_INLINE int compile_prelude(
    fc_solve_hard_thread_t * hard_thread
    )
{
    char * p_quota, * p_scan, * p;
    char * string;
    int last_one = 0;
    int num_items = 0;
    fcs_prelude_item_t * prelude;
    int st_idx;

    prelude = NULL;
    string = hard_thread->prelude_as_string;

    p = string;

    while (! last_one)
    {
        p_quota = p;
        while((*p) && isdigit(*p))
        {
            p++;
        }
        if (*p != '@')
        {
            free(prelude);
            return FCS_COMPILE_PRELUDE_NO_AT_SIGN;
        }
        *p = '\0';
        p++;
        p_scan = p;
        while((*p) && ((*p) != ','))
        {
            p++;
        }
        if ((*p) == '\0')
        {
            last_one = 1;
        }
        *p = '\0';
        p++;

        for(st_idx = 0; st_idx < hard_thread->num_soft_threads ; st_idx++)
        {
            if (!strcmp(hard_thread->soft_threads[st_idx].name, p_scan))
            {
                break;
            }
        }
        if (st_idx == hard_thread->num_soft_threads)
        {
            free(prelude);
            return FCS_COMPILE_PRELUDE_UNKNOWN_SCAN_ID;
        }
#define PRELUDE_GROW_BY 16
        if (! (num_items & (PRELUDE_GROW_BY-1)))
        {
            prelude = realloc(prelude, sizeof(prelude[0]) * (num_items+PRELUDE_GROW_BY));
        }
        prelude[num_items].scan_idx = st_idx;
        prelude[num_items].quota = atoi(p_quota);
        num_items++;
    }

    hard_thread->prelude = realloc(prelude, sizeof(prelude[0]) * num_items);
    hard_thread->prelude_num_items = num_items;
    hard_thread->prelude_idx = 0;

    return FCS_COMPILE_PRELUDE_OK;
}


void fc_solve_init_instance(fc_solve_instance_t * instance)
{
    int ht_idx;
    fc_solve_hard_thread_t * hard_thread;

    /* Initialize the state packs */
    for(ht_idx=0;ht_idx<instance->num_hard_threads;ht_idx++)
    {
        hard_thread = instance->hard_threads[ht_idx];
        if (hard_thread->prelude_as_string)
        {
            compile_prelude(hard_thread);
        }
        hard_thread->num_times_left_for_soft_thread =
            hard_thread->soft_threads[0].num_times_step;
    }

    {
        int total_tests = 0;
        foreach_soft_thread(
            instance, 
            FOREACH_SOFT_THREAD_ACCUM_TESTS_ORDER, 
            &total_tests
        );
        foreach_soft_thread(
            instance,
            FOREACH_SOFT_THREAD_DETERMINE_SCAN_COMPLETENESS,
            &total_tests
        );
        if (instance->opt_tests_order_set == 0)
        {
            /*
             *
             * What this code does is convert the bit map of total_tests
             * to a valid tests order.
             *
             * */
            int bit_idx, num_tests = 0;
            int * tests = malloc(sizeof(total_tests)*8*sizeof(tests[0]));

            for(bit_idx=0; total_tests != 0; bit_idx++, total_tests >>= 1)
            {
                if ((total_tests & 0x1) != 0)
                {
                    tests[num_tests++] = bit_idx;
                }
            }
            tests = realloc(tests, num_tests*sizeof(tests[0]));
            instance->opt_tests_order.tests = tests;
            instance->opt_tests_order.num =
                num_tests;
            instance->opt_tests_order_set = 1;
        }
    }
}

/* These are all stack comparison functions to be used for the stacks
   cache when using INDIRECT_STACK_STATES
*/
#if defined(INDIRECT_STACK_STATES)

#if ((FCS_STACK_STORAGE != FCS_STACK_STORAGE_GLIB_TREE) && (FCS_STACK_STORAGE != FCS_STACK_STORAGE_GLIB_HASH) && (FCS_STACK_STORAGE != FCS_STACK_STORAGE_JUDY))
#if (((FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH) \
       && (defined(FCS_WITH_CONTEXT_VARIABLE)) \
       && (!defined(FCS_INLINED_HASH_COMPARISON))) \
            || \
       (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE) \
            || \
       (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE) \
    )
static int fcs_stack_compare_for_comparison_with_context(
    const void * v_s1,
    const void * v_s2,
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    const
#endif
    void * context GCC_UNUSED

    )
{
    return fc_solve_stack_compare_for_comparison(v_s1, v_s2);
}
#endif
#endif


#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
/* A hash calculation function for use in glib's hash */
static guint fc_solve_glib_hash_stack_hash_function (
    gconstpointer key
    )
{
    guint hash_value_int;
    /* Calculate the hash value for the stack */
    /* This hash function was ripped from the Perl source code.
     * (It is not derived work however). */
    const char * s_ptr = (char*)key;
    const char * s_end = s_ptr+fcs_col_len((fcs_card_t *)key)+1;
    hash_value_int = 0;
    while (s_ptr < s_end)
    {
        hash_value_int += (hash_value_int << 5) + *(s_ptr++);
    }
    hash_value_int += (hash_value_int >> 5);

    return hash_value_int;
}


static gint fc_solve_glib_hash_stack_compare (
    gconstpointer a,
    gconstpointer b
)
{
    return !(fc_solve_stack_compare_for_comparison(a,b));
}
#endif /* (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH) */


#endif /* defined(INDIRECT_STACK_STATES) */

/*
 * This function traces the solution from the final state down
 * to the initial state
 * */
static void trace_solution(
    fc_solve_instance_t * instance
    )
{
    /*
        Trace the solution.
    */
    fcs_state_extra_info_t * s1_val;
    int move_idx;
    fcs_move_stack_t * stack;
    fcs_internal_move_t * moves;
    fcs_move_stack_t * solution_moves_ptr;

    if (instance->solution_moves.moves != NULL)
    {
        fcs_move_stack_static_destroy(instance->solution_moves);
        instance->solution_moves.moves = NULL;
    }

    fcs_move_stack_init(instance->solution_moves);

    s1_val = instance->final_state_val;

    solution_moves_ptr = &(instance->solution_moves);
    /* Retrace the step from the current state to its parents */
    while (s1_val->parent_val != NULL)
    {
        /* Mark the state as part of the non-optimized solution */
        s1_val->visited |= FCS_VISITED_IN_SOLUTION_PATH;
        /* Duplicate the move stack */
        {
            stack = s1_val->moves_to_parent;
            moves = stack->moves;
            for(move_idx=stack->num_moves-1;move_idx>=0;move_idx--)
            {
                fcs_move_stack_push(solution_moves_ptr, moves[move_idx]);
            }
        }
        /* Duplicate the state to a freshly malloced memory */

        /* Move to the parent state */
        s1_val = s1_val->parent_val;
    }
    /* There's one more state than there are move stacks */
    s1_val->visited |= FCS_VISITED_IN_SOLUTION_PATH;
}


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

    instance->to_reparent_states_real = 1;

    if (! instance->optimization_thread)
    {
        instance->optimization_thread = 
            optimization_thread =
            fc_solve_instance__alloc_hard_thread(instance);

    }
    else
    {
        optimization_thread = instance->optimization_thread;
    }

    soft_thread = optimization_thread->soft_threads;

    if (instance->opt_tests_order_set)
    {
        if (soft_thread->tests_order.tests != NULL)
        {
            free(soft_thread->tests_order.tests);
        }

        soft_thread->tests_order =
            tests_order_dup(&(instance->opt_tests_order));
    }

    soft_thread->method = FCS_METHOD_OPTIMIZE;

    soft_thread->is_a_complete_scan = 1;

    /* Initialize the optimization hard-thread and soft-thread */
    optimization_thread->num_times_left_for_soft_thread = 1000000;

    /* Instruct the optimization hard thread to run indefinitely AFA it
     * is concerned */
    optimization_thread->max_num_times = -1;
    optimization_thread->ht_max_num_times = -1;

    fc_solve_soft_thread_init_a_star_or_bfs(soft_thread);
    soft_thread->initialized = 1;

    instance->in_optimization_thread = 1;
   
    return
        fc_solve_a_star_or_bfs_do_solve(
            soft_thread
            );
}


/*
    This function starts the solution process _for the first time_. If one
    wishes to proceed after the iterations limit was reached, one should
    use fc_solve_resume_instance.

  */
int fc_solve_solve_instance(
    fc_solve_instance_t * instance,
    fcs_state_extra_info_t * init_state_val
    )
{
    fcs_state_t * state_copy_ptr_key;
    fcs_state_extra_info_t * state_copy_ptr_val;

    /* Allocate the first state and initialize it to init_state */
    state_copy_ptr_val = 
        fcs_state_ia_alloc_into_var(
            &(instance->hard_threads[0]->allocator)
        );

    state_copy_ptr_key = state_copy_ptr_val->key;

    fcs_duplicate_state(state_copy_ptr_key, state_copy_ptr_val,
            ((init_state_val->key)), init_state_val
            );

    {
        int a;
        for(a=0;a<INSTANCE_STACKS_NUM;a++)
        {
            fcs_copy_stack(*state_copy_ptr_key, *state_copy_ptr_val, a, instance->hard_threads[0]->indirect_stacks_buffer);
        }
    }

    /* Initialize the state to be a base state for the game tree */
    state_copy_ptr_val->depth = 0;
    state_copy_ptr_val->moves_to_parent = NULL;
    state_copy_ptr_val->visited = 0;
    state_copy_ptr_val->parent_val = NULL;
    memset(&(state_copy_ptr_val->scan_visited), '\0', sizeof(state_copy_ptr_val->scan_visited));

    instance->state_copy_ptr_val = state_copy_ptr_val;

    /* Initialize the data structure that will manage the state collection */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    instance->tree = rbinit(
            fc_solve_state_extra_info_compare_with_context,
            NULL
            );
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE)
    instance->tree = fcs_libavl2_states_tree_create(fc_solve_state_extra_info_compare_with_context, NULL, NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    instance->tree = g_tree_new(fc_solve_state_compare);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY)
    instance->judy_array = ((Pvoid_t)NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    instance->hash = g_hash_table_new(
        fc_solve_hash_function,
        fc_solve_state_compare_equal
        );
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
     fc_solve_hash_init(
            &(instance->hash),
            2048,
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
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    instance->num_prev_states_margin = 0;

    instance->indirect_prev_states = NULL;
#else
#error not defined
#endif

    /****************************************************/

#ifdef INDIRECT_STACK_STATES
    /* Initialize the data structure that will manage the stack
       collection */
#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH
    fc_solve_hash_init(
            &(instance->stacks_hash ),
            2048,
#ifdef FCS_INLINED_HASH_COMPARISON
            FCS_INLINED_HASH__COLUMNS
#else
#ifdef FCS_WITH_CONTEXT_VARIABLE
            fcs_stack_compare_for_comparison_with_context,
            NULL
#else
            fc_solve_stack_compare_for_comparison
#endif
#endif
        );
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)
    instance->stacks_tree = fcs_libavl2_stacks_tree_create(
            fcs_stack_compare_for_comparison_with_context,
            NULL,
            NULL
            );
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    instance->stacks_tree = rbinit(
        fcs_stack_compare_for_comparison_with_context,
        NULL
        );
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
    instance->stacks_tree = g_tree_new(fc_solve_stack_compare_for_comparison);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
    instance->stacks_hash = g_hash_table_new(
        fc_solve_glib_hash_stack_hash_function,
        fc_solve_glib_hash_stack_compare
        );
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)
    instance->stacks_judy_array = NULL;
#else
#error FCS_STACK_STORAGE is not set to a good value.
#endif
#endif

    /***********************************************/

#ifdef FCS_WITH_TALONS
    /* Initialize the Talon's Cache */
    if (instance->talon_type == FCS_TALON_KLONDIKE)
    {
        instance->talons_hash = fc_solve_hash_init(
            512,
            fcs_talon_compare_with_context,
            NULL
            );

        fc_solve_cache_talon(instance, instance->state_copy_ptr_val);
    }
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    /* Not working - ignore */
    db_open(
        NULL,
        DB_BTREE,
        O_CREAT|O_RDWR,
        0777,
        NULL,
        NULL,
        &(instance->db)
        );
#endif

    {
        fcs_state_extra_info_t * no_use_val;

        fc_solve_check_and_add_state(
            &(instance->hard_threads[0]->soft_threads[0]),
            state_copy_ptr_val,
            &no_use_val
            );

    }

    instance->ht_idx = 0;
    {
        int ht_idx;
        for(ht_idx=0; ht_idx < instance->num_hard_threads ; ht_idx++)
        {
            fc_solve_hard_thread_t * hard_thread;
            hard_thread = instance->hard_threads[ht_idx];

            if (hard_thread->prelude != NULL)
            {
                hard_thread->prelude_idx = 0;
                hard_thread->st_idx = hard_thread->prelude[hard_thread->prelude_idx].scan_idx;
                hard_thread->num_times_left_for_soft_thread = hard_thread->prelude[hard_thread->prelude_idx].quota;
                hard_thread->prelude_idx++;
            }
            else
            {
                hard_thread->st_idx = 0;
            }
        }
    }

    instance->to_reparent_states_real = instance->to_reparent_states_proto;

    return fc_solve_resume_instance(instance);
}

static GCC_INLINE void fc_solve_soft_thread_init_soft_dfs(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_instance_t * instance = soft_thread->hard_thread->instance;

    fcs_state_extra_info_t * ptr_orig_state_val = instance->state_copy_ptr_val;
    /*
        Allocate some space for the states at depth 0.
    */
    soft_thread->method_specific.soft_dfs.depth = 0;

    fc_solve_increase_dfs_max_depth(soft_thread);

    /* Initialize the initial state to indicate it is the first */
    ptr_orig_state_val->parent_val = NULL;
    ptr_orig_state_val->moves_to_parent = NULL;
    ptr_orig_state_val->depth = 0;

    soft_thread->method_specific.soft_dfs.soft_dfs_info[0].state_val = ptr_orig_state_val;
    fc_solve_rand_init(
            &(soft_thread->method_specific.soft_dfs.rand_gen), 
            soft_thread->method_specific.soft_dfs.rand_seed
    );

    return;
}

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
        if (soft_thread->is_finished)
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

            if (! soft_thread->initialized)
            {
                fc_solve_soft_thread_init_soft_dfs(soft_thread);
                soft_thread->initialized = 1;
            }

            ret =
                fc_solve_soft_dfs_do_solve(
                    soft_thread,
                    (soft_thread->method == FCS_METHOD_RANDOM_DFS)
                    );

            break;

            case FCS_METHOD_BFS:
            case FCS_METHOD_A_STAR:
            case FCS_METHOD_OPTIMIZE:

            if (! soft_thread->initialized)
            {
                fc_solve_soft_thread_init_a_star_or_bfs(soft_thread);

                soft_thread->initialized = 1;
            }

            ret = fc_solve_a_star_or_bfs_do_solve(soft_thread);

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
            soft_thread->is_finished = 1;
            hard_thread->num_soft_threads_finished++;
            if (hard_thread->num_soft_threads_finished == hard_thread->num_soft_threads)
            {
                instance->num_hard_threads_finished++;
            }
            /*
             * Check if this thread is a complete scan and if so,
             * terminate the search
             * */
            if (soft_thread->is_a_complete_scan)
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
                        (instance->max_num_times > 0) &&
                        (instance->num_times >= instance->max_num_times)
                    ) ||
                    (
                        (instance->max_num_states_in_collection > 0) &&
                        (instance->num_states_in_collection >= instance->max_num_states_in_collection)

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


/* Resume a solution process that was stopped in the middle */
int fc_solve_resume_instance(
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
    if (instance->in_optimization_thread)
    {
        ret =
            fc_solve_a_star_or_bfs_do_solve(
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
                hard_thread = instance->hard_threads[instance->ht_idx];

                ret = run_hard_thread(hard_thread);
                if ((ret == FCS_STATE_IS_NOT_SOLVEABLE) ||
                    (ret == FCS_STATE_WAS_SOLVED) ||
                    (
                        (ret == FCS_STATE_SUSPEND_PROCESS) &&
                        /* There's a limit to the scan only
                         * if max_num_times is greater than 0 */
                        (
                            (
                                (instance->max_num_times > 0) &&
                                (instance->num_times >= instance->max_num_times)
                            ) ||
                            (
                                (instance->max_num_states_in_collection > 0) &&
                                (instance->num_states_in_collection >= instance->max_num_states_in_collection)

                            )
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
            trace_solution(instance);
        }
    }


    if (ret == FCS_STATE_WAS_SOLVED)
    {
        if (instance->optimize_solution_path)
        {
            /* Call optimize_solution only once. Make sure that if
             * it has already run - we retain the old ret. */
            if (! instance->in_optimization_thread)
            {
                ret = fc_solve_optimize_solution(instance);
            }
            if (ret == FCS_STATE_WAS_SOLVED)
            {
                /* Create the solution_moves in the first place */
                trace_solution(instance);
            }
        }
    }

    return ret;
}



/***********************************************************/

static GCC_INLINE void finish_hard_thread(
    fc_solve_hard_thread_t * hard_thread
    )
{
    fc_solve_compact_allocator_finish(&(hard_thread->allocator));
    hard_thread->allocator.packs = NULL;

    return;
}

/*
    This function should be called after the user has retrieved the
    results generated by the scan as it will destroy them.
  */
void fc_solve_finish_instance(
    fc_solve_instance_t * instance
    )
{
    int ht_idx;


    /* De-allocate the state packs */
    for(ht_idx=0;ht_idx<instance->num_hard_threads;ht_idx++)
    {
        finish_hard_thread(instance->hard_threads[ht_idx]);
    }

    if (instance->optimization_thread)
    {
        finish_hard_thread(instance->optimization_thread);
    }


    /* De-allocate the state collection */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    rbdestroy(instance->tree);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE)
    fcs_libavl2_states_tree_destroy(instance->tree, NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    g_tree_destroy(instance->tree);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY)
    {
        Word_t rc_word;
        JHSFA(rc_word, instance->judy_array);
    }
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    g_hash_table_destroy(instance->hash);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    fc_solve_hash_free(&(instance->hash));
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    instance->num_prev_states_margin = 0;

    instance->num_indirect_prev_states = 0;

    free(instance->indirect_prev_states);
    instance->indirect_prev_states = NULL;
#else
#error not defined
#endif

    /* De-allocate the stack collection while free()'ing the stacks
    in the process */
#ifdef INDIRECT_STACK_STATES
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH)
    fc_solve_hash_free(&(instance->stacks_hash));
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)
    fcs_libavl2_stacks_tree_destroy(instance->stacks_tree, NULL);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    rbdestroy(instance->stacks_tree);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
    g_tree_destroy(instance->stacks_tree);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
    g_hash_table_destroy(instance->stacks_hash);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)
    {
        Word_t rc_word;
        JHSFA(rc_word, instance->stacks_judy_array);
    }
#else
#error FCS_STACK_STORAGE is not set to a good value.
#endif
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    instance->db->close(instance->db,0);
#endif


    clean_soft_dfs(instance);
}

fc_solve_soft_thread_t * fc_solve_new_soft_thread(
    fc_solve_hard_thread_t * hard_thread
    )
{
    fc_solve_soft_thread_t * ret;
    
    /* Make sure we are not exceeding the maximal number of soft threads
     * for an instance. */
    if (hard_thread->instance->next_soft_thread_id == MAX_NUM_SCANS)
    {
        return NULL;
    }

    hard_thread->soft_threads = realloc(hard_thread->soft_threads, sizeof(hard_thread->soft_threads[0])*(hard_thread->num_soft_threads+1));

    init_soft_thread(
            hard_thread, 
            (ret = &(hard_thread->soft_threads[hard_thread->num_soft_threads]))
            );

    hard_thread->num_soft_threads++;

    return ret;
}


