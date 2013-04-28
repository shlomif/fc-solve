#!/usr/bin/perl -C

use strict;
use warnings;

use utf8;
use 5.014;

sub to_unicode
{
    my ($s) = @_;
    return ($s =~ tr/HCDS/♥♣♦♠/r);
}

my $rank_re = qr/[A23456789TJQK]/;
my $ext_rank_re = qr/[0A23456789TJQK]/;
my $suit_re = qr/[HCDS]/;

binmode STDOUT, ':encoding(utf8)';
while (<>)
{
    print s{\b($rank_re)($suit_re)\b}{$1 . to_unicode($2)}reg =~
          s{\b($suit_re)(-$ext_rank_re)\b}{to_unicode($1). $2}reg
}
