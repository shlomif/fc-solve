package SplitFcc;

use strict;
use warnings;

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
    mkpath( [$output_dir, $queue_dir] );

    my $script_fn = "$fingerprint_dir/driver.bash";
    io->file($script_fn)->print(<<"EOF");
#!/bin/bash
split_fcc_fc_solver \
    --offload-dir-path "$queue_dir" \
    --output "$output_dir" \
    --board "$board_fn" \
    --fingerprint "$fingerprint_encoded" \
    --input "$input_fn" \
    | tee "$debug_output_fn"
EOF

    print "Run (in tmux):\nbash $script_fn\n";

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
