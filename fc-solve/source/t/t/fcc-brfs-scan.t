#!/usr/bin/perl

use strict;
use warnings;

use lib './t/lib';

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

use Test::More tests => 10;

use List::MoreUtils qw(any uniq none);

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

    my $canonize_state_sub = sub { 
        my ($state) = @_;
        return join '', sort { $a cmp $b } split/^/, $state;
    };

    my $canonized_state = $canonize_state_sub->(<<'EOF');
Foundations: H-Q C-8 D-5 S-Q 
Freecells:  KD  7D
: KH QC JD TC 9D
: KC
: 
: KS QD JC TD 9C
: 
: 
: 8D
: 6D
EOF

    # TEST
    ok (
        (none { 
            $canonize_state_sub->($_->get_state_string())
                eq 
            $canonized_state
        } 
            @{$obj->states()}
        ),
        "Intermediate states in the FCC are not placed in the list of start points.",
    );

    my $half_move_spec = $obj->compile_move_spec(
        { type => 'stack', idx => 3, },
    );

    # TEST
    ok (
        (none
            {
                (length($_->get_moves()) == 1) && 
                vec($_->get_moves(), 0, 4) == $half_move_spec,
            }
        ),
        "No intermediate states - moves",
    );
}

{
    # MS Freecell Board No. 24 - middle board.
    my $obj = FccStartPointsList->new(
        {
            start => <<'EOF',
Foundations: H-K C-K D-J S-Q 
Freecells:  KD    
: 
: 
: 
: KS QD
: 
: 
: 
: 
EOF
        }
    );

    # TEST*$sanity_check
    $obj->sanity_check();

    # TEST
    is (
        $obj->get_num_new_positions(),
        (
            # count when QD is above KS.
            1 + # Original
            1 + # KD on stack.
            # Now all cards must be on separate cells.
            # 2 Cards are in the freecell - one on a stack.
            3 +
            # 3 cards are on the stacks separately.
            1 +
            # 1 Card is in the freecell
            3
        ),
        "Checking get_num_new_positions for close-to-final state.",
    );
}
