#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "fcs.h"
#include "test_arr.h"

#define NUM_XPARAM 9
#define NUM_YPARAM 3

struct tom_holroyd_context_struct
{
    int Xparam[NUM_XPARAM];
    double Yparam[NUM_YPARAM];
};

typedef struct tom_holroyd_context_struct tom_holroyd_context_t;

static int tom_holroyd_init_instance(
    fcs_derived_states_order_t * order,
    fcs_derived_states_order_instance_t * order_instance,
    const char * spec_string,
    const char * * end_of_spec
    )
{
    tom_holroyd_context_t * context;
    int i;
    const char * s;
    order_instance->order = order;
    context = malloc(sizeof(*context));
    for(i=0;i<5;i++)
    {
        memset(&context, sizeof(*context), 0);
    }
    
    order_instance->context = context;
    s = strchr(spec_string, ')');
    if (s == NULL)
    {
        s = spec_string+strlen(spec_string);
        *end_of_spec = s;
    }
    else
    {
        *end_of_spec = s+1;
    }

    {
        int x_idx;
        s = spec_string;
        for(x_idx=0;x_idx<NUM_XPARAM;x_idx++)
        {
            context->Xparam[x_idx] = atoi(s);
            s = strchr(s, ',');
            if (s == NULL)
            {
                break;
            }
            s++;
        }
    }
    if (s)
    {
        int y_idx;
        for(y_idx=0;y_idx<NUM_YPARAM;y_idx++)
        {
            context->Yparam[y_idx] = atof(s);
            s = strchr(s, ',');
            if (s == NULL)
            {
                break;
            }
            s++;
        }
    }

    /*
     * This is done to facilitate the flooring of the value.
     *
     * */
    context->Yparam[2] += 0.5;

    return 0;
}

static void tom_holroyd_free_instance(
    fcs_derived_states_order_instance_t * order_instance
    )
{
    if (order_instance->context)
    {
        free(order_instance->context);
    }
    free(order_instance);
}

typedef struct 
{
    int index;
    int priority;    
} priority_and_index_t;

