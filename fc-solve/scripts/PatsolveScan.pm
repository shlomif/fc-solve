package PatsolveScan;

use strict;
use warnings;

use parent 'Exporter';

our @EXPORT = qw/ pats pats_fast /;

sub pats
{
    my ( $x_params, $y_params ) = @_;

    return (
        "--method",
        "patsolve",
        (
            map { ( "--patsolve-x-param", "$_,$x_params->[$_]" ) }
                keys @$x_params
        ),
        (
            map { ( "--patsolve-y-param", "$_,$y_params->[$_]" ) }
                keys @$y_params
        ),
    );
}

sub pats_fast
{
    my @x_params = (qw(4 1 8 -1 7 11 4 2 2 1 2));

    my @y_params = ( 0.0032, 0.32, -3.0 );

    return pats( \@x_params, \@y_params );
}

1;
