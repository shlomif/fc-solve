/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// state.h - header file for state functions and macros for Freecell Solver
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include "fcs_conf.h"

#ifdef FCS_WITH_MOVES
#include "freecell-solver/fcs_move.h"
#endif

#include "freecell-solver/fcs_enums.h"

#include "rinutils.h"
#include "game_type_limit.h"

#include "internal_move_struct.h"
#include "indirect_buffer.h"

#if MAX_NUM_INITIAL_CARDS_IN_A_STACK + 12 > (MAX_NUM_DECKS * 52)
#define MAX_NUM_CARDS_IN_A_STACK (MAX_NUM_DECKS * 52)
#else
#define MAX_NUM_CARDS_IN_A_STACK (MAX_NUM_INITIAL_CARDS_IN_A_STACK + 12)
#endif

#ifndef FCS_MAX_NUM_SCANS_BUCKETS
#define FCS_MAX_NUM_SCANS_BUCKETS 4
#endif

#define FCS_NUM_SUITS 4

#define FCS_CHAR_BIT_SIZE_LOG2 3
#define MAX_NUM_SCANS (FCS_MAX_NUM_SCANS_BUCKETS * (sizeof(unsigned char) * 8))

#define is_scan_visited(ptr_state, scan_id)                                    \
    ((FCS_S_SCAN_VISITED(ptr_state))[(scan_id) >> FCS_CHAR_BIT_SIZE_LOG2] &    \
        (1 << ((scan_id) & ((1 << (FCS_CHAR_BIT_SIZE_LOG2)) - 1))))

typedef char fcs_card;
typedef fcs_card *fcs_cards_column;
typedef const fcs_card *fcs_const_cards_column;
typedef fcs_card fcs_state_foundation;

#ifdef COMPACT_STATES
/*
 * Card:
 * Bits 0-3 - Card Number
 * Bits 4-5 - Suit
 *
 */

typedef struct
{
    fcs_card data[MAX_NUM_STACKS * (MAX_NUM_CARDS_IN_A_STACK + 1) +
                  MAX_NUM_FREECELLS + 4 * MAX_NUM_DECKS];
} fcs_state;

/*
 * Stack: 0 - Number of cards
 *        1-19 - Cards
 * Stacks: stack_num*20 where stack_num >= 0 and
 *         stack_num <= (MAX_NUM_STACKS-1)
 * Bytes: (MAX_NUM_STACKS*20) to
 *      (MAX_NUM_STACKS*20+MAX_NUM_FREECELLS-1)
 *      are Freecells.
 * Bytes: (MAX_NUM_STACKS*20+MAX_NUM_FREECELLS) to
 *      MAX_NUM_STACKS*20+MAX_NUM_FREECELLS+3
 *      are Foundations.
 * */

typedef char fcs_locs_type;

#define fcs_state_get_col(state, col_idx)                                      \
    ((state).data + ((col_idx) * (MAX_NUM_CARDS_IN_A_STACK + 1)))
#define fcs_freecell_card(state, f)                                            \
    ((state).data[(MAX_NUM_STACKS * (MAX_NUM_CARDS_IN_A_STACK + 1)) + (f)])
#define fcs_foundation_value(state, d)                                         \
    ((state).data[((MAX_NUM_STACKS * (MAX_NUM_CARDS_IN_A_STACK + 1)) +         \
                      MAX_NUM_FREECELLS) +                                     \
                  (d)])

#elif defined(INDIRECT_STACK_STATES) // #ifdef COMPACT_STATES
typedef struct
{
    fcs_cards_column columns[MAX_NUM_STACKS];
#if MAX_NUM_FREECELLS > 0
    fcs_card freecells[MAX_NUM_FREECELLS];
#endif
    fcs_state_foundation foundations[MAX_NUM_DECKS * 4];
} fcs_state;

#define fcs_state_get_col(state, col_idx) ((state).columns[(col_idx)])

#define fcs_freecell_card(state, f) ((state).freecells[(f)])

#define fcs_foundation_value(state, d) ((state).foundations[(d)])

#define fcs_copy_stack(state_key, state_val, idx, buffer)                      \
    {                                                                          \
        if (!((state_val).stacks_copy_on_write_flags & (1 << idx)))            \
        {                                                                      \
            (state_val).stacks_copy_on_write_flags |= (1 << idx);              \
            const_AUTO(copy_stack_col, fcs_state_get_col((state_key), idx));   \
            memcpy(&buffer[idx << 7], copy_stack_col,                          \
                fcs_col_len(copy_stack_col) + 1);                              \
            fcs_state_get_col((state_key), idx) = &buffer[idx << 7];           \
        }                                                                      \
    }

