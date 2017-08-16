use strict;
use warnings;
use Path::Tiny qw/ path /;

# The move functions
# 'function' - The function name without the fc_solve_sfs_ prefix
# 'aliases' - various aliases you wish to designate to this move func.
my @move_funcs = (
    {
        'function' => "move_top_stack_cards_to_founds",
        'aliases'  => [qw(0)],
    },
    {
        'function' => "move_freecell_cards_to_founds",
        'aliases'  => [qw(1)],
    },
    {
        'function' => "move_freecell_cards_on_top_of_stacks",
        'aliases'  => [qw(2)],
    },
    {
        'function' => "move_non_top_stack_cards_to_founds",
        'aliases'  => [qw(3)],
    },
    {
        'function' => "move_stack_cards_to_different_stacks",
        'aliases'  => [qw(4)],
    },
    {
        'function' => "move_stack_cards_to_a_parent_on_the_same_stack",
        'aliases'  => [qw(5)],
    },
    {
        'function' => "move_sequences_to_free_stacks",
        'aliases'  => [qw(6)],
    },
    {
        'function' => "move_freecell_cards_to_empty_stack",
        'aliases'  => [qw(7)],
    },
    {
        'function' => "move_cards_to_a_different_parent",
        'aliases'  => [qw(8)],
    },
    {
        'function' => "empty_stack_into_freecells",
        'aliases'  => [qw(9)],
    },
    {
        'function' => "atomic_move_card_to_empty_stack",
        'aliases'  => [qw(A)],
    },
    {
        'function' => "atomic_move_card_to_parent",
        'aliases'  => [qw(B)],
    },
    {
        'function' => "atomic_move_card_to_freecell",
        'aliases'  => [qw(C)],
    },
    {
        'function' => "atomic_move_freecell_card_to_parent",
        'aliases'  => [qw(D)],
    },
    {
        'function' => "atomic_move_freecell_card_to_empty_stack",
        'aliases'  => [qw(E)],
    },
    {
        'function' => "simple_simon_move_sequence_to_founds",
        'aliases'  => [qw(a)],
    },
    {
        'function' => "simple_simon_move_sequence_to_true_parent",
        'aliases'  => [qw(b)],
    },
    {
        'function' => "simple_simon_move_whole_stack_sequence_to_false_parent",
        'aliases'  => [qw(c)],
    },
    {
        'function' =>
            "simple_simon_move_sequence_to_true_parent_with_some_cards_above",
        'aliases' => [qw(d)],
    },
    {
        'function' =>
            "simple_simon_move_sequence_with_some_cards_above_to_true_parent",
        'aliases' => [qw(e)],
    },
    {
        'function' =>
"simple_simon_move_sequence_with_junk_seq_above_to_true_parent_with_some_cards_above",
        'aliases' => [qw(f)],
    },
    {
        'function' =>
"simple_simon_move_whole_stack_sequence_to_false_parent_with_some_cards_above",
        'aliases' => [qw(g)],
    },
    {
        'function' => "simple_simon_move_sequence_to_parent_on_the_same_stack",
        'aliases'  => [qw(h)],
    },

    # TODO : Enable later.
    {
        'function' => "simple_simon_move_sequence_to_false_parent",
        'aliases'  => [qw(i)],
    },
);

my ( %declared_move_funcs, %aliases );
while ( my ( $i, $move_func_struct ) = each @move_funcs )
{
    if ( !exists( $move_func_struct->{'function'} ) )
    {
        die "Move function #$i does not have a function field";
    }
    if ( !exists( $move_func_struct->{'aliases'} ) )
    {
        die "Move function #$i does not have an aliases field";
    }
    if ( ref( $move_func_struct->{'aliases'} ) ne "ARRAY" )
    {
        die "Move function #$i 's aliases is not an array ref";
    }
    if ( keys(%$move_func_struct) > 2 )
    {
        die "Move function #$i has excess elements";
    }
    my $f = $move_func_struct->{'function'};
    if ( exists( $declared_move_funcs{$f} ) )
    {
        die "Move function $f was already declared!";
    }
    $declared_move_funcs{$f} = $i;
    foreach my $alias ( @{ $move_func_struct->{'aliases'} } )
    {
        if ( exists( $aliases{$alias} ) )
        {
            die "Alias $alias was already declared";
        }
        $aliases{$alias} = $f;
    }
}

path('move_funcs_maps.h')->spew_utf8(<<"EOF");
// This file is generated from gen_move_funcs.pl.
// Do not edit by hand!!!
#pragma once
#define FCS_MOVE_FUNCS_NUM @{[0+@move_funcs]}
#define FCS_MOVE_FUNCS_ALIASES_NUM @{[0+keys%aliases]}

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
    ( map { "    " . func_name( $_->{'function'} ) } @move_funcs ) );
my $aliases_string = join(
    ',',
    map {
        my $c = $aliases{ chr($_) };
        defined($c) ? $declared_move_funcs{$c} : 0
    } 0 .. ( 256 - 1 )
);
path('move_funcs_maps.c')->spew_utf8(<<"EOF");
// This file is generated from gen_move_funcs.pl.
// Do not edit by hand!!!
#include "instance.h"
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
