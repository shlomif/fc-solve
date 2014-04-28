/* Copyright (c) 2012 Shlomi Fish
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
 * pseudo_dfs_atomic_moves_solver.c - a specialised solver that runs a pseudo-DFS
 * scan. See ../docs/distributed-pseudo-dfs-solver-spec.txt
 *
 */

#define FCS_LRU_KEY_IS_STATE

#include "dbm_solver_head.h"

#include <Judy.h>

typedef struct
{
    fcs_cache_key_t * curr_state;
    fcs_cache_key_t * next_states;
    int count_next_states, max_count_next_states, next_state_idx;
} pseduo_dfs_stack_item_t;

typedef Pvoid_t store_t;

static GCC_INLINE void delete_state(
    store_t * store,
    fcs_cache_key_t * key)
{
    int Rc_int;
    JHSD(Rc_int, *store, &(key->s), sizeof(key->s));
}

static GCC_INLINE void insert_state(
    store_t * store,
    fcs_cache_key_t * key)
{
    Word_t * PValue;
    JHSI(PValue, *store, &(key->s), sizeof(key->s));
    *PValue = 1;
}

static GCC_INLINE fcs_bool_t lookup_state(
    store_t * store,
    fcs_cache_key_t * key)
{
    Word_t * PValue;
    JHSG(PValue, *store, &(key->s), sizeof(key->s));
    return (PValue != NULL);
}

typedef struct
{
    fcs_lock_t storage_lock;
#if 0
    fcs_lru_cache_t cache;
#endif
    store_t store;

    long pre_cache_max_count;
    /* The stack */
    fcs_lock_t stack_lock;
    int stack_depth, max_stack_depth;
    pseduo_dfs_stack_item_t * stack;
    long count_num_processed, max_count_num_processed;
    fcs_bool_t solution_was_found;
    enum TERMINATE_REASON should_terminate;
    fcs_compact_allocator_t derived_list_allocator;
    fcs_meta_compact_allocator_t meta_alloc;
    fcs_derived_state_t * derived_list_recycle_bin;
    enum fcs_dbm_variant_type_t local_variant;
} fcs_dbm_solver_instance_t;



static GCC_INLINE fcs_bool_t instance__inspect_new_state(
    fcs_dbm_solver_instance_t * const instance,
    fcs_cache_key_t * const state
)
{
    const int depth = ((instance->stack_depth)++);
    const int max_depth = instance->max_stack_depth;
    if (depth == max_depth)
    {
        instance->stack = SREALLOC(instance->stack, ++(instance->max_stack_depth));
        pseduo_dfs_stack_item_t * const stack_item = instance->stack + max_depth;
        stack_item->next_states = NULL;
        stack_item->max_count_next_states = 0;
    }
    pseduo_dfs_stack_item_t * const stack_item = instance->stack + depth;
    stack_item->curr_state = state;

    fcs_derived_state_t * derived_list = NULL, * derived_iter = NULL;

    instance->count_num_processed++;

    if (instance_solver_thread_calc_derived_states(
        instance->local_variant,
        state,
        NULL,
        &derived_list,
        &(instance->derived_list_recycle_bin),
        &(instance->derived_list_allocator),
        TRUE
    ))
    {
        instance->should_terminate = SOLUTION_FOUND_TERMINATE;
        instance->solution_was_found = TRUE;
        return TRUE;
    }

    stack_item->count_next_states = 0;
    stack_item->next_state_idx = 0;
    fcs_kv_state_t kv;
    /* Now recycle the derived_list */
    while (derived_list)
    {
        kv.key = &(derived_list->state.s);
        kv.val = &(derived_list->state.info);
        fc_solve_canonize_state(&kv, 4, 8);

        if (lookup_state(&(instance->store), &(derived_list->state)))
        {
            int i = (stack_item->count_next_states)++;
            if (i >= stack_item->max_count_next_states)
            {
                stack_item->next_states = SREALLOC(stack_item->next_states, ++(stack_item->max_count_next_states));
            }
            stack_item->next_states[i] = derived_list->state;
            insert_state(&(instance->store), &(stack_item->next_states[i]));
        }
#define derived_list_next derived_iter
        derived_list_next = derived_list->next;
        derived_list->next = instance->derived_list_recycle_bin;
        instance->derived_list_recycle_bin = derived_list;
        derived_list = derived_list_next;
#undef derived_list_next
    }

    return FALSE;
}

