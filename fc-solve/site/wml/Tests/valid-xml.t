#!/usr/bin/env perl

use strict;
use warnings;

package Test::HTML::Tidy::Recursive::XML;

use MooX        qw/ late /;
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

use Test::More tests => 2;
use vars qw/ $TODO /;

TODO:
{
    local $TODO = (
        $ENV{FC_SOLVE__MULT_CONFIG_TESTS__TRACE}
        ? "FC_SOLVE__MULT_CONFIG_TESTS__TRACE"
        : undef()
    );
    my %whitelist = ( map { $_ => 1 } (), );

    my $err = '';

    # TEST
    subtest "wrapper" => sub {
        eval {
            Test::HTML::Tidy::Recursive::XML->new(
                {
                    filename_filter => sub {
                        my $fn = shift;
                        return not( exists $whitelist{$fn}
                            or $fn =~
m#\A \Q$T2_POST_DEST\E (?: js/(?: jquery-ui | yui-unpack )/ | mail-lists/ ) #x
                        );
                    },
                    targets => [$T2_POST_DEST],
                }
            )->run;
        };
        $err = $@;
    };

    # TEST
    is( $err, "", "no exception." );
}
