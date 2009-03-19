/*
 * caas.c - the various possible implementations of the function
 * fc_solve_check_and_add_state().
 *
 * Written by Shlomi Fish ( http://www.shlomifish.org/ ), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#ifndef FC_SOLVE__CAAS_C
#define FC_SOLVE__CAAS_C

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "fcs_dm.h"
#include "fcs.h"

#include "fcs_isa.h"

#include "lookup2.h"


#ifdef INDIRECT_STACK_STATES
#include "fcs_hash.h"
#endif

#include "caas.h"
#include "ms_ca.h"

#include "test_arr.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif





#ifdef INDIRECT_STACK_STATES

#define replace_with_cached(condition_expr) \
        if (condition_expr)     \
        {      \
            fcs_compact_alloc_release(hard_thread->stacks_allocator);    \
            new_state_key->stacks[a] = cached_stack;       \
        }

static void GCC_INLINE fc_solve_cache_stacks(
        fc_solve_hard_thread_t * hard_thread,
        fcs_state_t * new_state_key,
        fcs_state_extra_info_t * new_state_val
        )
{
    int a;
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH)
    SFO_hash_value_t hash_value_int;
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)
    PWord_t * PValue;
#endif
    void * cached_stack;
    char * new_ptr;
    fc_solve_instance_t * instance = hard_thread->instance;
    int stacks_num = instance->stacks_num;
    

    for(a=0 ; a<stacks_num ; a++)
    {
        /* 
         * If the stack is not a copy - it is already cached so skip
         * to the next stack
         * */
        if (! (new_state_val->stacks_copy_on_write_flags & (1 << a)))
        {
            continue;
        }
        /* new_state_key->stacks[a] = realloc(new_state_key->stacks[a], fcs_stack_len(new_state_key, a)+1); */
        fcs_compact_alloc_typed_ptr_into_var(new_ptr, char, hard_thread->stacks_allocator, (fcs_stack_len(*new_state_key, a)+1));
        memcpy(new_ptr, new_state_key->stacks[a], (fcs_stack_len(*new_state_key, a)+1));
        new_state_key->stacks[a] = new_ptr;

#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH
        /* Calculate the hash value for the stack */
        /* This hash function was ripped from the Perl source code.
         * (It is not derived work however). */
        {
            const char * s_ptr = (char*)(new_state_key->stacks[a]);
            const char * s_end = s_ptr+fcs_stack_len(*new_state_key, a)+1;
            hash_value_int = 0;
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
        

        {
            void * dummy;
            int verdict;

            verdict = fc_solve_hash_insert(
                instance->stacks_hash,
                new_state_key->stacks[a],
                new_state_key->stacks[a],
                &cached_stack,
                &dummy,
                freecell_solver_lookup2_hash_function(
                    (ub1 *)new_state_key->stacks[a],
                    (fcs_stack_len(*new_state_key, a)+1),
                    24
                    ),
                hash_value_int,
                1
                );


            replace_with_cached(verdict);
        }

        
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_AVL_TREE) || (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE)

#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_AVL_TREE)
#define LIBAVL_INSERT(x,y) avl_insert(x,y)
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE)
#define LIBAVL_INSERT(x,y) rb_insert(x,y)
#else
#error unknown FCS_STACK_STORAGE
#endif

        cached_stack =
            LIBAVL_INSERT(
            instance->stacks_tree,
            new_state_key->stacks[a]
            );

#undef LIBAVL_INSERT

        replace_with_cached(cached_stack != NULL);

#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
        cached_stack = (void *)rbsearch(
            new_state_key->stacks[a],
            instance->stacks_tree
            );

        replace_with_cached(cached_stack != new_state_key->stacks[a]);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
        cached_stack = g_tree_lookup(
             instance->stacks_tree,
             (gpointer)new_state_key->stacks[a]
             );

        /* replace_with_cached contains an if statement */
        replace_with_cached(cached_stack != NULL)
        else
        {
            g_tree_insert(
                instance->stacks_tree,
                (gpointer)new_state_key->stacks[a],
                (gpointer)new_state_key->stacks[a]
                );
        }
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
        cached_stack = g_hash_table_lookup(
            instance->stacks_hash,
            (gconstpointer)new_state_key->stacks[a]
            );
        replace_with_cached(cached_stack != NULL)
        else
        {
            g_hash_table_insert(
                instance->stacks_hash,
                (gpointer)new_state_key->stacks[a],
                (gpointer)new_state_key->stacks[a]
                );
        }
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_JUDY)
        JHSI(
            PValue,
            instance->stacks_judy_array,
            new_state_key->stacks[a], 
            (fcs_stack_len(*new_state_key, a)+1)
        );
        /* TODO : Handle out-of-memory. */
        if (*PValue == 0)
        {
            /*  A new stack */
            *PValue = (PWord_t)new_state_key->stacks[a];
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
}
#else
#define fc_solve_cache_stacks(instance, new_state)
#endif


