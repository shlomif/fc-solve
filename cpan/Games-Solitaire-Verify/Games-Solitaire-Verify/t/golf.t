#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;
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
