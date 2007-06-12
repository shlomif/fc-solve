#!/usr/bin/perl 

use strict;
use warnings;

use Template;

sub card_num_normalize
{
    my $arg = shift;

    if (ref($arg) eq "")
    {
        return +{ map { $_ => $arg } (qw(t non_t)) };
    }
    else
    {
        return $arg
    }
}

my @suits = (qw(H C D S));
my @card_nums =  ("A", (1 .. 9), 
    {
        't' => "T",
        'non_t' => "10",
    },
    , "J", "Q", "K");

@card_nums = (map { card_num_normalize($_) } @card_nums);

my $template = Template->new();

sub indexify
{
    my $array = shift;

    return 
    [ 
        map 
        { +{ 'idx' => $_, 'value' => $array->[$_] } } 
        (0 .. $#$array)
    ];
}

$template->process(
    "card-test-render.c.tt",
    {
        'suits' => indexify(\@suits),
        'card_nums' => indexify(\@card_nums),
    },
    "card-test-render.c",
) || die $template->error();

=begin Discard

open my $out, ">", "t/test.bash";
print {$out} <<"EOF";
#!/bin/bash
exec ./binary-tests/card-test-render
EOF
close($out);

=end Discard

=cut


