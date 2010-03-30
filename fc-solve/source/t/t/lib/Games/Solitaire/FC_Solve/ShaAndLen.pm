package Games::Solitaire::FC_Solve::ShaAndLen;

use strict;
use warnings;

use Digest::SHA;

use base 'Class::Accessor';

__PACKAGE__->mk_accessors(qw(
    _len
    _hasher
    ));

sub new
{
    my $class = shift;
    my $self = {};

    bless $self, $class;

    $self->_init(@_);

    return $self;
}

sub _init
{
    my $self = shift;
    my $args = shift;

    $self->_len(0);
    $self->_hasher(Digest::SHA->new(256));

    return 0;
}

sub add
{
    my $self = shift;
    my $string = shift;

    $self->_len($self->_len()+length($string));
    $self->_hasher->add($string);

    return;
}

sub len
{
    my $self = shift;

    return $self->_len();
}

sub hexdigest
{
    my $self = shift;

    return $self->_hasher->clone()->hexdigest();
}

sub _unity
{
    return shift;
}

sub add_file
{
    my $self = shift;
    my $fh = shift;

    return $self->add_processed_slurp($fh, \&_unity);
}

sub add_processed_slurp
{
    my $self = shift;
    my $fh = shift;
    my $callback = shift;

    my $buffer;
    {
        local $/;
        $buffer = <$fh>;
    }
    return $self->add(scalar($callback->($buffer)));
}

1;



=head1 COPYRIGHT AND LICENSE

Copyright (c) 2000 Shlomi Fish

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

