#if 0
#ifndef FCS_DBM_CACHE_ONLY
#ifndef FCS_DBM_WITHOUT_CACHES
        if (! pre_cache_lookup_parent(
            &(instance->pre_cache),
            key_ptr,
            (key_ptr+1)
            ))
#endif
        {
            if (!fc_solve_dbm_store_lookup_parent(
                instance->store,
                key_ptr->s,
                key_ptr[1].s
                ))
            {
                fprintf(stderr, "Trace problem in trace No. %d. Exiting\n", trace_num);
                exit(-1);
            }
        }
#endif
#endif
