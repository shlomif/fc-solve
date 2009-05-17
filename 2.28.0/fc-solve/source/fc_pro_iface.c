#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "fc_pro_iface_pos.h"

#ifdef WITH_FREE2SOLVER
char * moves_string;
#endif

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

static char * card_to_string(Card card, char * buf)
{
    rank_to_string(card&0x0F,buf);
    suit_to_string(card>>4,buf+1);

    return buf;
}

char * fc_solve_fc_pro_position_to_string(Position * pos, int num_freecells)
{
    int a, stack;
    char buffer[4000], temp[4][20];
    char * s_end;

    buffer[0] = '\0';
    s_end = buffer;
    
    for(a=0;a<4;a++)
    {
        if (pos->foundations[a] != 0)
        {
            break;
        }
    }
    if (a < 4)
    {
        s_end += sprintf(s_end, "Foundations:");
        for(a=0;a<4;a++)
        {
            if (pos->foundations[a] != 0)
            {
                s_end += sprintf(
                    s_end, 
                    " %s-%s", 
                    suit_to_string(a, temp[0]),
                    rank_to_string(pos->foundations[a], temp[1])
                    );
            }
        }
        *s_end = '\n';
        s_end++;
    }
    s_end += sprintf(s_end, "Freecells:");
    for(a=0;a<num_freecells;a++)
    {
        if (pos->hold[a] == 0)
        {
            s_end += sprintf(s_end, " -");
        }
        else
        {
            s_end += sprintf(s_end, " %s", card_to_string(pos->hold[a], temp[0]));
        }
    }
    *s_end = '\n';
    s_end++;

    for(stack=0;stack<8;stack++)
    {
        for(a=0;a<pos->tableau[stack].count;a++)
        {
            s_end += 
                sprintf(
                    s_end, 
                    "%s%s", 
                    ((a == 0)? "" : " "),
                    card_to_string(pos->tableau[stack].cards[a], temp[0])
                );                                
        }
        *s_end = '\n';
        s_end++;
    }
    *s_end = '\0';

    return strdup(buffer);
}

int Cvtf89(int fcn)
{
    return (fcn >= 7) ? (fcn+3) : fcn;
}

char * moves_processed_render_move(fcs_extended_move_t move, char * string)
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
                sprintf(string, "%c%c",
                    ('a'+Cvtf89(fcs_move_get_src_freecell(move.move))),
                    ('a'+Cvtf89(fcs_move_get_dest_freecell(move.move)))
                    );                        
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
static void moves_processed_add_new_move(moves_processed_t * moves, fcs_extended_move_t new_move)
{
    if (! ((++moves->num_moves) & (MOVES_PROCESSED_GROW_BY - 1)))
    {
        moves->moves =
            realloc(
                moves->moves,
                (
                    sizeof(moves->moves[0])
                    * (moves->num_moves + MOVES_PROCESSED_GROW_BY)
                )
            );
    }    
    moves->moves[moves->num_moves-1] = new_move;
}

