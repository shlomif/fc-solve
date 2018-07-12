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
 * dbm_solver.c - a specialised solver that offloads the states' collection
 * to an on-disk DBM database such as Berkeley DB or Google's LevelDB. Has
 * been adapted to be completely in-memory. It makes use of delta_states.c
 * for a very compact storage.
 */

#include "dbm_solver_head.h"

typedef struct
{
    fcs_dbm__cache_store__common cache_store;
    /* The queue */
    meta_allocator meta_alloc;
    fcs_offloading_queue queue;
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
    const char *offload_dir_path;
#endif
    dbm_instance_common_elems common;
} dbm_solver_instance;

#define CHECK_KEY_CALC_DEPTH() 0

#include "dbm_procs.h"
static inline void instance_init(dbm_solver_instance *const instance,
    const fcs_dbm_common_input *const inp, const long iters_delta_limit,
    FILE *const out_fh)
{
    fc_solve_meta_compact_allocator_init(&(instance->meta_alloc));
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
    fcs_offloading_queue__init(&(instance->queue),
        instance->offload_dir_path = inp->offload_dir_path, 0);
#else
    fcs_offloading_queue__init(&(instance->queue), &(instance->meta_alloc));
#endif
    fcs_dbm__common_init(
        &(instance->common), iters_delta_limit, inp->local_variant, out_fh);
    fcs_dbm__cache_store__init(&(instance->cache_store), &(instance->common),
        &(instance->meta_alloc), inp->dbm_store_path, inp->pre_cache_max_count,
        inp->caches_delta);
}

static inline void instance_recycle(dbm_solver_instance *const instance)
{
    fcs_offloading_queue__destroy(&(instance->queue));
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
    fcs_offloading_queue__init(
        &(instance->queue), instance->offload_dir_path, 0);
#else
    fcs_offloading_queue__init(&(instance->queue), &(instance->meta_alloc));
#endif

    instance->common.should_terminate = DONT_TERMINATE;
    instance->common.queue_num_extracted_and_processed = 0;
    instance->common.num_states_in_collection = 0;
    instance->common.count_num_processed = 0;
    instance->common.count_of_items_in_queue = 0;
}

static inline void instance_destroy(dbm_solver_instance *const instance)
{
    fcs_offloading_queue__destroy(&(instance->queue));
    DESTROY_CACHE(instance);
    fc_solve_meta_compact_allocator_finish(&(instance->meta_alloc));
    fcs_lock_destroy(&instance->common.storage_lock);
}

static inline void instance_check_key(
    dbm_solver_thread *const thread GCC_UNUSED,
    dbm_solver_instance *const instance, const int key_depth GCC_UNUSED,
    fcs_encoded_state_buffer *const key, fcs_dbm_record *const parent,
    const unsigned char move GCC_UNUSED,
    const fcs_which_moves_bitmask *const which_irreversible_moves_bitmask
        GCC_UNUSED
#ifdef FCS_DBM_CACHE_ONLY
    ,
    const fcs_fcc_move *moves_to_parent
#endif
)
{
    fcs_dbm_record *token;
    if ((token = cache_store__has_key(&instance->cache_store, key, parent)))
    {
#ifndef FCS_DBM_WITHOUT_CACHES
        fcs_cache_key_info *cache_key = cache_store__insert_key(
            &(instance->cache_store), key, parent, moves_to_parent, move);
#endif
        /* Now insert it into the queue. */

        ++instance->common.count_of_items_in_queue;
        ++instance->common.num_states_in_collection;

        instance_debug_out_state(instance, &(token->key));

        fcs_offloading_queue__insert(
            &(instance->queue), ((offloading_queue_item *)(&token)));
        ++instance->common.count_of_items_in_queue;
    }
}

struct fcs_dbm_solver_thread_struct
{
    dbm_solver_instance *instance;
    fcs_delta_stater delta_stater;
};

