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
 * check_and_add_state.c - the various possible implementations of the function
 * fc_solve_check_and_add_state().
 *
 */

#define BUILDING_DLL 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "instance.h"

#include "move_stack_compact_alloc.h"

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) || (FCS_STACK_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH))
/*
    This function "rehashes" a hash. I.e: it increases the size of its
    hash table, allowing for smaller chains, and faster lookup.
  */
static GCC_INLINE void fc_solve_hash_rehash(
    fc_solve_hash_t * const hash
    )
{
    const int old_size = hash->size;

    const int new_size = old_size << 1;

    /* Check for overflow. */
    if (new_size < old_size)
    {
        hash->max_num_elems_before_resize = FCS_INT_LIMIT_MAX;
        return;
    }

    const int new_size_bitmask = new_size - 1;

    fc_solve_hash_symlink_t * const new_entries = calloc(new_size, sizeof(new_entries[0]));

    /* Copy the items to the new hash while not allocating them again */
    for (int i=0 ; i < old_size ; i++)
    {
        fc_solve_hash_symlink_item_t * item = hash->entries[i].first_item;
        /* traverse the chain item by item */
        while(item != NULL)
        {
            /* The place in the new hash table */
            const int place = item->hash_value & new_size_bitmask;

            /* Store the next item in the linked list in a safe place,
               so we can retrieve it after the assignment */
            fc_solve_hash_symlink_item_t * const next_item = item->next;
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
    fcs_hash_set_max_num_elems(hash, new_size);
}

/*
 * Returns NULL if the key is new and the key/val pair was inserted.
 * Returns the existing key if the key is not new (= a truthy pointer).
 */
static GCC_INLINE void * const fc_solve_hash_insert(
    fc_solve_hash_t * const hash,
    void * const key,
#ifdef FCS_RCS_STATES
    void * const key_id,
#endif
    const fc_solve_hash_value_t hash_value
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
    , const fc_solve_hash_value_t secondary_hash_value
#endif
    )
{
#if defined(FCS_INLINED_HASH_COMPARISON) && defined(INDIRECT_STACK_STATES)
    const_SLOT(hash_type, hash);
#endif
    /* Get the index of the appropriate chain in the hash table */
#define PLACE() (hash_value & (hash->size_bitmask))

    typeof(hash->entries[0]) * const list = (hash->entries + PLACE());

#undef PLACE

    fc_solve_hash_symlink_item_t * * item_placeholder;
    /* If first_item is non-existent */
    if (list->first_item == NULL)
    {
        /* Allocate a first item with that key/val pair */
        item_placeholder = &(list->first_item);
    }
    else
    {
        /* Initialize item to the chain's first_item */
        fc_solve_hash_symlink_item_t * item = list->first_item;
        fc_solve_hash_symlink_item_t * last_item = NULL;

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
                return item->key;
            }
            /* Cache the item before the current in last_item */
            last_item = item;
            /* Move to the next item */
            item = item->next;
        }

        item_placeholder = &(last_item->next);
    }

#define ITEM_ALLOC() fcs_compact_alloc_ptr(&(hash->allocator), sizeof(*item))
#ifdef FCS_WITHOUT_TRIM_MAX_STORED_STATES
    fc_solve_hash_symlink_item_t * const item = ITEM_ALLOC();
#else
    fc_solve_hash_symlink_item_t * item;

    if ((item = hash->list_of_vacant_items))
    {
        hash->list_of_vacant_items = item->next;
    }
    else
    {
        item = ITEM_ALLOC();
    }
#endif

    *(*(item_placeholder) = item) =
    (typeof(*item)){
        .key = key,
        .hash_value = hash_value,
        .next = NULL,
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
        .secondary_hash_value = secondary_hash_value,
#endif
    };

    if ((++(hash->num_elems)) > hash->max_num_elems_before_resize)
    {
        fc_solve_hash_rehash(hash);
    }

    return NULL;
}

#endif

typedef  unsigned long  ul;
typedef  unsigned       char ub1;

static GCC_INLINE ul perl_hash_function(
    register const ub1 *s_ptr,        /* the key */
    register const ul length          /* the length of the key */
    )
{
    register ul hash_value_int = 0;
    register const ub1 * const s_end = s_ptr+length;

    while (s_ptr < s_end)
    {
        hash_value_int += (hash_value_int << 5) + *(s_ptr++);
    }
    hash_value_int += (hash_value_int>>5);

    return hash_value_int;
}

#ifdef INDIRECT_STACK_STATES

