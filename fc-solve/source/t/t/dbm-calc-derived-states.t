#!/usr/bin/perl

use strict;
use warnings;

use Test::More;

BEGIN
{
    if (-f "$ENV{FCS_PATH}/libfcs_dbm_calc_derived_test.so")
    {
        plan tests => 30;
    }
    else
    {
        plan skip_all => "Test object shared object not found - incompatible.";
    }
}

use lib './t/lib';

use Config;
use Cwd;

package DerivedState;

use Config;

use Inline (
    C => <<"EOF",
#include "dbm_calc_derived_iface.h"

typedef struct
{
    char * state_string;
    unsigned char move;
    int core_irreversible_moves_count;
    int num_non_reversible_moves_including_prune;
    unsigned char which_irreversible_moves_bitmask[RANK_KING];
} DerivedState;

SV * perl_perform_horne_prune(char * init_state_s) {
    AV * results;
    char * ret_state_s;
    int ret_count;

    results = (AV *)sv_2mortal((SV *)newAV());

    ret_count = fc_solve_user_INTERNAL_perform_horne_prune(FCS_DBM_VARIANT_2FC_FREECELL, init_state_s, &ret_state_s);

    av_push(results, newSViv(ret_count));
    av_push(results, newSVpv(ret_state_s, 0));

    free(ret_state_s);

    return newRV((SV *)results);
}

SV* get_derived_states_list(char * init_state_s, int perform_horne_prune) {
        AV * results;
        DerivedState* s;
        int count, i;
        fcs_derived_state_debug_t * derived_states, * iter;

        fc_solve_user_INTERNAL_calc_derived_states_wrapper(
            FCS_DBM_VARIANT_2FC_FREECELL,
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
            s->core_irreversible_moves_count = iter->core_irreversible_moves_count;
            s->num_non_reversible_moves_including_prune = iter->num_non_reversible_moves_including_prune;
            memcpy(s->which_irreversible_moves_bitmask,
                iter->which_irreversible_moves_bitmask,
                sizeof(s->which_irreversible_moves_bitmask)
            );

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

int get_core_irreversible_moves_count(SV* obj) {
  return ((DerivedState*)SvIV(SvRV(obj)))->core_irreversible_moves_count;
}

int get_num_non_reversible_moves_including_prune(SV* obj) {
  return ((DerivedState*)SvIV(SvRV(obj)))->num_non_reversible_moves_including_prune;
}

SV * get_which_irreversible_moves_bitmask(SV* obj) {
  DerivedState* s = ((DerivedState*)SvIV(SvRV(obj)));
  return newSVpvn(s->which_irreversible_moves_bitmask, sizeof(s->which_irreversible_moves_bitmask));
}

void DESTROY(SV* obj) {
  DerivedState* soldier = (DerivedState*)SvIV(SvRV(obj));
  Safefree(soldier->state_string);
  Safefree(soldier);
}
EOF
    CLEAN_AFTER_BUILD => 0,
    INC => "-I" . $ENV{FCS_PATH} . " -I" . $ENV{FCS_SRC_PATH},
    LIBS => "-L" . $ENV{FCS_PATH} . " -lfcs_dbm_calc_derived_test",
    CCFLAGS => "$Config{ccflags} -std=gnu99",
    # LDDLFLAGS => "$Config{lddlflags} -L$FindBin::Bin -lfcs_delta_states_test",
    # CCFLAGS => "-L$FindBin::Bin -lfcs_delta_states_test",
    # MYEXTLIB => "$FindBin::Bin/libfcs_delta_states_test.so",
);

package DerivedStatesList;

use base 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref([qw(
    states
    )]
);

sub _init
{
    my $self = shift;
    my $args = shift;

    $self->states(
        DerivedState::get_derived_states_list(
            @{$args}{qw(start perform_horne_prune)}
        )
    );

    return;
}

sub find_by_string
{
    my $self = shift;
    my $derived_state_s = shift;

    return DerivedStatesSearch->new({
        states => [grep { $_->get_state_string() eq $derived_state_s } @{$self->states}]
    });
}

package DerivedStatesSearch;

use base 'Games::Solitaire::FC_Solve::SingleMoveSearch';

use Test::More;

__PACKAGE__->mk_acc_ref([qw(
    states
    )]
);

sub _init
{
    my $self = shift;
    my $args = shift;

    $self->states(
        $args->{states}
    );

    return;
}

sub has_one
{
    my ($self,$blurb) = @_;

    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return is( scalar(@{$self->states()}), 1, $blurb);
}

sub count_irreversible_moves_is
{
    my ($self, $exp_count, $blurb) = @_;

    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return is(
        ($self->states->[0]->get_core_irreversible_moves_count()),
        ($exp_count),
        $blurb
    );
}

sub is_reversible
{
    my ($self, $is_reversible, $blurb) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return is(
        (!$self->states->[0]->get_core_irreversible_moves_count()),
        (!!$is_reversible),
        $blurb
    );
}

sub _move
{
    my ($self) = @_;

    return $self->states->[0]->get_move();
}

sub _move_half
{
    my ($self, $which) = @_;

    return $self->calc_move_half($self->_move(), $which);
}

sub is_src
{
    my ($self, $src_spec, $blurb) = @_;

    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return is (
        $self->_move_half(0),
        $self->compile_move_spec($src_spec),
        $blurb
    );
}

sub is_dest
{
    my ($self, $dest_spec, $blurb) = @_;

    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return is (
        $self->_move_half(1),
        $self->compile_move_spec($dest_spec),
        $blurb
    );
}

package main;

my $WS = ' ';

use Test::More;

my $TRUE = 1;
my $FALSE = 0;

{
    my $freecell_24_initial_layout_s = <<"EOF";
4C 2C 9C 8C QS 4S 2H$WS
5H QH 3C AC 3H 4H QD$WS
QC 9S 6H 9H 3S KS 3D$WS
5D 2S JC 5C JH 6D AS$WS
2D KD TH TC TD 8D$WS
7H JS KH TS KC 7C$WS
AH 5S 6S AD 8H JD$WS
7S 6C 7D 4D 8S 9D$WS
EOF

    my $fc_24_init = DerivedStatesList->new(
        {
            start => $freecell_24_initial_layout_s,
            perform_horne_prune => 0,
        }
    );

    {
        my $results = $fc_24_init->find_by_string(<<"EOF"
Foundations: H-0 C-0 D-0 S-0$WS
Freecells:      9D
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S
EOF
        );

        # TEST
        $results->has_one('9D to freecell has one result.');

        # TEST
        $results->is_reversible($FALSE, '9D to freecell is not reversible');

        # TEST
        $results->is_src({ type => 'stack', idx => 7, }, '9D to freecell is from stack No. 7');

        # TEST
        $results->is_dest({ type => 'freecell', idx => 1, }, '9D to freecell is to freecell No. 1');
    }

    # Check move from stack to foundations
    {
        my $results = $fc_24_init->find_by_string(<<"EOF"
Foundations: H-0 C-0 D-0 S-A$WS
Freecells:$WS$WS$WS$WS$WS$WS$WS$WS
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF
        );

        my $blurb_base = "Stack->Foundations Move";
        # TEST
        $results->has_one("$blurb_base has one.");

        # TEST
        $results->is_reversible($FALSE, "$blurb_base is not reversible.");

        # TEST
        $results->is_src({ type => 'stack', idx => 3, },
                "$blurb_base src");

        # TEST
        $results->is_dest({ type => 'found', idx => 3, },
                "$blurb_base dest");
    }
}

{
    my $freecell_24_middle_layout = <<"EOF";
Foundations: H-Q C-8 D-8 S-Q$WS
Freecells:  KD$WS$WS$WS$WS
: KH QC JD TC 9D
: KC
:$WS
: KS QD JC TD 9C
:$WS
:$WS
:$WS
:$WS
EOF

    my $fc_24 = DerivedStatesList->new(
        {
            start => $freecell_24_middle_layout,
            perform_horne_prune => 0,
        }
    );

    {
        my $results = $fc_24->find_by_string(<<"EOF"
Foundations: H-Q C-8 D-8 S-Q$WS
Freecells:$WS$WS$WS$WS$WS$WS$WS$WS
: KH QC JD TC 9D
: KC
:$WS
: KS QD JC TD 9C
:$WS
:$WS
:$WS
: KD
EOF
        );

        # TEST
        $results->has_one('KD from Freecell has exactly one result');

        # TEST
        $results->is_reversible($TRUE, 'KD from Freecell is reversible');

        # TEST
        $results->is_src({ type => 'freecell', idx => 0, },
            'from freecell 0');

        # TEST
        $results->is_dest({ type => 'stack', idx => 7, },
            'to stack No. 7');
    }
}

# Check that to-empty-stack moves are reversible dependening on the
# originating item
{
    my $freecell_24_middle_layout = <<"EOF";
Foundations: H-0 C-0 D-0 S-2$WS
Freecells:  JH  8D
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD JC TD
: QC 9S 6H 9H 3S KS 3D
:$WS
: 2D KD TH
: 7H JS KH TS KC 7C 6D 5C 4D
: AH 5S 6S AD 8H JD TC 9D 8S 7D
: 7S 6C 5D
EOF

    my $fc_24 = DerivedStatesList->new(
        {
            start => $freecell_24_middle_layout,
            perform_horne_prune => 0,
        }
    );

    {
        my $results = $fc_24->find_by_string(<<"EOF"
Foundations: H-0 C-0 D-0 S-2$WS
Freecells:  JH  8D
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD JC
: QC 9S 6H 9H 3S KS 3D
: TD
: 2D KD TH
: 7H JS KH TS KC 7C 6D 5C 4D
: AH 5S 6S AD 8H JD TC 9D 8S 7D
: 7S 6C 5D
EOF
        );

        my $blurb_base = "Moving from parent to empty";
        # TEST
        $results->has_one("$blurb_base has one");

        # TEST
        $results->is_reversible($TRUE, "$blurb_base is reversible");

        # TEST
        $results->is_src({ type => 'stack', idx => 1, },
            "$blurb_base - from stack No. 1");

        # TEST
        $results->is_dest({ type => 'stack', idx => 3, },
            "$blurb_base - to stack No. 3");
    }

    {
        my $results = $fc_24->find_by_string(<<"EOF"
Foundations: H-0 C-0 D-0 S-2$WS
Freecells:  JH  8D
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD JC TD
: QC 9S 6H 9H 3S KS
: 3D
: 2D KD TH
: 7H JS KH TS KC 7C 6D 5C 4D
: AH 5S 6S AD 8H JD TC 9D 8S 7D
: 7S 6C 5D
EOF
        );

        my $blurb_base = "Moving from non-seq-parent to empty column";
        # TEST
        $results->has_one("$blurb_base has one");

        # TEST
        $results->is_reversible($FALSE, "$blurb_base is not reversible");

        # TEST
        $results->is_src({ type => 'stack', idx => 2, },
            "$blurb_base - from stack No. 1");

        # TEST
        $results->is_dest({ type => 'stack', idx => 3, },
            "$blurb_base - to stack No. 3");
    }
}

# Checking move from freecell to foundation
{
    my $freecell_24_middle_layout = <<"EOF";
Freecells: AS
4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
EOF

    my $fc_24 = DerivedStatesList->new(
        {
            start => $freecell_24_middle_layout,
            perform_horne_prune => 0,
        }
    );

    {
        my $results = $fc_24->find_by_string(<<"EOF"
Foundations: H-0 C-0 D-0 S-A$WS
Freecells:$WS$WS$WS$WS$WS$WS$WS$WS
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF
        );

        my $blurb_base = "Moving from freecell to foundation";
        # TEST
        $results->has_one("$blurb_base has one");

        # TEST
        $results->is_reversible($FALSE, "$blurb_base is not reversible");

        # TEST
        $results->is_src({ type => 'freecell', idx => 0, },
            "$blurb_base - src");

        # TEST
        $results->is_dest({ type => 'found', idx => 3, },
            "$blurb_base - dest");
    }
}

{
    my $freecell_24ish_layout = <<"EOF";
Foundations: H-2 C-0 D-0 S-0$WS
4C 2C 9C 8C QS 4S$WS
5H QH 3C AC 4H QD 3H$WS
QC 9S 6H 9H 3S KS 3D$WS
5D 2S JC 5C JH 6D AS$WS
2D KD TH TC TD 8D$WS
7H JS KH TS KC 7C$WS
5S 6S AD 8H JD$WS
7S 6C 7D 4D 8S 9D$WS
EOF

    my $fc = DerivedStatesList->new(
        {
            start => $freecell_24ish_layout,
            perform_horne_prune => 0,
        }
    );

    {
        my $results = $fc->find_by_string(<<"EOF"
Foundations: H-3 C-0 D-0 S-0$WS
Freecells:$WS$WS$WS$WS$WS$WS$WS$WS
: 4C 2C 9C 8C QS 4S
: 5H QH 3C AC 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF
        );

        # TEST
        $results->has_one('3H to foundation has one result.');

        # TEST
        $results->count_irreversible_moves_is(2,
            '2H to foundation is two irreversible moves because' .
            ' it was not lying on a parent.'
        );

        # TEST
        $results->is_src({ type => 'stack', idx => 1, },
            'From stack #1');

        # TEST
        $results->is_dest({ type => 'found', idx => 0, }, "Foundation.")
    }
}

{
    my $ret = DerivedState::perl_perform_horne_prune(
        <<"EOF",
Foundations: H-A C-0 D-0 S-0
4C 2C 9C 8C QS 3H 4S
5H QH 3C AC 4H QD 2H
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
EOF

    );

    # TEST
    is (
        $ret->[0],
        4,
        "4 irreversible pseudo-moves",
    );

    # TEST
    is (
        $ret->[1],
        <<"EOF"
Foundations: H-2 C-0 D-0 S-A$WS
Freecells:$WS$WS$WS$WS$WS$WS$WS$WS
: 4C 2C 9C 8C QS 3H 4S
: 5H QH 3C AC 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF
    );
}
