#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2;

use Data::Dumper;

use Games::Solitaire::Verify::Solution;
use File::Spec;

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(), 
        qw(t data sample-solutions fcs-freecell-24.txt)
    );

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $input_fh,
            variant => "freecell",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok (!$verdict, "Everyting is OK.")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(), 
        qw(t data sample-solutions fcs-freecell-24-wrong-1.txt)
    );

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $input_fh,
            variant => "freecell",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok ($verdict, "Solution is invalid")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}
