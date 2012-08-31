#!/usr/bin/perl

use strict;
use warnings;

use autodie;

use Games::Solitaire::Verify::Solution::ExpandMultiCardMoves;

my $input_filename = shift(@ARGV);

open (my $input_fh, "<", $input_filename);

my $solution = Games::Solitaire::Verify::Solution::ExpandMultiCardMoves->new(
    {
        input_fh => $input_fh,
        variant => "freecell",
        output_fh => \*STDOUT,
    },
);

my $ret = $solution->verify();

close($input_fh);

if ($ret)
{
    die $ret;
}
