
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

