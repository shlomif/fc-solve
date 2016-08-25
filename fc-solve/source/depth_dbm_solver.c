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
 * depth_dbm_solver.c - a specialised solver that offloads the states'
 * collection to an on-disk DBM database such as Berkeley DB or Google's
 * LevelDB. Has been adapted to be completely in-memory. It makes use of
 * delta_states.c for a very compact storage.
 *
 * In addition, this solver implements the scheme in:
 * http://tech.groups.yahoo.com/group/fc-solve-discuss/message/1135
 *
 */

#include "dbm_solver_head.h"

typedef struct
{
#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
    fcs_pre_cache_t pre_cache;
#endif
    fcs_lru_cache_t cache;
#endif
#ifndef FCS_DBM_CACHE_ONLY
    fcs_dbm_store_t store;
#endif
    fcs_lock_t queue_lock;
    fcs_meta_compact_allocator_t queue_meta_alloc;
    fcs_offloading_queue_t queue;
} fcs_dbm_collection_by_depth_t;

typedef struct
{
    fcs_lock_t global_lock;
    void *tree_recycle_bin;
    fcs_lock_t storage_lock;
    const char *offload_dir_path;
    int curr_depth;
    fcs_dbm_collection_by_depth_t colls_by_depth[MAX_FCC_DEPTH];
    long pre_cache_max_count;
    /* The queue */
    long count_num_processed, count_of_items_in_queue, max_count_num_processed;
    fcs_bool_t queue_solution_was_found;
    enum TERMINATE_REASON should_terminate;
#ifdef FCS_DBM_WITHOUT_CACHES
    fcs_dbm_record_t *queue_solution_ptr;
#else
    fcs_encoded_state_buffer_t queue_solution;

#endif
    int queue_num_extracted_and_processed;
    long num_states_in_collection;
    FILE *out_fh;
    fcs_encoded_state_buffer_t first_key;
    enum fcs_dbm_variant_type_t variant;
} fcs_dbm_solver_instance_t;

static GCC_INLINE void instance_init(fcs_dbm_solver_instance_t *instance,
    enum fcs_dbm_variant_type_t local_variant,
    const long pre_cache_max_count GCC_UNUSED,
    const long caches_delta GCC_UNUSED, const char *dbm_store_path,
    long iters_delta_limit, const char *offload_dir_path, FILE *out_fh)
{
    instance->variant = local_variant;
    instance->curr_depth = 0;
    FCS_INIT_LOCK(instance->global_lock);
    instance->offload_dir_path = offload_dir_path;

    instance->out_fh = out_fh;

    instance->queue_solution_was_found = FALSE;
    instance->should_terminate = DONT_TERMINATE;
    instance->queue_num_extracted_and_processed = 0;
    instance->num_states_in_collection = 0;
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
    instance->count_of_items_in_queue = 0;
    instance->tree_recycle_bin = NULL;

    FCS_INIT_LOCK(instance->storage_lock);
    for (int depth = 0; depth < MAX_FCC_DEPTH; depth++)
    {
        const_AUTO(coll, &(instance->colls_by_depth[depth]));
        FCS_INIT_LOCK(coll->queue_lock);
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
#define NUM_ITEMS_PER_PAGE (128 * 1024)
        fcs_offloading_queue__init(
            &(coll->queue), NUM_ITEMS_PER_PAGE, offload_dir_path, depth);
#else
        fc_solve_meta_compact_allocator_init(&(coll->queue_meta_alloc));
        fcs_offloading_queue__init(&(coll->queue), &(coll->queue_meta_alloc));
#endif

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
        pre_cache_init(&(coll->pre_cache), &(coll->meta_alloc));
#endif
        coll->pre_cache_max_count = pre_cache_max_count;
        cache_init(&(coll->cache), pre_cache_max_count + caches_delta,
            &(coll->meta_alloc));
#endif
#ifndef FCS_DBM_CACHE_ONLY
        fc_solve_dbm_store_init(
            &(coll->store), dbm_store_path, &(instance->tree_recycle_bin));
#endif
    }
}

