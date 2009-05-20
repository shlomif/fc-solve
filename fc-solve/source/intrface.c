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
 * intrface.c - instance interface functions for Freecell Solver
 *
 */

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
#include "fcs_dm.h"
#include "fcs.h"

#include "fcs_isa.h"

#include "caas.h"

#include "preset.h"
#include "unused.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

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

#if 0
static const double fc_solve_a_star_default_weights[5] = {0.5,0,0.5,0,0};
#else
static const double fc_solve_a_star_default_weights[5] = {0.5,0,0.3,0,0.2};
#endif







static void fc_solve_initialize_bfs_queue(fc_solve_soft_thread_t * soft_thread)
{
    /* Initialize the BFS queue. We have one dummy element at the beginning
       in order to make operations simpler. */
    soft_thread->bfs_queue = (fcs_states_linked_list_item_t*)malloc(sizeof(fcs_states_linked_list_item_t));
    soft_thread->bfs_queue->next = (fcs_states_linked_list_item_t*)malloc(sizeof(fcs_states_linked_list_item_t));
    soft_thread->bfs_queue_last_item = soft_thread->bfs_queue->next;
    soft_thread->bfs_queue_last_item->next = NULL;
}

static void foreach_soft_thread(
    fc_solve_instance_t * instance,
    void (*soft_thread_callback)(
        fc_solve_soft_thread_t * soft_thread,
        void * context
        ),
    void * context
    )

{
    int ht_idx, st_idx;
    fc_solve_hard_thread_t * hard_thread;
    int num_soft_threads;
    fc_solve_soft_thread_t * * ht_soft_threads;
    for(ht_idx = 0 ; ht_idx<instance->num_hard_threads; ht_idx++)
    {
        hard_thread = instance->hard_threads[ht_idx];
        num_soft_threads = hard_thread->num_soft_threads;
        ht_soft_threads = hard_thread->soft_threads;
        for(st_idx = 0 ; st_idx < num_soft_threads; st_idx++)
        {
            soft_thread_callback(ht_soft_threads[st_idx], context);
        }
    }

    if (instance->optimization_thread)
    {
        soft_thread_callback(instance->optimization_thread->soft_threads[0], context);
    }
}



static void soft_thread_clean_soft_dfs(
    fc_solve_soft_thread_t * soft_thread,
    void * context GCC_UNUSED
    )
{
    int max_depth;
    int dfs_max_depth;
    fcs_soft_dfs_stack_item_t * soft_dfs_info, * info_ptr;
    /* Check if a Soft-DFS-type scan was called in the first place */
    if (soft_thread->soft_dfs_info == NULL)
    {
        /* If not - do nothing */
        return;
    }

    soft_dfs_info = soft_thread->soft_dfs_info;
    max_depth = soft_thread->depth;
    dfs_max_depth = soft_thread->dfs_max_depth;
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

        soft_thread->soft_dfs_info = NULL;

        soft_thread->dfs_max_depth = 0;

    }
}

static void clean_soft_dfs(
        fc_solve_instance_t * instance
        )
{
    foreach_soft_thread(instance, soft_thread_clean_soft_dfs, NULL);
}

