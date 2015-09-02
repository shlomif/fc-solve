#!/usr/bin/perl

use strict;
use warnings;

# The move functions
# 'function' - The function name without the fc_solve_sfs_ prefix
# 'aliases' - various aliases you wish to designate to this move func.
my @move_funcs =
(
    {
        'function' => "move_top_stack_cards_to_founds",
        'aliases' => [ qw(0) ],
    },
    {
        'function' => "move_freecell_cards_to_founds",
        'aliases' => [ qw(1) ],
    },
    {
        'function' => "move_freecell_cards_on_top_of_stacks",
        'aliases' => [ qw(2) ],
    },
    {
        'function' => "move_non_top_stack_cards_to_founds",
        'aliases' => [ qw(3) ],
    },
    {
        'function' => "move_stack_cards_to_different_stacks",
        'aliases' => [ qw(4) ],
    },
    {
        'function' => "move_stack_cards_to_a_parent_on_the_same_stack",
        'aliases' => [ qw(5) ],
    },
    {
        'function' => "move_sequences_to_free_stacks",
        'aliases' => [ qw(6) ],
    },
    {
        'function' => "move_freecell_cards_to_empty_stack",
        'aliases' => [ qw(7) ],
    },
    {
        'function' => "move_cards_to_a_different_parent",
        'aliases' => [ qw(8) ],
    },
    {
        'function' => "empty_stack_into_freecells",
        'aliases' => [ qw(9) ],
    },
    {
        'function' => "atomic_move_card_to_empty_stack",
        'aliases' => [ qw(A) ],
    },
    {
        'function' => "atomic_move_card_to_parent",
        'aliases' => [ qw(B) ],
    },
    {
        'function' => "atomic_move_card_to_freecell",
        'aliases' => [ qw(C) ],
    },
    {
        'function' => "atomic_move_freecell_card_to_parent",
        'aliases' => [ qw(D) ],
    },
    {
        'function' => "atomic_move_freecell_card_to_empty_stack",
        'aliases' => [ qw(E) ],
    },
    {
        'function' => "simple_simon_move_sequence_to_founds",
        'aliases' => [ qw(a) ],
    },
    {
        'function' => "simple_simon_move_sequence_to_true_parent",
        'aliases' => [ qw(b) ],
    },
    {
        'function' => "simple_simon_move_whole_stack_sequence_to_false_parent",
        'aliases' => [ qw(c) ],
    },
    {
        'function' => "simple_simon_move_sequence_to_true_parent_with_some_cards_above",
        'aliases' => [ qw(d) ],
    },
    {
        'function' => "simple_simon_move_sequence_with_some_cards_above_to_true_parent",
        'aliases' => [ qw(e) ],
    },
    {
        'function' => "simple_simon_move_sequence_with_junk_seq_above_to_true_parent_with_some_cards_above",
        'aliases' => [ qw(f) ],
    },
    {
        'function' => "simple_simon_move_whole_stack_sequence_to_false_parent_with_some_cards_above",
        'aliases' => [ qw(g) ],
    },
    {
        'function' => "simple_simon_move_sequence_to_parent_on_the_same_stack",
        'aliases' => [ qw(h) ],
    },
    # TODO : Enable later.
    {
        'function' => "simple_simon_move_sequence_to_false_parent",
        'aliases' => [ qw(i) ],
    },
);

my (%declared_move_funcs, %aliases);
for(my $i=0;$i<scalar(@move_funcs);$i++)
{
    my $move_func_struct = $move_funcs[$i];
    if (!exists($move_func_struct->{'function'}))
    {
        die "Move function #$i does not have a function field";
    }
    if (!exists($move_func_struct->{'aliases'}))
    {
        die "Move function #$i does not have an aliases field";
    }
    if (!(ref($move_func_struct->{'aliases'}) eq "ARRAY"))
    {
        die "Move function #$i 's aliases is not an array ref";
    }
    if (scalar(keys(%$move_func_struct)) > 2)
    {
        die "Move function #$i has excess elements";
    }
    my $f = $move_func_struct->{'function'};
    if (exists($declared_move_funcs{$f}))
    {
        die "Move function $f was already declared!";
    }
    $declared_move_funcs{$f} = $i;
    foreach my $alias (@{ $move_func_struct->{'aliases'} })
    {
        if (exists($aliases{$alias}))
        {
            die "Alias $alias was already declared";
        }
        $aliases{$alias} = $f;
    }
}

my $fcs_move_funcs_num = scalar(@move_funcs);
my $fcs_aliases_num = scalar(keys(%aliases));

my $num_filename = 'move_funcs_maps.h';
open my $num_fh, '>', $num_filename
    or die "Cannot open '$num_filename' for writing - $!";
print {$num_fh} <<"EOF" ;

/*
    This file is generated from gen_move_funcs.pl.

    Do not edit by hand!!!
*/

#ifndef FC_SOLVE__MOVE_FUNCS_MAPS_H
#define FC_SOLVE__MOVE_FUNCS_MAPS_H

#include "config.h"

#define FCS_MOVE_FUNCS_NUM $fcs_move_funcs_num

#define FCS_MOVE_FUNCS_ALIASES_NUM $fcs_aliases_num

typedef struct
{
    char alias[2];
    int move_func_num;
} fcs_move_func_aliases_mapping_t;

extern fc_solve_solve_for_state_move_func_t fc_solve_sfs_move_funcs[FCS_MOVE_FUNCS_NUM];
extern fcs_move_func_aliases_mapping_t fc_solve_sfs_move_funcs_aliases[FCS_MOVE_FUNCS_ALIASES_NUM];

#endif
EOF
;
close($num_fh);

sub func_name
{
    my $f = shift;
    my $s = "fc_solve_sfs_$f";
    if ($f =~ /simple_simon/)
    {
        return "WRAP_SIMPSIM($s)";
    }
    else
    {
        return $s;
    }
}

my $move_funcs_string = join(",\n", (map { "    " . func_name($_->{'function'}) } @move_funcs));
my $aliases_string = join(",\n", (map { "    { \"$_\", " . $declared_move_funcs{$aliases{$_}} . " }" } (sort { $a cmp $b } keys(%aliases))));
my $move_funcs_filename = 'move_funcs_maps.c';
open my $move_funcs_fh, '>', $move_funcs_filename
    or die "Cannot open '$move_funcs_filename' for writing - $!";
print {$move_funcs_fh} <<"EOF" ;
/*
    This file is generated from gen_move_funcs.pl.

    Do not edit by hand!!!
*/

#include "instance.h"
#include "freecell.h"
#include "simpsim.h"

#ifdef FCS_DISABLE_SIMPLE_SIMON
#define WRAP_SIMPSIM(f) NULL
#else
#define WRAP_SIMPSIM(f) f
#endif

fc_solve_solve_for_state_move_func_t fc_solve_sfs_move_funcs[FCS_MOVE_FUNCS_NUM] =
{
$move_funcs_string
};


fcs_move_func_aliases_mapping_t fc_solve_sfs_move_funcs_aliases[FCS_MOVE_FUNCS_ALIASES_NUM] =
{
$aliases_string
};
EOF
;

close ($move_funcs_fh);


