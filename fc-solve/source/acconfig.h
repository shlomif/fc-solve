/*
    config.h - Configuration file for Freecell Solver

    Written by Shlomi Fish, 2000

    This file is distributed under the public domain.
    (It is not copyrighted).
*/

#ifndef __CONFIG_H
#define __CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#undef DEBUG_STATES
#undef COMPACT_STATES
#undef INDIRECT_STACK_STATES

#undef CARD_DEBUG_PRES

/*
 * Define this macro if the C compiler supports the keyword inline or
 * a similar keyword that was found by Autoconf (and defined as inline).
 * */
#undef HAVE_C_INLINE


/*
    The sort margin size for the previous states array.
*/
#define PREV_STATES_SORT_MARGIN 32
/*
    The amount prev_states grow by each time it each resized.
    Should be greater than 0 and in order for the program to be
    efficient, should be much bigger than
    PREV_STATES_SORT_MARGIN.
*/
#define PREV_STATES_GROW_BY 128

/*
    The amount the pack pointers array grows by. Shouldn't be too high
    because it doesn't happen too often.
*/
#define IA_STATE_PACKS_GROW_BY 32

/*
 * The maximal number of Freecells. For efficiency's sake it should be a
 * multiple of 4.
 * */

#define MAX_NUM_FREECELLS 4

/*
 * The maximal number of Stacks. For efficiency's sake it should be a
 * multiple of 4.
 * */

#define MAX_NUM_STACKS 10
/*
 * The maximal number of initial cards that can be found in a stack. The rule
 * of the thumb is that it plus 13 should be a multiple of 4.
 * */
#define MAX_NUM_INITIAL_CARDS_IN_A_STACK 8

#define MAX_NUM_DECKS 2


#define FCS_STATE_STORAGE_INDIRECT 0
#define FCS_STATE_STORAGE_INTERNAL_HASH 1
#define FCS_STATE_STORAGE_LIBAVL_AVL_TREE 2
#define FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE 3
#define FCS_STATE_STORAGE_LIBREDBLACK_TREE 4
#define FCS_STATE_STORAGE_GLIB_TREE 5
#define FCS_STATE_STORAGE_GLIB_HASH 6
#define FCS_STATE_STORAGE_DB_FILE 7

#define FCS_STACK_STORAGE_INTERNAL_HASH 0
#define FCS_STACK_STORAGE_LIBAVL_AVL_TREE 1
#define FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE 2
#define FCS_STACK_STORAGE_LIBREDBLACK_TREE 3
#define FCS_STACK_STORAGE_GLIB_TREE 4
#define FCS_STACK_STORAGE_GLIB_HASH 5

#undef FCS_STATE_STORAGE
#undef FCS_STACK_STORAGE

#define FREECELL_SOLVER_PREFIX "@prefix@"

#ifdef __cplusplus
}
#endif

#endif
