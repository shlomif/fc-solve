#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
use Test::Differences (qw( eq_or_diff ));

package StateAsString;

use FC_Solve::InlineWrap (
    C => <<"EOF",
#include "state.h"
#include "state.c"
#include "card.c"

SV * non_parsable_state_as_string(char * input_state_string) {
#ifdef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    return &PL_sv_undef;
#else
    char ret[4096];

    fcs_state_keyval_pair state;
    DECLARE_IND_BUF_T(indirect_stacks_buffer);
    fc_solve_initial_user_state_to_c(
        input_state_string,
        &state,
        4,
        8,
        1,
        indirect_stacks_buffer
        );

    fcs_state_locs_struct locs;
    fc_solve_init_locs (&locs);

    fc_solve_state_as_string(
        ret,
        &(state.s),
        &locs
        PASS_FREECELLS(4)
        PASS_STACKS(8)
        PASS_DECKS(1)
        FC_SOLVE__PASS_PARSABLE(FALSE)
        , FALSE
        PASS_T(FALSE)
    );

    return newSVpv(ret, 0);
#endif
}
EOF
);

package main;

{
    my $output_s = StateAsString::non_parsable_state_as_string(<<'EOF');
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF

    # TEST
    if ( !defined($output_s) )
    {
        ok( 1, "Skipping for this version." );
    }
    else
    {
        use FC_Solve::Trim qw/trim_trail_ws/;
        eq_or_diff(
            trim_trail_ws($output_s),
            <<'EOF',
                        H-0 C-0 D-0 S-0
--- --- --- ---


 --  --  --  --  --  --  --  --
 4C  5H  QC  5D  2D  7H  AH  7S
 2C  QH  9S  2S  KD  JS  5S  6C
 9C  3C  6H  JC 10H  KH  6S  7D
 8C  AC  9H  5C 10C 10S  AD  4D
 QS  3H  3S  JH 10D  KC  8H  8S
 4S  4H  KS  6D  8D  7C  JD  9D
 2H  QD  3D  AS
EOF
            "Output from state_as_string in non parseable is fine",
        );
    }
}
