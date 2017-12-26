package FindSeed;

use strict;
use warnings;
use autodie;

use Storable qw/nfreeze thaw/;
use MIME::Base64 qw/encode_base64 decode_base64/;

sub enc
{
    my ($item) = @_;
    return ( encode_base64(nfreeze($item)) =~ s/\n+//gr);
}

sub dec
{
    my ($text) = @_;
    return thaw(decode_base64($text));
}

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

sub output_to_fh
{
    my ($iters_agg, $MAX_TH, $seed, $fh) = @_;

    $fh->print( "SUMMARY[$seed] = ", join(" ",
            map { my $th = $_;
                my $result = $iters_agg->get($th);
                sprintf("[%d=%d\@seed=%d]",
                $th+1,
                $result->iters,
                $result->seed,
                )
            } 0 .. $MAX_TH), "\n");

    for my $threshold (0 .. $MAX_TH)
    {
        my $result = $iters_agg->get($threshold);
        $fh->printf(" ==> %d = %s ; (%s)\n",
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

    $fh->flush();

    return;
}

sub run_scans
{
    my ($iters_agg, $args) = @_;

    my $MAX_TH = $args->{MAX_TH};
    my $seed = $args->{seed};
    my $scans = $args->{scans};
    my $deals = $args->{deals};

    my $max_iters = $iters_agg->get($MAX_TH)->iters;

    foreach my $scan (@$scans)
    {
        $iters_agg->merge_from(
            scalar FindSeed::ThresholdAgg->from_lines(
                {
                    seed => $seed,
                    scan => $scan,
                    lines => do {
                        chomp(my @l = `summary-fc-solve @$deals -- $scan -seed "$seed" -sp r:tf -mi "$max_iters"`);
                        \@l;
                    },
                }
            )
        );
    }

    return;
}

sub par_handle_seed
{
    my ($iters_agg, $seed) = @_;

    $iters_agg->run_scans(
        {
            seed => $seed,
            scans => FindSeed::Globals->obj->scans,
            deals => FindSeed::Globals->obj->deals,
            MAX_TH => FindSeed::Globals->obj->max_th,
        }
    );

    return;
}

{ # This is the instance held by the parent process for communicating with the child
	package FindSeed::Parallel::Child;
	use parent qw(Process::Async::Child);
	use POSIX qw(strftime);

	sub finished { $_[0]->{finished} ||= $_[0]->loop->new_future }

	sub cmd_done {
        my $self = shift;
        my $args = FindSeed::dec(shift);

        if (ref ( $args ) eq 'HASH')
        {
            my $start = $args->{'s'};
            my $end = $args->{'e'};
            FindSeed::Globals->obj->agg->merge_from($args->{agg});

            if (FindSeed::Globals->obj->remove_from_pq($end))
            {
                FindSeed::Globals->obj->agg->output_to_fh(
                    FindSeed::Globals->obj->max_th,
                    $end,
                    \*STDOUT,
                );
            }
        }

        my ($s, $e) = FindSeed::Globals->obj->get_next_range;

        FindSeed::Globals->obj->add_to_pq($e);
        $self->send_command(calc => FindSeed::enc({s => $s, e => $e, agg => FindSeed::Globals->obj->agg}));
	}
}

{ # This is the code which runs in the fork
	package FindSeed::Parallel::Worker;
	use parent qw(Process::Async::Worker);

	sub completion { $_[0]->{completion} ||= $_[0]->loop->new_future }

	sub cmd_calc {
        my $self = shift;
        my $args = FindSeed::dec(shift);

        my $agg = $args->{agg};
        my $start = $args->{'s'};
        my $end = $args->{'e'};

        for my $seed ($start .. $end)
        {
            $agg->par_handle_seed($seed);
        }

        $self->send_command(done => FindSeed::enc($args));

        return;
	}

	sub run {
		my ($self, $loop) = @_;
		$self->debug_printf("Worker started");
		$self->send_command(done => FindSeed::enc([]))->then(sub {
			$self->completion
		})->get;
		$self->debug_printf("Worker finished");

		# our exitcode
		return 0;
	}
}

package FindSeed;

use List::Util qw/max min/;
use List::UtilsBy qw/min_by/;
use bytes;
use integer;

use Cwd ();
sub f { return $_[0] =~ /Solved.*?Iters: ([0-9]+)/ ? $1 : 100_000 }

sub _calc_MAX_TH
{
    my ($self, $args) = @_;

    return min($args->{threshold}, scalar(@{$args->{deals}})) - 1;
}

sub find
{
    my ($self, $args) = @_;

    $ENV{PATH} .= ":" . Cwd::getcwd();

    my @deals = @{$args->{deals}};
    my $scan_arg = $args->{scan};

    my @scans = ((ref($scan_arg) eq '') ? ($scan_arg) : @$scan_arg);

    my $MAX_TH = $self->_calc_MAX_TH($args);
    my $first_seed = $ENV{FIRST_SEED} || 1;
    my $check_seed__str = $ENV{CHECK_SEED} || $first_seed;

    my @check_seeds = split/\s+/, $check_seed__str;

    {
        my $prev = shift(@ARGV);
        if (defined($prev))
        {
            my $seeds_str;

            if (($first_seed, $seeds_str) = ($prev =~ /\ASUMMARY\[([0-9]+)\] = ((?:\[[0-9]+=[0-9]+\@seed=[0-9]+\]\s*)+)\z/))
            {
                @check_seeds = map { s#\Aseed=##r } ($seeds_str =~ /(seed=[0-9]+)/g);
            }
            else
            {
                die "Argument is malformatted.";
            }
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

        $iters_agg->run_scans(
            {
                seed => $seed,
                scans => \@scans,
                deals => \@deals,
                MAX_TH => $MAX_TH,
            }
        );

        $iters_agg->output_to_fh($MAX_TH, $seed, \*STDOUT);
        return;
    };

    for my $seed (@check_seeds)
    {
        $handle->($seed);
    }
    for my $seed ($first_seed .. $LAST_SEED)
    {
        $handle->($seed);
    }
}

sub parallel_find
{
    my ($self, $args) = @_;

    $ENV{PATH} .= ":" . Cwd::getcwd();

    my @deals = @{$args->{deals}};
    my $scan_arg = $args->{scan};

    my @scans = ((ref($scan_arg) eq '') ? ($scan_arg) : @$scan_arg);

    my $MAX_TH = $self->_calc_MAX_TH($args);

    my $first_seed = $ENV{FIRST_SEED} || 1;
    my $check_seed__str = $ENV{CHECK_SEED} || $first_seed;

    my @check_seeds = split/\s+/, $check_seed__str;

    {
        my $prev = shift(@ARGV);
        if (defined($prev))
        {
            my $seeds_str;

            if (($first_seed, $seeds_str) = ($prev =~ /\ASUMMARY\[([0-9]+)\] = ((?:\[[0-9]+=[0-9]+\@seed=[0-9]+\]\s*)+)\z/))
            {
                @check_seeds = map { s#\Aseed=##r } ($seeds_str =~ /(seed=[0-9]+)/g);
            }
            else
            {
                die "Argument is malformatted.";
            }
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

    FindSeed::Globals->obj->scans(\@scans);
    FindSeed::Globals->obj->deals(\@deals);
    FindSeed::Globals->obj->max_th($MAX_TH);
    FindSeed::Globals->obj->agg($iters_agg);
    FindSeed::Globals->obj->_next_seed($first_seed);

    for my $seed (@check_seeds)
    {
        $iters_agg->par_handle_seed($seed);
        STDERR->print("Handled seed '$seed'.\n");
    }

    use Process::Async;
    use IO::Async::Loop;

    my $NUM_JOBS = $ENV{NUM_JOBS} || 4;

    my $loop = IO::Async::Loop->new;

    my @children;
    for my $job (1 .. $NUM_JOBS)
    {
        $loop->add(
            my $pm = Process::Async::Manager->new(
                worker => 'FindSeed::Parallel::Worker',
                child => 'FindSeed::Parallel::Child',
            )
        );
        push @children, $pm->spawn;
    }
    $children[0]->finished->get;

    return;
}

1;

package FindSeed::Globals;

use MooX qw/late/;

has 'agg' => (is => 'rw');
has 'scans' => (is => 'rw');
has 'deals' => (is => 'rw');
has 'max_th' => (is => 'rw');
has '_next_seed' => (is => 'rw');
has 'range_size' => (is => 'rw', default => sub { return 40; });
has 'pq' => (is => 'rw', default => sub { return []; });

# A singleton.
my $obj = FindSeed::Globals->new;

sub obj
{
    return $obj;
}

sub get_next_range
{
    my ($self) = @_;

    my $start = $self->_next_seed;

    my $end = $start + $self->range_size;

    $self->_next_seed($end);

    return ($start, $end-1);
}

sub remove_from_pq
{
    my ($self, $pivot) = @_;

    my $ret = ($self->pq->[0] == $pivot);

    $self->pq(
        [ grep {$_ != $pivot } @{$self->pq}]
    );

    return $ret;
}

sub add_to_pq
{
    my ($self, $pivot) = @_;

    $self->pq(
        [ sort {$a <=> $b} @{$self->pq}, $pivot]
    );

    return;
}

1;
