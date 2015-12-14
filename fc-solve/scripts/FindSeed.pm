package FindSeed;

use strict;
use warnings;
use autodie;

package FindSeed::DealAndSeedResult;

use MooX qw/late/;

has 'deal' => (is => 'ro');
has 'output' => (is => 'ro', lazy => 1);
has 'iters' => (is => 'ro', lazy => 1, default => sub { my $self = shift; return FindSeed::f($self->output); }, );
has 'seed' => (is => 'ro');

sub as_str
{
    my $self = shift;

    return sprintf("[%s %s %s]", $self->deal, $self->output, $self->seed);
}

package FindSeed::ScanResult;

use MooX qw/late/;

has 'scan' => (is => 'ro');
has 'seed' => (is => 'ro');
has 'results' => (is => 'ro');
has 'iters' => (is => 'ro');

package FindSeed;

use List::Util qw/max/;
use List::UtilsBy qw/min_by/;
use bytes;
use integer;

use Cwd ();
sub f { return $_[0] =~ /Solved.*?Iters: ([0-9]+)/ ? $1 : 1e6 }

sub find
{
    my ($self, $args) = @_;

    $ENV{PATH} .= ":" . Cwd::getcwd();

    my @deals = @{$args->{deals}};
    my $scan_arg = $args->{scan};

    my @scans = ((ref($scan_arg) eq '') ? ($scan_arg) : @$scan_arg);

    my $threshold = $args->{threshold};

    if ($threshold > @scans)
    {
        $threshold = @scans;
    }

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
    my $old_line;

    # 4086 923 Verdict: Solved ; Iters: 170 ; Length: 142
    # my $scan = q#--method random-dfs -to "01[2345789]"#;
    # 2463 694 Verdict: Solved ; Iters: 283 ; Length: 122
    # my $scan = q#--method random-dfs -to "01[234579]"#;
    # my $scan = q#--method random-dfs -to "01[234579]"#;

    my $handle = sub {
        my ($seed) = @_;

        my @new_ = (map {
                my $scan = $_;
                my @l = sort { $a->iters <=> $b->iters } map {
                    my $s = $_;
                    $s =~ s#\A([0-9]+) = ##;
                    my $deal = $1;
                    FindSeed::DealAndSeedResult->new(
                        {
                            deal => $deal,
                            output => $s,
                            seed => $seed,
                        }
                        );
                } do {
                    my @l = `summary-fc-solve @deals -- $scan -seed "$seed" -sp r:tf -mi "$iters"`;
                    chomp(@l);
                    @l;
                };
                my $v = $l[$threshold-1]->iters;
                FindSeed::ScanResult->new(
                    {
                        seed => $seed,
                        scan => $scan,
                        results => (\@l),
                        iters => $v,
                    }
                );
            } @scans
        );
        my $new_scan = min_by { $_->iters } @new_;
        my $new = $new_scan->iters;

        if ($new < $iters)
        {
            $old_line = $new_scan;
            $iters = $new;
        }
        print "$seed @{[$old_line->seed]} @{[map { $_->as_str } @{$old_line->results}[0 .. $threshold - 1]]} ; @{[$old_line->scan]}\n";

        return;
    };

    $handle->($check_seed);
    for my $seed ($first_seed .. $LAST_SEED)
    {
        $handle->($seed);
    }
}

1;

