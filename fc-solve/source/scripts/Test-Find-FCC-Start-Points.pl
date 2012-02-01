#!/usr/bin/perl

use strict;
use warnings;

use lib './t/t/lib';

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
    long num_new_positions;
} FccStartPoint;

SV* find_fcc_start_points(char * init_state_s, SV * moves_prefix) {
    AV * results;
    FccStartPoint* s;
    int count, i;
    fcs_FCC_start_point_result_t * fcc_start_points, * iter;
    long num_new_positions;

    STRLEN count_start_moves = SvLEN(moves_prefix);
    
    fc_solve_user_INTERNAL_find_fcc_start_points(
        init_state_s,
        (int)count_start_moves,
        SvPVbyte(moves_prefix, count_start_moves),
        &fcc_start_points,
        &num_new_positions
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
        s->num_new_positions = num_new_positions;

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

long get_num_new_positions(SV* obj) {
    return ((FccStartPoint*)SvIV(SvRV(obj)))->num_new_positions;
}

void DESTROY(SV* obj) {
  FccStartPoint* s = (FccStartPoint*)SvIV(SvRV(obj));
  Safefree(s->state_as_string);
  sv_free(s->moves);
  Safefree(s);
}
EOF
    CLEAN_AFTER_BUILD => 0,
    INC => "-I" . $ENV{FCS_PATH},
    LIBS => "-L" . $ENV{FCS_PATH} . " -lfcs_fcc_brfs_test",
    # LDDLFLAGS => "$Config{lddlflags} -L$FindBin::Bin -lfcs_delta_states_test", 
    # CCFLAGS => "-L$FindBin::Bin -lfcs_delta_states_test", 
    # MYEXTLIB => "$FindBin::Bin/libfcs_delta_states_test.so",
);

package FccStartPointsList;

use base 'Games::Solitaire::FC_Solve::SingleMoveSearch';

use List::MoreUtils qw(any uniq);

use Test::More;

__PACKAGE__->mk_acc_ref([qw(
    states
    )]
);

sub _init
{
    my $self = shift;
    my $args = shift;

    my $moves_prefix = $args->{moves_prefix} || '';

    $self->states(
        FccStartPoint::find_fcc_start_points(
            $args->{start}, $moves_prefix,
        )
    );

    return;
}

# TEST:$sanity_check=0;
sub sanity_check
{
    my ($self) = @_;

    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $fcc_start_points_list = $self->states();

    # TEST:$sanity_check++;
    is (
        scalar(uniq map { $_->get_state_string() } @$fcc_start_points_list),
        scalar(@$fcc_start_points_list),
        'The states are unique',
    );

    # TEST:$sanity_check++;
    is (
        scalar(uniq map { $_->get_moves() } @$fcc_start_points_list),
        scalar(@$fcc_start_points_list),
        'The states are unique',
    );
}

sub get_num_new_positions 
{
    my $self = shift;

    return $self->states->[0]->get_num_new_positions;
}

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
        do { my $s = $obj->get_state_string(); $s =~ s/\n/\\n/g; $s; },
        (join '', map { sprintf("{%.2x}", ord($_)) } split//, $obj->get_moves()),
    );
}

print "[[[END]]]\n";
__DATA__
__C__
