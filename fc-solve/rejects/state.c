
#else

int fc_solve_state_compare_indirect(const void * const s1, const void * const s2)
{
    return memcmp(*(fcs_state_t * *)s1, *(fcs_state_t * *)s2, sizeof(fcs_state_t));
}

int fc_solve_state_compare_indirect_with_context(const void * s1, const void * s2, void * context)
{
    return memcmp(*(fcs_state_t * *)s1, *(fcs_state_t * *)s2, sizeof(fcs_state_t));
}

#endif
