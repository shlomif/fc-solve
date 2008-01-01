use strict;

use FreeCell::Card;

use Data::Dumper;

my $card = FreeCell::Card::from_string($ARGV[0]);
my $d = Data::Dumper->new([$card], [ '$card']);
print $d->Dump();