static void *instance_run_solver_thread(void *const void_arg)
{
    fcs_dbm_queue_item physical_item;
    fcs_state_keyval_pair state;
    fcs_dbm_record *token = NULL;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    const_AUTO(thread, ((thread_arg *)void_arg)->thread);
    dbm_solver_instance *const instance = thread->instance;
    fcs_delta_stater *const delta_stater = &(thread->delta_stater);

    fcs_dbm_queue_item *item = NULL, *prev_item = NULL;
    long queue_num_extracted_and_processed = 0;

    compact_allocator derived_list_allocator;
    fc_solve_compact_allocator_init(
        &(derived_list_allocator), &(instance->meta_alloc));
    fcs_derived_state *derived_list_recycle_bin = NULL;
    fcs_derived_state *derived_list = NULL;
    FILE *const out_fh = instance->common.out_fh;
    fcs_dbm_variant_type local_variant = instance->common.variant;

    TRACE("%s\n", "instance_run_solver_thread start");
#ifdef DEBUG_OUT
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);
#endif
    while (1)
    {
        /* First of all extract an item. */
        fcs_lock_lock(&instance->common.storage_lock);

        if (prev_item)
        {
            --instance->common.queue_num_extracted_and_processed;
        }

        if (instance->common.should_terminate == DONT_TERMINATE)
        {
            if (fcs_offloading_queue__extract(
                    &(instance->queue), (offloading_queue_item *)(&token)))
            {
                physical_item.key = token->key;
                item = &physical_item;
                instance_increment(instance);
            }
            else
            {
                item = NULL;
            }

            queue_num_extracted_and_processed =
                instance->common.queue_num_extracted_and_processed;
        }
        fcs_lock_unlock(&instance->common.storage_lock);

        if ((instance->common.should_terminate != DONT_TERMINATE) ||
            (!queue_num_extracted_and_processed))
        {
            break;
        }

        if (!item)
        {
            /* Sleep until more items become available in the
             * queue. */
            const struct timespec ts = {.tv_sec = 0, .tv_nsec = 5000000};
            struct timespec no_use;
            nanosleep(&ts, &no_use);
        }
        else
        {
            /* Handle item. */
            fc_solve_delta_stater_decode_into_state(
                delta_stater, item->key.s, &state, indirect_stacks_buffer);
            /* A section for debugging. */
            FCS__OUTPUT_STATE(out_fh, "", &(state.s), &locs);

            if (instance_solver_thread_calc_derived_states(local_variant,
                    &state, token, &derived_list, &derived_list_recycle_bin,
                    &derived_list_allocator, TRUE))
            {
                fcs_lock_lock(&instance->common.storage_lock);
                fcs_dbm__found_solution(&(instance->common), token, item);
                fcs_lock_unlock(&instance->common.storage_lock);
                break;
            }

            /* Encode all the states. */
            for (var_AUTO(derived_iter, derived_list); derived_iter;
                 derived_iter = derived_iter->next)
            {
                fcs_init_and_encode_state(delta_stater, local_variant,
                    &(derived_iter->state), &(derived_iter->key));
            }

            instance_check_multiple_keys(
                thread, instance, &(instance->cache_store),
                &(instance->meta_alloc), &derived_list, 1
#ifdef FCS_DBM_CACHE_ONLY
                ,
                item->moves_to_key
#endif
            );

            fcs_derived_state_list__recycle(
                &derived_list_recycle_bin, &derived_list);
            /* End handle item. */
        }
        /* End of main thread loop */
        prev_item = item;
    }

    fc_solve_compact_allocator_finish(&(derived_list_allocator));
    TRACE("%s\n", "instance_run_solver_thread end");
    return NULL;
}

