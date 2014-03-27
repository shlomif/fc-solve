package FC_Solve::CmdLine::Simulate::Flare;

use strict;
use warnings;

use MooX qw( late );

has 'name' => (is => 'ro', isa => 'Str', required => 1);
has 'argv' => (is => 'ro', isa => 'ArrayRef[Str]', required => 1);

package FC_Solve::CmdLine::Simulate;

use strict;
use warnings;

use FC_Solve::CmdLine::Expand;

use MooX qw( late );

sub _build_expander
{
    my $self = shift;

    return FC_Solve::CmdLine::Expand->new(
        {
            input_argv => scalar($self->input_argv),
        }
    );
}

has input_argv => (is => 'ro', isa => 'ArrayRef[Str]', required => 1, );
has _expander =>
(
    is => 'ro',
    isa => 'FC_Solve::CmdLine::Expand',
    lazy => 1,
    default => \&_build_expander,
    handles =>
    {
        _expanded_argv => 'argv',
    },
);

has '_flares' =>
(
    is => 'ro',
    isa => 'ArrayRef[FC_Solve::CmdLine::Simulate::Flare]',
    lazy => 1,
    default => \&_calc_flares,
);

sub get_flares_num
{
    my ($self, $idx) = @_;

    return scalar(@{ $self->_flares });
}

sub get_flare_idx
{
    my ($self, $idx) = @_;

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
        while ($idx < @$argv)
        {
            my $arg = $argv->[$idx];

            if ($arg eq '-nf')
            {
                $idx++;
                last PARSE_FLARE_ARGV;
            }
            elsif ($arg eq '--flare-name')
            {
                $idx++;
                $flare_name = $argv->[$idx];
            }
            elsif ($arg eq '--flares-plan')
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

        return
        {
            name => $flare_name,
            args => (\@flare_args),
            is_flares_plan_start => $is_flares_plan_start,
        };
    };

    my @flares;
    GET_FLARES:
    while (1)
    {
        my $flare = $parse_flare->();

        if (! defined($flare->{name}))
        {
            die "no --flare-name given.";
        }

        push @flares, FC_Solve::CmdLine::Simulate::Flare->new(
            {
                name => $flare->{name},
                argv => $flare->{args},
            }
        );

        if ($flare->{is_flares_plan_start})
        {
            last GET_FLARES;
        }
    }

    return \@flares;
}

1;

=head1 COPYRIGHT & LICENSE

Copyright 2013 by Shlomi Fish

This program is distributed under the MIT (X11) License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut
