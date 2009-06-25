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
 * state.h - header file for state functions and macros for Freecell Solver
 *
 */
#ifndef FC_SOLVE__STATE_H
#define FC_SOLVE__STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "config.h"

#include "fcs_move.h"

#include "inline.h"

#include "card.h"

#if MAX_NUM_INITIAL_CARDS_IN_A_STACK+12>(MAX_NUM_DECKS*52)
#define MAX_NUM_CARDS_IN_A_STACK (MAX_NUM_DECKS*52)
#else
#define MAX_NUM_CARDS_IN_A_STACK (MAX_NUM_INITIAL_CARDS_IN_A_STACK+12)
#endif

#define MAX_NUM_SCANS_BUCKETS 1
#define MAX_NUM_SCANS (MAX_NUM_SCANS_BUCKETS * (sizeof(int)*8))

#define is_scan_visited(ptr_state_val, scan_id) (ptr_state_val->scan_visited[(scan_id)>>FCS_INT_BIT_SIZE_LOG2] & (1 << ((scan_id)&((1<<(FCS_INT_BIT_SIZE_LOG2))-1))))
#define set_scan_visited(ptr_state_val, scan_id) { ptr_state_val->scan_visited[(scan_id)>>FCS_INT_BIT_SIZE_LOG2] |= (1 << ((scan_id)&((1<<(FCS_INT_BIT_SIZE_LOG2))-1))); }


#ifdef DEBUG_STATES

typedef struct
{
    short card_num;
    char suit;
    char flags;
} fcs_card_t;

typedef struct
{
    int num_cards;
    fcs_card_t cards[MAX_NUM_CARDS_IN_A_STACK];
} fc_stack_t;

typedef fc_stack_t * fcs_cards_column_t;
typedef int fcs_state_foundation_t;

struct fcs_struct_state_t
{
    fc_stack_t stacks[MAX_NUM_STACKS];
    fcs_card_t freecells[MAX_NUM_FREECELLS];
    fcs_state_foundation_t foundations[MAX_NUM_DECKS*4];
#ifdef FCS_WITH_TALONS
    fcs_card_t * talon;
    char talon_params[4];
#endif
};

typedef struct fcs_struct_state_t fcs_state_t;

typedef int fcs_locs_t;

#define fcs_state_get_col(state, col_idx) \
    (&((state).stacks[(col_idx)]))

#define fcs_col_len(col) \
    ((col)->num_cards)

#define fcs_col_get_card(col, c) \
    ((col)->cards[(c)] )

#define fcs_card_card_num(card) \
    ( (card).card_num )

#define fcs_card_suit(card) \
    ((int)( (card).suit ))

#ifndef FCS_WITHOUT_CARD_FLIPPING
#define fcs_card_get_flipped(card) \
    ( (card).flags )
#endif

#define fcs_freecell_card(state, f) \
    ( (state).freecells[(f)] )

#define fcs_foundation_value(state, found) \
    ( (state).foundations[(found)] )

#define fcs_card_set_suit(card, d) \
    (card).suit = (d)

#define fcs_card_set_num(card, num) \
    (card).card_num = (num)

#ifndef FCS_WITHOUT_CARD_FLIPPING
#define fcs_card_set_flipped(card, flipped) \
    (card).flags = (flipped)
#endif

#ifdef FCS_WITH_TALONS
#define fcs_talon_len(state) \
    ((state).talon_params[0])

#define fcs_talon_pos(state) \
    ((state).talon_params[1])

#define fcs_get_talon_card(state, pos) \
    ((state).talon[pos])

#define fcs_put_card_in_talon(state, pos, card) \
    ((state).talon[pos] = (card))
#endif


#elif defined(COMPACT_STATES)    /* #ifdef DEBUG_STATES */


typedef char fcs_card_t;
typedef fcs_card_t * fcs_cards_column_t;
typedef fcs_card_t fcs_state_foundation_t;
/*
 * Card:
 * Bits 0-3 - Card Number
 * Bits 4-5 - Deck
 *
 */

