#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use File::Path;
use File::Temp qw / tempdir /;
use File::Spec;
use Cwd qw / getcwd /;

use Getopt::Long;

my $flto = 0;

my $who = 'sub';

my $mem = 200;
my $num_threads = 4;
my $num_hours = 120;

my $march_flag = "-march=native";
my $num_freecells = $ENV{NUM_FC} || 2;

GetOptions(
    'flto!' => \$flto,
    'who=s' => \$who,
    'mem=i' => \$mem,
) or die "No arguments";

if (!defined($who))
{
    die "Unknown who.";
}

my $sub = 1;
my $is_am = 0;

my $depth_dbm = 1;

if ($who eq 'am')
{
    $is_am = 1;
    $sub = 0;
}

my $dest_dir_base = 'dbm_fcs_for_sub';
if ($sub)
{
    $flto = 1;
    $num_threads = 24;
    $mem = 500;
    $num_hours = 700;
}
elsif ($is_am)
{
    $flto = 0;
    $num_threads = 12;
    $mem = 127;
    $num_hours = 700;
    $march_flag = '';
}

use FindBin ();
use lib "$FindBin::Bin";
use PrepareCommon;

my @deals = (9189909);
my $obj = PrepareCommon->new({depth_dbm => $depth_dbm, dest_dir_base => $dest_dir_base, flto => $flto, num_threads => $num_threads, mem => $mem, num_hours => $num_hours, march_flag => $march_flag, deals => [@deals],});
$obj->run;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
