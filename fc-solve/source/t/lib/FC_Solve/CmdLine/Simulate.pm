package FC_Solve::CmdLine::Simulate;

use strict;
use warnings;

use FC_Solve::CmdLine::Simulate::Flare ();
use FC_Solve::CmdLine::Expand          ();

use MooX qw( late );

sub _build_expander
{
    my $self = shift;

    return FC_Solve::CmdLine::Expand->new(
        {
            input_argv => $self->input_argv,
        }
    );
}

has input_argv => ( is => 'ro', isa => 'ArrayRef[Str]', required => 1, );
has _expander => (
    is      => 'ro',
    isa     => 'FC_Solve::CmdLine::Expand',
    lazy    => 1,
    default => \&_build_expander,
    handles => {
        _expanded_argv => 'argv',
    },
);

has '_flares' => (
    is      => 'ro',
    isa     => 'ArrayRef[FC_Solve::CmdLine::Simulate::Flare]',
    lazy    => 1,
    default => \&_calc_flares,
);

sub get_flares_num
{
    my ( $self, $idx ) = @_;

    return scalar @{ $self->_flares };
}

sub get_flare_by_idx
{
    my ( $self, $idx ) = @_;

    return $self->_flares->[$idx];
}

sub _calc_flares
{
    my $self = shift;

    my $argv = $self->_expanded_argv;

    my $idx = 0;

    my $parse_flare = sub {
        my @flare_args;
        my $flare_name;
        my $is_flares_plan_start = 0;

    PARSE_FLARE_ARGV:
        while ( $idx < @$argv )
        {
            my $arg = $argv->[$idx];

            if ( $arg eq '-nf' )
            {
                $idx++;
                last PARSE_FLARE_ARGV;
            }
            elsif ( $arg eq '--flare-name' )
            {
                $idx++;
                $flare_name = $argv->[$idx];
            }
            elsif ( $arg eq '--flares-plan' )
            {
                $is_flares_plan_start = 1;
                last PARSE_FLARE_ARGV;
            }
            else
            {
                push @flare_args, $arg;
            }
        }
        continue
        {
            $idx++;
        }

        return {
            name                 => $flare_name,
            args                 => ( \@flare_args ),
            is_flares_plan_start => $is_flares_plan_start,
        };
    };

    my @flares;
GET_FLARES:
    while (1)
    {
        my $flare = $parse_flare->();

        if ( !defined( $flare->{name} ) )
        {
            die "no --flare-name given.";
        }

        push @flares,
            FC_Solve::CmdLine::Simulate::Flare->new(
            {
                name => $flare->{name},
                argv => $flare->{args},
            }
            );

        if ( $flare->{is_flares_plan_start} )
        {
            last GET_FLARES;
        }
    }

    return \@flares;
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2013 Shlomi Fish

=cut
