package FC_Solve::MoveFuncs;

use 5.014;
use strict;
use warnings;

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
    {
        'function' => "move_fc_to_empty_and_put_on_top",
        'aliases'  => [qw(j)],
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

sub aliases
{
    return \%aliases;
}

sub declared_move_funcs
{
    return \%declared_move_funcs;
}

sub move_funcs
{
    return \@move_funcs;
}

1;
