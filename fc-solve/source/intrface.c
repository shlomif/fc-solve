/*
 * intrface.c - instance interface functions for Freecell Solver
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000-2001
 *
 * This file is in the public domain (it's uncopyrighted).
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

/* So the FCS_STATE_STORAGE macros would be defined */
#if FCS_STATE_STORAGE==FCS_STATE_STORAGE_LIBREDBLACK_TREE
#include <search.h>
#endif

#include "state.h"
#include "card.h"
#include "fcs_dm.h"
#include "fcs.h"

#include "fcs_isa.h"

#include "caas.h"

#include "preset.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/*
    General use of this interface:
    1. freecell_solver_alloc_instance()
    2. Set the parameters of the game
    3. If you wish to revert, go to step #11.
    4. freecell_solver_init_instance()
    5. Call freecell_solver_solve_instance() with the initial board.
    6. If it returns FCS_STATE_SUSPEND_PROCESS and you wish to proceed,
       then increase the iteration limit and call
       freecell_solver_resume_instance().
    7. Repeat Step #6 zero or more times.
    8. If the last call to solve_instance() or resume_instance() returned
       FCS_STATE_SUSPEND_PROCESS then call
       freecell_solver_unresume_instance().
    9. If the solving was successful you can use the move stacks or the
       intermediate stacks. (Just don't destory them in any way).
    10. Call freecell_solver_finish_instance().
    11. Call freecell_solver_free_instance().

    The library functions inside lib.c (a.k.a fcs_user()) give an
    easier approach for embedding Freecell Solver into your library. The
    intent of this comment is to document the code, rather than to be
    a guideline for the user.
*/

#if 0
static const double freecell_solver_a_star_default_weights[5] = {0.5,0,0.5,0,0};
#else
static const double freecell_solver_a_star_default_weights[5] = {0.5,0,0.3,0,0.2};
#endif







static void freecell_solver_initialize_bfs_queue(freecell_solver_soft_thread_t * soft_thread)
{
    /* Initialize the BFS queue. We have one dummy element at the beginning
       in order to make operations simpler. */
    soft_thread->bfs_queue = (fcs_states_linked_list_item_t*)malloc(sizeof(fcs_states_linked_list_item_t));
    soft_thread->bfs_queue->next = (fcs_states_linked_list_item_t*)malloc(sizeof(fcs_states_linked_list_item_t));
    soft_thread->bfs_queue_last_item = soft_thread->bfs_queue->next;
    soft_thread->bfs_queue_last_item->next = NULL;
}

static void foreach_soft_thread(
    freecell_solver_instance_t * instance,
    void (*soft_thread_callback)(
        freecell_solver_soft_thread_t * soft_thread,
        void * context
        ),
    void * context
    )

{
    int ht_idx, st_idx;
    freecell_solver_hard_thread_t * hard_thread;
    int num_soft_threads;
    freecell_solver_soft_thread_t * * ht_soft_threads;
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
    freecell_solver_soft_thread_t * soft_thread,
    void * context
    )
{
    int num_solution_states, num_states;
    int dfs_max_depth;
    int a;
    fcs_soft_dfs_stack_item_t * soft_dfs_info, * info_ptr;
    fcs_derived_state_t * derived_states;

    /* Check if a Soft-DFS-type scan was called in the first place */
    if (soft_thread->soft_dfs_info == NULL)
    {
        /* If not - do nothing */
        return;
    }

    soft_dfs_info = soft_thread->soft_dfs_info;
    num_solution_states = soft_thread->num_solution_states;
    dfs_max_depth = soft_thread->dfs_max_depth;
    /* De-allocate the Soft-DFS specific stacks */
    {
        int depth;
        info_ptr = soft_dfs_info;
        for(depth=0;depth<dfs_max_depth;depth++)
        {
            if (info_ptr->derived_states_list.max_num_states)
            {
                fcs_derived_states_list_free_move_stacks(&(info_ptr->derived_states_list));

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
        freecell_solver_instance_t * instance
        )
{
    foreach_soft_thread(instance, soft_thread_clean_soft_dfs, NULL);
}


static fcs_tests_order_t tests_order_dup(fcs_tests_order_t * orig)
{
    fcs_tests_order_t ret;
    int i;

    ret.max_num = ret.num = orig->num;
    ret.tests = malloc(sizeof(ret.tests[0]) * ret.num);
    memcpy(ret.tests, orig->tests, sizeof(ret.tests[0]) * ret.num);
    
    for(i=0;i<ret.num;i++)
    {
        if (ret.tests[i].states_order_instance)
        {
            ret.tests[i].states_order_instance = 
                ret.tests[i].states_order_type->duplicate_instance(
                    ret.tests[i].states_order_instance 
                    );
        }
    }
    
    return ret;
}

static freecell_solver_soft_thread_t * alloc_soft_thread(
        freecell_solver_hard_thread_t * hard_thread
        )
{
    freecell_solver_soft_thread_t * soft_thread;
    int a;

    /* Make sure we are not exceeding the maximal number of soft threads
     * for an instance. */
    if (hard_thread->instance->next_soft_thread_id == MAX_NUM_SCANS)
    {
        return NULL;
    }

    soft_thread = malloc(sizeof(freecell_solver_soft_thread_t));

    soft_thread->hard_thread = hard_thread;

    soft_thread->id = (hard_thread->instance->next_soft_thread_id)++;

    soft_thread->dfs_max_depth = 0;

    soft_thread->tests_order.num = 0;
    soft_thread->tests_order.tests = NULL;
    soft_thread->tests_order.max_num = 0;
    

    /* Initialize all the Soft-DFS stacks to NULL */
    soft_thread->soft_dfs_info = NULL;

    /* The default solving method */
    soft_thread->method = FCS_METHOD_SOFT_DFS;

    soft_thread->orig_method = FCS_METHOD_NONE;

    freecell_solver_initialize_bfs_queue(soft_thread);

    /* Initialize the priotity queue of the A* scan */
    soft_thread->a_star_pqueue = malloc(sizeof(PQUEUE));
    freecell_solver_PQueueInitialise(
        soft_thread->a_star_pqueue,
        1024
        );

    /* Set the default A* weigths */
    for(a=0;a<(int)(sizeof(soft_thread->a_star_weights)/sizeof(soft_thread->a_star_weights[0]));a++)
    {
        soft_thread->a_star_weights[a] = freecell_solver_a_star_default_weights[a];
    }

    soft_thread->rand_gen = freecell_solver_rand_alloc(soft_thread->rand_seed = 24);

    soft_thread->initialized = 0;

    soft_thread->num_times_step = NUM_TIMES_STEP;

#if 0
    {
        char * no_use;
        freecell_solver_apply_tests_order(soft_thread, "[01][23456789]", &no_use);
    }
#else
    soft_thread->tests_order = tests_order_dup(&(soft_thread->hard_thread->instance->instance_tests_order));
#if 0
    soft_thread->tests_order.num = soft_thread->hard_thread->instance->instance_tests_order.num;
    soft_thread->tests_order.tests = 
        malloc(sizeof(soft_thread->tests_order.tests[0]) * soft_thread->tests_order.num);
    memcpy(soft_thread->tests_order.tests, 
        soft_thread->hard_thread->instance->instance_tests_order.tests, 
        sizeof(soft_thread->tests_order.tests[0]) * soft_thread->tests_order.num
        );
    soft_thread->tests_order.max_num = soft_thread->tests_order.num;
#endif
#endif

    soft_thread->is_finished = 0;

    soft_thread->name = NULL;

    return soft_thread;
}

static freecell_solver_hard_thread_t * alloc_hard_thread(
        freecell_solver_instance_t * instance
        )
{
    freecell_solver_hard_thread_t * hard_thread;

    /* Make sure we are not exceeding the maximal number of soft threads
     * for an instance. */
    if (instance->next_soft_thread_id == MAX_NUM_SCANS)
    {
        return NULL;
    }

    hard_thread = malloc(sizeof(freecell_solver_hard_thread_t));

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
        freecell_solver_compact_allocator_new();
#endif
    hard_thread->move_stacks_allocator =
        freecell_solver_compact_allocator_new();

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

    Afterwards freecell_solver_init_instance() should be called in order
    to really prepare it for solving.
  */
freecell_solver_instance_t * freecell_solver_alloc_instance(void)
{
    freecell_solver_instance_t * instance;

    instance = malloc(sizeof(freecell_solver_instance_t));

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    instance->num_indirect_prev_states = 0;
    instance->max_num_indirect_prev_states = 0;
#endif

    instance->num_times = 0;

    instance->num_states_in_collection = 0;

    instance->max_num_times = -1;
    instance->max_depth = -1;
    instance->max_num_states_in_collection = -1;

    instance->instance_tests_order.num = 0;
    instance->instance_tests_order.tests = NULL;
    instance->instance_tests_order.max_num = 0;

    instance->opt_tests_order_set = 0;

    instance->opt_tests_order.num = 0;
    instance->opt_tests_order.tests = NULL;
    instance->opt_tests_order.max_num = 0;
    
    

#ifdef FCS_WITH_TALONS
    instance->talon_type = FCS_TALON_NONE;
#endif

    instance->num_hard_threads = 0;

    freecell_solver_apply_preset_by_name(instance, "freecell");

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

    instance->num_hard_threads_finished = 0;

    instance->calc_real_depth = 0;

    instance->to_reparent_states = 0;

    /* Make the 1 the default, because otherwise scans will not cooperate
     * with one another. */
    instance->scans_synergy = 1;

    return instance;
}





static void free_bfs_queue(freecell_solver_soft_thread_t * soft_thread)
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

void freecell_solver_free_tests_order(fcs_tests_order_t * tests_order)
{
    /* 
     * Free the tests order states_order instances
     * */
    {
        int num, i;
        fcs_test_t * tests;
        
        num = tests_order->num;
        tests = tests_order->tests;
        
        for(i=0;i<num;i++)
        {
            if (tests[i].states_order_instance)
            {
                tests[i].states_order_type->free_instance(
                    tests[i].states_order_instance
                    );
                tests[i].states_order_instance = NULL;
            }
        }
        /* Free the tests array itself */
        free(tests);
    }    
}

static void free_instance_soft_thread_callback(freecell_solver_soft_thread_t * soft_thread, void * context)
{
    free_bfs_queue(soft_thread);
    freecell_solver_rand_free(soft_thread->rand_gen);

    freecell_solver_PQueueFree(soft_thread->a_star_pqueue);
    free(soft_thread->a_star_pqueue);

    freecell_solver_free_tests_order(&(soft_thread->tests_order));

    if (soft_thread->name != NULL)
    {
        free(soft_thread->name);
    }
    /* The data-structure itself was allocated */
    free(soft_thread);
}

static void free_instance_hard_thread_callback(freecell_solver_hard_thread_t * hard_thread)
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
        freecell_solver_compact_allocator_finish(hard_thread->move_stacks_allocator);
    }
#ifdef INDIRECT_STACK_STATES
    if (hard_thread->stacks_allocator)
    {
        freecell_solver_compact_allocator_finish(hard_thread->stacks_allocator);
    }
#endif
    free(hard_thread);    
}

/*
    This function is the last function that should be called in the
    sequence of operations on instance, and it is meant for de-allocating
    whatever memory was allocated by alloc_instance().
  */
void freecell_solver_free_instance(freecell_solver_instance_t * instance)
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

    freecell_solver_free_tests_order(&(instance->instance_tests_order));

    if (instance->opt_tests_order_set)
    {
        freecell_solver_free_tests_order(&(instance->opt_tests_order));
    }

    free(instance);
}


static void normalize_a_star_weights(
    freecell_solver_soft_thread_t * soft_thread,
    void * context
    )
{
    /* Normalize the A* Weights, so the sum of all of them would be 1. */
    double sum;
    int a;
    sum = 0;
    for(a=0;a<(int)(sizeof(soft_thread->a_star_weights)/sizeof(soft_thread->a_star_weights[0]));a++)
    {
        if (soft_thread->a_star_weights[a] < 0)
        {
            soft_thread->a_star_weights[a] = freecell_solver_a_star_default_weights[a];
        }
        sum += soft_thread->a_star_weights[a];
    }
    if (sum == 0)
    {
        sum = 1;
    }
    for(a=0;a<(int)(sizeof(soft_thread->a_star_weights)/sizeof(soft_thread->a_star_weights[0]));a++)
    {
        soft_thread->a_star_weights[a] /= sum;
    }
}

static void accumulate_tests_order(
    freecell_solver_soft_thread_t * soft_thread,
    void * context
    )
{
    int * tests_order = (int *)context;
    int a;
    for(a=0;a<soft_thread->tests_order.num;a++)
    {
        *tests_order |= (1 << (soft_thread->tests_order.tests[a].test & FCS_TEST_ORDER_NO_FLAGS_MASK));
    }
}

static void determine_scan_completeness(
    freecell_solver_soft_thread_t * soft_thread,
    void * context
    )
{
    int global_tests_order = *(int *)context;
    int tests_order = 0;
    int a;
    for(a=0;a<soft_thread->tests_order.num;a++)
    {
        tests_order |= (1 << (soft_thread->tests_order.tests[a].test & FCS_TEST_ORDER_NO_FLAGS_MASK));
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
    freecell_solver_hard_thread_t * hard_thread
    )
{
    char * p_quota, * p_scan, * p;
    char * string;
    int last_one = 0;
    int num_items = 0;
    int max_num_items = 16;
    fcs_prelude_item_t * prelude;
    int st_idx;

    prelude = malloc(sizeof(prelude[0]) * max_num_items);
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
        prelude[num_items].scan_idx = st_idx;
        prelude[num_items].quota = atoi(p_quota);
        num_items++;
        if (num_items == max_num_items)
        {
            max_num_items += 16;
            prelude = realloc(prelude, sizeof(prelude[0]) * max_num_items);
        }
    }

    hard_thread->prelude = prelude;
    hard_thread->prelude_num_items = num_items;
    hard_thread->prelude_idx = 0;

    return FCS_COMPILE_PRELUDE_OK;    
}

static void init_tests_order_with_soft_thread_and_initial_state(
    freecell_solver_soft_thread_t * soft_thread,
    void * context
    )
{
    int test_idx;
    fcs_test_t * tests;
    int num_tests;
    fcs_state_with_locations_t * state_copy_ptr;
    fcs_derived_states_order_instance_t * order_instance;

    state_copy_ptr = (fcs_state_with_locations_t * )context;
    num_tests = soft_thread->tests_order.num;
    tests = soft_thread->tests_order.tests;
    
    for(test_idx = 0 ; 
        test_idx < num_tests;
        test_idx++)
    {
        order_instance = tests[test_idx].states_order_instance;
        if (order_instance)
        {
            order_instance->order->give_init_state(
                order_instance,
                soft_thread,
                state_copy_ptr
                );
        }
    }
}


void freecell_solver_init_instance(freecell_solver_instance_t * instance)
{
    int ht_idx;
    freecell_solver_hard_thread_t * hard_thread;
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    instance->num_prev_states_margin = 0;

    instance->max_num_indirect_prev_states = PREV_STATES_GROW_BY;

    instance->indirect_prev_states = (fcs_state_with_locations_t * *)malloc(sizeof(fcs_state_with_locations_t *) * instance->max_num_indirect_prev_states);
#endif

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
        freecell_solver_state_ia_init(hard_thread);
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
            fcs_test_t * tests;
            
            tests = malloc(sizeof(total_tests)*8*sizeof(tests[0]));
                        
            for(bit_idx=0; total_tests != 0; bit_idx++, total_tests >>= 1)
            {
                if ((total_tests & 0x1) != 0)
                {
                    tests[num_tests].test = bit_idx;
                    /* 
                     * Set the states' ordering to NULL to disable it.
                     * */
                    tests[num_tests].states_order_type = NULL;
                    tests[num_tests].states_order_instance = NULL;
                    /* Move to the next test. */
                    num_tests++;
                }
            }
            tests = realloc(tests, num_tests*sizeof(tests[0]));
            instance->opt_tests_order.tests = tests;
            instance->opt_tests_order.num =
                instance->opt_tests_order.max_num =
                num_tests;
            instance->opt_tests_order_set = 1;
        }
    }
}