#define replace_with_cached(condition_expr) \
        if (condition_expr)     \
        {      \
            fcs_compact_alloc_release(stacks_allocator);    \
            *(current_stack) = cached_stack;       \
        }

/* TODO : Maybe define an accesor for new_state_key->stacks (also see the
 * replaced_with_cached macro above.
 * */
static GCC_INLINE void fc_solve_cache_stacks(
        fc_solve_hard_thread_t * const hard_thread,
        fcs_kv_state_t * const new_state
        )
{
#ifdef FCS_SINGLE_HARD_THREAD
#define instance hard_thread
#else
    fc_solve_instance_t * const instance = hard_thread->instance;
#endif

#ifndef HARD_CODED_NUM_STACKS
    SET_GAME_PARAMS();
#endif
    register fcs_state_t * const new_state_key = new_state->key;
    register fcs_state_extra_info_t * const new_state_info = new_state->val;

    fcs_compact_allocator_t * const stacks_allocator = &(HT_FIELD(hard_thread, allocator));

    fcs_cards_column_t * current_stack = new_state_key->stacks;

    for (int i=0 ; i < LOCAL_STACKS_NUM ; i++, current_stack++)
    {
        /*
         * If the stack is not a copy - it is already cached so skip
         * to the next stack
         * */
        if (! (new_state_info->stacks_copy_on_write_flags & (1 << i)))
        {
            continue;
        }

        fcs_cards_column_t column = fcs_state_get_col(*(new_state_key), i);
        const int col_len = (fcs_col_len(column)+1);

        char * const new_ptr = (char*)fcs_compact_alloc_ptr(stacks_allocator, col_len);
        memcpy(new_ptr, column, col_len);
        *(current_stack) = new_ptr;

        void * cached_stack;
#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
        /* Calculate the hash value for the stack */
        /* This hash function was ripped from the Perl source code.
         * (It is not derived work however). */
        fc_solve_hash_value_t hash_value_int = 0;
        {
            const char * s_ptr = (char*)(*(current_stack));
            const char * s_end = s_ptr+fcs_col_len(s_ptr)+1;
            while (s_ptr < s_end)
            {
                hash_value_int += (hash_value_int << 5) + *(s_ptr++);
            }
            hash_value_int += (hash_value_int >> 5);
        }

        if (hash_value_int < 0)
        {
            /*
             * This is a bit mask that nullifies the sign bit of the
             * number so it will always be positive
             * */
            hash_value_int &= (~(1<<((sizeof(hash_value_int)<<3)-1)));
        }
#endif

        {
            column = fcs_state_get_col(*(new_state_key), i);

            cached_stack = fc_solve_hash_insert(
                &(instance->stacks_hash),
                column,
                perl_hash_function(
                    (ub1 *)*(current_stack),
                    col_len
                )
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
                , hash_value_int
#endif
            );
            replace_with_cached(cached_stack);
        }

#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)
        {
            column = fcs_state_get_col(*new_state_key, i);

            replace_with_cached(
                fc_solve_columns_google_hash_insert(
                    instance->stacks_hash,
                    column,
                    &cached_stack
                )
            );
        }
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)

        cached_stack =
            fcs_libavl2_stacks_tree_insert(
                instance->stacks_tree,
                new_state_key->stacks[i]
            );

        replace_with_cached(cached_stack != NULL);

#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
        cached_stack = (void *)rbsearch(
            *(current_stack),
            instance->stacks_tree
            );

        replace_with_cached(cached_stack != *(current_stack));
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
        cached_stack = g_tree_lookup(
             instance->stacks_tree,
             (gpointer)*(current_stack)
             );

        /* replace_with_cached contains an if statement */
        replace_with_cached(cached_stack != NULL)
        else
        {
            g_tree_insert(
                instance->stacks_tree,
                (gpointer)*(current_stack),
                (gpointer)*(current_stack)
                );
        }
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
        cached_stack = g_hash_table_lookup(
            instance->stacks_hash,
            (gconstpointer)*(current_stack)
            );
        replace_with_cached(cached_stack != NULL)
        else
        {
            g_hash_table_insert(
                instance->stacks_hash,
                (gpointer)*(current_stack),
                (gpointer)*(current_stack)
                );
        }
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)
        PWord_t * PValue;
        column = fcs_state_get_col(*new_state_key, i);

        JHSI(
            PValue,
            instance->stacks_judy_array,
            column,
            (1+fcs_col_len(column))
        );
        /* later_todo : Handle out-of-memory. */
        if (*PValue == 0)
        {
            /*  A new stack */
            *PValue = (PWord_t)column;
        }
        else
        {
            cached_stack = (void *)(*PValue);
            replace_with_cached(1);
        }
