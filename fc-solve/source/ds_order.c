#include <stdlib.h>
#include <string.h>

#include "fcs.h"
#include "rand.h"

static int random_init_instance(
    fcs_derived_states_order_t * order,
    fcs_derived_states_order_instance_t * order_instance,
    const char * spec_string,
    const char * * end_of_spec
    )
{
    order_instance->order = order;
    order_instance->context = NULL;
    {
        const char * s;

        s = spec_string;
        while ((*s) && (*s != ')'))
        {
            s++;
        }
        *end_of_spec = (*s == '\0') ? s : (s+1);
    }

    return 0;
}

static int random_give_init_state(
    fcs_derived_states_order_instance_t * order_instance,
    freecell_solver_soft_thread_t * soft_thread,
    fcs_state_with_locations_t * init_state
    )
{
    order_instance->soft_thread = soft_thread;
    return 0;
}

static void random_free_instance(
    fcs_derived_states_order_instance_t * order_instance
    )
{
    free(order_instance);
}

static int random_order_states(
    fcs_derived_states_order_instance_t * order_instance,
    fcs_state_with_locations_t * src_state,
    fcs_derived_states_list_t * derived_states,
    int * rand_array
    )
{
    int a, j, swap_save;
    int * ra_ptr;
    int num_states = derived_states->num_states;
    fcs_rand_t * rand_gen;

    for(a=0, ra_ptr = rand_array; a < num_states ; a++)
    {
        *(ra_ptr++) = a;
    }

    rand_gen = order_instance->soft_thread->rand_gen;

    a = num_states-1;
    while (a > 0)
    {
        j =
            (
                freecell_solver_rand_get_random_number(
                    rand_gen
                )
                % (a+1)
            );

        swap_save = rand_array[a];
        rand_array[a] = rand_array[j];
        rand_array[j] = swap_save;
        a--;
    }

    return 0;
}


fcs_derived_states_order_t freecell_solver_random_states_order = 
{
    /* Name */
    "random",
    /* init_instance */
    random_init_instance,
    /* give_init_state */
    random_give_init_state,
    /* free_instance */
    random_free_instance,
    /* order_states */
    random_order_states   
};

/***********************************/
/* The shlomif States Order */
/***********************************/
 
struct shlomif_context_struct
{
    double a_star_weights[5];
};

typedef struct shlomif_context_struct shlomif_context_t;

static const double a_star_default_weights[5] = {0.5,0,0.3,0,0.2
};

static int shlomif_init_instance(
    fcs_derived_states_order_t * order,
    fcs_derived_states_order_instance_t * order_instance,
    const char * spec_string,
    const char * * end_of_spec
    )
{
    shlomif_context_t * context;
    int i;
    const char * s;
    order_instance->order = order;
    context = malloc(sizeof(*context));
    for(i=0;i<5;i++)
    {
        context->a_star_weights[i] = a_star_default_weights[i];
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
        int a;
        char * start_num;
        char * end_num;
        char save;
        char * arg_copy;

        arg_copy = malloc(s-spec_string+1);
        strncpy(arg_copy, spec_string, s-spec_string);
        arg_copy[s-spec_string] = '\0';

        start_num = arg_copy;
        for(a=0;a<5;a++)
        {
            while ((*start_num > '9') && (*start_num < '0') && (*start_num != '\0'))
            {
                start_num++;
            }
            if (*start_num == '\0')
            {
                break;
            }
            end_num = start_num+1;
            while ((((*end_num >= '0') && (*end_num <= '9')) || (*end_num == '.')) && (*end_num != '\0'))
            {
                end_num++;
            }
            save = *end_num;
            *end_num = '\0';
            context->a_star_weights[a] = atof(start_num);
            *end_num = save;
            start_num=end_num+1;
        }
        free(arg_copy);
    }

    /* Normalize the Weights */
    {
        double sum;
        sum = 0;
        for(i=0;i<5;i++)
        {
            sum += context->a_star_weights[i];
        }
        for(i=0;i<5;i++)
        {
            context->a_star_weights[i] /= sum;
        }
    }

    return 0;
}

static void shlomif_free_instance(
    fcs_derived_states_order_instance_t * order_instance
    )
{
    if (order_instance->context)
    {
        free(order_instance->context);
    }
    free(order_instance);
}

static int shlomif_order_states(
    fcs_derived_states_order_instance_t * order_instance,
    fcs_state_with_locations_t * src_state,
    fcs_derived_states_list_t * derived_states,
    int * out_indexes
    )
{
    int i;
    PQUEUE pqueue;
    int num_states;
    freecell_solver_soft_thread_t * soft_thread;
    double * a_star_weights;
    fcs_derived_state_t * derived_states_states;

    num_states = derived_states->num_states;
    soft_thread = order_instance->soft_thread;
    a_star_weights = ((shlomif_context_t * )order_instance->context)->a_star_weights;
    derived_states_states = derived_states->states;

    freecell_solver_PQueueInitialise(&pqueue, num_states);

    for(i=0;i<num_states;i++)
    {
        freecell_solver_PQueuePush(
            &pqueue, 
            (void *)i,
            freecell_solver_a_star_rate_state(
                soft_thread,
                a_star_weights,
                derived_states_states[i].ptr_state
                )
            );
    }
    for(i=0;i<num_states;i++)
    {
        out_indexes[i] = (int)freecell_solver_PQueuePop(&pqueue);
    }

    freecell_solver_PQueueFree(&pqueue);

    return 0;
}


static fcs_derived_states_order_t shlomif_states_order = 
{
    /* Name */
    "random",
    /* init_instance */
    shlomif_init_instance,
    /* give_init_state */
    random_give_init_state,
    /* free_instance */
    shlomif_free_instance,
    /* order_states */
    shlomif_order_states
};

fcs_derived_states_order_name_match_t freecell_solver_states_orders_names[FREECELL_SOLVER_NUM_STATES_ORDERS_NAMES] =
{
    {
        "random(",
        &freecell_solver_random_states_order
    },
    {
        "shlomif(",
        &shlomif_states_order
    }
    
};