/* These are all stack comparison functions to be used for the stacks
   cache when using INDIRECT_STACK_STATES
*/
#if defined(INDIRECT_STACK_STATES)

#if ((FCS_STACK_STORAGE != FCS_STACK_STORAGE_GLIB_TREE) && (FCS_STACK_STORAGE != FCS_STACK_STORAGE_GLIB_HASH))
static int fcs_stack_compare_for_comparison_with_context(
    const void * v_s1,
    const void * v_s2,
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
    const
#endif
    void * context

    )
{
    return freecell_solver_stack_compare_for_comparison(v_s1, v_s2);
}
#endif





#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
/* A hash calculation function for use in glib's hash */
static guint freecell_solver_glib_hash_stack_hash_function (
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





static gint freecell_solver_glib_hash_stack_compare (
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
extern guint freecell_solver_hash_function(gconstpointer key);
#endif

/*
 * This function traces the solution from the final state down
 * to the initial state
 * */
static void trace_solution(
    freecell_solver_instance_t * instance
    )
{
    /*
        Trace the solution.
    */
    fcs_state_with_locations_t * s1;
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

    s1 = instance->final_state;

    /* Retrace the step from the current state to its parents */
    while (s1->parent != NULL)
    {
        /* Mark the state as part of the non-optimized solution */
        s1->visited |= FCS_VISITED_IN_SOLUTION_PATH;
        /* Duplicate the move stack */
        {
            stack = s1->moves_to_parent;
            moves = stack->moves;
            for(move_idx=stack->num_moves-1;move_idx>=0;move_idx--)
            {
                fcs_move_stack_push(solution_moves, moves[move_idx]);
            }            
        }
        /* Duplicate the state to a freshly malloced memory */

        /* Move to the parent state */
        s1 = s1->parent;
    }
    /* There's one more state than there are move stacks */
    s1->visited |= FCS_VISITED_IN_SOLUTION_PATH;
}

/*
    This function optimizes the solution path using a BFS scan on the
    states in the solution path.
*/
static int freecell_solver_optimize_solution(
    freecell_solver_instance_t * instance
    )
{
    freecell_solver_hard_thread_t * optimization_thread;
    freecell_solver_soft_thread_t * soft_thread;
    
    optimization_thread = alloc_hard_thread(instance);
    instance->optimization_thread = optimization_thread;

    soft_thread = optimization_thread->soft_threads[0];

    if (instance->opt_tests_order_set)
    {
        if (soft_thread->tests_order.tests != NULL)
        {
            freecell_solver_free_tests_order(&(soft_thread->tests_order));
        }
        
        soft_thread->tests_order = 
            tests_order_dup(&(instance->opt_tests_order));
    }
    
    soft_thread->method = FCS_METHOD_OPTIMIZE;

    soft_thread->is_a_complete_scan = 1;

    /* Initialize the optimization hard-thread and soft-thread */
    optimization_thread->num_times_left_for_soft_thread = 1000000;
    freecell_solver_state_ia_init(optimization_thread);

    /* Instruct the optimization hard thread to run indefinitely AFA it
     * is concerned */
    optimization_thread->max_num_times = -1;
    optimization_thread->ht_max_num_times = -1;

    return 
        freecell_solver_a_star_or_bfs_do_solve_or_resume(
            optimization_thread->soft_threads[0],
            instance->state_copy_ptr,
            0
            );

}


extern void freecell_solver_cache_talon(
    freecell_solver_instance_t * instance,
    fcs_state_with_locations_t * new_state
    );

/*
    This function starts the solution process _for the first time_. If one
    wishes to proceed after the iterations limit was reached, one should
    use freecell_solver_resume_instance.

  */
int freecell_solver_solve_instance(
    freecell_solver_instance_t * instance,
    fcs_state_with_locations_t * init_state
    )
{
    fcs_state_with_locations_t * state_copy_ptr;

    /* Allocate the first state and initialize it to init_state */
    fcs_state_ia_alloc_into_var(state_copy_ptr, instance->hard_threads[0]);

    fcs_duplicate_state(*state_copy_ptr, *init_state);

    {
        int a;
        for(a=0;a<instance->stacks_num;a++)
        {
            fcs_copy_stack(*state_copy_ptr, a, instance->hard_threads[0]->indirect_stacks_buffer);
        }
    }

    /* Initialize the state to be a base state for the game tree */
    state_copy_ptr->depth = 0;
    state_copy_ptr->moves_to_parent = NULL;
    state_copy_ptr->visited = 0;
    state_copy_ptr->parent = NULL;
    memset(&(state_copy_ptr->scan_visited), '\0', sizeof(state_copy_ptr->scan_visited));

    instance->state_copy_ptr = state_copy_ptr;

    /* Initialize the data structure that will manage the state collection */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    instance->tree = rbinit(freecell_solver_state_compare_with_context, NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE)
    instance->tree = avl_create(freecell_solver_state_compare_with_context, NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE)
    instance->tree = rb_create(freecell_solver_state_compare_with_context, NULL);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    instance->tree = g_tree_new(freecell_solver_state_compare);
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    instance->hash = g_hash_table_new(
        freecell_solver_hash_function,
        freecell_solver_state_compare_equal
        );
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    instance->hash = freecell_solver_hash_init(
            2048,
            freecell_solver_state_compare_with_context,
            NULL
       );
#endif

    /****************************************************/

#ifdef INDIRECT_STACK_STATES
    /* Initialize the data structure that will manage the stack
       collection */
#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH
    instance->stacks_hash = freecell_solver_hash_init(
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
        freecell_solver_glib_hash_stack_hash_function,
        freecell_solver_glib_hash_stack_compare
        );
#endif
#endif

    /***********************************************/

#ifdef FCS_WITH_TALONS
    /* Initialize the Talon's Cache */
    if (instance->talon_type == FCS_TALON_KLONDIKE)
    {
        instance->talons_hash = freecell_solver_hash_init(
            512,
            fcs_talon_compare_with_context,
            NULL
            );

        freecell_solver_cache_talon(instance, instance->state_copy_ptr);
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
        fcs_state_with_locations_t * no_use;

        freecell_solver_check_and_add_state(
            instance->hard_threads[0]->soft_threads[0],
            state_copy_ptr,
            &no_use
            );

    }

    instance->ht_idx = 0;
    {
        int ht_idx;
        for(ht_idx=0; ht_idx < instance->num_hard_threads ; ht_idx++)
        {
            freecell_solver_hard_thread_t * hard_thread;
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

    foreach_soft_thread(
        instance,
        init_tests_order_with_soft_thread_and_initial_state,
        (void *)state_copy_ptr
        );

    return freecell_solver_resume_instance(instance);
}


static int run_hard_thread(freecell_solver_hard_thread_t * hard_thread)
{
    freecell_solver_soft_thread_t * soft_thread;
    int num_times_started_at;
    int ret;
    freecell_solver_instance_t * instance = hard_thread->instance;
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


        /* Initalize the a_star rater.
         * This applies to random DFS as well, for the shlomif states
         * order.
         * * */
        if (! soft_thread->initialized)
        {
            freecell_solver_a_star_initialize_rater(
                soft_thread,
                instance->state_copy_ptr
                );
        }


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
            case FCS_METHOD_HARD_DFS:
                
            if (! soft_thread->initialized)
            {
                ret = freecell_solver_hard_dfs_solve_for_state(
                    soft_thread,
                    instance->state_copy_ptr,
                    0,
                    0);

                soft_thread->initialized = 1;
            }
            else
            {
                ret = freecell_solver_hard_dfs_resume_solution(soft_thread, 0);
            }
            break;

            case FCS_METHOD_RANDOM_DFS:
            case FCS_METHOD_SOFT_DFS:

            if (! soft_thread->initialized)
            {
                ret = 
                    freecell_solver_soft_dfs_or_random_dfs_do_solve_or_resume(
                        soft_thread,
                        instance->state_copy_ptr,
                        0,
                        (soft_thread->method == FCS_METHOD_RANDOM_DFS)
                        );

                soft_thread->initialized = 1;
            }
            else
            {
                ret = 
                    freecell_solver_soft_dfs_or_random_dfs_do_solve_or_resume(
                        soft_thread,
                        NULL,
                        1,
                        (soft_thread->method == FCS_METHOD_RANDOM_DFS)
                        );
            }
            break;

            case FCS_METHOD_BFS:
            case FCS_METHOD_A_STAR:
            case FCS_METHOD_OPTIMIZE:
            
            if (! soft_thread->initialized)
            {
                ret = freecell_solver_a_star_or_bfs_do_solve_or_resume(
                    soft_thread,
                    instance->state_copy_ptr,
                    0
                );

                soft_thread->initialized = 1;
            }
            else
            {
                ret =
                    freecell_solver_a_star_or_bfs_do_solve_or_resume(
                        soft_thread,
                        soft_thread->first_state_to_check,
                        1
                        );
            }
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
int freecell_solver_resume_instance(
    freecell_solver_instance_t * instance
    )
{
    int ret = FCS_STATE_SUSPEND_PROCESS;
    freecell_solver_hard_thread_t * hard_thread;

    /*
     * If the optimization thread is defined, it means we are in the 
     * optimization phase of the total scan. In that case, just call
     * its scanning function.
     *
     * Else, proceed with the normal total scan.
     * */
    if (instance->optimization_thread)
    {
        ret =
            freecell_solver_a_star_or_bfs_do_solve_or_resume(
                instance->optimization_thread->soft_threads[0],
                instance->optimization_thread->soft_threads[0]->first_state_to_check,
                1
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
            if (! instance->optimization_thread)
            {
                ret = freecell_solver_optimize_solution(instance);
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
void freecell_solver_unresume_instance(
    freecell_solver_instance_t * instance
    )
{
    /*
     * Do nothing - since finish_instance() can take care of solution_states
     * and proto_solution_moves as they were created by these scans, then
     * I don't need to do it here, too
     *
     * */
}


#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE)

static void freecell_solver_tree_do_nothing(void * data, void * context)
{
}

#endif


/* A function for freeing a stack for the cleanup of the
   stacks collection
*/
#ifdef INDIRECT_STACK_STATES
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH) || (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_AVL_TREE) || (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE)
#if 0
static void freecell_solver_stack_free(void * key, void * context)
{
    free(key);
}
#endif

#elif FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE
static void freecell_solver_libredblack_walk_destroy_stack_action
(
    const void * nodep,
    const VISIT which,
    const int depth,
    void * arg
 )
{
    if ((which == leaf) || (which == preorder))
    {
        free((void*)nodep);
    }
}
#elif FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE
static gint freecell_solver_glib_tree_walk_destroy_stack_action
(
    gpointer key,
    gpointer value,
    gpointer data
)
{
    free(key);

    return 0;
}

#elif FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH
static void freecell_solver_glib_hash_foreach_destroy_stack_action
(
    gpointer key,
    gpointer value,
    gpointer data
)
{
    free(key);
}
#endif

#endif

/***********************************************************/

/*
    This function should be called after the user has retrieved the
    results generated by the scan as it will destroy them.
  */
void freecell_solver_finish_instance(
    freecell_solver_instance_t * instance
    )
{
    int ht_idx;
    freecell_solver_hard_thread_t * hard_thread;

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    free(instance->indirect_prev_states);
#endif

    /* De-allocate the state packs */
    for(ht_idx=0;ht_idx<instance->num_hard_threads;ht_idx++)
    {
        hard_thread = instance->hard_threads[ht_idx];
        freecell_solver_state_ia_finish(hard_thread);

#ifdef INDIRECT_STACK_STATES
        freecell_solver_compact_allocator_finish(hard_thread->stacks_allocator);
        hard_thread->stacks_allocator = NULL;
#endif
        freecell_solver_compact_allocator_finish(hard_thread->move_stacks_allocator);
        hard_thread->move_stacks_allocator = NULL;
        
    }

    if (instance->optimization_thread)
    {
        freecell_solver_state_ia_finish(instance->optimization_thread);
    }


    /* De-allocate the state collection */
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    rbdestroy(instance->tree);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE)
    avl_destroy(instance->tree, freecell_solver_tree_do_nothing);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE)
    rb_destroy(instance->tree, freecell_solver_tree_do_nothing);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    g_tree_destroy(instance->tree);
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    g_hash_table_destroy(instance->hash);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    freecell_solver_hash_free(instance->hash);
#endif



    /* De-allocate the stack collection while free()'ing the stacks
    in the process */
#ifdef INDIRECT_STACK_STATES
#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH
#if 0
    freecell_solver_hash_free_with_callback(instance->stacks_hash, freecell_solver_stack_free);
#else
    freecell_solver_hash_free(instance->stacks_hash);
#endif
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_AVL_TREE)
#if 0
    avl_destroy(instance->stacks_tree, freecell_solver_stack_free);
#else
    avl_destroy(instance->stacks_tree, NULL);
#endif
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE)
#if 0
    rb_destroy(instance->stacks_tree, freecell_solver_stack_free);
#else
    rb_destroy(instance->stacks_tree, NULL);
#endif
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
#if 0
    rbwalk(instance->stacks_tree,
        freecell_solver_libredblack_walk_destroy_stack_action,
        NULL
        );
#endif
    rbdestroy(instance->stacks_tree);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
#if 0
    g_tree_traverse(
        instance->stacks_tree,
        freecell_solver_glib_tree_walk_destroy_stack_action,
        G_IN_ORDER,
        NULL
        );
#endif
    g_tree_destroy(instance->stacks_tree);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
#if 0
    g_hash_table_foreach(
        instance->stacks_hash,
        freecell_solver_glib_hash_foreach_destroy_stack_action,
        NULL
        );
#endif
    g_hash_table_destroy(instance->stacks_hash);
#endif
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    instance->db->close(instance->db,0);
#endif


    clean_soft_dfs(instance);    
}

freecell_solver_soft_thread_t * freecell_solver_instance_get_soft_thread(
        freecell_solver_instance_t * instance,
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
        freecell_solver_hard_thread_t * hard_thread;
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

freecell_solver_soft_thread_t * freecell_solver_new_soft_thread(
    freecell_solver_soft_thread_t * soft_thread
    )
{
    freecell_solver_soft_thread_t * ret;
    freecell_solver_hard_thread_t * hard_thread;

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

freecell_solver_soft_thread_t * freecell_solver_new_hard_thread(
    freecell_solver_instance_t * instance
    )
{
    freecell_solver_hard_thread_t * ret;

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

void freecell_solver_recycle_instance(
    freecell_solver_instance_t * instance
        )
{
    int ht_idx, st_idx;
    freecell_solver_hard_thread_t * hard_thread;
    freecell_solver_soft_thread_t * soft_thread;
    
    freecell_solver_finish_instance(instance);

    instance->num_times = 0;

    instance->num_hard_threads_finished = 0;

    for(ht_idx = 0;  ht_idx < instance->num_hard_threads; ht_idx++)
    {
        hard_thread = instance->hard_threads[ht_idx];
        hard_thread->num_times = 0;
        hard_thread->ht_max_num_times = hard_thread->num_times_step;
        hard_thread->max_num_times = -1;
        hard_thread->num_soft_threads_finished = 0;
        hard_thread->move_stacks_allocator =
            freecell_solver_compact_allocator_new();
#ifdef INDIRECT_STACK_STATES
        hard_thread->stacks_allocator = 
            freecell_solver_compact_allocator_new();
#endif
        for(st_idx = 0; st_idx < hard_thread->num_soft_threads ; st_idx++)
        {
            soft_thread = hard_thread->soft_threads[st_idx];
            soft_thread->is_finished = 0;
            soft_thread->initialized = 0;

            freecell_solver_rand_srand(soft_thread->rand_gen, soft_thread->rand_seed);
            /* Reset the priority queue */
            soft_thread->a_star_pqueue->CurrentSize = 0;
        }
    }
}