static fc_solve_soft_thread_t * alloc_soft_thread(
        fc_solve_hard_thread_t * hard_thread
        )
{
    fc_solve_soft_thread_t * soft_thread;
    int a;

    /* Make sure we are not exceeding the maximal number of soft threads
     * for an instance. */
    if (hard_thread->instance->next_soft_thread_id == MAX_NUM_SCANS)
    {
        return NULL;
    }

    soft_thread = malloc(sizeof(fc_solve_soft_thread_t));

    soft_thread->hard_thread = hard_thread;

    soft_thread->id = (hard_thread->instance->next_soft_thread_id)++;

    soft_thread->dfs_max_depth = 0;

    soft_thread->tests_order.num = 0;
    soft_thread->tests_order.tests = NULL;

    /* Initialize all the Soft-DFS stacks to NULL */
    soft_thread->soft_dfs_info = NULL;

    /* The default solving method */
    soft_thread->method = FCS_METHOD_SOFT_DFS;

    soft_thread->orig_method = FCS_METHOD_NONE;

    fc_solve_initialize_bfs_queue(soft_thread);

    /* Initialize the priotity queue of the A* scan */
    soft_thread->a_star_pqueue = malloc(sizeof(PQUEUE));
    fc_solve_PQueueInitialise(
        soft_thread->a_star_pqueue,
        1024
        );

    soft_thread->a_star_positions_by_rank = NULL;

    /* Set the default A* weigths */
    for(a=0;a<(sizeof(soft_thread->a_star_weights)/sizeof(soft_thread->a_star_weights[0]));a++)
    {
        soft_thread->a_star_weights[a] = fc_solve_a_star_default_weights[a];
    }

    soft_thread->rand_gen = fc_solve_rand_alloc(soft_thread->rand_seed = 24);

    soft_thread->initialized = 0;

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

    soft_thread->is_finished = 0;

    soft_thread->name = NULL;

    return soft_thread;
}

