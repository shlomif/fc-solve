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
#include "pseudo_dfs_cache.h"

#include <Judy.h>

typedef struct
{
    fcs_cache_key_t * curr_state;
    fcs_cache_key_t * next_states;
    int count_next_states, max_count_next_states, next_state_idx;
} pseduo_dfs_stack_item_t;

typedef Pvoid_t store_t;

static GCC_INLINE void delete_state(
    store_t * const store,
    fcs_pdfs_lru_cache_t * const cache,
    fcs_cache_key_t * const key)
{
    int Rc_int;
    fcs_pdfs_cache_insert(cache, &(key->s));
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

static GCC_INLINE const fcs_bool_t lookup_state(
    store_t * const store,
    fcs_pdfs_lru_cache_t * const cache,
    fcs_cache_key_t * const key)
{
    Word_t * PValue;
    JHSG(PValue, *store, &(key->s), sizeof(key->s));
#if 1
    return (PValue != NULL);
#else
    if (PValue != NULL)
    {
        return TRUE;
    }
    else
    {
        return fcs_pdfs_cache_does_key_exist(cache, &(key->s));
    }
#endif
}

typedef struct
{
    fcs_lock_t storage_lock;
    store_t store;
    fcs_pdfs_lru_cache_t cache;

    long pre_cache_max_count;
    /* The stack */
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



static GCC_INLINE void instance__inspect_new_state(
    fcs_dbm_solver_instance_t * const instance,
    fcs_cache_key_t * const state
)
{
    instance->count_num_processed++;

    if (fcs_pdfs_cache_does_key_exist(&(instance->cache), &(state->s)))
    {
        instance->stack_depth--;
        return;
    }
    const enum fcs_dbm_variant_type_t local_variant = instance->local_variant;
    const int depth = (instance->stack_depth);
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
        return;
    }

    stack_item->count_next_states = 0;
    stack_item->next_state_idx = 0;
    fcs_kv_state_t kv;
    /* Now recycle the derived_list */
    while (derived_list)
    {
        kv.key = &(derived_list->state.s);
        kv.val = &(derived_list->state.info);
        fc_solve_canonize_state(&kv, FREECELLS_NUM, STACKS_NUM);

        if (! lookup_state(&(instance->store), &(instance->cache), &(derived_list->state)))
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

    return;
}

static GCC_INLINE void instance_init(
    fcs_dbm_solver_instance_t * const instance,
    enum fcs_dbm_variant_type_t local_variant,
    fcs_cache_key_t * init_state,
    long max_num_elements_in_cache
)
{
    instance->local_variant = local_variant;
    FCS_INIT_LOCK(instance->storage_lock);

    instance->solution_was_found = FALSE;
    instance->should_terminate = DONT_TERMINATE;

    instance->count_num_processed = 0;
    instance->max_count_num_processed = LONG_MAX;

    instance->store = (Pvoid_t)NULL;
    instance->stack = NULL;
    instance->max_stack_depth = 0;
    instance->stack_depth = 0;

    fc_solve_meta_compact_allocator_init(
        &(instance->meta_alloc)
    );
    fc_solve_compact_allocator_init(&(instance->derived_list_allocator), &(instance->meta_alloc));
    instance->derived_list_recycle_bin = NULL;

    fcs_pdfs_cache_init(&(instance->cache), max_num_elements_in_cache, &(instance->meta_alloc));

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

    fcs_pdfs_cache_destroy(&(instance->cache));

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
                    delete_state(&(instance->store), &(instance->cache), &(stack_item->next_states[ i ]));
                }
                stack_item->count_next_states = 0;
                stack_item->next_state_idx = 0;
                (instance->stack_depth)--;
            }
            else
            {
                instance->stack_depth++;
                instance__inspect_new_state(instance, &(stack_item->next_states[idx]));
            }
        }
    }
}