#define fcs_duplicate_state_extra(dest_info)                                   \
    {                                                                          \
        (dest_info).stacks_copy_on_write_flags = 0;                            \
    }

typedef uint8_t fcs_locs_type;

#else
#error Neither COMPACT_STATES nor INDIRECT_STACK_STATES are defined.
#endif /* #if defined COMPACT_STATES -                                         \
          #elif defined INDIRECT_STACK_STATES                                  \
        */

// These are macros or functions that are common to all the _STATES types.
#define fcs_increment_foundation(state, foundation_idx)                        \
    (++(fcs_foundation_value((state), (foundation_idx))))
#define fcs_set_foundation(state, foundation_idx, value)                       \
    ((fcs_foundation_value((state), (foundation_idx))) =                       \
            (fcs_state_foundation)(value))
#define fcs_col_pop_top(col)                                                   \
    (fcs_col_get_card((col), (--fcs_col_len(col))) = fc_solve_empty_card)
#define fcs_col_pop_card(col, into)                                            \
    {                                                                          \
        into = fcs_col_get_card((col), (fcs_col_len(col) - 1));                \
        fcs_col_pop_top(col);                                                  \
    }
#define fcs_col_push_card(col, from)                                           \
    fcs_col_get_card((col), ((fcs_col_len(col))++)) = (from)
#define fcs_freecell_is_empty(state, idx)                                      \
    (fcs_card_is_empty(fcs_freecell_card(state, idx)))

#if defined(COMPACT_STATES)

#define fcs_duplicate_state_extra(dest_info)
#define fcs_copy_stack(state_key, state_val, idx, buffer)

#endif

#define fcs_put_card_in_freecell(state, f, card)                               \
    (fcs_freecell_card((state), (f)) = (card))

#define fcs_empty_freecell(state, f)                                           \
    fcs_put_card_in_freecell((state), (f), fc_solve_empty_card)

#define fcs_card_is_empty(card) ((card) == 0)
#define fcs_card_is_valid(card) ((card) != 0)

static inline fcs_card fcs_make_card(const int rank, const int suit)
{
    return (fcs_card)((((fcs_card)rank) << 2) | ((fcs_card)suit));
}

#define fcs_card2char(c) (c)
#define fcs_char2card(c) (c)

#define fcs_col_len(col) (((col)[0]))
#define fcs_col_get_card(col, card_idx) ((col)[(card_idx) + 1])
#define fcs_state_col_len(s, i) fcs_col_len(fcs_state_get_col((s), (i)))
#define fcs_state_col_is_empty(s, i) (fcs_state_col_len((s), (i)) == 0)

#define fcs_card_rank(card) ((card) >> 2)
#define fcs_card_suit(card) ((card)&0x03)

static inline fcs_card fcs_col_get_rank(
    const fcs_const_cards_column col, const int card_idx)
{
    return fcs_card_rank(fcs_col_get_card(col, card_idx));
}
#define FCS_RANK_KING FCS_MAX_RANK
#include "is_king.h"
static inline bool fcs_card_is_king(const fcs_card card)
{
    return fc_solve_is_king_buf[(size_t)card];
}

static inline bool fcs_col_is_king(
    const fcs_const_cards_column col, const int card_idx)
{
    return fcs_card_is_king(fcs_col_get_card(col, card_idx));
}

struct fcs_state_keyval_pair_struct;

/*
 * NOTE: the order of elements here is intended to reduce framgmentation
 * and memory consumption. Namely:
 *
 * 1. Pointers come first.
 *
 * 2. ints (32-bit on most platform) come next.
 *
 * 3. chars come next.
 *
 * */
struct fcs_state_extra_info_struct
{
#ifdef FCS_RCS_STATES
    struct fcs_state_extra_info_struct *parent;
#else
    struct fcs_state_keyval_pair_struct *parent;
#endif
#ifdef FCS_WITH_MOVES
    fcs_move_stack *moves_to_parent;
#endif

#ifndef FCS_WITHOUT_DEPTH_FIELD
    int depth;
#endif

#ifndef FCS_WITHOUT_VISITED_ITER
    /*
     * The iteration in which this state was marked as visited
     * */
    fcs_int_limit_t visited_iter;
#endif

    /*
     * This is the number of direct children of this state which were not
     * yet declared as dead ends. Once this counter reaches zero, this
     * state too is declared as a dead end.
     *
     * It was converted to an unsigned short , because it is extremely
     * unlikely that a state will have more than 64K active children.
     * */
    unsigned short num_active_children;

