// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2012 Shlomi Fish

// depth_dbm_solver.c - a specialised solver that offloads the states'
// collection to an on-disk DBM database such as Berkeley DB or Google's
// LevelDB. Has been adapted to be completely in-memory. It makes use of
// delta_states.c for a very compact storage.
//
// In addition, this solver implements the scheme in:
// https://groups.yahoo.com/neo/groups/fc-solve-discuss/conversations/topics/1135

#define IN_DEPTH_DBM_SOLVER_dot_c 1
#include "dbm_solver_head.h"

typedef struct
{
    fcs_dbm__cache_store__common cache_store;
    meta_allocator queue_meta_alloc;
    fcs_offloading_queue queue;
} fcs_dbm_collection_by_depth;

typedef size_t fcs_batch_size;
typedef struct
{
    fcs_dbm_collection_by_depth colls_by_depth[MAX_FCC_DEPTH];
    fcs_condvar monitor;
    const char *offload_dir_path;
    size_t curr_depth;
    dbm_instance_common_elems common;
    fcs_batch_size max_batch_size;
    fcs_dbm_record raw_found_sol;
} dbm_solver_instance;

#define CHECK_KEY_CALC_DEPTH()                                                 \
    (instance->curr_depth + list->num_non_reversible_moves_including_prune)

#include "dbm_procs.h"
static inline void instance_init(dbm_solver_instance *const instance,
    const fcs_dbm_common_input *const inp, const fcs_batch_size max_batch_size,
    FILE *const out_fh, const bool do_not_yield_solution)
{
    instance->max_batch_size = max_batch_size;
    instance->curr_depth = 0;
    instance->offload_dir_path = inp->offload_dir_path;
    fcs_dbm__common_init(&(instance->common), inp->iters_delta_limit,
        inp->max_num_states_in_collection, inp->local_variant, out_fh);
#ifdef FCS_DBM__STORE_KEYS_ONLY
    instance->common.do_not_yield_solution = do_not_yield_solution;
    if (!do_not_yield_solution)
    {
        /* initialize parent_lookup . */
        char mypath[2000];
        snprintf(mypath, COUNT(mypath), "%s/fcsdbm_rawdump.bin",
            instance->offload_dir_path);
        LAST(mypath) = '\0';

        parent_lookup__create(&(instance->common.parent_lookup), mypath);
    }
#endif

    for (int depth = 0; depth < MAX_FCC_DEPTH; depth++)
    {
        const_AUTO(coll, &(instance->colls_by_depth[depth]));
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
        fcs_offloading_queue__init(
            &(coll->queue), inp->offload_dir_path, depth);
#else
        fc_solve_meta_compact_allocator_init(&(coll->queue_meta_alloc));
        fcs_offloading_queue__init(&(coll->queue), &(coll->queue_meta_alloc));
#endif

        fcs_dbm__cache_store__init(&(coll->cache_store), &(instance->common),
            &(coll->queue_meta_alloc), inp->dbm_store_path,
            inp->pre_cache_max_count, inp->caches_delta);
    }
    fcs_condvar_init(&(instance->monitor));
}

static inline void instance_destroy(dbm_solver_instance *const instance)
{
    for (int depth = 0; depth < MAX_FCC_DEPTH; depth++)
    {
        const_AUTO(coll, &(instance->colls_by_depth[depth]));
        fcs_offloading_queue__destroy(&(coll->queue));
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
        fc_solve_meta_compact_allocator_finish(&(coll->queue_meta_alloc));
#endif
        DESTROY_CACHE(coll);
    }
    fcs_condvar_destroy(&(instance->monitor));
    fcs_lock_destroy(&instance->common.storage_lock);
}

struct fcs_dbm_solver_thread_struct
{
    dbm_solver_instance *instance;
    fcs_delta_stater delta_stater;
    meta_allocator thread_meta_alloc;
};

