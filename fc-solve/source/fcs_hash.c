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
 * fcs_hash.c - an implementation of a simplistic (keys only) hash. This
 * hash uses chaining and re-hashing and was found to be very fast. Not all
 * of the functions of the hash ADT are implemented, but it is useful enough
 * for Freecell Solver.
 *
 */

#define BUILDING_DLL 1
#include "config.h"

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) || (defined(INDIRECT_STACK_STATES) && (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH))

#include <stdlib.h>
#include <string.h>

#include "fcs_hash.h"

#include "alloc.h"

#include "inline.h"

#include "state.h"

#ifdef FCS_RCS_STATES
#include "instance.h"
#endif
static void GCC_INLINE fc_solve_hash_rehash(fc_solve_hash_t * hash);



void fc_solve_hash_init(
    fc_solve_hash_t * hash,
#ifdef FCS_INLINED_HASH_COMPARISON
    enum FCS_INLINED_HASH_DATA_TYPE hash_type
#else
#ifdef FCS_WITH_CONTEXT_VARIABLE
    int (*compare_function)(const void * key1, const void * key2, void * context),
    void * context
#else
    int (*compare_function)(const void * key1, const void * key2)
#endif
#endif
    )
{
#define HASH_WANTED_SIZE 2048

    hash->size = HASH_WANTED_SIZE;
    hash->size_bitmask = HASH_WANTED_SIZE-1;
    hash->max_num_elems_before_resize = (HASH_WANTED_SIZE << 1);

    hash->num_elems = 0;

    /* Allocate a table of size entries */
    hash->entries = (fc_solve_hash_symlink_t *)malloc(
        sizeof(fc_solve_hash_symlink_t) * HASH_WANTED_SIZE
        );

    hash->list_of_vacant_items = NULL;

#ifdef FCS_INLINED_HASH_COMPARISON
    hash->hash_type = hash_type;
#else
    hash->compare_function = compare_function;
#ifdef FCS_WITH_CONTEXT_VARIABLE
    hash->context = context;
#endif
#endif

    /* Initialize all the cells of the hash table to NULL, which indicate
       that the cork of the linked list is right at the start */
    memset(hash->entries, 0, sizeof(fc_solve_hash_symlink_t)*HASH_WANTED_SIZE);

    fc_solve_compact_allocator_init(&(hash->allocator));

    return;
}

