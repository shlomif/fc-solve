#include "fcs.h"

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
        memset(&contest, sizeof(context), 0);
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
            s = strchr(s, ",");
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
        for(y_idx;y_idx<NUM_YPARAM;y_idx++)
        {
            context->Yparam[y_idx] = atof(s);
            s = strchr(s, ",");
            if (s == NULL)
            {
                break;
            }
            s++;
        }
    }

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

static int tom_holroyd_order_states(
    fcs_derived_states_order_instance_t * order_instance,
    fcs_state_with_locations_t * src_state,
    fcs_derived_states_list_t * derived_states,
    int * out_indexes
    )
{

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
    tom_holroyd_order_states   
};

