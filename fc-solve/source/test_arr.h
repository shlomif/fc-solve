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
 * test_arr.h - header file for some routines and macros involving tests and
 * the like for Freecell Solver.
 *
 */

#ifndef FC_SOLVE__TEST_ARR_H
#define FC_SOLVE__TEST_ARR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*fc_solve_solve_for_state_test_t)(
        fc_solve_soft_thread_t *,
        fcs_state_extra_info_t *,
        int,
        int,
        fcs_derived_states_list_t *
        );

extern fc_solve_solve_for_state_test_t fc_solve_sfs_tests[FCS_TESTS_NUM];

#ifdef FCS_FREECELL_ONLY

#define fcs_is_parent_card(child, parent) \
    ((fcs_card_card_num(child)+1 == fcs_card_card_num(parent)) && \
            ((fcs_card_suit(child) & 0x1) != (fcs_card_suit(parent)&0x1)) \
    )

#else

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

#endif
/*
 * This macro traces the path of the state up to the original state,
 * and thus calculates its real depth.
 *
 * It then assigns the newly updated depth throughout the path.
 *
 * */
#define calculate_real_depth(ptr_state_orig_val) \
{                                                                  \
    if (calc_real_depth)                                           \
    {                                                              \
        int this_real_depth = 0;                                   \
        fcs_state_extra_info_t * temp_state_val = ptr_state_orig_val; \
        /* Count the number of states until the original state. */ \
        while(temp_state_val != NULL)                                   \
        {                                                          \
            temp_state_val = temp_state_val->parent_val;             \
            this_real_depth++;                                     \
        }                                                          \
        this_real_depth--;                                         \
        temp_state_val = (ptr_state_orig_val);                      \
        /* Assign the new depth throughout the path */             \
        while (temp_state_val->depth != this_real_depth)            \
        {                                                          \
            temp_state_val->depth = this_real_depth;                \
            this_real_depth--;                                     \
            temp_state_val = temp_state_val->parent_val;             \
        }                                                          \
    }                                                              \
}                                                                  \

/*
 * This macro marks a state as a dead end, and afterwards propogates
 * this information to its parent and ancestor states.
 * */
#define mark_as_dead_end(ptr_state_input_val) \
{      \
    if (scans_synergy)      \
    {        \
        fcs_state_extra_info_t * temp_state_val = (ptr_state_input_val); \
        /* Mark as a dead end */        \
        temp_state_val->visited |= FCS_VISITED_DEAD_END; \
        temp_state_val = temp_state_val->parent_val;          \
        if (temp_state_val != NULL)                    \
        {           \
            /* Decrease the refcount of the state */    \
            temp_state_val->num_active_children--;   \
            while((temp_state_val->num_active_children == 0) && (temp_state_val->visited & FCS_VISITED_ALL_TESTS_DONE))  \
            {          \
                /* Mark as dead end */        \
                temp_state_val->visited |= FCS_VISITED_DEAD_END;  \
                /* Go to its parent state */       \
                temp_state_val = temp_state_val->parent_val;    \
                if (temp_state_val == NULL)         \
                {                \
                    break;             \
                }      \
                /* Decrease the refcount */       \
                temp_state_val->num_active_children--;     \
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
