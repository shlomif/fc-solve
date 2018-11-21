/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// offloading_queue.h - header file for the offloading-to-hard-disk queue.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rinutils.h"
#ifdef __unix__
#include <fcntl.h>
#endif

typedef const unsigned char *offloading_queue_item;

typedef struct
{
    long num_inserted, num_items_in_queue, num_extracted;
} fcs_queue_stats;

static inline void fcs_queue_stats_init(fcs_queue_stats *const q)
{
    *q = (typeof(*q)){
        .num_inserted = 0, .num_items_in_queue = 0, .num_extracted = 0};
}

static inline void q_stats_insert(fcs_queue_stats *const q)
{
    q->num_inserted++;
    q->num_items_in_queue++;
}

static inline void q_stats_extract(fcs_queue_stats *const q)
{
    q->num_items_in_queue--;
    q->num_extracted++;
}

static inline bool q_stats_is_empty(fcs_queue_stats *const q)
{
    return (q->num_items_in_queue == 0);
}

#if !defined(FCS_DBM_USE_OFFLOADING_QUEUE)

typedef struct fcs_Q_item_wrapper_struct
{
    offloading_queue_item datum;
    struct fcs_Q_item_wrapper_struct *next;
} fcs_Q_item_wrapper;

typedef struct
{
    compact_allocator queue_allocator;
    fcs_Q_item_wrapper *queue_head, *queue_tail, *queue_recycle_bin;
    fcs_queue_stats stats;
} fcs_offloading_queue;

static inline void fcs_offloading_queue__init(
    fcs_offloading_queue *const queue, meta_allocator *const meta_alloc)
{
    fc_solve_compact_allocator_init(&(queue->queue_allocator), meta_alloc);

    queue->queue_head = queue->queue_tail = queue->queue_recycle_bin = NULL;
    fcs_queue_stats_init(&queue->stats);
}

static inline void fcs_offloading_queue__destroy(
    fcs_offloading_queue *const queue)
{
    fc_solve_compact_allocator_finish(&(queue->queue_allocator));
}

static inline bool fcs_offloading_queue__extract(
    fcs_offloading_queue *const queue, offloading_queue_item *const return_item)
{
    fcs_Q_item_wrapper *const item = queue->queue_head;

    if (!item)
    {
        *return_item = NULL;
        return FALSE;
    }

    *return_item = item->datum;

    if (!(queue->queue_head = item->next))
    {
        queue->queue_tail = NULL;
    }
    item->next = queue->queue_recycle_bin;
    queue->queue_recycle_bin = item;

    q_stats_extract(&queue->stats);
    return TRUE;
}

static inline void fcs_offloading_queue__insert(
    fcs_offloading_queue *const queue, const offloading_queue_item *const datum)
{
    fcs_Q_item_wrapper *new_item;
    if (queue->queue_recycle_bin)
    {
        queue->queue_recycle_bin = (new_item = queue->queue_recycle_bin)->next;
    }
    else
    {
        new_item = (fcs_Q_item_wrapper *)fcs_compact_alloc_ptr(
            &(queue->queue_allocator), sizeof(*new_item));
    }
    new_item->datum = *datum;
    new_item->next = NULL;

    if (queue->queue_tail)
    {
        queue->queue_tail = queue->queue_tail->next = new_item;
    }
    else
    {
        queue->queue_head = queue->queue_tail = new_item;
    }
    q_stats_insert(&queue->stats);
}

#else

static const size_t NUM_ITEMS_PER_PAGE = (128 * 1024);
typedef struct
{
    long page_index, queue_id;
    size_t write_to_idx;
    size_t read_from_idx;
    unsigned char *data;
} off_q_page;

static inline void fcs_offloading_queue_page__recycle(off_q_page *const page)
{
    page->write_to_idx = 0;
    page->read_from_idx = 0;
}

static inline void fcs_offloading_queue_page__init(
    off_q_page *const page, const long page_index, const long queue_id)
{
    *page = (typeof(*page)){.page_index = page_index,
        .queue_id = queue_id,
        .data = malloc(sizeof(offloading_queue_item) * NUM_ITEMS_PER_PAGE)};
    fcs_offloading_queue_page__recycle(page);
}

static inline void fcs_offloading_queue_page__destroy(off_q_page *const page)
{
    free(page->data);
    page->data = NULL;
}

static inline bool fcs_offloading_queue_page__can_extract(
    const off_q_page *const page)
{
    return (page->read_from_idx < page->write_to_idx);
}

static inline void fcs_offloading_queue_page__extract(
    off_q_page *const page, offloading_queue_item *const out_item)
{
    memcpy(out_item,
        (page->data + sizeof(*out_item) * ((page->read_from_idx)++)),
        sizeof(*out_item));
}

static inline bool fcs_offloading_queue_page__can_insert(
    const off_q_page *const page)
{
    return (page->write_to_idx < NUM_ITEMS_PER_PAGE);
}

static inline void fcs_offloading_queue_page__insert(
    off_q_page *const page, const offloading_queue_item *const in_item)
{
    memcpy(page->data + ((page->write_to_idx)++) * sizeof(*in_item), in_item,
        sizeof(*in_item));
}

static inline void fcs_offloading_queue_page__calc_filename(
    off_q_page *const page, char *const buffer,
    const char *const offload_dir_path)
{
    sprintf(buffer, "%s/fcs_queue%lXq_%020lX.page", offload_dir_path,
        (unsigned long)(page->queue_id), (unsigned long)(page->page_index));
}

