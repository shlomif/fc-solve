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
 * depth_dbm_solver.c - a specialised solver that offloads the states'
 * collection to an on-disk DBM database such as Berkeley DB or Google's
 * LevelDB. Has been adapted to be completely in-memory. It makes use of
 * delta_states.c for a very compact storage.
 *
 * In addition, this solver implements the scheme in:
 * http://tech.groups.yahoo.com/group/fc-solve-discuss/message/1135
 */
#include "dbm_solver_head.h"

typedef struct
{
    fcs_dbm__cache_store__common_t cache_store;
    fcs_meta_compact_allocator_t queue_meta_alloc;
    fcs_offloading_queue_t queue;
} fcs_dbm_collection_by_depth_t;

#define MAX_FCC_DEPTH (RANK_KING * 4 * DECKS_NUM * 2)
typedef size_t batch_size_t;
typedef struct
{
    fcs_dbm_collection_by_depth_t colls_by_depth[MAX_FCC_DEPTH];
    fcs_lock_t storage_lock;
    const char *offload_dir_path;
    int curr_depth;
    fcs_dbm_instance_common_elems_t common;
    batch_size_t max_batch_size;
} fcs_dbm_solver_instance_t;

#define CHECK_KEY_CALC_DEPTH()                                                 \
    (instance->curr_depth + list->num_non_reversible_moves_including_prune)

#include "dbm_procs.h"
static GCC_INLINE void instance_init(fcs_dbm_solver_instance_t *const instance,
    const fcs_dbm_common_input_t *const inp, const batch_size_t max_batch_size,
    FILE *const out_fh)
{
    instance->max_batch_size = max_batch_size;
    instance->curr_depth = 0;
    FCS_INIT_LOCK(instance->storage_lock);
    instance->offload_dir_path = inp->offload_dir_path;
    fcs_dbm__common_init(&(instance->common), inp->iters_delta_limit,
        inp->local_variant, out_fh);

    FCS_INIT_LOCK(instance->storage_lock);
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
}

static GCC_INLINE void instance_destroy(fcs_dbm_solver_instance_t *instance)
{
    for (int depth = 0; depth < MAX_FCC_DEPTH; depth++)
    {
        const_AUTO(coll, &(instance->colls_by_depth[depth]));
        fcs_offloading_queue__destroy(&(coll->queue));
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
        fc_solve_meta_compact_allocator_finish(&(coll->queue_meta_alloc));
#endif
        DESTROY_CACHE(coll);
        FCS_DESTROY_LOCK(coll->cache_store.queue_lock);
    }
    FCS_DESTROY_LOCK(instance->storage_lock);
}

struct fcs_dbm_solver_thread_struct
{
    fcs_dbm_solver_instance_t *instance;
    fc_solve_delta_stater_t delta_stater;
    fcs_meta_compact_allocator_t thread_meta_alloc;
};

typedef struct
{
    fcs_dbm_solver_thread_t *thread;
} thread_arg_t;

