#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 10;
use Carp;
use Data::Dumper;
use String::ShellQuote;

use YAML::Syck;

use lib './t/lib';

use Games::Solitaire::FC_Solve::ShaAndLen;

my $digests_storage_fn = "./t/data/digests-and-lens-storage.yml";
my $digests_storage = LoadFile($digests_storage_fn);

sub should_fill_in_id
{
    my $id = shift;

    return
    (
        exists($digests_storage->{digests}->{$id})
        ?  (($ENV{'FCS_DIGESTS_REPLACE_IDS'} || "") =~ m{\Q,$id,\E} )
        :  (($ENV{'FCS_DIGESTS_FILL_IDS'} || "") =~ m{\Q,$id,\E} )
    );
}

sub verify_solution_test
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $args = shift;
    my $msg = shift;

    if (! $args->{id})
    {
        Carp::confess("ID not specified");
    }

    my $id = $args->{id};

    my $deal = $args->{deal};

    if ($deal !~ m{\A[1-9][0-9]*\z})
    {
        confess "Invalid deal $deal";
    }

    my $theme = $args->{theme} || ["-l", "gi"];

    my $variant = $args->{variant}  || "freecell";

    my $fc_solve_exe = shell_quote($ENV{'FCS_PATH'} . "/fc-solve");

    open my $fc_solve_output,
        "make_pysol_freecell_board.py $deal $variant | " .
        "$fc_solve_exe -g $variant " . shell_quote(@$theme) . " -p -t -sam |"
        or Carp::confess "Error! Could not open the fc-solve pipeline";

    my $sha = Games::Solitaire::FC_Solve::ShaAndLen->new();

    $sha->add_file($fc_solve_output);

    close ($fc_solve_output);

    if (should_fill_in_id($id))
    {
        $digests_storage->{digests}->{$id} =
        {
            'len' => $sha->len(),
            'hexdigest' => $sha->hexdigest(),
        }
    }
    my $info = $digests_storage->{digests}->{$id};

    my $test_verdict = ok (
        ($sha->hexdigest() eq $info->{'hexdigest'}) &&
        ($sha->len() eq $info->{'len'})
        , $msg);

    if (!$test_verdict)
    {
        diag( "Expected Digest: " . $info->{'hexdigest'}. "\n"
            . "Got Digest: " . $sha->hexdigest() . "\n"
            . "Expected Len: " . $info->{'len'} . "\n"
            . "Got Len: " . $sha->len(). "\n"
        );
    }

    close($fc_solve_output);

    return $test_verdict;
}

# 24 is my lucky number. (Shlomif)
# TEST
verify_solution_test({id => "freecell24", deal => 24}, "Verifying the solution of deal #24");



# TEST
verify_solution_test({id => "freecell1941", deal => 1941}, "Verifying 1941 (The Hardest Deal)");

# TEST
verify_solution_test({id => "freecell24empty", deal => 24, theme => [],},
    "Solving Deal #24 with the default heuristic"
);

# TEST
verify_solution_test({id => "freecell617jgl", deal => 617, theme => ["-l", "john-galt-line"],},
    "Solving Deal #617 with the john-galt-line"
);

# TEST
verify_solution_test({id => "bakers_game24default", deal => 24, variant => "bakers_game", theme => [],},
    "Baker's Game Deal #24"
);

# TEST
verify_solution_test({id => "forecell1099default", deal => 1099, variant => "forecell", theme => [],},
    "Forecell Deal #1099"
);

# TEST
verify_solution_test({id => "relaxed_freecell11982",deal => 11982, variant => "relaxed_freecell", },
    "Relaxed Freecell Deal #11982"
);


# TEST
verify_solution_test(
    {
        id => "seahaven_towers1977fools-gold",
        deal => 1977,
        variant => "seahaven_towers",
        theme => ["-l", "fools-gold",],
    },
    "Seahaven Towers #1977"
);

# TEST
verify_solution_test({
        id => "eight_off200", deal => 200, variant => "eight_off",
    },
    "Eight Off #200 with -l gi"
);

# TEST
verify_solution_test(
    {id =>"eight_off200default", deal => 200, variant => "eight_off",
        theme => [],
    },
    "Eight Off #200 with default heuristic"
);

# Store the changes at the end so they won't get lost.
DumpFile($digests_storage_fn, $digests_storage);

