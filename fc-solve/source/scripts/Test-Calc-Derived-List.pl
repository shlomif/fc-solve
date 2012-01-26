#!/usr/bin/perl

use strict;
use warnings;

use Config;
use Cwd;

package DerivedState;

use Inline (
    C => <<'EOF',
#include "dbm_calc_derived_iface.h"

typedef struct 
{
    char * state_string;
    unsigned char move;
    fcs_bool_t is_reversible_move;
} DerivedState;

SV* get_derived_states_list(char * init_state_s, int perform_horne_prune) {
        AV * results;
        DerivedState* s;
        int count, i;
        fcs_derived_state_debug_t * derived_states, * iter;
        
        fc_solve_user_INTERNAL_calc_derived_states_wrapper(
            init_state_s,
            &count,
            &derived_states,
            perform_horne_prune
        );
        results = (AV *)sv_2mortal((SV *)newAV());

        iter = derived_states;
        for (i=0 ; i < count; i++, iter++)
        {
            SV*      obj_ref = newSViv(0);
            SV*      obj = newSVrv(obj_ref, "DerivedState");

            New(42, s, 1, DerivedState);

            s->state_string = savepv(iter->state_string);
            free(iter->state_string);
            s->move = iter->move;
            s->is_reversible_move = iter->is_reversible_move;

            sv_setiv(obj, (IV)s);
            SvREADONLY_on(obj);
            av_push(results, obj_ref);
        }
    free(derived_states);
    return newRV((SV *)results);
}

char* get_state_string(SV* obj) {
    return ((DerivedState*)SvIV(SvRV(obj)))->state_string;
}

int get_move(SV* obj) {
    return (((DerivedState*)SvIV(SvRV(obj)))->move + (int)0);
}

int get_is_reversible_move(SV* obj) {
  return ((DerivedState*)SvIV(SvRV(obj)))->is_reversible_move;
}

void DESTROY(SV* obj) {
  DerivedState* soldier = (DerivedState*)SvIV(SvRV(obj));
  Safefree(soldier->state_string);
  Safefree(soldier);
}
EOF
    CLEAN_AFTER_BUILD => 0,
    INC => "-I" . Cwd::getcwd(),
    LIBS => "-L" . Cwd::getcwd() . " -lfcs_dbm_calc_derived_test",
    # LDDLFLAGS => "$Config{lddlflags} -L$FindBin::Bin -lfcs_delta_states_test", 
    # CCFLAGS => "-L$FindBin::Bin -lfcs_delta_states_test", 
    # MYEXTLIB => "$FindBin::Bin/libfcs_delta_states_test.so",
);

package main;

use IO::All;

my ($start_fn, $perform_horne_prune) = @ARGV;

# MS Freecell Board No. 24.
my $derived_states_list = DerivedState::get_derived_states_list(
    scalar(io($start_fn)->slurp),
    $perform_horne_prune
);

foreach my $obj (@$derived_states_list)
{
    printf ("<<<%s>>>\nReversible: %d\nMove: %.2x\n\n-------------\n\n", $obj->get_state_string(), $obj->get_is_reversible_move(), $obj->get_move());
}

__DATA__
__C__