static void *instance_run_solver_thread(void *const void_arg)
{
    fcs_derived_state_t *derived_list_recycle_bin = NULL, *derived_iter;
    fcs_state_keyval_pair_t state;
#ifdef DEBUG_OUT
    fcs_state_locs_struct_t locs;
    fc_solve_init_locs(&locs);
#endif
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    const_AUTO(thread, ((thread_arg_t *)void_arg)->thread);
    const_SLOT(instance, thread);
    const_AUTO(delta_stater, &(thread->delta_stater));
    const_AUTO(local_variant, instance->common.variant);
    const_SLOT(max_batch_size, instance);

    batch_size_t prev_size = 0;
    fcs_compact_allocator_t derived_list_allocator;
    fc_solve_compact_allocator_init(
        &(derived_list_allocator), &(thread->thread_meta_alloc));
    const_AUTO(out_fh, instance->common.out_fh);

    TRACE("%s\n", "instance_run_solver_thread start");

    const_AUTO(coll, &(instance->colls_by_depth[instance->curr_depth]));
    int queue_num_extracted_and_processed = 0;
    fcs_dbm_record_t *tokens[max_batch_size];
    fcs_derived_state_t *derived_lists[max_batch_size];
    while (TRUE)
    {
        /* First of all extract a batch of items. */
        FCS_LOCK(instance->storage_lock);

        if (prev_size > 0)
        {
            instance->common.queue_num_extracted_and_processed -= prev_size;
        }

        batch_size_t batch_size = 0;
        if (instance->common.should_terminate == DONT_TERMINATE)
        {
            for (; batch_size < max_batch_size; ++batch_size)
            {
                if (fcs_offloading_queue__extract(&(coll->queue),
                        (fcs_offloading_queue_item_t *)(&tokens[batch_size])))
                {
                    derived_lists[batch_size] = NULL;
                    instance_increment(instance);
                }
                else
                {
                    break;
                }
            }

            queue_num_extracted_and_processed =
                instance->common.queue_num_extracted_and_processed;
        }
        FCS_UNLOCK(instance->storage_lock);

        if ((instance->common.should_terminate != DONT_TERMINATE) ||
            (!queue_num_extracted_and_processed))
        {
            break;
        }

        prev_size = batch_size;
        if (batch_size == 0)
        {
            /* Sleep until more items become available in the
             * queue. */
            usleep(5000);
            continue;
        }

        for (batch_size_t batch_i = 0; batch_i < batch_size; ++batch_i)
        {
            /* Handle item. */
            fc_solve_delta_stater_decode_into_state(delta_stater,
                tokens[batch_i]->key.s, &state, indirect_stacks_buffer);
            /* A section for debugging. */
            FCS__OUTPUT_STATE(out_fh, "", &(state.s), &locs);
        }

        for (batch_size_t batch_i = 0; batch_i < batch_size; ++batch_i)
        {
            /* Handle item. */
            fc_solve_delta_stater_decode_into_state(delta_stater,
                tokens[batch_i]->key.s, &state, indirect_stacks_buffer);
            /* A section for debugging. */
            // FCS__OUTPUT_STATE(out_fh, "", &(state.s), &locs);

            if (instance_solver_thread_calc_derived_states(local_variant,
                    &state, tokens[batch_i], &derived_lists[batch_i],
                    &derived_list_recycle_bin, &derived_list_allocator, TRUE))
            {
                fcs_dbm_queue_item_t physical_item;
                physical_item.key = tokens[batch_i]->key;
                FCS_LOCK(instance->storage_lock);
                fcs_dbm__found_solution(
                    &(instance->common), tokens[batch_i], &physical_item);
                FCS_UNLOCK(instance->storage_lock);
                goto thread_end;
            }
        }

        /* Encode all the states. */
        for (batch_size_t batch_i = 0; batch_i < batch_size; ++batch_i)
        {
            for (derived_iter = derived_lists[batch_i]; derived_iter;
                 derived_iter = derived_iter->next)
            {
                fcs_init_and_encode_state(delta_stater, local_variant,
                    &(derived_iter->state), &(derived_iter->key));
            }
        }

        instance_check_multiple_keys(thread, instance, &(coll->cache_store),
            &(thread->thread_meta_alloc), derived_lists, batch_size
#ifdef FCS_DBM_CACHE_ONLY
            ,
            item->moves_to_key
#endif
            );

        /* Now recycle the derived_list */
        for (batch_size_t batch_i = 0; batch_i < batch_size; ++batch_i)
        {
            var_AUTO(list, derived_lists[batch_i]);
            while (list)
            {
#define derived_list_next derived_iter
                derived_list_next = list->next;
                list->next = derived_list_recycle_bin;
                derived_list_recycle_bin = list;
                list = derived_list_next;
#undef derived_list_next
            }
        }
        /* End handle item. */
        /* End of main thread loop */
    }
thread_end:

    fc_solve_compact_allocator_finish(&(derived_list_allocator));

    TRACE("%s\n", "instance_run_solver_thread end");

    return NULL;
}

#include "depth_dbm_procs.h"

static GCC_INLINE void instance_check_key(
    fcs_dbm_solver_thread_t *const thread GCC_UNUSED,
    fcs_dbm_solver_instance_t *const instance, const int key_depth,
    fcs_encoded_state_buffer_t *const key, fcs_dbm_record_t *const parent,
    const unsigned char move GCC_UNUSED,
    const fcs_which_moves_bitmask_t *const which_irreversible_moves_bitmask
        GCC_UNUSED
#ifdef FCS_DBM_CACHE_ONLY
    ,
    const fcs_fcc_move_t *moves_to_parent
#endif
    )
{
    const_AUTO(coll, &(instance->colls_by_depth[key_depth]));
    {
        fcs_dbm_record_t *token;
#ifndef FCS_DBM_WITHOUT_CACHES
        fcs_lru_cache_t *const cache = &(coll->cache_store.cache);
        if (cache_does_key_exist(cache, key))
        {
            return;
        }
#ifndef FCS_DBM_CACHE_ONLY
        else if (pre_cache_does_key_exist(&(coll->cache_store.pre_cache), key))
        {
            return;
        }
#endif
#ifndef FCS_DBM_CACHE_ONLY
        else if (fc_solve_dbm_store_does_key_exist(
                     coll->cache_store.store, key->s))
        {
            cache_insert(cache, key, NULL, '\0');
            return;
        }
#endif
        else
#else
        if ((token = fc_solve_dbm_store_insert_key_value(
                 coll->cache_store.store, key, parent, TRUE)))
#endif
        {
#ifdef FCS_DBM_CACHE_ONLY
            fcs_cache_key_info_t *cache_key;
#endif

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
            pre_cache_insert(&(coll->cache_store.pre_cache), key, parent);
#else
            cache_key = cache_insert(cache, key, moves_to_parent, move);
#endif
#endif

            /* Now insert it into the queue. */

            fcs_offloading_queue__insert(
                &(coll->queue), (const fcs_offloading_queue_item_t *)(&token));

            instance->common.count_of_items_in_queue++;
            instance->common.num_states_in_collection++;
            instance_debug_out_state(instance, &(token->key));
        }
    }
}

