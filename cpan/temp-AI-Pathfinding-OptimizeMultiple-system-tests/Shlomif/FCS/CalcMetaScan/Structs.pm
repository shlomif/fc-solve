package Shlomif::FCS::CalcMetaScan::Structs::Scan;

use strict;
use warnings;

use base 'Shlomif::FCS::CalcMetaScan::Base';

use vars (qw(@fields %fields_map));
@fields = (qw(
    cmd_line
    id
    used
));

%fields_map = (map { $_ => 1 } @fields);

__PACKAGE__->mk_acc_ref(\@fields);

sub _init
{
    my $self = shift;
    my (%args) = @_;
    $self->used(0);
    foreach my $field (grep {exists($fields_map{$_})} keys(%args))
    {
        $self->$field($args{$field});
    }

    return 0;
}

sub mark_as_used
{
    my $self = shift;
    $self->used(1);
}

sub is_used
{
    my $self = shift;
    return $self->used();
}

=head2 $scan->data_file_path()

Returns the path to the data file of the scan, where the numbers of iterations
per board are stored.

=cut

sub data_file_path
{
    my $self = shift;

    return "./data/" . $self->id() .  ".data.bin";
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2010 Shlomi Fish

=cut
