package FC_Solve::CmdLine::Simulate::Flare;

use strict;
use warnings;

use MooX qw( late );

has 'name' => ( is => 'ro', isa => 'Str',           required => 1 );
has 'argv' => ( is => 'ro', isa => 'ArrayRef[Str]', required => 1 );

1;
