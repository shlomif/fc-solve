#!/usr/bin/perl

use strict;
use warnings;
use lib './t/lib';

use Test::More tests => 2;

use Test::Differences (qw( eq_or_diff ));

package State;

use FC_Solve::InlineWrap (
    C => <<"EOF",
#include "state.h"
#include "state.c"
#include "card.c"
#include "inline.h"

typedef struct
{
    fcs_state_keyval_pair_t state;
    fcs_state_locs_struct_t locs;
    DECLARE_IND_BUF_T(indirect_stacks_buffer);
} StateInC;

SV* _proto_new(const char * input_state_string) {
        StateInC * s;
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "State");
        New(42, s, 1, StateInC);

        fc_solve_init_locs (&(s->locs));

        fc_solve_initial_user_state_to_c(
        input_state_string,
        &s->state,
        4,
        8,
        1,
        s->indirect_stacks_buffer
        );
        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

static GCC_INLINE StateInC * deref(SV * const obj) {
    return (StateInC*)SvIV(SvRV(obj));
}

static GCC_INLINE fcs_state_keyval_pair_t * q(SV * const obj) {
    return &(deref(obj)->state);
}

SV * as_string(SV * obj) {
    char ret[4096];

    StateInC * o = deref(obj);

    fc_solve_state_as_string(
        ret,
        &(o->state),
        &o->locs
        PASS_FREECELLS(4)
        PASS_STACKS(8)
        PASS_DECKS(1)
        FC_SOLVE__PASS_PARSABLE(TRUE)
        , FALSE
        PASS_T(TRUE)
    );

    return newSVpv(ret, 0);
}

void transfer_cards(SV * obj, int to, int from, int cards_num) {
    fcs_state_t * s = &(deref(obj)->state.s);
    fcs_cards_column_t to_col = fcs_state_get_col(*s, to);
    fcs_cards_column_t from_col = fcs_state_get_col(*s, from);
    fcs_col_transfer_cards(to_col, from_col, cards_num);
}
EOF
    LIBS => "-L" . $ENV{FCS_PATH} . " ",
);

sub new
{
    my ($class, $string) = @_;
    return State::_proto_new($string);
}

package main;

{
    # A state from the middle of:
    # pi-make-microsoft-freecell-board -t 1024 | fc-solve -t -sam -p -sel
    my $state = State->new(<<'EOF');
Foundations: H-2 C-T D-6 S-A
Freecells:  8H  KH  JH  TD
: KC QH
: 3H KS 7H 2S QC JC JS TH 9S 8D 7S
:
: 6H
: 6S 5H 4S
:
: QD 7D 9H KD QS JD TS 9D 8S
: 5S 4H 3S
EOF

        use FC_Solve::Trim qw/trim_trail_ws/;
        # TEST
        is (trim_trail_ws($state->as_string()),
            <<'EOF',
Foundations: H-2 C-T D-6 S-A
Freecells:  8H  KH  JH  TD
: KC QH
: 3H KS 7H 2S QC JC JS TH 9S 8D 7S
:
: 6H
: 6S 5H 4S
:
: QD 7D 9H KD QS JD TS 9D 8S
: 5S 4H 3S
EOF
            "as_string is working fine."
        );

        $state->transfer_cards(3, 7, 3);

        # TEST
        is (trim_trail_ws($state->as_string()),
            <<'EOF',
Foundations: H-2 C-T D-6 S-A
Freecells:  8H  KH  JH  TD
: KC QH
: 3H KS 7H 2S QC JC JS TH 9S 8D 7S
:
: 6H 5S 4H 3S
: 6S 5H 4S
:
: QD 7D 9H KD QS JD TS 9D 8S
:
EOF
            "cards were transferred."
        );
}
