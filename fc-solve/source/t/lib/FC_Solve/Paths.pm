package FC_Solve::Paths;

use strict;
use warnings;

use parent 'Exporter';

our @EXPORT_OK = qw(data_file samp_board);

use File::Spec ();

my $DATA_DIR = File::Spec->catdir($ENV{FCS_SRC_PATH}, qw(t t data));
my $BOARDS_DIR = File::Spec->catdir($DATA_DIR, 'sample-boards');

sub data_file
{
    return File::Spec->catfile($DATA_DIR, @{shift@_});
}

# Returns a board from the sample-boards directory.
sub samp_board
{
    return File::Spec->catfile($BOARDS_DIR, shift);
}

1;
