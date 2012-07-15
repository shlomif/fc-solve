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
my $num_cpus = 4;
my $num_threads = $num_cpus;

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

if ($sub)
{
    $flto = 1;
    # $num_cpus = 12;
    $num_cpus = $num_threads = 4;
    $mem = 400;
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
    qw{-l x64b --nfc=2 --states-type=COMPACT_STATES --dbm=kaztree},
    $src_path
);

# my @modules = ('app_str.o', 'card.o', 'dbm_solver.o', 'state.o', 'dbm_kaztree.o', 'rwlock.o', 'queue.o', 'libavl/avl.o', 'meta_alloc.o',);
my @modules = ('app_str.o', 'card.o', 'dbm_solver.o', 'state.o', 'dbm_kaztree.o', 'libavl/avl.o', 'meta_alloc.o',);

foreach my $fn ('app_str.c', 'card.c', 'dbm_solver.c', 'state.c',
    'dbm_kaztree.c', 'card.h', 'state.h',
    'dbm_solver.h', 'kaz_tree.h', 'dbm_solver_key.h',
    'fcs_move.h', 'inline.h', 'bool.h', 'internal_move_struct.h', 'app_str.h',
    'delta_states.c', 'delta_states.h', 'fcs_dllexport.h', 'bit_rw.h',
    'fcs_enums.h', 'unused.h',
    'portable_time.h', 'dbm_calc_derived.h', 'dbm_calc_derived_iface.h',
    'dbm_common.h', 'libavl/avl.c', 'libavl/avl.h', 'offloading_queue.h',
    'indirect_buffer.h', 'generic_tree.h', 'meta_alloc.h', 'meta_alloc.c',
    'fcc_brfs_test.h','dbm_kaztree_compare.h', 'delta_states_iface.h',
    'dbm_cache.h', 'dbm_lru_cache.h',
    'delta_states_debondt.c', 'delta_states_any.h', 'delta_states_debondt.h',
    'debondt_delta_states_iface.h',
    'var_base_reader.h', 'var_base_writer.h',
)
{
    io("$src_path/$fn") > io("$dest_dir/$fn");
}

foreach my $fn ('config.h')
{
    io("./$fn") > io("$dest_dir/$fn");
}

foreach my $fn ('rwlock.c', 'queue.c', 'pthread/rwlock_fcfs.h', 'pthread/rwlock_fcfs_queue.h')
{
    io("/home/shlomif/progs/C/pthreads/rwlock/fcfs-rwlock/pthreads/$fn") > io("$dest_dir/$fn")
}

for my $fn ("prepare_pbs_deal.bash")
{
    io("$src_path/scripts/$fn") > io("$dest_dir/$fn")
}

=begin old

my @deals = (
    33668,
    34152,
    41701,
    48819
);

my @deals = (
    102257,
    129561,
    141951,
    142720,
    142864,
    146457,
    147148,
    155990,
    169944,
    171337,
    175864,
    176168,
    182980,
    198960,
    212009,
    215164,
    222860,
    224115,
    225805,
    234834,
    235561,
    239422,
    243540,
    244525,
    247736,
    267112,
    271131,
    284184,
    284204,
    287876,
    291131,
    295997,
    298904,
    310397,
    311564,
    312381,
    312736,
    317289,
    320583,
    329150,
    336145,
    347972,
    359432,
    361934,
    366584,
    367103,
    373120,
    375783,
    378764,
    384243,
    384543,
    392121,
    396792,
    397067,
    397251,
);

my @deals = (
    142720,
    146457,
    169944,
    176168,
    182980,
    198960,
    215164,
    225805,
    234834,
    235561,
    239422,
    243540,
    244525,
    287876,
    295997,
    298904,
    312381,
    317289,
    320583,
    329150,
    336145,
    347972,
    359432,
    361934,
    366584,
    367103,
    373120,
    375783,
    378764,
    384243,
    384543,
    392121,
    396792,
    397067,
    397251,
);

=end old

=cut

my @deals = (
    384243,
);

# my $deal_idx = 982;
foreach my $deal_idx (@deals)
{
    system(qq{python $src_path/board_gen/make_pysol_freecell_board.py -t --ms $deal_idx > $dest_dir/$deal_idx.board});
}

@modules = sort { $a cmp $b } @modules;

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

CFLAGS = -O3 -march=native -fomit-frame-pointer $more_cflags -DFCS_DBM_WITHOUT_CACHES=1 -DFCS_DBM_USE_LIBAVL=1 -DFCS_LIBAVL_STORE_WHOLE_KEYS=1 -DFCS_DBM_RECORD_POINTER_REPR=1 -DFCS_DEBONDT_DELTA_STATES=1 -I. -I./libavl
MODULES = @modules

JOBS = \$(patsubst %,jobs/%.job.sh,\$(DEALS))
JOBS_STAMP = jobs/STAMP

all: \$(TARGET) \$(JOBS)

\$(TARGET): \$(MODULES)
\tgcc \$(CFLAGS) -fwhole-program -o \$\@ \$(MODULES) -lgmp -Bstatic -lm -lpthread
\tstrip \$\@

\$(MODULES): %.o: %.c
\tgcc -c \$(CFLAGS) -o \$\@ \$<

\$(JOBS_STAMP):
\tmkdir -p jobs
\ttouch \$\@

\$(JOBS): %: \$(JOBS_STAMP) \$(RESULT)
\tTHREADS="\$(THREADS)" MEM="\$(MEM)" CPUS="\$(CPUS)" bash prepare_pbs_deal.bash "\$(patsubst jobs/%.job.sh,%,\$\@)" "\$\@"

%.show:
\t\@echo "\$* = \$(\$*)"
EOF

chdir($temp_dir);
system("tar", "-cavf", "$src_path/$dest_dir_base.tar.gz", "$dest_dir_base/");
chdir($src_path);

=head1 COPYRIGHT & LICENSE

Copyright 2012 by Shlomi Fish

This program is distributed under the MIT (X11) License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut
