#!perl -T

use Test::More tests => 3;

BEGIN {
	use_ok( 'Games::Solitaire::VerifySolution' );  # TEST
	use_ok( 'Games::Solitaire::VerifySolution::Base' ); # TEST
	use_ok( 'Games::Solitaire::VerifySolution::Move' ); # TEST
}

diag( "Testing Games::Solitaire::VerifySolution $Games::Solitaire::VerifySolution::VERSION, Perl $], $^X" );
