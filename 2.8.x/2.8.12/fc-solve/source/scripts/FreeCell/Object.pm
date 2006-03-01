package FreeCell::Object;

use strict;

sub new
{
    my $class = shift;
    my $self = {};

    bless $self, $class;

    $self->initialize(@_);

    return $self;
}

sub initialize
{
    my $self = shift;

    die "Should be overriden!";
}

1;
