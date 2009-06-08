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
#include "config.h"

#include "fcs_move.h"

#ifndef FC_SOLVE__STATE_H
#define FC_SOLVE__STATE_H

#ifdef __cplusplus
extern "C" {
#endif

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

struct fcs_struct_card_t
{
    short card_num;
    char suit;
    char flags;
};

typedef struct fcs_struct_card_t fcs_card_t;

struct fcs_struct_stack_t
{
    int num_cards;
    fcs_card_t cards[MAX_NUM_CARDS_IN_A_STACK];
};

typedef struct fcs_struct_stack_t fc_stack_t;
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
#ifdef FCS_WITH_TALON
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

#if 0
struct fcs_struct_state_with_locations_t
{
    fcs_state_t s;
    char stack_locs[MAX_NUM_STACKS];
    char fc_locs[MAX_NUM_FREECELLS];
    struct fcs_struct_state_with_locations_t * parent;
    fcs_move_stack_t * moves_to_parent;
    int depth;
    int visited;
    int visited_iter;
    int num_active_children;
    int scan_visited[MAX_NUM_SCANS_BUCKETS];
};

typedef struct fcs_struct_state_with_locations_t fcs_state_with_locations_t;
#endif
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

#define fcs_standalone_stack_len(stack) \
    ( (int)(stack[0]) )

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
            int stack_len;      \
            fcs_cards_column_t copy_stack_col; \
                                    \
            (state_val).stacks_copy_on_write_flags |= (1 << idx);       \
            copy_stack_col = fcs_state_get_col(state_key, idx); \
            stack_len = fcs_col_len(copy_stack_col);            \
            memcpy(&buffer[idx << 7], copy_stack_col, stack_len+1); \
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

#define fcs_col_get_card_num(col, c_idx) \
    fcs_card_card_num(fcs_col_get_card((col), (c_idx)))

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
        fcs_col_get_card((col), (--fcs_col_len(col))) = fcs_empty_card;  \
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
    fcs_put_card_in_freecell((state), (f), fcs_empty_card)

#ifndef FCS_WITHOUT_CARD_FLIPPING
#define fcs_col_flip_card(col, c) \
    (fcs_card_set_flipped(fcs_col_get_card((col), (c)), 0))
#endif

/* These are macros that are common to COMPACT_STATES and 
 * INDIRECT_STACK_STATES */
#if defined(COMPACT_STATES) || defined(INDIRECT_STACK_STATES)

#define fcs_col_len(col) \
    ( ((col)[0]) )

#define fcs_col_get_card(col, c_idx) \
    ((col)[(c_idx)+1])

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

/* Commenting out so the API will be broken - we're now using key/value
 * pairs.
 * */
#if 0
struct fcs_struct_state_with_locations_t
{
    fcs_state_t s;
    fcs_locs_t stack_locs[MAX_NUM_STACKS];
    fcs_locs_t fc_locs[MAX_NUM_FREECELLS];
    struct fcs_struct_state_with_locations_t * parent;
    fcs_move_stack_t * moves_to_parent;
    int depth;
    /*
     * This field contains global, scan-independant flags, which are used
     * from the FCS_VISITED_T enum below.
     *
     * FCS_VISITED_VISITED - deprecated
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

typedef struct fcs_struct_state_with_locations_t fcs_state_with_locations_t;
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
     * from the FCS_VISITED_T enum below.
     *
     * FCS_VISITED_VISITED - deprecated
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
#define fcs_empty_card fc_solve_empty_card


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
    ((state).talon[(int)((fcs_klondike_talon_stack_pos(state)--)+1)] = fcs_empty_card)
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
extern int fc_solve_state_compare_equal(const void * s1, const void * s2);
extern int fc_solve_state_compare_with_context(const void * s1, const void * s2, fcs_compare_context_t context);
extern int fc_solve_state_extra_info_compare_with_context(const void * s1, const void * s2, fcs_compare_context_t context);

#else
extern int fc_solve_state_compare_indirect(const void * s1, const void * s2);
extern int fc_solve_state_compare_indirect_with_context(const void * s1, const void * s2, void * context);
#endif

#ifdef FCS_WITH_TALONS
extern int fcs_talon_compare_with_context(const void * s1, const void * s2, fcs_compare_context_t context);
#endif

enum FCS_USER_STATE_TO_C_RETURN_CODES
{
    FCS_USER_STATE_TO_C__SUCCESS = 0,
    FCS_USER_STATE_TO_C__PREMATURE_END_OF_INPUT
};

int fc_solve_initial_user_state_to_c(
    const char * string,
    fcs_state_t * key,
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
    );


extern char * fc_solve_state_as_string(
    fcs_state_extra_info_t * state_val,
    int freecells_num,
    int stacks_num,
    int decks_num,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    );

enum FCS_STATE_VALIDITY_CODES
{
    FCS_STATE_VALIDITY__OK = 0,
    FCS_STATE_VALIDITY__EMPTY_SLOT = 3,
    FCS_STATE_VALIDITY__EXTRA_CARD = 2,
    FCS_STATE_VALIDITY__MISSING_CARD = 1,
    FCS_STATE_VALIDITY__PREMATURE_END_OF_INPUT = 4
};

extern int fc_solve_check_state_validity(
    fcs_state_extra_info_t * state_val,
    int freecells_num,
    int stacks_num,
    int decks_num,
#ifdef FCS_WITH_TALONS
    int talon_type,
#endif
    fcs_card_t * misplaced_card
    );

#ifdef __cplusplus
}
#endif

enum FCS_VISITED_T
{
    FCS_VISITED_VISITED = 0x1,
    FCS_VISITED_IN_SOLUTION_PATH = 0x2,
    FCS_VISITED_IN_OPTIMIZED_PATH = 0x4,
    FCS_VISITED_DEAD_END = 0x8,
    FCS_VISITED_ALL_TESTS_DONE = 0x10
};

#if defined(INDIRECT_STACK_STATES)
extern int fc_solve_stack_compare_for_comparison(const void * v_s1, const void * v_s2);
#endif

#endif /* FC_SOLVE__STATE_H */
