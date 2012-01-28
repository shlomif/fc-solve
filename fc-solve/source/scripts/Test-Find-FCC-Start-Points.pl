#!/usr/bin/perl

use strict;
use warnings;

use Config;
use Cwd;

package FccStartPoint;

use Inline (
    C => <<'EOF',
#include "fcc_brfs_test.h"

typedef struct 
{
    char * state_as_string;
    SV * moves;
} FccStartPoint;

SV* find_fcc_start_points(char * init_state_s, SV * moves_prefix) {
    AV * results;
    FccStartPoint* s;
    int count, i;
    fcs_FCC_start_point_result_t * fcc_start_points, * iter;

    STRLEN count_start_moves = SvLEN(moves_prefix);
    
    fc_solve_user_INTERNAL_find_fcc_start_points(
        init_state_s,
        (int)count_start_moves,
        SvPVbyte(moves_prefix, count_start_moves),
        &fcc_start_points
    );
    results = (AV *)sv_2mortal((SV *)newAV());
    
    for (iter = fcc_start_points ; (*iter).count_moves ; iter++)
    {
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "FccStartPoint");

        New(42, s, 1, FccStartPoint);

        s->state_as_string = savepv(iter->state_as_string);
        free(iter->state_as_string);
        s->moves = newSVpvn(iter->moves, iter->count_moves);
        free(iter->moves);

        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        av_push(results, obj_ref);
    }
    free(fcc_start_points);
    return newRV((SV *)results);
}

char* get_state_string(SV* obj) {
    return ((FccStartPoint*)SvIV(SvRV(obj)))->state_as_string;
}

SV* get_moves(SV* obj) {
    SV * ret = newSV(0);
    SvSetSV(ret, (((FccStartPoint*)SvIV(SvRV(obj)))->moves));
    return ret;
}

void DESTROY(SV* obj) {
  FccStartPoint* s = (FccStartPoint*)SvIV(SvRV(obj));
  Safefree(s->state_as_string);
  sv_free(s->moves);
  Safefree(s);
}
EOF
    CLEAN_AFTER_BUILD => 0,
    INC => "-I" . Cwd::getcwd(),
    LIBS => "-L" . Cwd::getcwd() . " -lfcs_fcc_brfs_test",
    # LDDLFLAGS => "$Config{lddlflags} -L$FindBin::Bin -lfcs_delta_states_test", 
    # CCFLAGS => "-L$FindBin::Bin -lfcs_delta_states_test", 
    # MYEXTLIB => "$FindBin::Bin/libfcs_delta_states_test.so",
);

package main;

use IO::All;

my ($start_fn) = @ARGV;

# MS Freecell Board No. 24.
my $derived_states_list = FccStartPoint::find_fcc_start_points(
    scalar(io($start_fn)->slurp),
    ''
);

foreach my $obj (@$derived_states_list)
{
    print sprintf("<<<%s>>>\nLeading Moves: %s\n\n-------------\n\n",
        $obj->get_state_string(),
        (map { sprintf("{%.2x}", ord($_)) } split//, $obj->get_moves()),
    );
}

print "[[[END]]]\n";
__DATA__
__C__