static GCC_INLINE void instance_destroy(fcs_dbm_solver_instance_t *instance)
{
    int depth;
    fcs_dbm_collection_by_depth_t *coll;

    for (depth = 0; depth < MAX_FCC_DEPTH; depth++)
    {
        coll = &(instance->colls_by_depth[depth]);
        fcs_offloading_queue__destroy(&(coll->queue));
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
        fc_solve_meta_compact_allocator_finish(&(coll->queue_meta_alloc));
#endif

#ifndef FCS_DBM_WITHOUT_CACHES

#ifndef FCS_DBM_CACHE_ONLY
        pre_cache_offload_and_destroy(
            &(coll->pre_cache), coll->store, &(coll->cache));
#endif

        cache_destroy(&(coll->cache));
#endif

#ifndef FCS_DBM_CACHE_ONLY
        fc_solve_dbm_store_destroy(coll->store);
#endif
        FCS_DESTROY_LOCK(coll->queue_lock);
    }
    FCS_DESTROY_LOCK(instance->storage_lock);
}

#define CHECK_KEY_CALC_DEPTH()                                                 \
    (instance->curr_depth + list->num_non_reversible_moves_including_prune)

#include "dbm_procs.h"

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
#ifdef FCS_DBM_WITHOUT_CACHES
        fcs_dbm_record_t *token;
#endif
#ifndef FCS_DBM_WITHOUT_CACHES
        fcs_lru_cache_t *cache;
#ifndef FCS_DBM_CACHE_ONLY
        fcs_pre_cache_t *pre_cache;
#endif

        cache = &(instance->cache);
#ifndef FCS_DBM_CACHE_ONLY
        pre_cache = &(instance->pre_cache);
#endif

        if (cache_does_key_exist(cache, key))
        {
            return;
        }
#ifndef FCS_DBM_CACHE_ONLY
        else if (pre_cache_does_key_exist(pre_cache, key))
        {
            return;
        }
#endif
#ifndef FCS_DBM_CACHE_ONLY
        else if (fc_solve_dbm_store_does_key_exist(instance->store, key->s))
        {
            cache_insert(cache, key, NULL, '\0');
            return;
        }
#endif
        else
#else
        if ((token = fc_solve_dbm_store_insert_key_value(
                 coll->store, key, parent, TRUE)))
#endif
        {
#ifdef FCS_DBM_CACHE_ONLY
            fcs_cache_key_info_t *cache_key;
#endif

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
            pre_cache_insert(pre_cache, key, parent);
#else
            cache_key = cache_insert(cache, key, moves_to_parent, move);
#endif
#endif

            /* Now insert it into the queue. */

            FCS_LOCK(coll->queue_lock);
            fcs_offloading_queue__insert(
                &(coll->queue), (const fcs_offloading_queue_item_t *)(&token));
            FCS_UNLOCK(coll->queue_lock);

            FCS_LOCK(instance->global_lock);

            instance->count_of_items_in_queue++;
            instance->num_states_in_collection++;

            instance_debug_out_state(instance, &(token->key));

            FCS_UNLOCK(instance->global_lock);
        }
    }
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

