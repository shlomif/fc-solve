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

#define calculate_real_depth(ptr_state_orig)                       \
{                                                                  \
    if (1)                                                         \
    {                                                              \
        int this_real_depth = 0;                                   \
        fcs_state_with_locations_t * ptr_state = (ptr_state_orig); \
        while(ptr_state != NULL)                                   \
        {                                                          \
            ptr_state = ptr_state->parent;                         \
            this_real_depth++;                                     \
        }                                                          \
        this_real_depth--;                                         \
        ptr_state = (ptr_state_orig);                              \
        while (ptr_state->depth != this_real_depth)                \
        {                                                          \
            ptr_state->depth = this_real_depth;                    \
            this_real_depth--;                                     \
            ptr_state = ptr_state->parent;                         \
        }                                                          \
    }                                                              \
}                                                                  \
        

#ifdef __cplusplus
}
#endif

#endif
