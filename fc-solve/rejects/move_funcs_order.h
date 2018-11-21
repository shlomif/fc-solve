fc_solve_string_to_test_num_compare_func(
    const void *const a, const void *const b)
{
#define MAP(x) (((const fcs_move_func_aliases_mapping_t *const)(x))->alias)
    return MAP(a) - MAP(b);
}
#undef MAP
