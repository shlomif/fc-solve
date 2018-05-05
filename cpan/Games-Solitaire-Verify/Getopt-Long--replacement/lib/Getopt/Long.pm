package Getopt::Long;

use strict;
use warnings;

use parent 'Exporter';

our @EXPORT_OK = (qw( GetOptionsFromArray ));

sub GetOptionsFromArray
{
    my $cmd_line_args = shift;

    my @rules = @_;

    my $i = 0;

    while (@$cmd_line_args)
    {
        my $arg = $cmd_line_args->[0];
        if ( $arg !~ s/\A-// )
        {
            return 1;
        }
        shift(@$cmd_line_args);

        # Double dash
        $arg =~ s/\A-//;

        if ( !@$cmd_line_args )
        {
            die "Missing param to $arg";
        }

        my $param = shift(@$cmd_line_args);

    RULES:
        for ( my $r_idx = 0 ; $r_idx < @rules ; $r_idx += 2 )
        {
            my $spec = $rules[$r_idx];
            my $cb   = $rules[ $r_idx + 1 ];

            $spec =~ s/=.*//ms;
            my %h = ( map { $_ => 1 } split( /\|/, $spec ) );
            if ( exists( $h{$arg} ) )
            {
                $cb->( $arg, $param );
                last RULES;
            }
        }
    }
}

1;
