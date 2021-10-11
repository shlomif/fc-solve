package Shlomif::Spelling::Iface_Local;

use strict;
use warnings;

use Moo;

use Shlomif::Spelling::Check_Local ();
use Shlomif::Spelling::FindFiles   ();

has obj => (
    is      => 'ro',
    default => sub { return Shlomif::Spelling::Check_Local->new(); }
);

has files => (
    is      => 'ro',
    default => sub { return Shlomif::Spelling::FindFiles->new->list_htmls(); }
);

sub run
{
    my ($self) = @_;

    return $self->obj->spell_check(
        {
            files => $self->files,
        },
    );
}

sub test_spelling
{
    my ( $self, $blurb ) = @_;

    return $self->obj->obj->test_spelling(
        {
            files => $self->files,
            blurb => $blurb,
        },
    );
}

1;
