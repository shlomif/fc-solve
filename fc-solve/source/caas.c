/*
 * caas.c - the various possible implementations of the function
 * freecell_solver_check_and_add_state().
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#ifndef __CAAS_C
#define __CAAS_C

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "fcs_dm.h"
#include "fcs.h"

#include "fcs_isa.h"


#ifdef INDIRECT_STACK_STATES
#include "fcs_hash.h"
#endif

#include "caas.h"
#include "ms_ca.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif


/*
    The objective of the fcs_caas_check_and_insert macros is:
    1. To check if new_state is already in the prev_states collection.
    2. If not, to add it and to set check to true.
    3. If so, to set check to false.
  */


#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
#ifdef FCS_WITH_MHASH
#define fcs_caas_check_and_insert()            \
    /*                                            \
        Calculate the has function of the state.   \
    */                   \
    {        \
        char * temp_ptr;    \
        instance->mhash_context = mhash_init(instance->mhash_type); \
        mhash(instance->mhash_context, (void *)new_state, sizeof(fcs_state_t));    \
        temp_ptr = mhash_end(instance->mhash_context); \
        /* Retrieve the first 32 bits and make them the hash value */      \
        hash_value_int = *(SFO_hash_value_t*)temp_ptr;      \
        free(temp_ptr);      \
    }      \
            \
    if (hash_value_int < 0)       \
    {    \
        /*             \
         * This is a bit mask that nullifies the sign bit of the  \
         * number so it will always be positive           \
         * */            \
        hash_value_int &= (~(1<<((sizeof(hash_value_int)<<3)-1)));     \
    }    \
    check = ((*existing_state = freecell_solver_hash_insert(          \
        instance->hash,              \
        new_state,                   \
        hash_value_int,              \
        1                            \
        )) == NULL);



#else
#define fcs_caas_check_and_insert()              \
    {        \
        const char * s_ptr = (char*)new_state;       \
        const char * s_end = s_ptr+sizeof(fcs_state_t);   \
        hash_value_int = 0;        \
        while (s_ptr < s_end)        \
        {            \
            hash_value_int += (hash_value_int << 5) + *(s_ptr++);       \
        }         \
        hash_value_int += (hash_value_int>>5);       \
    }           \
    if (hash_value_int < 0)       \
    {    \
        /*             \
         * This is a bit mask that nullifies the sign bit of the  \
         * number so it will always be positive           \
         * */            \
        hash_value_int &= (~(1<<((sizeof(hash_value_int)<<3)-1)));     \
    }    \
    check = ((*existing_state = freecell_solver_hash_insert(          \
        instance->hash,              \
        new_state,                   \
        hash_value_int,              \
        1                            \
        )) == NULL);

#endif
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
#define fcs_caas_check_and_insert()              \
    /* Try to see if the state is found in indirect_prev_states */  \
    if ((pos_ptr = (fcs_state_with_locations_t * *)bsearch(&new_state,                                         \
                instance->indirect_prev_states,                     \
                instance->num_indirect_prev_states,                 \
                sizeof(fcs_state_with_locations_t *),               \
                freecell_solver_state_compare_indirect)) == NULL)                \
    {                                                               \
        /* It isn't in prev_states, but maybe it's in the sort margin */        \
        pos_ptr = (fcs_state_with_locations_t * *)freecell_solver_bsearch(              \
            &new_state,                                                     \
            instance->indirect_prev_states_margin,                          \
            instance->num_prev_states_margin,                               \
            sizeof(fcs_state_with_locations_t *),                           \
            freecell_solver_state_compare_indirect_with_context,                        \
            NULL,                  \
            &found);              \
                             \
        if (found)                \
        {                             \
            check = 0;                   \
            *existing_state = *pos_ptr;     \
        }                                 \
        else                               \
        {                                     \
            /* Insert the state into its corresponding place in the sort         \
             * margin */                             \
            memmove((void*)(pos_ptr+1), (void*)pos_ptr, sizeof(fcs_state_with_locations_t *)*(instance->num_prev_states_margin-(pos_ptr-instance->indirect_prev_states_margin)));          \
            *pos_ptr = new_state;                \
                       \
            instance->num_prev_states_margin++;             \
              \
            if (instance->num_prev_states_margin >= PREV_STATES_SORT_MARGIN)         \
            {          \
                /* The sort margin is full, let's combine it with the main array */         \
                if (instance->num_indirect_prev_states + instance->num_prev_states_margin > instance->max_num_indirect_prev_states)       \
                {               \
                    while (instance->num_indirect_prev_states + instance->num_prev_states_margin > instance->max_num_indirect_prev_states)        \
                    {            \
                        instance->max_num_indirect_prev_states += PREV_STATES_GROW_BY;         \
                    }                \
                    instance->indirect_prev_states = realloc(instance->indirect_prev_states, sizeof(fcs_state_with_locations_t *) * instance->max_num_indirect_prev_states);       \
                }             \
                            \
                freecell_solver_merge_large_and_small_sorted_arrays(           \
                    instance->indirect_prev_states,              \
                    instance->num_indirect_prev_states,           \
                    instance->indirect_prev_states_margin,          \
                    instance->num_prev_states_margin,              \
                    sizeof(fcs_state_with_locations_t *),           \
                    freecell_solver_state_compare_indirect_with_context,          \
                    NULL                        \
                );                   \
                                  \
                instance->num_indirect_prev_states += instance->num_prev_states_margin;       \
                          \
                instance->num_prev_states_margin=0;           \
            }                  \
            check = 1;               \
        }                  \
                    \
    }                   \
    else                 \
    {         \
        *existing_state = *pos_ptr; \
        check = 0;          \
    }

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBREDBLACK_TREE)

