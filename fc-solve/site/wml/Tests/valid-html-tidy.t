#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

use HTML::Tidy;
use File::Find::Object::Rule;
use IO::All qw/ io /;

local $SIG{__WARN__} = sub {
    my $w = shift;
    if ($w !~ /\AUse of uninitialized/)
    {
        die $w;
    }
    return;
};

my $tidy = HTML::Tidy->new({ output_xhtml => 1, });
$tidy->ignore( type => TIDY_WARNING, type => TIDY_INFO );

my $error_count = 0;

foreach my $dir ('./dest', './dest-prod/')
{
    foreach my $fn (File::Find::Object::Rule->file()->name(qr/\.x?html\z/)->in($dir))
    {
        $tidy->parse( $fn, (scalar io->file($fn)->slurp()));

        foreach my $message ( $tidy->messages ) {
            $error_count++;
            diag( $message->as_string);
        }
    }
}

# TEST
is ($error_count, 0, "No errors");
