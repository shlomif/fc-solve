#!/usr/bin/perl

use strict;
use warnings;

# The tests
# 'function' - The function name without the fc_solve_sfs_ prefix
# 'aliases' - various aliases you wish to designate to this test.
my @tests =
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

my (%declared_tests, %aliases);
for(my $i=0;$i<scalar(@tests);$i++)
{
    my $test_struct = $tests[$i];
    if (!exists($test_struct->{'function'}))
    {
        die "Test #$i does not have a function field";
    }
    if (!exists($test_struct->{'aliases'}))
    {
        die "Test #$i does not have an aliases field";
    }
    if (!(ref($test_struct->{'aliases'}) eq "ARRAY"))
    {
        die "Test #$i 's aliases is not an array ref";
    }
    if (scalar(keys(%$test_struct)) > 2)
    {
        die "Test #$i has excess elements";
    }
    my $f = $test_struct->{'function'};
    my $test_aliases = $test_struct->{'aliases'};
    if (exists($declared_tests{$f}))
    {
        die "Test $f was already declared!";
    }
    $declared_tests{$f} = $i;
    foreach my $alias (@$test_aliases)
    {
        if (exists($aliases{$alias}))
        {
            die "Alias $alias was already declared";
        }
        $aliases{$alias} = $f;
    }
}

my $fcs_tests_num = scalar(@tests);
my $fcs_aliases_num = scalar(keys(%aliases));

open NUM, ">move_funcs_maps.h";
print NUM <<"EOF" ;

/*
    This file is generated from gen_move_funcs.pl.

    Do not edit by hand!!!
*/

#ifndef FC_SOLVE__MOVE_FUNCS_MAPS_H
#define FC_SOLVE__MOVE_FUNCS_MAPS_H

#include "config.h"
#include "scans.h"

#define FCS_TESTS_NUM $fcs_tests_num

#define FCS_TESTS_ALIASES_NUM $fcs_aliases_num

typedef struct
{
    const char * alias;
    int test_num;
} fcs_test_aliases_mapping_t;

extern fc_solve_solve_for_state_test_t fc_solve_sfs_tests[FCS_TESTS_NUM];
extern fcs_test_aliases_mapping_t fc_solve_sfs_tests_aliases[FCS_TESTS_ALIASES_NUM];

#endif
EOF
;
close(NUM);

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

my $tests_string = join(",\n", (map { "    " . func_name($_->{'function'}) } @tests));
my $aliases_string = join(",\n", (map { "    { \"$_\", " . $declared_tests{$aliases{$_}} . " }" } (sort { $a cmp $b } keys(%aliases))));
open TESTS, ">move_funcs_maps.c";
print TESTS <<"EOF" ;
/*
    This file is generated from gen_move_funcs.pl.

    Do not edit by hand!!!
*/

#include "move_funcs_maps.h"
#include "freecell.h"
#include "simpsim.h"

#ifdef FCS_DISABLE_SIMPLE_SIMON
#define WRAP_SIMPSIM(f) NULL
#else
#define WRAP_SIMPSIM(f) f
#endif

fc_solve_solve_for_state_test_t fc_solve_sfs_tests[FCS_TESTS_NUM] =
{
$tests_string
};


fcs_test_aliases_mapping_t fc_solve_sfs_tests_aliases[FCS_TESTS_ALIASES_NUM] =
{
$aliases_string
};
EOF
;

close(TESTS);