static void instance_run_all_threads(fcs_dbm_solver_instance_t *instance,
    fcs_state_keyval_pair_t *init_state, size_t num_threads)
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
        mark_and_sweep_old_states(
            instance, fc_solve_dbm_store_get_dict(
                          instance->colls_by_depth[instance->curr_depth]
                              .cache_store.store),
            instance->curr_depth);
        instance->curr_depth++;
    }

    dbm__free_threads(instance, num_threads, threads, free_thread);
}

int main(int argc, char *argv[])
{
    const char *out_filename = NULL;
    DECLARE_IND_BUF_T(init_indirect_stacks_buffer)
    fcs_dbm_common_input_t inp = fcs_dbm_common_input_init;
    batch_size_t max_batch_size = 1;
    const char *param;

    int real_arg;
    int *arg = &real_arg;
    for (real_arg = 1; real_arg < argc; real_arg++)
    {
        if (fcs_dbm__extract_common_from_argv(argc, argv, arg, &inp))
        {
        }
        else if ((param = TRY_PARAM("--batch-size")))
        {
            max_batch_size = (batch_size_t)atol(param);
            if (max_batch_size < 1)
            {
                fc_solve_err("--batch-size must be at least 1.\n");
            }
        }
        else if (!strcmp(argv[*arg], "-o"))
        {
            if (++(*arg) == argc)
            {
                fc_solve_err("-o came without an argument.\n");
            }
            out_filename = argv[*arg];
        }
        else
        {
            break;
        }
    }

    if (real_arg < argc - 1)
    {
        fc_solve_err("%s\n", "Junk arguments!");
    }
    else if (real_arg == argc)
    {
        fc_solve_err("%s\n", "No board specified.");
    }
    const_AUTO(num_threads, inp.num_threads);

    FILE *const out_fh = calc_out_fh(out_filename);
    fcs_state_keyval_pair_t init_state;
    read_state_from_file(inp.local_variant, argv[real_arg],
        &init_state PASS_IND_BUF_T(init_indirect_stacks_buffer));
    horne_prune__simple(inp.local_variant, &init_state);

    const_AUTO(local_variant, inp.local_variant);
    fc_solve_delta_stater_t delta;
    fc_solve_delta_stater_init(&delta, &init_state.s, STACKS_NUM, FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
        ,
        CALC_SEQUENCES_ARE_BUILT_BY()
#endif
            );

#define KEY_PTR() (key_ptr)
    fcs_dbm_solver_instance_t instance;
    instance_init(&instance, &inp, max_batch_size, out_fh);

    fcs_encoded_state_buffer_t *const key_ptr = &(instance.common.first_key);
    fcs_init_and_encode_state(&delta, local_variant, &init_state, KEY_PTR());

    /* The NULL parent_state_enc and move for indicating this is the
     * initial state. */
    fcs_encoded_state_buffer_t parent_state_enc;
    fcs_init_encoded_state(&(parent_state_enc));

    fcs_dbm_record_t *token;
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
    pre_cache_insert(&(instance.colls_by_depth[0].cache_store.pre_cache),
        KEY_PTR(), &parent_state_enc);
#else
    cache_insert(
        &(instance.colls_by_depth[0].cache_store.cache), KEY_PTR(), NULL, '\0');
#endif
#else
    token = fc_solve_dbm_store_insert_key_value(
        instance.colls_by_depth[0].cache_store.store, KEY_PTR(), NULL, TRUE);
#endif

    fcs_offloading_queue__insert(&(instance.colls_by_depth[0].queue),
        (const fcs_offloading_queue_item_t *)(&token));
    instance.common.num_states_in_collection++;
    instance.common.count_of_items_in_queue++;

    instance_run_all_threads(&instance, &init_state, NUM_THREADS());
    handle_and_destroy_instance_solution(&instance, &delta);

    fc_solve_delta_stater_release(&delta);

    if (out_filename)
    {
        fclose(out_fh);
    }

    return 0;
}