static void *instance_run_solver_thread(void *void_arg)
{
    int curr_depth;

    enum TERMINATE_REASON should_terminate;
    fcs_dbm_queue_item_t physical_item;
    fcs_dbm_record_t *token;
    fcs_dbm_queue_item_t *item, *prev_item;
    int queue_num_extracted_and_processed;
    fcs_derived_state_t *derived_list, *derived_list_recycle_bin, *derived_iter;
    fcs_compact_allocator_t derived_list_allocator;
    fcs_state_keyval_pair_t state;
#ifdef DEBUG_OUT
    fcs_state_locs_struct_t locs;
    fc_solve_init_locs(&locs);
#endif
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    const_AUTO(thread, ((thread_arg_t *)void_arg)->thread);
    const_SLOT(instance, thread);
    const_AUTO(delta_stater, &(thread->delta_stater));
    const_AUTO(local_variant, instance->variant);

    prev_item = item = NULL;
    queue_num_extracted_and_processed = 0;

    fc_solve_compact_allocator_init(
        &(derived_list_allocator), &(thread->thread_meta_alloc));
    derived_list_recycle_bin = NULL;
    derived_list = NULL;
    const_SLOT(out_fh, instance);

    TRACE("%s\n", "instance_run_solver_thread start");

    curr_depth = instance->curr_depth;
    const_AUTO(coll, &(instance->colls_by_depth[curr_depth]));

    while (1)
    {
        /* First of all extract an item. */
        FCS_LOCK(coll->queue_lock);

        if (prev_item)
        {
            FCS_LOCK(instance->global_lock);
            instance->queue_num_extracted_and_processed--;
            FCS_UNLOCK(instance->global_lock);
        }

        if ((should_terminate = instance->should_terminate) == DONT_TERMINATE)
        {
            if (fcs_offloading_queue__extract(
                    &(coll->queue), (fcs_offloading_queue_item_t *)(&token)))
            {
                physical_item.key = token->key;
                item = &physical_item;
                instance->count_of_items_in_queue--;
                instance->queue_num_extracted_and_processed++;
                if (++instance->count_num_processed % 100000 == 0)
                {
                    instance_print_stats(instance, out_fh);
                }
                if (instance->count_num_processed >=
                    instance->max_count_num_processed)
                {
                    instance->should_terminate = should_terminate =
                        MAX_ITERS_TERMINATE;
                }
            }
            else
            {
                item = NULL;
            }

            queue_num_extracted_and_processed =
                instance->queue_num_extracted_and_processed;
        }
        FCS_UNLOCK(coll->queue_lock);

        if ((should_terminate != DONT_TERMINATE) ||
            (!queue_num_extracted_and_processed))
        {
            break;
        }

        if (!item)
        {
            /* Sleep until more items become available in the
             * queue. */
            usleep(5000);
        }
        else
        {
            /* Handle item. */
            fc_solve_delta_stater_decode_into_state(
                delta_stater, item->key.s, &state, indirect_stacks_buffer);

/* A section for debugging. */
#ifdef DEBUG_OUT
            {
                char *state_str;
                state_str = fc_solve_state_as_string(
                    &(state.s), &locs, FREECELLS_NUM, STACKS_NUM, 1, 1, 0, 1);

                fprintf(out_fh, "<<<\n%s>>>\n", state_str);
                fflush(out_fh);
                free(state_str);
            }
#endif

            if (instance_solver_thread_calc_derived_states(local_variant,
                    &state, token, &derived_list, &derived_list_recycle_bin,
                    &derived_list_allocator, TRUE))
            {
                FCS_LOCK(instance->global_lock);
                instance->should_terminate = SOLUTION_FOUND_TERMINATE;
                instance->queue_solution_was_found = TRUE;
#ifdef FCS_DBM_WITHOUT_CACHES
                instance->queue_solution_ptr = token;
#else
                instance->queue_solution = item->key;
#endif
                FCS_UNLOCK(instance->global_lock);
                break;
            }

            /* Encode all the states. */
            for (derived_iter = derived_list; derived_iter;
                 derived_iter = derived_iter->next)
            {
                fcs_init_and_encode_state(delta_stater, local_variant,
                    &(derived_iter->state), &(derived_iter->key));
            }

            instance_check_multiple_keys(thread, instance, derived_list
#ifdef FCS_DBM_CACHE_ONLY
                ,
                item->moves_to_key
#endif
                );

            /* Now recycle the derived_list */
            while (derived_list)
            {
#define derived_list_next derived_iter
                derived_list_next = derived_list->next;
                derived_list->next = derived_list_recycle_bin;
                derived_list_recycle_bin = derived_list;
                derived_list = derived_list_next;
#undef derived_list_next
            }
            /* End handle item. */
        }
        /* End of main thread loop */
        prev_item = item;
    }

    fc_solve_compact_allocator_finish(&(derived_list_allocator));

    TRACE("%s\n", "instance_run_solver_thread end");

    return NULL;
}

#ifdef FCS_DEBONDT_DELTA_STATES

static int compare_enc_states(
    const fcs_encoded_state_buffer_t *a, const fcs_encoded_state_buffer_t *b)
{
    return memcmp(a, b, sizeof(*a));
}

#else

static int compare_enc_states(
    const fcs_encoded_state_buffer_t *a, const fcs_encoded_state_buffer_t *b)
{
    if (a->s[0] < b->s[0])
    {
        return -1;
    }
    else if (a->s[0] > b->s[0])
    {
        return 1;
    }
    else
    {
        return memcmp(a->s, b->s, a->s[0] + 1);
    }
}

#endif

