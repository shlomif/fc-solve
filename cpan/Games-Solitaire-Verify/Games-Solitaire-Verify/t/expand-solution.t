#!/usr/bin/perl

use strict;
use warnings;

require 5.008;

use File::Spec;

use autodie;

use Test::More tests => 6;

use Games::Solitaire::Verify::Solution::ExpandMultiCardMoves;
use Games::Solitaire::Verify::Solution::ExpandMultiCardMoves::Lax;

my $input_filename = File::Spec->catfile( File::Spec->curdir(),
    qw(t data sample-solutions fcs-freecell-9-orig.txt) );

my $want_out_filename = File::Spec->catfile( File::Spec->curdir(),
    qw(t data sample-solutions fcs-freecell-9-expanded.txt) );

sub _slurp
{
    my $filename = shift;

    open my $in, '<', $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $num_classesontents = <$in>;

    close($in);

    return $num_classesontents;
}

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
        scalar( _slurp($want_out_filename) ),
        "Class=$class Got the right results.",
    );

    # TEST*$num_classes
    unlike(
        $got_buffer,
        qr/Move (?:[2-9]\d*|1\d+) cards/,
        "Class=$class No column moves."
    );

}
