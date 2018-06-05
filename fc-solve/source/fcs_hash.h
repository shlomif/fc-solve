/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * fcs_hash.h - header file of Freecell Solver's internal hash implementation.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "meta_alloc.h"
#include "rinutils.h"

#ifdef FCS_INLINED_HASH_COMPARISON
enum FCS_INLINED_HASH_DATA_TYPE
{
    FCS_INLINED_HASH__COLUMNS,
    FCS_INLINED_HASH__STATES,
};
#endif

typedef size_t fcs_hash_value;

struct fc_solve_hash_symlink_item_struct
{
    /* A pointer to the data structure that is to be collected */
    void *key;
    /* We also store the hash value corresponding to this key for faster
       comparisons */
    fcs_hash_value hash_value;
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
    /*
     * We also store a secondary hash value, which is not used for indexing,
     * but is used to speed up comparison.
     * */
    fcs_hash_value secondary_hash_value;
#endif
    /* The next item in the list */
    struct fc_solve_hash_symlink_item_struct *next;
};

typedef struct fc_solve_hash_symlink_item_struct hash_item;

typedef struct
{
    hash_item *first_item;
} hash_table_entry;

struct fc_solve_instance_struct;

#ifndef FCS_INLINED_HASH_COMPARISON
#ifdef FCS_WITH_CONTEXT_VARIABLE
typedef int (*hash_compare_function)(
    const void *const, const void *const, void *context);
#else
typedef int (*hash_compare_function)(const void *const, const void *const);
#endif
#endif

typedef struct
{
    /* The vector of the hash table itself */
    hash_table_entry *entries;
#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
    /* The list of vacant items as freed by the garbage collector. Use
     * if before allocating more. */
    hash_item *list_of_vacant_items;
#endif
/* A comparison function that can be used for comparing two keys
   in the collection */
#ifdef FCS_INLINED_HASH_COMPARISON
    enum FCS_INLINED_HASH_DATA_TYPE hash_type;
#else
    hash_compare_function compare_function;
#ifdef FCS_WITH_CONTEXT_VARIABLE
    /* A context to pass to the comparison function */
    void *context;
#else
#endif
#endif

    /* The size of the hash table */
    int size;

    /* A bit mask that extract the lowest bits out of the hash value */
    int size_bitmask;
    /* The number of elements stored inside the hash */
    fcs_int_limit_t num_elems;

    fcs_int_limit_t max_num_elems_before_resize;

    compact_allocator allocator;
#ifdef FCS_RCS_STATES
    struct fc_solve_instance_struct *instance;
#endif
} hash_table;

static inline void fcs_hash_set_max_num_elems(
    hash_table *const hash, const fcs_int_limit_t new_size)
{
    hash->max_num_elems_before_resize = (new_size << 1);
}

static inline void fc_solve_hash_init(
    meta_allocator *const meta_alloc, hash_table *const hash,
#ifdef FCS_INLINED_HASH_COMPARISON
    const enum FCS_INLINED_HASH_DATA_TYPE hash_type
#else
    hash_compare_function compare_function
#ifdef FCS_WITH_CONTEXT_VARIABLE
    ,
    void *const context
#else
#endif
#endif
)
{
    const typeof(hash->size) initial_hash_size = 2048;

    hash->size = initial_hash_size;
    hash->size_bitmask = initial_hash_size - 1;
    fcs_hash_set_max_num_elems(hash, initial_hash_size);

    hash->num_elems = 0;

    /* Allocate a table of size entries */
    /* Initialize all the cells of the hash table to NULL, which indicate
       that the end of each chain is right at the start. */
    hash->entries =
        (hash_table_entry *)calloc(initial_hash_size, sizeof(hash->entries[0]));

#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
    hash->list_of_vacant_items = NULL;
#endif

#ifdef FCS_INLINED_HASH_COMPARISON
    hash->hash_type = hash_type;
#else
    hash->compare_function = compare_function;
#ifdef FCS_WITH_CONTEXT_VARIABLE
    hash->context = context;
#endif
#endif

    fc_solve_compact_allocator_init(&(hash->allocator), meta_alloc);
}

static inline void fc_solve_hash_free(hash_table *const hash)
{
    fc_solve_compact_allocator_finish(&(hash->allocator));
    free(hash->entries);
    hash->entries = NULL;
}

#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
static inline void fc_solve_hash_foreach(hash_table *const hash,
    bool (*should_delete_ptr)(void *const key, void *const context),
    void *const context)
{
    const_SLOT(size, hash);
    var_AUTO(entries, hash->entries);
    for (int i = 0; i < size; i++)
    {
        hash_item **item = &(entries[i].first_item);
        while ((*item) != NULL)
        {
            if (should_delete_ptr((*item)->key, context))
            {
                hash_item *const next_item = (*item)->next;
                /* Garbage collect (*item). */
                (*item)->next = hash->list_of_vacant_items;
                hash->list_of_vacant_items = (*item);
                /* Skip the item in the chain. */
                (*item) = next_item;

                hash->num_elems--;
            }
            else
            {
                item = &((*item)->next);
            }
        }
    }
}
#endif

#ifdef __cplusplus
}
#endif
