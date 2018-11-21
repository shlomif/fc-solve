#!/usr/bin/env perl

use strict;
use warnings;

package Test::HTML::Tidy::Recursive::XML;

use MooX qw/ late /;
use XML::LibXML ();
use lib './lib';

my $T2_POST_DEST = 'dest/';

extends('Test::HTML::Tidy::Recursive');

sub calc_tidy
{
    return XML::LibXML->new( load_ext_dtd => 1 );
}

sub check_file
{
    my ( $self, $args ) = @_;

    $self->_tidy->parse_file( $args->{filename} );

    return 1;
}

package main;

my %whitelist = ( map { $_ => 1 } (), );

Test::HTML::Tidy::Recursive::XML->new(
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
