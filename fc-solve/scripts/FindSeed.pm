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

package FindSeed::ThresholdAgg;

use MooX qw/late/;

has 'by_threshold' => (is => 'ro', default => sub { return []; },);

sub create
{
    my ($pkg, $args) = @_;

    my $seed = $args->{seed};
    my $scan = $args->{scan};
    my $results = $args->{results};

    my $self = $pkg->new;

    push @{$self->by_threshold},
    map {
        FindSeed::ScanResult->new(
            {
                seed => $seed,
                scan => $scan,
                results => $results,
                iters => $_->iters,
            }
        );
    } @$results;

    return $self;
}

sub from_lines
{
    my ($pkg, $args) = @_;

    my $seed = $args->{seed};

    return $pkg->create(
        {
            %$args,
            results =>
            [
                sort { $a->iters <=> $b->iters }
                map
                {
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
                }
                @{$args->{lines}},
            ],
        }
    );
}

sub add
{
    my ($self, $item) = @_;

    push @{$self->by_threshold}, $item;

    return;
}

sub get
{
    my ($self, $idx) = @_;

    return $self->by_threshold->[$idx];
}

sub merge_from
{
    my ($self, $agg) = @_;

    for my $th (keys (@{$self->by_threshold}))
    {
        my $new = $agg->get($th);
        if ($new->iters < $self->get($th)->iters)
        {
            $self->by_threshold->[$th] = $new;
        }
    }

    return;
}

package FindSeed;

use List::Util qw/max/;
use List::UtilsBy qw/min_by/;
use bytes;
use integer;

use Cwd ();
sub f { return $_[0] =~ /Solved.*?Iters: ([0-9]+)/ ? $1 : 100_000 }

sub find
{
    my ($self, $args) = @_;

    $ENV{PATH} .= ":" . Cwd::getcwd();

    my @deals = @{$args->{deals}};
    my $scan_arg = $args->{scan};

    my @scans = ((ref($scan_arg) eq '') ? ($scan_arg) : @$scan_arg);

    my $MAX_THRESHOLD = $args->{threshold};

    if ($MAX_THRESHOLD > @scans)
    {
        $MAX_THRESHOLD = @scans;
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
    my $iters_agg = FindSeed::ThresholdAgg->from_lines(
        {
            seed => -1,
            scan => '',
            lines => [ map { "$_ = " } @deals],
        }
    );

    my $old_line;

    # 4086 923 Verdict: Solved ; Iters: 170 ; Length: 142
    # my $scan = q#--method random-dfs -to "01[2345789]"#;
    # 2463 694 Verdict: Solved ; Iters: 283 ; Length: 122
    # my $scan = q#--method random-dfs -to "01[234579]"#;
    # my $scan = q#--method random-dfs -to "01[234579]"#;

    my $handle = sub {
        my ($seed) = @_;

        my $max_iters = $iters_agg->get($MAX_THRESHOLD-1)->iters;

        foreach my $scan (@scans)
        {
            my $agg = FindSeed::ThresholdAgg->from_lines(
                {
                    seed => $seed,
                    scan => $scan,
                    lines => do {
                        chomp(my @l = `summary-fc-solve @deals -- $scan -seed "$seed" -sp r:tf -mi "$max_iters"`);
                        \@l;
                    },
                }
            );

            $iters_agg->merge_from($agg);
        }
        print "SUMMARY[$seed] = ", join(" ",
            map { my $th = $_;
                my $result = $iters_agg->get($th);
                sprintf("[%d=%d\@seed=%d]",
                    $th+1,
                    $result->iters,
                    $result->seed,
                )
            } 0 .. $MAX_THRESHOLD-1), "\n";
        for my $threshold (0 .. $MAX_THRESHOLD-1)
        {
            my $result = $iters_agg->get($threshold);
            printf(" ==> %d = %s ; (%s)\n",
                $threshold+1,
                join(" ", map {
                        my $th = $_;
                        sprintf("{%d %d}",
                            $th+1,
                            $result->results->[$th]->iters,
                        );
                    }
                    (0 .. $threshold)
                ),
                $result->scan,
            );
        }
        return;
    };

    $handle->($check_seed);
    for my $seed ($first_seed .. $LAST_SEED)
    {
        $handle->($seed);
    }
}

1;