static bool populate_instance_with_intermediate_input_line(
    dbm_solver_instance *const instance, fcs_delta_stater *const delta,
    fcs_state_keyval_pair *const init_state_ptr, char *const line,
    const long line_num, fcs_encoded_state_buffer *const parent_state_enc)
{
    fcs_encoded_state_buffer final_stack_encoded_state;
    fcs_encoded_state_buffer running_key;
    fcs_state_keyval_pair running_state;
    fcs_dbm_record *token = NULL;
#ifdef DEBUG_OUT
    fcs_state_locs_struct locs;
    fc_solve_init_locs(&locs);
#endif

    DECLARE_IND_BUF_T(running_indirect_stacks_buffer)

    const_AUTO(local_variant, instance->common.variant);
    fc_solve_state_init(
        &running_state, STACKS_NUM, running_indirect_stacks_buffer);
    fcs_init_encoded_state(&(final_stack_encoded_state));
    *parent_state_enc = final_stack_encoded_state;

    const char *s_ptr = line;
    while (*(s_ptr) != '|')
    {
        unsigned int hex_digits;
        if (sscanf(s_ptr, "%2X", &hex_digits) != 1)
        {
            fc_solve_err("Error in reading state in line %ld of the "
                         "--intermediate-input",
                line_num);
        }
#ifdef FCS_DEBONDT_DELTA_STATES
        final_stack_encoded_state.s[(s_ptr - line) >> 1] =
            (unsigned char)hex_digits;
#else
        final_stack_encoded_state.s[++final_stack_encoded_state.s[0]] =
            (unsigned char)hex_digits;
#endif
        s_ptr += 2;
    }

    /* Skip the '|'. */
    ++s_ptr;

    FCS_STATE__DUP_keyval_pair(running_state, *init_state_ptr);

    /* The NULL parent and move for indicating this is the initial
     * state. */
    fcs_init_and_encode_state(
        delta, local_variant, &(running_state), &running_key);
    fcs_dbm_record *running_parent = NULL;

#ifdef FCS_DBM_CACHE_ONLY
    fcs_fcc_move *running_moves = NULL;
    cache_store__insert_key(&(instance->cache_store), &(running_key),
        &running_parent, running_moves, '\0');
#else
    running_parent = fc_solve_dbm_store_insert_key_value(
        instance->cache_store.store, &(running_key), running_parent, TRUE);
#endif
    ++instance->common.num_states_in_collection;

    unsigned int hex_digits;
    while (sscanf(s_ptr, "%2X,", &hex_digits) == 1)
    {
        FCS__OUTPUT_STATE(
            stdout, "BEFORE_RUNNING_STATE == ", &(running_state.s), &locs);
        fcs_card src_card;

        s_ptr += 3;

        const_AUTO(move, (fcs_fcc_move)hex_digits);
        /* Apply the move. */
        int src = (move & 0xF);
        int dest = ((move >> 4) & 0xF);

#define the_state (running_state.s)
        /* Extract the card from the source. */
        if (src < 8)
        {
            var_AUTO(src_col, fcs_state_get_col(the_state, src));
            fcs_col_pop_card(src_col, src_card);
        }
        else
        {
            src -= 8;
            if (src >= 4)
            {
                fc_solve_err("Error in reading state in line %ld of the "
                             "--intermediate-input - source cannot be a "
                             "foundation.",
                    line_num);
            }
#if MAX_NUM_FREECELLS > 0
            src_card = fcs_freecell_card(the_state, src);
            fcs_empty_freecell(the_state, src);
#else
            abort();
#endif
        }
        /* Apply src_card to dest. */
        if (dest < 8)
        {
            fcs_state_push(&the_state, dest, src_card);
        }
        else
        {
            dest -= 8;
            if (dest < 4)
            {
#if MAX_NUM_FREECELLS > 0
                fcs_put_card_in_freecell(the_state, dest, src_card);
#endif
            }
            else
            {
                dest -= 4;
                fcs_increment_foundation(the_state, dest);
            }
        }
#undef the_state
        horne_prune__simple(local_variant, &running_state);

        fcs_init_and_encode_state(
            delta, local_variant, &(running_state), &(running_key));

#ifndef FCS_DBM_WITHOUT_CACHES
        var_AUTO(cache_ret,
            cache_store__insert_key(&(instance->cache_store), &(running_key),
                &running_parent, running_moves, move));
        if (cache_ret)
        {
            running_moves = cache_ret->moves_to_key;
        }
#else
        token = fc_solve_dbm_store_insert_key_value(
            instance->cache_store.store, &(running_key), running_parent, TRUE);
        if (!token)
        {
            return FALSE;
        }
#endif
        ++instance->common.num_states_in_collection;

        /* We need to do the round-trip from encoding back
         * to decoding, because the order can change after
         * the encoding.
         * */
        fc_solve_delta_stater_decode_into_state(delta, running_key.s,
            &running_state, running_indirect_stacks_buffer);
        FCS__OUTPUT_STATE(
            stdout, "RUNNING_STATE == ", &(running_state.s), &locs);
    }

    if (memcmp(&running_key, &final_stack_encoded_state, sizeof(running_key)) !=
        0)
    {
        fc_solve_err("Error in reading state in line %ld of the "
                     "--intermediate-input - final state does not match "
                     "that with all states applied.\n",
            line_num);
    }
    fcs_offloading_queue__insert(
        &(instance->queue), (const offloading_queue_item *)(&token));
    ++instance->common.count_of_items_in_queue;

    return TRUE;
}