struct fcs_struct_state_t
{
    fcs_card_t data[MAX_NUM_STACKS*(MAX_NUM_CARDS_IN_A_STACK+1)+MAX_NUM_FREECELLS+4*MAX_NUM_DECKS];
#ifdef FCS_WITH_TALONS
    fcs_card_t * talon;
    char talon_params[4];
#endif
};
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

/*  ===== Depracated Information =====
 * Stack: 0 - Number of cards 1-19 - Cards
 * Stacks: stack_num*20 where stack_num >= 0 and stack_num <= 7
 * Bytes 160-163 - Freecells
 * Bytes 164-167 - Decks
 */

typedef struct fcs_struct_state_t fcs_state_t;

typedef char fcs_locs_t;

#define fcs_state_get_col(state, col_idx) \
    ( (state).data + ((col_idx)*(MAX_NUM_CARDS_IN_A_STACK+1)) )


#define FCS_FREECELLS_OFFSET ((MAX_NUM_STACKS)*(MAX_NUM_CARDS_IN_A_STACK+1))

#define fcs_freecell_card(state, f) \
    ( (state).data[FCS_FREECELLS_OFFSET+(f)] )

#define FCS_FOUNDATIONS_OFFSET (((MAX_NUM_STACKS)*(MAX_NUM_CARDS_IN_A_STACK+1))+(MAX_NUM_FREECELLS))

#define fcs_foundation_value(state, d) \
    ( (state).data[FCS_FOUNDATIONS_OFFSET+(d)])

#ifdef FCS_WITH_TALONS
#define fcs_talon_len(state) \
    ((state).talon_params[0])

#define fcs_talon_pos(state) \
    ((state).talon_params[1])

#define fcs_put_card_in_talon(state, pos, card) \
    ((state).talon[pos] = (card))

#define fcs_get_talon_card(state, pos) \
    ((state).talon[pos])
#endif

#elif defined(INDIRECT_STACK_STATES) /* #ifdef DEBUG_STATES
                                        #elif defined(COMPACT_STATES)
                                      */

typedef char fcs_card_t;
typedef fcs_card_t * fcs_cards_column_t;
typedef char fcs_state_foundation_t;

struct fcs_struct_state_t
{
    fcs_cards_column_t stacks[MAX_NUM_STACKS];
    fcs_card_t freecells[MAX_NUM_FREECELLS];
    fcs_state_foundation_t foundations[MAX_NUM_DECKS*4];
#ifdef FCS_WITH_TALONS
    fcs_card_t * talon;
    char talon_params[4];
#endif
};

typedef struct fcs_struct_state_t fcs_state_t;

#define fcs_state_get_col(state, col_idx) \
    ( (state).stacks[(col_idx)] )



#define fcs_freecell_card(state, f) \
    ( (state).freecells[(f)] )

#define fcs_foundation_value(state, d) \
    ( (state).foundations[(d)] )

#ifdef FCS_WITH_TALONS
#define fcs_talon_len(state) \
    ((state).talon_params[0])

#define fcs_talon_pos(state) \
    ((state).talon_params[1])

#define fcs_put_card_in_talon(state, pos, card) \
    ((state).talon[pos] = (card))

#define fcs_get_talon_card(state, pos) \
    ((state).talon[pos])
#endif

#define fcs_copy_stack(state_key, state_val, idx, buffer) \
    {     \
        if (! ((state_val).stacks_copy_on_write_flags & (1 << idx)))        \
        {          \
            fcs_cards_column_t copy_stack_col; \
                                    \
            (state_val).stacks_copy_on_write_flags |= (1 << idx);       \
            copy_stack_col = fcs_state_get_col(state_key, idx); \
            memcpy(&buffer[idx << 7], copy_stack_col, fcs_col_len(copy_stack_col)+1); \
            fcs_state_get_col(state_key, idx) = &buffer[idx << 7];     \
        }     \
    }

#define fcs_duplicate_state_extra(ptr_dest_key, ptr_dest_val, ptr_src_key, ptr_src_val)  \
    {   \
        (ptr_dest_val)->stacks_copy_on_write_flags = 0; \
    }

