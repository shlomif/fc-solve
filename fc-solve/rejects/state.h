/*
 * =====================================================================================
 *
 *       Filename:  state.h
 *
 *    Description:  state.h rejects file. Contains various code that was
 *    phased out.
 *
 *        Version:  1.0
 *        Created:  02/05/2011 05:46:39 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 */

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

#ifdef FCS_WITH_TALONS
static int fcs_talon_compare_with_context(const void * p1, const void * p2, fcs_compare_context_t context)
{
    fcs_card_t * t1 = (fcs_card_t *)p1;
    fcs_card_t * t2 = (fcs_card_t *)p2;

    if (t1[0] < t2[0])
    {
        return -1;
    }
    else if (t1[0] > t2[0])
    {
        return 1;
    }
    else
    {
        return memcmp(t1,t2,t1[0]+1);
    }
}
#endif


struct fcs_struct_state_t
{
    fcs_card_t data[MAX_NUM_STACKS*(MAX_NUM_CARDS_IN_A_STACK+1)+MAX_NUM_FREECELLS+4*MAX_NUM_DECKS];
#ifdef FCS_WITH_TALONS
    fcs_card_t * talon;
    char talon_params[4];
#endif
};

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

#ifdef FCS_WITH_TALONS
extern int fcs_talon_compare_with_context(const void * s1, const void * s2, fcs_compare_context_t context);
#endif

#ifdef FCS_WITH_TALONS
static const char * const fc_solve_talon_prefixes[] = { "Talon:", "Queue:" , ""};
static const char * const fc_solve_num_redeals_prefixes[] = { "Num-Redeals:", "Readels-Num:", "Readeals-Number:", ""};
#endif

#ifdef FCS_WITH_TALONS
    if (talon_type == FCS_TALON_KLONDIKE)
    {
        fcs_klondike_talon_num_redeals_left(ret) = -1;
    }
#endif

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

#define fcs_freecell_rank(state, f) \
    ( fcs_card_rank(fcs_freecell_card((state),(f))) )

#ifdef FCS_RCS_STATES

#define FCS_STATE_kv_to_pass(s) (s)

#else

#define FCS_STATE_kv_to_pass(s) FCS_STATE_kv_to_collectible(s)

#endif

#ifdef DEBUG_STATES

#define fcs_card_set_suit(card, d) \
    (card).suit = (d)

#define fcs_card_set_rank(card, num) \
    (card).rank = (num)

#else

#define fcs_card_set_rank(card, num) \
    (card) = ((fcs_card_t)(((card)&0x03)|((num)<<2)))

#define fcs_card_set_suit(card, suit) \
    (card) = ((fcs_card_t)(((card)&0xFC)|(suit)))

#endif

#else
extern int fc_solve_state_compare_indirect(const void * s1, const void * s2);
extern int fc_solve_state_compare_indirect_with_context(const void * s1, const void * s2, void * context);
#endif

#if (FCS_STATE_STORAGE != FCS_STATE_STORAGE_INDIRECT)
static GCC_INLINE int fc_solve_state_compare(const void * s1, const void * s2)
{
    return memcmp(s1,s2,sizeof(fcs_state_t));
}
#endif

#define fcs_col_push_col_card(dest_col, src_col, card_idx) \
    fcs_col_push_card((dest_col), fcs_col_get_card((src_col), (card_idx)))

#ifdef DEBUG_STATES

typedef struct
{
    short rank;
    short suit;
} fcs_card_t;

typedef struct
{
    int num_cards;
    fcs_card_t cards[MAX_NUM_CARDS_IN_A_STACK];
} fcs_DEBUG_STATES_stack_t;

typedef fcs_DEBUG_STATES_stack_t *fcs_cards_column_t;
typedef const fcs_DEBUG_STATES_stack_t *fcs_const_cards_column_t;
typedef int fcs_state_foundation_t;

typedef struct
{
    fcs_DEBUG_STATES_stack_t columns[MAX_NUM_STACKS];
    fcs_card_t freecells[MAX_NUM_FREECELLS];
    fcs_state_foundation_t foundations[MAX_NUM_DECKS * 4];
} fcs_state_t;

typedef int fcs_locs_t;

static inline fcs_card_t fcs_make_card(const int rank, const int suit)
{
    fcs_card_t ret = {.rank = rank, .suit = suit};

    return ret;
}
static inline char fcs_card2char(const fcs_card_t card)
{
    return (char)(card.suit | (card.rank << 2));
}
static inline fcs_card_t fcs_char2card(unsigned char c)
{
    return fcs_make_card((c >> 2), (c & 0x3));
}

#define fcs_state_get_col(state, col_idx) (&((state).columns[(col_idx)]))

#define fcs_col_len(col) ((col)->num_cards)

#define fcs_col_get_card(col, c) ((col)->cards[(c)])

#define fcs_card_rank(card) ((card).rank)

#define fcs_card_suit(card) ((int)((card).suit))

#define fcs_freecell_card(state, f) ((state).freecells[(f)])

#define fcs_foundation_value(state, foundation_idx)                            \
    ((state).foundations[(foundation_idx)])

#define fcs_card_is_empty(card) (fcs_card_rank(card) == 0)
#define fcs_card_is_valid(card) (fcs_card_rank(card) != 0)

#endif
static inline fcs_bool_t fcs_is_parent_card__helper(
    const fcs_card_t child, const fcs_card_t parent)
{
    return ((fcs_card_rank(child) + 1 == fcs_card_rank(parent)) &&
            ((fcs_card_suit(child) & 0x1) != (fcs_card_suit(parent) & 0x1)));
}
