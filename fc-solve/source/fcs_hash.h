/*
 * fcs_hash.h - header file of Freecell Solver's internal hash implementation.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#ifndef __FCS_HASH_H
#define __FCS_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "alloc.h"

typedef int SFO_hash_value_t;

struct SFO_hash_symlink_item_struct
{
    /* A pointer to the data structure that is to be collected */
    void * key;
    /* We also store the hash value corresponding to this key for faster
       comparisons */
    SFO_hash_value_t hash_value;
    /* The next item in the list */
    struct SFO_hash_symlink_item_struct * next;
};

typedef struct SFO_hash_symlink_item_struct SFO_hash_symlink_item_t;

struct SFO_hash_symlink_struct
{
    SFO_hash_symlink_item_t * first_item;
};

typedef struct SFO_hash_symlink_struct SFO_hash_symlink_t;

struct SFO_hash_struct
{
    /* The vector of the hash table itself */
    SFO_hash_symlink_t * entries;
    /* A comparison function that can be used for comparing two keys
       in the collection */
    int (*compare_function)(const void * key1, const void * key2, void * context);
    /* The size of the hash table */
    int size;
    /* The number of elements stored inside the hash */
    int num_elems;
    /* A context to pass to the comparison function */
    void * context;

    fcs_compact_allocator_t * allocator;
};

typedef struct SFO_hash_struct SFO_hash_t;


SFO_hash_t * freecell_solver_hash_init(
    SFO_hash_value_t wanted_size,
    int (*compare_function)(const void * key1, const void * key2, void * context),
    void * context
    );

void * freecell_solver_hash_insert(
    SFO_hash_t * hash,
    void * key,
    SFO_hash_value_t hash_value,
    int optimize_for_caching
    );


void freecell_solver_hash_free(
    SFO_hash_t * hash
    );

void freecell_solver_hash_free_with_callback(
    SFO_hash_t * hash,
    void (*function_ptr)(void * key, void * context)
    );


#ifdef __cplusplus
}
#endif

#endif /* __FCS_HASH_H */