typedef char fcs_locs_t;

#endif /* #ifdef DEBUG_STATES -
          #elif defined COMPACT_STATES -
          #elif defined INDIRECT_STACK_STATES
        */

/* These are macros that are common to all three _STATES types. */

/*
 * This macro determines if child can be placed above parent.
 *
 * The variable sequences_are_built_by has to be initialized to
 * the sequences_are_built_by member of the instance.
 *
 * */

#ifdef FCS_FREECELL_ONLY

#define fcs_is_parent_card(child, parent) \
    ((fcs_card_card_num(child)+1 == fcs_card_card_num(parent)) && \
            ((fcs_card_suit(child) & 0x1) != (fcs_card_suit(parent)&0x1)) \
    )

#else

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

#define fcs_col_get_card_num(col, card_idx) \
    fcs_card_card_num(fcs_col_get_card((col), (card_idx)))

#define fcs_freecell_card_num(state, f) \
    ( fcs_card_card_num(fcs_freecell_card((state),(f))) )

#define fcs_freecell_card_suit(state, f) \
    ( fcs_card_suit(fcs_freecell_card((state),(f))) )

#define fcs_increment_foundation(state, d) \
    ( (fcs_foundation_value((state), (d)))++)

#define fcs_set_foundation(state, found, value) \
    ( (fcs_foundation_value((state), (found))) = (fcs_state_foundation_t)(value) )

#define fcs_col_pop_top(col) \
    {       \
        fcs_col_get_card((col), (--fcs_col_len(col))) = fc_solve_empty_card;  \
    }

#define fcs_col_pop_card(col, into) \
    {   \
        into = fcs_col_get_card((col), (fcs_col_len(col)-1)); \
        fcs_col_pop_top(col); \
    }

#define fcs_col_push_card(col, from) \
{ \
  fcs_col_get_card((col), ((fcs_col_len(col))++)) = (from); \
}

#define fcs_col_push_col_card(dest_col, src_col, card_idx) \
    fcs_col_push_card((dest_col), fcs_col_get_card((src_col), (card_idx)))

#define fcs_duplicate_state(ptr_dest_key, ptr_dest_val, ptr_src_key, ptr_src_val) \
    { \
    *(ptr_dest_key) = *(ptr_src_key); \
    *(ptr_dest_val) = *(ptr_src_val); \
    (ptr_dest_val)->key = ptr_dest_key; \
    fcs_duplicate_state_extra(ptr_dest_key, ptr_dest_val, ptr_src_key, ptr_src_val);   \
    }

#if defined(COMPACT_STATES) || defined(DEBUG_STATES)

#define fcs_duplicate_state_extra(ptr_dest_key, ptr_dest_val, ptr_src_key, ptr_src_val) \
    {} 

#define fcs_copy_stack(state_key, state_val, idx, buffer) {}

#endif

#define fcs_put_card_in_freecell(state, f, card) \
    (fcs_freecell_card((state), (f)) = (card))

#define fcs_empty_freecell(state, f) \
    fcs_put_card_in_freecell((state), (f), fc_solve_empty_card)

#ifndef FCS_WITHOUT_CARD_FLIPPING
#define fcs_col_flip_card(col, c) \
    (fcs_card_set_flipped(fcs_col_get_card((col), (c)), 0))
#endif

/* These are macros that are common to COMPACT_STATES and 
 * INDIRECT_STACK_STATES */
#if defined(COMPACT_STATES) || defined(INDIRECT_STACK_STATES)

#define fcs_col_len(col) \
    ( ((col)[0]) )

#define fcs_col_get_card(col, card_idx) \
    ((col)[(card_idx)+1])

#define fcs_card_card_num(card) \
    ( (card) & 0x0F )

#define fcs_card_suit(card) \
    ( ((card) >> 4) & 0x03 )

#ifndef FCS_WITHOUT_CARD_FLIPPING
#define fcs_card_get_flipped(card) \
    ( (card) >> 6 )
