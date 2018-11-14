package FreecellSolver::QunitWrap;

use strict;
use warnings;

use Moo;
use Path::Tiny qw/ cwd /;

has 'tests' => ( is => 'ro', required => 1 );

sub run
{
    my ( $self, $args ) = @_;

    local $ENV{NODE_PATH} = cwd->child(qw(lib for-node js));
    system( 'qunit', $self->tests ) and die "qunit failed!";

    return;
}

1;
