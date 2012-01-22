#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use File::Path;

my $dest_dir = 'dbm_fcs_for_amadiro';
mkpath("$dest_dir");
mkpath("$dest_dir/pthread");

system(qq{./Tatzer -l x64b --nfc=2 --states-type=COMPACT_STATES --dbm=kaztree});
my @modules = ('alloc.o', 'app_str.o', 'card.o', 'dbm_solver.o', 'state.o', 'dbm_kaztree.o', 'rwlock.o', 'queue.o',);

foreach my $fn ('alloc.c', 'app_str.c', 'card.c', 'dbm_solver.c', 'state.c',
    'dbm_kaztree.c', 'alloc.h', 'card.h', 'config.h', 'state.h',
    'dbm_solver.h', 'kaz_tree.c', 'kaz_tree.h', 'dbm_solver_key.h',
    'fcs_move.h', 'inline.h', 'bool.h', 'internal_move_struct.h', 'app_str.h',
    'delta_states.c', 'fcs_dllexport.h', 'bit_rw.h', 'fcs_enums.h', 'unused.h',
    'portable_time.h',
)
{
    io($fn) > io("$dest_dir/$fn");
}

foreach my $fn ('rwlock.c', 'queue.c', 'pthread/rwlock_fcfs.h', 'pthread/rwlock_fcfs_queue.h')
{
    io("/home/shlomif/progs/C/pthreads/rwlock/fcfs-rwlock/pthreads/$fn") > io("$dest_dir/$fn")
}

my @deals = (
    5435,
    6090,
    7728,
    9034,
    11266,
    13705,
    17760,
    17880,
    18446,
    19671,
    28188,
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

DEALS_DUMPS = \$(patsubst %,%.dump,\$(DEALS))
THREADS = 12

CFLAGS = -O3 -march=native -fomit-frame-pointer -DFCS_DBM_WITHOUT_CACHES=1 -DFCS_DBM_USE_RWLOCK=1 -I.
MODULES = @modules

all: \$(TARGET)

\$(TARGET): \$(MODULES)
\tgcc \$(CFLAGS) -fwhole-program -o \$\@ \$(MODULES) -lm -lpthread

\$(MODULES): %.o: %.c
\tgcc -c \$(CFLAGS) -o \$\@ \$<

run: \$(DEALS_DUMPS)

\$(DEALS_DUMPS): %.dump: all
\t./\$(TARGET) --num-threads \$(THREADS) \$(patsubst %.dump,%.board,\$\@) | tee \$\@

%.show:
\t\@echo "\$* = \$(\$*)"
EOF