    /*
     * This field contains global, scan-independant flags, which are used
     * from the FCS_VISITED_* enum below.
     *
     * FCS_VISITED_IN_SOLUTION_PATH - indicates that the state is in the
     * solution path found by the scan. (used by the optimization scan)
     *
     * FCS_VISITED_IN_OPTIMIZED_PATH - indicates that the state is in the
     * optimized solution path which is computed by the optimization scan.
     *
     * FCS_VISITED_DEAD_END - indicates that the state does not lead to
     * anywhere useful, and scans should not examine it in the first place.
     *
     * FCS_VISITED_GENERATED_BY_PRUNING - indicates that the state was
     * generated by pruning, so one can skip calling the pruning function
     * for it.
     * */
    fcs_game_limit visited;

    /*
     * This is a vector of flags - one for each scan. Each indicates whether
     * its scan has already visited this state
     * */
    unsigned char scan_visited[FCS_MAX_NUM_SCANS_BUCKETS];

#ifdef INDIRECT_STACK_STATES
    /*
     * A vector of flags that indicates which columns were already copied.
     * */
    int stacks_copy_on_write_flags;
#endif
};

typedef struct
{
    /*
     * These contain the location of the original columns and freecells
     * in the permutation of them. They are sorted by the canonization
     * function.
     * */
    fcs_locs_type stack_locs[MAX_NUM_STACKS];
#if MAX_NUM_FREECELLS > 0
    fcs_locs_type fc_locs[MAX_NUM_FREECELLS];
#endif
} fcs_state_locs_struct;

static inline void fc_solve_init_locs(fcs_state_locs_struct *const locs)
{
    for (int i = 0; i < MAX_NUM_STACKS; ++i)
    {
        locs->stack_locs[i] = (fcs_locs_type)i;
    }
#if MAX_NUM_FREECELLS > 0
    for (int i = 0; i < MAX_NUM_FREECELLS; ++i)
    {
        locs->fc_locs[i] = (fcs_locs_type)i;
    }
#endif
}

typedef struct fcs_state_extra_info_struct fcs_state_extra_info;

struct fcs_state_keyval_pair_struct
{
    union {
        struct
        {
            fcs_state s;
            fcs_state_extra_info info;
        };
        struct fcs_state_keyval_pair_struct *next;
    };
};

typedef struct fcs_state_keyval_pair_struct fcs_state_keyval_pair;

typedef struct
{
    fcs_state *key;
    fcs_state_extra_info *val;
} fcs_kv_state;

static inline fcs_kv_state FCS_STATE_keyval_pair_to_kv(
    fcs_state_keyval_pair *const s)
{
    return (const fcs_kv_state){.key = &(s->s), .val = &(s->info)};
}

/* Always the same. */
#define fcs_duplicate_kv_state(ptr_dest, ptr_src)                              \
    {                                                                          \
        *((ptr_dest)->key) = *((ptr_src)->key);                                \
        *((ptr_dest)->val) = *((ptr_src)->val);                                \
        fcs_duplicate_state_extra(*((ptr_dest)->val));                         \
    }

// This type is the struct that is collectible inside the hash.
//
// In FCS_RCS_STATES we only collect the extra_info's and the state themselves
// are kept in an LRU cache because they can be calculated from the
// extra_infos and the original state by applying the moves.
#ifdef FCS_RCS_STATES
typedef fcs_state_extra_info fcs_collectible_state;
#define FCS_S_ACCESSOR(s, field) ((s)->field)

#define fcs_duplicate_state(x, y) fcs_duplicate_kv_state((x), (y))

#define FCS_STATE_keyval_pair_to_collectible(s) (&((s)->info))
static inline fcs_collectible_state *FCS_STATE_kv_to_collectible(
    fcs_kv_state *const s)
{
    return s->val;
}

static inline void FCS_STATE_collectible_to_kv(
    fcs_kv_state *const ret, fcs_collectible_state *const s)
{
    ret->val = s;
}

#else

typedef fcs_state_keyval_pair fcs_collectible_state;

#define FCS_S_ACCESSOR(s, field) (((s)->info).field)

#define fcs_duplicate_state(ptr_dest, ptr_src)                                 \
    {                                                                          \
        *(ptr_dest) = *(ptr_src);                                              \
        fcs_duplicate_state_extra((ptr_dest)->info);                           \
    }