#define fcs_caas_check_and_insert()               \
    *existing_state = (fcs_state_with_locations_t *)rbsearch(new_state, instance->tree); \
    check = ((*existing_state) == new_state);

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE) || (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE)

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_AVL_TREE)
#define fcs_libavl_states_tree_insert(a,b) avl_insert((a),(b))
#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE)
#define fcs_libavl_states_tree_insert(a,b) rb_insert((a),(b))
#endif

#define fcs_caas_check_and_insert()       \
    *existing_state = fcs_libavl_states_tree_insert(instance->tree, new_state); \
    check = (*existing_state == NULL);

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_TREE)
#define fcs_caas_check_and_insert()       \
    *existing_state = g_tree_lookup(instance->tree, (gpointer)new_state);  \
    if (*existing_state == NULL) \
    {            \
        /* The new state was not found. Let's insert it.       \
         * The value must be the same as the key, so g_tree_lookup()   \
         * will return it. */                  \
        g_tree_insert(                        \
            instance->tree,                      \
            (gpointer)new_state,              \
            (gpointer)new_state                 \
            );                         \
        check = 1;                  \
    }              \
    else        \
    {          \
        check = 0;     \
    }



#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
#define fcs_caas_check_and_insert()       \
    *existing_state = g_hash_table_lookup(instance->hash, (gpointer)new_state); \
    if (*existing_state == NULL) \
    { \
        /* The new state was not found. Let's insert it.       \
         * The value must be the same as the key, so g_tree_lookup()   \
         * will return it. */                  \
        g_hash_table_insert(         \
            instance->hash,          \
            (gpointer)new_state,          \
            (gpointer)new_state            \
        \
            );           \
        check = 1;              \
    }          \
    else        \
    {          \
        check = 0;     \
    }

#elif (FCS_STATE_STORAGE == FCS_STATE_STORAGE_DB_FILE)
#define fcs_caas_check_and_insert()     \
    {         \
        DBT key, value;      \
        key.data = new_state; \
        key.size = sizeof(*new_state);      \
        if (instance->db->get(         \
            instance->db,        \
            NULL,        \
            &key,      \
            &value,      \
            0        \
            ) == 0) \
        {      \
            /* The new state was not found. Let's insert it.       \
             * The value must be the same as the key, so g_tree_lookup()   \
             * will return it. */                  \
                \
            value.data = key.data;     \
            value.size = key.size;     \
            instance->db->put(         \
                instance->db,      \
                NULL,           \
                &key,         \
                &value,         \
                0);             \
            check = 1;        \
        }         \
        else         \
        {         \
            check = 0;        \
            *existing_state = (fcs_state_with_locations_t *)(value.data);     \
        }         \
    }

#else
#error no define
#endif

#ifdef INDIRECT_STACK_STATES
static void GCC_INLINE freecell_solver_cache_stacks(
        freecell_solver_hard_thread_t * hard_thread,
        fcs_state_with_locations_t * new_state
        )
{
    int a;
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH)
    SFO_hash_value_t hash_value_int;