static fc_solve_hard_thread_t * alloc_hard_thread(
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

    hard_thread->num_times = 0;

    hard_thread->num_soft_threads = 1;

    hard_thread->soft_threads =
        malloc(sizeof(hard_thread->soft_threads[0]) *
               hard_thread->num_soft_threads
        );

    hard_thread->soft_threads[0] = alloc_soft_thread(hard_thread);

    /* Set a limit on the Hard-Thread's scan. */
    hard_thread->num_times_step = NUM_TIMES_STEP;

    hard_thread->ht_max_num_times = hard_thread->num_times_step;

    hard_thread->max_num_times = -1;

    hard_thread->num_soft_threads_finished = 0;

#ifdef INDIRECT_STACK_STATES
    hard_thread->stacks_allocator =
        fc_solve_compact_allocator_new();
#endif
    hard_thread->move_stacks_allocator =
        fc_solve_compact_allocator_new();

    fcs_move_stack_alloc_into_var(hard_thread->reusable_move_stack);

    hard_thread->prelude_as_string = NULL;
    hard_thread->prelude = NULL;
    hard_thread->prelude_num_items = 0;
    hard_thread->prelude_idx = 0;

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

    fc_solve_apply_preset_by_name(instance, "freecell");

    /****************************************/

    instance->debug_iter_output = 0;

    instance->next_soft_thread_id = 0;

    instance->num_hard_threads = 1;

    instance->hard_threads = malloc(sizeof(instance->hard_threads[0]) * instance->num_hard_threads);

    instance->hard_threads[0] = alloc_hard_thread(instance);

    instance->solution_moves = NULL;

    instance->optimize_solution_path = 0;

#ifdef FCS_WITH_MHASH
    instance->mhash_type = MHASH_MD5;
#endif

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





static void free_bfs_queue(fc_solve_soft_thread_t * soft_thread)
{
    /* Free the BFS linked list */
    fcs_states_linked_list_item_t * item, * next_item;
    item = soft_thread->bfs_queue;
    while (item != NULL)
    {
        next_item = item->next;
        free(item);
        item = next_item;
    }
}

static void free_instance_soft_thread_callback(
        fc_solve_soft_thread_t * soft_thread,
        void * context GCC_UNUSED
        )
{
    free_bfs_queue(soft_thread);
    fc_solve_rand_free(soft_thread->rand_gen);

    fc_solve_PQueueFree(soft_thread->a_star_pqueue);
    free(soft_thread->a_star_pqueue);

    free(soft_thread->tests_order.tests);

    if (soft_thread->name != NULL)
    {
        free(soft_thread->name);
    }
    /* The data-structure itself was allocated */
    free(soft_thread);
}

static void free_instance_hard_thread_callback(fc_solve_hard_thread_t * hard_thread)
{
    if (hard_thread->prelude_as_string)
    {
        free (hard_thread->prelude_as_string);
    }
    if (hard_thread->prelude)
    {
        free (hard_thread->prelude);
    }
    fcs_move_stack_destroy(hard_thread->reusable_move_stack);

    free(hard_thread->soft_threads);

    if (hard_thread->move_stacks_allocator)
    {
        fc_solve_compact_allocator_finish(hard_thread->move_stacks_allocator);
    }
#ifdef INDIRECT_STACK_STATES
    if (hard_thread->stacks_allocator)
    {
        fc_solve_compact_allocator_finish(hard_thread->stacks_allocator);
    }
#endif
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

    foreach_soft_thread(instance, free_instance_soft_thread_callback, NULL);

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


static void normalize_a_star_weights(
    fc_solve_soft_thread_t * soft_thread,
    void * context GCC_UNUSED
    )
{
    /* Normalize the A* Weights, so the sum of all of them would be 1. */
    double sum;
    int a;
    sum = 0;
    for(a=0;a<(sizeof(soft_thread->a_star_weights)/sizeof(soft_thread->a_star_weights[0]));a++)
    {
        if (soft_thread->a_star_weights[a] < 0)
        {
            soft_thread->a_star_weights[a] = fc_solve_a_star_default_weights[a];
        }
        sum += soft_thread->a_star_weights[a];
    }
    if (sum < 1e-6)
    {
        sum = 1;
    }
    for(a=0;a<(sizeof(soft_thread->a_star_weights)/sizeof(soft_thread->a_star_weights[0]));a++)
    {
        soft_thread->a_star_weights[a] /= sum;
    }
}

static void accumulate_tests_order(
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

static void determine_scan_completeness(
    fc_solve_soft_thread_t * soft_thread,
    void * context
    )
{
    int global_tests_order = *(int *)context;
    int tests_order = 0;
    int a;
    for(a=0;a<soft_thread->tests_order.num;a++)
    {
        tests_order |= (1 << (soft_thread->tests_order.tests[a] & FCS_TEST_ORDER_NO_FLAGS_MASK));
    }
    soft_thread->is_a_complete_scan = (tests_order == global_tests_order);
}

enum FCS_COMPILE_PRELUDE_ERRORS_T
{
    FCS_COMPILE_PRELUDE_OK,
    FCS_COMPILE_PRELUDE_NO_AT_SIGN,
    FCS_COMPILE_PRELUDE_UNKNOWN_SCAN_ID
};

static int compile_prelude(
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
            if (!strcmp(hard_thread->soft_threads[st_idx]->name, p_scan))
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
            hard_thread->soft_threads[0]->num_times_step;
        fc_solve_state_ia_init(hard_thread);
    }

    if (instance->optimization_thread)
    {
        fc_solve_state_ia_init(instance->optimization_thread);
    }

    /* Normalize the A* Weights, so the sum of all of them would be 1. */
    foreach_soft_thread(instance, normalize_a_star_weights, NULL);

    {
        int total_tests = 0;
        foreach_soft_thread(instance, accumulate_tests_order, &total_tests);
        foreach_soft_thread(instance, determine_scan_completeness, &total_tests);
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
    const char * s_end = s_ptr+fcs_standalone_stack_len((fcs_card_t *)key)+1;
    hash_value_int = 0;
    while (s_ptr < s_end)
    {
        hash_value_int += (hash_value_int << 5) + *(s_ptr++);
    }
    hash_value_int += (hash_value_int >> 5);

}





static gint fc_solve_glib_hash_stack_compare (
    gconstpointer a,
    gconstpointer b
)
{
    return !(fcs_stack_compare_for_comparison(a,b));
}
#endif /* (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH) */





#endif /* defined(INDIRECT_STACK_STATES) */





#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
/*
 * This hash function is defined in caas.c
 *
 * */
extern guint fc_solve_hash_function(gconstpointer key);
#endif

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
    fcs_move_stack_t * solution_moves;
    int move_idx;
    fcs_move_stack_t * stack;
    fcs_move_t * moves;

    if (instance->solution_moves != NULL)
    {
        fcs_move_stack_destroy(instance->solution_moves);
        instance->solution_moves = NULL;
    }

    fcs_move_stack_alloc_into_var(solution_moves);
    instance->solution_moves = solution_moves;

    s1_val = instance->final_state_val;

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
                fcs_move_stack_push(solution_moves, moves[move_idx]);
            }
        }
        /* Duplicate the state to a freshly malloced memory */

        /* Move to the parent state */
        s1_val = s1_val->parent_val;
    }
    /* There's one more state than there are move stacks */
    s1_val->visited |= FCS_VISITED_IN_SOLUTION_PATH;
}