#define FCS_STATE_keyval_pair_to_collectible(s) (s)
static inline fcs_collectible_state *FCS_STATE_kv_to_collectible(
    fcs_kv_state *const s)
{
    return (fcs_collectible_state *)(s->key);
}

static inline void FCS_STATE_collectible_to_kv(
    fcs_kv_state *const ret, fcs_collectible_state *const s)
{
    *ret = FCS_STATE_keyval_pair_to_kv(s);
}

#endif

#define FCS_S_NEXT(s) FCS_S_ACCESSOR(s, parent)
#define FCS_S_PARENT(s) FCS_S_ACCESSOR(s, parent)
#define FCS_S_NUM_ACTIVE_CHILDREN(s) FCS_S_ACCESSOR(s, num_active_children)
#define FCS_S_MOVES_TO_PARENT(s) FCS_S_ACCESSOR(s, moves_to_parent)
#define FCS_S_VISITED(s) FCS_S_ACCESSOR(s, visited)
#define FCS_S_SCAN_VISITED(s) FCS_S_ACCESSOR(s, scan_visited)
#ifndef FCS_WITHOUT_DEPTH_FIELD
#define FCS_S_DEPTH(s) FCS_S_ACCESSOR(s, depth)
#endif
#ifndef FCS_WITHOUT_VISITED_ITER
#define FCS_S_VISITED_ITER(s) FCS_S_ACCESSOR(s, visited_iter)
#endif

#define fc_solve_empty_card ((fcs_card)0)

#ifdef HARD_CODED_NUM_FREECELLS
#define PASS_FREECELLS(arg)
#define FREECELLS_NUM__VAL HARD_CODED_NUM_FREECELLS
#else
#define PASS_FREECELLS(arg) , arg
#define FREECELLS_NUM__VAL freecells_num
#endif

#define FREECELLS_NUM__ARG PASS_FREECELLS(const size_t freecells_num)

#ifdef HARD_CODED_NUM_STACKS
#define PASS_STACKS(arg)
#define STACKS_NUM__VAL HARD_CODED_NUM_STACKS
#else
#define PASS_STACKS(arg) , arg
#define STACKS_NUM__VAL stacks_num
#endif

#define STACKS_NUM__ARG PASS_STACKS(const size_t stacks_num)

#ifdef HARD_CODED_NUM_DECKS
#define PASS_DECKS(arg)
#define DECKS_NUM__VAL HARD_CODED_NUM_DECKS
#else
#define PASS_DECKS(arg) , arg
#define DECKS_NUM__VAL decks_num
#endif

#define FREECELLS_AND_STACKS_ARGS() FREECELLS_NUM__ARG STACKS_NUM__ARG
#define FREECELLS_STACKS_DECKS__ARGS()                                         \
    FREECELLS_AND_STACKS_ARGS() PASS_DECKS(const size_t decks_num)

#define PASS_T(arg) FC_SOLVE__PASS_T(arg)

extern void fc_solve_canonize_state(
    fcs_state *const ptr_state_key FREECELLS_AND_STACKS_ARGS());

void fc_solve_canonize_state_with_locs(fcs_state *const ptr_state_key,
    fcs_state_locs_struct *const locs FREECELLS_AND_STACKS_ARGS());

#if (FCS_STATE_STORAGE != FCS_STATE_STORAGE_LIBREDBLACK_TREE)
typedef void *fcs_compare_context;
#else
typedef const void *fcs_compare_context;
#endif

static inline int fc_solve_state_compare(
    const void *const s1, const void *const s2)
{
    return memcmp(s1, s2, sizeof(fcs_state));
}

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
extern int fc_solve_state_compare_equal(const void *, const void *);
#endif
extern int fc_solve_state_compare_with_context(
    const void *, const void *, fcs_compare_context);

/*
 * Convert an entire card to its user representation.
 *
 * */
extern void fc_solve_card_stringify(
    const fcs_card card, char *const str PASS_T(const bool t));

#ifdef FC_SOLVE__STRICTER_BOARD_PARSING
#define FC_SOLVE_MAP_CHAR(c) (c)
#else
#define FC_SOLVE_MAP_CHAR(c) (toupper(c))
#endif

/*
 * This function converts a string containing a suit letter (that is
 * one of H,S,D,C) into its suit ID.
 *
 * The suit letter may come somewhat after the beginning of the string.
 *
 * */
