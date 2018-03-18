#!/usr/bin/perl

use strict;
use warnings;

my $S        = 'A23456789TJQK';
my @RANKS    = split //, $S;
my $SUITS    = 'HCDS';
my $SUITS_RE = qr/[$SUITS]/;

my $BASE_RANK = $ENV{PENGUIN_BASE_RANK};
if ( !defined $BASE_RANK or ( ( length $BASE_RANK ) != 1 ) )
{
    die "BASE_RANK is not specified.";
}
my $BASE_RANK_IDX = index( $S, $BASE_RANK );

if ( $BASE_RANK_IDX < 0 )
{
    die "BASE_RANK is invalid.";
}

my $REV = $ENV{PENGUIN_REVERSE};
if ($REV)
{
    $BASE_RANK_IDX = ( 13 - $BASE_RANK_IDX );
}

my %MAP = ( map { $RANKS[$_] => $RANKS[ ( $_ + 1 ) % @RANKS ] } keys @RANKS );

{
    local $/;
    my $text = <ARGV>;
    for my $i ( 1 .. $BASE_RANK_IDX )
    {
        $text =~ s#([$S])($SUITS_RE)#$MAP{$1}.$2#eg;
        $text =~ s#($SUITS_RE)-([$S])#$1.'-'.$MAP{$2}#eg;
    }
    print $text;
}

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2018 Shlomi Fish

=cut
