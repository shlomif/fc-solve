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
my $num_freecells = $ENV{NUM_FC} || 4;

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
    $num_cpus = $num_threads = 24;
    $mem = 500;
    $num_hours = 700;
}
elsif ($is_am)
{
    $flto = 0;
    $num_cpus = $num_threads = 12;
    $mem = 127;
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

my $main_base = $depth_dbm ? "depth_dbm_solver" : "dbm_solver";

# my @modules = ('card.o', 'dbm_solver.o', 'state.o', 'dbm_kaztree.o', 'rwlock.o', 'queue.o', 'libavl/avl.o', 'meta_alloc.o',);
my @modules = ('card.o', "$main_base.o", 'state.o', 'dbm_kaztree.o', 'libavl/avl.o', 'meta_alloc.o',);

foreach my $fn ('card.c', "$main_base.c", 'state.c',
    'dbm_kaztree.c', 'state.h', 'lock.h',
    'dbm_solver.h', 'dbm_solver_head.h', 'dll_thunk.h', 'kaz_tree.h', 'dbm_solver_key.h',
    'fcs_move.h', 'inline.h', 'bool.h', 'internal_move_struct.h',
    'delta_states.c', 'delta_states.h', 'fcs_dllexport.h', 'bit_rw.h',
    'fcs_enums.h', 'unused.h', 'fcs_limit.h',
    'portable_time.h', 'dbm_calc_derived.h', 'dbm_calc_derived_iface.h',
    'libavl/avl.c', 'libavl/avl.h', 'offloading_queue.h',
    'indirect_buffer.h', 'generic_tree.h', 'meta_alloc.h', 'meta_alloc.c',
    'fcc_brfs_test.h','dbm_kaztree_compare.h', 'delta_states_iface.h',
    'dbm_cache.h', 'dbm_lru_cache.h', 'dbm_trace.h', 'dbm_procs.h',
    'delta_states_debondt.c', 'delta_states_any.h', 'delta_states_debondt.h',
    'debondt_delta_states_iface.h', 'read_state.h',
    'var_base_reader.h', 'var_base_writer.h', 'rinutils.h', 'alloc_wrap.h',
    'count.h', 'likely.h', 'min_and_max.h', 'portable_int32.h', 'str_utils.h',
    'typeof_wrap.h', 'game_type_limit.h', 'p2u_rank.h', 'delta_states_debondt_impl.h', 'var_base_int.h', 'dbm_move_to_string.h', 'render_state.h',
    'depth_dbm_procs.h', 'dbm_procs_inner.h',
)
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

for my $fn ("prepare_pbs_deal.bash")
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
6825625742
/);

foreach my $deal_idx (@deals)
{
    if (system(qq{pi-make-microsoft-freecell-board -t $deal_idx > $dest_dir/$deal_idx.board}) != 0)
    {
        die "Could not generate deal no. $deal_idx.";
    }
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
\tTHREADS="\$(THREADS)" MEM="\$(MEM)" CPUS="\$(CPUS)" HOURS="\$(HOURS)" bash prepare_pbs_deal.bash "\$(patsubst jobs/%.job.sh,%,\$\@)" "\$\@"

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
