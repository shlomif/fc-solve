#!/usr/bin/perl

use strict;
use warnings;
use autodie;
use File::Spec ();
use Path::Tiny qw/ path /;
use List::Util qw/ product sum /;

my $FALSE = 0;
my $TRUE  = 1;

my @SUITS = ( 0 .. 3 );
my @RANKS = ( 1 .. 13 );

sub make_card
{
    my ( $rank, $suit ) = @_;
    return ( ( $rank << 2 ) | $suit );
}

my $NUM_CARDS = 64;
my @lookup = ( map { '' } ( 0 .. $NUM_CARDS ) );

foreach my $parent_suit (@SUITS)
{
    foreach my $parent_rank (@RANKS)
    {
        foreach my $child_suit (@SUITS)
        {
            foreach my $child_rank (@RANKS)
            {
                if (    ( $parent_rank == $child_rank + 1 )
                    and ( ( $parent_suit & 0x1 ) != ( $child_suit & 0x1 ) ) )
                {
                    vec( $lookup[ make_card( $parent_rank, $parent_suit ) ],
                        make_card( $child_rank, $child_suit ), 1 )
                        = $TRUE;
                }
            }
        }
    }
}

my $LEN = ( ( 64 * 64 ) );
my $DECL = qq#const fcs_bool_t fc_solve_is_parent_buf[$NUM_CARDS][$NUM_CARDS]#;
path("is_parent.h")->spew_utf8(<<"EOF");
#pragma once
#include "bool.h"
extern $DECL;
EOF

path("is_parent.c")->spew_utf8(
    qq/#include "is_parent.h"\n\n/ . "$DECL = {" . join(
        ',',
        map {
            my $row = $_;
            '{'
                . join( ',',
                map { vec( $lookup[$row], $_, 1 ) ? 'TRUE' : 'FALSE' }
                    ( 0 .. $NUM_CARDS - 1 ) )
                . '}'
        } ( 0 .. $NUM_CARDS - 1 )
        )
        . "};"
);
