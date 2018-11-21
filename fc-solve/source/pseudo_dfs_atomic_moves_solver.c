/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2012 Shlomi Fish
 */
/*
 * pseudo_dfs_atomic_moves_solver.c - a specialised solver that runs a
 * pseudo-DFS scan. See ../docs/distributed-pseudo-dfs-solver-spec.txt
 */

// #define FCS_LRU_KEY_IS_STATE

#include "dbm_solver_head.h"
#include "pseudo_dfs_cache.h"
#include "render_state.h"

#include <Judy.h>

typedef struct
{
    fcs_cache_key *curr_state;
    fcs_cache_key *next_states;
    int count_next_states, max_count_next_states, next_state_idx;
} pseudo_dfs_stack_item;

typedef Pvoid_t store_type;

static inline void delete_state(store_type *const store,
    fcs_pseudo_dfs_lru_cache *const cache, fcs_cache_key *const key)
{
    fcs_pdfs_cache_insert(cache, key);
    int rc_int;
    JHSD(rc_int, *store, key, sizeof(*key));
}

static inline void insert_state(store_type *store, fcs_cache_key *key)
{
    Word_t *PValue;
    JHSI(PValue, *store, key, sizeof(*key));
    *PValue = 1;
}

static inline bool lookup_state(store_type *const store,
    fcs_pseudo_dfs_lru_cache *const cache GCC_UNUSED, fcs_cache_key *const key)
{
    Word_t *PValue;
    JHSG(PValue, *store, key, sizeof(*key));
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
    store_type store;
    fcs_pseudo_dfs_lru_cache cache;

    long pre_cache_max_count;
    /* The stack */
    ssize_t stack_depth, max_stack_depth;
    pseudo_dfs_stack_item *stack;
    long count_num_processed, max_count_num_processed;
    bool solution_was_found;
    enum TERMINATE_REASON should_terminate;
    compact_allocator derived_list_allocator;
    meta_allocator meta_alloc;
    fcs_derived_state *derived_list_recycle_bin;
    fcs_dbm_variant_type local_variant;
    fcs_delta_stater delta_stater;
    fcs_cache_key init_key;
} dbm_solver_instance;

static inline void instance__inspect_new_state(
    dbm_solver_instance *const instance, fcs_cache_key *const state)
{
    if (++instance->count_num_processed % 1000000 == 0)
    {
        printf("Reached iteration %lld\n",
            (long long)instance->count_num_processed);
        fflush(stdout);
    }

    if (fcs_pdfs_cache_does_key_exist(&(instance->cache), state))
    {
        --instance->stack_depth;
        return;
    }
    const_AUTO(local_variant, instance->local_variant);
    const_AUTO(depth, instance->stack_depth);
    const_AUTO(max_depth, instance->max_stack_depth);
    if (depth == max_depth)
    {
        instance->stack =
            SREALLOC(instance->stack, ++(instance->max_stack_depth));
        printf("Increasing to %lld\n", (long long)instance->max_stack_depth);
        fflush(stdout);
        instance->stack[max_depth] = (pseudo_dfs_stack_item){
            .next_states = NULL, .max_count_next_states = 0};
    }
    pseudo_dfs_stack_item *const stack_item = instance->stack + depth;
    stack_item->curr_state = state;

    fcs_derived_state *derived_list = NULL;

    fcs_state_keyval_pair s;
    const_AUTO(delta_stater, &(instance->delta_stater));
    fc_solve_delta_stater_decode_into_state(
        delta_stater, state->s, &s, indirect_stacks_buffer);
    if (instance_solver_thread_calc_derived_states(instance->local_variant, &s,
            NULL, &derived_list, &(instance->derived_list_recycle_bin),
            &(instance->derived_list_allocator), TRUE))
    {
        instance->should_terminate = SOLUTION_FOUND_TERMINATE;
        instance->solution_was_found = TRUE;
        return;
    }

    stack_item->count_next_states = 0;
    stack_item->next_state_idx = 0;
    /* Now recycle the derived_list */
    while (derived_list)
    {
        fc_solve_canonize_state(&(derived_list->state.s)PASS_FREECELLS(
            FREECELLS_NUM) PASS_STACKS(STACKS_NUM));
        fcs_init_and_encode_state(delta_stater, local_variant,
            &(derived_list->state), &(derived_list->key));
        if (!lookup_state(
                &(instance->store), &(instance->cache), &(derived_list->key)))
        {
            int i = (stack_item->count_next_states)++;
            if (i >= stack_item->max_count_next_states)
            {
                stack_item->next_states = SREALLOC(stack_item->next_states,
                    ++(stack_item->max_count_next_states));
            }
            stack_item->next_states[i] = derived_list->key;
            insert_state(&(instance->store), &(stack_item->next_states[i]));
        }
        var_AUTO(list_next, derived_list->next);
        derived_list->next = instance->derived_list_recycle_bin;
        instance->derived_list_recycle_bin = derived_list;
        derived_list = list_next;
    }
}

static inline void instance_init(dbm_solver_instance *const instance,
    const fcs_dbm_variant_type local_variant,
    fcs_state_keyval_pair *const init_state,
    const long max_num_elements_in_cache)
{
    instance->local_variant = local_variant;
    fc_solve_delta_stater_init(&(instance->delta_stater), local_variant,
        &(init_state->s), STACKS_NUM,
        FREECELLS_NUM PASS_ON_NOT_FC_ONLY(FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));
    fcs_init_and_encode_state(&instance->delta_stater, local_variant,
        init_state, &instance->init_key);
    instance->solution_was_found = FALSE;
    instance->should_terminate = DONT_TERMINATE;
    instance->count_num_processed = 0;
    instance->max_count_num_processed = LONG_MAX;
    instance->store = (Pvoid_t)NULL;
    instance->stack = NULL;
    instance->max_stack_depth = 0;
    instance->stack_depth = 0;
    fc_solve_meta_compact_allocator_init(&(instance->meta_alloc));
    fc_solve_compact_allocator_init(
        &(instance->derived_list_allocator), &(instance->meta_alloc));
    instance->derived_list_recycle_bin = NULL;
    fcs_pdfs_cache_init(
        &(instance->cache), max_num_elements_in_cache, &(instance->meta_alloc));
    insert_state(&(instance->store), &instance->init_key);
    instance__inspect_new_state(instance, &instance->init_key);
}

