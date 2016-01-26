package Games::Solitaire::Verify::Solution::Base;

use strict;
use warnings;

=head1 NAME

Games::Solitaire::Verify::Solution::Base - common base class for
all Games::Solitaire::Verify::Solution::* classes.

=cut

use parent 'Games::Solitaire::Verify::Base';

# "_ln" is line number
# "_i" is input filehandle.
__PACKAGE__->mk_acc_ref([qw(
    _i
    _ln
    _variant
    _variant_params
    _state
    _move
    _reached_end
    )]);

# _l is short for _get_line()
sub _l
{
    my $s = shift;

    # We use this instead of the accessor for speed.
    $s->{_ln}++;

    return shift(@{$s->{_i}});
}

sub _init
{
    my ($self, $args) = @_;

    $self->_i(
        [ split /^/ms, do {
                local $/;
                my $t = readline($args->{input_fh});
                $t =~ s/ +$//gms;
                $t;
            }
    ]);
    $self->_ln(0);

    return;
}

1;

