package FC_Solve::PreSanityCheck;

use strict;
use warnings;
use 5.024;

use lib '.';
use FC_Solve::Solve ();

use lib '../Games-Solitaire-Verify/lib';

use parent 'Exporter';

# our @EXPORT_OK = qw(solve fc_solve_init);

use autodie;

use List::MoreUtils qw/ true /;

sub verify
{
    #my ( $game_params, $board_idx ) = @_;

# my $fc_solve_output = `pi-make-microsoft-freecell-board -t $board_idx | fc-solve -p -t -sam -sel @fc_solve_args`;
    my $board_idx = 1;
    FC_Solve::Solve::fc_solve_init( [qw(-l lg)] );
    my $fc_solve_output = FC_Solve::Solve::fc_solve_solve($board_idx);
    if ( $fc_solve_output =~ /\b10[CDHS]\b/ )
    {
        say $fc_solve_output;
        die "no t in fc_solve_output";
    }

    if ( $fc_solve_output =~ /---/ )
    {
        say $fc_solve_output;
        die "no parsable sol";
    }
    my $_line_found = sub {
        my ($s) = @_;

        return ( index( $fc_solve_output, $s ) >= 0 );
    };

    my $is_solvable = $_line_found->('This game is solveable');
    my $unsolved    = $_line_found->('I could not solve');
    my $intractable = $_line_found->('Iterations count exceeded');

    if ( 1 != true { $_ } ( $is_solvable, $unsolved, $intractable ) )
    {
        die "Game is more than one of solved, unsolvable or intractable!";
    }

    if ( !$is_solvable )
    {
        die "not solveable";
    }

    say "All ok";
    return;
}

verify();

1;
