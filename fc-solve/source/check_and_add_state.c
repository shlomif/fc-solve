/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// check_and_add_state.c - the various possible implementations of the function
// fc_solve_check_and_add_state().

#include "move_stack_compact_alloc.h"

#if ((FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH) ||                 \
     (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH))
/*
    This function "rehashes" a hash. I.e: it increases the size of its
    hash table, allowing for smaller chains, and faster lookup.
  */
static inline void fc_solve_hash_rehash(hash_table *const hash)
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

    const_SLOT(entries, hash);
    hash_table_entry *const new_entries =
        calloc(new_size, sizeof(new_entries[0]));

    /* Copy the items to the new hash while not allocating them again */
    for (int i = 0; i < old_size; i++)
    {
        hash_item *item = entries[i].first_item;
        /* traverse the chain item by item */
        while (item != NULL)
        {
            /* The place in the new hash table */
            const int place = item->hash_value & new_size_bitmask;

            /* Store the next item in the linked list in a safe place,
               so we can retrieve it after the assignment */
            hash_item *const next_item = item->next;
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
static inline void *fc_solve_hash_insert(
    hash_table *const hash, void *const key,
#ifdef FCS_RCS_STATES
    void *const key_id,
#endif
    const fcs_hash_value hash_value
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
    ,
    const fcs_hash_value secondary_hash_value
#endif
)
{
#if defined(FCS_INLINED_HASH_COMPARISON) && defined(INDIRECT_STACK_STATES)
    const_SLOT(hash_type, hash);
#endif
    typeof(hash->entries[0]) *const list =
        (hash->entries + (hash_value & (hash->size_bitmask)));
    hash_item **item_placeholder;
    /* If first_item is non-existent */
    if (list->first_item == NULL)
    {
        /* Allocate a first item with that key/val pair */
        item_placeholder = &(list->first_item);
    }
    else
    {
        /* Initialize item to the chain's first_item */
        hash_item *item = list->first_item;
        hash_item *last_item = NULL;

// MY_HASH_COMPARE_PROTO() returns -1/0/+1 depending on the compared
// states order. We need to negate it for the desired condition of equality.
#define MY_HASH_COMPARE() (!MY_HASH_COMPARE_PROTO())

/* Define MY_HASH_COMPARE_PROTO() */
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
        /* End of MY_HASH_COMPARE_PROTO() */

        while (item != NULL)
        {
            /*
                We first compare the hash values, because it is faster than
                comparing the entire data structure.
            */
            if ((item->hash_value == hash_value)
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
                && (item->secondary_hash_value == secondary_hash_value)
#endif
                && MY_HASH_COMPARE())
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

#if ((defined(INDIRECT_STACK_STATES) &&                                        \
         (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH)) ||            \
     (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH))

#include "wrap_xxhash.h"
#endif

#ifdef INDIRECT_STACK_STATES

#define REPLACE_WITH_CACHED(condition_expr)                                    \
    if (condition_expr)                                                        \
    {                                                                          \
        fcs_compact_alloc_release(stacks_allocator);                           \
        *(current_stack) = cached_stack;                                       \
    }

/* TODO : Maybe define an accesor for new_state_key->columns (also see the
 * replaced_with_cached macro above.
 * */
static inline void fc_solve_cache_stacks(
    fcs_hard_thread *const hard_thread, fcs_kv_state *const new_state)
{
#ifdef FCS_SINGLE_HARD_THREAD
#define instance hard_thread
#else
    fcs_instance *const instance = hard_thread->instance;
#endif
    STACKS__SET_PARAMS();
    register fcs_state *const new_state_key = new_state->key;
    register fcs_state_extra_info *const new_state_info = new_state->val;
    compact_allocator *const stacks_allocator =
        &(HT_FIELD(hard_thread, allocator));
    fcs_cards_column *current_stack = new_state_key->columns;

    for (int i = 0; i < LOCAL_STACKS_NUM; ++i, ++current_stack)
    {
        /*
         * If the stack is not a copy - it is already cached so skip
         * to the next stack
         * */
        if (!(new_state_info->stacks_copy_on_write_flags & (1 << i)))
        {
            continue;
        }

        var_AUTO(column, fcs_state_get_col(*(new_state_key), i));
        const int col_len = (fcs_col_len(column) + 1);

        char *const new_ptr =
            (char *)fcs_compact_alloc_ptr(stacks_allocator, col_len);
        memcpy(new_ptr, column, col_len);
        *(current_stack) = new_ptr;
        column = fcs_state_get_col(*new_state_key, i);

        void *cached_stack;
#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
        /* Calculate the hash value for the stack */
        /* This hash function was ripped from the Perl source code.
         * (It is not derived work however). */
        fcs_hash_value hash_value_int = 0;
        {
            const char *s_ptr = (char *)(*(current_stack));
            const char *s_end = s_ptr + fcs_col_len(s_ptr) + 1;
            while (s_ptr < s_end)
            {
                hash_value_int += (hash_value_int << 5) + *(s_ptr++);
            }
            hash_value_int += (hash_value_int >> 5);
        }
#endif

        cached_stack = fc_solve_hash_insert(
            &(instance->stacks_hash), column, DO_XXH(*(current_stack), col_len)
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
                                                  ,
            hash_value_int
#endif
        );
        REPLACE_WITH_CACHED(cached_stack);

#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)
        REPLACE_WITH_CACHED(fc_solve_columns_google_hash_insert(
            instance->stacks_hash, column, &cached_stack));
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL2_TREE)

        cached_stack =
            fcs_libavl2_stacks_tree_insert(instance->stacks_tree, column);

        REPLACE_WITH_CACHED(cached_stack != NULL);

#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
        cached_stack =
            (void *)rbsearch(*(current_stack), instance->stacks_tree);

        REPLACE_WITH_CACHED(cached_stack != *(current_stack));
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
        cached_stack =
            g_tree_lookup(instance->stacks_tree, (gpointer) * (current_stack));

        /* REPLACE_WITH_CACHED contains an if statement */
        REPLACE_WITH_CACHED(cached_stack != NULL) else
        {
            g_tree_insert(instance->stacks_tree, (gpointer) * (current_stack),
                (gpointer) * (current_stack));
        }
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
        cached_stack = g_hash_table_lookup(
            instance->stacks_hash, (gconstpointer) * (current_stack));
        REPLACE_WITH_CACHED(cached_stack != NULL) else
        {
            g_hash_table_insert(instance->stacks_hash,
                (gpointer) * (current_stack), (gpointer) * (current_stack));
        }
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)
        PWord_t *PValue;
        JHSI(PValue, instance->stacks_judy_array, column, col_len);
        /* later_todo : Handle out-of-memory. */
        if (*PValue == 0)
        {
            /*  A new stack */
            *PValue = (PWord_t)column;
        }
        else
        {
            cached_stack = (void *)(*PValue);
            REPLACE_WITH_CACHED(1);
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
#define fc_solve_cache_stacks(hard_thread, new_state_key)
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
guint fc_solve_hash_function(gconstpointer key)
{
    const char *s_ptr = (const char *)key;
    const char *const s_end = s_ptr + sizeof(fcs_state);
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

static inline void upon_new_state(fcs_instance *const instance GCC_UNUSED,
    fcs_hard_thread *const hard_thread GCC_UNUSED,
    fcs_state_extra_info *const new_state_info)
{
    fcs_collectible_state *const parent_state = new_state_info->parent;
    /* The new state was not found in the cache, and it was already inserted */
    if (likely(parent_state))
    {
        (FCS_S_NUM_ACTIVE_CHILDREN(parent_state))++;
#ifdef FCS_WITH_MOVES
        /* If parent_val is defined, so is moves_to_parent */
        new_state_info->moves_to_parent = fc_solve_move_stack_compact_allocate(
            hard_thread, new_state_info->moves_to_parent);
#endif
    }

#ifndef FCS_DISABLE_NUM_STORED_STATES
#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
    ++instance->active_num_states_in_collection;
#endif
    ++instance->num_states_in_collection;
#endif
}

static inline bool handle_existing_void(fcs_instance *const instance,
    fcs_hard_thread *const hard_thread, fcs_kv_state *const new_state,
    fcs_kv_state *const existing_state_raw, void *const existing_void)
{
    if (existing_void)
    {
        FCS_STATE_collectible_to_kv(existing_state_raw, existing_void);
        return FALSE;
    }
    else
    {
        upon_new_state(instance, hard_thread, new_state->val);
#ifdef DEBUG
        if (getenv("FCS_DEBUG2"))
        {
            printf("%s", "\n\nAdded state: ");
            for (size_t i = 0; i < sizeof(new_state->key[0]); ++i)
            {
                printf("\\x%02x", (int)((unsigned char *)(new_state->key))[i]);
            }
            printf("\n\n");
        }
#endif
        return TRUE;
    }
}
#define HANDLE_existing_void(existing_void)                                    \
    handle_existing_void(                                                      \
        instance, hard_thread, new_state, existing_state_raw, (existing_void))

bool fc_solve_check_and_add_state(fcs_hard_thread *const hard_thread,
    fcs_kv_state *const new_state, fcs_kv_state *const existing_state_raw)
{
/*
 * TODO : these accessor macros are probably out-of-date and won't work with
 * some of the less commonly tested data storage backends. They should be
 * tested and updated.
 * */
#define new_state_key (new_state->key)

#ifdef FCS_SINGLE_HARD_THREAD
#define instance hard_thread
#else
    fcs_instance *const instance = hard_thread->instance;
#endif
    fc_solve_cache_stacks(hard_thread, new_state);
    fc_solve_canonize_state(new_state_key PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
            PASS_STACKS(INSTANCE_STACKS_NUM));

    /*
        The objective of this part of the code is:
        1. To check if new_state_key / new_state_val is already in the
       prev_states collection.
        2. If not, to add it and to set check to true.
        3. If so, to set check to false.
      */

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
    fcs_hash_value hash_value_int = 0;
    {
        const char *s_ptr = (char *)new_state_key;
        const char *s_end = s_ptr + sizeof(*new_state_key);
        while (s_ptr < s_end)
        {
            hash_value_int += (hash_value_int << 5) + *(s_ptr++);
        }
        hash_value_int += (hash_value_int >> 5);
    }
#endif
#ifdef FCS_RCS_STATES
#define A new_state->val, new_state->key
#else
#define A FCS_STATE_kv_to_collectible(new_state)
#endif
#ifdef FCS_ENABLE_SECONDARY_HASH_VALUE
#define B , hash_value_int
#else
#define B
#endif
    return HANDLE_existing_void(fc_solve_hash_insert(
        &(instance->hash), A, DO_XXH(new_state_key, sizeof(*new_state_key)) B));
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)
    void *existing_void;
    if (!fc_solve_states_google_hash_insert(instance->hash,
            FCS_STATE_kv_to_collectible(new_state), &(existing_void)))
    {
        existing_void = NULL;
    }
    return HANDLE_existing_void(existing_void);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    const_AUTO(new_state_void, FCS_STATE_kv_to_collectible(new_state));
    const void *existing_void = rbsearch(new_state_void, instance->tree);
    return HANDLE_existing_void(
        existing_void == new_state_void ? NULL : existing_void);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_KAZ_TREE)
#ifdef FCS_RCS_STATES
    instance->tree_new_state_key = new_state->key;
    instance->tree_new_state = new_state->val;
#endif
    return HANDLE_existing_void(fc_solve_kaz_tree_alloc_insert(instance->tree,
#ifdef FCS_RCS_STATES
        new_state_val
#else
        FCS_STATE_kv_to_collectible(new_state)
#endif
        ));
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL2_TREE)
#ifdef FCS_RCS_STATES
    instance->tree_new_state_key = new_state->key;
    instance->tree_new_state = new_state->val;
#endif
    return HANDLE_existing_void(fcs_libavl2_states_tree_insert(
        instance->tree, FCS_STATE_kv_to_collectible(new_state)));
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    const_AUTO(new_state_void, FCS_STATE_kv_to_collectible(new_state));
    const_AUTO(
        existing_void, g_tree_lookup(instance->tree, (gpointer)new_state_void));
    if (!existing_void)
    {
        g_tree_insert(
            instance->tree, (gpointer)new_state_void, (gpointer)new_state_void);
    }
    return HANDLE_existing_void(
        existing_void == new_state_void ? NULL : existing_void);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    const_AUTO(new_state_void, FCS_STATE_kv_to_collectible(new_state));
    const_AUTO(existing_void,
        g_hash_table_lookup(instance->hash, (gpointer)new_state_void));
    if (!existing_void)
    {
        g_hash_table_insert(
            instance->hash, (gpointer)new_state_void, (gpointer)new_state_void);
    }
    return HANDLE_existing_void(
        existing_void == new_state_void ? NULL : existing_void);
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    DBT key, value;
    key.data = new_state;
    key.size = sizeof(*new_state);
    if ((is_state_new =
                (instance->db->get(instance->db, NULL, &key, &value, 0) == 0)))
    {
        /* The new state was not found. Let's insert it.
         * The value must be the same as the key, so g_tree_lookup()
         * will return it. */

        value.data = key.data;
        value.size = key.size;
        instance->db->put(instance->db, NULL, &key, &value, 0);
    }
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY)
    PWord_t *PValue;

    JHSI(PValue, instance->judy_array, new_state_key, sizeof(*new_state_key));

    /* later_todo : Handle out-of-memory. */
    const_AUTO(val, *PValue);
    if (val == 0)
    {
        /* A new state. */
        *PValue = (PWord_t)(FCS_STATE_kv_to_collectible(new_state));
    }
    return HANDLE_existing_void(val);
#else
#error Unknown FCS_STATE_STORAGE. Please define it to a valid value.
#endif
}
#ifdef FCS_SINGLE_HARD_THREAD
#undef instance
#endif