static unsigned char get_move_from_parent_to_child(
    fcs_dbm_solver_instance_t *instance, fc_solve_delta_stater_t *delta,
    fcs_encoded_state_buffer_t parent, fcs_encoded_state_buffer_t child)
{
    unsigned char move_to_return;
    fcs_encoded_state_buffer_t got_child;
    fcs_state_keyval_pair_t parent_state;
    fcs_derived_state_t *derived_list, *derived_list_recycle_bin, *derived_iter;
    fcs_compact_allocator_t derived_list_allocator;
    fcs_meta_compact_allocator_t meta_alloc;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    const_AUTO(local_variant, instance->variant);

    fc_solve_meta_compact_allocator_init(&meta_alloc);
    fc_solve_compact_allocator_init(&(derived_list_allocator), &meta_alloc);
    fc_solve_delta_stater_decode_into_state(
        delta, parent.s, &parent_state, indirect_stacks_buffer);

    derived_list = NULL;
    derived_list_recycle_bin = NULL;

    instance_solver_thread_calc_derived_states(local_variant, &parent_state,
        NULL, &derived_list, &derived_list_recycle_bin, &derived_list_allocator,
        TRUE);

    for (derived_iter = derived_list; derived_iter;
         derived_iter = derived_iter->next)
    {
        fcs_init_and_encode_state(
            delta, local_variant, &(derived_iter->state), &got_child);

        if (compare_enc_states(&got_child, &child) == 0)
        {
            break;
        }
    }

    if (!derived_iter)
    {
        fprintf(stderr, "%s\n", "Failed to find move. Terminating.");
        exit(-1);
    }
    move_to_return = derived_iter->move;

    fc_solve_compact_allocator_finish(&(derived_list_allocator));
    fc_solve_meta_compact_allocator_finish(&meta_alloc);

    return move_to_return;
}

#include "depth_dbm_procs.h"

static void instance_run_all_threads(fcs_dbm_solver_instance_t *instance,
    fcs_state_keyval_pair_t *init_state, size_t num_threads)
{
    main_thread_item_t *const threads =
        dbm__calc_threads(instance, init_state, num_threads, init_thread);
    while (instance->curr_depth < MAX_FCC_DEPTH)
    {
        dbm__spawn_threads(instance, num_threads, threads);
        if (instance->queue_solution_was_found)
        {
            break;
        }
        mark_and_sweep_old_states(
            instance, fc_solve_dbm_store_get_dict(
                          instance->colls_by_depth[instance->curr_depth].store),
            instance->curr_depth);
        instance->curr_depth++;
    }

    dbm__free_threads(instance, num_threads, threads, free_thread);
}