static inline __attribute__((pure)) int fcs_str2suit(const char *suit)
{
    while (TRUE)
    {
        switch (FC_SOLVE_MAP_CHAR(*suit))
        {
        case 'H':
        case ' ':
        case '\0':
            return 0;
        case 'C':
            return 1;
        case 'D':
            return 2;
        case 'S':
            return 3;
        default:
            ++suit;
        }
    }
}

/*
 * This function converts a card number from its user representation
 * (e.g: "A", "K", "9") to its card number that can be used by
 * the program.
 * */

static inline __attribute__((pure)) int fcs_str2rank(const char *string)
{
    while (1)
    {
        switch (FC_SOLVE_MAP_CHAR(*string))
        {
        case '\0':
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return 0;
        case 'A':
            return 1;
        case 'J':
            return 11;
        case 'Q':
            return 12;
        case 'K':
            return 13;
#ifndef FC_SOLVE__STRICTER_BOARD_PARSING
        case '1':
            return (string[1] == '0') ? 10 : 1;
#endif
        case 'T':
#ifndef FC_SOLVE__STRICTER_BOARD_PARSING
        case '0':
#endif
            return 10;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        default:
            ++string;
        }
    }
}
/*
 * This function converts an entire card from its string representations
 * (e.g: "AH", "KS", "8D"), to a fcs_card data type.
 * */
static inline __attribute__((pure)) fcs_card fc_solve_card_parse_str(
    const char *const str)
{
    return fcs_make_card(fcs_str2rank(str), fcs_str2suit(str));
}

#ifdef INDIRECT_STACK_STATES
#define fc_solve_state_init(state, stacks_num, indirect_stacks_buffer)         \
    fc_solve_state_init_proto(                                                 \
        state PASS_STACKS(stacks_num), indirect_stacks_buffer)
#else
#define fc_solve_state_init(state, stacks_num, indirect_stacks_buffer)         \
    fc_solve_state_init_proto(state PASS_STACKS(stacks_num))
#endif

static inline void fc_solve_state_init_proto(fcs_state_keyval_pair *const state
        STACKS_NUM__ARG GCC_UNUSED IND_BUF_T_PARAM(indirect_stacks_buffer))
{
    memset(&(state->s), 0, sizeof(state->s));
#ifdef INDIRECT_STACK_STATES
    size_t i;
    for (i = 0; i < STACKS_NUM__VAL; ++i)
    {
        memset(state->s.columns[i] = &indirect_stacks_buffer[i << 7], '\0',
            MAX_NUM_DECKS * 52 + 1);
    }
    for (; i < MAX_NUM_STACKS; ++i)
    {
        state->s.columns[i] = NULL;
    }
#endif
    state->info.parent = NULL;
#ifdef FCS_WITH_MOVES
    state->info.moves_to_parent = NULL;
#endif
#ifndef FCS_WITHOUT_DEPTH_FIELD
    state->info.depth = 0;
#endif
    state->info.visited = 0;
#ifndef FCS_WITHOUT_VISITED_ITER
    state->info.visited_iter = 0;
#endif
    state->info.num_active_children = 0;
    memset(state->info.scan_visited, '\0', sizeof(state->info.scan_visited));
#ifdef INDIRECT_STACK_STATES
    state->info.stacks_copy_on_write_flags = 0;
#endif
}

#ifdef FCS_BREAK_BACKWARD_COMPAT_1
#define FCS_PARSE_try_prefix(str, p, p_s) try_str_prefix(str, p)
#else
#if MAX_NUM_FREECELLS > 0
static const char *const fc_solve_freecells_prefixes[] = {
    "FC:", "Freecells:", "Freecell:", NULL};
#endif

static const char *const fc_solve_foundations_prefixes[] = {"Decks:", "Deck:",
    "Founds:", "Foundations:", "Foundation:", "Found:", NULL};

static inline __attribute__((pure)) const char *fc_solve__try_prefixes(
    const char *const str, const char *const *const prefixes)
{
    for (const char *const *prefix = prefixes; (*prefix); ++prefix)
    {
        if (!strncasecmp(str, (*prefix), strlen(*prefix)))
        {
            return str + strlen(*prefix);
        }
    }
    return NULL;
}
#define FCS_PARSE_try_prefix(str, p, p_s) fc_solve__try_prefixes(str, p_s)
#endif
#if defined(WIN32) && (!defined(HAVE_STRNCASECMP))
#ifndef strncasecmp
#define strncasecmp(a, b, c) (strnicmp((a), (b), (c)))
#endif
#endif

