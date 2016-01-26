package Games::Solitaire::Verify::Solution::Base;

use strict;
use warnings;

=head1 NAME

Games::Solitaire::Verify::Solution::Base - common base class for
all Games::Solitaire::Verify::Solution::* classes.

=cut

use parent 'Games::Solitaire::Verify::Base';

# "_ln" is line number
# "_st" is the "state" (or board/layout).
# "_i" is input filehandle.
# "_V" is short for variant arguments.
__PACKAGE__->mk_acc_ref([qw(
    _V
    _i
    _ln
    _variant
    _variant_params
    _st
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

sub _calc_variant_args
{
    my $self = shift;

    return [
        variant => $self->_variant(),
        (
            ($self->_variant eq 'custom')
            ? ('variant_params' => $self->_variant_params())
            : ()
        )
    ];
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

    $self->_variant($args->{variant});

    $self->_variant_params(
        $self->_variant eq 'custom'
        ? $args->{variant_params}
        : Games::Solitaire::Verify::VariantsMap->get_variant_by_id(
            $self->_variant()
        )
    );

    $self->_V($self->_calc_variant_args());

    return;
}

1;