static fcs_tests_order_t tests_order_dup(fcs_tests_order_t * orig)
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
static int fc_solve_optimize_solution(
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
            alloc_hard_thread(instance);

        fc_solve_state_ia_init(optimization_thread);
    }
    else
    {
        optimization_thread = instance->optimization_thread;
    }

    soft_thread = optimization_thread->soft_threads[0];

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


#ifdef FCS_WITH_TALONS

extern void fc_solve_cache_talon(
    fc_solve_instance_t * instance,
    fcs_state_t * new_state_key,
    fcs_state_extra_info_t * new_state_val
    );

#endif

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
    fcs_state_ia_alloc_into_var(
            state_copy_ptr_val,
            instance->hard_threads[0]
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
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE)
    instance->tree = avl_create(fc_solve_state_compare_with_context, NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE)
    instance->tree = rb_create(fc_solve_state_compare_with_context, NULL);
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
    instance->hash = fc_solve_hash_init(
            2048,
            fc_solve_state_compare_with_context,
            NULL
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
    instance->stacks_hash = fc_solve_hash_init(
            2048,
            fcs_stack_compare_for_comparison_with_context,
            NULL
        );
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_AVL_TREE)
    instance->stacks_tree = avl_create(
            fcs_stack_compare_for_comparison_with_context,
            NULL
            );
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE)
    instance->stacks_tree = rb_create(
            fcs_stack_compare_for_comparison_with_context,
            NULL
            );
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    instance->stacks_tree = rbinit(
        fcs_stack_compare_for_comparison_with_context,
        NULL
        );
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
    instance->stacks_tree = g_tree_new(fcs_stack_compare_for_comparison);
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

        fc_solve_cache_talon(instance, instance->state_copy_ptr);
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
            instance->hard_threads[0]->soft_threads[0],
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

static int run_hard_thread(fc_solve_hard_thread_t * hard_thread)
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
        soft_thread = hard_thread->soft_threads[hard_thread->st_idx];
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
                hard_thread->num_times_left_for_soft_thread = hard_thread->soft_threads[hard_thread->st_idx]->num_times_step;  \
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
                instance->optimization_thread->soft_threads[0]
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



/*
    Clean up a solving process that was terminated in the middle.
    This function does not substitute for later calling
    finish_instance() and free_instance().
  */
void fc_solve_unresume_instance(
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

/***********************************************************/

static void finish_hard_thread(
    fc_solve_hard_thread_t * hard_thread
    )
{
    fc_solve_state_ia_finish(hard_thread);

#ifdef INDIRECT_STACK_STATES
    fc_solve_compact_allocator_finish(hard_thread->stacks_allocator);
    hard_thread->stacks_allocator = NULL;
#endif

    fc_solve_compact_allocator_finish(hard_thread->move_stacks_allocator);
    hard_thread->move_stacks_allocator = NULL;

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
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE)
    avl_destroy(instance->tree, NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE)
    rb_destroy(instance->tree, NULL);
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
    fc_solve_hash_free(instance->hash);
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
#if 0
    fc_solve_hash_free_with_callback(instance->stacks_hash, fc_solve_stack_free);
#else
    fc_solve_hash_free(instance->stacks_hash);
#endif
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_AVL_TREE)
    avl_destroy(instance->stacks_tree, NULL);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE)
    rb_destroy(instance->stacks_tree, NULL);
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