#endif

#define fcs_card_set_num(card, num) \
    (card) = ((fcs_card_t)(((card)&0xF0)|(num)));

#define fcs_card_set_suit(card, suit) \
    (card) = ((fcs_card_t)(((card)&0x4F)|((suit)<<4)));

#ifndef FCS_WITHOUT_CARD_FLIPPING
#define fcs_card_set_flipped(card, flipped) \
    (card) = ((fcs_card_t)(((card)&((fcs_card_t)0x3F))|((fcs_card_t)((flipped)<<6))))
#endif

#endif

struct fcs_state_extra_info_struct
{
    fcs_locs_t stack_locs[MAX_NUM_STACKS];
    fcs_locs_t fc_locs[MAX_NUM_FREECELLS];
    fcs_state_t * key;
    struct fcs_state_extra_info_struct * parent_val;
    fcs_move_stack_t * moves_to_parent;
    int depth;
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
     * */
    int visited;
    /*
     * The iteration in which this state was marked as visited
     * */
    int visited_iter;
    /*
     * This is the number of direct children of this state which were not
     * yet declared as dead ends. Once this counter reaches zero, this
     * state too is declared as a dead end.
     * */
    int num_active_children;
    /*
     * This is a vector of flags - one for each scan. Each indicates whether
     * its scan has already visited this state
     * */
    int scan_visited[MAX_NUM_SCANS_BUCKETS];
#ifdef INDIRECT_STACK_STATES
    /*
     * A vector of flags that indicates which stacks were already copied.
     * */
    int stacks_copy_on_write_flags;
#endif
};

typedef struct fcs_state_extra_info_struct fcs_state_extra_info_t;

typedef struct
{
    fcs_state_t s;
    fcs_state_extra_info_t info;
} fcs_state_keyval_pair_t;

typedef struct {
    fcs_state_t * key;
    fcs_state_extra_info_t * val;
} fcs_standalone_state_ptrs_t;

extern fcs_card_t fc_solve_empty_card;

#ifdef FCS_WITH_TALONS
#define fcs_klondike_talon_len(state) \
    ((state).talon[0])

#define fcs_klondike_talon_stack_pos(state) \
    ((state).talon_params[0])

#define fcs_klondike_talon_queue_pos(state) \
    ((state).talon_params[1])

#define fcs_klondike_talon_num_redeals_left(state) \
    ((state).talon_params[2])

#define fcs_klondike_talon_get_top_card(state) \
    ((state).talon[(int)fcs_klondike_talon_stack_pos(state)])

#define fcs_klondike_talon_queue_to_stack(state) \
    ( ((state).talon[(int)((++fcs_klondike_talon_stack_pos(state))+1)]) =  \
      ((state).talon[(int)((fcs_klondike_talon_queue_pos(state)++)+1)]) )

#define fcs_klondike_talon_redeal_bare(state) \
    {         \
        fcs_klondike_talon_stack_pos(state) = -1; \
        fcs_klondike_talon_queue_pos(state) = 0; \
    }

#define fcs_klondike_talon_decrement_stack(state) \
    ((state).talon[(int)((fcs_klondike_talon_stack_pos(state)--)+1)] = fc_solve_empty_card)
#endif


extern void fc_solve_canonize_state(
    fcs_state_extra_info_t * state_val,
    int freecells_num,
    int stacks_num
    );

#if (FCS_STATE_STORAGE != FCS_STATE_STORAGE_INDIRECT)

#if (FCS_STATE_STORAGE != FCS_STATE_STORAGE_LIBREDBLACK_TREE)
typedef void * fcs_compare_context_t;
#else
typedef const void * fcs_compare_context_t;
#endif

extern int fc_solve_state_compare(const void * s1, const void * s2);
#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
extern int fc_solve_state_compare_equal(const void * s1, const void * s2);
#endif
extern int fc_solve_state_compare_with_context(const void * s1, const void * s2, fcs_compare_context_t context);
extern int fc_solve_state_extra_info_compare_with_context(const void * s1, const void * s2, fcs_compare_context_t context);

