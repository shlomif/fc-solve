#!/usr/bin/perl

use strict;
use warnings;

package Test::HTML::My;

use MooX qw/ late /;

extends('Test::HTML::Tidy::Recursive');

use HTML::TokeParser;

sub check_file
{
    my ( $self, $args ) = @_;

    my $fn = $args->{filename};
    my $p  = HTML::TokeParser->new($fn);
TOKENS:
    while ( my $token = $p->get_token )
    {
        if ( $token->[0] eq 'S' or $token->[0] eq 'E' )
        {
            if ( 'tt' eq lc $token->[1] )
            {
                $self->report_error( { message => "tt tag found in $fn ." } );
                last TOKENS;
            }
        }
    }

    return;
}

1;

package main;

Test::HTML::My->new(
    {
        targets         => ['./dest'],
    }
)->run;
