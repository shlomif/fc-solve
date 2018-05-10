/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2013 Shlomi Fish
 */
/*
 * depth_multi_queue.h - header file for the depth-based / depth-tracking
 * multi-queue which can handle more than one starting point.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "offloading_queue.h"

typedef struct
{
    const char *offload_dir_path;
    fcs_queue_stats stats;
    long min_depth, max_depth, max_depth_margin;
    /*
     * page_to_write_to, page_for_backup and page_to_read_from always
     * point to the two "pages" below, but they can be swapped and
     * page_for_backup may be NULL.
     */
    fcs_offloading_queue *queues_by_depth;
    long next_queue_id;
#ifndef FCS_DBM_USE_OFFLOADING_QUEUE
    meta_allocator *meta_alloc;
#endif
} fcs_depth_multi_queue;

#define DEPTH_Q_GROW_BY 32

static inline void fcs_depth_multi_queue__new_queue(
    fcs_depth_multi_queue *const queue, fcs_offloading_queue *const q)
{
    fcs_offloading_queue__init(q
#ifdef FCS_DBM_USE_OFFLOADING_QUEUE
        ,
        queue->offload_dir_path, (queue->next_queue_id++)
#else
        ,
        queue->meta_alloc
#endif
    );
}

static inline void fcs_depth_multi_queue__insert(
    fcs_depth_multi_queue *const queue, const int depth,
    const offloading_queue_item *const item)
{
    while (depth > queue->max_depth)
    {
        queue->max_depth++;
        if (queue->max_depth == queue->max_depth_margin)
        {
            queue->max_depth_margin += DEPTH_Q_GROW_BY;
            queue->queues_by_depth =
                SREALLOC(queue->queues_by_depth, queue->max_depth_margin);
        }
        fcs_depth_multi_queue__new_queue(queue,
            &(queue->queues_by_depth[queue->max_depth - queue->min_depth]));
    }

    fcs_offloading_queue__insert(
        &(queue->queues_by_depth[depth - queue->min_depth]), item);

    q_stats_insert(&queue->stats);
}

static inline void fcs_depth_multi_queue__init(
    fcs_depth_multi_queue *const queue, const char *offload_dir_path,
    const int first_depth, const offloading_queue_item *const first_item)
{
    queue->offload_dir_path = offload_dir_path;
    fcs_queue_stats_init(&queue->stats);
    queue->next_queue_id = 0;
    queue->min_depth = first_depth;
    queue->max_depth_margin = first_depth + DEPTH_Q_GROW_BY;

    queue->queues_by_depth = SMALLOC(
        queue->queues_by_depth, queue->max_depth_margin - queue->min_depth + 1);
    fcs_depth_multi_queue__new_queue(queue, &(queue->queues_by_depth[0]));
    queue->max_depth = first_depth;

    fcs_depth_multi_queue__insert(queue, first_depth, first_item);
}

static inline void fcs_depth_multi_queue__destroy(
    fcs_depth_multi_queue *const queue)
{
    const int limit = queue->max_depth - queue->min_depth;
    if (queue->queues_by_depth == NULL)
    {
        return;
    }
    for (int i = 0; i <= limit; i++)
    {
        fcs_offloading_queue__destroy(&(queue->queues_by_depth[i]));
    }
    free(queue->queues_by_depth);
    queue->queues_by_depth = NULL;
}

static inline bool fcs_depth_multi_queue__extract(
    fcs_depth_multi_queue *const queue, int *const return_depth,
    offloading_queue_item *const return_item)
{
    if (q_stats_is_empty(&queue->stats))
    {
        return FALSE;
    }

    while (q_stats_is_empty(&queue->queues_by_depth[0].stats))
    {
        var_AUTO(save_queue, queue->queues_by_depth[0]);
        memmove(queue->queues_by_depth, queue->queues_by_depth + 1,
            sizeof(queue->queues_by_depth[0]) *
                (queue->max_depth - queue->min_depth));
        queue->queues_by_depth[queue->max_depth - queue->min_depth] =
            save_queue;
        queue->max_depth++;
        queue->min_depth++;
        queue->max_depth_margin++;
    }

    *return_depth = queue->min_depth;
    fcs_offloading_queue__extract(&(queue->queues_by_depth[0]), return_item);
    q_stats_extract(&queue->stats);
    return TRUE;
}

#ifdef __cplusplus
}
#endif