static void *instance_run_solver_thread(void *const void_arg)
{
    fcs_derived_state *derived_list_recycle_bin = NULL;
    fcs_state_keyval_pair state;
#ifdef DEBUG_OUT
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);
#endif
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    const_AUTO(thread, ((thread_arg *)void_arg)->thread);
    const_SLOT(instance, thread);
    const_AUTO(delta_stater, &(thread->delta_stater));
    const_AUTO(local_variant, instance->common.variant);
    const_SLOT(max_batch_size, instance);

    fcs_batch_size prev_size = 0;
    compact_allocator derived_list_allocator;
    fc_solve_compact_allocator_init(
        &(derived_list_allocator), &(thread->thread_meta_alloc));
    const_AUTO(out_fh, instance->common.out_fh);

    TRACE("%s\n", "instance_run_solver_thread start");

    const_AUTO(coll, &(instance->colls_by_depth[instance->curr_depth]));
    fcs_encoded_state_buffer tokens[max_batch_size];
    fcs_derived_state *derived_lists[max_batch_size];
    while (true)
    {
        // First of all extract a batch of items.
        fcs_lock_lock(&instance->common.storage_lock);
        bool should_break = false;
        fcs_batch_size batch_size = 0;
        if (prev_size > 0)
        {
            if (!(instance->common.queue_num_extracted_and_processed -=
                    prev_size))
            {
                fcs_condvar_broadcast(&(instance->monitor));
            }
            prev_size = 0;
        }
        else
        {
            should_break =
                (instance->common.should_terminate != DONT_TERMINATE);
            if (instance->common.should_terminate == DONT_TERMINATE)
            {
                for (; batch_size < max_batch_size; ++batch_size)
                {
                    if (fcs_offloading_queue__extract(
                            &(coll->queue), (&tokens[batch_size])))
                    {
                        derived_lists[batch_size] = NULL;
                        instance_increment(instance);
                    }
                    else
                    {
                        break;
                    }
                }

                should_break =
                    !instance->common.queue_num_extracted_and_processed;

                prev_size = batch_size;
                if ((!should_break) && (batch_size == 0))
                {
                    // Sleep until more items become available in the queue.
                    fcs_condvar__wait_on(
                        &(instance->monitor), &(instance->common.storage_lock));
                }
            }
        }
        fcs_lock_unlock(&instance->common.storage_lock);

        if (should_break && (prev_size == 0))
        {
            break;
        }
        if (batch_size == 0)
        {
            continue;
        }

        for (fcs_batch_size batch_i = 0; batch_i < batch_size; ++batch_i)
        {
            const_AUTO(token, tokens[batch_i]);
            // Handle the item in "token".
            fc_solve_delta_stater_decode_into_state(
                delta_stater, token.s, &state, indirect_stacks_buffer);
            FCS__OUTPUT_STATE(out_fh, "", &(state.s), &locs);

            if (instance_solver_thread_calc_derived_states(local_variant,
                    &state, token, &derived_lists[batch_i],
                    &derived_list_recycle_bin, &derived_list_allocator, true))
            {
                fcs_dbm_queue_item physical_item;
                physical_item.key = token;
                fcs_lock_lock(&instance->common.storage_lock);
                instance->raw_found_sol.key = token;
                fcs_dbm_store stores_by_depth[MAX_FCC_DEPTH];
                for (size_t i = 0; i < MAX_FCC_DEPTH; ++i)
                {
                    stores_by_depth[i] =
                        instance->colls_by_depth[i].cache_store.store;
                }
                assert(dbm_lookup_parent(MAX_FCC_DEPTH, stores_by_depth,
                    instance->raw_found_sol.key,
                    &instance->raw_found_sol.parent));
                fcs_dbm__found_solution(&(instance->common),
                    &instance->raw_found_sol, &physical_item);
                fcs_condvar_broadcast(&(instance->monitor));
                fcs_lock_unlock(&instance->common.storage_lock);
                goto thread_end;
            }
        }

        // Encode all the states.
        for (fcs_batch_size batch_i = 0; batch_i < batch_size; ++batch_i)
        {
            for (var_AUTO(derived_iter, derived_lists[batch_i]); derived_iter;
                derived_iter = derived_iter->next)
            {
                fcs_init_and_encode_state(delta_stater, local_variant,
                    &(derived_iter->state),
                    &(derived_iter->key_and_parent.key));
            }
        }

        instance_check_multiple_keys(thread, instance, &(coll->cache_store),
            &(thread->thread_meta_alloc), derived_lists, batch_size);

        for (fcs_batch_size batch_i = 0; batch_i < batch_size; ++batch_i)
        {
            fcs_derived_state_list__recycle(
                &derived_list_recycle_bin, &derived_lists[batch_i]);
        }
        // End of the thread's main loop
    }
thread_end:

    fc_solve_compact_allocator_finish(&(derived_list_allocator));
    TRACE("%s\n", "instance_run_solver_thread end");
    return NULL;
}

#include "depth_dbm_procs.h"

static inline void instance_check_key(
    dbm_solver_thread *const thread GCC_UNUSED,
    dbm_solver_instance *const instance, const size_t key_depth,
    fcs_encoded_state_buffer *const key,
    const fcs_encoded_state_buffer raw_parent,
    const unsigned char move GCC_UNUSED,
    const fcs_which_moves_bitmask *const which_irreversible_moves_bitmask
        GCC_UNUSED)
{
    const_AUTO(coll, &(instance->colls_by_depth[key_depth]));
    fcs_dbm_record *token;

    if ((token = cache_store__has_key(&coll->cache_store, key, raw_parent)))
    {
        fcs_offloading_queue__insert(&(coll->queue), (token->key));

        ++instance->common.count_of_items_in_queue;
        ++instance->common.num_states_in_collection;
        instance_debug_out_state(instance, &(token->key));
    }
}

