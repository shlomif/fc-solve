// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// fcs_hash__insert.h - header file of Freecell Solver's internal hash
// implementation.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "move_stack_compact_alloc.h"

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) ||                 \
     (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH))
// Increase the size, for smaller chains, and faster lookup.
static inline void fc_solve_hash_rehash(hash_table *const hash)
{
    const_AUTO(old_size, hash->size);

    const_AUTO(new_size, old_size << 1);

    // Check for overflow.
    if (new_size < old_size)
    {
        hash->max_num_elems_before_resize = SIZE_MAX;
        return;
    }

    const size_t new_size_bitmask = new_size - 1;

    const_SLOT(entries, hash);
    hash_table_entry *const new_entries =
        calloc(new_size, sizeof(new_entries[0]));

    // Copy the items to the new hash while not allocating them again
    for (size_t i = 0; i < old_size; ++i)
    {
        hash_item *item = entries[i].first_item;
        while (item != NULL)
        {
            const size_t place = item->hash_value & new_size_bitmask;
            hash_item *const next_item = item->next;
            // It is placed before the first element in the chain,
            // so it should link to it
            item->next = new_entries[place].first_item;
            new_entries[place].first_item = item;
            item = next_item;
        }
    };

    free(hash->entries);

    // Copy the new hash to the old one
    hash->entries = new_entries;
    hash->size = new_size;
    hash->size_bitmask = new_size_bitmask;
    fcs_hash_set_max_num_elems(hash, new_size);
}

// Returns NULL if the key is new and the key/val pair was inserted.
// Returns the existing key if the key is not new (= a truthy pointer).
static inline void *fc_solve_hash_insert(hash_table *const hash,
    void *const key,
#ifdef FCS_RCS_STATES
    void *const key_id,
#endif
    const fcs_hash_value hash_value)
{
#if defined(FCS_INLINED_HASH_COMPARISON) && defined(INDIRECT_STACK_STATES)
    const_SLOT(hash_type, hash);
#endif
    typeof(hash->entries[0]) *const list =
        (hash->entries + (hash_value & (hash->size_bitmask)));
    hash_item **item_placeholder;
    if (!list->first_item)
    {
        item_placeholder = &(list->first_item);
    }
    else
    {
        hash_item *item = list->first_item;
        hash_item *last_valid_item = NULL;

// MY_HASH_COMPARE_PROTO() returns -1/0/+1 depending on the compared
// states order. We need to negate it for the desired condition of equality.
#define MY_HASH_COMPARE() (!MY_HASH_COMPARE_PROTO())

// Define MY_HASH_COMPARE_PROTO()
#ifdef FCS_RCS_STATES

#define MY_HASH_COMPARE_PROTO()                                                \
    (fc_solve_state_compare(key_id,                                            \
        fc_solve_lookup_state_key_from_val(hash->instance, item->key)))

#elif !defined(INDIRECT_STACK_STATES)

#define MY_HASH_COMPARE_PROTO() (fc_solve_state_compare(item->key, key))

#elif defined(FCS_INLINED_HASH_COMPARISON)

#define MY_HASH_COMPARE_PROTO()                                                \
    ((hash_type == FCS_INLINED_HASH__COLUMNS)                                  \
            ? fc_solve_stack_compare_for_comparison(item->key, key)            \
            : fc_solve_state_compare(item->key, key))

#else

#ifdef FCS_WITH_CONTEXT_VARIABLE
#define MY_HASH_CONTEXT_VAR , hash->context
#else
#define MY_HASH_CONTEXT_VAR
#endif

#define MY_HASH_COMPARE_PROTO()                                                \
    (hash->compare_function(item->key, key MY_HASH_CONTEXT_VAR))

#endif
        // End of MY_HASH_COMPARE_PROTO()

        while (item != NULL)
        {
            // We first compare the hash values, because it is faster than
            // comparing the entire data structure.
            if ((item->hash_value == hash_value) && MY_HASH_COMPARE())
            {
                return item->key;
            }
            last_valid_item = item;
            item = item->next;
        }

        item_placeholder = &(last_valid_item->next);
    }

#define ITEM_ALLOC() fcs_compact_alloc_ptr(&(hash->allocator), sizeof(*item))
#ifdef FCS_WITHOUT_TRIM_MAX_STORED_STATES
    hash_item *const item = ITEM_ALLOC();
#else
    hash_item *item;

    if ((item = hash->list_of_vacant_items))
    {
        hash->list_of_vacant_items = item->next;
    }
    else
    {
        item = ITEM_ALLOC();
    }
#endif

    *(*(item_placeholder) = item) = (typeof(*item)){
        .key = key,
        .hash_value = hash_value,
        .next = NULL,
    };

    if ((++(hash->num_elems)) > hash->max_num_elems_before_resize)
    {
        fc_solve_hash_rehash(hash);
    }

    return NULL;
}

#endif
#ifdef __cplusplus
}
#endif