#else
extern int fc_solve_state_compare_indirect(const void * s1, const void * s2);
extern int fc_solve_state_compare_indirect_with_context(const void * s1, const void * s2, void * context);
#endif

#ifdef FCS_WITH_TALONS
extern int fcs_talon_compare_with_context(const void * s1, const void * s2, fcs_compare_context_t context);
#endif

enum
{
    FCS_USER_STATE_TO_C__SUCCESS = 0,
    FCS_USER_STATE_TO_C__PREMATURE_END_OF_INPUT
};


/*
 * This function converts an entire card from its string representations
 * (e.g: "AH", "KS", "8D"), to a fcs_card_t data type.
 * */
extern fcs_card_t fc_solve_card_user2perl(const char * str);

/*
 * Convert an entire card to its user representation.
 *
 * */
extern char * fc_solve_card_perl2user(
    fcs_card_t card,
    char * str,
    int t
    );

/*
 * Converts a card_number from its internal representation to a string.
 *
 * num - the card number
 * str - the string to output to.
 * card_num_is_null - a pointer to a bool that indicates whether
 *      the card number is out of range or equal to zero
 * t - whether 10 should be printed as T or not.
 * */
extern char * fc_solve_p2u_card_number(
    int num,
    char * str,
    int * card_num_is_null,
    int t
#ifndef FCS_WITHOUT_CARD_FLIPPING
    , int flipped
#endif
    );

/*
 * This function converts a card number from its user representation
 * (e.g: "A", "K", "9") to its card number that can be used by
 * the program.
 * */
extern int fc_solve_u2p_card_number(const char * string);

/*
 * This function converts a string containing a suit letter (that is
 * one of H,S,D,C) into its suit ID.
 *
 * The suit letter may come somewhat after the beginning of the string.
 *
 * */
extern int fc_solve_u2p_suit(const char * deck);

static GCC_INLINE void fc_solve_state_init(
    fcs_state_t * state_key,
    fcs_state_extra_info_t * state_val,
    int stacks_num
#ifdef INDIRECT_STACK_STATES
    ,char * indirect_stacks_buffer
#endif
    )
{
    int i;

    memset(state_key, 0, sizeof(*state_key));

    for(i=0;i<MAX_NUM_STACKS;i++)
    {
        state_val->stack_locs[i] = (char)i;
    }
#ifdef INDIRECT_STACK_STATES
    for(i=0;i<stacks_num;i++)
    {
        state_key->stacks[i] = &indirect_stacks_buffer[i << 7];
        memset(state_key->stacks[i], '\0', MAX_NUM_DECKS*52+1);
    }
    for(;i<MAX_NUM_STACKS;i++)
    {
        state_key->stacks[i] = NULL;
    }
#endif
    for(i=0;i<MAX_NUM_FREECELLS;i++)
    {
        state_val->fc_locs[i] = (char)i;
    }
    state_val->key = state_key;
    state_val->parent_val = NULL;
    state_val->moves_to_parent = NULL;
    state_val->depth = 0;
    state_val->visited = 0;
    state_val->visited_iter = 0;
    state_val->num_active_children = 0;
    memset(state_val->scan_visited, '\0', sizeof(state_val->scan_visited));
#ifdef INDIRECT_STACK_STATES
    state_val->stacks_copy_on_write_flags = 0;
#endif
}

static const char * const fc_solve_freecells_prefixes[] = { "FC:", "Freecells:", "Freecell:", ""};

static const char * const fc_solve_foundations_prefixes[] = { "Decks:", "Deck:", "Founds:", "Foundations:", "Foundation:", "Found:", ""};

#ifdef FCS_WITH_TALONS
static const char * const fc_solve_talon_prefixes[] = { "Talon:", "Queue:" , ""};
static const char * const fc_solve_num_redeals_prefixes[] = { "Num-Redeals:", "Readels-Num:", "Readeals-Number:", ""};
#endif

