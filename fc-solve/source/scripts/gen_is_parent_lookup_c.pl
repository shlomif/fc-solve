#!/usr/bin/perl

use strict;
use warnings;
use autodie;
use Path::Tiny qw/ path /;

my $TRUE = 1;

my $MAX_RANK     = 13;
my $NUM_SUITS    = 4;
my @SUITS        = ( 0 .. $NUM_SUITS - 1 );
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

my $NUM_CHILD_CARDS = 64;
my $NUM_PARENT_CARDS = make_card( $MAX_RANK, $SUITS[-1] ) + 1;
my %lookup;
my @state_pos = ( map { [ (0) x $NUM_SUITS ] } 0 .. $MAX_RANK );
my @card_pos;

foreach my $parent_suit (@SUITS)
{
    foreach my $parent_rank (@RANKS)
    {
        my $parent = make_card( $parent_rank, $parent_suit );
        $state_pos[$parent_rank][$parent_suit] = $card_pos[$parent] =
            $parent_rank - 1 + $parent_suit * $MAX_RANK;

        if ( $parent_rank > 1 )
        {
            my $start = ( ( $parent_suit ^ 0x1 ) & ( ~0x2 ) );
            foreach my $child_suit ( $start, $start + 2 )
            {
                foreach my $child_rank ( $parent_rank - 1 )
                {
                    $lookup{ key( $parent,
                            make_card( $child_rank, $child_suit ), ) } = $TRUE;
                }
            }
        }
    }
}

sub emit
{
    my ( $DECL, $header_fn, $c_fn, $header_headers, $contents ) = @_;

    path($header_fn)
        ->spew_utf8( "#pragma once\n"
            . join( '', map { qq{#include $_\n} } @$header_headers )
            . "extern $DECL;\n" );
    path($c_fn)
        ->spew_utf8( qq/#include "$header_fn"\n\n$DECL = {/
            . join( ',', @$contents )
            . "};\n" );

    return;
}

emit(
qq#const fcs_bool_t fc_solve_is_parent_buf[$NUM_PARENT_CARDS][$NUM_CHILD_CARDS]#,
    'is_parent.h',
    'is_parent.c',
    [ q/"bool.h"/, ],
    [
        map {
            my $parent = $_;
            '{' . join(
                ',',
                map {
                    exists( $lookup{ key( $parent, $_ ) } )
                        ? 'TRUE'
                        : 'FALSE'
                } ( 0 .. $NUM_CHILD_CARDS - 1 )
                )
                . '}'
        } ( 0 .. $NUM_PARENT_CARDS - 1 )
    ],
);
emit(
    qq#const size_t fc_solve__state_pos[@{[$MAX_RANK+1]}][$NUM_SUITS]#,
    'debondt__state_pos.h',
    'debondt__state_pos.c',
    [ q/<stddef.h>/, ],
    [ map { '{' . join( ',', @$_ ) . '}'; } @state_pos ],
);
emit(
    qq#const size_t fc_solve__card_pos[@{[scalar @card_pos]}]#,
    'debondt__card_pos.h',
    'debondt__card_pos.c',
    [ q/<stddef.h>/, ],
    [ map { $_ || 0 } @card_pos ],
);
