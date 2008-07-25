#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 13;

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
    ok (!$verdict, "Everything is OK.")
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

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(), 
        qw(t data sample-solutions fcs-freecell-24-wrong-2.txt)
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

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(), 
        qw(t data sample-solutions fcs-bakers-game-24.txt)
    );

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $input_fh,
            variant => "bakers_game",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok (!$verdict, "Everything is OK.")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}

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
            variant => "bakers_game",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok ($verdict, "bakers_game cannot solve a Freecell solution")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(), 
        qw(t data sample-solutions fcs-forecell-24.txt)
    );

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $input_fh,
            variant => "forecell",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok (!$verdict, "Everything is OK.")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}

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
            variant => "forecell",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok ($verdict, "forecell cannot solve a Freecell solution")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(), 
        qw(t data sample-solutions fcs-seahaven-towers-1977.txt)
    );

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $input_fh,
            variant => "seahaven_towers",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok (!$verdict, "Seahaven Towers Solution is OK.")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(), 
        qw(t data sample-solutions fcs-relaxed-freecell-11982.txt)
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
    ok ($verdict, "Freecell cannot solve a Relaxed Freecell Game")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(), 
        qw(t data sample-solutions fcs-relaxed-freecell-11982.txt)
    );

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $input_fh,
            variant => "relaxed_freecell",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok (!$verdict, "Relaxed Freecell Solution is OK.")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(), 
        qw(t data sample-solutions fcs-eight-off-200.txt)
    );

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $input_fh,
            variant => "eight_off",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok (!$verdict, "Eight Off Solution is OK.")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(), 
        qw(t data sample-solutions fcs-eight-off-200-l-gi.txt)
    );

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $input_fh,
            variant => "eight_off",
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok (!$verdict, "Eight Off -l gi Solution is OK.")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}

{
    my $input_filename = File::Spec->catfile(File::Spec->curdir(),
        qw(t data sample-solutions fcs-relaxed-freecell-11982.txt)
    );

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    my $relaxed_fc_variant_params = 
           Games::Solitaire::Verify::VariantParams->new(
            {
                'num_decks' => 1,
                'num_columns' => 8,
                'num_freecells' => 4,
                'sequence_move' => "unlimited",
                'seq_build_by' => "alt_color",
                'empty_stacks_filled_by' => "any",
            }
        );

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $input_fh,
            variant => "custom",
            variant_params => $relaxed_fc_variant_params,
        },
    );

    my $verdict = $solution->verify();
    # TEST
    ok (!$verdict, "Custom Relaxed Freecell Solution is OK.")
        or diag("Verdict == " . Dumper($verdict));

    close($input_fh);
}
