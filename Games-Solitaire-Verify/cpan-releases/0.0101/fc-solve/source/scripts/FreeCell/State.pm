package FreeCell::State;

use strict;

use FreeCell::Object;
use FreeCell::Card;

use vars qw(@ISA);
@ISA=qw(FreeCell::Object);

sub initialize
{
    my $self = shift;

    $self->{'stacks'} = (map { [] } (1 .. 8));
    $self->{'founds'} = [ 0, 0, 0, 0];
    $self->{'fc'} = (map { FreeCell::Card->new(); } (1 .. 4));

    return 0;
}

sub read_from_file
{
    die "Not Implemented Yet!\n";
}
