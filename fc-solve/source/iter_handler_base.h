static GCC_INLINE void my_iter_handler_base(const fcs_int_limit_t iter_num,
    const int depth, void *const user_instance,
    const fc_solve_display_information_context_t *const display_context,
    const fcs_int_limit_t parent_iter_num)
{
    printf("Iteration: %li\nDepth: %i\nStored-States: %li\nScan: %s\n",
        (long)iter_num, depth,
        (long)freecell_solver_user_get_num_states_in_collection_long(
               user_instance),
        freecell_solver_user_get_current_soft_thread_name(user_instance));
    if (display_context->display_parent_iter_num)
    {
        printf("Parent Iteration: %li\n", (long)parent_iter_num);
    }
    putchar('\n');
}
