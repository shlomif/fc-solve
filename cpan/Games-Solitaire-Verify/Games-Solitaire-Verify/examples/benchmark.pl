#!/usr/bin/perl

use strict;
use warnings;

use autodie;

use List::Util qw( first );

use Games::Solitaire::Verify::Solution     ();
use Games::Solitaire::Verify::App::CmdLine ();

if ( $ARGV[0] =~ m{\A-h|--help\z} )
{
    print <<'EOF';
summarize-fc-solve [Game Params] -- [Extra fc-solve Args]

Display the solution status, the iterations count and the solution length
of the Freecell Solver invocation specified by the arguments. Also verifies
that the solution is true.

The [Game Params] flags (e.g: -g , --freecells-num, --stacks-num, etc.) are also
used to determine the variant used by the solution verifier.
EOF
    exit(0);
}

my @args = @ARGV;

my $separator = first { $args[$_] eq '--' } keys @args;
if ( $separator < 0 )
{
    die
"You must specify both [Game Params] and [Extra fc-solve Args]. See --help";
}

my @game_params   = @args[ 0 .. $separator - 1 ];
my @fc_solve_args = ( @game_params, @args[ $separator + 1 .. $#args ] );

my $LAST_INDEX = ( $ENV{L} || 100 );

foreach my $board_idx ( 1 .. $LAST_INDEX )
{
    print "== $board_idx ==\n";
    my $fc_solve_output =
`pi-make-microsoft-freecell-board -t $board_idx | fc-solve -p -t -sam -sel @fc_solve_args`;

    my $_line_found = sub {
        my ($s) = @_;

        return ( ( $fc_solve_output =~ m{^\Q$s\E}ms ) ? 1 : 0 );
    };

    my $is_solvable = $_line_found->('This game is solveable');
    my $unsolved    = $_line_found->('I could not solve');
    my $intractable = $_line_found->('Iterations count exceeded');

    if ( 1 != scalar( grep { $_ } ( $is_solvable, $unsolved, $intractable ) ) )
    {
        die "Game is more than one of solved, unsolvable or intractable!";
    }

    my $sol_len = 0;

    if ($is_solvable)
    {
        open my $input_fh, "<", ( \$fc_solve_output )
            or die "Cannnot open fc_solve_output.";
        my $varianter = Games::Solitaire::Verify::App::CmdLine->new(
            {
                argv => \@game_params,
            },
        );
        my $solution = Games::Solitaire::Verify::Solution->new(
            {
                input_fh       => $input_fh,
                variant        => "custom",
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

        $sol_len = () = ( $fc_solve_output =~ m{^Move}msg );
    }

    my ($num_iters) = ( $fc_solve_output =~
            m{^Total number of states checked is ([0-9]+)\.$}ms );
    print "Verdict: "
        . (
          $is_solvable ? "Solved"
        : $intractable ? "Intractable"
        :                "Unsolved"
        ) . " ; Iters: $num_iters ; Length: $sol_len\n";
}
