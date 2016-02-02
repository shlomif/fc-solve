package FC_Solve::Par;

use strict;
use warnings;
use autodie;

my $FIRST_INDEX = ($ENV{F} || 1);
my $LAST_INDEX = ($ENV{L} || 32000);
my $STEP = 4;
my $next_i = $FIRST_INDEX;
my @game_params;

{ # This is the instance held by the parent process for communicating with the child
	package FC_Solve::Par::Child;
	use parent qw(Process::Async::Child);

	sub finished { $_[0]->{finished} ||= $_[0]->loop->new_future }

	sub cmd_done {
        my $self = shift;

        if ($next_i > $LAST_INDEX)
        {
            $self->send_command(end => 1);
            $self->finished->done(1);
        }
        else
        {
            my $s = $next_i;
            my $e = $next_i += $STEP;
            if ($e > $LAST_INDEX)
            {
                $e = $LAST_INDEX;
            }
            $next_i++;

            $self->send_command(calc => "$s-$e");
        }

        return;
    }
}

{ # This is the code which runs in the fork
	package FC_Solve::Par::Worker;
	use parent qw(Process::Async::Worker);

    use FC_Solve::Solve (qw/solve/);

    # Completion.
	sub _c { $_[0]->{_c} ||= $_[0]->loop->new_future }

    sub cmd_end {
        shift->_c->done(1);
        return;
    }

	sub cmd_calc {
        my ($self, $arg) = @_;
        my ($s, $e) = $arg =~ /\A([0-9]+)-([0-9]+)\z/;
        for my $i ($s .. $e)
        {
            STDERR->print(solve (\@game_params, $i));
        }
        $self->send_command(done => 1);

        return;
	}

	sub run {
		my ($self, $loop) = @_;
		$self->send_command(done => 1)->then(sub {
			$self->_c
		})->get;

		# our exitcode
		return 0;
	}
}

package FC_Solve::Par;

use lib '../Games-Solitaire-Verify/lib';
use FC_Solve::Solve (qw/fc_solve_init/);

use List::MoreUtils qw/firstidx/;

sub main
{
    use Process::Async;
    use IO::Async::Loop;


    my @args = @ARGV;

    my $separator = firstidx { $_ eq '--' } @args;
    if ($separator < 0)
    {
        die "You must specify both [Game Params] and [Extra fc-solve Args]. See --help";
    }

    @game_params = @args[0 .. $separator - 1];
    fc_solve_init( [ @game_params, @args[$separator+1 .. $#args] ]);

    my $NUM_JOBS = $ENV{NUM_JOBS} || 4;

    my $loop = IO::Async::Loop->new;

    my @children;
    for my $job (1 .. $NUM_JOBS)
    {
        $loop->add(
            my $pm = Process::Async::Manager->new(
                worker => 'FC_Solve::Par::Worker',
                child => 'FC_Solve::Par::Child',
            )
        );
        push @children, $pm->spawn;
    }
    foreach my $child (@children)
    {
        $child->finished->get;
    }

    return;
}

1;
