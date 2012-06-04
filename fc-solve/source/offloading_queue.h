/* Copyright (c) 2000 Shlomi Fish
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
 * offloading_queue.h - header file for the offloading-to-hard-disk
 * queue.
 */

#ifndef FC_SOLVE__OFFLOADING_QUEUE_H
#define FC_SOLVE__OFFLOADING_QUEUE_H

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

#include "delta_states.h"

#if 0
typedef struct { unsigned char s[24]; } fcs_offloading_queue_item_t;
#endif

typedef const unsigned char * fcs_offloading_queue_item_t;

typedef struct
{
    int num_items_per_page;
    long page_index;
    int write_to_idx;
    int read_from_idx;
    unsigned char * data;
} fcs_offloading_queue_page_t;

static GCC_INLINE void fcs_offloading_queue_page__recycle(
    fcs_offloading_queue_page_t * page
    )
{
    page->write_to_idx = 0;
    page->read_from_idx = 0;
}

static GCC_INLINE void fcs_offloading_queue_page__init(
    fcs_offloading_queue_page_t * page,
    int num_items_per_page,
    long page_index
    )
{
    page->num_items_per_page = num_items_per_page;
    page->page_index = page_index;

    page->data = malloc(
        sizeof(fcs_offloading_queue_item_t) * num_items_per_page
    );
    fcs_offloading_queue_page__recycle(page);

    return;
}

static GCC_INLINE void fcs_offloading_queue_page__destroy(
    fcs_offloading_queue_page_t * page
)
{
    free(page->data);
    page->data = NULL;
}

static GCC_INLINE fcs_bool_t fcs_offloading_queue_page__can_extract(
    fcs_offloading_queue_page_t * page
)
{
    return (page->read_from_idx < page->write_to_idx);
}

static GCC_INLINE void fcs_offloading_queue_page__extract(
    fcs_offloading_queue_page_t * page,
    fcs_offloading_queue_item_t * out_item
)
{
    int read_from_idx;

    read_from_idx = ((page->read_from_idx)++);

    memcpy(out_item, page->data + sizeof(fcs_offloading_queue_item_t) * read_from_idx, sizeof(fcs_offloading_queue_item_t));
}

static GCC_INLINE fcs_bool_t fcs_offloading_queue_page__can_insert(
    fcs_offloading_queue_page_t * page
)
{
    return (page->write_to_idx < page->num_items_per_page);
}

static GCC_INLINE void fcs_offloading_queue_page__insert(
    fcs_offloading_queue_page_t * page,
    const fcs_offloading_queue_item_t const * in_item
    )
{
    int write_to_idx = ((page->write_to_idx)++);

    memcpy(
        page->data+write_to_idx * sizeof(fcs_offloading_queue_item_t),
        in_item,
        sizeof(*in_item)
    );
}

static GCC_INLINE const char * fcs_offloading_queue_page__calc_filename(
    fcs_offloading_queue_page_t * page,
    char * buffer,
    const char * offload_dir_path)
{
    sprintf(buffer, "%s/fcs_queue_%020lX.page", offload_dir_path, page->page_index);

    return buffer;
}

static GCC_INLINE void fcs_offloading_queue_page__start_after(
    fcs_offloading_queue_page_t * page,
    fcs_offloading_queue_page_t * other_page
    )
{
    page->page_index = other_page->page_index+1;
    fcs_offloading_queue_page__recycle(page);
}

static GCC_INLINE void fcs_offloading_queue_page__bump(
    fcs_offloading_queue_page_t * page
)
{
    fcs_offloading_queue_page__start_after(page, page);
}

static GCC_INLINE void fcs_offloading_queue_page__read_next_from_disk(
    fcs_offloading_queue_page_t * page,
    const char * offload_dir_path
    )
{
    FILE * f;
    char page_filename[PATH_MAX+1];

    fcs_offloading_queue_page__bump(page);

    fcs_offloading_queue_page__calc_filename(page, page_filename, offload_dir_path);

    f = fopen(page_filename, "rb");
    fread( page->data, sizeof(fcs_offloading_queue_item_t),
           page->num_items_per_page, f
    );
    fclose(f);

    /* We need to set this limit because it's a read-only page that we
     * retrieve from the disk and otherwise ->can_extract() will return
     * false for most items.
     * */
    page->write_to_idx = page->num_items_per_page;

    unlink(page_filename);
}