#ifdef FCS_WITH_TALONS
void fc_solve_cache_talon(
    fc_solve_instance_t * instance,
    fcs_state_t * new_state_key,
    fcs_state_extra_info_t * new_state_val
    )
{
    void * cached_talon;
    int hash_value_int;

    new_state_key->talon = realloc(new_state_key->talon, fcs_klondike_talon_len(new_state_key)+1);
#error Add Hash Code
    hash_value_int = *(SFO_hash_value_t*)instance->hash_value;
    if (hash_value_int < 0)
    {
        /*
         * This is a bit mask that nullifies the sign bit of the
         * number so it will always be positive
         * */
        hash_value_int &= (~(1<<((sizeof(hash_value_int)<<3)-1)));
    }

    cached_talon = (void *)fc_solve_hash_insert(
        instance->talons_hash,
        new_state_key->talon,
        hash_value_int,
        1
        );

    if (cached_talon != NULL)
    {
        free(new_state_key->talon);
        new_state_key->talon = cached_talon;
    }
}
#endif


#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
guint fc_solve_hash_function(gconstpointer key)
{
    guint hash_value;
    const char * s_ptr = (char*)key;
    const char * s_end = s_ptr+sizeof(fcs_state_t);
    hash_value = 0;
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
 *        5a. Return FCS_STATE_ALREADY_EXISTS
 *
 *    If it isn't:
 *
 *        5b. Call solve_for_state() on the board.
 *
 * */

GCC_INLINE int fc_solve_check_and_add_state(
    fc_solve_soft_thread_t * soft_thread,
    fcs_state_extra_info_t * new_state_val,
    fcs_state_extra_info_t * * existing_state_val
    )
{
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    SFO_hash_value_t hash_value_int;
#endif
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    fcs_state_with_locations_t * * pos_ptr;
    int found;
#endif
    fc_solve_hard_thread_t * hard_thread = soft_thread->hard_thread;
    fc_solve_instance_t * instance = hard_thread->instance;
    fcs_state_t * new_state_key = new_state_val->key;

    int is_state_new;

    if (check_if_limits_exceeded())
    {
        return FCS_STATE_BEGIN_SUSPEND_PROCESS;
    }

    fc_solve_cache_stacks(hard_thread, new_state_key, new_state_val);

    fc_solve_canonize_state(new_state_val, instance->freecells_num, instance->stacks_num);

/*
    The objective of this part of the code is:
    1. To check if new_state_key / new_state_val is already in the prev_states 
       collection.
    2. If not, to add it and to set check to true.
    3. If so, to set check to false.
  */

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
#ifdef FCS_WITH_MHASH
    /*
        Calculate the has function of the state.
    */
    {
        char * temp_ptr;
        instance->mhash_context = mhash_init(instance->mhash_type);
        mhash(instance->mhash_context, (void *)new_state, sizeof(fcs_state_t));
        temp_ptr = mhash_end(instance->mhash_context);
        /* Retrieve the first 32 bits and make them the hash value */
        hash_value_int = *(SFO_hash_value_t*)temp_ptr;
        free(temp_ptr);
    }

    if (hash_value_int < 0)
    {
        /*
         * This is a bit mask that nullifies the sign bit of the
         * number so it will always be positive
         * */
        hash_value_int &= (~(1<<((sizeof(hash_value_int)<<3)-1)));
    }
    is_state_new = ((*existing_state = fc_solve_hash_insert(
        instance->hash,
        new_state,
        hash_value_int,
        1
        )) == NULL);



#else
    {
        const char * s_ptr = (char*)new_state_key;
        const char * s_end = s_ptr+sizeof(*new_state_key);
        hash_value_int = 0;
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
    {
        void * existing_key_void, * existing_val_void;
    is_state_new = (fc_solve_hash_insert(
        instance->hash,
        new_state_key,
        new_state_val,
        &existing_key_void,
        &existing_val_void,
        freecell_solver_lookup2_hash_function(
            (ub1 *)new_state_key,
            sizeof(*new_state_key),
            24
            ),
        hash_value_int,
        1
        ) == 0);
        if (! is_state_new)
        {
            *existing_state_val = existing_val_void;
        }
    }


#endif
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    /* Try to see if the state is found in indirect_prev_states */
    if ((pos_ptr = (fcs_state_with_locations_t * *)bsearch(&new_state,
                instance->indirect_prev_states,
                instance->num_indirect_prev_states,
                sizeof(fcs_state_with_locations_t *),
                fc_solve_state_compare_indirect)) == NULL)
    {
        /* It isn't in prev_states, but maybe it's in the sort margin */
        pos_ptr = (fcs_state_with_locations_t * *)fc_solve_bsearch(
            &new_state,
            instance->indirect_prev_states_margin,
            instance->num_prev_states_margin,
            sizeof(fcs_state_with_locations_t *),
            fc_solve_state_compare_indirect_with_context,
            NULL,
            &found);

        if (found)
        {
            is_state_new = 0;
            *existing_state = *pos_ptr;
        }
        else
        {
            /* Insert the state into its corresponding place in the sort
             * margin */
            memmove((void*)(pos_ptr+1),
                    (void*)pos_ptr,
                    sizeof(fcs_state_with_locations_t *) *
                    (instance->num_prev_states_margin-
                      (pos_ptr-instance->indirect_prev_states_margin)
                    ));
            *pos_ptr = new_state;

            instance->num_prev_states_margin++;

            if (instance->num_prev_states_margin >= PREV_STATES_SORT_MARGIN)
            {
                /* The sort margin is full, let's combine it with the main array */
                if (instance->num_indirect_prev_states + instance->num_prev_states_margin > instance->max_num_indirect_prev_states)
                {
                    while (instance->num_indirect_prev_states + instance->num_prev_states_margin > instance->max_num_indirect_prev_states)
                    {
                        instance->max_num_indirect_prev_states += PREV_STATES_GROW_BY;
                    }
                    instance->indirect_prev_states = realloc(instance->indirect_prev_states, sizeof(fcs_state_with_locations_t *) * instance->max_num_indirect_prev_states);
                }

                fc_solve_merge_large_and_small_sorted_arrays(
                    instance->indirect_prev_states,
                    instance->num_indirect_prev_states,
                    instance->indirect_prev_states_margin,
                    instance->num_prev_states_margin,
                    sizeof(fcs_state_with_locations_t *),
                    fc_solve_state_compare_indirect_with_context,
                    NULL
                );

                instance->num_indirect_prev_states += instance->num_prev_states_margin;

                instance->num_prev_states_margin=0;
            }
            is_state_new = 1;
        }

    }
    else
    {
        *existing_state = *pos_ptr;
        is_state_new = 0;
    }

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)
    *existing_state_val = (fcs_state_extra_info_t *)rbsearch(new_state_val, 
            instance->tree
            );
    is_state_new = ((*existing_state_val) == new_state_val);

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE)

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE)
#define fcs_libavl_states_tree_insert(a,b) avl_insert((a),(b))
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE)
#define fcs_libavl_states_tree_insert(a,b) rb_insert((a),(b))
#endif

    *existing_state = fcs_libavl_states_tree_insert(instance->tree, new_state);
    is_state_new = (*existing_state == NULL);

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
    *existing_state = g_tree_lookup(instance->tree, (gpointer)new_state);
    if (*existing_state == NULL)
    {
        /* The new state was not found. Let's insert it.
         * The value must be the same as the key, so g_tree_lookup()
         * will return it. */
        g_tree_insert(
            instance->tree,
            (gpointer)new_state,
            (gpointer)new_state
            );
        is_state_new = 1;
    }
    else
    {
        is_state_new = 0;
    }