fc_solve_soft_thread_t * fc_solve_instance_get_soft_thread(
        fc_solve_instance_t * instance,
        int ht_idx,
        int st_idx
        )
{
    if (ht_idx >= instance->num_hard_threads)
    {
        return NULL;
    }
    else
    {
        fc_solve_hard_thread_t * hard_thread;
        hard_thread = instance->hard_threads[ht_idx];
        if (st_idx >= hard_thread->num_soft_threads)
        {
            return NULL;
        }
        else
        {
            return hard_thread->soft_threads[st_idx];
        }
    }
}

fc_solve_soft_thread_t * fc_solve_new_soft_thread(
    fc_solve_soft_thread_t * soft_thread
    )
{
    fc_solve_soft_thread_t * ret;
    fc_solve_hard_thread_t * hard_thread;

    hard_thread = soft_thread->hard_thread;
    ret = alloc_soft_thread(hard_thread);

    /* Exceeded the maximal number of Soft-Threads in an instance */
    if (ret == NULL)
    {
        return NULL;
    }

    hard_thread->soft_threads = realloc(hard_thread->soft_threads, sizeof(hard_thread->soft_threads[0])*(hard_thread->num_soft_threads+1));
    hard_thread->soft_threads[hard_thread->num_soft_threads] = ret;
    hard_thread->num_soft_threads++;

    return ret;
}

fc_solve_soft_thread_t * fc_solve_new_hard_thread(
    fc_solve_instance_t * instance
    )
{
    fc_solve_hard_thread_t * ret;

    /* Exceeded the maximal number of Soft-Threads in an instance */
    ret = alloc_hard_thread(instance);

    if (ret == NULL)
    {
        return NULL;
    }

    instance->hard_threads =
        realloc(
            instance->hard_threads,
            (sizeof(instance->hard_threads[0]) * (instance->num_hard_threads+1))
            );

    instance->hard_threads[instance->num_hard_threads] = ret;

    instance->num_hard_threads++;

    return ret->soft_threads[0];
}

static void recycle_hard_thread(
    fc_solve_hard_thread_t * hard_thread
    )
{
    int st_idx;
    fc_solve_soft_thread_t * soft_thread;

    hard_thread->num_times = 0;
    hard_thread->ht_max_num_times = hard_thread->num_times_step;
    hard_thread->max_num_times = -1;
    hard_thread->num_soft_threads_finished = 0;
    hard_thread->move_stacks_allocator =
        fc_solve_compact_allocator_new();
#ifdef INDIRECT_STACK_STATES
    hard_thread->stacks_allocator =
        fc_solve_compact_allocator_new();
#endif
    for(st_idx = 0; st_idx < hard_thread->num_soft_threads ; st_idx++)
    {
        soft_thread = hard_thread->soft_threads[st_idx];
        soft_thread->is_finished = 0;
        soft_thread->initialized = 0;

        fc_solve_rand_srand(soft_thread->rand_gen, soft_thread->rand_seed);
        /* Reset the priority queue */
        soft_thread->a_star_pqueue->CurrentSize = 0;
        /* Rest the BFS Queue (also used for the optimization scan. */
        free_bfs_queue(soft_thread);
        fc_solve_initialize_bfs_queue(soft_thread);
    }

    return;
}

void fc_solve_recycle_instance(
    fc_solve_instance_t * instance
        )
{
    int ht_idx;

    fc_solve_finish_instance(instance);

    instance->num_times = 0;

    instance->num_hard_threads_finished = 0;

    for(ht_idx = 0;  ht_idx < instance->num_hard_threads; ht_idx++)
    {
        recycle_hard_thread(instance->hard_threads[ht_idx]);
    }
#if 1
    if (instance->optimization_thread)
    {
        recycle_hard_thread(instance->optimization_thread);
    }
    instance->in_optimization_thread = 0;
#else
    /*
        For the time being instance->optimization_thread must be NULL when
        the solution has started. Else, it breaks the internal code's
        expectations.
    */    
    if (instance->optimization_thread)
    {
        free_instance_hard_thread_callback(instance->optimization_thread);
        instance->optimization_thread = NULL;
    }
#endif
}

