#ifdef FCS_WITH_TALONS
static GCC_INLINE void fc_solve_cache_talon(
    fc_solve_instance_t * instance,
    fcs_state_t * new_state_key,
    fcs_state_extra_info_t * new_state_val
    )
{
    void * cached_talon;
    int hash_value_int;

    new_state_key->talon = realloc(new_state_key->talon, fcs_klondike_talon_len(new_state_key)+1);
    hash_value_int = *(SFO_hash_value_t*)instance->hash_value;
    if (hash_value_int < 0)
    {
        /*
         * This is a bit mask that nullifies the sign bit of the
         * number so it will always be positive
         * */
        hash_value_int &= (~(1<<((sizeof(hash_value_int)<<3)-1)));
    }

    cached_talon = (void *)fc_solve_hash_insert(
        instance->talons_hash,
        new_state_key->talon,
        hash_value_int,
        1
        );

    if (cached_talon != NULL)
    {
        free(new_state_key->talon);
        new_state_key->talon = cached_talon;
    }
}
#endif

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    fcs_standalone_state_ptrs_t * pos_ptr;
    /* Try to see if the state is found in indirect_prev_states */
    if ((pos_ptr = (fcs_standalone_state_ptrs_t *)bsearch(&new_state_key,
                instance->indirect_prev_states,
                instance->num_indirect_prev_states,
                sizeof(instance->indirect_prev_states[0]),
                fc_solve_state_compare_indirect)) == NULL)
    {
        fcs_bool_t found;
        /* It isn't in prev_states, but maybe it's in the sort margin */
        pos_ptr = (fcs_standalone_state_ptrs_t *)fc_solve_bsearch(
            &new_state_key,
            instance->indirect_prev_states_margin,
            instance->num_prev_states_margin,
            sizeof(instance->indirect_prev_states_margin[0]),
            fc_solve_state_compare_indirect_with_context,
            NULL,
            &found);

        if (found)
        {
            is_state_new = FALSE;
            existing_state_val = pos_ptr->val;
        }
        else
        {
            /* Insert the state into its corresponding place in the sort
             * margin */
            memmove((void*)(pos_ptr+1),
                    (void*)pos_ptr,
                    sizeof(*pos_ptr) *
                    (instance->num_prev_states_margin-
                      (pos_ptr-instance->indirect_prev_states_margin)
                    ));
            pos_ptr->key = new_state_key;
            pos_ptr->val = new_state_val;

            instance->num_prev_states_margin++;

            if (instance->num_prev_states_margin >= PREV_STATES_SORT_MARGIN)
            {
                /* The sort margin is full, let's combine it with the main array */

                instance->indirect_prev_states =
                    SREALLOC(
                        instance->indirect_prev_states,
                        instance->num_indirect_prev_states
                            + instance->num_prev_states_margin
                    );

                fc_solve_merge_large_and_small_sorted_arrays(
                    instance->indirect_prev_states,
                    instance->num_indirect_prev_states,
                    instance->indirect_prev_states_margin,
                    instance->num_prev_states_margin,
                    sizeof(instance->indirect_prev_states[0]),
                    fc_solve_state_compare_indirect_with_context,
                    NULL
                );

                instance->num_indirect_prev_states += instance->num_prev_states_margin;

                instance->num_prev_states_margin=0;
            }
            is_state_new = TRUE;
        }

    }
    else
    {
        existing_state_val = pos_ptr->val;
        is_state_new = FALSE;
    }
