package SplitFcc;

use strict;
use warnings;

use autodie;

use MooX qw/late/;
use Getopt::Long qw/GetOptions/;
use FC_Solve::Base64;
use File::Path qw/mkpath/;
use List::Util qw(max);

use IO::All;

sub _calc_fingerprint_depth
{
    my ($self, $fingerprint_encoded) = @_;

    my $fingerprint = FC_Solve::Base64::base64_decode($fingerprint_encoded);

    my $depth = 0;
    for my $i (0 .. (4*13-1))
    {
        $depth += vec($fingerprint, $i, 2);
    }

    return $depth;
}

sub _calc_depth_dir
{
    my ($self, $depth) = @_;

    return "by-depth/$depth";
}

sub _calc_fingerprint_dir
{
    my ($self, $depth, $fingerprint_encoded) = @_;

    return $self->_calc_depth_dir($depth) . "/active/$fingerprint_encoded";
}

sub _calc_existing_input
{
    my ($self, $target_dir) = @_;

    return max( map {
        ($_->filename() =~ /\Ainput.txtish\.(\d+)\z/) ? $1 : (-1)
        }
        (io->dir($target_dir)->all_files())
    );
}

sub _map_existing_input
{
    my ($self, $target_dir, $existing) = @_;

    return "$target_dir/input.txtish.$existing";
}
sub driver
{
    my ($self, $args) = @_;

    my $board_fn = $args->{board_fn};
    my $fingerprint_encoded = $args->{fingerprint_encoded};

    my $depth = $self->_calc_fingerprint_depth($fingerprint_encoded);

    my $depth_dir = $self->_calc_depth_dir($depth);
    my $fingerprint_dir = $self->_calc_fingerprint_dir($depth, $fingerprint_encoded);

    my $finished_stamp_fn = "$fingerprint_dir/FINISHED.stamp";

    if (-e $finished_stamp_fn)
    {
        print "Already processed and finished.\n";
        return;
    }

    my $input_existing = $self->_calc_existing_input($fingerprint_dir);
    if (!defined($input_existing) or ($input_existing < 0))
    {
        print "Cannot find input.txtish. Perhaps no such one was created.\n";
        return;
    }

    my $input_fn = $self->_map_existing_input($fingerprint_dir, $input_existing);
    my $output_dir = "$fingerprint_dir/out";
    my $queue_dir = "$fingerprint_dir/queue";
    my $debug_output_fn = "$fingerprint_dir/debug.out.txt";
    my $status_output_fn = "$fingerprint_dir/status_output.stamp";
    mkpath( [$output_dir, $queue_dir] );

    my $script_fn = "$fingerprint_dir/driver.bash";
    io->file($script_fn)->print(<<"EOF");
#!/bin/bash
s="$status_output_fn"
rm -f "\$s"
(split_fcc_fc_solver \\
    --offload-dir-path "$queue_dir" \\
    --output "$output_dir" \\
    --board "$board_fn" \\
    --fingerprint "$fingerprint_encoded" \\
    --input "$input_fn" && touch "\$s") \\
    | tee "$debug_output_fn"
if test -e "\$s" ; then
    rm -f "\$s"
    exit 0;
else
    exit 1;
fi
EOF

    print "Run (in tmux):\nbash $script_fn\n";

    if (system("bash", $script_fn))
    {
        die "Did not finish correctly.";
    }

    my $by_fingerprint_dir = "$output_dir/by-fingerprint";
    mkpath([$by_fingerprint_dir]);
    my $idx = 1;
    my $exits_fn = sub {
        return "$output_dir/exits.$idx";
    };
    while (-f $exits_fn->())
    {
        open my $fh, '<', $exits_fn->();
        while (my $line = <$fh>)
        {
            chomp ($line);
            if (my ($exit_fingerprint_encoded) = $line =~ m{\A(\S+)})
            {
                io("$by_fingerprint_dir/$exit_fingerprint_encoded")->append("$line\n");
            }
            else
            {
                die "Does not match.";
            }
        }
        close ($fh);
    }
    continue
    {
        $idx++;
    }

    my $proc_line = sub
    {
        my ($l) = @_;

        if (my ($state, $depth, $moves) = $l =~ /\A\S+ (\S+) (\d+) (\S+)\z/)
        {
            return ($state => [$depth, $moves]);
        }
        else
        {
            Carp::confess ("<<$l>> does not match.");
        }
    };

    while (defined(my $by_fingerprint_fh = (io->dir($by_fingerprint_dir)->all_files)[0]))
    {
        my $fingerprint_encoded = $by_fingerprint_fh->filename();
        my $depth = $self->_calc_fingerprint_depth(
            $fingerprint_encoded
        );

        my %l = (map
            {
            $proc_line->($_)
            }
            $by_fingerprint_fh->chomp->getlines()
        );

        my $target_dir = $self->_calc_fingerprint_dir($depth, $fingerprint_encoded);
        io->dir($target_dir)->mkpath();
        my $existing = $self->_calc_existing_input($target_dir);
        if (!defined($existing) or ($existing < 0))
        {
            $existing = 0;
        }

        my $new_digit = $existing+1;

        my $exist_fn = $self->_map_existing_input($target_dir, $existing);
        if (-e $exist_fn)
        {
            my $fh = io->file($exist_fn);
            while (my $line = $fh->chomp->getline())
            {
                my ($state, $depth, $moves) = split(/\s+/, $line, -1);
                $moves //= '';

                if ((!exists($l{$state})) or ($depth < $l{$state}->[0]))
                {
                    $l{$state} = [$depth, $moves];
                }
            }
        }
        my $new_fn = "$target_dir/input.txtish.$new_digit";
        my $temp_fn = "$new_fn.temp";
        io->file($temp_fn)->print(
            map
            {
                join(' ', $_, @{$l{$_}}) . "\n";
            }
            sort
            {
                ($l{$a}->[0] <=> $l{$b}->[0])
                    or
                ($a cmp $b)
            }
            keys(%l)
        );

        rename($temp_fn, $new_fn);
        $by_fingerprint_fh->unlink();
    }

    io->file($finished_stamp_fn)->print("\n");

    return;
}

sub driver_run
{
    my ($self) = @_;

    my $fingerprint_encoded = '';
    GetOptions(
        "fingerprint=s" => \$fingerprint_encoded,
    ) or die "Error in command line args.";

    my $board_fn = $ENV{BOARD_FN};

    return $self->driver(
        {
            board_fn => $board_fn,
            fingerprint_encoded => $fingerprint_encoded,
        }
    );
}

1;
