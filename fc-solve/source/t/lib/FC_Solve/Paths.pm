package FC_Solve::Paths;

use strict;
use warnings;

use parent 'Exporter';

our @EXPORT_OK = qw(data_file samp_board samp_sol);

use File::Spec ();

my $DATA_DIR = File::Spec->catdir($ENV{FCS_SRC_PATH}, qw(t data));
my $BOARDS_DIR = File::Spec->catdir($DATA_DIR, 'sample-boards');
my $SOLS_DIR = File::Spec->catdir($DATA_DIR, 'sample-solutions');

sub data_file
{
    return File::Spec->catfile($DATA_DIR, @{shift@_});
}

# Returns a board from the sample-boards directory.
sub samp_board
{
    return File::Spec->catfile($BOARDS_DIR, shift);
}

# Returns a board from the sample-boards directory.
sub samp_sol
{
    return File::Spec->catfile($SOLS_DIR, shift);
}

1;
