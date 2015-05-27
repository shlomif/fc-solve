
#define my_brfs_queue (soft_thread->method_specific.befs.meth.brfs.bfs_queue)
#define my_brfs_queue_last_item \
    (soft_thread->method_specific.befs.meth.brfs.bfs_queue_last_item)

static GCC_INLINE void fc_solve_free_bfs_queue(fc_solve_soft_thread_t * soft_thread)
{
    /* Free the BFS linked list */
    fcs_states_linked_list_item_t * item, * next_item;
    item = my_brfs_queue;
    while (item != NULL)
    {
        next_item = item->next;
        free(item);
        item = next_item;
    }
    my_brfs_queue = my_brfs_queue_last_item = NULL;
}

#undef my_brfs_queue
#undef my_brfs_queue_last_item

#ifdef FCS_WITH_TALONS
    /*
     * The talon for Gypsy-like games. Since only the position changes from
     * state to state.
     * We can keep it here.
     *
     * */
    fcs_card_t * gypsy_talon;

    /*
     * The length of the Gypsy talon
     * */
    int gypsy_talon_len;

    int talon_type;

    /* The Klondike Talons' Cache */
    fc_solve_hash_t * talons_hash;

#endif

#define FC_SOLVE_IS_DFS(soft_thread) \
    ((soft_thread)->super_method_type == FCS_SUPER_METHOD_DFS)

/*
    Clean up a solving process that was terminated in the middle.
    This function does not substitute for later calling
    finish_instance() and free_instance().
  */
static GCC_INLINE void fc_solve_unresume_instance(
    fc_solve_instance_t * const instance GCC_UNUSED
)
{
    /*
     * Do nothing - since finish_instance() can take care of solution_states
     * and proto_solution_moves as they were created by these scans, then
     * I don't need to do it here, too
     *
     * */
}

