package FC_Solve::DeltaStater::Constants;

use strict;
use warnings;
use 5.014;

# Author Shlomi Fish <shlomif@cpan.org>

use parent 'Exporter';

our %EXPORT_TAGS = (
    'all' => [
        qw(
            $OPT_DONT_CARE
            $OPT_FREECELL
            $OPT_TOPMOST
            $RANK_KING
            )
    ]
);
our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );
our @EXPORT    = qw();

our $RANK_KING     = 13;
our $OPT_TOPMOST   = 0;
our $OPT_DONT_CARE = $OPT_TOPMOST;
our $OPT_FREECELL  = 1;

1;
