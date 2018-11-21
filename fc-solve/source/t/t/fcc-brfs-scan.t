#!/usr/bin/perl

use strict;
use warnings;
use Test::More;
use FC_Solve::Paths qw/ dll_file /;

BEGIN
{
    if ( not -f dll_file('fcs_fcc_brfs_test') )
    {
        plan skip_all => "Test object shared object not found - incompatible.";
    }
    plan tests => 12;
}

package FccStartPoint;

use FC_Solve::InlineWrap (
    C => <<"EOF",
#include "fcc_brfs_test.h"

typedef struct
{
    char * state_as_string;
    SV * moves;
    long num_new_positions;
} FccStartPoint;

AV * find_fcc_start_points(char * init_state_s, SV * moves_prefix) {
    fcs_FCC_start_point_result * fcc_start_points;
    long num_new_positions;
    STRLEN count_start_moves = SvLEN(moves_prefix);

    fc_solve_user_INTERNAL_find_fcc_start_points(
        FCS_DBM_VARIANT_2FC_FREECELL,
        init_state_s,
        (int)count_start_moves,
        SvPVbyte(moves_prefix, count_start_moves),
        &fcc_start_points,
        &num_new_positions
    );
    AV *const results = (AV *)sv_2mortal((SV *)newAV());

    for (var_AUTO(iter, fcc_start_points) ; iter->count ; iter++)
    {
        SV *const obj_ref = newSViv(0);
        SV *const obj = newSVrv(obj_ref, "FccStartPoint");

        FccStartPoint* s;
        New(42, s, 1, FccStartPoint);

        s->state_as_string = savepv(iter->state_as_string);
        s->moves = newSVpvn(iter->moves, iter->count);
        s->num_new_positions = num_new_positions;

        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        av_push(results, obj_ref);
    }
    fc_solve_user_INTERNAL_free_fcc_start_points(fcc_start_points);
    return results;
}

static inline FccStartPoint * deref(SV * const obj) {
    return ((FccStartPoint *)SvIV(SvRV(obj)));
}

char* get_state_string(SV* obj) {
    return deref(obj)->state_as_string;
}

SV* get_moves(SV* obj) {
    SV * ret = newSV(0);
    SvSetSV(ret, (deref(obj)->moves));
    return ret;
}

long get_num_new_positions(SV* obj) {
    return deref(obj)->num_new_positions;
}

void DESTROY(SV* obj) {
  FccStartPoint* s = deref(obj);
  Safefree(s->state_as_string);
  sv_free(s->moves);
  Safefree(s);
}
EOF
    l => '-lfcs_fcc_brfs_test',
);

package FccIsNew;

use FC_Solve::InlineWrap (
    C => <<"EOF",
#include "fcc_brfs_test.h"

typedef char * * string_list_t;

static string_list_t av_to_char_p_p(AV * av)
{
    const int len = av_len(av)+1;
    string_list_t ret;
    Newx(ret, (sizeof(ret[0]) * (len+1)), char *);

    for (int i = 0; i < len ; i++)
    {
        SV * * item = av_fetch(av, i, FALSE);

        assert(item);
        ret[i] = savepv(SvPVbyte_nolen((*item)));
    }
    ret[len] = NULL;
    return ret;
}

static void free_char_p_p(string_list_t p)
{
    string_list_t p_iter = p;
    while ((*p_iter))
    {
        Safefree(*p_iter);
        p_iter++;
    }
    Safefree(p);
}

int is_fcc_new(char * init_state_s, char * start_state_s, AV * min_states_av, AV * states_in_cache_av) {
    bool ret;
    string_list_t min_states = av_to_char_p_p(min_states_av);
    string_list_t states_in_cache = av_to_char_p_p(states_in_cache_av);

    fc_solve_user_INTERNAL_is_fcc_new(
        FCS_DBM_VARIANT_2FC_FREECELL,
        init_state_s,
        start_state_s,
        min_states,
        states_in_cache,
        &ret
    );

    free_char_p_p(min_states);
    free_char_p_p(states_in_cache);

    return ret;
}

EOF
    l => '-lfcs_fcc_brfs_test',
);

sub is_fcc_new_named_args
{
    my ($args) = @_;

    return is_fcc_new(
        @{$args}{qw(init_state start_state min_states states_in_cache)} );
}

package FccStartPointsList;

use parent 'FC_Solve::SingleMoveSearch';

use List::MoreUtils qw/ uniq /;