#define fc_solve_initial_user_state_to_c(string, out_state, freecells_num,     \
    stacks_num, decks_num, indirect_stacks_buffer)                             \
    fc_solve_initial_user_state_to_c_proto(string,                             \
        out_state PASS_FREECELLS(freecells_num) PASS_STACKS(stacks_num)        \
            PASS_DECKS(decks_num) PASS_IND_BUF_T(indirect_stacks_buffer))

static inline bool fc_solve_initial_user_state_to_c_proto(
    const char *const string,
    fcs_state_keyval_pair *const out_state FREECELLS_STACKS_DECKS__ARGS()
        IND_BUF_T_PARAM(indirect_stacks_buffer))
{
    fc_solve_state_init(out_state, STACKS_NUM__VAL, indirect_stacks_buffer);
    const char *str = string;

    bool first_line = TRUE;

#define out (out_state->s)
/* Handle the end of string - shouldn't happen */
#define HANDLE_EOS()                                                           \
    {                                                                          \
        if ((*str) == '\0')                                                    \
        {                                                                      \
            return FALSE;                                                      \
        }                                                                      \
    }

    for (size_t s = 0; s < STACKS_NUM__VAL; s++)
    {
        /* Move to the next stack */
        if (!first_line)
        {
            while ((*str) != '\n')
            {
                HANDLE_EOS();
                ++str;
            }
            ++str;
        }

        first_line = FALSE;

#if MAX_NUM_FREECELLS > 0
        const_AUTO(new_str, FCS_PARSE_try_prefix(str,
                                "Freecells:", fc_solve_freecells_prefixes));
        if (new_str)
        {
            str = new_str;
            for (size_t c = 0; c < FREECELLS_NUM__VAL; ++c)
            {
                fcs_empty_freecell(out, c);
            }
            for (size_t c = 0; c < FREECELLS_NUM__VAL; ++c)
            {
                if (c != 0)
                {
                    while (((*str) != ' ') && ((*str) != '\t') &&
                           ((*str) != '\n') && ((*str) != '\r'))
                    {
                        HANDLE_EOS();
                        ++str;
                    }
                    if ((*str == '\n') || (*str == '\r'))
                    {
                        break;
                    }
                    ++str;
                }

                while ((*str == ' ') || (*str == '\t'))
                {
                    ++str;
                }
                if ((*str == '\r') || (*str == '\n'))
                    break;

                fcs_put_card_in_freecell(out, c,
                    ((*str == '*') || (*str == '-')) ? fc_solve_empty_card : ({
                        const char rank = fcs_str2rank(str);
                        if (!rank)
                        {
                            return FALSE;
                        }
                        fcs_make_card(rank, fcs_str2suit(str));
                    }));
            }

            while (*str != '\n')
            {
                HANDLE_EOS();
                ++str;
            }
            --s;
            continue;
        }
#endif

        const_AUTO(new_str2, FCS_PARSE_try_prefix(str, "Foundations:",
                                 fc_solve_foundations_prefixes));
        if (new_str2)
        {
            str = new_str2;
            for (size_t f_idx = 0; f_idx < (DECKS_NUM__VAL << 2); f_idx++)
            {
                fcs_set_foundation(out, f_idx, 0);
            }

            size_t decks_index[4] = {0, 0, 0, 0};
            while (1)
            {
                while ((*str == ' ') || (*str == '\t'))
                    ++str;
                if ((*str == '\n') || (*str == '\r'))
                    break;
                const int f_idx = fcs_str2suit(str);
                ++str;
                while (*str == '-')
                    ++str;
                /* Workaround for fcs_str2rank's willingness
                 * to designate the string '0' as 10. */
                const int c = ((str[0] == '0') ? 0 : fcs_str2rank(str));
                while ((*str != ' ') && (*str != '\t') && (*str != '\n') &&
                       (*str != '\r'))
                {
                    HANDLE_EOS();
                    ++str;
                }

                fcs_set_foundation(out, ((decks_index[f_idx] << 2) + f_idx), c);
                if ((++decks_index[f_idx]) >= DECKS_NUM__VAL)
                {
                    decks_index[f_idx] = 0;
                }
            }
            s--;
            continue;
        }

        /* Handle columns that start with an initial colon, so we can
         * input states from -p -t mid-play. */
        if ((*str) == ':')
        {
            ++str;
        }

        var_AUTO(col, fcs_state_get_col(out, s));
        for (int c = 0; c < MAX_NUM_CARDS_IN_A_STACK; c++)
        {
            /* Move to the next card */
            if (c != 0)
            {
                while (((*str) != ' ') && ((*str) != '\t') &&
                       ((*str) != '\n') && ((*str) != '\r'))
                {
                    HANDLE_EOS();
                    ++str;
                }
                if ((*str == '\n') || (*str == '\r'))
                {
                    break;
                }
            }

            while ((*str == ' ') || (*str == '\t'))
            {
                ++str;
            }
            HANDLE_EOS();
            if ((*str == '\n') || (*str == '\r'))
            {
                break;
            }
            const_AUTO(my_card, fc_solve_card_parse_str(str));
            if (!fcs_card_rank(my_card))
            {
                return FALSE;
            }
            fcs_col_push_card(col, my_card);
        }
    }

    return TRUE;
}

