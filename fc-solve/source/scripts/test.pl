use strict;

use FreeCell::Card;
use FreeCell::State;

use FileHandle;


use Data::Dumper;

# my $card = FreeCell::Card::from_string($ARGV[0]);
# my $d = Data::Dumper->new([$card], [ '$card']);
# print $d->Dump();

my $fh = FileHandle->new();
$fh->open("<test.txt");
my $state = FreeCell::State::read_from_file($fh);
my $d = Data::Dumper->new([$state], [ '$card']);
print $d->Dump();
$fh->close();

