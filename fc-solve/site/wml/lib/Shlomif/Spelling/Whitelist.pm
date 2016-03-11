package Shlomif::Spelling::Whitelist;

use strict;
use warnings;

use MooX qw/late/;

extends('HTML::Spelling::Site::Whitelist');

has '+filename' => (default => 'lib/hunspell/whitelist1.txt');

1;

