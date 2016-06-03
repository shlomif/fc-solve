static GCC_INLINE void instance_recycle(
    fcs_dbm_solver_instance_t * const instance
    )
{
     for (int depth = instance->curr_depth ; depth < MAX_FCC_DEPTH ; depth++)
     {
         fcs_dbm_collection_by_depth_t * coll;

         coll = &(instance->colls_by_depth[depth]);
         fcs_offloading_queue__destroy(&(coll->queue));
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
         fcs_offloading_queue__init(&(coll->queue), NUM_ITEMS_PER_PAGE, instance->offload_dir_path, depth);
#else
         fcs_offloading_queue__init(&(coll->queue), &(coll->queue_meta_alloc));
#endif
     }

    instance->should_terminate = DONT_TERMINATE;
    instance->queue_num_extracted_and_processed = 0;
    instance->num_states_in_collection = 0;
    instance->count_num_processed = 0;
    instance->count_of_items_in_queue = 0;
}
