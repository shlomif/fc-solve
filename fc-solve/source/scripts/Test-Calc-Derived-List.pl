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

SV* get_derived_states_list(char * init_state_s, char * key_state_s, int perform_horne_prune) {
        AV * results;
        DerivedState* s;
        int count, i;
        fcs_derived_state_debug_t * derived_states, * iter;
        
        fc_solve_user_INTERNAL_calc_derived_states_wrapper(
            init_state_s,
            key_state_s,
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
            s->is_reversible_move = iter->move;

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

# MS Freecell Board No. 24.
my $derived_states_list = DerivedState::get_derived_states_list(
    ((<<'EOF') x 2) , 0);
4C 2C 9C 8C QS 4S 2H 
5H QH 3C AC 3H 4H QD 
QC 9S 6H 9H 3S KS 3D 
5D 2S JC 5C JH 6D AS 
2D KD TH TC TD 8D 
7H JS KH TS KC 7C 
AH 5S 6S AD 8H JD 
7S 6C 7D 4D 8S 9D 
EOF

foreach my $obj (@$derived_states_list)
{
    print $obj->get_state_string(), "\n\n\n";
}

__DATA__
__C__
