#!/usr/bin/perl

use strict;
use warnings;

use autodie;

use List::MoreUtils qw(first_index);

use Games::Solitaire::Verify::Solution;
use Games::Solitaire::Verify::App::CmdLine;

if ($ARGV[0] =~ m{\A-h|--help\z})
{
    print <<'EOF';
summarize-fc-solve [Game Params] -- [Extra fc-solve Args]

Display the solution status, the iterations count and the solution length
of the Freecell Solver invocation specified by the arguments. Also verifies
that the solution is true.

The [Game Params] flags (e.g: -g , --freecells-num, --stacks-num, etc.) are also
used to determine the variant used by the solution verifier.
EOF
    exit (0);
}

my @args = @ARGV;

my $separator = first_index { $_ eq '--' } @args;
if ($separator < 0)
{
    die "You must specify both [Game Params] and [Extra fc-solve Args]. See --help";
}

my @game_params = @args[0 .. $separator - 1];
my @fc_solve_args = (@game_params, @args[$separator+1 .. $#args]);

my $LAST_INDEX = ($ENV{L} || 100);

foreach my $board_idx (1 .. $LAST_INDEX)
{
    my $board_str = `pi-make-microsoft-freecell-board -t $board_idx`;
    my $board_fn = 'board.txt';
    open my $out_fh, '>', $board_fn;
    print {$out_fh} $board_str;
    close($out_fh);
    my $fc_solve_output = `fc-solve -p -t -sam -sel @fc_solve_args $board_fn`;

    sub _line_found
    {
        my ($s) = @_;

        return (($fc_solve_output =~ m{^\Q$s\E}ms) ? 1 : 0);
    }

    my $is_solvable = _line_found('This game is solveable');
    my $unsolved = _line_found('I could not solve');
    my $intractable = _line_found('Iterations count exceeded');

    my @true = (grep { $_ } ($is_solvable, $unsolved, $intractable));

    if (! (@true == 1))
    {
        die "Game is more than one of solved, unsolvable or intractable!";
    }

    if ($is_solvable)
    {
        open my $input_fh, "<", (\$fc_solve_output)
            or die "Cannnot open fc_solve_output.";
        my $varianter = Games::Solitaire::Verify::App::CmdLine->new(
            {
                argv => \@game_params,
            },
        );
        my $solution = Games::Solitaire::Verify::Solution->new(
            {
                input_fh => $input_fh,
                variant => "custom",
                variant_params => $varianter->_variant_params(),
            },
        );

        my $verdict = $solution->verify();

        close($input_fh);

        if ($verdict)
        {
            # require Data::Dumper;
            # die "Verdict == " . Dumper($verdict);
            die "Invalid solution!";
        }
    }

    my ($num_iters) = ($fc_solve_output =~ m{^Total number of states checked is (\d+)\.$}ms);
    my $sol_len = () = ($fc_solve_output =~ m{^Move}msg);

    print "Verdict: " .
    ($is_solvable ? "Solved"
        : $intractable ? "Intractable"
        : "Unsolved"
    )
    . " ; Iters: $num_iters ; Length: $sol_len\n";
}
