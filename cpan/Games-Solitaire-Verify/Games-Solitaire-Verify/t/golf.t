#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use Path::Tiny qw/ cwd path /;
use Dir::Manifest                  ();
use Games::Solitaire::Verify::Golf ();

my $data_dir  = cwd()->child( 't', 'data' );
my $texts_dir = $data_dir->child('texts');

my $mani = Dir::Manifest->new(
    {
        manifest_fn => $texts_dir->child('list.txt'),
        dir         => $texts_dir->child('texts'),
    }
);

{
    my $verifier = Games::Solitaire::Verify::Golf->new(
        {
            variant      => "all_in_a_row",
            board_string => $mani->text("3.all_in_a_row.board"),
        }
    );

    my $fh = $mani->get_obj("3.all_in_a_row.sol")->fh->openr;
    $verifier->process_solution( sub { my $l = <$fh>; chomp $l; return $l; } );

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

    my $fh = $mani->get_obj("6.black_hole.sol")->fh->openr;
    $verifier->process_solution( sub { my $l = <$fh>; chomp $l; return $l; } );

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

    my $fh = $mani->get_obj("5.golf.sol")->fh->openr;
    $verifier->process_solution( sub { my $l = <$fh>; chomp $l; return $l; } );

    # TEST
    pass("No error on verifying golf pysol fc 5 sol");
}
