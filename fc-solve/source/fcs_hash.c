/*
 * fcs_hash.c - an implementation of a simplistic (keys only) hash. This
 * hash uses chaining and re-hashing and was found to be very fast. Not all
 * of the functions of the hash ADT are implemented, but it is useful enough
 * for Freecell Solver.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#include "config.h"

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) || (defined(INDIRECT_STACK_STATES) && (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH))

#include <stdlib.h>
#include <string.h>

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

#include "fcs_hash.h"

#include "alloc.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif



#define NUM_PRIMES 136

/*
    A list of primes in the range between 256 and 2^31. There
    are roughly 6 in an x .. 2x decade.
  */
static const int primes_list[NUM_PRIMES+1] = {
    257,
    293,
    331,
    367,
    409,
    461,
    521,
    587,
    653,
    739,
    827,
    929,
    1049,
    1181,
    1321,
    1487,
    1693,
    1879,
    2113,
    2377,
    2677,
    3011,
    3389,
    3821,
    4283,
    4813,
    5417,
    6091,
    6857,
    7717,
    8677,
    9767,
    10973,
    12343,
    13901,
    15629,
    17573,
    19777,
    22247,
    25031,
    28151,
    31663,
    35671,
    40087,
    45083,
    50723,
    57059,
    64187,
    72211,
    81239,
    91393,
    102829,
    115663,
    130121,
    146389,
    164683,
    185291,
    208433,
    234499,
    263803,
    296767,
    333857,
    375593,
    422537,
    475367,
    534799,
    601631,
    676829,
    761429,
    856627,
    963689,
    1084133,
    1219649,
    1372109,
    1543631,
    1736599,
    1953659,
    2197847,
    2472577,
    2781677,
    3129383,
    3520519,
    3960587,
    4455667,
    5012627,
    5639191,
    6344087,
    7137101,
    8029243,
    9032887,
    10161997,
    11432249,
    12861281,
    14468933,
    16277561,
    18312263,
    20601271,
    23176429,
    26073497,
    29332687,
    32999269,
    37124167,
    41764741,
    46985261,
    52858427,
    59465723,
    66898963,
    75261311,
    84668971,
    95252603,
    107159153,
    120554059,
    135623317,
    152576233,
    171648317,
    193104269,
    217242323,
    244397591,
    274947317,
    309315703,
    347980163,
    391477727,
    440412409,
    495463937,
    557396929,
    627071569,
    705455519,
    793637437,
    892842107,
    1004447359,
    1130003291,
    1271253703,
    1430160409,
    1608930451,
    1810046779,
    2036302607,
    -1
};


static void SFO_hash_rehash(SFO_hash_t * hash);



SFO_hash_t * freecell_solver_hash_init(
    SFO_hash_value_t wanted_size,
    int (*compare_function)(const void * key1, const void * key2, void * context),
    void * context
    )
{
    int size,i;
    SFO_hash_t * hash;

    /* Find a prime number that is greater than the initial wanted size */
    for(i=0;i<NUM_PRIMES;i++)
    {
        if (primes_list[i] > wanted_size)
        {
            break;
        }
    }

    size = primes_list[i];

    hash = (SFO_hash_t *)malloc(sizeof(SFO_hash_t));

    hash->size = size;

    hash->num_elems = 0;

    /* Allocate a table of size entries */
    hash->entries = (SFO_hash_symlink_t *)malloc(
        sizeof(SFO_hash_symlink_t) * size
        );

    hash->compare_function = compare_function;
    hash->context = context;

    /* Initialize all the cells of the hash table to NULL, which indicate
       that the cork of the linked list is right at the start */
    memset(hash->entries, 0, sizeof(SFO_hash_symlink_t)*size);

    hash->allocator = freecell_solver_compact_allocator_new();

    return hash;
}

