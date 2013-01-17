#!/usr/bin/perl

use strict;
use warnings;

require 5.008;

use File::Spec;

use autodie;

use Test::More tests => 3;
use Games::Solitaire::Verify::Solution::ExpandMultiCardMoves;

my $input_filename = File::Spec->catfile(File::Spec->curdir(),
    qw(t data sample-solutions fcs-freecell-9-orig.txt)
);

my $want_out_filename = File::Spec->catfile(File::Spec->curdir(),
    qw(t data sample-solutions fcs-freecell-9-expanded.txt)
);

sub _slurp
{
    my $filename = shift;

    open my $in, '<', $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $contents = <$in>;

    close($in);

    return $contents;
}

{
    open (my $input_fh, "<", $input_filename);

    my $got_buffer = '';
    open my $out_fh, '>', \$got_buffer;
    my $solution = Games::Solitaire::Verify::Solution::ExpandMultiCardMoves->new(
    {
        input_fh => $input_fh,
        variant => "freecell",
        output_fh => $out_fh,
    },
);

    my $ret = $solution->verify();

    close($input_fh);
    close($out_fh);

    # TEST
    ok (!$ret, "Successful.");

    # TEST
    is (
        $got_buffer,
        scalar( _slurp($want_out_filename) ),
        "Got the right results.",
    );

    # TEST
    unlike($got_buffer, qr/Move (?:[2-9]\d*|1\d+) cards/,
        "No column moves.");

}

