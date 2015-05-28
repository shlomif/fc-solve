fcs_dbm_record_t * fc_solve_dbm_store_lookup_val(
    fcs_dbm_store_t store,
    const unsigned char * key
)
{
    fcs_dbm_record_t to_check;
    to_check.key = *(const fcs_encoded_state_buffer_t *)key;

    dict_key_t existing = fc_solve_kaz_tree_lookup_value(((dbm_t *)store)->kaz_tree, &to_check);

    return (fcs_dbm_record_t *)existing;
}