static void instance_run_all_threads(dbm_solver_instance *const instance,
    fcs_state_keyval_pair *const init_state, const size_t num_threads)
{
    const_AUTO(threads,
        dbm__calc_threads(instance, init_state, num_threads, init_thread));
    while (instance->curr_depth < MAX_FCC_DEPTH)
    {
        dbm__spawn_threads(instance, num_threads, threads);
        if (instance->common.queue_solution_was_found)
        {
            break;
        }
        mark_and_sweep_old_states(instance,
            fc_solve_dbm_store_get_dict(
                instance->colls_by_depth[instance->curr_depth]
                    .cache_store.store),
            instance->curr_depth);
        ++instance->curr_depth;
    }

#ifdef FCS_DBM__STORE_KEYS_ONLY
    parent_lookup__finish_writing(&(instance->common.parent_lookup));
#endif
    dbm__free_threads(instance, num_threads, threads, free_thread);
}

int main(int argc, char *argv[])
{
    apr_initialize();
    const char *out_filename = NULL;
    DECLARE_IND_BUF_T(init_indirect_stacks_buffer)
    fcs_dbm_common_input inp = fcs_dbm_common_input_init;
    fcs_batch_size max_batch_size = 1;
    const char *param;
    bool do_not_yield_solution = false;

    int real_arg;
    int *arg = &real_arg;
    for (real_arg = 1; real_arg < argc; real_arg++)
    {
        if (fcs_dbm__extract_common_from_argv(argc, argv, arg, &inp))
        {
        }
        else if ((param = TRY_PARAM("--batch-size")))
        {
            max_batch_size = (fcs_batch_size)atol(param);
            if (max_batch_size < 1)
            {
                exit_error("--batch-size must be at least 1.\n");
            }
        }
        else if ((param = TRY_PARAM("-o")))
        {
            out_filename = param;
        }
        else if ((param = TRY_PARAM("--do-not-yield-solution")))
        {
            do_not_yield_solution = ((!strcmp(param, "0")) ? false : true);
        }
        else
        {
            break;
        }
    }

    if (real_arg < argc - 1)
    {
        exit_error("%s\n", "Junk arguments!");
    }
    if (real_arg == argc)
    {
        exit_error("%s\n", "No board specified.");
    }

#ifndef FCS_DBM_SINGLE_THREAD
    const_AUTO(num_threads, inp.num_threads);
#endif

    FILE *const out_fh = calc_out_fh(out_filename);
    fcs_state_keyval_pair init_state;
    read_state_from_file(inp.local_variant, argv[real_arg],
        &init_state PASS_IND_BUF_T(init_indirect_stacks_buffer));
    horne_prune__simple(inp.local_variant, &init_state);

    const_AUTO(local_variant, inp.local_variant);
    fcs_delta_stater delta;
    fc_solve_delta_stater_init(&delta, local_variant, &init_state.s, STACKS_NUM,
        FREECELLS_NUM PASS_ON_NOT_FC_ONLY(CALC_SEQUENCES_ARE_BUILT_BY()));

    dbm_solver_instance instance;
    instance_init(
        &instance, &inp, max_batch_size, out_fh, do_not_yield_solution);

    fcs_encoded_state_buffer *const key_ptr = &(instance.common.first_key);
    fcs_init_and_encode_state(&delta, local_variant, &init_state, key_ptr);

    // The NULL parent for indicating this is the initial state.
    fcs_encoded_state_buffer parent_state_enc;
    fcs_init_encoded_state(&(parent_state_enc));

    fcs_dbm_record *token;
    fcs_encoded_state_buffer null_parent;
    fcs_init_encoded_state(&null_parent);
    token = fc_solve_dbm_store_insert_key_value(
        instance.colls_by_depth[0].cache_store.store, key_ptr, null_parent);

    fcs_offloading_queue__insert(
        &(instance.colls_by_depth[0].queue), (token->key));
    ++instance.common.num_states_in_collection;
    ++instance.common.count_of_items_in_queue;

    instance_run_all_threads(&instance, &init_state, NUM_THREADS());
    handle_and_destroy_instance_solution(&instance, &delta);

    fc_solve_delta_stater_release(&delta);
    if (out_filename)
    {
        fclose(out_fh);
    }
    apr_terminate();
    return 0;
}