#include "dbm_procs_inner.h"

static void init_thread(dbm_solver_thread *const thread GCC_UNUSED) {}
static void free_thread(dbm_solver_thread *const thread GCC_UNUSED) {}

static void instance_run_all_threads(dbm_solver_instance *const instance,
    fcs_state_keyval_pair *init_state, const size_t num_threads)
{
    const_AUTO(threads,
        dbm__calc_threads(instance, init_state, num_threads, init_thread));
    for (size_t i = 0; i < num_threads; i++)
    {
        if (pthread_create(&(threads[i].id), NULL, instance_run_solver_thread,
                &(threads[i].arg)))
        {
            fc_solve_err("Worker Thread No. %zu Initialization failed!\n", i);
        }
    }

    for (size_t i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i].id, NULL);
    }

    dbm__free_threads(instance, num_threads, threads, free_thread);
}

/* Returns if the process should terminate. */
static bool handle_and_destroy_instance_solution(
    dbm_solver_instance *const instance, fcs_delta_stater *const delta)
{
    FILE *const out_fh = instance->common.out_fh;
    bool ret = FALSE;
    fcs_dbm_record *token;
#ifdef DEBUG_OUT
    fcs_dbm_variant_type local_variant = instance->common.variant;
#endif

    TRACE("%s\n", "handle_and_destroy_instance_solution start");
    instance_print_stats(instance);

    if (instance->common.queue_solution_was_found)
    {
        trace_solution(instance, out_fh, delta);
        ret = TRUE;
    }
    else if (instance->common.should_terminate != DONT_TERMINATE)
    {
        fprintf(out_fh, "%s\n", "Intractable.");
        fflush(out_fh);
        if (instance->common.should_terminate == QUEUE_TERMINATE)
        {
            fcs_dbm_queue_item physical_item;
            fcs_dbm_queue_item *const item = &physical_item;

            while (fcs_offloading_queue__extract(
                &(instance->queue), (offloading_queue_item *)(&token)))
            {
                physical_item.key = token->key;

#ifdef FCS_DEBONDT_DELTA_STATES
                for (size_t i = 0; i < COUNT(item->key.s); i++)
                {
                    fprintf(out_fh, "%.2X", (int)item->key.s[i]);
                }
#else
                for (size_t i = 0; i < item->key.s[0]; i++)
                {
                    fprintf(out_fh, "%.2X", (int)item->key.s[1 + i]);
                }
#endif
                fprintf(out_fh, "%s", "|");
                fflush(out_fh);

#ifdef FCS_DBM_CACHE_ONLY
                fcs_fcc_move *move_ptr = item->moves_to_key;
                if (move_ptr)
                {
                    while (*(move_ptr))
                    {
                        fprintf(out_fh, "%.2X,", *(move_ptr));
                        move_ptr++;
                    }
                }
#else
                int trace_num;
                fcs_encoded_state_buffer *trace;
                calc_trace(token, &trace, &trace_num);

/*
 * We stop at 1 because the deepest state does not contain
 * a move (as it is the ultimate state).
 * */
#define PENULTIMATE_DEPTH 1
                for (int i = trace_num - 1; i >= PENULTIMATE_DEPTH; i--)
                {
                    fprintf(out_fh, "%.2X,",
                        get_move_from_parent_to_child(
                            instance, delta, trace[i], trace[i - 1]));
                }
#undef PENULTIMATE_DEPTH
                free(trace);
#endif
                fprintf(out_fh, "\n");
                fflush(out_fh);
#ifdef DEBUG_OUT
                fcs_state_keyval_pair state;
                fcs_state_locs_struct locs;
                fc_solve_init_locs(&locs);
                DECLARE_IND_BUF_T(indirect_stacks_buffer)

                fc_solve_delta_stater_decode_into_state(
                    delta, item->key.s, &state, indirect_stacks_buffer);

                FCS__OUTPUT_STATE(out_fh, "", &(state.s), &locs);
#endif
            }
        }
        else if (instance->common.should_terminate == MAX_ITERS_TERMINATE)
        {
            fprintf(out_fh, "Reached Max-or-more iterations of %ld.\n",
                instance->common.max_count_num_processed);
        }
    }
    else
    {
        fprintf(out_fh, "%s\n", "Could not solve successfully.");
    }

    TRACE("%s\n", "handle_and_destroy_instance_solution end");
    instance_destroy(instance);

    return ret;
}

