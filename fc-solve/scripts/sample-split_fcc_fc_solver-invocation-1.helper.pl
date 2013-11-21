#!/usr/bin/perl

use strict;
use warnings;

use FC_Solve::Base64;
use Games::Solitaire::FC_Solve::DeltaStater::DeBondt;
use IO::All;

my $delta = Games::Solitaire::FC_Solve::DeltaStater::DeBondt->new(
    {init_state_str => io->file($ENV{I})->all(), }
);
$delta->set_derived({state_str => io->file($ENV{I})->all(), });

my $token = $delta->encode_composite();

my $buffer = '';
my $count = 0;
while ($token > 0)
{
    $count++;
    $buffer .= pack('C', ($token & 0xFF));
    $token >>= 8;
}
while ($count < 16)
{
    $count++;
    $buffer .= '\0';
}

print
    FC_Solve::Base64::base64_encode($buffer),
    " 0 ",
    FC_Solve::Base64::base64_encode(q{}),
    "\n"
    ;
