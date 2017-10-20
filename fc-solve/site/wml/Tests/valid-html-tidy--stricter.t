#!/usr/bin/perl

use strict;
use warnings;

package MyTidy;

use MooX qw/ late /;

extends('Test::HTML::Tidy::Recursive');

sub calc_tidy
{
    my $self = shift;

    my $tidy = HTML::Tidy->new( { output_xhtml => 1, } );

    # $tidy->ignore( type => TIDY_WARNING, type => TIDY_INFO );

    return $tidy;
}

package main;

my %whitelist = ( map { $_ => 1 } (), );

MyTidy->new(
    {
        filename_filter => sub {
            my $fn = shift;
            return not(
                exists $whitelist{$fn}
                or $fn =~ m#\A dest/(?: js/jquery-ui/ ) #x,
            );
        },
        targets => ['./dest'],
    }
)->run;
