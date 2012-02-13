#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use File::Path;

use Getopt::Long;

my $to_upload = 0;
GetOptions(
    '--upload!' => \$to_upload,
);

my $dest_dir = 'fcc_solver_binary_for_amadiro';
mkpath("$dest_dir");

my $exe = "fcc_fc_solver";
system(qq{./Tatzer -l x64b --nfc=2 --states-type=COMPACT_STATES --dbm=kaztree});
system(qq{make -f Makefile.gnu NATIVE_ARCH=0 clean});
system(qq{make -f Makefile.gnu NATIVE_ARCH=0 $exe});
system('strip', $exe);

io->file($exe) > io("$dest_dir/$exe");
chmod(0755, "$dest_dir/$exe");

my $cache_size = 64_000_000;
my @deals = (
    982,
);

# my $deal_idx = 982;
foreach my $deal_idx (@deals)
{
    system(qq{python board_gen/make_pysol_freecell_board.py -t --ms $deal_idx > $dest_dir/$deal_idx.board});
}

io("$dest_dir/Makefile")->print(<<"EOF");
TARGET = fcc_fc_solver
DEALS = @deals

DEALS_DUMPS = \$(patsubst %,%.dump,\$(DEALS))
THREADS = 12
CACHE_SIZE = $cache_size

all: 

run: \$(DEALS_DUMPS)

\$(DEALS_DUMPS): %.dump: all
\t./\$(TARGET) --caches-delta \$(CACHE_SIZE) \$(patsubst %.dump,%.board,\$\@) > \$\@

%.show:
\t\@echo "\$* = \$(\$*)"
EOF

if ($to_upload)
{
    my $arc_name = "$dest_dir.tar.bz2";
    if (system('tar', '-cjvf', $arc_name, $dest_dir))
    {
        die "tar failed!";
    }
    system("rsync", "-a", "-v", "--progress", "--inplace", $arc_name, 
        "hostgator:public_html/Files/files/code/"
    );
}
