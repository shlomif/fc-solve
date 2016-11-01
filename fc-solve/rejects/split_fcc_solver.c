#if 0
static GCC_INLINE void release_starting_state_specific_instance_resources(
    fcs_dbm_solver_instance_t * const instance
)
{
    fcs_dbm_collection_by_depth_t * coll = &(instance->coll);

    /* TODO : Implement. */
    DESTROY_CACHE(coll);
    /* That is important to avoid stale nodes. */
    instance->common.tree_recycle_bin = NULL;

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
        pre_cache_init (&(coll->pre_cache), &(coll->meta_alloc));
#endif
        coll->pre_cache_max_count = pre_cache_max_count;
        cache_init (&(coll->cache), pre_cache_max_count+caches_delta, &(coll->meta_alloc));
#endif
#ifndef FCS_DBM_CACHE_ONLY
        fc_solve_dbm_store_init(&(coll->store), instance->dbm_store_path, &(instance->common.tree_recycle_bin));
#endif
}
#endif

#if 0
        while (getline(&fingerprint_line, &fingerprint_line_size, fingerprint_fh) != -1)
        {
            char state_base64[100];
            FccEntryPointNode key;
            int state_depth;
            sscanf( fingerprint_line, "%99s %d", state_base64, &state_depth );
            size_t unused_size;
            base64_decode(state_base64, strlen(state_base64),
                ((unsigned char *)&(key.kv.key.key)), &(unused_size));


            FccEntryPointNode * val_proto = RB_FIND(
                FccEntryPointList,
                &(instance.fcc_entry_points),
                &(key)
            );

            if (! val_proto->kv.val.was_consumed)
            {
                char * moves_to_state_enc = strchr(fingerprint_line, ' ');
                moves_to_state_enc = strchr(moves_to_state_enc+1, ' ');
                moves_to_state_enc++;
                char *const trailing_newline = strchr(moves_to_state_enc, '\n');
                if (trailing_newline)
                {
                    *trailing_newline = '\0';
                }
                const size_t string_len = strlen(moves_to_state_enc);
                const size_t buffer_size = (((string_len * 3) >> 2) + 20);
                if (buffer_size > instance.max_moves_to_state_len)
                {
                    instance.moves_to_state = SREALLOC(
                        instance.moves_to_state,
                        buffer_size
                    );
                    instance.max_moves_to_state_len = buffer_size;
                }
                base64_decode (moves_to_state_enc, string_len,
                    ((unsigned char *)instance.moves_to_state),
                    &(instance.moves_to_state_len));

                /* TODO : Should traverse starting from key. */
                key_ptr = val_proto;
                /* The NULL parent_state_enc and move for indicating this is the
                 * initial state. */
                fcs_init_encoded_state(&(parent_state_enc));

#ifndef FCS_DBM_WITHOUT_CACHES
#ifndef FCS_DBM_CACHE_ONLY
                pre_cache_insert(&(instance.pre_cache), &(key_ptr->kv.key.key), &parent_state_enc);
#else
                cache_insert(&(instance.cache), &(key_ptr->kv.key.key), NULL, '\0');
#endif
#else
                token = fc_solve_dbm_store_insert_key_value(instance.coll.store, &(key_ptr->kv.key.key), NULL, TRUE);
#endif

                count_of_instance_runs++;

                char consumed_states_fn[PATH_MAX], consumed_states_fn_temp[PATH_MAX];
                sprintf(consumed_states_fn, "%s/consumed.%ld", path_to_output_dir, count_of_instance_runs);
                sprintf(consumed_states_fn_temp, "%s.temp", consumed_states_fn);
                instance.consumed_states_fh
                    = fopen(consumed_states_fn_temp, "wt");

                char fcc_exit_points_out_fn[PATH_MAX], fcc_exit_points_out_fn_temp[PATH_MAX];
                sprintf(fcc_exit_points_out_fn, "%s/exits.%ld", path_to_output_dir, count_of_instance_runs);
                sprintf(fcc_exit_points_out_fn_temp, "%s.temp", fcc_exit_points_out_fn);
                instance.fcc_exit_points_out_fh
                    = fopen(fcc_exit_points_out_fn_temp, "wt");

                instance_run_all_threads(&instance, &init_state, key_ptr, NUM_THREADS());

                fclose (instance.consumed_states_fh);
                rename(consumed_states_fn_temp, consumed_states_fn);

                fclose (instance.fcc_exit_points_out_fh);
                rename(fcc_exit_points_out_fn_temp, fcc_exit_points_out_fn);

                release_starting_state_specific_instance_resources(&instance);

            }

        }
#endif