static GCC_INLINE void instance__print_coords_to_log(
    fcs_dbm_solver_instance_t * const instance,
    FILE * const log_fh
)
{
    const enum fcs_dbm_variant_type_t local_variant = instance->local_variant;
    fprintf (log_fh, "At %ld iterations Coords=[", instance->count_num_processed);

    const pseduo_dfs_stack_item_t * stack_item = instance->stack;
    const pseduo_dfs_stack_item_t * const end_stack_item = stack_item + instance->stack_depth;
    for (; stack_item <= end_stack_item ; stack_item++)
    {
        fprintf(log_fh, "%d,", stack_item->next_state_idx);
    }

#if 1
    {

        fcs_state_locs_struct_t locs;
        fc_solve_init_locs(&locs);
        char * const state_as_string =
            fc_solve_state_as_string(
                &(end_stack_item->curr_state->s),
                &locs,
                FREECELLS_NUM,
                STACKS_NUM,
                DECKS_NUM,
                TRUE,
                FALSE,
                TRUE
            );
        printf("Found State=<<'STATE'\n%s\nSTATE\n\n", state_as_string);
        fflush(stdout);
        free (state_as_string);
    }
#endif

    fprintf (log_fh, "]\n");
}

static GCC_INLINE void instance__load_coords_from_fh(
    fcs_dbm_solver_instance_t * const instance,
    FILE * const fh
)
{
    int coord_from_input;
    while (fscanf(fh, "%d,", &coord_from_input) == 1)
    {
        const int coord = coord_from_input - 1;
        if (coord >= 0)
        {
            pseduo_dfs_stack_item_t * const stack_item = &(instance->stack[instance->stack_depth++]);
            stack_item->next_state_idx = coord+1;
            instance__inspect_new_state(instance, &(stack_item->next_states[coord]));
        }
    }
}

#define USER_STATE_SIZE 2000

int main(int argc, char * argv[])
{
    enum fcs_dbm_variant_type_t local_variant;
#if 1
    const int delta_limit = 100000;
#else
    const int delta_limit = 2;
#endif

    const int max_num_elements_in_cache = 8000000;

    const char * filename = argv[1];


    local_variant = FCS_DBM_VARIANT_2FC_FREECELL;

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

    fcs_state_keyval_pair_t init_state_pair;
    fc_solve_initial_user_state_to_c(user_state, &init_state_pair, FREECELLS_NUM, STACKS_NUM, 1, NULL);

    init_state_ptr = &(init_state_pair);

    fcs_dbm_solver_instance_t instance;

    instance_init(
        &instance,
        local_variant,
        init_state_ptr,
        max_num_elements_in_cache
    );

#define LOG_FILENAME "fc-solve-pseudo-dfs.log.txt"

    {
        FILE * const last_line_fh = popen(("tail -1 " LOG_FILENAME), "r");

        if (last_line_fh)
        {
            long count_num_processed;
            if (fscanf(
                    last_line_fh,
                    "At %ld iterations Coords=[",
                    &count_num_processed
                    ) == 1
            )
            {
                instance__load_coords_from_fh(&instance, last_line_fh);
                /*
                 * instance__inspect_new_state increments count_num_processed
                 * so let's set it after loading the coordinates.
                 * */
                instance.count_num_processed = count_num_processed;
            }
        }
        pclose(last_line_fh);
    }

    instance.max_count_num_processed
        = instance.count_num_processed + delta_limit;

    while (instance.max_count_num_processed % delta_limit != 0)
    {
        instance.max_count_num_processed += delta_limit - (instance.max_count_num_processed % delta_limit);
    }

    while ( instance.should_terminate == DONT_TERMINATE )
    {
        instance_run(&instance);

        FILE * const log_fh = fopen(LOG_FILENAME, "at");
        instance__print_coords_to_log(&instance, log_fh);
        fclose(log_fh);

        instance.max_count_num_processed
            = instance.count_num_processed + delta_limit;
    }

    if (instance.should_terminate == SOLUTION_FOUND_TERMINATE)
    {
        printf("%s\n", "Solution was found.");
    }
    else
    {
        printf("%s\n", "I could not solve it.");
    }

    instance_free(&instance);

    return 0;
}