#ifdef WIN32
#define strncasecmp(a,b,c) (strnicmp((a),(b),(c)))
#endif

static GCC_INLINE int fc_solve_initial_user_state_to_c(
    const char * string,
    fcs_state_t * out_state_key,
    fcs_state_extra_info_t * out_state_val,
    int freecells_num,
    int stacks_num,
    int decks_num
#ifdef FCS_WITH_TALONS
    ,int talon_type
#endif
#ifdef INDIRECT_STACK_STATES
    , char * indirect_stacks_buffer
#endif
    )
{
    int s,c;
    const char * str;
    fcs_card_t card;
    fcs_cards_column_t col;
    int first_line;

    int prefix_found;
    const char * const * prefixes;
    int i;
    int decks_index[4];

    fc_solve_state_init(
        out_state_key,
        out_state_val,
        stacks_num
#ifdef INDIRECT_STACK_STATES
        , indirect_stacks_buffer
#endif
        );
    str = string;

    first_line = 1;

#define ret (*out_state_key)
/* Handle the end of string - shouldn't happen */
#define handle_eos() \
    { \
        if ((*str) == '\0') \
        {  \
            return FCS_USER_STATE_TO_C__PREMATURE_END_OF_INPUT; \
        } \
    }

#ifdef FCS_WITH_TALONS
    if (talon_type == FCS_TALON_KLONDIKE)
    {
        fcs_klondike_talon_num_redeals_left(ret) = -1;
    }
#endif

    for(s=0;s<stacks_num;s++)
    {
        /* Move to the next stack */
        if (!first_line)
        {
            while((*str) != '\n')
            {
                handle_eos();
                str++;
            }
            str++;
        }
        first_line = 0;

        prefixes = fc_solve_freecells_prefixes;
        prefix_found = 0;
        for(i=0;prefixes[i][0] != '\0'; i++)
        {
            if (!strncasecmp(str, prefixes[i], strlen(prefixes[i])))
            {
                prefix_found = 1;
                str += strlen(prefixes[i]);
                break;
            }
        }

        if (prefix_found)
        {
            for(c=0;c<freecells_num;c++)
            {
                fcs_empty_freecell(ret, c);
            }
            for(c=0;c<freecells_num;c++)
            {
                if (c!=0)
                {
                    while(
                            ((*str) != ' ') &&
                            ((*str) != '\t') &&
                            ((*str) != '\n') &&
                            ((*str) != '\r')
                         )
                    {
                        handle_eos();
                        str++;
                    }
                    if ((*str == '\n') || (*str == '\r'))
                    {
                        break;
                    }
                    str++;
                }

                while ((*str == ' ') || (*str == '\t'))
                {
                    str++;
                }
                if ((*str == '\r') || (*str == '\n'))
                    break;

                if ((*str == '*') || (*str == '-'))
                {
                    card = fc_solve_empty_card;
                }
                else
                {
                    card = fc_solve_card_user2perl(str);
                }

                fcs_put_card_in_freecell(ret, c, card);
            }

            while (*str != '\n')
            {
                handle_eos();
                str++;
            }
            s--;
            continue;
        }

        prefixes = fc_solve_foundations_prefixes;
        prefix_found = 0;
        for(i=0;prefixes[i][0] != '\0'; i++)
        {
            if (!strncasecmp(str, prefixes[i], strlen(prefixes[i])))
            {
                prefix_found = 1;
                str += strlen(prefixes[i]);
                break;
            }
        }

        if (prefix_found)
        {
            int d;

            for(d=0;d<decks_num*4;d++)
            {
                fcs_set_foundation(ret, d, 0);
            }

            for(d=0;d<4;d++)
            {
                decks_index[d] = 0;
            }
            while (1)
            {
                while((*str == ' ') || (*str == '\t'))
                    str++;
                if ((*str == '\n') || (*str == '\r'))
                    break;
                d = fc_solve_u2p_suit(str);
                str++;
                while (*str == '-')
                    str++;
                c = fc_solve_u2p_card_number(str);
                while (
                        (*str != ' ') &&
                        (*str != '\t') &&
                        (*str != '\n') &&
                        (*str != '\r')
                      )
                {
                    handle_eos();
                    str++;
                }

                fcs_set_foundation(ret, (decks_index[d]*4+d), c);
                decks_index[d]++;
                if (decks_index[d] >= decks_num)
                {
                    decks_index[d] = 0;
                }
            }
            s--;
            continue;
        }

#ifdef FCS_WITH_TALONS
        prefixes = fc_solve_talon_prefixes;
        prefix_found = 0;
        for(i=0;prefixes[i][0] != '\0'; i++)
        {
            if (!strncasecmp(str, prefixes[i], strlen(prefixes[i])))
            {
                prefix_found = 1;
                str += strlen(prefixes[i]);
                break;
            }
        }

        if (prefix_found)
        {
            /* Input the Talon */
            int talon_size;

            talon_size = MAX_NUM_DECKS*52+16;
            ret.talon = malloc(sizeof(fcs_card_t)*talon_size);
            fcs_talon_pos(ret) = 0;

            for(c=0 ; c < talon_size ; c++)
            {
                /* Move to the next card */
                if (c!=0)
                {
                    while(
                        ((*str) != ' ') &&
                        ((*str) != '\t') &&
                        ((*str) != '\n') &&
                        ((*str) != '\r')
                    )
                    {
                        handle_eos();
                        str++;
                    }
                    if ((*str == '\n') || (*str == '\r'))
                    {
                        break;
                    }
                }

                while ((*str == ' ') || (*str == '\t'))
                {
                    str++;
                }

                if ((*str == '\n') || (*str == '\r'))
                {
                    break;
                }

                card = fc_solve_card_user2perl(str);

                fcs_put_card_in_talon(ret, c+(talon_type==FCS_TALON_KLONDIKE), card);
            }
            fcs_talon_len(ret) = c;

            if (talon_type == FCS_TALON_KLONDIKE)
            {
                int talon_len;

                talon_len = fcs_talon_len(ret);
                fcs_klondike_talon_len(ret) = talon_len;
                fcs_klondike_talon_stack_pos(ret) = -1;
                fcs_klondike_talon_queue_pos(ret) = 0;
            }

            s--;
            continue;
        }

        prefixes = fc_solve_num_redeals_prefixes;
        prefix_found = 0;
        for(i=0;prefixes[i][0] != '\0'; i++)
        {
            if (!strncasecmp(str, prefixes[i], strlen(prefixes[i])))
            {
                prefix_found = 1;
                str += strlen(prefixes[i]);
                break;
            }
        }

        if (prefix_found)
        {
            while ((*str < '0') && (*str > '9') && (*str != '\n'))
            {
                handle_eos();
                str++;
            }
            if (*str != '\n')
            {
                int num_redeals;

                num_redeals = atoi(str);
                if (talon_type == FCS_TALON_KLONDIKE)
                {
                    fcs_klondike_talon_num_redeals_left(ret) =
                        (num_redeals < 0) ?
                            (-1) :
                            ((num_redeals > 127) ? 127 : num_redeals)
                                ;
                }
            }
            s--;
            continue;
        }
#endif

        col = fcs_state_get_col(ret, s);
        for(c=0 ; c < MAX_NUM_CARDS_IN_A_STACK ; c++)
        {
            /* Move to the next card */
            if (c!=0)
            {
                while(
                    ((*str) != ' ') &&
                    ((*str) != '\t') &&
                    ((*str) != '\n') &&
                    ((*str) != '\r')
                )
                {
                    handle_eos();
                    str++;
                }
                if ((*str == '\n') || (*str == '\r'))
                {
                    break;
                }
            }

            while ((*str == ' ') || (*str == '\t'))
            {
                str++;
            }
            handle_eos();
            if ((*str == '\n') || (*str == '\r'))
            {
                break;
            }
            card = fc_solve_card_user2perl(str);

            fcs_col_push_card(col, card);
        }
    }

    return FCS_USER_STATE_TO_C__SUCCESS;
}