static inline void instance_free(dbm_solver_instance *const instance)
{
    for (ssize_t d = 0; d < instance->stack_depth; d++)
    {
        free(instance->stack[d].next_states);
        instance->stack[d].next_states = NULL;
    }
    free(instance->stack);
    instance->stack = NULL;

    Word_t rc_word;
    JHSFA(rc_word, instance->store);

    fcs_pdfs_cache_destroy(&(instance->cache));
    fc_solve_compact_allocator_finish(&(instance->derived_list_allocator));
    fc_solve_meta_compact_allocator_finish(&(instance->meta_alloc));
}

static inline void instance_run(dbm_solver_instance *const instance)
{
    while (instance->count_num_processed < instance->max_count_num_processed &&
           (instance->should_terminate == DONT_TERMINATE))
    {
        const_AUTO(depth, instance->stack_depth);
        if (depth < 0)
        {
            instance->should_terminate = QUEUE_TERMINATE;
        }
        else
        {
            pseudo_dfs_stack_item *const stack_item = instance->stack + depth;
            const int idx = (stack_item->next_state_idx)++;
            if (idx == stack_item->count_next_states)
            {
                /* Demote from the current depth. */
                for (int i = 0; i < stack_item->count_next_states; i++)
                {
                    delete_state(&(instance->store), &(instance->cache),
                        &(stack_item->next_states[i]));
                }
                stack_item->count_next_states = 0;
                stack_item->next_state_idx = 0;
                --(instance->stack_depth);
            }
            else
            {
                ++instance->stack_depth;
                instance__inspect_new_state(
                    instance, &(stack_item->next_states[idx]));
            }
        }
    }
}

static inline void instance__print_coords_to_log(
    dbm_solver_instance *const instance, FILE *const log_fh)
{
#if 0
    const fcs_dbm_variant_type local_variant = instance->local_variant;
#endif
    fprintf(
        log_fh, "At %ld iterations Coords=[", instance->count_num_processed);

    const pseudo_dfs_stack_item *stack_item = instance->stack;
    const pseudo_dfs_stack_item *const end_stack_item =
        stack_item + instance->stack_depth;
    for (; stack_item <= end_stack_item; stack_item++)
    {
        fprintf(log_fh, "%d,", stack_item->next_state_idx);
#if 0
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);
    char state_as_string[2000];
    FCS__RENDER_STATE(state_as_string, &(stack_item->curr_state->s), &locs);
    fprintf(log_fh, "Found State=<<'STATE'\n%s\nSTATE\n\n", state_as_string);
#endif
    }
#if 0
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);
    char state_as_string[2000];
    FCS__RENDER_STATE(state_as_string, &(end_stack_item->curr_state->s), &locs);
    printf("Found State=<<'STATE'\n%s\nSTATE\n\n", state_as_string);
    fflush(stdout);
#endif
    fprintf(log_fh, "]\n");
}

static inline void instance__load_coords_from_fh(
    dbm_solver_instance *const instance, FILE *const fh)
{
    int coord_from_input;
    while (fscanf(fh, "%d,", &coord_from_input) == 1)
    {
        const int coord = coord_from_input - 1;
        if (coord >= 0)
        {
            pseudo_dfs_stack_item *const stack_item =
                &(instance->stack[instance->stack_depth++]);
            stack_item->next_state_idx = coord + 1;
            instance__inspect_new_state(
                instance, &(stack_item->next_states[coord]));
        }
    }
}

#include "read_state.h"

int main(int argc GCC_UNUSED, char *argv[])
{
    fcs_dbm_variant_type local_variant = FCS_DBM_VARIANT_2FC_FREECELL;
    const long delta_limit = 100000;
    const char *filename = argv[1];
    FILE *const fh = fopen(filename, "r");
    if (!fh)
    {
        fc_solve_err("Could not open file '%s' for input.\n", filename);
    }
    const fcs_user_state_str user_state = read_state(fh);
    fcs_state_keyval_pair init_state_pair;
    fc_solve_initial_user_state_to_c(
        user_state.s, &init_state_pair, FREECELLS_NUM, STACKS_NUM, 1, NULL);

    dbm_solver_instance instance;

    instance_init(&instance, local_variant, &init_state_pair, 70000000);

#define LOG_FILENAME "fc-solve-pseudo-dfs.log.txt"
    FILE *const last_line_fh = popen(("tail -1 " LOG_FILENAME), "r");
    if (last_line_fh)
    {
        long count_num_processed;
        if (fscanf(last_line_fh, "At %ld iterations Coords=[",
                &count_num_processed) == 1)
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

    instance.max_count_num_processed =
        instance.count_num_processed + delta_limit;

    while (instance.max_count_num_processed % delta_limit != 0)
    {
        instance.max_count_num_processed +=
            delta_limit - (instance.max_count_num_processed % delta_limit);
    }

    while (instance.should_terminate == DONT_TERMINATE)
    {
        instance_run(&instance);

        FILE *const log_fh = fopen(LOG_FILENAME, "at");
        instance__print_coords_to_log(&instance, log_fh);
        fclose(log_fh);

        instance.max_count_num_processed =
            instance.count_num_processed + delta_limit;
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
