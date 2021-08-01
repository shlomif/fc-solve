#!/usr/bin/env perl

use strict;
use warnings;
use 5.014;
use Test::More tests => 2;
use lib './lib';

# diag(`env`);
use Path::Tiny qw/ path cwd /;

delete $ENV{MAKEFLAGS};

my %cache;

sub gmake_lookup
{
    my ( $var, ) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return scalar(
        $cache{$var} //= do { my $ret = `gmake $var.show`; chomp $ret; $ret }
    );
}

sub gmake_test
{
    my ( $var, $word, $blurb ) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return like( scalar( gmake_lookup($var) ),
        qr%(?:\A| )\Q$word\E(?:\n|\z| )%ms, $blurb, );
}

{
    my $val = gmake_lookup('TYPESCRIPT_basenames');

    $val =~ m/\A .* ^ TYPESCRIPT_basenames \s* \= \s* ( [^\n]+ ) /msx
        or die "output not matching";
    $val = $1;
    my @bn = split /\s+/, $val;

    # TEST
    is_deeply( [ grep { !/\.js\z/ } @bn ], [], "good extension", );

    # TEST
    is_deeply( [ sort @bn ], [@bn], "sorted", );
}