#undef ret
#undef handle_eos

extern char * fc_solve_state_as_string(
    fcs_state_extra_info_t * state_val,
    int freecells_num,
    int stacks_num,
    int decks_num,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    );

enum
{
    FCS_STATE_VALIDITY__OK = 0,
    FCS_STATE_VALIDITY__MISSING_CARD = 1,
    FCS_STATE_VALIDITY__EXTRA_CARD = 2,
    FCS_STATE_VALIDITY__EMPTY_SLOT = 3,
    FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT = 4
};

static GCC_INLINE int fc_solve_check_state_validity(
    fcs_state_extra_info_t * state_val,
    int freecells_num,
    int stacks_num,
    int decks_num,
#ifdef FCS_WITH_TALONS
    int talon_type,
#endif
    fcs_card_t * misplaced_card)
{
    int cards[4][14];
    int c, s, d, f;
    int col_len;

    fcs_state_t * state;
    fcs_cards_column_t col;
    fcs_card_t card;

    state = state_val->key;

    /* Initialize all cards to 0 */
    for(d=0;d<4;d++)
    {
        for(c=1;c<=13;c++)
        {
            cards[d][c] = 0;
        }
    }

    /* Mark the cards in the decks */
    for(d=0;d<decks_num*4;d++)
    {
        for(c=1;c<=fcs_foundation_value(*state, d);c++)
        {
            cards[d%4][c]++;
        }
    }

    /* Mark the cards in the freecells */
    for(f=0;f<freecells_num;f++)
    {
        if (fcs_freecell_card_num(*state, f) != 0)
        {
            cards
                [fcs_freecell_card_suit(*state, f)]
                [fcs_freecell_card_num(*state, f)] ++;
        }
    }

    /* Mark the cards in the stacks */
    for(s=0;s<stacks_num;s++)
    {
        col = fcs_state_get_col(*state, s);
        col_len = fcs_col_len(col);
        for(c=0;c<col_len;c++)
        {
            card = fcs_col_get_card(col,c);
            if (fcs_card_card_num(card) == 0)
            {
                *misplaced_card = fc_solve_empty_card;
                return FCS_STATE_VALIDITY__EMPTY_SLOT;
            }
            cards
                [fcs_card_suit(card)]
                [fcs_card_card_num(card)] ++;

        }
    }

#ifdef FCS_WITH_TALONS
    /* Mark the cards in the (gypsy) talon */
    if ((talon_type == FCS_TALON_GYPSY) || (talon_type == FCS_TALON_KLONDIKE))
    {
        for(c = ((talon_type == FCS_TALON_GYPSY)?fcs_talon_pos(*state):1) ;
            c < ((talon_type==FCS_TALON_GYPSY) ? fcs_talon_len(*state) : (fcs_klondike_talon_len(*state)+1)) ;
            c++)
        {
            if (fcs_get_talon_card(*state,c) != fc_solve_empty_card)
            {
                cards
                    [fcs_card_suit(fcs_get_talon_card(*state, c))]
                    [fcs_card_card_num(fcs_get_talon_card(*state, c))] ++;
            }
        }
    }
#endif

    /* Now check if there are extra or missing cards */

    for(d=0;d<4;d++)
    {
        for(c=1;c<=13;c++)
        {
            if (cards[d][c] != decks_num)
            {
                *misplaced_card = fc_solve_empty_card;
                fcs_card_set_suit(*misplaced_card, d);
                fcs_card_set_num(*misplaced_card, c);
                return ((cards[d][c] < decks_num) 
                    ? FCS_STATE_VALIDITY__MISSING_CARD
                    : FCS_STATE_VALIDITY__EXTRA_CARD
                    )
                    ;
            }
        }
    }

    return FCS_STATE_VALIDITY__OK;
}

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
};

#if defined(INDIRECT_STACK_STATES)
extern int fc_solve_stack_compare_for_comparison(const void * v_s1, const void * v_s2);
#endif

#endif /* FC_SOLVE__STATE_H */
