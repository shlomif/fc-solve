#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use File::Path;

use Getopt::Long;

my $flto = 0;

my $who = 'sub';

my $mem = 200;
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
}
my $dest_dir = $sub ? 'dbm_fcs_for_sub' : 'dbm_fcs_for_amadiro';
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

for my $fn ("prepare_pbs_deal.bash")
{
    io("./scripts/$fn") > io("$dest_dir/$fn")
}

=begin old

my @deals = (
    33668,
    34152,
    41701,
    48819
);

=end old

=cut

my @deals = (
    398881,
    397524,
    397251,
    397067,
    396792,
    393377,
    392877,
    392597,
    392121,
    389579,
    384543,
    384243,
    378764,
    378381,
    375783,
    373120,
    368535,
    367103,
    366584,
    361934,
    361933,
    360875,
    359432,
    359366,
    356941,
    355994,
    355678,
    355421,
    355311,
    354170,
    353925,
    350550,
    349903,
    348096,
    347972,
    345325,
    338815,
    337653,
    336145,
    334877,
    333278,
    332182,
    332153,
    331840,
    331514,
    329150,
    327885,
    326051,
    323759,
    323305,
    320583,
    317289,
    315548,
    313501,
    312736,
    312539,
    312381,
    311564,
    310397,
    309308,
    308137,
    301812,
    299249,
    298904,
    298457,
    296584,
    295997,
    293529,
    292420,
    292034,
    291601,
    291131,
    291026,
    289314,
    288510,
    287876,
    287574,
    286095,
    284908,
    284872,
    284204,
    284184,
    282064,
    281621,
    277178,
    277102,
    276418,
    275258,
    275048,
    274934,
    274064,
    273132,
    271773,
    271703,
    271131,
    270188,
    270089,
    269898,
    267466,
    267112,
    265979,
    263634,
    262653,
    262240,
    261983,
    261447,
    256371,
    253759,
    253359,
    253175,
    249846,
    247736,
    247265,
    244525,
    243946,
    243929,
    243540,
    241084,
    239741,
    239422,
    239400,
    238928,
    235676,
    235561,
    234834,
    233848,
    231846,
    229525,
    226947,
    225805,
    225648,
    224115,
    222860,
    220573,
    218816,
    216433,
    216103,
    215765,
    215436,
    215164,
    212009,
    212005,
    210503,
    206603,
    205967,
    205928,
    202885,
    200684,
    198960,
    194004,
    193378,
    192800,
    190534,
    189755,
    187507,
    184908,
    184432,
    182980,
    182001,
    181392,
    179927,
    176992,
    176168,
    175864,
    174026,
    174011,
    173599,
    171706,
    171683,
    171553,
    171337,
    171153,
    169944,
    168605,
    165986,
    164279,
    163324,
    161762,
    161511,
    160245,
    159621,
    156804,
    156228,
    155990,
    155523,
    154008,
    153726,
    149592,
    149167,
    149144,
    147148,
    146457,
    144658,
    143655,
    142864,
    142720,
    142416,
    141951,
    139360,
    135373,
    134813,
    129561,
    124637,
    119086,
    113410,
    104384,
    102257,
);

# my $deal_idx = 982;
foreach my $deal_idx (@deals)
{
    system(qq{python board_gen/make_pysol_freecell_board.py -t --ms $deal_idx > $dest_dir/$deal_idx.board});
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

THREADS = 16
MEM = $mem

CFLAGS = -O3 -march=native -fomit-frame-pointer $more_cflags -DFCS_DBM_WITHOUT_CACHES=1 -DFCS_DBM_USE_LIBAVL=1 -DFCS_LIBAVL_STORE_WHOLE_KEYS=1 -DFCS_DBM_RECORD_POINTER_REPR=1 -I. -I./libavl
MODULES = @modules

JOBS = \$(patsubst %,jobs/%.job.sh,\$(DEALS))
JOBS_STAMP = jobs/STAMP

all: \$(TARGET) \$(JOBS)

\$(TARGET): \$(MODULES)
\tgcc -static \$(CFLAGS) -fwhole-program -o \$\@ \$(MODULES) -lm -lpthread
\tstrip \$\@

\$(MODULES): %.o: %.c
\tgcc -c \$(CFLAGS) -o \$\@ \$<

\$(JOBS_STAMP):
\tmkdir -p jobs
\ttouch \$\@

\$(JOBS): %: \$(JOBS_STAMP) \$(RESULT)
\tTHREADS="\$(THREADS)" MEM="\$(MEM)" bash prepare_pbs_deal.bash "\$(patsubst jobs/%.job.sh,%,\$\@)" "\$\@"

%.show:
\t\@echo "\$* = \$(\$*)"
EOF

system("tar", "-cavf", "$dest_dir.tar.gz", "$dest_dir/");