#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
    *existing_state = g_hash_table_lookup(instance->hash, (gpointer)new_state);
    if (*existing_state == NULL)
    {
        /* The new state was not found. Let's insert it.
         * The value must be the same as the key, so g_tree_lookup()
         * will return it. */
        g_hash_table_insert(
            instance->hash,
            (gpointer)new_state,
            (gpointer)new_state

            );
        is_state_new = 1;
    }
    else
    {
        is_state_new = 0;
    }

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
    {
        DBT key, value;
        key.data = new_state;
        key.size = sizeof(*new_state);
        if (instance->db->get(
            instance->db,
            NULL,
            &key,
            &value,
            0
            ) == 0)
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
            is_state_new = 1;
        }
        else
        {
            is_state_new = 0;
            *existing_state = (fcs_state_with_locations_t *)(value.data);
        }
    }
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_JUDY)
    {
        PWord_t * PValue;

        JHSI(PValue, instance->judy_array, new_state_key, sizeof(*new_state_key));

        /* TODO : Handle out-of-memory. */
        if (*PValue == 0)
        {
            /* A new state. */
            is_state_new = 1;
            *PValue = (PWord_t)(*existing_state_val = new_state_val);
        }
        else
        {            
            /* Already exists. */
            is_state_new = 0;
            *existing_state_val = (fcs_state_extra_info_t *)(*PValue);
        }
    }
#else
#error no define
#endif
    if (is_state_new)
    {
        /* The new state was not found in the cache, and it was already inserted */
        if (new_state_val->parent_val)
        {
            new_state_val->parent_val->num_active_children++;
        }
        instance->num_states_in_collection++;

        if (new_state_val->moves_to_parent != NULL)
        {
            new_state_val->moves_to_parent = 
                fc_solve_move_stack_compact_allocate(
                    hard_thread, 
                    new_state_val->moves_to_parent
                    );
        }

        return FCS_STATE_DOES_NOT_EXIST;
    }
    else
    {
        return FCS_STATE_ALREADY_EXISTS;
    }
}

#endif /* #ifndef FC_SOLVE__CAAS_C */
