package Games::Solitaire::FC_Solve::DeltaStater::VariableBaseDigitsWriter;

use strict;
use warnings;

use base 'Games::Solitaire::Verify::Base';

use Math::BigInt try => 'GMP';

use Carp ();

__PACKAGE__->mk_acc_ref([qw(_data _multiplier)]);

sub _init
{
    my $self = shift;

    $self->_data( Math::BigInt->new(0) );
    $self->_multiplier( Math::BigInt->new(1) );

    return;
}

sub write
{
    my ($self, $args) = @_;

    my $base = $args->{base};
    my $item = $args->{item};

    if ($item < 0)
    {
        Carp::confess ("Item '$item' cannot be less than 0.");
    }
    elsif ($item >= $base)
    {
        Carp::confess ("Base '$base' must be greater than item '$item'");
    }

    $self->_data(
        $self->_data() + $self->_multiplier() * $item
    );

    $self->_multiplier(
        $self->_multiplier() * $base
    );

    return;
}

sub get_data
{
    my $self = shift;

    return $self->_data()->copy();
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

