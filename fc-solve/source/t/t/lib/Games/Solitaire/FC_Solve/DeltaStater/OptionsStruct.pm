package Games::Solitaire::FC_Solve::DeltaStater::OptionsStruct;

use strict;
use warnings;

use base 'Games::Solitaire::Verify::Base';

my $UNKNOWN = 0;
my $IMPOSSIBLE = 1;
my $TRUE = 2;

use List::MoreUtils qw(firstidx);

__PACKAGE__->mk_acc_ref([qw(_options _which_option _num_unknowns)]);

sub _init
{
    my ($self, $args) = @_;

    my $count = $args->{count};

    $self->_options([($UNKNOWN) x $count]);

    $self->_which_option(-1);

    $self->_num_unknowns($count);

    return;
}

sub get_option
{
    my ($self,$idx) = @_;

    return $self->_options->[$idx];
}

sub _set_option
{
    my ($self,$idx, $val) = @_;

    if (($val != $TRUE) && ($val != $IMPOSSIBLE))
    {
        Carp::confess "Must be true or impossible.";
    }

    if ($self->get_option($idx) != $UNKNOWN)
    {
        Carp::confess "Option was already set.";
    }

    $self->_options->[$idx] = $val;
}

sub get_verdict
{
    my ($self) = @_;

    return $self->_which_option;
}

sub mark_as_impossible
{
    my $self = shift;
    my $idx = shift;

    if ($self->_which_option() >= 0)
    {
        Carp::confess "Already decided!";
    }

    if ($self->get_option($idx) != $UNKNOWN)
    {
        Carp::confess "Already set.";
    }

    $self->_set_option($idx, $IMPOSSIBLE);
    $self->_num_unknowns($self->_num_unknowns() -1);

    if ($self->_num_unknowns() == 1)
    {
        $self->_which_option(
            firstidx { $self->get_option($_) == $UNKNOWN }
            (0 .. $#{$self->_options()})
        );
        $self->_set_option($self->_which_option(), $TRUE);
        $self->_num_unknowns(0);
    }

    return;
}

sub mark_as_true
{
    my $self = shift;
    my $option_idx = shift;

    if ($self->_which_option() >= 0)
    {
        Carp::confess "Already decided!";
    }

    if ($self->get_option($option_idx) != $UNKNOWN)
    {
        Carp::confess "Already set.";
    }

    $self->_set_option($option_idx, $TRUE);
    $self->_which_option($option_idx);
    $self->_num_unknowns(0);

    foreach my $idx (0 .. $#{$self->_options})
    {
        if ($self->get_option($idx) == $UNKNOWN)
        {
            $self->_set_option($idx, $IMPOSSIBLE);
        }
    }

    return;
}

1;

=head1 COPYRIGHT & LICENSE

Copyright 2012 by Shlomi Fish

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
