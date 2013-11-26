/* Copyright (c) 2013 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * depth_multi_queue.h - header file for the depth-based / depth-tracking
 * multi-queue which can handle more than one starting point.
 */

#ifndef FC_SOLVE__DEPTH_MULTI_QUEUE_H
#define FC_SOLVE__DEPTH_MULTI_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

#include "config.h"
#include "state.h"
#include "inline.h"
#include "bool.h"
#include "alloc_wrap.h"

#include "offloading_queue.h"

typedef struct
{
    int num_items_per_page;
    const char * offload_dir_path;
    long num_inserted, num_items_in_queue, num_extracted;
    long min_depth, max_depth, max_depth_margin;
    /*
     * page_to_write_to, page_for_backup and page_to_read_from always
     * point to the two "pages" below, but they can be swapped and
     * page_for_backup may be NULL.
     */
     fcs_offloading_queue_t * queues_by_depth;
     long next_queue_id;
} fcs_depth_multi_queue_t;

#define DEPTH_Q_GROW_BY 32

static GCC_INLINE void fcs_depth_multi_queue__new_queue(
    fcs_depth_multi_queue_t * queue,
    fcs_offloading_queue_t * q
)
{
    fcs_offloading_queue__init(
        q,
        queue->num_items_per_page,
        queue->offload_dir_path,
        (queue->next_queue_id++)
    );
}

static GCC_INLINE void fcs_depth_multi_queue__insert(
    fcs_depth_multi_queue_t * const queue,
    const int depth,
    const fcs_offloading_queue_item_t * const item
)
{
    while (depth > queue->max_depth)
    {
        queue->max_depth++;
        if (queue->max_depth == queue->max_depth_margin)
        {
            queue->max_depth_margin += DEPTH_Q_GROW_BY;
            queue->queues_by_depth =
                SREALLOC(
                    queue->queues_by_depth,
                    queue->max_depth_margin
                );
        }
        fcs_depth_multi_queue__new_queue(
            queue,
            &(queue->queues_by_depth[queue->max_depth - queue->min_depth])
        );
    }

    fcs_offloading_queue__insert(
        &(queue->queues_by_depth[depth - queue->min_depth]),
        item
    );

    queue->num_inserted++;
    queue->num_items_in_queue++;
}

static GCC_INLINE void fcs_depth_multi_queue__init(
    fcs_depth_multi_queue_t * const queue,
    const int num_items_per_page,
    const char * offload_dir_path,
    const int first_depth,
    const fcs_offloading_queue_item_t * const first_item
    )
{
    queue->num_items_per_page = num_items_per_page;
    queue->offload_dir_path = offload_dir_path;
    queue->num_inserted = queue->num_items_in_queue = queue->num_extracted = 0;
    queue->next_queue_id = 0;
    queue->min_depth = first_depth;
    queue->max_depth_margin = first_depth + DEPTH_Q_GROW_BY;

    queue->queues_by_depth = SMALLOC(
        queue->queues_by_depth,
        queue->max_depth_margin - queue->min_depth + 1
    );
    fcs_depth_multi_queue__new_queue(
        queue,
        &(queue->queues_by_depth[0])
    );
    queue->max_depth = first_depth;

    fcs_depth_multi_queue__insert(
        queue,
        first_depth,
        first_item
    );
}

static GCC_INLINE void fcs_depth_multi_queue__destroy(
    fcs_depth_multi_queue_t * queue
    )
{
    int limit = queue->max_depth - queue->min_depth;
    for (int i=0 ; i <= limit ; i++)
    {
        fcs_offloading_queue__destroy(&(queue->queues_by_depth[i]));
    }
    free ( queue->queues_by_depth );
}


static GCC_INLINE fcs_bool_t fcs_depth_multi_queue__extract(
    fcs_depth_multi_queue_t * queue,
    int * return_depth,
    fcs_offloading_queue_item_t * return_item
)
{
    if (queue->num_items_in_queue == 0)
    {
        return FALSE;
    }

    while (queue->queues_by_depth[0].num_items_in_queue == 0)
    {
        typeof(queue->queues_by_depth[0]) save_queue = queue->queues_by_depth[0];
        memmove(
            queue->queues_by_depth,
            queue->queues_by_depth+1,
            sizeof(queue->queues_by_depth[0]) * (queue->max_depth - queue->min_depth)
        );
        queue->queues_by_depth[queue->max_depth - queue->min_depth] = save_queue;
        queue->max_depth++;
        queue->min_depth++;
        queue->max_depth_margin++;
    }

    *return_depth = queue->min_depth;
    fcs_offloading_queue__extract(
        &(queue->queues_by_depth[0]),
        return_item
    );

    queue->num_items_in_queue--;
    queue->num_extracted++;

    return TRUE;
}

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__DEPTH_MULTI_QUEUE_H */
