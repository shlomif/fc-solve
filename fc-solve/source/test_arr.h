/*
 * test_arr.h - header file for some routines and macros involving tests and 
 * the like for Freecell Solver.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2002
 *
 * This file is in the public domain (it's uncopyrighted).
 * */

#ifndef __TEST_ARR_H
#define __TEST_ARR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*freecell_solver_solve_for_state_test_t)(
        freecell_solver_soft_thread_t *,
        fcs_state_with_locations_t *,
        int,
        int,
        fcs_derived_states_list_t *,
        int
        );

extern freecell_solver_solve_for_state_test_t freecell_solver_sfs_tests[FCS_TESTS_NUM];

/*
 * This macro determines if child can be placed above parent.
 *
 * The variable sequences_are_built_by has to be initialized to
 * the sequences_are_built_by member of the instance.
 *
 * */
#define fcs_is_parent_card(child, parent) \
    ((fcs_card_card_num(child)+1 == fcs_card_card_num(parent)) && \
    ((sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK) ?   \
        1 :                                                          \
        ((sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT) ?   \
            (fcs_card_suit(child) == fcs_card_suit(parent)) :     \
            ((fcs_card_suit(child) & 0x1) != (fcs_card_suit(parent)&0x1))   \
        ))                \
    )

/*
 * This macro traces the path of the state up to the original state,
 * and thus calculates its real depth.
 *
 * It then assigns the newly updated depth throughout the path.
 * 
 * */
#define calculate_real_depth(ptr_state_orig)                       \
{                                                                  \
    if (calc_real_depth)                                           \
    {                                                              \
        int this_real_depth = 0;                                   \
        fcs_state_with_locations_t * ptr_state = (ptr_state_orig); \
        /* Count the number of states until the original state. */ \
        while(ptr_state != NULL)                                   \
        {                                                          \
            ptr_state = ptr_state->parent;                         \
            this_real_depth++;                                     \
        }                                                          \
        this_real_depth--;                                         \
        ptr_state = (ptr_state_orig);                              \
        /* Assign the new depth throughout the path*/              \
        while (ptr_state->depth != this_real_depth)                \
        {                                                          \
            ptr_state->depth = this_real_depth;                    \
            this_real_depth--;                                     \
            ptr_state = ptr_state->parent;                         \
        }                                                          \
    }                                                              \
}                                                                  \

/*
 * This macro marks a state as a dead end, and afterwards propogates
 * this information to its parent and ancestor states.
 * */
#define mark_as_dead_end(ptr_state_input) \
{      \
    if (scans_synergy)      \
    {        \
        fcs_state_with_locations_t * ptr_state = (ptr_state_input); \
        /* Mark as a dead end */        \
        ptr_state->visited |= FCS_VISITED_DEAD_END; \
        ptr_state = ptr_state->parent;          \
        if (ptr_state != NULL)                    \
        {           \
            /* Decrease the refcount of the state */    \
            ptr_state->num_active_children--;   \
            while((ptr_state->num_active_children == 0) && (ptr_state->visited & FCS_VISITED_ALL_TESTS_DONE))  \
            {          \
                /* Mark as dead end */        \
                ptr_state->visited |= FCS_VISITED_DEAD_END;  \
                /* Go to its parent state */       \
                ptr_state = ptr_state->parent;    \
                if (ptr_state == NULL)         \
                {                \
                    break;             \
                }      \
                /* Decrease the refcount */       \
                ptr_state->num_active_children--;     \
            }       \
        }   \
    }      \
}

/*
 * This macro checks if we need to terminate from running this soft
 * thread and return to the soft thread manager with an
 * FCS_STATE_SUSPEND_PROCESS
 * */
#define check_if_limits_exceeded()                                    \
    (                                                                 \
        ((instance->max_num_times >= 0) &&                            \
        (instance->num_times >= instance->max_num_times))             \
            ||                                                        \
        ((hard_thread->ht_max_num_times >= 0) &&                      \
        (hard_thread->num_times >= hard_thread->ht_max_num_times))    \
            ||                                                        \
        ((hard_thread->max_num_times >= 0) &&                         \
        (hard_thread->num_times >= hard_thread->max_num_times))       \
            ||                                                        \
        ((instance->max_num_states_in_collection >= 0) &&             \
        (instance->num_states_in_collection >=                        \
            instance->max_num_states_in_collection)                   \
        )                                                             \
    )
        

#ifdef __cplusplus
}
#endif

#endif
