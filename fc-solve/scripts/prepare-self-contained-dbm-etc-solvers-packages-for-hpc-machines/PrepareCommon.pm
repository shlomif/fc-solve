package PrepareCommon;

use strict;
use warnings;

use File::Spec;
use File::Basename ();
use File::Temp qw / tempdir /;
use Cwd qw / getcwd /;
use File::Path qw/ mkpath /;
use IO::All qw/ io /;

use Moose;

has 'depth_dbm'  => ( is => 'ro', isa => 'Bool', required => 1 );
has 'fcc_solver' => ( is => 'ro', isa => 'Bool', default  => 0 );
has [ 'dest_dir_base', 'march_flag' ] =>
    ( is => 'ro', isa => 'Str', required => 1 );
has 'flto' => ( is => 'ro', isa => 'Bool', required => 1 );
has [ 'mem', 'num_hours', 'num_threads' ] =>
    ( is => 'ro', isa => 'Int', required => 1 );
has ['num_freecells'] => ( is => 'ro', isa => 'Int', default => 4, );
has 'deals' => ( is => 'ro', isa => 'ArrayRef[Int]', required => 1 );

sub main_base
{
    my ($self) = @_;
    return
          $self->fcc_solver ? 'split_fcc_solver'
        : $self->depth_dbm  ? "depth_dbm_solver"
        :                     "dbm_solver";
}

sub src_filenames
{
    my ($self) = @_;

    my $on_fcc = sub {
        my $filenames = shift;
        return ( $self->fcc_solver ? (@$filenames) : () ),;
    };

    return [
        $self->main_base . '.c',
        'alloc_wrap.h',
        'bit_rw.h',
        'bool.h',
        'card.c',
        'count.h',
        'dbm_cache.h',
        'dbm_calc_derived.h',
        'dbm_calc_derived_iface.h',
        'dbm_kaztree_compare.h',
        'dbm_kaztree.c',
        'dbm_lru_cache.h',
        'dbm_move_to_string.h',
        'dbm_procs.h',
        'dbm_procs_inner.h',
        'dbm_solver.h',
        'dbm_solver_head.h',
        'dbm_trace.h',
        'debondt_delta_states_iface.h',
        'delta_states.c',
        'delta_states.h',
        'delta_states_any.h',
        'delta_states_debondt.c',
        'delta_states_debondt.h',
        'delta_states_debondt_impl.h',
        'delta_states_iface.h',
        'depth_dbm_procs.h',
        $on_fcc->( [ 'depth_multi_queue.h', ] ),
        $on_fcc->( [ 'fcs_base64.h', ] ),
        'fcc_brfs_test.h',
        'fcs_dllexport.h',
        'fcs-libavl/rb.c',
        'fcs-libavl/rb.h',
        'fcs_enums.h',
        'fcs_err.h',
        'fcs_limit.h',
        'fcs_move.h',
        'game_type_limit.h',
        'generic_tree.h',
        'indirect_buffer.h',
        'internal_move_struct.h',
        'is_parent.c',
        'is_parent.h',
        'kaz_tree.h',
        'likely.h',
        'lock.h',
        'meta_alloc.c',
        'meta_alloc.h',
        'min_and_max.h',
        'offloading_queue.h',
        'portable_time.h',
        'rank2str.h',
        'read_state.h',
        'render_state.h',
        'rinutils.h',
        'state.c',
        'state.h',
        'str_utils.h',
        $on_fcc->( [ 'sys/tree.h', ] ),
        'try_param.h',
        'typeof_wrap.h',
        'unused.h',
        'var_base_int.h',
        'var_base_reader.h',
        'var_base_writer.h',
    ];
}

sub modules
{
    my $self = shift;

    return [
        sort { $a cmp $b } $self->main_base . '.o', 'card.o',
        'dbm_kaztree.o',        'debondt__card_pos.o',
        'debondt__state_pos.o', 'is_parent.o',
        'fcs-libavl/rb.o',      'meta_alloc.o',
        'state.o',
    ];
}

