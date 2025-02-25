// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// check_and_add_state.c - the various possible implementations of the function
// fc_solve_check_and_add_state().

#include "fcs_hash__insert.h"

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

static inline void fc_solve_cache_stacks(
    fcs_hard_thread *const hard_thread, fcs_kv_state *const new_state)
{
    fcs_instance *const instance = HT_INSTANCE(hard_thread);
    STACKS__SET_PARAMS();
    register fcs_state *const new_state_key = new_state->key;
    register fcs_state_extra_info *const new_state_info = new_state->val;
    compact_allocator *const stacks_allocator =
        &(HT_FIELD(hard_thread, allocator));
    fcs_cards_column *current_stack = new_state_key->columns;

    for (int i = 0; i < LOCAL_STACKS_NUM; ++i, ++current_stack)
    {
        // If the stack is not a copy - it is already cached so skip
        // to the next stack
        if (!(new_state_info->stacks_copy_on_write_flags & (1 << i)))
        {
            continue;
        }

        var_AUTO(column, fcs_state_get_col(*(new_state_key), i));
        const size_t col_len = (fcs_col_len(column) + 1);

        fcs_cards_column new_ptr =
            (fcs_cards_column)fcs_compact_alloc_ptr(stacks_allocator, col_len);
        memcpy(new_ptr, column, col_len);
        *(current_stack) = new_ptr;
        column = fcs_state_get_col(*new_state_key, i);

        void *cached_stack;
#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH

        cached_stack = fc_solve_hash_insert(&(instance->stacks_hash), column,
            DO_XXH(*(current_stack), col_len));
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

        // REPLACE_WITH_CACHED contains an if statement
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
        // later_todo : Handle out-of-memory.
        if (*PValue == 0)
        {
            // A new column.
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
}

#else // #ifdef INDIRECT_STACK_STATES
#define fc_solve_cache_stacks(hard_thread, new_state_key)
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
guint fc_solve_hash_function(gconstpointer key)
{
    const int8_t *s_ptr = (const int8_t *)key;
    const int8_t *const s_end = s_ptr + sizeof(fcs_state);
    guint hash_value = 0;
    while (s_ptr < s_end)
    {
        hash_value += (hash_value << 5) + *(s_ptr++);
    }
    hash_value += (hash_value >> 5);

    return hash_value;
}
#endif

// check_and_add_state() does the following things:
//
// 1. Check if the number of iterations exceeded its maximum, and if so
//    return FCS_STATE_EXCEEDS_MAX_NUM_TIMES in order to terminate the
//    solving process.
// 2. Check if the maximal depth was reached and if so return
//    FCS_STATE_EXCEEDS_MAX_DEPTH
// 3. Canonize the state.
// 4. Check if the state is already found in the collection of the states
//    that were already checked.
//    If it is:
//
//        5a. Return false.
//
//    If it isn't:
//
//        5b. Add the new state and return true.

static inline void upon_new_state(fcs_instance *const instance GCC_UNUSED,
    fcs_hard_thread *const hard_thread GCC_UNUSED,
    fcs_state_extra_info *const new_state_info)
{
    fcs_collectible_state *const parent_state = new_state_info->parent;
    // The new state was not found in the cache, and it was already inserted
    if (likely(parent_state))
    {
        (FCS_S_NUM_ACTIVE_CHILDREN(parent_state))++;
#ifdef FCS_WITH_MOVES
        // If parent_val is defined, so is moves_to_parent
        new_state_info->moves_to_parent = fc_solve_move_stack_compact_allocate(
            hard_thread, new_state_info->moves_to_parent);
#endif
    }

#ifndef FCS_DISABLE_NUM_STORED_STATES
#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
    ++instance->active_num_states_in_collection;
#endif
    ++instance->i__stats.num_states_in_collection;
#endif
}

static inline bool handle_existing_void(fcs_instance *const instance,
    fcs_hard_thread *const hard_thread, fcs_kv_state *const new_state,
    fcs_kv_state *const existing_state_raw, void *const existing_void)
{
    if (existing_void)
    {
        FCS_STATE_collectible_to_kv(existing_state_raw, existing_void);
        return false;
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
                printf("\\x%02x", (unsigned)((uint8_t *)(new_state->key))[i]);
            }
            printf("\n\n");
        }
#endif
        return true;
    }
}
#define HANDLE_existing_void(existing_void)                                    \
    handle_existing_void(                                                      \
        instance, hard_thread, new_state, existing_state_raw, (existing_void))

bool fc_solve_check_and_add_state(fcs_hard_thread *const hard_thread,
    fcs_kv_state *const new_state, fcs_kv_state *const existing_state_raw)
{
#define new_state_key (new_state->key)

    fcs_instance *const instance = HT_INSTANCE(hard_thread);
    fc_solve_cache_stacks(hard_thread, new_state);
    fc_solve_canonize_state(new_state_key PASS_FREECELLS(INSTANCE_FREECELLS_NUM)
            PASS_STACKS(INSTANCE_STACKS_NUM));

    // The objective of this part of the code is:
    // 1. To check if new_state_key / new_state_val is already in the
    // prev_states collection.
    // 2. If not, to add it and to set check to true.
    // 3. If so, to set check to false.

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
#ifdef FCS_RCS_STATES
#define FCS_MY_STATE new_state->val, new_state->key
#else
#define FCS_MY_STATE FCS_STATE_kv_to_collectible(new_state)
#endif
    return HANDLE_existing_void(fc_solve_hash_insert(&(instance->hash),
        FCS_MY_STATE, DO_XXH(new_state_key, sizeof(*new_state_key))));
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
        // The new state was not found. Let's insert it.
        // The value must be the same as the key, so g_tree_lookup()
        // will return it.
        value.data = key.data;
        value.size = key.size;
        instance->db->put(instance->db, NULL, &key, &value, 0);
    }
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY)
    PWord_t *PValue;

    JHSI(PValue, instance->judy_array, new_state_key, sizeof(*new_state_key));

    // later_todo : Handle out-of-memory
    const_AUTO(val, *PValue);
    if (val == 0)
    {
        // A new state.
        *PValue = (PWord_t)(FCS_STATE_kv_to_collectible(new_state));
    }
    return HANDLE_existing_void(val);
#else
#error Unknown FCS_STATE_STORAGE. Please define it to a valid value.
#endif
}