#endif
    void * cached_stack;
    char * new_ptr;
    freecell_solver_instance_t * instance = hard_thread->instance;
    int stacks_num = instance->stacks_num;
    

    for(a=0 ; a<stacks_num ; a++)
    {
        /* 
         * If the stack is not a copy - it is already cached so skip
         * to the next stack
         * */
        if (! (new_state->stacks_copy_on_write_flags & (1 << a)))
        {
            continue;
        }
        //new_state->s.stacks[a] = realloc(new_state->s.stacks[a], fcs_stack_len(new_state->s, a)+1);
        fcs_compact_alloc_typed_ptr_into_var(new_ptr, char, hard_thread->stacks_allocator, (fcs_stack_len(new_state->s, a)+1));
        memcpy(new_ptr, new_state->s.stacks[a], (fcs_stack_len(new_state->s, a)+1));
        new_state->s.stacks[a] = new_ptr;

#if FCS_STACK_STORAGE == FCS_STACK_STORAGE_INTERNAL_HASH
        /* Calculate the hash value for the stack */
        /* This hash function was ripped from the Perl source code.
         * (It is not derived work however). */
        {
            const char * s_ptr = (char*)(new_state->s.stacks[a]);
            const char * s_end = s_ptr+fcs_stack_len(new_state->s, a)+1;
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

        cached_stack = (void *)freecell_solver_hash_insert(
            instance->stacks_hash,
            new_state->s.stacks[a],
            hash_value_int,
            1
            );

#define replace_with_cached(condition_expr) \
        if (cached_stack != NULL)     \
        {      \
            fcs_compact_alloc_release(hard_thread->stacks_allocator);    \
            new_state->s.stacks[a] = cached_stack;       \
        }

        replace_with_cached(cached_stack != NULL);
        
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_AVL_TREE) || (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE)
        cached_stack =
#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_AVL_TREE)
            avl_insert(
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE)
            rb_insert(
#endif
            instance->stacks_tree,
            new_state->s.stacks[a]
            );
#if 0
            )        /* In order to settle gvim and other editors that
                         are keen on parenthesis matching */
#endif

        replace_with_cached(cached_stack != NULL);

#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_LIBREDBLACK_TREE)
        cached_stack = (void *)rbsearch(
            new_state->s.stacks[a],
            instance->stacks_tree
            );

        replace_with_cached(cached_stack != new_state->s.stacks[a]);
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_TREE)
        cached_stack = g_tree_lookup(
             instance->stacks_tree,
             (gpointer)new_state->s.stacks[a]
             );

        /* replace_with_cached contains an if statement */
        replace_with_cached(cached_stack != NULL)
        else
        {
            g_tree_insert(
                instance->stacks_tree,
                (gpointer)new_state->s.stacks[a],
                (gpointer)new_state->s.stacks[a]
                );
        }
#elif (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GLIB_HASH)
        cached_stack = g_hash_table_lookup(
            instance->stacks_hash,
            (gconstpointer)new_state->s.stacks[a]
            );
        replace_with_cached(cached_stack != NULL)
        else
        {
            g_hash_table_insert(
                instance->stacks_hash,
                (gpointer)new_state->s.stacks[a],
                (gpointer)new_state->s.stacks[a]
                );
        }
#endif
    }
}
#else
#define freecell_solver_cache_stacks(instance, new_state)
#endif


#ifdef FCS_WITH_TALONS
void freecell_solver_cache_talon(
    freecell_solver_instance_t * instance,
    fcs_state_with_locations_t * new_state
    )
{
    void * cached_talon;
    int hash_value_int;

    new_state->s.talon = realloc(new_state->s.talon, fcs_klondike_talon_len(new_state->s)+1);
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

    cached_talon = (void *)freecell_solver_hash_insert(
        instance->talons_hash,
        new_state->s.talon,
        hash_value_int,
        1
        );

    if (cached_talon != NULL)
    {
        free(new_state->s.talon);
        new_state->s.talon = cached_talon;
    }
}
#endif


#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
guint freecell_solver_hash_function(gconstpointer key)
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

GCC_INLINE int freecell_solver_check_and_add_state(
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * new_state,
    fcs_state_with_locations_t * * existing_state,
    int depth)
{
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INTERNAL_HASH)
    SFO_hash_value_t hash_value_int;
#endif
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_INDIRECT)
    fcs_state_with_locations_t * * pos_ptr;
    int found;