static GCC_INLINE void instance_init(
    fcs_dbm_solver_instance_t * const instance,
    enum fcs_dbm_variant_type_t local_variant,
    long iters_delta_limit,
    fcs_cache_key_t * init_state
)
{
    instance->local_variant = local_variant;
    FCS_INIT_LOCK(instance->stack_lock);
    FCS_INIT_LOCK(instance->storage_lock);

    instance->solution_was_found = FALSE;
    instance->should_terminate = DONT_TERMINATE;

    instance->count_num_processed = 0;
    if (iters_delta_limit >= 0)
    {
        instance->max_count_num_processed =
            instance->count_num_processed + iters_delta_limit;
    }
    else
    {
        instance->max_count_num_processed = LONG_MAX;
    }

    instance->store = (Pvoid_t)NULL;

    instance->stack = NULL;
    instance->max_stack_depth = 0;
    instance->stack_depth = 0;

    fc_solve_meta_compact_allocator_init(
        &(instance->meta_alloc)
    );
    fc_solve_compact_allocator_init(&(instance->derived_list_allocator), &(instance->meta_alloc));
    instance->derived_list_recycle_bin = NULL;

    insert_state(&(instance->store), init_state);

    instance__inspect_new_state(instance, init_state);
}

static GCC_INLINE void instance_free(
    fcs_dbm_solver_instance_t * const instance
)
{
    for (int d = 0; d < instance->stack_depth ; d++)
    {
        free (instance->stack[d].next_states);
        instance->stack[d].next_states = NULL;
    }
    free (instance->stack);
    instance->stack = NULL;

    Word_t Rc_word;
    JHSFA(Rc_word, instance->store);

    fc_solve_compact_allocator_finish(&(instance->derived_list_allocator));
    fc_solve_meta_compact_allocator_finish(&(instance->meta_alloc));
}

static GCC_INLINE void instance_run(
    fcs_dbm_solver_instance_t * const instance
)
{
    while (instance->count_num_processed < instance->max_count_num_processed
        && (instance->should_terminate == DONT_TERMINATE))
    {
        const int depth = (instance->stack_depth);
        if (depth < 0)
        {
            instance->should_terminate = QUEUE_TERMINATE;
        }
        else
        {
            pseduo_dfs_stack_item_t * const stack_item = instance->stack + depth;
            const int idx = (stack_item->next_state_idx)++;
            if (idx == stack_item->count_next_states)
            {
                /* Demote from the current depth. */
                for (int i = 0; i < stack_item->count_next_states ; i++)
                {
                    delete_state(&(instance->store), &(stack_item->next_states[ i ]));
                }
                stack_item->count_next_states = 0;
                stack_item->next_state_idx = 0;
                (instance->stack_depth)--;
            }
            else
            {
                instance__inspect_new_state(instance, &(stack_item->next_states[idx]));
            }
        }
    }
}

#define USER_STATE_SIZE 2000

int main(int argc, char * argv[])
{
    enum fcs_dbm_variant_type_t local_variant;
    const int delta_limit = 100000;
    const char * filename = argv[1];

    local_variant = FCS_DBM_VARIANT_2FC_FREECELL;

    fcs_state_keyval_pair_t init_state_pair;
    fcs_cache_key_t * init_state_ptr;

    FILE * fh = fopen(filename, "r");
    if (fh == NULL)
    {
        fprintf (stderr, "Could not open file '%s' for input.\n", filename);
        exit(-1);
    }
    char user_state[USER_STATE_SIZE];
    memset(user_state, '\0', sizeof(user_state));
    fread(user_state, sizeof(user_state[0]), USER_STATE_SIZE-1, fh);
    fclose(fh);

    fc_solve_initial_user_state_to_c(user_state, &init_state_pair, 4, 8, 1, NULL);

    init_state_ptr = &(init_state_pair);

    fcs_dbm_solver_instance_t instance;

    instance_init(
        &instance,
        local_variant,
        delta_limit,
        init_state_ptr
    );

    while ( instance.should_terminate == DONT_TERMINATE );
    {
        instance_run(&instance);
        instance.max_count_num_processed += delta_limit;
    }

    instance_free(&instance);

    return 0;
}
