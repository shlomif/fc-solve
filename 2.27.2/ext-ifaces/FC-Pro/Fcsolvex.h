// 1997 wilson callan
// 1994 don woods

#define CLUBS	 (0 << 4)
#define DIAMONDS (1 << 4)
#define SPADES	 (2 << 4)
#define HEARTS   (3 << 4)

#define SOLVE_RESULT	(0)
#define SOLVE_RANGE		(1)
#define SOLVE_SOLUTION	(2)
#define SOLVE_LIST		(3)

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
