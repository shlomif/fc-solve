#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "fc_pro_iface_pos.h"

#include "inline.h"
#include "alloc_wrap.h"

#if 0
static const char * ranks_map = "0A23456789TJQK";

static char * rank_to_string(int rank, char * buf)
{
    buf[0] = ranks_map[rank];
    buf[1] = '\0';
    return buf;
}

static const char * suits_map = "HCDS";

static char * suit_to_string(int suit, char * buf)
{
    buf[0] = suits_map[suit];
    buf[1] = '\0';
    return buf;
}

static char * card_to_string(fcs_card_t card, char * buf)
{
    rank_to_string(card&0x0F,buf);
    suit_to_string(card>>4,buf+1);

    return buf;
}

#endif

static GCC_INLINE int Cvtf89(int fcn)
{
    return (fcn >= 7) ? (fcn+3) : fcn;
}

char * fc_solve_moves_processed_render_move(fcs_extended_move_t move, char * string)
{
    switch(fcs_move_get_type(move.move))
    {
        case FCS_MOVE_TYPE_STACK_TO_STACK:
                if (move.to_empty_stack && (fcs_move_get_num_cards_in_seq(move.move) > 1))
                {
                    sprintf(string, "%i%iv%x",
                        1+fcs_move_get_src_stack(move.move),
                        1+fcs_move_get_dest_stack(move.move),
                        fcs_move_get_num_cards_in_seq(move.move)
                        );
                }
                else
                {
                    sprintf(string, "%i%i",
                        1+fcs_move_get_src_stack(move.move),
                        1+fcs_move_get_dest_stack(move.move)
                        );
                }
        break;

        case FCS_MOVE_TYPE_FREECELL_TO_STACK:
                sprintf(string, "%c%i",
                    ('a'+Cvtf89(fcs_move_get_src_freecell(move.move))),
                    1+fcs_move_get_dest_stack(move.move)
                    );
        break;

        case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
        {
            char c1, c2;

            c1 = (char)('a'+Cvtf89(fcs_move_get_src_freecell(move.move)));
            c2 = (char)('a'+Cvtf89(fcs_move_get_dest_freecell(move.move)));
            sprintf(string, "%c%c", c1, c2);
        }
        break;

        case FCS_MOVE_TYPE_STACK_TO_FREECELL:
                sprintf(string, "%i%c",
                    1+fcs_move_get_src_stack(move.move),
                    ('a'+Cvtf89(fcs_move_get_dest_freecell(move.move)))
                    );
        break;

        case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
                sprintf(string, "%ih", 1+fcs_move_get_src_stack(move.move));
        break;


        case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
                sprintf(string, "%ch", ('a'+Cvtf89(fcs_move_get_src_freecell(move.move))));
        break;

        case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
                sprintf(string, "%ih", fcs_move_get_src_stack(move.move));
        break;

        default:
            string[0] = '\0';
        break;
    }
    return string+strlen(string);
}

#define MOVES_PROCESSED_GROW_BY 32
static void moves_processed_add_new_move(fcs_moves_processed_t * moves, fcs_extended_move_t new_move)
{
    if (! ((++moves->num_moves) & (MOVES_PROCESSED_GROW_BY - 1)))
    {
        moves->moves =
            SREALLOC (moves->moves, moves->num_moves + MOVES_PROCESSED_GROW_BY);
    }
    moves->moves[moves->num_moves-1] = new_move;
}

fcs_moves_processed_t * fc_solve_moves_processed_gen(
    const fcs_state_keyval_pair_t * const orig,
    const int num_freecells,
    const fcs_moves_sequence_t * const moves_seq
)
{
    fcs_state_keyval_pair_t pos_proto;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    fcs_kv_state_t orig_pass;
    orig_pass.key = &(orig->s);
    orig_pass.val = &(orig->info);

    fcs_kv_state_t pos_pass;
    pos_pass.key = &(pos_proto.s);
    pos_pass.val = &(pos_proto.info);

    fcs_duplicate_kv_state(&pos_pass, &orig_pass);
    for (int i = 0 ; i < 8 ; i++)
    {
        fcs_copy_stack(pos_proto.s, pos_proto.info, i, indirect_stacks_buffer);
    }

#define pos (pos_proto.s)
    fcs_moves_processed_t * ret;
    int virtual_stack_len[8];
#ifndef NDEBUG
    int virtual_freecell_len[12];
#endif
    int i, j, move_idx, num_back_end_moves;
    fcs_move_t move, out_move, * next_move_ptr;

    num_back_end_moves = moves_seq->num_moves;
    next_move_ptr = moves_seq->moves;

    ret = SMALLOC1(ret);
    ret->num_moves = 0;
    ret->moves = SMALLOC(ret->moves, MOVES_PROCESSED_GROW_BY);
    ret->next_move_idx = 0;

    for(i=0;i<8;i++)
    {
        fcs_cards_column_t col = fcs_state_get_col(pos, i);
        virtual_stack_len[i] = fcs_col_len(col);
    }
#ifndef NDEBUG
    for(i=0;i<num_freecells;i++)
    {
        virtual_freecell_len[i] = (! fcs_freecell_is_empty(pos, i)) ? 1 : 0;
    }
#endif

    for(move_idx=0; move_idx < num_back_end_moves ; move_idx ++)
    {
        /*
         * Move safe cards to the foundations
         * */
        while (1)
        {
#if 0
            {
                /* Check the intermediate position validity */
                char exists[4*13];
                int rank, suit;
                int fc, col, count;
                fcs_card_t card;

                memset(exists, '\0', sizeof(exists));
                for (suit=0;suit<4;suit++)
                {
                    for(rank=1;rank<=fcs_foundation_value(pos, suit);rank++)
                    {
                        exists[rank-1+suit*13] = 1;
                    }
                }
                for (col=0;col<8;col++)
                {
                    fcs_cards_column_t col_col = fcs_state_get_col(pos, col);
                    count = fcs_col_len(col_col);
                    for (i=0;i<count;i++)
                    {
                        card = fcs_col_get_card(col_col, i);
                        exists[fcs_card_rank(card)-1+fcs_card_suit(card)*13] = 1;
                    }
                }
                for (fc=0;fc<num_freecells;fc++)
                {
                    card = fcs_freecell_card(pos, fc);
                    if (! fcs_card_is_empty(card))
                    {
                        exists[fcs_card_rank(card)-1+fcs_card_suit(card)*13] = 1;
                    }
                }
                for (i=0;i<52;i++)
                {
                    if (exists[i] != 1)
                    {
                        printf("Invalid position!!!!!!!!!!!\n");
                        exit(-1);
                    }
                }
            }
#endif

#if 0
    fcs_state_locs_struct_t locs;
    fc_solve_init_locs(&locs);
            printf("STATE=<<<\n%s\n>>>\n",
                fc_solve_state_as_string(
                    &pos,
                    &pos_proto.info,
                    &locs,
                    4,
                    8,
                    1,
                    TRUE,
                    FALSE,
                    TRUE
                )
            );
#endif

            for (i = 0 ; i < 8 ; i++)
            {
                int rank, suit;
                fcs_card_t card;

                fcs_cards_column_t col = fcs_state_get_col(pos, i);
                if (fcs_col_len(col) > 0)
                {
                    card = fcs_col_get_card(col, fcs_col_len(col) - 1);
                    rank = fcs_card_rank(card);
                    suit = fcs_card_suit(card);
                    /* Check if we can safely move it */
                    if ((fcs_foundation_value(pos, suit^0x1) >= rank-2) &&
                        (fcs_foundation_value(pos, suit^0x1^0x2) >= rank-2) &&
                        (fcs_foundation_value(pos, suit^0x2) >= rank-3) &&
                        (fcs_foundation_value(pos, suit) == rank-1))
                    {
                        fcs_increment_foundation(pos, suit);
                        fcs_col_pop_top(col);
                        /* An Automove. */

                        break;
                    }
                }
            }
            if (i < 8)
            {
                continue;
            }
            for(j=0;j<num_freecells;j++)
            {
                int rank, suit;
                fcs_card_t card;

                if (! fcs_freecell_is_empty(pos, j))
                {
                    card = fcs_freecell_card(pos, j);
                    rank = fcs_card_rank(card);
                    suit = fcs_card_suit(card);
                    /* Check if we can safely move it */
                    if ((fcs_foundation_value(pos, suit^0x1) >= rank-2) &&
                        (fcs_foundation_value(pos, suit^0x1^0x2) >= rank-2) &&
                        (fcs_foundation_value(pos, suit^0x2) >= rank-3) &&
                        (fcs_foundation_value(pos, suit) == rank-1))
                    {
                        fcs_increment_foundation(pos, suit);
                        fcs_empty_freecell(pos, j);

                        /* We've just done an auto-move */
                        break;
                    }

                }
            }
            if ((i == 8) && (j == num_freecells))
            {
                break;
            }
        }
        move = *(next_move_ptr++);

        {
            int src, dest;
            fcs_card_t card;
            switch(fcs_move_get_type(move))
            {
                case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
                    {
                        src = fcs_move_get_src_stack(move);
                        fcs_cards_column_t col = fcs_state_get_col(pos, src);
                        assert(virtual_stack_len[src] >= fcs_col_len(col));
                        if (virtual_stack_len[src] == fcs_col_len(col))
                        {
                            fcs_col_pop_card(col, card);
                            fcs_increment_foundation(pos, fcs_card_suit(card));
                            virtual_stack_len[src]--;
                            {
                                fcs_extended_move_t ext_move;
                                ext_move.move = move;
                                /* Stub value to settle gcc. Isn't used. */
                                ext_move.to_empty_stack = 0;

                                moves_processed_add_new_move(ret, ext_move);
                            }
                        }
                        else
                        {
                            virtual_stack_len[src]--;
                        }
                    }
                    break;

                case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
                    {
                        src = fcs_move_get_src_freecell(move);
                        assert((virtual_freecell_len[src] == 1));
                        if (fcs_freecell_is_empty(pos, src))
                        {
                            /* Do nothing */
                        }
                        else
                        {
                            {
                                fcs_extended_move_t ext_move;
                                ext_move.move = move;
                                /* Stub value to settle gcc. Isn't used. */
                                ext_move.to_empty_stack = 0;

                                moves_processed_add_new_move(ret, ext_move);
                            }
                            fcs_increment_foundation(pos, fcs_freecell_card_suit(pos, src));
                            fcs_empty_freecell(pos, src);
                        }
#ifndef NDEBUG
                        virtual_freecell_len[src] = 0;
#endif
                    }
                    break;

                case FCS_MOVE_TYPE_FREECELL_TO_STACK:
                    {
                        src = fcs_move_get_src_freecell(move);
                        dest = fcs_move_get_dest_stack(move);
                        assert(virtual_freecell_len[src] == 1);
                        if (fcs_freecell_is_empty(pos, src))
                        {
                            /* Do nothing */
                        }
                        else
                        {
                            {
                                fcs_extended_move_t ext_move;
                                ext_move.move = move;
                                /* Stub value to settle gcc. Isn't used. */
                                ext_move.to_empty_stack = 0;

                                moves_processed_add_new_move(ret, ext_move);
                            }
                            fcs_cards_column_t dest_col = fcs_state_get_col(pos, dest);
                            fcs_col_push_card(dest_col, fcs_freecell_card(pos, src));
                            fcs_empty_freecell(pos, src);
                        }
#ifndef NDEBUG
                        virtual_freecell_len[src] = 0;
#endif
                        virtual_stack_len[dest]++;
                    }
                    break;

               case FCS_MOVE_TYPE_STACK_TO_FREECELL:
                    {
                        src = fcs_move_get_src_stack(move);
                        dest = fcs_move_get_dest_freecell(move);
                        assert(virtual_stack_len[src] > 0);
                        fcs_cards_column_t col = fcs_state_get_col(pos, src);
                        assert(fcs_col_len(col) <= virtual_stack_len[src]);
                        if (fcs_col_len(col) < virtual_stack_len[src])
                        {
                            /* Do nothing */
                        }
                        else
                        {
                            {
                                fcs_extended_move_t ext_move;

                                ext_move.move = move;
                                /* Stub value to settle gcc. Isn't used. */
                                ext_move.to_empty_stack = 0;

                                moves_processed_add_new_move(ret, ext_move);
                            }
                            fcs_card_t temp_card;
                            fcs_col_pop_card(col, temp_card);
                            fcs_put_card_in_freecell(pos, dest, temp_card);
                        }
                        virtual_stack_len[src]--;
#ifndef NDEBUG
                        virtual_freecell_len[dest] = 1;
#endif
                    }
                    break;

               case FCS_MOVE_TYPE_STACK_TO_STACK:
                    {
                        int num_cards, virt_num_cards;

                        src = fcs_move_get_src_stack(move);
                        dest = fcs_move_get_dest_stack(move);
                        num_cards = fcs_move_get_num_cards_in_seq(move);
                        fcs_cards_column_t src_col = fcs_state_get_col(pos, src);
                        fcs_cards_column_t dest_col = fcs_state_get_col(pos, dest);
                        int src_len = fcs_col_len(src_col);
                        assert(virtual_stack_len[src] >= src_len);
                        if (virtual_stack_len[src] > src_len)
                        {
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
                            virt_num_cards = min((virtual_stack_len[src]-src_len), num_cards);
#undef min
                            virtual_stack_len[src] -= virt_num_cards;
                            virtual_stack_len[dest] += virt_num_cards;
                            num_cards -= virt_num_cards;
                        }
                        if (num_cards > 0)
                        {
                            fcs_move_set_type(out_move, FCS_MOVE_TYPE_STACK_TO_STACK);
                            fcs_move_set_src_stack(out_move, src);
                            fcs_move_set_dest_stack(out_move, dest);
                            fcs_move_set_num_cards_in_seq(out_move, num_cards);
                            {
                                fcs_extended_move_t ext_move;
                                ext_move.move = out_move;
                                ext_move.to_empty_stack = (fcs_col_len(dest_col) == 0);
                                moves_processed_add_new_move(ret, ext_move);
                            }
                            for(i=0;i<num_cards;i++)
                            {
                                fcs_col_push_col_card(dest_col, src_col, fcs_col_len(src_col)-num_cards+i);
                            }
                            for(i=0;i<num_cards;i++)
                            {
                                fcs_col_pop_top(src_col);
                            }
                            virtual_stack_len[dest] += num_cards;
                            virtual_stack_len[src] -= num_cards;
                        }
                    }
                    break;

            }
        }
    }

    return ret;
}


int fc_solve_moves_processed_get_next_move(fcs_moves_processed_t * moves, fcs_extended_move_t * move)
{
    if (moves->next_move_idx == moves->num_moves)
    {
        return 1;
    }
    *move = moves->moves[moves->next_move_idx++];
    return 0;
}

void fc_solve_moves_processed_free(fcs_moves_processed_t * moves)
{
    free(moves->moves);
    free(moves);
}
