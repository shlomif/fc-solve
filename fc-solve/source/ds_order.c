#include <stdlib.h>

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
        *end_of_spec = s;
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

fcs_derived_states_order_name_match_t freecell_solver_states_orders_names[FREECELL_SOLVER_NUM_STATES_ORDERS_NAMES] =
{
    {
        "random(",
        &freecell_solver_random_states_order
    }
};


