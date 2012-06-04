#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use File::Path;

my $dest_dir = 'dbm_fcs_for_amadiro';
mkpath("$dest_dir");

system(qq{./Tatzer -l x64b --nfc=2 --states-type=COMPACT_STATES --dbm=kaztree});
my @modules = ('alloc.o', 'app_str.o', 'card.o', 'dbm_solver.o', 'state.o', 'dbm_kaztree.o');

foreach my $fn ('alloc.c', 'app_str.c', 'card.c', 'dbm_solver.c', 'state.c',
    'dbm_kaztree.c', 'alloc.h', 'card.h', 'config.h', 'state.h',
    'dbm_solver.h', 'kaz_tree.c', 'kaz_tree.h', 'dbm_solver_key.h',
    'fcs_move.h', 'inline.h', 'bool.h', 'internal_move_struct.h', 'app_str.h',
    'delta_states.c', 'fcs_dllexport.h', 'bit_rw.h', 'fcs_enums.h', 'unused.h',
)
{
    io($fn) > io("$dest_dir/$fn");

}

my $deal_idx = 982;
system(qq{python board_gen/make_pysol_freecell_board.py -t --ms $deal_idx > $dest_dir/$deal_idx.board});

@modules = sort { $a cmp $b } @modules;

io("$dest_dir/Makefile")->print(<<"EOF");
TARGET = dbm_fc_solver
DEAL_IDX = $deal_idx
BOARD = \$(DEAL_IDX).board

CFLAGS = -O3 -march=native -fomit-frame-pointer -DFCS_DBM_WITHOUT_CACHES=1
MODULES = @modules

all: \$(TARGET)

\$(TARGET): \$(MODULES)
\tgcc \$(CFLAGS) -fwhole-program -o \$\@ \$(MODULES) -lm -lpthread

\$(MODULES): %.o: %.c
\tgcc -c \$(CFLAGS) -o \$\@ \$<

run: all
\t./\$(TARGET) --num-threads 64 \$(BOARD) | tee \$(DEAL_IDX).dump
EOF

