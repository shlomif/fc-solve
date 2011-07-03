/*
    This file is generated from gen_move_funcs.pl.

    Do not edit by hand!!!
*/

#include "instance.h"
#include "freecell.h"
#include "simpsim.h"

#ifdef FCS_DISABLE_SIMPLE_SIMON
#define WRAP_SIMPSIM(f) NULL
#else
#define WRAP_SIMPSIM(f) f
#endif

fc_solve_solve_for_state_test_t fc_solve_sfs_tests[FCS_TESTS_NUM] =
{
    fc_solve_sfs_move_top_stack_cards_to_founds,
    fc_solve_sfs_move_freecell_cards_to_founds,
    fc_solve_sfs_move_freecell_cards_on_top_of_stacks,
    fc_solve_sfs_move_non_top_stack_cards_to_founds,
    fc_solve_sfs_move_stack_cards_to_different_stacks,
    fc_solve_sfs_move_stack_cards_to_a_parent_on_the_same_stack,
    fc_solve_sfs_move_sequences_to_free_stacks,
    fc_solve_sfs_move_freecell_cards_to_empty_stack,
    fc_solve_sfs_move_cards_to_a_different_parent,
    fc_solve_sfs_empty_stack_into_freecells,
    fc_solve_sfs_atomic_move_card_to_empty_stack,
    fc_solve_sfs_atomic_move_card_to_parent,
    fc_solve_sfs_atomic_move_card_to_freecell,
    fc_solve_sfs_atomic_move_freecell_card_to_parent,
    fc_solve_sfs_atomic_move_freecell_card_to_empty_stack,
    WRAP_SIMPSIM(fc_solve_sfs_simple_simon_move_sequence_to_founds),
    WRAP_SIMPSIM(fc_solve_sfs_simple_simon_move_sequence_to_true_parent),
    WRAP_SIMPSIM(fc_solve_sfs_simple_simon_move_whole_stack_sequence_to_false_parent),
    WRAP_SIMPSIM(fc_solve_sfs_simple_simon_move_sequence_to_true_parent_with_some_cards_above),
    WRAP_SIMPSIM(fc_solve_sfs_simple_simon_move_sequence_with_some_cards_above_to_true_parent),
    WRAP_SIMPSIM(fc_solve_sfs_simple_simon_move_sequence_with_junk_seq_above_to_true_parent_with_some_cards_above),
    WRAP_SIMPSIM(fc_solve_sfs_simple_simon_move_whole_stack_sequence_to_false_parent_with_some_cards_above),
    WRAP_SIMPSIM(fc_solve_sfs_simple_simon_move_sequence_to_parent_on_the_same_stack),
    WRAP_SIMPSIM(fc_solve_sfs_simple_simon_move_sequence_to_false_parent)
};


fcs_test_aliases_mapping_t fc_solve_sfs_tests_aliases[FCS_TESTS_ALIASES_NUM] =
{
    { "0", 0 },
    { "1", 1 },
    { "2", 2 },
    { "3", 3 },
    { "4", 4 },
    { "5", 5 },
    { "6", 6 },
    { "7", 7 },
    { "8", 8 },
    { "9", 9 },
    { "A", 10 },
    { "B", 11 },
    { "C", 12 },
    { "D", 13 },
    { "E", 14 },
    { "a", 15 },
    { "b", 16 },
    { "c", 17 },
    { "d", 18 },
    { "e", 19 },
    { "f", 20 },
    { "g", 21 },
    { "h", 22 },
    { "i", 23 }
};
