#ifdef FCS_WITH_TALONS
    /* Initialize the Talon's Cache */
    if (instance->talon_type == FCS_TALON_KLONDIKE)
    {
        instance->talons_hash = fc_solve_hash_init(
            fcs_talon_compare_with_context,
            NULL
            );

        fc_solve_cache_talon(instance, instance->state_copy_ptr_val);
    }
#endif

static GCC_INLINE void normalize_befs_weights(
    fc_solve_state_weighting_t * const weighting
)
{
    /* Normalize the Best-First-Search Weights, so the sum of all of them would be 1. */

    double sum = 0;

#define my_befs_weights weighting->befs_weights
    for (
        int weight_idx = 0
            ;
        weight_idx < COUNT(my_befs_weights)
            ;
        weight_idx++
    )
    {
        if (unlikely(my_befs_weights[weight_idx] < 0))
        {
            my_befs_weights[weight_idx] = fc_solve_default_befs_weights[weight_idx];
        }
        sum += my_befs_weights[weight_idx];
    }
    if (unlikely(sum < 1e-6))
    {
        sum = 1;
    }
    for (int weight_idx=0 ; weight_idx < COUNT(my_befs_weights) ; weight_idx++)
    {
        my_befs_weights[weight_idx] /= sum;
    }
}

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    instance->num_prev_states_margin = 0;

    instance->num_indirect_prev_states = 0;

    free(instance->indirect_prev_states);
    instance->indirect_prev_states = NULL;