#else
#error FCS_STACK_STORAGE is not set to a good value.
#endif
    }
#ifdef FCS_SINGLE_HARD_THREAD
#undef instance
#endif
}

#else /* #ifdef INDIRECT_STACK_STATES */

#define fc_solve_cache_stacks(hard_thread, new_state_key) \
    {}

#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
guint fc_solve_hash_function(gconstpointer key)
{
    const char * s_ptr = (char*)key;
    const char * const s_end = s_ptr+sizeof(fcs_state_t);
    guint hash_value = 0;
    while (s_ptr < s_end)
    {
        hash_value += (hash_value << 5) + *(s_ptr++);
    }
    hash_value += (hash_value >> 5);

    return hash_value;
}
#endif


/*
 * check_and_add_state() does the following things:
 *
 * 1. Check if the number of iterations exceeded its maximum, and if so
 *    return FCS_STATE_EXCEEDS_MAX_NUM_TIMES in order to terminate the
 *    solving process.
 * 2. Check if the maximal depth was reached and if so return
 *    FCS_STATE_EXCEEDS_MAX_DEPTH
 * 3. Canonize the state.
 * 4. Check if the state is already found in the collection of the states
 *    that were already checked.
 *    If it is:
 *
 *        5a. Return FALSE.
 *
 *    If it isn't:
 *
 *        5b. Add the new state and return TRUE.
 * */

static GCC_INLINE void upon_new_state(
    fc_solve_instance_t * const instance,
    fc_solve_hard_thread_t * const hard_thread,
    fcs_state_extra_info_t * const new_state_info
)
{
    fcs_collectible_state_t * const parent_state = new_state_info->parent;
    /* The new state was not found in the cache, and it was already inserted */
    if (likely(parent_state))
    {
        (FCS_S_NUM_ACTIVE_CHILDREN(parent_state))++;
        /* If parent_val is defined, so is moves_to_parent */
        new_state_info->moves_to_parent =
            fc_solve_move_stack_compact_allocate(
                hard_thread,
                new_state_info->moves_to_parent
            );
    }

    instance->active_num_states_in_collection++;
    instance->num_states_in_collection++;

    return;
}

fcs_bool_t fc_solve_check_and_add_state(
    fc_solve_hard_thread_t * const hard_thread,
    fcs_kv_state_t * const new_state,
    fcs_kv_state_t * const existing_state_raw
    )
{
/*
 * TODO : these accessor macros are probably out-of-date and won't work with
 * some of the less commonly tested data storage backends. They should be
 * tested and updated.
 * */
#define existing_state_val (existing_state_raw->val)
#define new_state_key      (new_state->key)

#define ON_STATE_NEW() upon_new_state(instance, hard_thread, new_state->val);

#ifdef FCS_SINGLE_HARD_THREAD
#define instance hard_thread
#else
    fc_solve_instance_t * const instance = hard_thread->instance;
#endif

    /* #if'ing out because it doesn't belong here. */
#if 0
    if ((instance->max_depth >= 0) &&
        (new_state_val->depth >= instance->max_depth))
    {
        return FCS_STATE_EXCEEDS_MAX_DEPTH;
    }
#endif

    fc_solve_cache_stacks(hard_thread, new_state);

    {
        fc_solve_canonize_state(
            new_state_key
            PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
            PASS_STACKS(INSTANCE_STACKS_NUM)
            );

    }

/*
    The objective of this part of the code is:
    1. To check if new_state_key / new_state_val is already in the prev_states
       collection.
    2. If not, to add it and to set check to true.
    3. If so, to set check to false.
  */

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
    fc_solve_hash_value_t hash_value_int = 0;
    {
        const char * s_ptr = (char*)new_state_key;
        const char * s_end = s_ptr+sizeof(*new_state_key);
        while (s_ptr < s_end)
        {
            hash_value_int += (hash_value_int << 5) + *(s_ptr++);
        }
        hash_value_int += (hash_value_int>>5);
    }
    if (hash_value_int < 0)
    {
        /*
         * This is a bit mask that nullifies the sign bit of the
         * number so it will always be positive
         * */
        hash_value_int &= (~(1<<((sizeof(hash_value_int)<<3)-1)));
    }
#endif
    {
        void * const existing_void =
        fc_solve_hash_insert(
        &(instance->hash),
#ifdef FCS_RCS_STATES
        new_state->val,
        new_state->key,
#else
        FCS_STATE_kv_to_collectible(new_state),
#endif
        perl_hash_function(
            (ub1 *)(new_state_key),
            sizeof(*(new_state_key))
            )
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
        , hash_value_int
#endif
        );
        if (existing_void)
        {
            FCS_STATE_collectible_to_kv(existing_state_raw, existing_void);
            return FALSE;
        }
        else
        {
            ON_STATE_NEW();
            return TRUE;
        }
    }
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)
    {
        void * existing_void;

        /*  TODO : check if this condition should be negated. */
        if (fc_solve_states_google_hash_insert(
            instance->hash,
            new_state,
            &(existing_void)
            )
        )
        {
            existing_state_val = existing_void;
            return FALSE;
        }
        else
        {
            ON_STATE_NEW();
            return TRUE;
        }
    }
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    existing_state_val = (fcs_state_extra_info_t *)rbsearch(new_state_val,
            instance->tree
            );
    is_state_new = ((*existing_state_val) == new_state_val);

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE)
#ifdef FCS_RCS_STATES
    instance->tree_new_state_key = new_state->key;
    instance->tree_new_state = new_state->val;
