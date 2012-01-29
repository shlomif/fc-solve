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
    INC => "-I" . $ENV{FCS_PATH},
    LIBS => "-L" . $ENV{FCS_PATH} . " -lfcs_fcc_brfs_test",
    # LDDLFLAGS => "$Config{lddlflags} -L$FindBin::Bin -lfcs_delta_states_test", 
    # CCFLAGS => "-L$FindBin::Bin -lfcs_delta_states_test", 
    # MYEXTLIB => "$FindBin::Bin/libfcs_delta_states_test.so",
);

package FccStartPointsList;

use base 'Games::Solitaire::Verify::Base';

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

package main;

use Test::More tests => 5;

use List::MoreUtils qw(any uniq);

{
    # MS Freecell Board No. 24.
    my $obj = FccStartPointsList->new(
        {
            start => <<'EOF',
4C 2C 9C 8C QS 4S 2H 
5H QH 3C AC 3H 4H QD 
QC 9S 6H 9H 3S KS 3D 
5D 2S JC 5C JH 6D AS 
2D KD TH TC TD 8D 
7H JS KH TS KC 7C 
AH 5S 6S AD 8H JD 
7S 6C 7D 4D 8S 9D 
EOF
        }
    );

    # TEST*$sanity_check
    $obj->sanity_check();
}

# Testing the horne's prune is not applied.
{
    # MS Freecell Board No. 24 - middle board.
    my $obj = FccStartPointsList->new(
        {
            start => <<'EOF',
Foundations: H-Q C-8 D-5 S-Q 
Freecells:  KD  7D
: KH QC JD TC 9D
: KC
: 
: KS QD JC TD 9C 8D
: 
: 
: 
: 6D
EOF
        }
    );

    # TEST*$sanity_check
    $obj->sanity_check();

    # TEST
    ok (
        (any { $_->get_state_string() =~ m/^: 8D$/ms } @{$obj->states()}),
        "Horne prune did not take effect (found intermediate state)"
    );
}
