/*
 * ds_order.h - An abstract way to determine the order of the derived states
 * 
 * */
#ifndef __DS_ORDER_H
#define __DS_ORDER_H

#include "move.h"

struct fcs_derived_states_order_struct;

typedef struct fcs_derived_states_order_struct fcs_derived_states_order_t;

struct fcs_derived_states_order_instance_struct;

typedef struct fcs_derived_states_order_instance_struct fcs_derived_states_order_instance_t;

struct freecell_solver_soft_thread_struct;

typedef struct freecell_solver_soft_thread_struct freecell_solver_soft_thread_t;



struct fcs_derived_states_order_struct
{
    char * name;
    int (*init_instance)(
        fcs_derived_states_order_t * order,
        fcs_derived_states_order_instance_t * order_instance,        
        freecell_solver_soft_thread_t * soft_thread,
        char * spec_string,
        char * * end_of_spec
        );
    int (*give_init_state)(
        fcs_derived_states_order_instance_t * order_instance,
        fcs_state_with_locations_t * init_state
        );
    void (*free_instance)(
        fcs_derived_states_order_instance_t * order_instance
        );
    int (*order_states)(
        fcs_derived_states_order_instance_t * order_instance,
        fcs_state_with_locations_t * src_state,
        fcs_derived_states_list_t * derived_states,
        int * * out_indexes
        );
};

struct fcs_derived_states_order_instance_struct
{
    fcs_derived_states_order_t * order;
    freecell_solver_soft_thread_t * soft_thread;
    fcs_state_with_locations_t * init_state;
    void * context;
};

#endif /* #ifndef __DS_ORDER_H */