#endif

    {
        void * existing_void;
    if ((existing_void =
        fc_solve_kaz_tree_alloc_insert(instance->tree,
#ifdef FCS_RCS_STATES
                                       new_state_val
#else
                                       FCS_STATE_kv_to_collectible(new_state)
#endif
                                       ))
            == NULL
       )
    {
        ON_STATE_NEW();
        return TRUE;
    }
    else
    {
        FCS_STATE_collectible_to_kv(existing_state_raw, existing_void);
        return FALSE;
    }
    }
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE)
#ifdef FCS_RCS_STATES
    instance->tree_new_state_key = new_state->key;
    instance->tree_new_state = new_state->val;
#endif

    {
        void * existing_void;
    if ((existing_void =
        fcs_libavl2_states_tree_insert(instance->tree, FCS_STATE_kv_to_collectible(new_state)))
            == NULL
       )
    {
        ON_STATE_NEW();
        return TRUE;
    }
    else
    {
        FCS_STATE_collectible_to_kv(existing_state_raw, existing_void);
        return FALSE;
    }
    }
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    existing_state_val = g_tree_lookup(instance->tree, (gpointer)new_state_key);
    if ((is_state_new = (existing_state_val == NULL)))
    {
        /* The new state was not found. Let's insert it.
         * The value must be the same as the key, so g_tree_lookup()
         * will return it. */
        g_tree_insert(
            instance->tree,
            (gpointer)new_state_key,
            (gpointer)new_state_val
            );
    }

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    existing_state_val = g_hash_table_lookup(instance->hash,
            (gpointer)new_state_key);
    if ((is_state_new = (existing_state_val == NULL)))
    {
        /* The new state was not found. Let's insert it.
         * The value must be the same as the key, so g_tree_lookup()
         * will return it. */
        g_hash_table_insert(
            instance->hash,
            (gpointer)new_state_key,
            (gpointer)new_state_val

            );
    }

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    {
        DBT key, value;
        key.data = new_state;
        key.size = sizeof(*new_state);
        if ((is_state_new = (instance->db->get(
            instance->db,
            NULL,
            &key,
            &value,
            0
            ) == 0)))
        {
            /* The new state was not found. Let's insert it.
             * The value must be the same as the key, so g_tree_lookup()
             * will return it. */

            value.data = key.data;
            value.size = key.size;
            instance->db->put(
                instance->db,
                NULL,
                &key,
                &value,
                0);
        }
    }
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY)
    {
        PWord_t * PValue;

        JHSI(PValue, instance->judy_array, new_state_key, sizeof(*new_state_key));

        /* later_todo : Handle out-of-memory. */
        if (*PValue == 0)
        {
            /* A new state. */
            *PValue = (PWord_t)(FCS_STATE_kv_to_collectible(new_state));
            ON_STATE_NEW();
            return TRUE;
        }
        else
        {
            /* Already exists. */
            FCS_STATE_collectible_to_kv(existing_state_raw, (fcs_collectible_state_t *)(*PValue));
            return FALSE;
        }
    }
#else
#error Unknown FCS_STATE_STORAGE. Please define it to a valid value.
#endif
}
#ifdef FCS_SINGLE_HARD_THREAD
#undef instance
#endif