#undef out
#undef HANDLE_EOS

extern void fc_solve_state_as_string(char *output_s,
    const fcs_state *const state,
    const fcs_state_locs_struct *const state_locs FREECELLS_STACKS_DECKS__ARGS()
        FC_SOLVE__PASS_PARSABLE(const bool parseable_output),
    const bool canonized_order_output PASS_T(const bool display_10_as_t));

typedef enum
{
    FCS_STATE_VALIDITY__OK = 0,
    FCS_STATE_VALIDITY__MISSING_CARD = 1,
    FCS_STATE_VALIDITY__EXTRA_CARD = 2,
    FCS_STATE_VALIDITY__EMPTY_SLOT = 3,
    FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT = 4
} state_validity_ret;

#ifndef FCS_DISABLE_STATE_VALIDITY_CHECK
static inline state_validity_ret fc_solve_check_state_validity(
    const fcs_state_keyval_pair *const state_pair
        FREECELLS_STACKS_DECKS__ARGS(),
    fcs_card *const misplaced_card)
{
    size_t card_counts[FCS_NUM_SUITS][FCS_MAX_RANK + 1];

    const fcs_state *const state = &(state_pair->s);

    /* Initialize all card_counts to 0 */
    for (int suit_idx = 0; suit_idx < FCS_NUM_SUITS; suit_idx++)
    {
        for (int c = 1; c <= FCS_MAX_RANK; c++)
        {
            card_counts[suit_idx][c] = 0;
        }
    }

    /* Mark the card_counts in the decks */
    for (size_t suit_idx = 0; suit_idx < (DECKS_NUM__VAL << 2); suit_idx++)
    {
        for (int c = 1; c <= fcs_foundation_value(*state, suit_idx); c++)
        {
            card_counts[suit_idx % FCS_NUM_SUITS][c]++;
        }
    }

#if MAX_NUM_FREECELLS > 0
    // Mark the card_counts in the freecells
    for (size_t f = 0; f < FREECELLS_NUM__VAL; f++)
    {
        const fcs_card card = fcs_freecell_card(*state, f);
        if (fcs_card_is_valid(card))
        {
            card_counts[fcs_card_suit(card)][fcs_card_rank(card)]++;
        }
    }
#endif

    /* Mark the card_counts in the columns */
    for (size_t s = 0; s < STACKS_NUM__VAL; s++)
    {
        const_AUTO(col, fcs_state_get_col(*state, s));
        const int col_len = fcs_col_len(col);
        for (int c = 0; c < col_len; c++)
        {
            const fcs_card card = fcs_col_get_card(col, c);
            if (fcs_card_is_empty(card))
            {
                *misplaced_card = fc_solve_empty_card;
                return FCS_STATE_VALIDITY__EMPTY_SLOT;
            }
            card_counts[fcs_card_suit(card)][fcs_card_rank(card)]++;
        }
    }

    /* Now check if there are extra or missing card_counts */

    for (size_t suit_idx = 0; suit_idx < FCS_NUM_SUITS; suit_idx++)
    {
        for (size_t rank = 1; rank <= FCS_MAX_RANK; rank++)
        {
            if (card_counts[suit_idx][rank] != DECKS_NUM__VAL)
            {
                *misplaced_card = fcs_make_card(rank, suit_idx);
                return ((card_counts[suit_idx][rank] < DECKS_NUM__VAL)
                            ? FCS_STATE_VALIDITY__MISSING_CARD
                            : FCS_STATE_VALIDITY__EXTRA_CARD);
            }
        }
    }

    return FCS_STATE_VALIDITY__OK;
}
#endif

#undef state

#ifdef __cplusplus
}
#endif

enum
{
    FCS_VISITED_IN_SOLUTION_PATH = 0x1,
    FCS_VISITED_IN_OPTIMIZED_PATH = 0x2,
    FCS_VISITED_DEAD_END = 0x4,
    FCS_VISITED_ALL_TESTS_DONE = 0x8,
    FCS_VISITED_GENERATED_BY_PRUNING = 0x10,
};

