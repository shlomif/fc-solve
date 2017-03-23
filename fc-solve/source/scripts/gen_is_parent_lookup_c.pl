#!/usr/bin/perl

use strict;
use warnings;
use autodie;
use Path::Tiny qw/ path /;

my $TRUE = 1;

my $MAX_RANK     = 13;
my @SUITS        = ( 0 .. 3 );
my @RANKS        = ( 1 .. $MAX_RANK );
my @PARENT_RANKS = ( 2 .. $MAX_RANK );

sub make_card
{
    my ( $rank, $suit ) = @_;
    return ( ( $rank << 2 ) | $suit );
}

sub key
{
    my ( $parent, $child ) = @_;
    return "${parent}\t${child}";
}

my $NUM_CARDS = 64;
my %lookup;

foreach my $parent_suit (@SUITS)
{
    foreach my $parent_rank (@PARENT_RANKS)
    {
        my $parent = make_card( $parent_rank, $parent_suit );
        my $start = ( ( $parent_suit ^ 0x1 ) & ( ~0x2 ) );
        foreach my $child_suit ( $start, $start + 2 )
        {
            foreach my $child_rank ( $parent_rank - 1 )
            {
                $lookup{ key( $parent, make_card( $child_rank, $child_suit ), )
                } = $TRUE;
            }
        }
    }
}

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
## Please see file perltidy.ERR
            my $parent = $_;
            '{' . join(
                ',',
                map {
                    exists( $lookup{ key( $parent, $_ ) } )
                        ? 'TRUE'
                        : 'FALSE'
                } ( 0 .. $NUM_CARDS - 1 )
                )
                . '}'
        } ( 0 .. $NUM_CARDS - 1 )
        )
        . "};"
);