int main(int argc, char *argv[])
{
    long start_line = 1;
    const char *out_filename = NULL, *intermediate_input_filename = NULL;
    FILE *intermediate_in_fh = NULL;
    bool skip_queue_output = FALSE;
    DECLARE_IND_BUF_T(init_indirect_stacks_buffer)
    const char *param;
    fcs_dbm_common_input inp = fcs_dbm_common_input_init;

    int arg;
    for (arg = 1; arg < argc; arg++)
    {
        if (fcs_dbm__extract_common_from_argv(argc, argv, &arg, &inp))
        {
        }
        else if ((param = TRY_P("--start-line")))
        {
            start_line = atol(param);
        }
        else if ((param = TRY_P("-o")))
        {
            out_filename = param;
        }
        else if ((param = TRY_P("--intermediate-input")))
        {
            intermediate_input_filename = param;
        }
        else
        {
            break;
        }
    }

    if (arg < argc - 1)
    {
        fc_solve_err("%s\n", "Junk arguments!");
    }
    if (arg == argc)
    {
        fc_solve_err("%s\n", "No board specified.");
    }
    const_AUTO(num_threads, inp.num_threads);

    FILE *const out_fh = calc_out_fh(out_filename);
    fcs_state_keyval_pair init_state;
    read_state_from_file(inp.local_variant, argv[arg],
        &init_state PASS_IND_BUF_T(init_indirect_stacks_buffer));
    horne_prune__simple(inp.local_variant, &init_state);

    const_AUTO(local_variant, inp.local_variant);
    fcs_delta_stater delta;
    fc_solve_delta_stater_init(&delta, local_variant, &init_state.s, STACKS_NUM,
        FREECELLS_NUM PASS_ON_NOT_FC_ONLY(FCS_SEQ_BUILT_BY_ALTERNATE_COLOR));

    if (intermediate_input_filename)
    {
        intermediate_in_fh = fopen(intermediate_input_filename, "rt");
        if (!intermediate_in_fh)
        {
            fc_solve_err(
                "Could not open file '%s' as --intermediate-input-filename.\n",
                intermediate_input_filename);
        }
    }

    if (intermediate_in_fh)
    {
#ifdef HAVE_GETLINE
        char *line = NULL;
        size_t line_size = 0;
#else
        size_t line_size = 16000;
        char *line = SMALLOC(line, line_size);
#endif
        long line_num = 0;
        bool queue_solution_was_found = FALSE;
        dbm_solver_instance queue_instance;
        dbm_solver_instance limit_instance;

        instance_init(&queue_instance, &inp, -1, out_fh);
        instance_init(&limit_instance, &inp, inp.iters_delta_limit, out_fh);

        bool found_line;
        do
        {
            ++line_num;
            found_line = FALSE;
#ifdef HAVE_GETLINE
            while (getline(&line, &line_size, intermediate_in_fh) >= 0)
#else
            while (fgets(line, line_size - 1, intermediate_in_fh))
#endif
            {
                if (strchr(line, '|') != NULL)
                {
                    found_line = TRUE;
                    break;
                }
                ++line_num;
            }

            if (found_line)
            {
                if (line_num < start_line)
                {
                    continue;
                }
                skip_queue_output = FALSE;
                {
                    fcs_encoded_state_buffer parent_state_enc;
                    if (!populate_instance_with_intermediate_input_line(
                            &limit_instance, &delta, &init_state, line,
                            line_num, &parent_state_enc))
                    {
                        continue;
                    }

                    instance_run_all_threads(
                        &limit_instance, &init_state, NUM_THREADS());

                    if (limit_instance.common.queue_solution_was_found)
                    {
                        trace_solution(&limit_instance, out_fh, &delta);
                        skip_queue_output = TRUE;
                        queue_solution_was_found = TRUE;
                    }
                    else if (limit_instance.common.should_terminate ==
                             MAX_ITERS_TERMINATE)
                    {
                        fprintf(out_fh,
                            "Reached Max-or-more iterations of %ld "
                            "in intermediate-input line No. %ld.\n",
                            limit_instance.common.max_count_num_processed,
                            line_num);
                    }
                    else if (limit_instance.common.should_terminate ==
                             DONT_TERMINATE)
                    {
                        fprintf(out_fh,
                            "Pruning due to unsolvability in "
                            "intermediate-input line No. %ld\n",
                            line_num);
                        skip_queue_output = TRUE;
                    }
                }

                if (!skip_queue_output)
                {
                    fcs_encoded_state_buffer parent_state_enc;
                    if (!populate_instance_with_intermediate_input_line(
                            &queue_instance, &delta, &init_state, line,
                            line_num, &parent_state_enc))
                    {
                        continue;
                    }

                    instance_run_all_threads(
                        &queue_instance, &init_state, NUM_THREADS());

                    if (handle_and_destroy_instance_solution(
                            &queue_instance, &delta))
                    {
                        queue_solution_was_found = TRUE;
                    }
                }
            }

            if (!queue_solution_was_found)
            {
                /*
                 * This recycles the instances by keeping the cache,
                 * but making sure that the statistics and the queue are reset.
                 * */
                instance_recycle(&limit_instance);
                instance_recycle(&queue_instance);
            }
        } while (found_line && (!queue_solution_was_found));

        instance_destroy(&limit_instance);
        if (!queue_solution_was_found)
        {
            instance_destroy(&queue_instance);
        }

        free(line);
        line = NULL;
    }
    else
    {
        dbm_solver_instance instance;
        instance_init(&instance, &inp, inp.iters_delta_limit, out_fh);
        fcs_encoded_state_buffer *key_ptr;
#define KEY_PTR() (key_ptr)
        fcs_encoded_state_buffer parent_state_enc;

        key_ptr = &(instance.common.first_key);
        fcs_init_and_encode_state(
            &delta, local_variant, &(init_state), KEY_PTR());
        /* The NULL parent_state_enc and move for indicating this is the
         * initial state. */
        fcs_init_encoded_state(&(parent_state_enc));

        fcs_dbm_record *token;
#ifndef FCS_DBM_WITHOUT_CACHES
        cache_store__insert_key(
            &(instance.cache_store), KEY_PTR(), &parent_state_enc, NULL, '\0');
#else
        token = fc_solve_dbm_store_insert_key_value(
            instance.cache_store.store, KEY_PTR(), NULL, TRUE);
#endif

        fcs_offloading_queue__insert(
            &(instance.queue), (const offloading_queue_item *)&token);
        ++instance.common.num_states_in_collection;
        ++instance.common.count_of_items_in_queue;

        instance_run_all_threads(&instance, &init_state, NUM_THREADS());
        handle_and_destroy_instance_solution(&instance, &delta);
    }

    fc_solve_delta_stater_release(&delta);
    if (out_filename)
    {
        fclose(out_fh);
    }
    if (intermediate_in_fh)
    {
        fclose(intermediate_in_fh);
    }

    return 0;
}
