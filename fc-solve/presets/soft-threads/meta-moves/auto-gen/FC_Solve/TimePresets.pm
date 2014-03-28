package FC_Solve::TimePresets;

use strict;
use warnings;

use MooX qw/ late /;

use AI::Pathfinding::OptimizeMultiple::DataInputObj;

use PDL ();

has start_board => (is => 'ro', isa => 'Int', default => 1);
has num_boards  => (is => 'ro', isa => 'Int', default => 32000);
has input_obj   => (
    is => 'ro',
    isa => 'AI::Pathfinding::OptimizeMultiple::DataInputObj',
    lazy => 1,
    default => sub {
        my ($self) = @_;

        return AI::Pathfinding::OptimizeMultiple::DataInputObj->new(
            {
                start_board => $self->start_board,
                num_boards => $self->num_boards,
            }
        );
    },
    handles =>
    [
        qw(
        _get_scan_cmd_line
        get_next_id
        get_scans_lens_iters_pdls
        get_scans_iters_pdls
        get_scan_ids_aref
        lookup_scan_idx_based_on_id
        selected_scans
        time_scan
        )
    ]
);

sub calc_params_from_environment
{
    my ($self) = @_;

    my %params;
    if (exists($ENV{FC_NUM}))
    {
        $params{freecells_num} = $ENV{FC_NUM};
    }
    if (exists($ENV{FC_VARIANT}))
    {
        $params{variant} = $ENV{FC_VARIANT};
    }

    return \%params;
}

sub calc_scans_lens_data
{
    my $self = shift;

    my $data_hash_ref = $self->get_scans_lens_iters_pdls();

    my $scans_lens_data = PDL::cat( @{$data_hash_ref}{
            @{ $self->get_scan_ids_aref }
        })->xchg(1,3)->clump(2..3);

    return $scans_lens_data;
}

1;

=head1 COPYRIGHT & LICENSE

Copyright 2014 by Shlomi Fish

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