moves_processed_t * moves_processed_gen(Position * orig, int NoFcs, void * instance)
{
    Position pos;
    moves_processed_t * ret;
    int virtual_stack_len[8];
    int virtual_freecell_len[12];
    int i, j, move_idx, num_back_end_moves;
    fcs_move_t move, out_move;

    pos = *orig;
    
    num_back_end_moves = freecell_solver_user_get_moves_left(instance);

    ret = malloc(sizeof(*ret));
    ret->num_moves = 0;
    ret->moves = malloc(sizeof(ret->moves[0]) * MOVES_PROCESSED_GROW_BY);
    ret->next_move_idx = 0;

    for(i=0;i<8;i++)
    {
        virtual_stack_len[i] = orig->tableau[i].count;        
    }
    for(i=0;i<NoFcs;i++)
    {
        virtual_freecell_len[i] = (orig->hold[i] != 0) ? 1 : 0;
    }
    
    for(move_idx=0; move_idx < num_back_end_moves ; move_idx ++)
    {
#if 0
        if (getenv("FCS_OUTPUT_INTERMEDIATE_POS"))
        {
            char * as_str;
            as_str = fc_solve_fc_pro_position_to_string(&pos, NoFcs);
            printf("state =\n<<<\n%s\n>>>\n\n", as_str);
            free(as_str);
        }
#endif


        /* 
         * Move safe cards to the foundations 
         * */
        while (1)
        {
#ifdef CHECK_STATE
            {
                /* Check the intermediate position validity */
                char exists[4*13];
                int rank, suit;
                int fc, col, count, i;
                Card card;

                memset(exists, '\0', sizeof(exists));
                for (suit=0;suit<4;suit++)
                {
                    for(rank=1;rank<=pos.foundations[suit];rank++)
                    {
                        exists[rank-1+suit*13] = 1;
                    }
                }
                for (col=0;col<8;col++)
                {
                    count = pos.tableau[col].count;
                    for (i=0;i<count;i++)
                    {
                        card = pos.tableau[col].cards[i];
                        exists[(card & 0x0F)-1+(card >> 4)*13] = 1;
                    }
                }
                for (fc=0;fc<NoFcs;fc++)
                {
                    card = pos.hold[fc];
                    if (card != 0)
                    {
                        exists[(card & 0x0F)-1+(card >> 4)*13] = 1;
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
            
            for(i=0;i<8;i++)
            {
                int rank, suit;
                Card card;
                
                if (pos.tableau[i].count > 0)
                {
                    card = pos.tableau[i].cards[pos.tableau[i].count-1];
                    rank = card & 0x0F;
                    suit = card >> 4;
                    /* Check if we can safely move it */
                    if ((pos.foundations[suit^0x1] > rank-2) &&
                        (pos.foundations[suit^0x1^0x2] > rank-2) && 
                        (pos.foundations[suit^0x2] > rank-3) &&
                        (pos.foundations[suit] == rank-1))
                    {
                        pos.foundations[suit]++;
                        pos.tableau[i].count--;

                        /* An Automove. */
                        
                        break;
                    }
                }
            }
            if (i < 8)
            {
                continue;
            }
            for(j=0;j<NoFcs;j++)
            {
                int rank, suit;
                Card card;

                if (pos.hold[j] != 0)
                {
                    card = pos.hold[j];
                    rank = card & 0x0F;
                    suit = card >> 4;
                    /* Check if we can safely move it */
                    if ((pos.foundations[suit^0x1] > rank-2) &&
                        (pos.foundations[suit^0x1^0x2] > rank-2) && 
                        (pos.foundations[suit^0x2] > rank-3) &&
                        (pos.foundations[suit] == rank-1))
                    {
                        pos.foundations[suit]++;
                        pos.hold[j] = 0;

                        /* We've just done an auto-move */
                        break;
                    }                        
                    
                }
            }
            if ((i == 8) && (j == NoFcs))
            {
                break;
            }
        }
        freecell_solver_user_get_next_move(instance, &move);

        {
            int src, dest, len;
            Card card;
            switch(fcs_move_get_type(move))
            {
                case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
                    {
                        src = fcs_move_get_src_stack(move);
                        assert(virtual_stack_len[src] >= pos.tableau[src].count);
                        if (virtual_stack_len[src] == pos.tableau[src].count)
                        {
                            len = virtual_stack_len[src];
                            card = pos.tableau[src].cards[len-1];
                            pos.foundations[card >> 4]++;
                            virtual_stack_len[src]--;
                            pos.tableau[src].count--;
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
                        if (pos.hold[src] == 0)
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
                            pos.foundations[pos.hold[src] >> 4]++;
                            pos.hold[src] = 0;
                        }
                        virtual_freecell_len[src] = 0;
                    }
                    break;

                case FCS_MOVE_TYPE_FREECELL_TO_STACK:
                    {
                        src = fcs_move_get_src_freecell(move);
                        dest = fcs_move_get_dest_stack(move);
                        assert(virtual_freecell_len[src] == 1);
                        if (pos.hold[src] == 0)
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
                            pos.tableau[dest].cards[pos.tableau[dest].count++] = pos.hold[src];
                            pos.hold[src] = 0;
                        }                        
                        virtual_freecell_len[src] = 0;
                        virtual_stack_len[dest]++;
                    }
                    break;

               case FCS_MOVE_TYPE_STACK_TO_FREECELL:
                    {
                        src = fcs_move_get_src_stack(move);
                        dest = fcs_move_get_dest_freecell(move);
                        assert(virtual_stack_len[src] > 0);
                        assert(pos.tableau[src].count <= virtual_stack_len[src]);
                        if (pos.tableau[src].count < virtual_stack_len[src])
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
                            pos.hold[dest] = pos.tableau[src].cards[--pos.tableau[src].count];
                        }
                        virtual_stack_len[src]--;
                        virtual_freecell_len[dest] = 1;
                    }
                    break;

               case FCS_MOVE_TYPE_STACK_TO_STACK:
                    {
                        int num_cards, virt_num_cards;
                        
                        src = fcs_move_get_src_stack(move);
                        dest = fcs_move_get_dest_stack(move);
                        num_cards = fcs_move_get_num_cards_in_seq(move);
                        assert(virtual_stack_len[src] >= pos.tableau[src].count);
                        if (virtual_stack_len[src] > pos.tableau[src].count)
                        {
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
                            virt_num_cards = min((virtual_stack_len[src]-pos.tableau[src].count), num_cards);
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
                                ext_move.to_empty_stack = (pos.tableau[dest].count == 0);
                                moves_processed_add_new_move(ret, ext_move);
                            }
                            for(i=0;i<num_cards;i++)
                            {
                                pos.tableau[dest].cards[pos.tableau[dest].count+i] = pos.tableau[src].cards[pos.tableau[src].count-num_cards+i];
                            }
                            pos.tableau[dest].count += num_cards;
                            pos.tableau[src].count -= num_cards;
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

int moves_processed_get_moves_left(moves_processed_t * moves)
{
    return moves->num_moves- moves->next_move_idx;
}

int moves_processed_get_next_move(moves_processed_t * moves, fcs_extended_move_t * move)
{
    if (moves->next_move_idx == moves->num_moves)
    {
        return 1;
    }
    *move = moves->moves[moves->next_move_idx++];
    return 0;
}

void moves_processed_free(moves_processed_t * moves)
{
    free(moves->moves);
    free(moves);
}

#ifdef WITH_FREE2SOLVER
static char * cmd_line_known_parameters[] = { NULL };

int Free2Solver(Position * orig, int NoFcs, int limit, int cmd_line_argc, char * * cmd_line_argv,
    int (*signal_step)(int step_limit))
{
    char * state_string;
    void * instance;
    int verdict;
    int num_iters;
    int ret;
    int current_limit = 1000;
    char * err_str;
    int ret_arg, parser_ret;


    state_string = fc_solve_fc_pro_position_to_string(orig, NoFcs);

    instance = freecell_solver_user_alloc();

    NumFCs = NoFcs ;

    parser_ret = freecell_solver_user_cmd_line_parse_args(
            instance,
            cmd_line_argc,
            cmd_line_argv,
            0,
            cmd_line_known_parameters,
            NULL,
            NULL,
            &err_str,
            &ret_arg
            );

    if (parser_ret == FCS_CMD_LINE_UNRECOGNIZED_OPTION)
    {
        freecell_solver_user_free(instance);        
        return 0;
    }
    
            
    freecell_solver_user_set_game(
        instance,

        NoFcs,
        8,
        1,
        FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
        0,
        FCS_ES_FILLED_BY_ANY_CARD
        );

    freecell_solver_user_limit_iterations(instance, current_limit);

    verdict = freecell_solver_user_solve_board(instance, state_string);

    free(state_string);

    while ((verdict == FCS_STATE_SUSPEND_PROCESS) && 
           (
                (limit > 0) ? 
                    (current_limit < limit) :
                    1
           )
          )
    {
        if (signal_step(current_limit) != 0)
        {
            break;
        }
        current_limit += 1000;
        freecell_solver_user_limit_iterations(instance, current_limit);
        verdict = freecell_solver_user_resume_solution(instance);
    }

    num_iters = freecell_solver_user_get_num_times(instance);

    if (verdict == FCS_STATE_WAS_SOLVED)
    {
        int num_moves;
        int a;
        fcs_extended_move_t move;
        char * str, * moves_string_proto;
        int len;
        moves_processed_t * moves_processed;

        ret = num_iters;
        
        moves_processed = moves_processed_gen(orig, NoFcs, instance);
        num_moves = moves_processed_get_moves_left(moves_processed);
        moves_string_proto = (char *)malloc(moves_processed->num_moves*4+1);
        
        /* a = num_moves-1; */
        str = moves_string_proto;

        while (! moves_processed_get_next_move(moves_processed, &move))
        {
            str = moves_processed_render_move(move, str);
        }
        moves_processed_free(moves_processed);
        len = str-moves_string_proto;
        moves_string = malloc(len+1);
        for(a=0;a<len;a++)
        {
            moves_string[a] = moves_string_proto[len-1-a];
        }
        moves_string[a] = '\0';
        free(moves_string_proto);
        
    }
    else if (verdict == FCS_STATE_IS_NOT_SOLVEABLE)
    {
        if (num_iters == 0)
            num_iters++ ;
        ret = -num_iters;
    }
    else if ((verdict == FCS_STATE_SUSPEND_PROCESS) || (verdict == FCS_STATE_BEGIN_SUSPEND_PROCESS))
    {
        ret = 0;
    }
    
    freecell_solver_user_free(instance);

    return ret;
}
#endif

