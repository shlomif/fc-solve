#ifndef FC_SOLVE__FC_PRO_IFACE_POS_H
#define FC_SOLVE__FC_PRO_IFACE_POS_H

#ifdef __cplusplus
extern "C" {
#endif

// 1997 wilson callan
// 1994 don woods

#include "fcs_user.h"
#include "fcs_cl.h"

typedef unsigned char	uchar;
typedef uchar		Card;/* 00ssrrrr: s=suit, r=rank(1-13) */

typedef struct column_struct {
	uchar	count;		/* number rdof cards in column */
Card	cards[19];
} Column;

typedef struct pos_struct {
	uchar	foundations[4];
	Card	hold[8];
	Column	tableau[8];
} Position;

extern char * fc_solve_fc_pro_position_to_string(Position * pos, int num_freecells);

struct fcs_extended_move_struct
{
    fcs_move_t move;
    int to_empty_stack;
};

typedef struct fcs_extended_move_struct fcs_extended_move_t;

struct moves_processed_struct
{
    int next_move_idx;
    int num_moves;
    fcs_extended_move_t * moves;
};

typedef struct moves_processed_struct moves_processed_t;

moves_processed_t * moves_processed_gen(
        Position * orig,
        int freecells_num,
        void * instance
        );

char * moves_processed_render_move(fcs_extended_move_t move, char * string);

extern int moves_processed_get_moves_left(moves_processed_t * moves);
extern int moves_processed_get_next_move(moves_processed_t * moves, fcs_extended_move_t * move);
extern void moves_processed_free(moves_processed_t * moves);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FC_SOLVE__FC_PRO_IFACE_POS_H */
