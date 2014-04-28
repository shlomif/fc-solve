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
    int count_next_states, next_state_idx;
} pseduo_dfs_stack_item_t;

typedef Pvoid_t store_t;

static GCC_INLINE void insert_state(
    store_t * store,
    fcs_cache_key_t * key)
{
    Word_t * PValue;
    JHSI(PValue, *store, key, sizeof(*key));
    *PValue = 1;
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
} fcs_dbm_solver_instance_t;

static GCC_INLINE void instance_init(
    fcs_dbm_solver_instance_t * const instance,
    enum fcs_dbm_variant_type_t local_variant,
    long iters_delta_limit,
    fcs_cache_key_t * init_state
)
{
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

    instance->stack = SREALLOC(instance->stack, ++(instance->max_stack_depth));
    instance->stack[0].curr_state = init_state;
    instance->stack[0].next_states = NULL;
    instance->stack[0].count_next_states = -1;
    instance->stack[0].next_state_idx = -1;

    insert_state(&(instance->store), init_state);

    return;
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

    init_state_ptr = &(init_state_pair.s);

    fcs_dbm_solver_instance_t instance;

    instance_init(
        &instance,
        local_variant,
        delta_limit,
        init_state_ptr
    );

    instance_free(&instance);

    return 0;
}
