#!/usr/bin/perl

use 5.014;
use strict;
use warnings;
use autodie;
use Path::Tiny qw/ path /;

my $FALSE = 0;
my $TRUE  = 1;

my $MAX_RANK              = $ENV{FCS_MAX_RANK} || 13;
my $NUM_SUITS             = 4;
my @SUITS                 = ( 0 .. $NUM_SUITS - 1 );
my @RANKS                 = ( 1 .. $MAX_RANK );
my @PARENT_RANKS          = ( 2 .. $MAX_RANK );
my $MAX_NUM_DECKS         = 1;
my $FCS_POS_BY_RANK_WIDTH = ( $MAX_NUM_DECKS << 3 );

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

my $NUM_CHILD_CARDS  = 64;
my $NUM_PARENT_CARDS = make_card( $MAX_RANK, $SUITS[-1] ) + 1;
my @is_king          = ( ($FALSE) x $NUM_PARENT_CARDS );
my %lookup;
my @state_pos = ( map { [ (0) x $NUM_SUITS ] } 0 .. $MAX_RANK );
my @card_pos;
my @positions_by_rank__lookup;
my @pos_by_rank;

foreach my $parent_suit (@SUITS)
{
    foreach my $parent_rank (@RANKS)
    {
        my $parent = make_card( $parent_rank, $parent_suit );
        $is_king[$parent] = ( $parent_rank == $MAX_RANK ? $TRUE : $FALSE );
        $state_pos[$parent_rank][$parent_suit] = $card_pos[$parent] =
            $parent_rank - 1 + $parent_suit * $MAX_RANK;

        $positions_by_rank__lookup[$parent] =
            ($FCS_POS_BY_RANK_WIDTH) * ( $parent_rank - 1 ) +
            ( $parent_suit << 1 );

        my $start    = $FCS_POS_BY_RANK_WIDTH * $parent_rank;
        my $end      = $start + $FCS_POS_BY_RANK_WIDTH;
        my $offset_s = $start + ( ( ( $parent_suit & 0b1 ) ^ 0b1 ) << 1 );

        $pos_by_rank[$parent] = { start => $offset_s, end => $end };
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

path('board_gen_lookup1.h')->spew_utf8(
    "#pragma once\n",
    'static const size_t offset_by_i[52] = {',
    join(
        ',',
        map {
            my $i = $_;
            my $col = ( $i & ( 8 - 1 ) );
            3 *
                ( $col * 7 - ( ( $col > 4 ) ? ( $col - 4 ) : 0 ) + ( $i >> 3 ) )
        } 0 .. ( 52 - 1 )
    ),
    "};\n"
);

sub emit
{
    my ( $DECL, $bn, $header_headers, $contents, $types ) = @_;
    $types //= '';

    my $header_fn = "$bn.h";

    path($header_fn)
        ->spew_utf8( "#pragma once\n"
            . join( '', map { qq{#include $_\n} } @$header_headers )
            . $types
            . "extern $DECL;\n" );
    path("$bn.c")
        ->spew_utf8( qq/#include "$header_fn"\n\n$DECL = {/
            . join( ',', @$contents )
            . "};\n" );

    return;
}

emit(
    qq#const bool fc_solve_is_king_buf[$NUM_PARENT_CARDS]#,
    'is_king',
    [ q/"bool.h"/, ],
    [ map { $_ ? 'TRUE' : 'FALSE' } @is_king ],
);
emit(
    qq#const bool fc_solve_is_parent_buf[$NUM_PARENT_CARDS][$NUM_CHILD_CARDS]#,
    'is_parent',
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
    'debondt__state_pos',
    [ q/<stddef.h>/, ],
    [ map { '{' . join( ',', @$_ ) . '}'; } @state_pos ],
);
emit(
    qq#const size_t fc_solve__card_pos[@{[0+@card_pos]}]#,
    'debondt__card_pos',
    [ q/<stddef.h>/, ],
    [ map { $_ || 0 } @card_pos ],
);
emit(
qq#const size_t positions_by_rank__lookup[@{[0+@positions_by_rank__lookup]}]#,
    'pos_by_rank__lookup',
    [ q/<stddef.h>/, ],
    [ map { $_ || 0 } @positions_by_rank__lookup ],
);
emit(
qq#const pos_by_rank__freecell_t pos_by_rank__freecell[@{[0+@pos_by_rank]}]#,
    'pos_by_rank__freecell',
    [ q/<stddef.h>/, ],
    [
        map {
            my $s = $_ || +{ start => 0, end => 0 };
            "{.start = $s->{start}, .end = $s->{end}}";
        } @pos_by_rank
    ],
    "\ntypedef struct { size_t start, end; } pos_by_rank__freecell_t;\n",
);
{
    my $TYPE_NAME  = 'fcs_seq_cards_power_type';
    my $ARRAY_NAME = 'fc_solve_seqs_over_cards_lookup';
    my $POWER      = 1.3;
    my $TOP        = 2 * $MAX_RANK * 4 + 1;
    emit(
        "const $TYPE_NAME ${ARRAY_NAME}[$TOP]",
        'rate_state',
        [],
        [ map { $_**$POWER } ( 0 .. $TOP - 1 ) ],
"\ntypedef double $TYPE_NAME;\n#define FCS_SEQS_OVER_RENEGADE_POWER(n) ${ARRAY_NAME}[(n)]\n",
    );
}
