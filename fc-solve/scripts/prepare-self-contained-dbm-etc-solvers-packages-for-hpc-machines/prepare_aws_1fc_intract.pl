#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use File::Basename;
use File::Path;
use File::Temp qw / tempdir /;
use File::Spec;
use Cwd qw / getcwd /;

use Getopt::Long;

my $BIN_DIR = File::Spec->rel2abs(File::Basename::dirname($0));

my $flto = 0;

my $who = 'sub';

my $mem = 200;
my $num_cpus = 4;
my $num_threads = $num_cpus;
my $num_hours = 120;

my $march_flag = "-march=native";
my $num_freecells = $ENV{NUM_FC} || 1;

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

if ($sub)
{
    $flto = 1;
    # $num_cpus = 12;
    $num_cpus = $num_threads = 16;
    $mem = 64;
    $num_hours = 700;
}
elsif ($is_am)
{
    $flto = 0;
    $num_cpus = $num_threads = 16;
    $mem = 64;
    $num_hours = 700;
    $march_flag = '';
}

my $temp_dir = tempdir( CLEANUP => 1 );
my $dest_dir_base = $sub ? 'dbm_fcs_for_sub' : 'dbm_fcs_for_amadiro';
my $dest_dir = File::Spec->catdir($temp_dir, $dest_dir_base);
my $build_dir = File::Spec->catdir($temp_dir, 'build');

my $src_path = getcwd();

mkpath("$dest_dir");
mkpath("$dest_dir/libavl");
mkpath("$dest_dir/pthread");
mkpath($build_dir);

chdir ($build_dir);
system(File::Spec->catdir($src_path, "Tatzer"),
    qw{-l x64b}, "--nfc=$num_freecells", qw{--states-type=COMPACT_STATES --dbm=kaztree},
    $src_path
);

use FindBin ();
use lib "$FindBin::Bin";
use PrepareCommon;

my $obj = PrepareCommon->new({depth_dbm => $depth_dbm});

foreach my $fn (@{$obj->src_filenames()})
{
    io("$src_path/$fn") > io("$dest_dir/$fn");
}

foreach my $fn ('dbm_common.h')
{
    io("$dest_dir/$fn")->print(io("$src_path/$fn")->slurp() =~ s{^(#define FCS_DBM_FREECELLS_NUM\s*)\d+(\s*)$}{$1$num_freecells$2}mrs);
}

foreach my $fn ('config.h', 'fcs_back_compat.h',)
{
    io("./$fn") > io("$dest_dir/$fn");
}

foreach my $fn ('rwlock.c', 'queue.c', 'pthread/rwlock_fcfs.h', 'pthread/rwlock_fcfs_queue.h')
{
    io("/home/shlomif/progs/C/pthreads/rwlock/fcfs-rwlock/pthreads/$fn") > io("$dest_dir/$fn")
}

for my $fn ("prepare_vendu_deal.bash")
{
    io("$BIN_DIR/$fn") > io("$dest_dir/$fn")
}

=begin old

my @deals =
(qw/
219837216
1252215044
2255988055
2901685480
2902413565
4260084873
6687921694
6825625742
7489392343
    /);

=end old

=cut

my @deals =
(qw/
20410
24966
/);

foreach my $deal_idx (@deals)
{
    if (system(qq{pi-make-microsoft-freecell-board -t $deal_idx > $dest_dir/$deal_idx.board}) != 0)
    {
        die "Could not generate deal no. $deal_idx.";
    }
}

my @modules = @{ $obj->modules };

my $more_cflags = $flto ? " -flto " : '';

if (0)
{
    $more_cflags .= ' -DFCS_DBM_USE_RWLOCK=1 ';
}

io("$dest_dir/Makefile")->print(<<"EOF");
TARGET = dbm_fc_solver
DEALS = @deals

DEALS_DUMPS = \$(patsubst %,%.dump,\$(DEALS))
DEALS_BOARDS = \$(patsubst %,%.board,\$(DEALS))

THREADS = $num_threads
MEM = $mem
CPUS = $num_cpus
HOURS = $num_hours

CFLAGS = -std=gnu99 -O3 $march_flag -fomit-frame-pointer $more_cflags -DFCS_DBM_WITHOUT_CACHES=1 -DFCS_DBM_USE_LIBAVL=1 -DFCS_LIBAVL_STORE_WHOLE_KEYS=1 -DFCS_DBM_RECORD_POINTER_REPR=1 -DFCS_DEBONDT_DELTA_STATES=1 -I. -I./libavl
MODULES = @modules

JOBS = \$(patsubst %,jobs/%.job.sh,\$(DEALS))
JOBS_STAMP = jobs/STAMP

all: \$(TARGET) \$(JOBS)

\$(TARGET): \$(MODULES)
\tgcc \$(CFLAGS) -fwhole-program -o \$\@ \$(MODULES) -Bstatic -lm -lpthread -ltcmalloc
\tstrip \$\@

\$(MODULES): %.o: %.c
\tgcc -c \$(CFLAGS) -o \$\@ \$<

\$(JOBS_STAMP):
\tmkdir -p jobs
\ttouch \$\@

\$(JOBS): %: \$(JOBS_STAMP) \$(RESULT)
\tTHREADS="\$(THREADS)" MEM="\$(MEM)" CPUS="\$(CPUS)" HOURS="\$(HOURS)" bash prepare_vendu_deal.bash "\$(patsubst jobs/%.job.sh,%,\$\@)" "\$\@"

%.show:
\t\@echo "\$* = \$(\$*)"
EOF

chdir($temp_dir);
system("tar", "-cavf", "$src_path/$dest_dir_base.tar.xz", "$dest_dir_base/");
chdir($src_path);

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