static int tom_holroyd_order_states(
    fcs_derived_states_order_instance_t * order_instance,
    fcs_state_with_locations_t * src_state,
    fcs_derived_states_list_t * derived_states,
    int * out_indexes
    )
{
    tom_holroyd_context_t c;
    int priority;
    int a, b;
    int num_states;
    fcs_derived_state_t * derived_states_array;
    fcs_state_with_locations_t * d_state;
    fcs_move_stack_t * moves;
    fcs_move_t first_move;
    fcs_card_t parent_card, moved_card, card;
    int src_stack;
    int num_cards;
    int sequences_are_built_by;
    freecell_solver_instance_t * instance;
    int * num_needed_cards;
    int stacks_num, founds_num;
    int c_idx, s_idx, found_val, f_idx;
    int move_type;
    int num_cards_out;
    priority_and_index_t * result, temp_p_and_i;
    
    c = *(tom_holroyd_context_t *)(order_instance->context);

    instance = order_instance->soft_thread->hard_thread->instance;
    sequences_are_built_by = instance->sequences_are_built_by;

    num_states = derived_states->num_states;
    derived_states_array = derived_states->states;
    stacks_num = instance->stacks_num;
    founds_num = instance->decks_num<<2;

#define state (src_state->s)
    /*
     * Calculate the number of needed cards in each card stack.
     * */
    num_needed_cards = calloc(stacks_num, sizeof(num_needed_cards[0]));
    
    for(s_idx=0;s_idx<stacks_num;s_idx++)
    {
        for(c_idx=0;c_idx<fcs_stack_len(state, s_idx);c_idx++)
        {
            card = fcs_stack_card(state, s_idx, c_idx);
            found_val = fcs_foundation_value(state, fcs_card_suit(card));
            /* Todo: support for the second set of foundations - for
             * double-deck games ? */
            if ((found_val+1 == fcs_card_card_num(card)) ||
                (found_val+2 == fcs_card_card_num(card)))
            {
                num_needed_cards[s_idx]++;
            }
        }
    }

    result = malloc(sizeof(result[0]) * num_states);

    for(a=0;a<num_states;a++)
    {
        d_state = derived_states_array[a].ptr_state;
        moves = derived_states_array[a].move_stack;
        first_move = moves->moves[0];
        move_type = fcs_move_get_type(first_move);
        switch(move_type)
        {
            case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
            case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
                /* This is an irreversible move, so we bump its priority */
                /* The original priority is 0 */
                priority = c.Xparam[8];
                break;
            case FCS_MOVE_TYPE_STACK_TO_FREECELL:
                priority = c.Xparam[7];
                break;
            case FCS_MOVE_TYPE_FREECELL_TO_STACK:
                if (fcs_stack_len(state, fcs_move_get_dest_stack(first_move)))
                {
                    priority = c.Xparam[5];
                }
                else
                {
                    priority = c.Xparam[6];
                }
                break;
            case FCS_MOVE_TYPE_STACK_TO_STACK:
                if (fcs_stack_len(state, fcs_move_get_dest_stack(first_move)))
                {
                    priority = c.Xparam[4];
                }
                else
                {
                    priority = c.Xparam[3];
                }
                break;
                
            default:
                priority = 0;
                break;
        }

        /*
         * Check for other irreversible moves and if so - bump their priorities.
         * */
        if ((move_type == FCS_MOVE_TYPE_STACK_TO_STACK) ||
            (move_type == FCS_MOVE_TYPE_STACK_TO_FREECELL))
        {
            src_stack = fcs_move_get_src_stack(first_move);
            num_cards = (fcs_move_get_type(first_move) == FCS_MOVE_TYPE_STACK_TO_STACK) ? fcs_move_get_num_cards_in_seq(first_move) : 1;
            parent_card = fcs_stack_card(state, src_stack, fcs_stack_len(state, src_stack)-(1+num_cards));
            moved_card = fcs_stack_card(state, src_stack, fcs_stack_len(state, src_stack)-(num_cards));
            if (!fcs_is_parent_card(moved_card, parent_card))
            {
                priority += c.Xparam[8];
            }
        }

        if ((move_type == FCS_MOVE_TYPE_STACK_TO_STACK) ||
            (move_type == FCS_MOVE_TYPE_STACK_TO_FREECELL) ||
            (move_type == FCS_MOVE_TYPE_STACK_TO_FOUNDATION))
        {
            src_stack = fcs_move_get_src_stack(first_move);
            priority += c.Xparam[0] * num_needed_cards[src_stack];
            num_cards = (fcs_move_get_type(first_move) == FCS_MOVE_TYPE_STACK_TO_STACK) ? fcs_move_get_num_cards_in_seq(first_move) : 1;
            parent_card = fcs_stack_card(state, src_stack, fcs_stack_len(state, src_stack)-(1+num_cards));
            /*
             * Increase the priority by Xparam[1] if the card that is one 
             * card below the top of the column (i.e: right under the card 
             * that is moved) is immediately needed for the foundations.
             * */
            if (fcs_foundation_value(state, fcs_card_suit(parent_card))+1 == fcs_card_card_num(parent_card))
            {
                priority += c.Xparam[1];
            }
        }
        
        if ((move_type == FCS_MOVE_TYPE_STACK_TO_STACK) ||
            (move_type == FCS_MOVE_TYPE_FREECELL_TO_STACK))
        {
            priority -= c.Xparam[2] * num_needed_cards[fcs_move_get_dest_stack(first_move)];
        }

        num_cards_out = 0;
        for(f_idx = 0;f_idx<founds_num;f_idx++)
        {
            num_cards_out += fcs_foundation_value(d_state->s, f_idx);
        }
        priority += (int)floor((c.Yparam[0] * num_cards_out + c.Yparam[1]) * num_cards_out + c.Yparam[2]);

        /* Assign the priority and index to the result array */
        result[a].priority = priority;
        result[a].index = a;
    }
#undef state

    /* 
     * Now sort the result array based on the priority.
     * */
    for(b=1;b<num_states;b++)
    {
        a = b;

        while(
            (a>0)    &&
            (result[a].priority > result[a-1].priority)
             )
        {
            temp_p_and_i = result[a];
            result[a] = result[a-1];
            result[a-1] = temp_p_and_i;

            a--;
        }
    }

    for(a=0;a<num_states;a++)
    {
        out_indexes[a] = result[a].index;
    }
    
    free(num_needed_cards);
    free(result);

    return 0;
}

static fcs_derived_states_order_instance_t * tom_holroyd_duplicate_instance(
    fcs_derived_states_order_instance_t * order_instance
    )
{
    fcs_derived_states_order_instance_t * ret;
    ret = malloc(sizeof(*ret));
    memcpy(ret, order_instance, sizeof(*ret));
    if (order_instance->context)
    {
        ret->context = malloc(sizeof(tom_holroyd_context_t));
        memcpy(
            ret->context, 
            order_instance->context, 
            sizeof(tom_holroyd_context_t)
            );
    }
    return ret;
}


fcs_derived_states_order_t freecell_solver_tom_holroyd_states_order =
{
    /* Name */
    "random",
    /* init_instance */
    tom_holroyd_init_instance,
    /* give_init_state */
    freecell_solver_random_give_init_state,
    /* free_instance */
    tom_holroyd_free_instance,
    /* order_states */
    tom_holroyd_order_states,
    /* duplicate_instance */
    tom_holroyd_duplicate_instance,
};

