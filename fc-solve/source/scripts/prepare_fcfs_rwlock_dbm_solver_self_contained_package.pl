#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use File::Path;

my $dest_dir = 'dbm_fcs_for_amadiro';
mkpath("$dest_dir");
mkpath("$dest_dir/libavl");
mkpath("$dest_dir/pthread");

system(qq{./Tatzer -l x64b --nfc=2 --states-type=COMPACT_STATES --dbm=kaztree});
# my @modules = ('app_str.o', 'card.o', 'dbm_solver.o', 'state.o', 'dbm_kaztree.o', 'rwlock.o', 'queue.o', 'libavl/avl.o', 'meta_alloc.o',);
my @modules = ('app_str.o', 'card.o', 'dbm_solver.o', 'state.o', 'dbm_kaztree.o', 'libavl/avl.o', 'meta_alloc.o',);

foreach my $fn ('app_str.c', 'card.c', 'dbm_solver.c', 'state.c',
    'dbm_kaztree.c', 'card.h', 'config.h', 'state.h',
    'dbm_solver.h', 'kaz_tree.h', 'dbm_solver_key.h',
    'fcs_move.h', 'inline.h', 'bool.h', 'internal_move_struct.h', 'app_str.h',
    'delta_states.c', 'delta_states.h', 'fcs_dllexport.h', 'bit_rw.h', 
    'fcs_enums.h', 'unused.h',
    'portable_time.h', 'dbm_calc_derived.h', 'dbm_calc_derived_iface.h',
    'dbm_common.h', 'libavl/avl.c', 'libavl/avl.h', 'offloading_queue.h',
    'indirect_buffer.h', 'generic_tree.h', 'meta_alloc.h', 'meta_alloc.c',
    'fcc_brfs_test.h','dbm_kaztree_compare.h', 'delta_states_iface.h',
    'dbm_cache.h', 'dbm_lru_cache.h',
)
{
    io($fn) > io("$dest_dir/$fn");
}

foreach my $fn ('rwlock.c', 'queue.c', 'pthread/rwlock_fcfs.h', 'pthread/rwlock_fcfs_queue.h')
{
    io("/home/shlomif/progs/C/pthreads/rwlock/fcfs-rwlock/pthreads/$fn") > io("$dest_dir/$fn")
}

my @deals = (
    982,
);

# my $deal_idx = 982;
foreach my $deal_idx (@deals)
{
    system(qq{python board_gen/make_pysol_freecell_board.py -t --ms $deal_idx > $dest_dir/$deal_idx.board});
}

@modules = sort { $a cmp $b } @modules;

io("$dest_dir/Makefile")->print(<<"EOF");
TARGET = dbm_fc_solver
DEALS = @deals

OFFLOAD_DIR_PATH = /tmp/fc-solve-dbm-queue-offload-dir
OFFLOAD_DIR_PATH_STAMP = \$(OFFLOAD_DIR_PATH)/stamp

DEALS_DUMPS = \$(patsubst %,%.dump,\$(DEALS))
THREADS = 12

CFLAGS = -O3 -march=native -fomit-frame-pointer -DFCS_DBM_WITHOUT_CACHES=1 -DFCS_DBM_USE_RWLOCK=1 -DFCS_DBM_USE_LIBAVL=1 -DFCS_LIBAVL_STORE_WHOLE_KEYS=1 -DFCS_DBM_RECORD_POINTER_REPR=1 -I. -I./libavl
MODULES = @modules

all: \$(TARGET) \$(OFFLOAD_DIR_PATH_STAMP)

\$(OFFLOAD_DIR_PATH_STAMP):
\tmkdir -p \$(OFFLOAD_DIR_PATH)
\ttouch \$\@

\$(TARGET): \$(MODULES)
\tgcc \$(CFLAGS) -fwhole-program -o \$\@ \$(MODULES) -lm -lpthread

\$(MODULES): %.o: %.c
\tgcc -c \$(CFLAGS) -o \$\@ \$<

run: \$(DEALS_DUMPS)

\$(DEALS_DUMPS): %.dump: all
\t./\$(TARGET) --num-threads \$(THREADS) --offload-dir-path \$(OFFLOAD_DIR_PATH) \$(patsubst %.dump,%.board,\$\@) | tee \$\@ \$\@.backup

%.show:
\t\@echo "\$* = \$(\$*)"
EOF

