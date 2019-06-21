#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 5;
use Path::Tiny qw/ cwd /;
use Dir::Manifest                  ();
use Games::Solitaire::Verify::Golf ();

my $texts_dir = cwd()->child( 't', 'data', 'texts' );

my $mani = Dir::Manifest->new(
    {
        manifest_fn => $texts_dir->child('list.txt'),
        dir         => $texts_dir->child('texts'),
    }
);

sub _sol_iter
{
    my ($fn) = @_;
    my $fh = $mani->get_obj($fn)->fh->openr;

    return sub { my $l = <$fh>; chomp $l; return $l; };
}

{
    my $verifier = Games::Solitaire::Verify::Golf->new(
        {
            variant      => "all_in_a_row",
            board_string => $mani->text("3.all_in_a_row.board"),
        }
    );

    $verifier->process_solution( _sol_iter("3.all_in_a_row.sol") );

    # TEST
    pass("No error on verifying pysol fc 3 sol");
}

{
    my $verifier = Games::Solitaire::Verify::Golf->new(
        {
            variant      => "black_hole",
            board_string => $mani->text("6.black_hole.board"),
        }
    );

    $verifier->process_solution( _sol_iter("6.black_hole.sol") );

    # TEST
    pass("No error on verifying black_hole pysol fc 6 sol");
}

{
    my $verifier = Games::Solitaire::Verify::Golf->new(
        {
            variant         => "golf",
            board_string    => $mani->text("5.golf.board"),
            queens_on_kings => 0,
            wrap_ranks      => 0,
        }
    );

    $verifier->process_solution( _sol_iter("5.golf.sol") );

    # TEST
    pass("No error on verifying golf pysol fc 5 sol");
}

{
    my $verifier = Games::Solitaire::Verify::Golf->new(
        {
            variant      => "golf",
            board_string => $mani->text("4.golf.board"),
            wrap_ranks   => 1,
        }
    );

    $verifier->process_solution( _sol_iter("4.golf-wrap-ranks.sol") );

    # TEST
    pass("No error on verifying golf wrap ranks pysol fc 4 sol");
}

{
    my $verifier = Games::Solitaire::Verify::Golf->new(
        {
            variant         => "golf",
            board_string    => $mani->text("4.golf.board"),
            queens_on_kings => 0,
            wrap_ranks      => 0,
        }
    );

    eval { $verifier->process_solution( _sol_iter("4.golf-wrap-ranks.sol") ); };

    my $err = $@;

    # TEST
    like( $err, qr#\ACannot put \S{2} in the foundations#, "error thrown" );
}
