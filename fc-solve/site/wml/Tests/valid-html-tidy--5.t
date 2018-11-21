#!/usr/bin/env perl

use strict;
use warnings;

package Test::HTML::Tidy::Recursive::Tidy5;

use MooX qw/ late /;
use HTML::T5;

extends('Test::HTML::Tidy::Recursive');

sub calc_tidy
{
    my $self = shift;

    return HTML::T5->new( { input_xml => 1, output_xhtml => 1, } );
}

package main;

use lib './lib';

my $T2_POST_DEST = 'dest/';
my %whitelist    = ( map { $_ => 1 } (), );

Test::HTML::Tidy::Recursive::Tidy5->new(
    {
        filename_filter => sub {
            my $fn = shift;
            return not(
                exists $whitelist{$fn}
                or $fn =~
                m#\A \Q$T2_POST_DEST\E (?: MathVentures | js/jquery-ui/ ) #x,
            );
        },
        targets => [$T2_POST_DEST],
    }
)->run;
