use strict;
use warnings;
use Path::Tiny qw/ path /;
use FindBin ();
use lib "$FindBin::Bin/../t/lib";
use FC_Solve::MoveFuncs;

my $move_funcs          = FC_Solve::MoveFuncs::move_funcs();
my $declared_move_funcs = FC_Solve::MoveFuncs::declared_move_funcs();
my $aliases             = FC_Solve::MoveFuncs::aliases();

path('move_funcs_maps.h')->spew_utf8(<<"EOF");
// This file is generated from gen_move_funcs.pl.
// Do not edit by hand!!!
#pragma once
#define FCS_MOVE_FUNCS_NUM @{[0+@$move_funcs]}
#define FCS_MOVE_FUNCS_ALIASES_NUM @{[0+keys%$aliases]}

typedef uint8_t fcs_move_func_aliases_mapping_t;

extern fc_solve_solve_for_state_move_func_t fc_solve_sfs_move_funcs[FCS_MOVE_FUNCS_NUM];
extern fcs_move_func_aliases_mapping_t fc_solve_sfs_move_funcs_aliases[256];
EOF

sub func_name
{
    my $f = shift;
    my $s = "fc_solve_sfs_$f";
    return $f =~ /simple_simon/ ? "WRAP_SIMPSIM($s)" : $s;
}

my $move_funcs_string = join( ",\n",
    ( map { "    " . func_name( $_->{'function'} ) } @$move_funcs ) );
my $aliases_string = join(
    ',',
    map {
        my $c = $aliases->{ chr($_) };
        defined($c) ? $declared_move_funcs->{$c} : 0
    } 0 .. ( 256 - 1 )
);
path('move_funcs_maps.c')->spew_utf8(<<"EOF");
// This file is generated from gen_move_funcs.pl.
// Do not edit by hand!!!
#include "freecell.h"
#include "simpsim.h"

#ifdef FCS_DISABLE_SIMPLE_SIMON
#define WRAP_SIMPSIM(f) fc_solve_sfs_move_top_stack_cards_to_founds
#else
#define WRAP_SIMPSIM(f) f
#endif

fc_solve_solve_for_state_move_func_t fc_solve_sfs_move_funcs[FCS_MOVE_FUNCS_NUM] =
{
$move_funcs_string
};


fcs_move_func_aliases_mapping_t fc_solve_sfs_move_funcs_aliases[256] =
{
$aliases_string
};
EOF
