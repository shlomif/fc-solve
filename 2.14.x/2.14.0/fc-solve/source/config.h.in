/*
    config.h - Configuration file for Freecell Solver

    Written by Shlomi Fish, 2000

    This file is distributed under the public domain.
    (It is not copyrighted).
*/

#ifndef FC_SOLVE__CONFIG_H
#define FC_SOLVE__CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#cmakedefine DEBUG_STATES
#cmakedefine COMPACT_STATES
#cmakedefine INDIRECT_STACK_STATES

#cmakedefine CARD_DEBUG_PRES

/*
 * Define this macro if the C compiler supports the keyword inline or
 * a similar keyword that was found by Autoconf (and defined as inline).
 * */
#cmakedefine HAVE_C_INLINE


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
#cmakedefine IA_STATE_PACKS_GROW_BY 32

/*
 * The maximal number of Freecells. For efficiency's sake it should be a
 * multiple of 4.
 * */

#define MAX_NUM_FREECELLS ${MAX_NUM_FREECELLS}

/*
 * The maximal number of Stacks. For efficiency's sake it should be a
 * multiple of 4.
 * */

#define MAX_NUM_STACKS ${MAX_NUM_STACKS}
/*
 * The maximal number of initial cards that can be found in a stack.
 * */
#define MAX_NUM_INITIAL_CARDS_IN_A_STACK ${MAX_NUM_INITIAL_CARDS_IN_A_STACK}

#define MAX_NUM_DECKS ${MAX_NUM_DECKS}


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

#define FCS_STATE_STORAGE ${FCS_STATE_STORAGE}
#define FCS_STACK_STORAGE ${FCS_STACK_STORAGE}

#ifdef __cplusplus
}
#endif

#endif

/* Define to 1 if you have the `avl' library (-lavl). */
#undef HAVE_LIBAVL

/* Define to 1 if you have the `glib' library (-lglib). */
#undef HAVE_LIBGLIB

/* Define to 1 if you have the `m' library (-lm). */
#cmakedefine HAVE_LIBM

/* Define to 1 if you have the `redblack' library (-lredblack). */
#undef HAVE_LIBREDBLACK

/* Name of package */
#define PACKAGE "${PACKAGE}"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "${PACKAGE_BUGREPORT}"

/* Define to the full name of this package. */
#define PACKAGE_NAME "${PACKAGE_NAME}"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "${PACKAGE_STRING}"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "${PACKAGE_TARNAME}"

/* Define to the version of this package. */
#define PACKAGE_VERSION "${PACKAGE_VERSION}"

/* Version number of package */
#define VERSION "${VERSION}"

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#undef inline
#endif

