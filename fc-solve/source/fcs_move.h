/*
 * fcs_move.h - header file for the move structure and enums of
 * Freecell Solver. This file is common to the main code and to the
 * library headers.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#ifndef __FCS_MOVE_H
#define __FCS_MOVE_H

/* #define FCS_DEBUG_MOVES */
#define FCS_COMPACT_MOVES

#ifdef __cplusplus
extern "C" {
#endif

enum fcs_move_types
{
    FCS_MOVE_TYPE_STACK_TO_STACK,
    FCS_MOVE_TYPE_STACK_TO_FREECELL,
    FCS_MOVE_TYPE_FREECELL_TO_STACK,
    FCS_MOVE_TYPE_FREECELL_TO_FREECELL,
    FCS_MOVE_TYPE_STACK_TO_FOUNDATION,
    FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION,
    FCS_MOVE_TYPE_FLIP_CARD,
    FCS_MOVE_TYPE_DEAL_GYPSY_TALON,
    FCS_MOVE_TYPE_KLONDIKE_TALON_TO_STACK,
    FCS_MOVE_TYPE_KLONDIKE_FLIP_TALON,
    FCS_MOVE_TYPE_KLONDIKE_REDEAL_TALON,
    FCS_MOVE_TYPE_SEQ_TO_FOUNDATION,
    FCS_MOVE_TYPE_CANONIZE,
    FCS_MOVE_TYPE_SEPARATOR,
    FCS_MOVE_TYPE_NULL,
};

#ifdef FCS_DEBUG_MOVES
struct fcs_move_struct
{
    /* The index of the foundation, in case there are more than one decks */
    int foundation;
    /* Used in the case of a stack to stack move */
    int num_cards_in_sequence;
    /* There are two freecells, one for the source and the other
     * for the destination */
    int src_freecell;
    int dest_freecell;
    /* Ditto for the stacks */
    int src_stack;
    int dest_stack;
    /* The type of the move see the enum fcs_move_types */
    int type;
};

#define fcs_move_set_src_stack(move,value)         (move).src_stack = (value);
#define fcs_move_set_src_freecell(move,value)      (move).src_freecell = (value);
#define fcs_move_set_dest_stack(move,value)        (move).dest_stack = (value);
#define fcs_move_set_dest_freecell(move,value)     (move).dest_freecell = (value);
#define fcs_move_set_foundation(move,value)        (move).foundation = (value);
#define fcs_move_set_type(move,value)              (move).type = (value);
#define fcs_move_set_num_cards_in_seq(move,value)  (move).num_cards_in_sequence = (value);

#define fcs_move_get_src_stack(move)               ((move).src_stack)
#define fcs_move_get_src_freecell(move)            ((move).src_freecell)
#define fcs_move_get_dest_stack(move)              ((move).dest_stack)
#define fcs_move_get_dest_freecell(move)           ((move).dest_freecell)
#define fcs_move_get_foundation(move)              ((move).foundation)
#define fcs_move_get_type(move)                    ((move).type)
#define fcs_move_get_num_cards_in_seq(move)        ((move).num_cards_in_sequence)

#elif defined(FCS_COMPACT_MOVES)
struct fcs_move_struct
{
    unsigned char c[4];
};

#define FCS_MOVE_TYPE                0
#define FCS_MOVE_SRC                 1
#define FCS_MOVE_DEST                2
#define FCS_MOVE_NUM_CARDS_IN_SEQ    3
#define FCS_MOVE_NUM_CARDS_FLIPPED   3

#define fcs_move_set_src_stack(move,value)        (move).c[FCS_MOVE_SRC] = (value);
#define fcs_move_set_src_freecell(move,value)     (move).c[FCS_MOVE_SRC] = (value);
#define fcs_move_set_dest_stack(move,value)       (move).c[FCS_MOVE_DEST] = (value);
#define fcs_move_set_dest_freecell(move,value)    (move).c[FCS_MOVE_DEST] = (value);
#define fcs_move_set_foundation(move,value)       (move).c[FCS_MOVE_DEST] = (value);
#define fcs_move_set_type(move,value)             (move).c[FCS_MOVE_TYPE] = (value);
#define fcs_move_set_num_cards_in_seq(move,value) (move).c[FCS_MOVE_NUM_CARDS_IN_SEQ] = (value);
#define fcs_move_set_num_cards_flipped(move,value) (move).c[FCS_MOVE_NUM_CARDS_FLIPPED] = (value);

#define fcs_move_get_src_stack(move)              ((move).c[FCS_MOVE_SRC])
#define fcs_move_get_src_freecell(move)           ((move).c[FCS_MOVE_SRC])
#define fcs_move_get_dest_stack(move)             ((move).c[FCS_MOVE_DEST])
#define fcs_move_get_dest_freecell(move)          ((move).c[FCS_MOVE_DEST])
#define fcs_move_get_foundation(move)             ((move).c[FCS_MOVE_DEST])
#define fcs_move_get_type(move)                   ((move).c[FCS_MOVE_TYPE])
#define fcs_move_get_num_cards_in_seq(move)       ((move).c[FCS_MOVE_NUM_CARDS_IN_SEQ])
#define fcs_move_get_num_cards_flipped(move,value) ((move).c[FCS_MOVE_NUM_CARDS_FLIPPED])
#endif

typedef struct fcs_move_struct fcs_move_t;

struct fcs_move_stack_struct
{
    fcs_move_t * moves;
    int max_num_moves;
    int num_moves;
};

typedef struct fcs_move_stack_struct fcs_move_stack_t;


#ifdef __cplusplus
}
#endif

#endif  /* __FCS_MOVE_H */
