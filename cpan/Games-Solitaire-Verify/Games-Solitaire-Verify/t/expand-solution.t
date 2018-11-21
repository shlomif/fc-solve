#!/usr/bin/env perl

use 5.014;
use strict;
use warnings;
use autodie;
use Path::Tiny qw/ path /;

use Test::More tests => 6;

use Games::Solitaire::Verify::Solution::ExpandMultiCardMoves      ();
use Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax ();

sub _samp_sol
{
    return scalar Path::Tiny->cwd->child( qw/t data sample-solutions/, shift );
}
my $input_filename = _samp_sol('fcs-freecell-9-orig.txt');

my $want_out_filename = _samp_sol('fcs-freecell-9-expanded.txt');

# TEST:$num_classes=2;
foreach my $class (
    qw(
    Games::Solitaire::Verify::Solution::ExpandMultiCardMoves
    Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax
    )
    )
{
    open( my $input_fh, "<", $input_filename );

    my $got_buffer = '';
    open my $out_fh, '>', \$got_buffer;
    my $solution = $class->new(
        {
            input_fh  => $input_fh,
            variant   => "freecell",
            output_fh => $out_fh,
        },
    );

    my $ret = $solution->verify();

    close($input_fh);
    close($out_fh);

    # TEST*$num_classes
    ok( !$ret, "Class=$class Successful." );

    # TEST*$num_classes
    is(
        $got_buffer,
        scalar( $want_out_filename->slurp_utf8 ),
        "Class=$class Got the right results.",
    );

    # TEST*$num_classes
    unlike(
        $got_buffer,
        qr/Move (?:[2-9]\d*|1\d+) cards/,
        "Class=$class No column moves."
    );

}
