package Games::Solitaire::FC_Solve::DeltaStater::BitReader;

use strict;
use warnings;

use base 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref([qw(bits _bit_idx)]);

sub _init
{
    my $self = shift;
    my $args = shift;

    $self->bits($args->{bits});

    $self->_bit_idx(0);

    return;
}

sub _next_idx
{
    my $self = shift;

    my $ret = $self->_bit_idx;

    $self->_bit_idx($ret+1);

    return $ret;
}

sub read
{
    my ($self, $len) = @_;

    my $idx = 0;
    my $ret = 0;
    while ($idx < $len)
    {
        $ret |= (vec($self->bits(), $self->_next_idx(), 1) << $idx);
    }
    continue
    {
        $idx++;
    }

    return $ret;
}

1;

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2011 Shlomi Fish

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

