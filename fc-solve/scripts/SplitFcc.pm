package SplitFcc;

use strict;
use warnings;

use autodie;

use MooX qw/late/;
use Getopt::Long qw/GetOptions/;
use FC_Solve::Base64;
use File::Path qw/mkpath/;

use IO::All;

sub driver
{
    my ($self, $args) = @_;

    my $board_fn = $args->{board_fn};
    my $fingerprint_encoded = $args->{fingerprint_encoded};

    # my $offload_dir_path = tempdir( CLEANUP => 1 );
    my $fingerprint = FC_Solve::Base64::base64_decode($fingerprint);

    my $depth = 0;
    for my $i (0 .. (4*13-1))
    {
        $depth += vec($fingerprint, $i, 2);
    }

    my $depth_dir = "by-depth/$depth";
    my $fingerprint_dir = "$depth_dir/active/$fingerprint_encoded";

    my $input_fn = "$fingerprint_dir/input.txtish";
    my $output_dir = "$fingerprint_dir/out");
    my $queue_dir = "$fingerprint_dir/queue");
    my $debug_output_fn = "$fingerprint_dir/debug.out.txt";
    my $status_output_fn = "$fingerprint_dir/status_output.stamp";
    mkpath( [$output_dir, $queue_dir] );

    my $script_fn = "$fingerprint_dir/driver.bash";
    io->file($script_fn)->print(<<"EOF");
#!/bin/bash
s="$status_output_fn"
rm -f "\$s"
(split_fcc_fc_solver \
    --offload-dir-path "$queue_dir" \
    --output "$output_dir" \
    --board "$board_fn" \
    --fingerprint "$fingerprint_encoded" \
    --input "$input_fn" && touch "\$s") \
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