#endif
    freecell_solver_hard_thread_t * hard_thread = soft_thread->hard_thread;
    freecell_solver_instance_t * instance = hard_thread->instance;

    int check;

    if ((instance->max_num_times >= 0) &&
        (instance->num_times >= instance->max_num_times))
    {
        return FCS_STATE_BEGIN_SUSPEND_PROCESS;
    }

    if ((hard_thread->ht_max_num_times >= 0) &&
        (hard_thread->num_times >= hard_thread->ht_max_num_times))
    {
        return FCS_STATE_BEGIN_SUSPEND_PROCESS;
    }

    if ((hard_thread->max_num_times >= 0) &&
        (hard_thread->num_times >= hard_thread->max_num_times))
    {
        return FCS_STATE_BEGIN_SUSPEND_PROCESS;
    }


    if ((instance->max_num_states_in_collection >= 0) &&
        (instance->num_states_in_collection >= instance->max_num_states_in_collection))
    {
        return FCS_STATE_BEGIN_SUSPEND_PROCESS;
    }

    if ((instance->max_depth >= 0) &&
        (instance->max_depth <= depth))
    {
        return FCS_STATE_EXCEEDS_MAX_DEPTH;
    }

    freecell_solver_cache_stacks(hard_thread, new_state);

    fcs_canonize_state(new_state, instance->freecells_num, instance->stacks_num);

    fcs_caas_check_and_insert();
    if (check)
    {
        /* The new state was not found in the cache, and it was already inserted */
        if (new_state->parent)
        {
            new_state->parent->num_active_children++;
        }
        instance->num_states_in_collection++;

#if 0
        {
            char * ptr;
            fcs_move_stack_t * old_move_stack_to_parent, * new_move_stack_to_parent;
            fcs_move_t * new_moves_to_parent;

            old_move_stack_to_parent = new_state->moves_to_parent;
            
            fcs_compact_alloc_typed_ptr_into_var(
                ptr, 
                char, 
                hard_thread->move_stacks_allocator, 
                (sizeof(fcs_move_stack_t) + sizeof(fcs_move_t)*old_move_stack_to_parent->num_moves)
                );
            new_move_stack_to_parent = (fcs_move_stack_t *)ptr;
            new_moves_to_parent = (fcs_move_t *)(ptr+sizeof(fcs_move_stack_t));
            new_move_stack_to_parent->moves = new_moves_to_parent;
            new_move_stack_to_parent->num_moves = 
                new_move_stack_to_parent->max_num_moves = 
                old_move_stack_to_parent->num_moves;
            memcpy(new_moves_to_parent, old_move_stack_to_parent->moves, sizeof(fcs_move_t)*old_move_stack_to_parent->num_moves);
            new_state->moves_to_parent = new_move_stack_to_parent;
        }
#endif
        if (new_state->moves_to_parent != NULL)
        {
            new_state->moves_to_parent = 
                freecell_solver_move_stack_compact_allocate(
                    hard_thread, 
                    new_state->moves_to_parent
                    );
        }

        return FCS_STATE_DOES_NOT_EXIST;
    }
    else
    {
        return FCS_STATE_ALREADY_EXISTS;
    }
}



/*
 * This implementation crashes for some reason, so don't use it.
 *
 * */


#if 0

static char meaningless_data[16] = "Hello World!";

int freecell_solver_check_and_add_state(freecell_solver_instance_t * instance, fcs_state_with_locations_t * new_state, int depth)
{
    DBT key, value;

    if ((instance->max_num_times >= 0) &&
        (instance->max_num_times <= instance->num_times))
    {
        return FCS_STATE_EXCEEDS_MAX_NUM_TIMES;
    }

    if ((instance->max_depth >= 0) &&
        (instance->max_depth <= depth))
    {
        return FCS_STATE_EXCEEDS_MAX_DEPTH;
    }

    fcs_canonize_state(new_state, instance->freecells_num, instance->stacks_num);

    freecell_solver_cache_stacks(instance, new_state);

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
         * The value should be non-NULL or else g_hash_table_lookup() will
         * return NULL even if it exists. */

        value.data = meaningless_data;
        value.size = 8;
        instance->db->put(
            instance->db,
            NULL,
            &key,
            &value,
            0);
        if (freecell_solver_solve_for_state(instance, new_state, depth+1,0) == FCS_STATE_WAS_SOLVED)
        {
            return FCS_STATE_WAS_SOLVED;
        }
        else
        {
            return FCS_STATE_IS_NOT_SOLVEABLE;
        }
    }
    else
    {
        /* free (value.data) ; */
        return FCS_STATE_ALREADY_EXISTS;
    }
}


#endif

#endif /* #ifndef __CAAS_C */
