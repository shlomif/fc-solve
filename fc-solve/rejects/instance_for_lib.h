
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
            .num_indirect_prev_states = 0,
#endif
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    instance->num_prev_states_margin = 0;

    instance->indirect_prev_states = NULL;