use Test::More;

__PACKAGE__->mk_acc_ref(
    [
        qw(
            states
            )
    ]
);

sub _init
{
    my $self = shift;
    my $args = shift;

    my $moves_prefix = $args->{moves_prefix} || '';

    $self->states(
        FccStartPoint::find_fcc_start_points( $args->{start}, $moves_prefix, )
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
    is(
        scalar( uniq map { $_->get_state_string() } @$fcc_start_points_list ),
        scalar(@$fcc_start_points_list),
        'The states are unique',
    );

    # TEST:$sanity_check++;
    is(
        scalar( uniq map { $_->get_moves() } @$fcc_start_points_list ),
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

use List::MoreUtils qw/ any none /;

{
    # MS Freecell Board No. 24.
    my $obj = FccStartPointsList->new(
        {
            start => <<"EOF",
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
            start => <<"EOF",
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
    ok(
        ( any { $_->get_state_string() =~ m/^: 8D$/ms } @{ $obj->states() } ),
        "Horne prune did not take effect (found intermediate state)"
    );

    my $canonize_state_sub = sub {
        my ($state) = @_;
        return join '', sort { $a cmp $b } split /^/, $state;
    };

    my $canonized_state = $canonize_state_sub->(<<"EOF");
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
    ok(
        (
            none
            {
                $canonize_state_sub->( $_->get_state_string() ) eq
                    $canonized_state
            }
            @{ $obj->states() }
        ),
"Intermediate states in the FCC are not placed in the list of start points.",
    );

    my $half_move_spec =
        $obj->compile_move_spec( { type => 'stack', idx => 3, }, );

    # TEST
    ok(
        (
            none
            {
                ( length( $_->get_moves() ) == 1 )
                    && vec( $_->get_moves(), 0, 4 ) == $half_move_spec,
            }
        ),
        "No intermediate states - moves",
    );
}

{
    # MS Freecell Board No. 24 - middle board.
    my $obj = FccStartPointsList->new(
        {
            start => <<"EOF",
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
    is(
        $obj->get_num_new_positions(),
        (
            # count when QD is above KS.
            1 +        # Original
                1 +    # KD on stack.
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

{
    my $init_state_s = <<"EOF";
4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
EOF

    my $start_state_s = <<"EOF";
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

    # Empty.
    my @min_states;

    # Empty;
    my @states_in_cache;

    # TEST
    ok(
        FccIsNew::is_fcc_new(
            $init_state_s, $start_state_s, \@min_states, \@states_in_cache
        ),
        'State with empty cache and min_states is new',
    );

    # TEST
    ok(
        FccIsNew::is_fcc_new_named_args(
            {
                init_state      => $init_state_s,
                start_state     => $start_state_s,
                min_states      => \@min_states,
                states_in_cache => \@states_in_cache,
            }
        ),
        'Meta-test for FccIsNew::is_fcc_named_args()',
    );

    # Broke due to only putting the so-far-minimal states in the cache.
    if (0)
    {
        #CANCELED_TEST
        ok(
            (
                !FccIsNew::is_fcc_new_named_args(
                    {
                        init_state      => $init_state_s,
                        start_state     => $start_state_s,
                        min_states      => [],
                        states_in_cache => [<<"EOF"],
Foundations: H-K C-K D-J S-Q
Freecells:  KD
:
:
: QD
: KS
:
:
:
:
EOF
                    }
                )
            ),
            'Testing that it returns FALSE if state is in the cache.',
        );
    }
}

# Disabling because the minimal state is different on the DeBondt and
# the regular delta_states.c encodings.
if (0)
{
    my $init_state_s = <<"EOF";
4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
EOF

    my $start_state_s = <<"EOF";
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

    # The minimal state in the FCC as determined by gdb.
    my $min_state_s = <<"EOF";
Foundations: H-Q C-8 D-5 S-Q
Freecells:  6D  KH
:
:
:
:
: 7D
: KC
: KD QC JD TC 9D
: KS QD JC TD 9C 8D
EOF

    ## CANCELLED TEST
    ok(
        (
            !FccIsNew::is_fcc_new_named_args(
                {
                    init_state  => $init_state_s,
                    start_state => $start_state_s,
                    min_states  => [$min_state_s],

                    # No states in the cache, because we are not interested in
                    # testing it.
                    states_in_cache => [],
                }
            )
        ),
        'Min state present returns FALSE.',
    );
}