sub run
{
    my $self = shift;

    my $BIN_DIR       = File::Spec->rel2abs( File::Basename::dirname($0) );
    my $num_freecells = $ENV{NUM_FC} || $self->num_freecells;
    my $temp_dir      = tempdir( CLEANUP => 1 );
    my $dest_dir_base = $self->dest_dir_base;
    my $dest_dir      = File::Spec->catdir( $temp_dir, $dest_dir_base );
    my $build_dir     = File::Spec->catdir( $temp_dir, 'build' );

    my $src_path = getcwd();

    mkpath("$dest_dir");
    mkpath("$dest_dir/fcs-libavl");
    mkpath("$dest_dir/pthread");
    if ( $self->fcc_solver )
    {
        mkpath("$dest_dir/sys");
    }
    mkpath($build_dir);

    chdir($build_dir);
    system( File::Spec->catdir( $src_path, "Tatzer" ),
        qw{-l x64b}, "--nfc=$num_freecells",
        qw{--states-type=COMPACT_STATES --dbm=kaztree}, $src_path );

    foreach my $fn ('dbm_common.h')
    {
        io("$dest_dir/$fn")
            ->print( io("$src_path/$fn")->slurp() =~
s{^(#define FCS_DBM_FREECELLS_NUM\s*)\d+(\s*)$}{$1$num_freecells$2}mrs
            );
    }

    foreach my $fn ( 'config.h', 'fcs_back_compat.h', )
    {
        io("./$fn") > io("$dest_dir/$fn");
    }

    foreach my $fn (
        'rwlock.c',              'queue.c',
        'pthread/rwlock_fcfs.h', 'pthread/rwlock_fcfs_queue.h'
        )
    {
        io("/home/shlomif/progs/C/pthreads/rwlock/fcfs-rwlock/pthreads/$fn") >
            io("$dest_dir/$fn");
    }

    for my $fn ("prepare_vendu_deal.bash")
    {
        io("$BIN_DIR/$fn") > io("$dest_dir/$fn");
    }

    my @deals = @{ $self->deals };
    foreach my $deal_idx (@deals)
    {
        if (
            system(
qq{python3 $src_path/board_gen/make_pysol_freecell_board.py --ms -t $deal_idx > $dest_dir/$deal_idx.board}
            ) != 0
            )
        {
            die "Could not generate deal no. $deal_idx.";
        }
    }

    {
        my $cwd = getcwd();
        chdir($dest_dir);
        if ( system( $^X, "$src_path/scripts/gen_is_parent_lookup_c.pl" ) != 0 )
        {
            die "Could not generate is_parent.{c,h}!";
        }
        chdir($cwd);
    }
    foreach my $fn ( @{ $self->src_filenames() } )
    {
        my $src  = "$src_path/$fn";
        my $dest = "$dest_dir/$fn";
        if ( !-f $dest )
        {
            if ( !-f $src )
            {
                die "'$src' is not a file!";
            }
            io($src) > io($dest);
        }
    }

    my @modules = @{ $self->modules };

    my $more_cflags = $self->flto ? " -flto " : '';

    if (0)
    {
        $more_cflags .= ' -DFCS_DBM_USE_RWLOCK=1 ';
    }

    my $num_threads = $self->num_threads;
    my $num_hours   = $self->num_hours;
    my $num_cpus    = $num_threads;
    my $mem         = $self->mem;
    my $march_flag  = $self->march_flag;

    io("$dest_dir/Makefile")->print(<<"EOF");
TARGET = dbm_fc_solver
DEALS = @deals

DEALS_DUMPS = \$(patsubst %,%.dump,\$(DEALS))
DEALS_BOARDS = \$(patsubst %,%.board,\$(DEALS))

THREADS = $num_threads
MEM = $mem
CPUS = $num_cpus
HOURS = $num_hours

CFLAGS = -std=gnu99 -O3 $march_flag -fomit-frame-pointer $more_cflags -DFCS_DBM_WITHOUT_CACHES=1 -DFCS_DBM_USE_LIBAVL=1 -DFCS_LIBAVL_STORE_WHOLE_KEYS=1 -DFCS_DBM_RECORD_POINTER_REPR=1 -DFCS_DEBONDT_DELTA_STATES=1 -I. -I./fcs-libavl
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
    system(
        "tar",                             "-cavf",
        "$src_path/$dest_dir_base.tar.xz", "$dest_dir_base/"
    );
    chdir($src_path);
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
