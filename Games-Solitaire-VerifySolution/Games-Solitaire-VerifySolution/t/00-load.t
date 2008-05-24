#!perl -T

use Test::More tests => 2;

BEGIN {
	use_ok( 'Games::Solitaire::VerifySolution' );
	use_ok( 'Games::Solitaire::VerifySolution::Move' );
}

diag( "Testing Games::Solitaire::VerifySolution $Games::Solitaire::VerifySolution::VERSION, Perl $], $^X" );
