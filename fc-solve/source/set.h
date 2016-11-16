/*
 * Copyright © 2009 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *
 */

#ifndef SET_H
#define SET_H

#include <inttypes.h>
#include <stdbool.h>

#include "inline.h"

struct set_entry
{
    uint32_t hash;
    void *key;
};

struct set
{
    struct set_entry *table;
    uint32_t (*hash_function)(void *key);
    int (*key_equals_function)(const void *a, const void *b);
    uint32_t size;
    uint32_t rehash;
    uint32_t max_entries;
    uint32_t size_index;
    uint32_t entries;
    uint32_t deleted_entries;
};

void set_create(struct set *set, uint32_t (*hash_function)(void *key),
    int (*key_equals_function)(const void *a, const void *b));
void set_destroy(
    struct set *set, void (*delete_function)(struct set_entry *entry));

struct set_entry *set_add(struct set *set, void *key);

bool set_contains(struct set *set, void *key);

void set_remove(struct set *set, void *key);

struct set_entry *set_search(struct set *set, void *key);

void set_remove_entry(struct set *set, struct set_entry *entry);

struct set_entry *set_next_entry(struct set *set, struct set_entry *entry);

struct set_entry *set_random_entry(
    struct set *set, int (*predicate)(struct set_entry *entry));

/* Alternate interfaces to reduce repeated calls to hash function. */
struct set_entry *set_search_pre_hashed(
    struct set *set, uint32_t hash, void *key);

void set_rehash(struct set *set, int new_size_index);
extern void *deleted_key;

static GCC_INLINE int entry_is_free(const struct set_entry *entry)
{
    return entry->key == NULL;
}

static GCC_INLINE int entry_is_deleted(const struct set_entry *entry)
{
    return entry->key == deleted_key;
}

static GCC_INLINE int entry_is_present(const struct set_entry *entry)
{
    return entry->key != NULL && entry->key != deleted_key;
}

/**
 * Inserts the key with the given hash into the set.
 *
 * Note that insertion may rearrange the set on a resize or rehash, so
 * previously found set_entry pointers are no longer valid after this
 * function.
 */
static GCC_INLINE struct set_entry *set_add_pre_hashed(
    struct set *const set, const uint32_t hash, void *const key)
{
    uint32_t hash_address;
    struct set_entry *available_entry = NULL;

    if (set->entries >= set->max_entries)
    {
        set_rehash(set, set->size_index + 1);
    }
    else if (set->deleted_entries + set->entries >= set->max_entries)
    {
        set_rehash(set, set->size_index);
    }

    hash_address = hash % set->size;
    do
    {
        struct set_entry *entry = set->table + hash_address;
        uint32_t double_hash;

        if (!entry_is_present(entry))
        {
            /* Stash the first available entry we find */
            if (available_entry == NULL)
                available_entry = entry;
            if (entry_is_free(entry))
                break;
        }

        /* Implement replacement when another insert happens
         * with a matching key.  This is a relatively common
         * feature of hash tables, with the alternative
         * generally being "insert the new value as well, and
         * return it first when the key is searched for".
         *
         * Note that the set doesn't have a delete callback.
         * If freeing of old keys is required to avoid memory leaks,
         * perform a search before inserting.
         */
        if (!entry_is_deleted(entry) && entry->hash == hash &&
            set->key_equals_function(key, entry->key))
        {
            return entry;
        }

        double_hash = 1 + hash % set->rehash;

        hash_address = (hash_address + double_hash) % set->size;
    } while (hash_address != hash % set->size);

    if (available_entry)
    {
        if (entry_is_deleted(available_entry))
            set->deleted_entries--;
        available_entry->hash = hash;
        available_entry->key = key;
        set->entries++;
        return NULL;
    }

    /* We could hit here if a required resize failed. An unchecked-malloc
     * application could ignore this result.
     */
    return NULL;
}

#endif