fcs_bool_t fc_solve_hash_insert(
    fc_solve_hash_t * hash,
    void * key,
#ifdef FCS_RCS_STATES
    void * key_id,
#endif
    void * * existing_key,
    fc_solve_hash_value_t hash_value
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
    , fc_solve_hash_value_t secondary_hash_value
#endif
    )
{
    fc_solve_hash_symlink_t * list;
    fc_solve_hash_symlink_item_t * item, * last_item;
    fc_solve_hash_symlink_item_t * * item_placeholder;
#ifdef FCS_INLINED_HASH_COMPARISON
    enum FCS_INLINED_HASH_DATA_TYPE hash_type;
#endif

#ifdef FCS_INLINED_HASH_COMPARISON
    hash_type = hash->hash_type;
#endif
    /* Get the index of the appropriate chain in the hash table */
#define PLACE() (hash_value & (hash->size_bitmask))

    list = (hash->entries + PLACE());

#undef PLACE

    /* If first_item is non-existent */
    if (list->first_item == NULL)
    {
        /* Allocate a first item with that key/val pair */
        item_placeholder = &(list->first_item);
    }
    else
    {
        /* Initialize item to the chain's first_item */
        item = list->first_item;
        last_item = NULL;

#ifdef FCS_WITH_CONTEXT_VARIABLE
#define MY_HASH_CONTEXT_VAR    , hash->context
#else
#define MY_HASH_CONTEXT_VAR
#endif

/*
 * MY_HASH_COMPARE_PROTO() returns -1/0/+1 depending on the compared
 * states order. We need to negate it for the desired condition of equality.
 * */
#define MY_HASH_COMPARE() (! MY_HASH_COMPARE_PROTO())

/* Define MY_HASH_COMPARE_PROTO() */
#if defined(FCS_RCS_STATES)

#define MY_HASH_COMPARE_PROTO() (fc_solve_state_compare(key_id, fc_solve_lookup_state_key_from_val(hash->instance, item->key)))

#elif !defined(INDIRECT_STACK_STATES)

#define MY_HASH_COMPARE_PROTO() (fc_solve_state_compare(item->key, key))

#elif defined(FCS_INLINED_HASH_COMPARISON)

#define MY_HASH_COMPARE_PROTO() \
        ((hash_type == FCS_INLINED_HASH__COLUMNS) \
            ? fc_solve_stack_compare_for_comparison(item->key, key) \
            : fc_solve_state_compare(item->key, key) \
        )

#else

#define MY_HASH_COMPARE_PROTO() (hash->compare_function(item->key, key MY_HASH_CONTEXT_VAR))

#endif
/* End of MY_HASH_COMPARE_PROTO() */

        while (item != NULL)
        {
            /*
                We first compare the hash values, because it is faster than
                comparing the entire data structure.
            */
            if (
                (item->hash_value == hash_value)
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
                && (item->secondary_hash_value == secondary_hash_value)
#endif
                && MY_HASH_COMPARE()
               )
            {
                *existing_key = item->key;

                return TRUE;
            }
            /* Cache the item before the current in last_item */
            last_item = item;
            /* Move to the next item */
            item = item->next;
        }

        item_placeholder = &(last_item->next);
    }

    if (hash->list_of_vacant_items)
    {
        hash->list_of_vacant_items = (item = hash->list_of_vacant_items)->next;
    }
    else
    {
        item = fcs_compact_alloc_ptr(&(hash->allocator), sizeof(*item));
    }

    *(item_placeholder) = item;

    /* Put the new element at the end of the list */
    /* Do an in-order insertion. */
    item->key = key;
    item->hash_value = hash_value;
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
    item->secondary_hash_value = secondary_hash_value;
#endif
    item->next = NULL;

    if ((++hash->num_elems) > hash->max_num_elems_before_resize)
    {
        fc_solve_hash_rehash(hash);
    }

    *existing_key = NULL;

    return FALSE;
}

/*
    This function "rehashes" a hash. I.e: it increases the size of its
    hash table, allowing for smaller chains, and faster lookup.

  */
static void GCC_INLINE fc_solve_hash_rehash(
    fc_solve_hash_t * hash
    )
{
    int old_size, new_size, new_size_bitmask;
    int i;
    fc_solve_hash_symlink_item_t * item, * next_item;
    int place;
    fc_solve_hash_symlink_t * new_entries;

    old_size = hash->size;

    new_size = old_size << 1;
    new_size_bitmask = new_size - 1;

    new_entries = calloc(new_size, sizeof(fc_solve_hash_symlink_t));

    /* Copy the items to the new hash while not allocating them again */
    for(i=0;i<old_size;i++)
    {
        item = hash->entries[i].first_item;
        /* traverse the chain item by item */
        while(item != NULL)
        {
            /* The place in the new hash table */
            place = item->hash_value & new_size_bitmask;

            /* Store the next item in the linked list in a safe place,
               so we can retrieve it after the assignment */
            next_item = item->next;
            /* It is placed in front of the first element in the chain,
               so it should link to it */
            item->next = new_entries[place].first_item;

            /* Make it the first item in its chain */
            new_entries[place].first_item = item;

            /* Move to the next item this one. */
            item = next_item;
        }
    };

    /* Free the entries of the old hash */
    free(hash->entries);

    /* Copy the new hash to the old one */
    hash->entries = new_entries;
    hash->size = new_size;
    hash->size_bitmask = new_size_bitmask;
    hash->max_num_elems_before_resize = (new_size << 1);
}

#else

/* ANSI C doesn't allow empty compilation */
static void fc_solve_hash_c_dummy();

#endif /* (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) || defined(INDIRECT_STACK_STATES) */
