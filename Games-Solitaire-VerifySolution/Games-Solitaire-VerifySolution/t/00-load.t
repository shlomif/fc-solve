#!perl -T

use Test::More tests => 3;

BEGIN {
	use_ok( 'Games::Solitaire::Verify' );  # TEST
	use_ok( 'Games::Solitaire::Verify::Base' ); # TEST
	use_ok( 'Games::Solitaire::Verify::Move' ); # TEST
}

diag( "Testing Games::Solitaire::Verify $Games::Solitaire::Verify::VERSION, Perl $], $^X" );