int main(int argc, char *argv[])
{
    long pre_cache_max_count;
    long caches_delta;
    long iters_delta_limit = -1;
#if 0
    long start_line = 1;
#endif
    const char *dbm_store_path;
    int arg;
    const char *filename = NULL, *out_filename = NULL, *offload_dir_path = NULL;
    FILE *fh = NULL, *out_fh = NULL;
    char user_state[USER_STATE_SIZE];
    fc_solve_delta_stater_t delta;
    fcs_dbm_record_t *token;
    enum fcs_dbm_variant_type_t local_variant = FCS_DBM_VARIANT_2FC_FREECELL;
    fcs_state_keyval_pair_t init_state;
#if 0
    fcs_bool_t skip_queue_output = FALSE;
#endif
    DECLARE_IND_BUF_T(init_indirect_stacks_buffer)
    pre_cache_max_count = 1000000;
    caches_delta = 1000000;
    dbm_store_path = "./fc_solve_dbm_store";
    size_t num_threads = 2;

    for (arg = 1; arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--pre-cache-max-count"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr,
                    "--pre-cache-max-count came without an argument!\n");
                exit(-1);
            }
            pre_cache_max_count = atol(argv[arg]);
            if (pre_cache_max_count < 1000)
            {
                fprintf(
                    stderr, "--pre-cache-max-count must be at least 1,000.\n");
                exit(-1);
            }
        }
        else if (!strcmp(argv[arg], "--game"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--game came without an argument!\n");
                exit(-1);
            }
            if (!strcmp(argv[arg], "bakers_dozen"))
            {
                local_variant = FCS_DBM_VARIANT_BAKERS_DOZEN;
            }
            else if (!strcmp(argv[arg], "freecell"))
            {
                local_variant = FCS_DBM_VARIANT_2FC_FREECELL;
            }
            else
            {
                fprintf(stderr, "Unknown game '%s'. Aborting\n", argv[arg]);
                exit(-1);
            }
        }
        else if (!strcmp(argv[arg], "--caches-delta"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--caches-delta came without an argument!\n");
                exit(-1);
            }
            caches_delta = atol(argv[arg]);
            if (caches_delta < 1000)
            {
                fprintf(stderr, "--caches-delta must be at least 1,000.\n");
                exit(-1);
            }
        }
        else if (!strcmp(argv[arg], "--num-threads"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--num-threads came without an argument!\n");
                exit(-1);
            }
            num_threads = (size_t)atoi(argv[arg]);
            if (num_threads < 1)
            {
                fprintf(stderr, "--num-threads must be at least 1.\n");
                exit(-1);
            }
        }
        else if (!strcmp(argv[arg], "--dbm-store-path"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--dbm-store-path came without an argument.\n");
                exit(-1);
            }
            dbm_store_path = argv[arg];
        }
        else if (!strcmp(argv[arg], "--iters-delta-limit"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(
                    stderr, "--iters-delta-limit came without an argument.\n");
                exit(-1);
            }
            iters_delta_limit = atol(argv[arg]);
        }
        else if (!strcmp(argv[arg], "-o"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "-o came without an argument.\n");
                exit(-1);
            }
            out_filename = argv[arg];
        }
        else if (!strcmp(argv[arg], "--offload-dir-path"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(
                    stderr, "--offload-dir-path came without an argument.\n");
                exit(-1);
            }
            offload_dir_path = argv[arg];
        }
        else
        {
            break;
        }
    }

    if (arg < argc - 1)
    {
        fprintf(stderr, "%s\n", "Junk arguments!");
        exit(-1);
    }
    else if (arg == argc)
    {
        fprintf(stderr, "%s\n", "No board specified.");
        exit(-1);
    }

    if (out_filename)
    {
        out_fh = fopen(out_filename, "at");
        if (!out_fh)
        {
            fprintf(stderr, "Cannot open '%s' for output.\n", "out_filename");
            exit(-1);
        }
    }
    else
    {
        out_fh = stdout;
    }

    filename = argv[arg];

    fh = fopen(filename, "r");
    if (fh == NULL)
    {
        fprintf(stderr, "Could not open file '%s' for input.\n", filename);
        exit(-1);
    }
    memset(user_state, '\0', sizeof(user_state));
    fread(user_state, sizeof(user_state[0]), USER_STATE_SIZE - 1, fh);
    fclose(fh);

    fc_solve_initial_user_state_to_c(user_state, &init_state, FREECELLS_NUM,
        STACKS_NUM, DECKS_NUM, init_indirect_stacks_buffer);

    {
        fcs_which_moves_bitmask_t which_no_use = {{'\0'}};
        horne_prune(local_variant, &init_state, &which_no_use, NULL, NULL);
    }

    fc_solve_delta_stater_init(&delta, &init_state.s, STACKS_NUM, FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
        ,
        ((local_variant == FCS_DBM_VARIANT_BAKERS_DOZEN)
                ? FCS_SEQ_BUILT_BY_RANK
                : FCS_SEQ_BUILT_BY_ALTERNATE_COLOR)
#endif
            );

    {
        fcs_dbm_solver_instance_t instance;
        fcs_encoded_state_buffer_t *key_ptr;
#define KEY_PTR() (key_ptr)

        fcs_encoded_state_buffer_t parent_state_enc;

        instance_init(&instance, local_variant, pre_cache_max_count,
            caches_delta, dbm_store_path, iters_delta_limit, offload_dir_path,
            out_fh);

        key_ptr = &(instance.first_key);
        fcs_init_and_encode_state(
            &delta, local_variant, &(init_state), KEY_PTR());

        /* The NULL parent_state_enc and move for indicating this is the
         * initial state. */
        fcs_init_encoded_state(&(parent_state_enc));

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
        pre_cache_insert(&(instance.pre_cache), KEY_PTR(), &parent_state_enc);
#else
        cache_insert(&(instance.cache), KEY_PTR(), NULL, '\0');
#endif
#else
        token = fc_solve_dbm_store_insert_key_value(
            instance.colls_by_depth[0].store, KEY_PTR(), NULL, TRUE);
#endif

        fcs_offloading_queue__insert(&(instance.colls_by_depth[0].queue),
            (const fcs_offloading_queue_item_t *)(&token));
        instance.num_states_in_collection++;
        instance.count_of_items_in_queue++;

        instance_run_all_threads(&instance, &init_state, NUM_THREADS());
        handle_and_destroy_instance_solution(&instance, out_fh, &delta);
    }

    fc_solve_delta_stater_release(&delta);

    if (out_filename)
    {
        fclose(out_fh);
        out_fh = NULL;
    }

    return 0;
}
