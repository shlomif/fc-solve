#!/usr/bin/env perl

use strict;
use warnings;
use lib './lib';
use HTML::Latemp::GenDeps ();

HTML::Latemp::GenDeps->new(
    {
        src_dir => 'src',
        src_var => '$(D)',
    }
)->run;