static inline void fcs_offloading_queue_page__start_after(
    off_q_page *const page, const off_q_page *const other_page)
{
    page->page_index = other_page->page_index + 1;
    fcs_offloading_queue_page__recycle(page);
}

static inline void fcs_offloading_queue_page__bump(off_q_page *const page)
{
    fcs_offloading_queue_page__start_after(page, page);
}

static inline void fcs_offloading_queue_page__read_next_from_disk(
    off_q_page *const page, const char *const offload_dir_path)
{
    fcs_offloading_queue_page__bump(page);
    char page_filename[PATH_MAX + 1];
    fcs_offloading_queue_page__calc_filename(
        page, page_filename, offload_dir_path);
#ifdef __unix__
    const int f = open(page_filename, O_RDONLY);
    read(f, page->data, sizeof(offloading_queue_item) * NUM_ITEMS_PER_PAGE);
    close(f);
#else
    FILE *const f = fopen(page_filename, "rb");
    fread(page->data, sizeof(offloading_queue_item), NUM_ITEMS_PER_PAGE, f);
    fclose(f);
#endif

    /* We need to set this limit because it's a read-only page that we
     * retrieve from the disk and otherwise ->can_extract() will return
     * false for most items.
     * */
    page->write_to_idx = NUM_ITEMS_PER_PAGE;

    unlink(page_filename);
}

static inline void fcs_offloading_queue_page__offload(
    off_q_page *const page, const char *const offload_dir_path)
{
    char page_filename[PATH_MAX + 1];
    fcs_offloading_queue_page__calc_filename(
        page, page_filename, offload_dir_path);
#ifdef __unix__
    const int f = creat(page_filename, 0644);
    write(f, page->data, sizeof(offloading_queue_item) * NUM_ITEMS_PER_PAGE);
    close(f);
#else
    FILE *const f = fopen(page_filename, "wb");
    fwrite(page->data, sizeof(offloading_queue_item), NUM_ITEMS_PER_PAGE, f);
    fclose(f);
#endif
}

typedef struct
{
    const char *offload_dir_path;
    fcs_queue_stats stats;
    long id;
    /*
     * page_idx_to_write_to, page_idx_for_backup and page_idx_to_read_from
     * always point to the two "pages" below, but they can be swapped and
     * page_idx_for_backup may be NULL.
     */
    int_fast32_t page_idx_to_write_to, page_idx_for_backup,
        page_idx_to_read_from;
    off_q_page pages[2];
} fcs_offloading_queue;

static inline void fcs_offloading_queue__init(fcs_offloading_queue *const queue,
    const char *const offload_dir_path, const long id)
{
    queue->offload_dir_path = offload_dir_path;
    fcs_queue_stats_init(&queue->stats);
    queue->id = id;

    fcs_offloading_queue_page__init(&(queue->pages[0]), 0, queue->id);
    fcs_offloading_queue_page__init(&(queue->pages[1]), 0, queue->id);

    queue->page_idx_to_read_from = queue->page_idx_to_write_to = 0;
    queue->page_idx_for_backup = 1;
}

static inline void fcs_offloading_queue__destroy(
    fcs_offloading_queue *const queue)
{
    fcs_offloading_queue_page__destroy(&(queue->pages[0]));
    fcs_offloading_queue_page__destroy(&(queue->pages[1]));
}

static inline void fcs_offloading_queue__insert(
    fcs_offloading_queue *queue, const offloading_queue_item *item)
{
    if (!fcs_offloading_queue_page__can_insert(
            queue->pages + queue->page_idx_to_write_to))
    {
        if (queue->pages[queue->page_idx_to_read_from].page_index !=
            queue->pages[queue->page_idx_to_write_to].page_index)
        {
            fcs_offloading_queue_page__offload(
                queue->pages + queue->page_idx_to_write_to,
                queue->offload_dir_path);
            fcs_offloading_queue_page__bump(
                queue->pages + queue->page_idx_to_write_to);
        }
        else
        {
            queue->page_idx_to_write_to = queue->page_idx_for_backup;
            fcs_offloading_queue_page__start_after(
                queue->pages + queue->page_idx_to_write_to,
                queue->pages + queue->page_idx_to_read_from);
            queue->page_idx_for_backup = -1;
        }
    }

    fcs_offloading_queue_page__insert(
        queue->pages + queue->page_idx_to_write_to, item);
    q_stats_insert(&queue->stats);
}

static inline bool fcs_offloading_queue__extract(
    fcs_offloading_queue *const queue, offloading_queue_item *const return_item)
{
    if (q_stats_is_empty(&queue->stats))
    {
        *return_item = NULL;
        return FALSE;
    }

    if (!fcs_offloading_queue_page__can_extract(
            queue->pages + queue->page_idx_to_read_from))
    {
        /* Cannot really happen due to the num_items_in_queue check.
         *
         * if (queue->_page_idx_to_read_from->page_index ==
         *     queue->_page_idx_to_write_to->page_index)
         */
        if (queue->pages[queue->page_idx_to_read_from].page_index + 1 ==
            queue->pages[queue->page_idx_to_write_to].page_index)
        {
            queue->page_idx_for_backup = queue->page_idx_to_read_from;
            queue->page_idx_to_read_from = queue->page_idx_to_write_to;
        }
        else
        {
            fcs_offloading_queue_page__read_next_from_disk(
                queue->pages + queue->page_idx_to_read_from,
                queue->offload_dir_path);
        }
    }

    q_stats_extract(&queue->stats);

    fcs_offloading_queue_page__extract(
        queue->pages + queue->page_idx_to_read_from, return_item);

    return TRUE;
}

#endif

#ifdef __cplusplus
}
#endif
