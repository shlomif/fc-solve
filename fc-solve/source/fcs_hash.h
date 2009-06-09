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
 * fcs_hash.h - header file of Freecell Solver's internal hash implementation.
 *
 */

#ifndef FC_SOLVE__FCS_HASH_H
#define FC_SOLVE__FCS_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "alloc.h"

typedef int SFO_hash_value_t;

struct SFO_hash_symlink_item_struct
{
    /* A pointer to the data structure that is to be collected */
    void * key;
    /* A pointer to the value associated with the key */
    void * val;
    /* We also store the hash value corresponding to this key for faster
       comparisons */
    SFO_hash_value_t hash_value;
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
    /*
     * We also store a secondary hash value, which is not used for indexing,
     * but is used to speed up comparison.
     * */
    SFO_hash_value_t secondary_hash_value;
#endif
    /* The next item in the list */
    struct SFO_hash_symlink_item_struct * next;
};

typedef struct SFO_hash_symlink_item_struct SFO_hash_symlink_item_t;

struct SFO_hash_symlink_struct
{
    SFO_hash_symlink_item_t * first_item;
};

typedef struct SFO_hash_symlink_struct SFO_hash_symlink_t;

typedef struct
{
    /* The vector of the hash table itself */
    SFO_hash_symlink_t * entries;
    /* A comparison function that can be used for comparing two keys
       in the collection */
    int (*compare_function)(const void * key1, const void * key2, void * context);
    /* The size of the hash table */
    int size;

    /* A bit mask that extract the lowest bits out of the hash value */
    int size_bitmask;
    /* The number of elements stored inside the hash */
    int num_elems;
    /* A context to pass to the comparison function */
    void * context;

    fcs_compact_allocator_t * allocator;
} SFO_hash_t;

SFO_hash_t * fc_solve_hash_init(
    SFO_hash_value_t wanted_size,
    int (*compare_function)(const void * key1, const void * key2, void * context),
    void * context
    );

/*
 * Returns 0 if the key is new and the key/val pair was inserted.
 *      - in that case *existing_key / *existing_val will be set to key
 *      and val respectively.
 * Returns 1 if the key is not new and *existing_key / *existing_val
 * was set to it.
 */
int fc_solve_hash_insert(
    SFO_hash_t * hash,
    void * key,
    void * val,
    void * * existing_key,
    void * * existing_val,
    SFO_hash_value_t hash_value
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
    , SFO_hash_value_t secondary_hash_value
#endif
    );


void fc_solve_hash_free(
    SFO_hash_t * hash
    );

void fc_solve_hash_free_with_callback(
    SFO_hash_t * hash,
    void (*function_ptr)(void * key, void * context)
    );


#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__FCS_HASH_H */




