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
    INC => "-I" . $ENV{FCS_PATH},
    LIBS => "-L" . $ENV{FCS_PATH} . " -lfcs_dbm_calc_derived_test",
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

use base 'Games::Solitaire::Verify::Base';

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

sub is_reversible
{
    my ($self,$is_reversible, $blurb) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return is(
        (!!$self->states->[0]->get_is_reversible_move()), 
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

    return (($self->_move() >> ($which * 4)) & 0xF);
}

sub _compile_move_spec
{
    my ($self, $move_spec) = @_;

    my $t = $move_spec->{type};
    my $idx = $move_spec->{idx};

    return
    (
          ($t eq "stack") ? $idx
        : ($t eq "freecell") ? ($idx+8)
        : ($t eq "found") ? ($idx+8+4)
        : (die "Unknown type '$t'")
    )
}

sub is_src
{
    my ($self, $src_spec, $blurb) = @_;

    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return is (
        $self->_move_half(0),
        $self->_compile_move_spec($src_spec),
        $blurb
    );
}

sub is_dest
{
    my ($self, $dest_spec, $blurb) = @_;

    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return is (
        $self->_move_half(1),
        $self->_compile_move_spec($dest_spec),
        $blurb
    );
}

package main;

use Test::More tests => 16;

my $TRUE = 1;
my $FALSE = 0;

{
    my $freecell_24_initial_layout_s = <<'EOF';
4C 2C 9C 8C QS 4S 2H 
5H QH 3C AC 3H 4H QD 
QC 9S 6H 9H 3S KS 3D 
5D 2S JC 5C JH 6D AS 
2D KD TH TC TD 8D 
7H JS KH TS KC 7C 
AH 5S 6S AD 8H JD 
7S 6C 7D 4D 8S 9D 
EOF

    my $fc_24_init = DerivedStatesList->new(
        { 
            start => $freecell_24_initial_layout_s,
            perform_horne_prune => 0,
        }
    );

    {
        my $results = $fc_24_init->find_by_string(<<'EOF'
Foundations: H-0 C-0 D-0 S-0 
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
}

{
    my $freecell_24_middle_layout = <<'EOF';
Foundations: H-Q C-8 D-8 S-Q 
Freecells:  KD    
: KH QC JD TC 9D
: KC
: 
: KS QD JC TD 9C
: 
: 
: 
: 
EOF

    my $fc_24 = DerivedStatesList->new(
        { 
            start => $freecell_24_middle_layout,
            perform_horne_prune => 0,
        }
    );

    {
        my $results = $fc_24->find_by_string(<<'EOF'
Foundations: H-Q C-8 D-8 S-Q 
Freecells:        
: KH QC JD TC 9D
: KC
: 
: KS QD JC TD 9C
: 
: 
: 
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
    my $freecell_24_middle_layout = <<'EOF';
Foundations: H-0 C-0 D-0 S-2 
Freecells:  JH  8D
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD JC TD
: QC 9S 6H 9H 3S KS 3D
: 
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
        my $results = $fc_24->find_by_string(<<'EOF'
Foundations: H-0 C-0 D-0 S-2 
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
        my $results = $fc_24->find_by_string(<<'EOF'
Foundations: H-0 C-0 D-0 S-2 
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