void * freecell_solver_hash_insert(
    SFO_hash_t * hash,
    void * key,
    SFO_hash_value_t hash_value,
    SFO_hash_value_t secondary_hash_value,
    int optimize_for_caching
    )
{
    int place;
    SFO_hash_symlink_t * list;
    SFO_hash_symlink_item_t * item, * last_item;

    /* Get the index of the appropriate chain in the hash table */
    place = hash_value % (hash->size);

    list = &(hash->entries[place]);
    /* If first_item is non-existent */
    if (list->first_item == NULL)
    {
        /* Allocate a first item with that key */
        fcs_compact_alloc_into_var(item, hash->allocator, SFO_hash_symlink_item_t);
        list->first_item = item;
        item->next = NULL;
        item->key = key;
        item->hash_value = hash_value;
        item->secondary_hash_value = secondary_hash_value;

        goto rehash_check;
    }

    /* Initialize item to the chain's first_item */
    item = list->first_item;
    last_item = NULL;

    while (item != NULL)
    {
        /*
            We first compare the hash values, because it is faster than
            comparing the entire data structure.

        */
        if (
            (item->hash_value == hash_value) &&
            (item->secondary_hash_value == secondary_hash_value) &&
            (!(hash->compare_function(item->key, key, hash->context)))
           )
        {
            if (optimize_for_caching)
            {
                /*
                 * Place the item in the beginning of the chain.
                 * If last_item == NULL it is already the first item so leave
                 * it alone
                 * */
                if (last_item != NULL)
                {
                    last_item->next = item->next;
                    item->next = list->first_item;
                    list->first_item = item;
                }
            }
            return item->key;
        }
        /* Cache the item before the current in last_item */
        last_item = item;
        /* Move to the next item */
        item = item->next;
    }

    if (optimize_for_caching)
    {
        /* Put the new element at the beginning of the list */
        fcs_compact_alloc_into_var(item, hash->allocator, SFO_hash_symlink_item_t);
        item->next = list->first_item;
        item->key = key;
        item->hash_value = hash_value;
        list->first_item = item;
        item->secondary_hash_value = secondary_hash_value;
    }
    else
    {
        /* Put the new element at the end of the list */
        fcs_compact_alloc_into_var(item, hash->allocator, SFO_hash_symlink_item_t);
        last_item->next = item;
        item->next = NULL;
        item->key = key;
        item->hash_value = hash_value;
        item->secondary_hash_value = secondary_hash_value;
    }

rehash_check:

    hash->num_elems++;

    if (hash->num_elems > ((hash->size*3)>>2))
    {
        SFO_hash_rehash(hash);
    }

    return NULL;
}

void freecell_solver_hash_free_with_callback(
    SFO_hash_t * hash,
    void (*function_ptr)(void * key, void * context)
    )
{
    int i;
    SFO_hash_symlink_item_t * item, * next_item;

    for(i=0;i<hash->size;i++)
    {
        item = hash->entries[i].first_item;
        while (item != NULL)
        {
            function_ptr(item->key, hash->context);
            next_item = item->next;

            item = next_item;
        }
    }

    freecell_solver_hash_free(hash);
}

void freecell_solver_hash_free(
    SFO_hash_t * hash
    )
{
    freecell_solver_compact_allocator_finish(hash->allocator);

    free(hash->entries);

    free(hash);
}


/*
    This function "rehashes" a hash. I.e: it increases the size of its
    hash table, allowing for smaller chains, and faster lookup.

  */
static void SFO_hash_rehash(
    SFO_hash_t * hash
    )
{
    int old_size, new_size;
    int i;
#if 0
    SFO_hash_t * new_hash;
#endif
    SFO_hash_symlink_item_t * item, * next_item;
    int place;
    SFO_hash_symlink_t * new_entries;

    old_size = hash->size;

#if 0
    /* Allocate a new hash with hash_init() */
    new_hash = freecell_solver_hash_init_proto(
        old_size * 2,
        hash->compare_function,
        hash->context
        );
#endif    

    old_size = hash->size;
    new_size = old_size * 2;

    /* Find a prime number that is greater than the initial wanted size */
    for(i=0;i<NUM_PRIMES;i++)
    {
        if (primes_list[i] > new_size)
        {
            break;
        }
    }

    new_size = primes_list[i];

    new_entries = calloc(new_size, sizeof(SFO_hash_symlink_t));

    /* Copy the items to the new hash while not allocating them again */
    for(i=0;i<old_size;i++)
    {
        item = hash->entries[i].first_item;
        /* traverse the chain item by item */
        while(item != NULL)
        {
            /* The place in the new hash table */
            place = item->hash_value % new_size;

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
#if 0
    *hash = *new_hash;
#endif
    hash->entries = new_entries;
    hash->size = new_size;
}

#else

/* ANSI C doesn't allow empty compilation */
static void freecell_solver_hash_c_dummy(); 

#endif /* (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) || defined(INDIRECT_STACK_STATES) */
