package FindSeed;

use strict;
use warnings;
use autodie;

use List::Util qw/max/;
use List::UtilsBy qw/min_by/;
use bytes;
use integer;

sub f { return $_[0] =~ /Solved.*?Iters: ([0-9]+)/ ? $1 : 1e6 }

sub find
{
    my ($self, $args) = @_;

    my @deals = @{$args->{deals}};
    my $scan_arg = $args->{scan};

    my @scans = ((ref($scan_arg) eq '') ? ($scan_arg) : @$scan_arg);

    # my @deals = (14249, 10692);
    # my @deals = (14249);

    my $first_seed = $ENV{FIRST_SEED} || 1;
    my $check_seed = $ENV{CHECK_SEED} || $first_seed;

    {
        my $prev = shift(@ARGV);
        if (defined($prev))
        {
            ($first_seed, $check_seed) = ($prev =~ /\A([0-9]+)\s+([0-9]+)/);
        }
    }

    my $LAST_SEED = ((1 << 31)-1);
    my $iters = 100000;
    my @old_line;

    # 4086 923 Verdict: Solved ; Iters: 170 ; Length: 142
    # my $scan = q#--method random-dfs -to "01[2345789]"#;
    # 2463 694 Verdict: Solved ; Iters: 283 ; Length: 122
    # my $scan = q#--method random-dfs -to "01[234579]"#;
    # my $scan = q#--method random-dfs -to "01[234579]"#;

    my $handle = sub {
        my ($seed) = @_;

        my @new_ = (map {
                my $scan = $_;
                my @l = map { "$seed " . `pi-make-microsoft-freecell-board -t $_ | ../../source/scripts/summarize-fc-solve -- $scan -seed "$seed" -sp r:tf -mi "$iters"` =~ s/\n+\z//r } @deals;
                my $v = max ( map { f($_) } @l);
                [$scan,\@l, $v ]
            } @scans
        );
        my $new_scan = min_by { $_->[2] } @new_;
        my $new = $new_scan->[2];

        if ($new < $iters)
        {
            @old_line = @$new_scan;
            $iters = $new;
        }
        print "$seed @{$old_line[1]} ; $old_line[0]\n";

        return;
    };

    $handle->($check_seed);
    for my $seed ($first_seed .. $LAST_SEED)
    {
        $handle->($seed);
    }
}

1;

