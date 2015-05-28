static GCC_INLINE fcs_bool_t pre_cache_lookup_parent(
    fcs_pre_cache_t * pre_cache,
    fcs_encoded_state_buffer_t * key,
    fcs_encoded_state_buffer_t * parent
    )
{
    fcs_pre_cache_key_val_pair_t to_check;
    dict_key_t found_key;

    to_check.key = *key;

    found_key = fc_solve_kaz_tree_lookup_value(pre_cache->kaz_tree, &to_check);

    if (found_key)
    {
        *parent =
            ((fcs_pre_cache_key_val_pair_t *)(found_key))->parent;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