static GCC_INLINE void fcs_offloading_queue_page__offload(
    fcs_offloading_queue_page_t * page,
    const char * offload_dir_path
    )
{
    FILE * f;
    char page_filename[PATH_MAX+1];

    fcs_offloading_queue_page__calc_filename(page, page_filename, offload_dir_path);

    f = fopen(page_filename, "wb");
    fwrite( page->data, sizeof(fcs_offloading_queue_item_t),
           page->num_items_per_page, f
    );
    fclose(f);
}

typedef struct
{
    int num_items_per_page;
    const char * offload_dir_path;
    long num_inserted, num_items_in_queue, num_extracted;
    /*
     * page_to_write_to, page_for_backup and page_to_read_from always
     * point to the two "pages" below, but they can be swapped and
     * page_for_backup may be NULL.
     */
    fcs_offloading_queue_page_t * page_to_write_to, * page_for_backup,
                                * page_to_read_from;
    fcs_offloading_queue_page_t pages[2];
} fcs_offloading_queue_t;

static GCC_INLINE void fcs_offloading_queue__init(
    fcs_offloading_queue_t * queue,
    int num_items_per_page,
    const char * offload_dir_path
    )
{
    queue->num_items_per_page = num_items_per_page;
    queue->offload_dir_path = offload_dir_path;
    queue->num_inserted = queue->num_items_in_queue = queue->num_extracted = 0;

    fcs_offloading_queue_page__init(&(queue->pages[0]), num_items_per_page, 0);
    fcs_offloading_queue_page__init(&(queue->pages[1]), num_items_per_page, 0);

    queue->page_to_read_from = queue->page_to_write_to = &(queue->pages[0]);
    queue->page_for_backup = &(queue->pages[1]);
}

static GCC_INLINE void fcs_offloading_queue__destroy(
    fcs_offloading_queue_t * queue
    )
{
    fcs_offloading_queue_page__destroy(&(queue->pages[0]));
    fcs_offloading_queue_page__destroy(&(queue->pages[1]));
}

static GCC_INLINE void fcs_offloading_queue__insert(
    fcs_offloading_queue_t * queue,
    const fcs_offloading_queue_item_t * item
)
{
    if (! fcs_offloading_queue_page__can_insert(queue->page_to_write_to))
    {
        if (queue->page_to_read_from->page_index != queue->page_to_write_to->page_index)
        {
            fcs_offloading_queue_page__offload(
                queue->page_to_write_to,
                queue->offload_dir_path
            );
            fcs_offloading_queue_page__bump(queue->page_to_write_to);
        }
        else
        {
            queue->page_to_write_to = queue->page_for_backup;
            fcs_offloading_queue_page__start_after(
                queue->page_to_write_to,
                queue->page_to_read_from
            );
            queue->page_for_backup = NULL;
        }
    }

    fcs_offloading_queue_page__insert(
        queue->page_to_write_to,
        item
    );

    queue->num_inserted++;
    queue->num_items_in_queue++;
}

static GCC_INLINE fcs_bool_t fcs_offloading_queue__extract(
    fcs_offloading_queue_t * queue,
    fcs_offloading_queue_item_t * return_item
)
{
    if (queue->num_items_in_queue == 0)
    {
        return FALSE;
    }

    if (! fcs_offloading_queue_page__can_extract(queue->page_to_read_from))
    {
        /* Cannot really happen due to the num_items_in_queue check.
         *
         * if (queue->_page_to_read_from->page_index ==
         *     queue->_page_to_write_to->page_index)
        */
        if (queue->page_to_read_from->page_index + 1 == queue->page_to_write_to->page_index)
        {
            queue->page_for_backup = queue->page_to_read_from;
            queue->page_to_read_from = queue->page_to_write_to;
        }
        else
        {
            fcs_offloading_queue_page__read_next_from_disk(
                queue->page_to_read_from,
                queue->offload_dir_path
            );
        }
    }

    queue->num_items_in_queue--;
    queue->num_extracted++;

    fcs_offloading_queue_page__extract(
        queue->page_to_read_from,
        return_item
    );

    return TRUE;
}

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__OFFLOADING_QUEUE_H */