static inline int fc_solve_card_compare(const fcs_card c1, const fcs_card c2)
{
    return (c1) - (c2);
}

#ifdef INDIRECT_STACK_STATES
static inline int fc_solve_stack_compare_for_comparison(
    const void *const v_s1, const void *const v_s2)
{
    const fcs_card *const s1 = (const fcs_card *const)v_s1;
    const fcs_card *const s2 = (const fcs_card *const)v_s2;

    {
        const int min_len = min(s1[0], s2[0]);
        for (int a = 1; a <= min_len; a++)
        {
            const int ret = fc_solve_card_compare(s1[a], s2[a]);
            if (ret != 0)
            {
                return ret;
            }
        }
    }
    /*
     * The reason I do the stack length comparisons after the card-by-card
     * comparison is to maintain correspondence with
     * fcs_stack_compare_for_stack_sort, and with the one card comparison
     * of the other state representation mechanisms.
     * */
    /* For some reason this code is faster than s1[0]-s2[0] */
    if (s1[0] < s2[0])
    {
        return -1;
    }
    else if (s1[0] > s2[0])
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
#endif

static inline void set_scan_visited(
    fcs_collectible_state *const ptr_state, const int scan_id)
{
    (FCS_S_SCAN_VISITED(ptr_state))[scan_id >> FCS_CHAR_BIT_SIZE_LOG2] |=
        (1 << ((scan_id) & ((1 << (FCS_CHAR_BIT_SIZE_LOG2)) - 1)));
}

/*
 * This macro determines if child can be placed above parent.
 *
 * The variable sequences_are_built_by has to be initialized to
 * the sequences_are_built_by member of the instance.
 *
 * */

#ifdef FCS_FREECELL_ONLY

#include "is_parent.h"
static inline bool fcs_is_parent_card(
    const fcs_card child, const fcs_card parent)
{
    return fc_solve_is_parent_buf[(size_t)parent][(size_t)child];
}

#else

static inline bool fcs_is_parent_card__helper(const fcs_card child,
    const fcs_card parent, const int sequences_are_built_by)
{
    return ((fcs_card_rank(child) + 1 == fcs_card_rank(parent)) &&
            ((sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK)
                    ? TRUE
                    : ((sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT)
                              ? (fcs_card_suit(child) == fcs_card_suit(parent))
                              : ((fcs_card_suit(child) & 0x1) !=
                                    (fcs_card_suit(parent) & 0x1)))));
}
#define fcs_is_parent_card(child, parent)                                      \
    fcs_is_parent_card__helper(child, parent, sequences_are_built_by)

#endif

#define FCS_STATE__DUP_keyval_pair(dest, src)                                  \
    {                                                                          \
        (dest) = (src);                                                        \
        fcs_duplicate_state_extra((dest).info);                                \
    }

static inline void fcs_col_transfer_cards(
    fcs_cards_column dest_col, fcs_cards_column src_col, const int cards_num)
{
    fcs_card *const src_cards_ptr =
        &fcs_col_get_card(src_col, (fcs_col_len(src_col) -= cards_num));
    const size_t cards_size = (((size_t)cards_num) * sizeof(fcs_card));
    memcpy(&fcs_col_get_card(dest_col, fcs_col_len(dest_col)), src_cards_ptr,
        cards_size);
    fcs_col_len(dest_col) += cards_num;
    memset(src_cards_ptr, 0, cards_size);
}

static inline fcs_card fcs_state_pop_col_card(
    fcs_state *const state, const int col_idx)
{
    var_AUTO(col, fcs_state_get_col(*state, col_idx));
    fcs_card ret;
    fcs_col_pop_card(col, ret);
    return ret;
}

static inline void fcs_state_pop_col_top(
    fcs_state *const state, const int col_idx)
{
    var_AUTO(col, fcs_state_get_col(*state, col_idx));
    fcs_col_pop_top(col);
}

static inline void fcs_state_push(
    fcs_state *const state, const int col_idx, const fcs_card card)
{
    var_AUTO(col, fcs_state_get_col(*state, col_idx));
    fcs_col_push_card(col, card);
}

#ifdef FCS_FREECELL_ONLY
#define FCS__SEQS_ARE_BUILT_BY_RANK() FALSE
#else
#define FCS__SEQS_ARE_BUILT_BY_RANK()                                          \
    (sequences_are_built_by == FCS_SEQ_BUILT_BY_RANK)
#endif
